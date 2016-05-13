#include "codegen.h"
#include "lit.h"
#include "asm.h"
#include "expr.h"
#include "exprtype.h"
#include "util.h"
#include "parse.h"

llvm::LLVMContext &context(llvm::getGlobalContext());
llvm::IRBuilder<> builder(context);
llvm::Module *mod;
llvm::FunctionPassManager func_pass_mgr(mod);

struct stdfunc_t {
	std::string name, mod_name;
	int args, addr, type; // if args is -1, the function has vector args.
	llvm::Function *func;
};

std::map<std::string, stdfunc_t> stdfunc;

extern "C" {
	void put_num(int n) {
		printf("%d", n);
	}
	void put_num_float(float n) {
		printf("%f", n);
	}
	void put_string(char *s) {
		printf("%s", s);
	}
	void put_ln() {
		putchar('\n');
	}
	char *str_concat(char *a, char *b) {
		char *t = (char *)calloc(strlen(a) + strlen(b) + 1, 1);
		strcpy(t, a);
		return strcat(t, b);
	}
}

llvm::AllocaInst *create_entry_alloca(llvm::Function *TheFunction, std::string &VarName, llvm::Type *type =  NULL) {
		llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
				TheFunction->getEntryBlock().begin());
		return TmpB.CreateAlloca(type == NULL ? llvm::Type::getInt32Ty(context) : type, 0, VarName.c_str());
}

int codegen_entry(ast_vector &program) {
	llvm::InitializeNativeTarget();
	mod = new llvm::Module("LIT", context);
	{ // initialize standard functions
		stdfunc["Array"] = stdfunc_t {"Array", "", 1, 12, T_INT | T_ARRAY};
		stdfunc["gets"] = stdfunc_t {"gets", "File", 0, 52, T_STRING};
		stdfunc["strlen"] = {"strlen", "", 1, 64, T_INT};
		stdfunc["puts"] = {"puts", "", -1, -1, T_VOID};
		stdfunc["put_num"] = {"put_num", "", 1, 1, T_VOID};
		stdfunc["put_num_float"] = {"put_num_float", "", 1, 1, T_VOID};
		stdfunc["put_string"] = {"put_string", "", 1, 1, T_VOID};
		stdfunc["strcat"] = {"strcat", "", 1, 1, T_STRING};

		// create put_string function
		std::vector<llvm::Type *> func_args;
			func_args.push_back(builder.getInt8Ty()->getPointerTo());
		llvm::Function *func = llvm::Function::Create(
				llvm::FunctionType::get(/*ret*/builder.getVoidTy(), func_args, false),
				llvm::GlobalValue::ExternalLinkage,
				"put_string", mod);
		func->setCallingConv(llvm::CallingConv::C);
		stdfunc["put_string"].func = func;
		func_args.clear();
		// create put_num function
		func_args.push_back(builder.getInt32Ty());
		func = llvm::Function::Create(
				llvm::FunctionType::get(/*ret*/builder.getVoidTy(), func_args, false),
				llvm::GlobalValue::ExternalLinkage,
				"put_num", mod);
		func->setCallingConv(llvm::CallingConv::C);
		stdfunc["put_num"].func = func;
		func_args.clear();
		// create put_num_float function
		func_args.push_back(builder.getFloatTy());
		func = llvm::Function::Create(
				llvm::FunctionType::get(builder.getVoidTy(), func_args, false),
				llvm::GlobalValue::ExternalLinkage,
				"put_num_float", mod);
		func->setCallingConv(llvm::CallingConv::C);
		stdfunc["put_num_float"].func = func;
		func_args.clear();
		// create put_ln function
		func = llvm::Function::Create(
				llvm::FunctionType::get(/*ret*/builder.getVoidTy(), func_args, false),
				llvm::GlobalValue::ExternalLinkage,
				"put_ln", mod);
		func->setCallingConv(llvm::CallingConv::C);
		stdfunc["put_ln"].func = func;
		// create strcat function
		func_args.push_back(builder.getInt8PtrTy());
		func_args.push_back(builder.getInt8PtrTy());
		func = llvm::Function::Create(
				llvm::FunctionType::get(/*ret*/builder.getInt8PtrTy(), func_args, false),
				llvm::GlobalValue::ExternalLinkage,
				"str_concat", mod);
		func->setCallingConv(llvm::CallingConv::C);
		stdfunc["strcat"].func = func;
		func_args.clear();
	}
	std::string module = "";
	Program list(module);

	Function main;
	std::vector<AST *> main_code;
	for(ast_vector::iterator it = program.begin(); it != program.end(); ++it) {
		if((*it)->get_type() == AST_FUNCTION) {
			Function f = ((FunctionAST *)*it)->codegen(list);
		} else if((*it)->get_type() == AST_LIBRARY) {
			((LibraryAST *)*it)->codegen(list);
		} else {
			main_code.push_back(*it);
		}
	}
	list.append(main);

	// create main function
	llvm::Function *func_main = llvm::Function::Create(
			llvm::FunctionType::get(builder.getInt32Ty(), std::vector<llvm::Type *>(), false),
			llvm::Function::ExternalLinkage, "main", mod);
	func_main->setCallingConv(llvm::CallingConv::C);
	// create entry point of main function
	llvm::BasicBlock *entry = llvm::BasicBlock::Create(context, "entry", func_main);
	builder.SetInsertPoint(entry);

	for(std::vector<AST *>::iterator it = main_code.begin(); it != main_code.end(); ++it) {
		codegen_expression(main, list, *it);
	}

	builder.CreateRet(llvm::ConstantInt::get(builder.getInt32Ty(), 0));

	llvm::verifyFunction(*func_main);
	
	std::string err;
	llvm::ExecutionEngine *exec_engine = llvm::EngineBuilder(mod).setErrorStr(&err).create();
	if(!exec_engine) error("LitSystemError: LLVMError: %s\n", err.c_str());

	// Set up the optimizer pipeline.  Start with registering info about how the
	// target lays out data structures.
	func_pass_mgr.add(new llvm::DataLayout(*exec_engine->getDataLayout()));
	// Provide basic AliasAnalysis support for GVN.
	func_pass_mgr.add(llvm::createBasicAliasAnalysisPass());
	// Promote allocas to registers.
	func_pass_mgr.add(llvm::createPromoteMemoryToRegisterPass());
	// Do simple "peephole" optimizations and bit-twiddling optzns.
	func_pass_mgr.add(llvm::createInstructionCombiningPass());
	// Reassociate expressions.
	func_pass_mgr.add(llvm::createReassociatePass());
	// Eliminate Common SubExpressions.
	func_pass_mgr.add(llvm::createGVNPass());
	// Simplify the control flow graph (deleting unreachable blocks, etc).
	func_pass_mgr.add(llvm::createCFGSimplificationPass());
	func_pass_mgr.doInitialization();

	void *prog_ptr = exec_engine->getPointerToFunction(mod->getFunction("main"));
	int (*f)() = (int (*)())(int*)prog_ptr;
	f(); // run
	
	return 0;
}

