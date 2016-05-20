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

struct stdfunc_t {
	std::string name;
	int args, type; // if args is -1, the function has vector args.
	llvm::Function *func;
};

std::map<std::string, stdfunc_t> stdfunc;

extern "C" {
	void put_num(int n) {
		printf("%d", n);
	}
	void put_num_float(float n) {
		printf("%g", n);
	}
	void put_char(char ch) {
		putchar(ch);
	}
	void put_string(char *s) {
		printf("%s", s);
	}
	void put_array(int *ary) {
		int size = LitMemory::get_size(ary);
		if(size == -1) return;
		printf("[ ");
		for(int i = 0; i < size; i++)
			printf("%d ", ary[i]);
		printf("]");
	}
	void put_ln() {
		putchar('\n');
	}
	void *create_array(uint32_t size, uint32_t byte) {
		return LitMemory::alloc(size, byte);
	}
	char *str_concat(char *a, char *b) {
		char *t = (char *)LitMemory::alloc(strlen(a) + strlen(b) + 1, 1);
		strcpy(t, a);
		return strcat(t, b);
	}
	char *str_concat_char(char *a, char b) {
		char *t = (char *)LitMemory::alloc(strlen(a) + 2, 1);
		t[strlen(strcpy(t, a))] = b;
		return t;
	}
	char *get_string_stdin() {
		std::string input_from_stdin;
		std::getline(std::cin, input_from_stdin);
		char *ret = (char *)LitMemory::alloc(input_from_stdin.size() + 1, sizeof(char));
		return strcpy(ret, input_from_stdin.c_str());
	}
	int get_memory_length(void *ptr) {
		return LitMemory::get_size(ptr);
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
		stdfunc["create_array"] = {"create_array", 1, T_ARRAY};
		stdfunc["gets"] = {"gets", 0, T_STRING};
		stdfunc["strlen"] = {"strlen", 1, T_INT};
		stdfunc["printf"] = {"printf", -1, T_VOID};
		stdfunc["puts"] = {"puts", -1, T_VOID};
		stdfunc["put_num"] = {"put_num", 1, T_VOID};
		stdfunc["put_num_float"] = {"put_num_float", 1, T_VOID};
		stdfunc["put_char"] = {"put_char", 1, T_VOID};
		stdfunc["put_array"] = {"put_array", 1, T_VOID};
		stdfunc["put_string"] = {"put_string", 1, T_VOID};
		stdfunc["strcat"] = {"strcat", 2, T_STRING};
		stdfunc["len"] = {"len", 1, T_INT};

		// create put_string function
		std::vector<llvm::Type *> func_args;
			func_args.push_back(builder.getInt8Ty()->getPointerTo());
		llvm::Function *func = llvm::Function::Create(
				llvm::FunctionType::get(/*ret*/builder.getVoidTy(), func_args, false),
				llvm::GlobalValue::ExternalLinkage,
				"put_string", mod);
		stdfunc["put_string"].func = func;
		func_args.clear();
		// create printf function
		func_args.push_back(builder.getInt8Ty());
		func = llvm::Function::Create(
				llvm::FunctionType::get(/*ret*/builder.getVoidTy(), func_args, true),
				llvm::GlobalValue::ExternalLinkage,
				"printf", mod);
		stdfunc["printf"].func = func;
		func_args.clear();
		// create put_string function
		func_args.push_back(builder.getInt8Ty());
		func = llvm::Function::Create(
				llvm::FunctionType::get(/*ret*/builder.getVoidTy(), func_args, false),
				llvm::GlobalValue::ExternalLinkage,
				"put_char", mod);
		stdfunc["put_char"].func = func;
		func_args.clear();
		// create put_num function
		func_args.push_back(builder.getInt32Ty());
		func = llvm::Function::Create(
				llvm::FunctionType::get(/*ret*/builder.getVoidTy(), func_args, false),
				llvm::GlobalValue::ExternalLinkage,
				"put_num", mod);
		stdfunc["put_num"].func = func;
		func_args.clear();
		// create put_array function
		func_args.push_back(builder.getVoidTy()->getPointerTo());
		func = llvm::Function::Create(
				llvm::FunctionType::get(/*ret*/builder.getVoidTy(), func_args, false),
				llvm::GlobalValue::ExternalLinkage,
				"put_array", mod);
		stdfunc["put_array"].func = func;
		func_args.clear();
		// create put_num_float function
		func_args.push_back(builder.getFloatTy());
		func = llvm::Function::Create(
				llvm::FunctionType::get(builder.getVoidTy(), func_args, false),
				llvm::GlobalValue::ExternalLinkage,
				"put_num_float", mod);
		stdfunc["put_num_float"].func = func;
		func_args.clear();
		// create put_ln function
		func = llvm::Function::Create(
				llvm::FunctionType::get(/*ret*/builder.getVoidTy(), func_args, false),
				llvm::GlobalValue::ExternalLinkage,
				"put_ln", mod);
		stdfunc["put_ln"].func = func;
		// create strcat function
		func_args.push_back(builder.getInt8PtrTy());
		func_args.push_back(builder.getInt8PtrTy());
		func = llvm::Function::Create(
				llvm::FunctionType::get(/*ret*/builder.getInt8PtrTy(), func_args, false),
				llvm::GlobalValue::ExternalLinkage,
				"str_concat", mod);
		stdfunc["strcat"].func = func;
		func_args.clear();
		// create create_array function
		func_args.push_back(builder.getInt32Ty());
		func_args.push_back(builder.getInt32Ty());
		func = llvm::Function::Create(
				llvm::FunctionType::get(/*ret*/builder.getVoidTy()->getPointerTo(), func_args, false),
				llvm::GlobalValue::ExternalLinkage,
				"create_array", mod);
		stdfunc["create_array"].func = func;
		func_args.clear();
		// create gets function
		func = llvm::Function::Create(
				llvm::FunctionType::get(/*ret*/builder.getInt8PtrTy(), func_args, false),
				llvm::GlobalValue::ExternalLinkage,
				"get_string_stdin", mod);
		stdfunc["gets"].func = func;
		// create len Function
		func_args.push_back(builder.getInt32Ty()->getPointerTo());
		func = llvm::Function::Create(
				llvm::FunctionType::get(/*ret*/builder.getInt32Ty(), func_args, false),
				llvm::GlobalValue::ExternalLinkage,
				"get_memory_length", mod);
		stdfunc["len"].func = func;
		func_args.clear();
		// create str_concat_char Function
		func_args.push_back(builder.getInt32Ty()->getPointerTo());
		func = llvm::Function::Create(
				llvm::FunctionType::get(/*ret*/builder.getInt32Ty(), func_args, false),
				llvm::GlobalValue::ExternalLinkage,
				"str_concat_char", mod);
		stdfunc["concat_char"].func = func;
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
	// create entry point of main function
	llvm::BasicBlock *entry = llvm::BasicBlock::Create(context, "entry", func_main);
	builder.SetInsertPoint(entry);

	for(std::vector<AST *>::iterator it = main_code.begin(); it != main_code.end(); ++it) {
		codegen_expression(main, list, *it);
	}

	builder.CreateRet(llvm::ConstantInt::get(builder.getInt32Ty(), 0));

	std::string err;
	llvm::ExecutionEngine *exec_engine = llvm::EngineBuilder(mod).setErrorStr(&err).create();
	if(!exec_engine) error("LitSystemError: LLVMError: %s\n", err.c_str());

	{ // optimize 
		llvm::PassManager pass_mgr;
		// target lays out data structures.
		pass_mgr.add(new llvm::DataLayout(*exec_engine->getDataLayout()));
		// mem2reg
		pass_mgr.add(llvm::createPromoteMemoryToRegisterPass());
		// Provide basic AliasAnalysis support for GVN.
		pass_mgr.add(llvm::createBasicAliasAnalysisPass());
		// Do simple "peephole" optimizations and bit-twiddling optzns.
		pass_mgr.add(llvm::createInstructionCombiningPass());
		// Reassociate expressions.
		pass_mgr.add(llvm::createReassociatePass());
		pass_mgr.run(*mod);
	}

	void *prog_ptr = exec_engine->getPointerToFunction(mod->getFunction("main"));
	int (*program_entry)() = (int (*)())(int*)prog_ptr;
	mod->dump();
	
	program_entry(); // run
	
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
	llvm::SMDiagnostic smd_err;
	llvm::Module *lib_mod = llvm::ParseIRFile(("./lib/" + lib_name + ".ll"), smd_err, context);
	if(lib_mod == nullptr)
		error("LitSystemError: LLVMError: %s", smd_err.getMessage().str().c_str());
	std::string msg_err;
	if(llvm::Linker::LinkModules(mod, lib_mod, llvm::Linker::DestroySource, &msg_err))
		error("LitSystemError: LLVMError: %s", msg_err.c_str());
	for(ast_vector::iterator it = proto.begin(); it != proto.end(); ++it) {
		((PrototypeAST *)*it)->append(lib_mod, f_list);
	}
	return NULL;
}

void PrototypeAST::append(llvm::Module *lib_mod, Program &f_list) {
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

	f_list.append(f);
	f_list.rep_undef(f.info.name, func_bgn);

	// append arguments 
	std::vector<llvm::Type *> arg_types;
	std::vector<std::string> arg_names;
	for(ast_vector::iterator it = args.begin(); it != args.end(); ++it) {
		if((*it)->get_type() == AST_VARIABLE) {
			var_t *v = ((VariableAST *)*it)->append(f, f_list);
			arg_types.push_back(builder.getInt32Ty());
			arg_names.push_back(v->name);
		} else if((*it)->get_type() == AST_VARIABLE_DECL) {
			var_t *v = ((VariableDeclAST *)*it)->append(f, f_list);
			if(v->type == T_STRING) 
				arg_types.push_back(builder.getInt8PtrTy());
			else if(v->type & T_ARRAY && v->type & T_INT) 
				arg_types.push_back(builder.getInt32Ty()->getPointerTo());
			else
				arg_types.push_back(builder.getInt32Ty());
			arg_names.push_back(v->name);
		}
	}

	// definition the Function
	llvm::Type *func_ret_type = info.type == T_STRING ? (llvm::Type *)builder.getInt8PtrTy() : 
		(info.type & T_ARRAY) ? (llvm::Type *)builder.getInt32Ty()->getPointerTo() : (llvm::Type *)builder.getInt32Ty();
	llvm::FunctionType *func_type = llvm::FunctionType::get(func_ret_type, arg_types, false);
	llvm::Function *func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, f.info.name, mod);

	llvm::BasicBlock *entry = llvm::BasicBlock::Create(context, "entry", func);
	builder.SetInsertPoint(entry);

	// set argment variable names and store
	auto arg_types_it = arg_types.begin();
	auto arg_names_it = arg_names.begin();
	for(auto arg_it = func->arg_begin(); arg_it != func->arg_end(); ++arg_it) {
		arg_it->setName(*arg_names_it);
		llvm::AllocaInst *ainst = create_entry_alloca(func, *arg_names_it, (*arg_types_it));
		builder.CreateStore(arg_it, ainst);
		var_t *v = f.var.get(*arg_names_it, "");
		if(v) v->val = ainst;
		arg_types_it++; arg_names_it++;
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
	bool has_br = false;
	f.has_br.push(has_br);
	for(auto expr : then_block) 
		val_then = codegen_expression(f, f_list, expr);

	if(!f.has_br.top()) builder.CreateBr(bb_merge);
	else f.has_br.pop();
	bb_then = builder.GetInsertBlock();

	func->getBasicBlockList().push_back(bb_else);
	builder.SetInsertPoint(bb_else);

	llvm::Value *val_else = NULL;
	has_br = false;
	f.has_br.push(has_br);
	for(auto expr : else_block)
		val_else = codegen_expression(f, f_list, expr);

	if(!f.has_br.top()) builder.CreateBr(bb_merge);
	else f.has_br.pop();
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
	llvm::Function *func = builder.GetInsertBlock()->getParent();

	llvm::BasicBlock *bb_loop = llvm::BasicBlock::Create(context, "loop", func);
	llvm::BasicBlock *bb_after_loop = llvm::BasicBlock::Create(context, "after_loop", func);
	f.break_br_list.push(bb_after_loop);

	llvm::Value *frst_cond_val = builder.CreateICmpNE(
			codegen_expression(f, f_list, cond), llvm::ConstantInt::get(builder.getInt32Ty(), 0 ));
	builder.CreateCondBr(frst_cond_val, bb_loop, bb_after_loop);

	builder.SetInsertPoint(bb_loop);

	f.break_br_list.push(bb_after_loop);
	bool has_br = false; f.has_br.push(has_br);
		for(auto expr : block) codegen_expression(f, f_list, expr);
	f.has_br.pop();
	f.break_br_list.pop();

		llvm::Value *cond_val = builder.CreateICmpNE(
				codegen_expression(f, f_list, cond), llvm::ConstantInt::get(builder.getInt32Ty(), 0));

	builder.CreateCondBr(cond_val, bb_loop, bb_after_loop);

	builder.SetInsertPoint(bb_after_loop);
	f.break_br_list.pop();

	return NULL;
}

llvm::Value * ForAST::codegen(Function &f, Program &f_list) {
	llvm::Function *func = builder.GetInsertBlock()->getParent();
	codegen_expression(f, f_list, asgmt);

	llvm::BasicBlock *bb_loop = llvm::BasicBlock::Create(context, "loop", func);
	llvm::BasicBlock *bb_after_loop = llvm::BasicBlock::Create(context, "after_loop", func);

	llvm::Value *frst_cond_val = builder.CreateICmpNE(
			codegen_expression(f, f_list, cond), llvm::ConstantInt::get(builder.getInt32Ty(), 0 ));
	builder.CreateCondBr(frst_cond_val, bb_loop, bb_after_loop);

	builder.SetInsertPoint(bb_loop);

	f.break_br_list.push(bb_after_loop);
	bool has_br = false; f.has_br.push(has_br);
		for(auto expr : block) codegen_expression(f, f_list, expr);
	f.has_br.pop();
	f.break_br_list.pop();

		codegen_expression(f, f_list, step);

		llvm::Value *cond_val = builder.CreateICmpNE(
				codegen_expression(f, f_list, cond), llvm::ConstantInt::get(builder.getInt32Ty(), 0));

	if(!has_br) builder.CreateCondBr(cond_val, bb_loop, bb_after_loop);

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
					builder.CreateCall(stdfunc["put_char"].func, func_args)->setCallingConv(llvm::CallingConv::C);
				} else if(ty == T_DOUBLE) {
					builder.CreateCall(stdfunc["put_num_float"].func, func_args)->setCallingConv(llvm::CallingConv::C);
				} else if(ty & T_ARRAY) {
					builder.CreateCall(stdfunc["put_array"].func, func_args)->setCallingConv(llvm::CallingConv::C);
				} else {
					builder.CreateCall(stdfunc["put_num"].func, func_args)->setCallingConv(llvm::CallingConv::C);
				}
			}
			builder.CreateCall(stdfunc["put_ln"].func, std::vector<llvm::Value *>())->setCallingConv(llvm::CallingConv::C); // for new line
		} else {
			if(stdfunc[info.name].args == -1) { // vector
				llvm::Value *val = codegen_expression(f, f_list, args[0]);
				std::vector<llvm::Value*> func_args;
				func_args.push_back(val);
				for(int n = 1; n < args.size(); ++n)
					func_args.push_back(codegen_expression(f, f_list, args[n]));
				builder.CreateCall(stdfunc[info.name].func, func_args);
			} else { // normal function
				std::vector<llvm::Value*> func_args;
				for(auto arg : args)
					func_args.push_back(codegen_expression(f, f_list, arg));
				stdfunc_ret_value = builder.CreateCall(stdfunc[info.name].func, func_args);
			}
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

	{ // cast float to int when lhs is integer type
		if(ty_l == T_INT && ty_r == T_DOUBLE) {
			rhs = builder.CreateFPToSI(rhs, builder.getInt32Ty());
			ty_l = T_DOUBLE;
		} else if(ty_l == T_DOUBLE && ty_r == T_INT) {
			rhs = builder.CreateSIToFP(rhs, builder.getFloatTy());
		} else if(ty_l == T_CHAR) {
			rhs = builder.CreateZExt(rhs, builder.getInt8Ty());
		} else if(ty_l == T_INT && ty_r != T_INT) {
			rhs = builder.CreateZExt(rhs, builder.getInt32Ty());
		}
	}

	// if(ty_l != ty_r) if(op != "+") error("error: type error"); // except string concat
	if(op == "+") {
		if(ty_l == T_STRING && ty_r == T_STRING) { // string + string
			std::vector<llvm::Value*> func_args;
			func_args.push_back(lhs);
			func_args.push_back(rhs);
			llvm::Value *ret = builder.CreateCall(stdfunc["strcat"].func, func_args);
			return ret;	
		} else if(ty_l == T_STRING && ty_r == T_CHAR) {
			std::vector<llvm::Value*> func_args;
			func_args.push_back(lhs);
			func_args.push_back(rhs);
			llvm::Value *ret = builder.CreateCall(stdfunc["concat_char"].func, func_args);
			return ret;	
		} else if(ty_l == T_DOUBLE) {
			return builder.CreateFAdd(lhs, rhs, "addtmp");
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
	std::vector<llvm::Value*> func_args;
	func_args.push_back(codegen_expression(f, f_list, size));
	func_args.push_back(llvm::ConstantInt::get(builder.getInt32Ty(), 4));
	llvm::Value *ret = builder.CreateCall(stdfunc["create_array"].func, func_args);
	*ty = alloc_type | T_ARRAY;
	return builder.CreateBitCast(ret, builder.getInt32Ty()->getPointerTo());
}

llvm::Value * VariableAsgmtAST::codegen(Function &f, Program &f_list, int *ty) {
	var_t *v = NULL;
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
		VariableIndexAST *vidx = (VariableIndexAST *)var;
		int expr_ty;
		llvm::Value *elem = llvm::GetElementPtrInst::CreateInBounds(
				codegen_expression(f, f_list, vidx->var, &expr_ty),
				llvm::ArrayRef<llvm::Value *>(codegen_expression(f, f_list, vidx->idx)), "elem_tmp", builder.GetInsertBlock());
		llvm::Value *val = codegen_expression(f, f_list, src);
		if(expr_ty == T_STRING) {
			val = builder.CreateZExt(val, builder.getInt8Ty());
		}
		return builder.CreateStore(val, elem);
	}

	// single assignment
	int v_ty;
	llvm::Value *val = codegen_expression(f, f_list, src, &v_ty);
	*ty = v_ty;
	if(v->is_global) {
		// ntv.gencode(0xa3); f_list.append_addr_of_global_var(v->name, ntv.count); 
		// ntv.gencode_int32(0x00000000); // GLOBAL_INSERT
	} else {
		if(var->get_type() == AST_VARIABLE) v->type = v_ty; 
		if(first_decl) {
			llvm::AllocaInst *ai;
			if(v_ty == T_STRING) { // string
				ai = builder.CreateAlloca(llvm::Type::getInt8PtrTy(context), 0, v->name);
			} else if(v_ty == T_DOUBLE) {
				ai = builder.CreateAlloca(llvm::Type::getFloatTy(context), 0, v->name);
			} else if(v_ty == (T_INT | T_ARRAY)) { // integer array
				ai = builder.CreateAlloca(llvm::Type::getInt32PtrTy(context), 0, v->name);
			} else { // integer
				ai = builder.CreateAlloca(llvm::Type::getInt32Ty(context), 0, v->name);
			}
			v->val = ai;
		}
		builder.CreateStore(val, v->val);
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
	int ty;
	llvm::Value *ret = llvm::GetElementPtrInst::CreateInBounds(
			codegen_expression(f, f_list, var, &ty), 
			llvm::ArrayRef<llvm::Value *>(codegen_expression(f, f_list, idx)), "elem_tmp", builder.GetInsertBlock());
	ret = builder.CreateLoad(ret);
	*ret_ty = ty & T_INT ? T_INT : ty & T_STRING ? T_CHAR : T_STRING;
	return ret;
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
		return builder.CreateLoad(v->val, "var_tmp");
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
	f.has_br.top() = true;
	return builder.CreateRet(codegen_expression(f, f_list, expr));
}

llvm::Value * BreakAST::codegen(Function &f, Program &f_list) {
	f.has_br.top() = true;
	builder.CreateBr(f.break_br_list.top());
	return NULL;
}

llvm::Value * ArrayAST::codegen(Function &f, Program &f_list, int *ret_ty) {
	std::vector<llvm::Value*> func_args;
	func_args.push_back(llvm::ConstantInt::get(builder.getInt32Ty(), elems.size()));
	func_args.push_back(llvm::ConstantInt::get(builder.getInt32Ty(), 4));
	llvm::Value *ary = builder.CreateCall(stdfunc["create_array"].func, func_args);
	ary = builder.CreateBitCast(ary, builder.getInt32Ty()->getPointerTo(), "bitcast_tmp");
	uint32_t a = 0;
	int ty;
	for(ast_vector::iterator it = elems.begin(); it != elems.end(); ++it) {
		int expr_ty;
		llvm::Value *elem = llvm::GetElementPtrInst::CreateInBounds(
				ary,
				llvm::ArrayRef<llvm::Value *>(llvm::ConstantInt::get(builder.getInt32Ty(), a)), "elem_tmp", builder.GetInsertBlock());
		llvm::Value *val = codegen_expression(f, f_list, *it, &ty);
		builder.CreateStore(val, elem);
		a += 1;
	}
	*ret_ty = T_ARRAY | ty;
	return ary;
}

llvm::Value *StringAST::codegen(Function &f, int *type) {
	 // llvm::Value *value = builder.CreateGlobalStringPtr(<STRING>);
	*type = T_STRING;
	char *embed = (char *)LitMemory::alloc_const(str.length() + 1); // TODO: fix!
	replace_escape(strcpy(embed, str.c_str()));
	return builder.CreateGlobalStringPtr(str.c_str());
}

llvm::Value * CharAST::codegen(Function &f, int *ty) {
	*ty = T_CHAR;
	return llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), ch);
}

llvm::Value * NumberAST::codegen(Function &f, int *ty) {
	*ty = T_INT;
	return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), number, true);
}
llvm::Value * FloatNumberAST::codegen(Function &f, int *ty) {
	*ty = T_DOUBLE;
	return llvm::ConstantFP::get(builder.getFloatTy(), number);
}