bool const_folding(AST *e, int *res) { // TODO: rewrite more beautiful!
	BinaryAST *expr = (BinaryAST *)e;
	int tmp = 0;
	int a, b;

	if(expr->left->get_type() == AST_BINARY) {
		if(!const_folding(expr->left, &tmp))
			return false;
		a = tmp;
	} else if(expr->left->get_type() == AST_NUMBER) 
		a = ((NumberAST *)expr->left)->number;
	else return false;

	if(expr->right->get_type() == AST_BINARY) {
		if(!const_folding(expr->right, &tmp))
			return false;
		b = tmp;
	} else if(expr->right->get_type() == AST_NUMBER) 
		b = ((NumberAST *)expr->right)->number;
	else return false;

	if(expr->op == "+") *res = a + b;
	else if(expr->op == "-") *res = a - b;
	else if(expr->op == "*") *res = a * b;
	else if(expr->op == "/") *res = a / b;
	else if(expr->op == "%") *res = a % b;
	else return false;

	return true;
}

llvm::Value *codegen_expression(Function &f, Program &f_list, AST *ast, int *ty) {
	int buf; if(ty == NULL) ty = &buf;
	switch(ast->get_type()) {
	case AST_NUMBER:
		return ((NumberAST *)ast)->codegen(f, ty);
	case AST_NUMBER_FLOAT:
		return ((FloatNumberAST *)ast)->codegen(f, ty);
	case AST_STRING:
		return ((StringAST *)ast)->codegen(f, ty);
	case AST_CHAR:
		return ((CharAST *)ast)->codegen(f, ty);
	case AST_NEW:
		return ((NewAllocAST *)ast)->codegen(f, f_list, ty);
	case AST_VARIABLE:
		return ((VariableAST *)ast)->codegen(f, f_list, ty);
	case AST_VARIABLE_ASGMT:
		return ((VariableAsgmtAST *)ast)->codegen(f, f_list, ty);
	case AST_FUNCTION_CALL:
		return ((FunctionCallAST *)ast)->codegen(f, f_list, ty);
	case AST_BINARY: 
		return ((BinaryAST *)ast)->codegen(f, f_list, ty);
	case AST_ARRAY:
		return ((ArrayAST *)ast)->codegen(f, f_list, ty);
	case AST_VARIABLE_INDEX:
		return ((VariableIndexAST *)ast)->codegen(f, f_list, ty);
	case AST_IF:
		return ((IfAST *)ast)->codegen(f, f_list);
	case AST_WHILE:
		return ((WhileAST *)ast)->codegen(f, f_list);
	case AST_FOR:
		return ((ForAST *)ast)->codegen(f, f_list);
	case AST_BREAK:
		return ((BreakAST *)ast)->codegen(f, f_list);
	case AST_RETURN:
		return ((ReturnAST *)ast)->codegen(f, f_list);
	}
	return NULL;
}

llvm::Value * LibraryAST::codegen(Program &f_list) {
	void *lib = dlopen(("./lib/" + lib_name + ".so").c_str(), RTLD_LAZY | RTLD_NOW);
	if(lib == NULL) error("LitSystemError: cannot load library '%s'", lib_name.c_str());
	/*
		lib_t l = {
			.name = name,
			.no = lib.size(),
			.handle = dlopen(("./lib/" + name + ".so").c_str(), RTLD_LAZY | RTLD_NOW)
		};
		if(l.handle == NULL)
	 */
	for(ast_vector::iterator it = proto.begin(); it != proto.end(); ++it) {
		((PrototypeAST *)*it)->append(lib, f_list);
	}
	return 0;
}

void PrototypeAST::append(void *lib, Program &f_list) {
	Function f;
	f.info.name = proto.name;
	f.info.mod_name = "";
	f.info.is_lib = true;
	// f.info.address = (uint_t)dlsym(lib, proto.name.c_str());
	f.info.params = args_type.size();
	f.info.type = proto.type;
	f_list.append(f);
}

Function FunctionAST::codegen(Program &f_list) {
	Function f;
	f.info.name = info.name;
	f.info.mod_name = "";
	f.info.address = ntv.count;
	f.info.params = args.size();
	f.info.type = info.type;
	uint32_t func_bgn = ntv.count;


	// append arguments 
	std::vector<llvm::Type *> arg_types;
	std::vector<var_t *> arg_variables;
	for(ast_vector::iterator it = args.begin(); it != args.end(); ++it) {
		if((*it)->get_type() == AST_VARIABLE) {
			var_t *v = ((VariableAST *)*it)->append(f, f_list);
			arg_types.push_back(builder.getInt32Ty());
			arg_variables.push_back(v);
		} else if((*it)->get_type() == AST_VARIABLE_DECL) {
			var_t *v = ((VariableDeclAST *)*it)->append(f, f_list);
			if(v->type == T_STRING) 
				arg_types.push_back(builder.getInt8PtrTy());
			else 
				arg_types.push_back(builder.getInt32Ty());
			arg_variables.push_back(v);
			// if(v->type & T_ARRAY || v->type == T_STRING) {
			// 	ntv.gencode(0x8d); ntv.gencode(0x45);
			// 		ntv.gencode(256 - ADDR_SIZE * v->id); // lea eax [esp - id*ADDR_SIZE]
			// 	ntv.gencode(0x89); ntv.gencode(0x04); ntv.gencode(0x24); // mov [esp], eax
			// 	ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(44);// call *44(esi) LitMemory::append_ptr
			// }
		}
	}

	// definition the Function
	llvm::Type *func_ret_type = info.type == T_STRING ? (llvm::Type *)builder.getInt8PtrTy() : (llvm::Type *)builder.getInt32Ty();
	llvm::FunctionType *func_type = llvm::FunctionType::get(func_ret_type, arg_types, false);
	llvm::Function *func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, f.info.name, mod);
	func->setCallingConv(llvm::CallingConv::C);

	f_list.append(f);
	f_list.rep_undef(f.info.name, func_bgn);

	llvm::BasicBlock *entry = llvm::BasicBlock::Create(context, "entry", func);
	builder.SetInsertPoint(entry);


	// set argment variable names and store
	auto arg_it = func->arg_begin();
	auto arg_types_it = arg_types.begin();
	for(auto it : arg_variables) {
		arg_it->setName((*it).name);
		llvm::AllocaInst *ainst = create_entry_alloca(func, (*it).name, (*arg_types_it));
		builder.CreateStore(arg_it, ainst);
		(*it).val = ainst;
		++arg_it; ++arg_types_it;
	}

	llvm::Value *ret_value = llvm::ConstantInt::get(builder.getInt32Ty(), 0); // default return code 0
	for(ast_vector::iterator it = statement.begin(); it != statement.end(); ++it) { // function body
		ret_value = codegen_expression(f, f_list, *it);
	}
	if(ret_value) {
		if(ret_value->getType()->getTypeID() != func_ret_type->getTypeID())
			ret_value = llvm::ConstantInt::get(builder.getInt32Ty(), 0);
	} else if(func_ret_type->getTypeID() == builder.getInt32Ty()->getTypeID()) {
		ret_value = llvm::ConstantInt::get(builder.getInt32Ty(), 0);
	} else error("error: return code of function is incorrect");


	builder.CreateRet(ret_value);
	llvm::verifyFunction(*func);
	func_pass_mgr.run(*func);

	return f;
}

llvm::Value * IfAST::codegen(Function &f, Program &f_list) {
	llvm::Value *cond_val = codegen_expression(f, f_list, cond);
	cond_val = builder.CreateICmpNE(cond_val, llvm::ConstantInt::get(builder.getInt32Ty(), 0), "if_cond");

	llvm::Function *func = builder.GetInsertBlock()->getParent();

	llvm::BasicBlock *bb_then = llvm::BasicBlock::Create(context, "then", func);
	llvm::BasicBlock *bb_else = llvm::BasicBlock::Create(context, "else");
	llvm::BasicBlock *bb_merge= llvm::BasicBlock::Create(context, "merge");

	builder.CreateCondBr(cond_val, bb_then, bb_else);
	builder.SetInsertPoint(bb_then);

	llvm::Value *val_then = NULL;
	for(auto expr : then_block) 
		val_then = codegen_expression(f, f_list, expr);

	builder.CreateBr(bb_merge);
	bb_then = builder.GetInsertBlock();

	func->getBasicBlockList().push_back(bb_else);
	builder.SetInsertPoint(bb_else);

	llvm::Value *val_else = NULL;
	for(auto expr : else_block)
		val_else = codegen_expression(f, f_list, expr);

	builder.CreateBr(bb_merge);
	bb_else = builder.GetInsertBlock();

	func->getBasicBlockList().push_back(bb_merge);
	builder.SetInsertPoint(bb_merge);
	if(val_then != NULL && val_else != NULL && val_then->getType()->getTypeID() == val_else->getType()->getTypeID()) {
		llvm::PHINode *pnode = builder.CreatePHI(val_then->getType(), 2, "if_tmp");

		pnode->addIncoming(val_then, bb_then);
		pnode->addIncoming(val_else, bb_else);
		return pnode;
	}
	return NULL;
}

llvm::Value * WhileAST::codegen(Function &f, Program &f_list) {
	ntv.gencode(0x90); int loop_bgn = ntv.count;
		std::vector<int> *break_list = new std::vector<int>;
		f.break_list.push(break_list);
	// codegen_expression(f, f_list, cond);
	ntv.gencode(0x83); ntv.gencode(0xf8); ntv.gencode(0x00);// cmp eax, 0
	ntv.gencode(0x75); ntv.gencode(0x05); // jne 5
	ntv.gencode(0xe9); int end = ntv.count; ntv.gencode_int32(0);// jmp while end

	for(ast_vector::iterator it = block.begin(); it != block.end(); ++it) {
		// codegen_expression(f, f_list, *it);
	}

	ntv.gencode(0xe9); ntv.gencode_int32(0xFFFFFFFF - ntv.count + loop_bgn - ADDR_SIZE); // jmp n
	ntv.gencode_int32_insert(ntv.count - end - ADDR_SIZE, end);

	for(std::vector<int>::iterator it = f.break_list.top()->begin(); it != f.break_list.top()->end(); ++it) {
		ntv.gencode_int32_insert(ntv.count - *it - ADDR_SIZE, *it);
	}
	f.break_list.pop();
}

llvm::Value * ForAST::codegen(Function &f, Program &f_list) {
	llvm::Function *func = builder.GetInsertBlock()->getParent();
	codegen_expression(f, f_list, asgmt);

	llvm::BasicBlock *bb_loop = llvm::BasicBlock::Create(context, "loop", func);
	llvm::BasicBlock *bb_after_loop = llvm::BasicBlock::Create(context, "after_loop", func);

	llvm::Value *frst_cond_val = builder.CreateICmpNE(
			codegen_expression(f, f_list, cond), llvm::ConstantInt::get(builder.getInt32Ty(), 0, "loop_cond"));
	builder.CreateCondBr(frst_cond_val, bb_loop, bb_after_loop);

	builder.SetInsertPoint(bb_loop);

		for(auto expr : block) 
			codegen_expression(f, f_list, expr);
		
		codegen_expression(f, f_list, step);

		llvm::Value *cond_val = builder.CreateICmpNE(
				codegen_expression(f, f_list, cond), llvm::ConstantInt::get(builder.getInt32Ty(), 0, "loop_cond"));

	builder.CreateCondBr(cond_val, bb_loop, bb_after_loop);

	builder.SetInsertPoint(bb_after_loop);

	return NULL;
}

llvm::Value * FunctionCallAST::codegen(Function &f, Program &f_list, int *ty) {
	if(stdfunc.count(info.name)) {
		llvm::Value *stdfunc_ret_value = llvm::ConstantInt::get(builder.getInt32Ty(), 0);
		if(info.name == "Array") {
			codegen_expression(f, f_list, args[0]);
			ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(0 * ADDR_SIZE); // mov [esp+arg*ADDR_SIZE], eax
			ntv.genas("mov eax 4");
		ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(1 * ADDR_SIZE); // mov [esp+arg*ADDR_SIZE], eax
			ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(12); // call malloc
		} else if(info.name == "puts") {
			for(int n = 0; n < args.size(); n++) {
				int ty;
				llvm::Value *val = codegen_expression(f, f_list, args[n], &ty);
				std::vector<llvm::Value*> func_args;
					func_args.push_back(val);
				if(ty == T_STRING) {
					builder.CreateCall(stdfunc["put_string"].func, func_args)->setCallingConv(llvm::CallingConv::C);
				} else if(ty == T_CHAR) {
				} else if(ty == T_DOUBLE) {
					builder.CreateCall(stdfunc["put_num_float"].func, func_args)->setCallingConv(llvm::CallingConv::C);
				} else {
					builder.CreateCall(stdfunc["put_num"].func, func_args)->setCallingConv(llvm::CallingConv::C);
				}
			}
			builder.CreateCall(stdfunc["put_ln"].func, std::vector<llvm::Value *>())->setCallingConv(llvm::CallingConv::C); // for new line
		} else {
			if(stdfunc[info.name].args == -1) { // vector
				uint32_t a = 0;
				for(ast_vector::iterator it = args.begin(); it != args.end(); ++it) {
					codegen_expression(f, f_list, *it);
					ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(ADDR_SIZE * a++); // mov [esp+a*ADDR_SIZE], eax
				}
			} else { // normal function
				for(int arg = 0; arg < stdfunc[info.name].args; arg++) {
					codegen_expression(f, f_list, args[arg]);
					ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(arg * ADDR_SIZE); // mov [esp+arg*ADDR_SIZE], eax
				}
			}
			ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(stdfunc[info.name].addr); // call $function
		}
		*ty = stdfunc[info.name].type;
		return stdfunc_ret_value;
	}

	// user function
	Function *function = f_list.get(info.name);
	llvm::Function *callee = mod->getFunction(info.name);
	if(function == NULL) { // undefined
		uint32_t a = 3;
		for(ast_vector::iterator it = args.begin(); it != args.end(); ++it) {
			codegen_expression(f, f_list, *it);
			ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(256 - a++ * ADDR_SIZE); // mov [esp+ADDR*a], eax
		}
		ntv.gencode(0xe8); f_list.append_undef(info.name, info.mod_name, ntv.count);
			ntv.gencode_int32(0x00000000); // call function
		*ty = T_INT;
		return NULL;
	} else { // defined
		if(args.size() != function->info.params) error("error: the number of arguments is not same");
		// if(function->info.is_lib) {
		// 	uint32_t a = 0;
		// 	for(ast_vector::iterator it = args.begin(); it != args.end(); ++it) {
		// 		codegen_expression(f, f_list, *it);
		// 		ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(a++ * ADDR_SIZE); // mov [esp+ADDR*a], eax
		// 	}
		// } else {
		std::vector<llvm::Value *> callee_args;
		for(auto arg = args.begin(); arg != args.end(); ++arg) {
			callee_args.push_back(codegen_expression(f, f_list, *arg));
		}
		*ty = function->info.type;
		return builder.CreateCall(callee, callee_args, "call_tmp");
	}
}

llvm::Value * BinaryAST::codegen(Function &f, Program &f_list, int *ty) {
	int ty_l = T_VOID, ty_r = T_VOID;
	llvm::Value *lhs = codegen_expression(f, f_list, left, &ty_l);
	llvm::Value *rhs = codegen_expression(f, f_list, right, &ty_r);
	*ty = ty_l;

	if(ty_l != ty_r) if(op != "+") error("error: type error"); // except string concat
	if(op == "+") {
		if(ty_l == T_STRING && ty_r == T_STRING) { // string + string
			std::vector<llvm::Value*> func_args;
			func_args.push_back(lhs);
			func_args.push_back(rhs);
			llvm::Value *ret = builder.CreateCall(stdfunc["strcat"].func, func_args);
			return ret;	
		} else {
			return builder.CreateAdd(lhs, rhs, "addtmp");
		}
	} else if(op == "-") return builder.CreateSub(lhs, rhs, "subtmp");
	else if(op == "*") return builder.CreateMul(lhs, rhs, "multmp");
	else if(op == "/") return builder.CreateSDiv(lhs, rhs, "divtmp");
	else if(op == "%") return builder.CreateSRem(lhs, rhs, "remtmp"); 
	else if(op == "<" || op == ">" || op == "!=" ||
			op == "==" || op == "<=" || op == ">=") {
		bool lt = op == "<", gt = op == ">", ne = op == "!=", eql = op == "==", fle = op == "<=";
		bool str_cmp = false;
		std::string tmp_name = "cmp_tmp";
		if(op == "<") {
			lhs = builder.CreateICmpSLT(lhs, rhs, tmp_name);
		} else if(op == ">") {
			lhs = builder.CreateICmpSGT(lhs, rhs, tmp_name);
		} else if(op == "!=") {
			lhs = builder.CreateICmpNE(lhs, rhs, tmp_name);
		} else if(op == "==") {
			lhs = builder.CreateICmpEQ(lhs, rhs, tmp_name);
		} else if(op == "<=") {
			lhs = builder.CreateICmpSLE(lhs, rhs, tmp_name);
		} else if(op == ">=") {
			lhs = builder.CreateICmpSGE(lhs, rhs, tmp_name);
		}
		lhs = builder.CreateZExt(lhs, builder.getInt32Ty());
		return lhs;
		// if(ne || eql) {
		// 	if(ty_l == T_STRING && ty_r == T_STRING) { // string == or != string
		// 		ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(ADDR_SIZE * 0); // mov [esp+0*ADDR_SIZE], eax
		// 		ntv.gencode(0x89); ntv.gencode(0x5c); ntv.gencode(0x24); ntv.gencode(ADDR_SIZE * 1); // mov [esp+1*ADDR_SIZE], ebx
		// 		ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(eql ? 80 : 84); // call streql or strne
		// 		str_cmp = true;
		// 	}
		// } 
		// if(!str_cmp) {
		// 	ntv.gencode(0x39); ntv.gencode(0xd8); // cmp %eax, %ebx
		// 	ntv.gencode(0x0f); ntv.gencode(lt ? 0x9c : gt ? 0x9f : ne ? 0x95 : eql ? 0x94 : fle ? 0x9e : 0x9d); ntv.gencode(0xc0); // setX al
		// 	ntv.gencode(0x0f); ntv.gencode(0xb6); ntv.gencode(0xc0); // movzx eax al
		// }
	} else if(op == "and" || op == "&" || op == "or" ||
			op == "|" || op == "xor" || op == "^") {
		bool andop = op == "and" || op == "&", orop = op == "or" || op == "|";
		if(andop) {
			lhs = builder.CreateAnd(lhs, rhs);
		} else if(orop) {
			lhs = builder.CreateOr(lhs, rhs);
		} else {
			lhs = builder.CreateXor(lhs, rhs);
		}
		lhs = builder.CreateZExt(lhs, builder.getInt32Ty());
		return lhs;
	}
	return NULL;
}

llvm::Value * NewAllocAST::codegen(Function &f, Program &f_list, int *ty) {
	int alloc_type = Type::str_to_type(type);
	// codegen_expression(f, f_list, size);
	ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(0 * ADDR_SIZE); // mov [esp], eax
	ntv.genas("mov eax 4");
	ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(1 * ADDR_SIZE); // mov [esp+ADDR_SIZE], eax
	ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(12); // call malloc
	*ty = alloc_type | T_ARRAY;
	return NULL;
}

llvm::Value * VariableAsgmtAST::codegen(Function &f, Program &f_list, int *ty) {
	var_t *v;
	bool first_decl = false;

	if(var->get_type() == AST_VARIABLE) {
		if((v = ((VariableAST *)var)->get(f, f_list)) == NULL) {
			v = ((VariableAST *)var)->append(f, f_list);
			first_decl = true;
		}
	} else if(var->get_type() == AST_VARIABLE_DECL) {
		if((v = ((VariableDeclAST *)var)->get(f, f_list)) == NULL) {
			v = ((VariableDeclAST *)var)->append(f, f_list);
			first_decl = true;
		}
	} else if(var->get_type() == AST_VARIABLE_INDEX) {
		return NULL;
	}

	int v_ty;
	llvm::Value *val = codegen_expression(f, f_list, src, &v_ty);
	if(v->is_global) {
		ntv.gencode(0xa3); f_list.append_addr_of_global_var(v->name, ntv.count); 
		ntv.gencode_int32(0x00000000); // GLOBAL_INSERT
	} else {
		// if(v->type == T_STRING) { // string is copied
		// 	ntv.gencode(0x89); ntv.gencode(0x04); ntv.gencode(0x24); // mov [esp], eax
		// 	ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(96);// call *96(esi) str_copy
		// }
		*ty = v_ty; // type of this expression
		if(first_decl) {
			llvm::AllocaInst *ai;
			v->type = v_ty;
			if(v_ty == T_STRING) { // string
				ai = builder.CreateAlloca(llvm::Type::getInt8PtrTy(context), 0, v->name);
			} else if(v_ty == T_DOUBLE) {
				ai = builder.CreateAlloca(llvm::Type::getFloatTy(context), 0, v->name);
			} else { // integer
				ai = builder.CreateAlloca(llvm::Type::getInt32Ty(context), 0, v->name);
			}
			v->val = ai;
		}
		builder.CreateStore(val, v->val);
		return NULL;
		// if(v->type & T_ARRAY || v->type == T_STRING) { // append root for GC
		// 	ntv.genas("push eax");
		// 		ntv.gencode(0x8d); ntv.gencode(0x45);
		// 			ntv.gencode(256 - ADDR_SIZE * v->id); // lea eax [esp - id*ADDR_SIZE]
		// 		ntv.gencode(0x89); ntv.gencode(0x04); ntv.gencode(0x24); // mov [esp], eax
		// 		ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(44);// call *44(esi) LitMemory::append_ptr
		// 	ntv.genas("pop eax");
		// }
	}
	return NULL;
}

var_t *VariableDeclAST::get(Function &f, Program &f_list) {
	if(info.is_global) return f_list.var_global.get(info.name, info.mod_name);
	return f.var.get(info.name, info.mod_name);
}

var_t *VariableDeclAST::append(Function &f, Program &f_list) {
	if(info.is_global) return f_list.var_global.append(info.name, info.type, true);
	return f.var.append(info.name, info.type);
}

llvm::Value * VariableIndexAST::codegen(Function &f, Program &f_list, int *ret_ty) {
	// int ty = codegen_expression(f, f_list, var);
	int ty;
	ntv.genas("push eax");
	// codegen_expression(f, f_list, idx);
	ntv.genas("mov ecx eax");
	ntv.genas("pop edx");
	if(IS_TYPE(ty, T_STRING)) {
		ntv.gencode(0x0f); ntv.gencode(0xb6); ntv.gencode(0x04); ntv.gencode(0x0a);// movzx eax, [edx + ecx]
		return NULL;
	} else {
		ntv.gencode(0x8b); ntv.gencode(0x04); ntv.gencode(0x8a);// mov eax, [edx + ecx * 4]
		return NULL;//((T_STRING | T_ARRAY) == ty) ? T_STRING : T_INT;
	}
}

llvm::Value * VariableAST::codegen(Function &f, Program &f_list, int *ty) {
	var_t *v;
	if(info.is_global) {
		v = f_list.var_global.get(info.name, info.mod_name);
		if(v == NULL) v = f_list.append_global_var(info.name, info.type); // global variable can be used if didn't declared
	} else 
		v = f.var.get(info.name, info.mod_name);
	if(v == NULL) error("error: '%s' was not declared", info.name.c_str());
	if(info.is_global == false) { // local
		*ty = v->type;
		return builder.CreateLoad(v->val, "vartmp");
	} else { // global
		ntv.gencode(0xa1); f_list.append_addr_of_global_var(v->name, ntv.count);
			ntv.gencode_int32(0x00000000); // mov eax GLOBAL_ADDR GLOBAL_INSERT
	}
}
var_t *VariableAST::get(Function &f, Program &f_list) {
	if(info.is_global) return f_list.var_global.get(info.name, info.mod_name);
	return f.var.get(info.name, info.mod_name);
}
var_t *VariableAST::append(Function &f, Program &f_list) {
	if(info.is_global) return f_list.append_global_var(info.name, info.type);
	return f.var.append(info.name, info.type);
}

llvm::Value *ReturnAST::codegen(Function &f, Program &f_list) {
	builder.CreateBr(f.bb_return);
	return NULL;
}

llvm::Value * BreakAST::codegen(Function &f, Program &f_list) {
	ntv.gencode(0xe9); // jmp
		f.break_list.top()->push_back(ntv.count);
		ntv.gencode_int32(0x00000000);
}

llvm::Value * ArrayAST::codegen(Function &f, Program &f_list, int *ret_ty) {
	ntv.genas("mov eax %d", elems.size() * ADDR_SIZE);
	ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(0 * ADDR_SIZE); // mov [esp+arg*ADDR_SIZE], eax
	ntv.genas("mov eax 4");
	ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(1 * ADDR_SIZE); // mov [esp+arg*ADDR_SIZE], eax
	ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(12); // call malloc
	ntv.genas("push eax");
	uint32_t a = 0;
	int ty;
	for(ast_vector::iterator it = elems.begin(); it != elems.end(); ++it) {
		// ty = codegen_expression(f, f_list, *it);
		ntv.genas("pop ecx");
		ntv.genas("mov edx %d", a);
		ntv.gencode(0x89); ntv.gencode(0x81); ntv.gencode_int32(a); // mov [ecx+a], eax
		ntv.genas("push ecx");
		a += 4;
	}
	ntv.genas("pop eax");
	// return ty | T_ARRAY;
}

llvm::Value *StringAST::codegen(Function &f, int *type) {
	 // llvm::Value *value = builder.CreateGlobalStringPtr(<STRING>);
	*type = T_STRING;
	char *embed = (char *)LitMemory::alloc_const(str.length() + 1); // TODO: fix!
	replace_escape(strcpy(embed, str.c_str()));
	return builder.CreateGlobalStringPtr(str.c_str());
}

llvm::Value * CharAST::codegen(Function &f, int *ty) {
	ntv.genas("mov eax %d", ch);
}

llvm::Value * NumberAST::codegen(Function &f, int *ty) {
	*ty = T_INT;
	return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), number, true);
}
llvm::Value * FloatNumberAST::codegen(Function &f, int *ty) {
	*ty = T_DOUBLE;
	return llvm::ConstantFP::get(builder.getFloatTy(), number);
}
