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
		for(int i = 0; i < size; i++) {
			if(LitMemory::is_allocated_addr((void *)ary[i]))
				put_array((int *)ary[i]);
			else
				printf("%d ", ary[i]);
		}
		printf("] ");
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
	char *str_concat_char_str(char a, char *b) {
		char *t = (char *)LitMemory::alloc(strlen(b) + 2, 1);
		t[0] = a;
		strcpy(&(t[1]), b);
		return t;
	}
	char *str_copy(char *a) {
		char *t = (char *)LitMemory::alloc(strlen(a) + 1, sizeof(char));
		if(!t) return NULL;
		return strcpy(t, a);
	}
	char *get_string_stdin() {
		std::string input_from_stdin;
		std::getline(std::cin, input_from_stdin);
		char *ret = (char *)LitMemory::alloc(input_from_stdin.size() + 1, sizeof(char));
		return strcpy(ret, input_from_stdin.c_str());
	}
	int str_to_int(char *str) {
		return atoi(str);
	}
	char *int_to_str(int n) {
		char buf[16], *ret;
		sprintf(buf, "%d", n);
		ret = (char *)LitMemory::alloc(strlen(buf)+1, sizeof(char));
		return strcpy(ret, buf);
	}
	int get_memory_length(void *ptr) {
		return LitMemory::get_size(ptr);
	}
	void append_addr_for_gc(void *addr) {
		LitMemory::append_ptr(addr);
	}
	void delete_addr_for_gc(void *addr) {
		LitMemory::delete_ptr(addr);
	}
}

llvm::AllocaInst *create_entry_alloca(llvm::Function *TheFunction, std::string &VarName, llvm::Type *type =  NULL) {
	llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
			TheFunction->getEntryBlock().begin());
	return TmpB.CreateAlloca(type == NULL ? llvm::Type::getInt32Ty(context) : type, 0, VarName.c_str());
}

namespace Codegen {
	llvm::Module *codegen(ast_vector &program) {
		llvm::InitializeNativeTarget();
		mod = new llvm::Module("LIT", context);
		{ // initialize standard functions
			stdfunc["create_array"] = {"create_array", 1, T_ARRAY};
			stdfunc["gets"] = {"gets", 0, T_STRING};
			stdfunc["strlen"] = {"strlen", 1, T_INT};
			stdfunc["printf"] = {"printf", -1, T_VOID};
			stdfunc["puts"] = {"puts", -1, T_VOID};
			stdfunc["print"] = {"print", -1, T_VOID};
			stdfunc["put_num"] = {"put_num", 1, T_VOID};
			stdfunc["put_num_float"] = {"put_num_float", 1, T_VOID};
			stdfunc["put_char"] = {"put_char", 1, T_VOID};
			stdfunc["put_array"] = {"put_array", 1, T_VOID};
			stdfunc["put_string"] = {"put_string", 1, T_VOID};
			stdfunc["strcat"] = {"strcat", 2, T_STRING};
			stdfunc["concat_char_str"] = {"concat_char_str", 2, T_STRING};
			stdfunc["str_to_int"] = {"str_to_int", 1, T_INT};
			stdfunc["int_to_str"] = {"int_to_str", 1, T_STRING};
			stdfunc["len"] = {"len", 1, T_INT};
			stdfunc["str_copy"] = {"str_copy", 1, T_STRING};
			stdfunc["append_addr_for_gc"] = {"append_addr_for_gc", 1, T_VOID};

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
			// create str_to_int function
			func_args.push_back(builder.getInt8PtrTy());
			func = llvm::Function::Create(
					llvm::FunctionType::get(/*ret*/builder.getInt32Ty(), func_args, false),
					llvm::GlobalValue::ExternalLinkage,
					"str_to_int", mod);
			stdfunc["str_to_int"].func = func;
			func_args.clear();
			// create int_to_str function
			func_args.push_back(builder.getInt32Ty());
			func = llvm::Function::Create(
					llvm::FunctionType::get(/*ret*/builder.getInt8Ty()->getPointerTo(), func_args, false),
					llvm::GlobalValue::ExternalLinkage,
					"int_to_str", mod);
			stdfunc["int_to_str"].func = func;
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
			// create str_copy Function
			func_args.push_back(builder.getInt8Ty()->getPointerTo());
			func = llvm::Function::Create(
					llvm::FunctionType::get(/*ret*/builder.getInt8PtrTy(), func_args, false),
					llvm::GlobalValue::ExternalLinkage,
					"str_copy", mod);
			stdfunc["str_copy"].func = func;
			func_args.clear();
			// create strlen Function
			func_args.push_back(builder.getInt8Ty()->getPointerTo());
			func = llvm::Function::Create(
					llvm::FunctionType::get(/*ret*/builder.getInt32Ty(), func_args, false),
					llvm::GlobalValue::ExternalLinkage,
					"strlen", mod);
			stdfunc["strlen"].func = func;
			func_args.clear();
			// create str_concat_char Function
			func_args.push_back(builder.getInt8Ty()->getPointerTo());
			func_args.push_back(builder.getInt8Ty());
			func = llvm::Function::Create(
					llvm::FunctionType::get(/*ret*/builder.getInt8PtrTy(), func_args, false),
					llvm::GlobalValue::ExternalLinkage,
					"str_concat_char", mod);
			stdfunc["concat_char"].func = func;
			func_args.clear();
			// create str_concat_char Function
			func_args.push_back(builder.getInt8Ty());
			func_args.push_back(builder.getInt8Ty()->getPointerTo());
			func = llvm::Function::Create(
					llvm::FunctionType::get(/*ret*/builder.getInt8PtrTy(), func_args, false),
					llvm::GlobalValue::ExternalLinkage,
					"str_concat_char_str", mod);
			stdfunc["concat_char_str"].func = func;
			func_args.clear();
			// create append_addr_for_gc Function
			func_args.push_back(builder.getVoidTy()->getPointerTo());
			func = llvm::Function::Create(
					llvm::FunctionType::get(/*ret*/builder.getInt32Ty(), func_args, false),
					llvm::GlobalValue::ExternalLinkage,
					"append_addr_for_gc", mod);
			stdfunc["append_addr_for_gc"].func = func;
			func_args.clear();
			// create append_addr_for_gc Function
			func_args.push_back(builder.getVoidTy()->getPointerTo());
			func = llvm::Function::Create(
					llvm::FunctionType::get(/*ret*/builder.getInt32Ty(), func_args, false),
					llvm::GlobalValue::ExternalLinkage,
					"delete_addr_for_gc", mod);
			stdfunc["delete_addr_for_gc"].func = func;
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
			expression(main, list, *it);
		}

		builder.CreateRet(llvm::ConstantInt::get(builder.getInt32Ty(), 0));

		return mod;
	}

	int run(llvm::Module *module, bool enable_optimize, bool enable_emit_llvm) {
		std::string err;
		llvm::ExecutionEngine *exec_engine = llvm::EngineBuilder(module).setErrorStr(&err).create();
		if(!exec_engine) error("LitSystemError: LLVMError: %s\n", err.c_str());

		{ // optimize 
			llvm::PassManager pass_mgr;
			// target lays out data structures.
			pass_mgr.add(new llvm::DataLayout(*exec_engine->getDataLayout()));
			// mem2reg
			pass_mgr.add(llvm::createPromoteMemoryToRegisterPass());
			// // Provide basic AliasAnalysis support for GVN.
			// pass_mgr.add(llvm::createBasicAliasAnalysisPass());
			// // Do simple "peephole" optimizations and bit-twiddling optzns.
			// pass_mgr.add(llvm::createInstructionCombiningPass());
			// // Reassociate expressions.
			// pass_mgr.add(llvm::createReassociatePass());
			pass_mgr.run(*module);
		}
		if(enable_emit_llvm) module->dump();

		void *prog_ptr = exec_engine->getPointerToFunction(module->getFunction("main"));
		int (*program_entry)() = (int (*)())(int*)prog_ptr;
		return program_entry(); // run
	}

	llvm::Value *expression(Function &f, Program &f_list, AST *ast, ExprType *ty) {
		ExprType buf; if(ty == NULL) ty = &buf;
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
};

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
	f.info.name = name.empty() ? proto.name : name;
	f.info.params = args_type.size();
	f.info.type = proto.type;
	f.info.func_addr = mod->getFunction(proto.name);
	std::vector<ExprType *> proto_args_type;
	for(auto it = args_type.begin(); it != args_type.end(); ++it) {
		AST *ast = (*it);
		if(ast->get_type() == AST_VARIABLE)
			proto_args_type.push_back(new ExprType(T_INT));
		else if(ast->get_type() == AST_VARIABLE_DECL) 
			proto_args_type.push_back(new ExprType(((VariableDeclAST *)ast)->info.type));
	}
	f.info.args_type = proto_args_type;
	f_list.append(f);
}

Function FunctionAST::codegen(Program &f_list) {
	Function f;
	f.info.name = info.name;
	f.info.mod_name = "";
	f.info.address = ntv.count;
	f.info.params = args.size(); 	
	f.info.func_addr = NULL;
	f.info.type.change(new ExprType(info.type));
	uint32_t func_bgn = ntv.count;

	// append arguments 
	std::vector<llvm::Type *> arg_types;
	std::vector<std::string> arg_names;
	std::vector<ExprType *> args_type_for_overload;
	for(ast_vector::iterator it = args.begin(); it != args.end(); ++it) {
		if((*it)->get_type() == AST_VARIABLE) {
			var_t *v = ((VariableAST *)*it)->append(f, f_list);
			arg_types.push_back(builder.getInt32Ty());
			arg_names.push_back(v->name);
			args_type_for_overload.push_back(new ExprType(T_INT));
		} else if((*it)->get_type() == AST_VARIABLE_DECL) {
			var_t *v = ((VariableDeclAST *)*it)->append(f, f_list);
			if(v->type.eql_type(T_STRING))
				arg_types.push_back(builder.getInt8PtrTy());
			else if(v->type.is_array() && v->type.next->eql_type(T_STRING)) // string array?
				arg_types.push_back(builder.getInt8PtrTy()->getPointerTo());
			else if(v->type.is_array()) // int array? 
				arg_types.push_back(builder.getInt32Ty()->getPointerTo());
			else
				arg_types.push_back(builder.getInt32Ty());
			arg_names.push_back(v->name);
			args_type_for_overload.push_back(new ExprType(v->type));
		}
	}
	f.info.args_type = args_type_for_overload;

	Function *function = f_list.append(f);
	f_list.rep_undef(f.info.name, func_bgn);

	// definition the Function
	
	// set function return type
	llvm::Type *func_ret_type = 
		info.type.eql_type(T_STRING) ? 
		(llvm::Type *)builder.getInt8PtrTy() : 
		(info.type.eql_type(T_ARRAY)) ? 
			(info.type.next->eql_type(T_STRING)) ? 
				(llvm::Type *)builder.getInt8PtrTy()->getPointerTo() : 
				(llvm::Type *)builder.getInt32Ty()->getPointerTo() : 
			(llvm::Type *)builder.getInt32Ty();

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

	for(auto v = f.var.local.begin(); v != f.var.local.end(); v++) {
		if(v->type.is_array() || v->type.eql_type(T_STRING)) {
			std::vector<llvm::Value*> func_args;
			func_args.push_back(builder.CreateBitCast(v->val, v->val->getType()->getPointerTo())); // get address of var
			builder.CreateCall(stdfunc["append_addr_for_gc"].func, func_args);
			// if(v->type.eql_type(T_STRING)) {
			// 	v->val = builder.CreateCall(stdfunc["str_copy"].func, v->val);
			// }
		}
	}

	llvm::Value *ret_value = llvm::ConstantInt::get(builder.getInt32Ty(), 0); // default return code 0
	for(ast_vector::iterator it = statement.begin(); it != statement.end(); ++it) { // function body
		ret_value = Codegen::expression(f, f_list, *it);
	}

	for(auto v = f.var.local.begin(); v != f.var.local.end(); v++) {
		if(v->type.is_array() || v->type.eql_type(T_STRING)) {
			std::vector<llvm::Value*> func_args;
			func_args.push_back(builder.CreateBitCast(v->val, v->val->getType()->getPointerTo())); // get address of var
			builder.CreateCall(stdfunc["delete_addr_for_gc"].func, func_args);
		}
	}

	if(ret_value) {
		if(ret_value->getType()->getTypeID() != func_ret_type->getTypeID()) {
			ret_value = llvm::ConstantInt::get(builder.getInt32Ty(), 0);
			fprintf(stderr, "warning: type of expression that evaluated last is not match\n");
		}
	} else if(func_ret_type->getTypeID() == builder.getInt32Ty()->getTypeID()) {
		ret_value = llvm::ConstantInt::get(builder.getInt32Ty(), 0);
	} else error("error: return code of function is incorrect");

	builder.CreateRet(ret_value);

	function->info.func_addr = func;

	return f;
}

llvm::Value * IfAST::codegen(Function &f, Program &f_list) {
	llvm::Value *cond_val = Codegen::expression(f, f_list, cond);
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
		val_then = Codegen::expression(f, f_list, expr);

	if(!f.has_br.top()) builder.CreateBr(bb_merge);
	else f.has_br.pop();
	bb_then = builder.GetInsertBlock();

	func->getBasicBlockList().push_back(bb_else);
	builder.SetInsertPoint(bb_else);

	llvm::Value *val_else = NULL;
	has_br = false;
	f.has_br.push(has_br);
	for(auto expr : else_block)
		val_else = Codegen::expression(f, f_list, expr);

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
			Codegen::expression(f, f_list, cond), llvm::ConstantInt::get(builder.getInt32Ty(), 0 ));
	builder.CreateCondBr(frst_cond_val, bb_loop, bb_after_loop);

	builder.SetInsertPoint(bb_loop);

	f.break_br_list.push(bb_after_loop);
	bool has_br = false; f.has_br.push(has_br);
	for(auto expr : block) Codegen::expression(f, f_list, expr);
	f.has_br.pop();
	f.break_br_list.pop();

	llvm::Value *cond_val = builder.CreateICmpNE(
			Codegen::expression(f, f_list, cond), llvm::ConstantInt::get(builder.getInt32Ty(), 0));

	builder.CreateCondBr(cond_val, bb_loop, bb_after_loop);

	builder.SetInsertPoint(bb_after_loop);
	f.break_br_list.pop();

	return NULL;
}

llvm::Value * ForAST::codegen(Function &f, Program &f_list) {
	llvm::Function *func = builder.GetInsertBlock()->getParent();
	Codegen::expression(f, f_list, asgmt);

	llvm::BasicBlock *bb_loop = llvm::BasicBlock::Create(context, "loop", func);
	llvm::BasicBlock *bb_after_loop = llvm::BasicBlock::Create(context, "after_loop", func);

	llvm::Value *frst_cond_val = builder.CreateICmpNE(
			Codegen::expression(f, f_list, cond), llvm::ConstantInt::get(builder.getInt32Ty(), 0 ));
	builder.CreateCondBr(frst_cond_val, bb_loop, bb_after_loop);

	builder.SetInsertPoint(bb_loop);

	f.break_br_list.push(bb_after_loop);
	bool has_br = false; f.has_br.push(has_br);
	for(auto expr : block) Codegen::expression(f, f_list, expr);
	f.has_br.pop();
	f.break_br_list.pop();

	Codegen::expression(f, f_list, step);

	llvm::Value *cond_val = builder.CreateICmpNE(
			Codegen::expression(f, f_list, cond), llvm::ConstantInt::get(builder.getInt32Ty(), 0));

	if(!has_br) builder.CreateCondBr(cond_val, bb_loop, bb_after_loop);

	builder.SetInsertPoint(bb_after_loop);

	return NULL;
}

llvm::Value * FunctionCallAST::codegen(Function &f, Program &f_list, ExprType *ty) {
	if(stdfunc.count(info.name)) {
		llvm::Value *stdfunc_ret_value = llvm::ConstantInt::get(builder.getInt32Ty(), 0);
		if(info.name == "Array") { // TODO: implementation ASAP
		} else if(info.name == "puts" || info.name == "print") {
			for(int n = 0; n < args.size(); n++) {
				ExprType ty;
				llvm::Value *val = Codegen::expression(f, f_list, args[n], &ty);
				std::vector<llvm::Value*> func_args;
				func_args.push_back(val);
				if(ty.eql_type(T_STRING)) {
					builder.CreateCall(stdfunc["put_string"].func, func_args)->setCallingConv(llvm::CallingConv::C);
				} else if(ty.eql_type(T_CHAR)) {
					builder.CreateCall(stdfunc["put_char"].func, func_args)->setCallingConv(llvm::CallingConv::C);
				} else if(ty.eql_type(T_DOUBLE)) {
					builder.CreateCall(stdfunc["put_num_float"].func, func_args)->setCallingConv(llvm::CallingConv::C);
				} else if(ty.is_array()) {
					builder.CreateCall(stdfunc["put_array"].func, func_args)->setCallingConv(llvm::CallingConv::C);
				} else {
					builder.CreateCall(stdfunc["put_num"].func, func_args)->setCallingConv(llvm::CallingConv::C);
				}
			}
			if(info.name == "puts")
				builder.CreateCall(stdfunc["put_ln"].func, std::vector<llvm::Value *>())->setCallingConv(llvm::CallingConv::C); // for new line
		} else {
			if(stdfunc[info.name].args == -1) { // vector
				llvm::Value *val = Codegen::expression(f, f_list, args[0]);
				std::vector<llvm::Value*> func_args;
				func_args.push_back(val);
				for(int n = 1; n < args.size(); ++n)
					func_args.push_back(Codegen::expression(f, f_list, args[n]));
				builder.CreateCall(stdfunc[info.name].func, func_args);
			} else { // normal function
				std::vector<llvm::Value*> func_args;
				for(auto arg : args)
					func_args.push_back(Codegen::expression(f, f_list, arg));
				stdfunc_ret_value = builder.CreateCall(stdfunc[info.name].func, func_args);
			}
		}
		ty->change(stdfunc[info.name].type);
		return stdfunc_ret_value;
	}

	// user Function
	// process args and get args type
	std::vector<ExprType *> args_type;
	std::vector<llvm::Value *> callee_args;
	for(auto arg = args.begin(); arg != args.end(); ++arg) {
		ExprType ty;
		callee_args.push_back(Codegen::expression(f, f_list, *arg, &ty));
		args_type.push_back(new ExprType(ty));
	}
	Function *function = f_list.get(info.name, args_type);
	llvm::Function *callee = (function->info.func_addr) ? function->info.func_addr : mod->getFunction(info.name);
	if(!callee) error("no function: %s", info.name.c_str());
	if(function == NULL) { // undefined
		uint32_t a = 3;
		for(ast_vector::iterator it = args.begin(); it != args.end(); ++it) {
			Codegen::expression(f, f_list, *it);
			ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(256 - a++ * ADDR_SIZE); // mov [esp+ADDR*a], eax
		}
		ntv.gencode(0xe8); f_list.append_undef(info.name, info.mod_name, ntv.count);
		ntv.gencode_int32(0x00000000); // call function
		ty->change(T_INT);
		return NULL;
	} else { // defined
		ty->change(new ExprType(function->info.type));
		return builder.CreateCall(callee, callee_args, "call_tmp");
	}
}

llvm::Value * BinaryAST::codegen(Function &f, Program &f_list, ExprType *ty) {
	ExprType ty_l(T_VOID), ty_r(T_VOID);
	llvm::Value *lhs = Codegen::expression(f, f_list, left, &ty_l);
	llvm::Value *rhs = Codegen::expression(f, f_list, right, &ty_r);
	ty->change(new ExprType(ty_l));

	std::vector<ExprType *> args_type;
	args_type.push_back(&ty_l);
	args_type.push_back(&ty_r);
	Function *user_op = f_list.get("operator"+op, args_type);
	if(user_op) {
		llvm::Function *callee = user_op->info.func_addr;
		ty->change(new ExprType(user_op->info.type));
		llvm::Value *callee_args[] = {lhs, rhs};
		return builder.CreateCall(callee, callee_args, "call_tmp");
	}

	{ // cast float to int when lhs is integer type
		if(ty_l.eql_type(T_INT) && ty_r.eql_type(T_DOUBLE)) {
			rhs = builder.CreateFPToSI(rhs, builder.getInt32Ty());
			ty_l = T_DOUBLE;
		} else if(ty_l.eql_type(T_DOUBLE) && ty_r.eql_type(T_INT)) {
			rhs = builder.CreateSIToFP(rhs, builder.getFloatTy());
		} else if(ty_l.eql_type(T_CHAR)) {
			rhs = builder.CreateZExt(rhs, builder.getInt8Ty());
		} else if(ty_l.eql_type(T_INT) && !ty_r.eql_type(T_INT)) {
			rhs = builder.CreateZExt(rhs, builder.getInt32Ty());
		}
	}

	if(!ty_l.eql_type(&ty_r)) {
		// std::cout << "warning: different types to each other: " << ty_l.to_string() << ", " << ty_r.to_string() << std::endl;
	}
	if(op == "+") {
		if(ty_l.eql_type(T_STRING) && ty_r.eql_type(T_STRING)) { // string + string
			std::vector<llvm::Value*> func_args;
			func_args.push_back(lhs);
			func_args.push_back(rhs);
			llvm::Value *ret = builder.CreateCall(stdfunc["strcat"].func, func_args);
			return ret;	
		} else if(ty_l.eql_type(T_STRING) && ty_r.eql_type(T_CHAR)) {
			std::vector<llvm::Value*> func_args;
			func_args.push_back(lhs);
			func_args.push_back(rhs);
			llvm::Value *ret = builder.CreateCall(stdfunc["concat_char"].func, func_args);
			return ret;	
		} else if(ty_l.eql_type(T_CHAR) && ty_r.eql_type(T_STRING)) {
			std::vector<llvm::Value*> func_args;
			func_args.push_back(lhs);
			func_args.push_back(rhs);
			llvm::Value *ret = builder.CreateCall(stdfunc["concat_char_str"].func, func_args);
			ty->change(T_STRING);
			return ret;	
		} else if(ty_l.eql_type(T_DOUBLE)) {
			return builder.CreateFAdd(lhs, rhs, "addtmp");
		} else {
			return builder.CreateAdd(lhs, rhs, "addtmp");
		}
	} else if(op == "-") {
		if(ty_l.eql_type(T_DOUBLE))
			return builder.CreateFSub(lhs, rhs, "subtmp");
		else 
			return builder.CreateSub(lhs, rhs, "subtmp");
	} else if(op == "*") {
		if(ty_l.eql_type(T_DOUBLE))
			return builder.CreateFMul(lhs, rhs, "multmp");
		else
			return builder.CreateMul(lhs, rhs, "multmp");
	} else if(op == "/") { 
		if(ty_l.eql_type(T_DOUBLE))
			return builder.CreateFDiv(lhs, rhs, "divtmp");
		else
			return builder.CreateSDiv(lhs, rhs, "divtmp");
	} else if(op == "%") return builder.CreateSRem(lhs, rhs, "remtmp"); 
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

llvm::Value * NewAllocAST::codegen(Function &f, Program &f_list, ExprType *ty) {
	int alloc_type = Type::str_to_type(type);
	std::vector<llvm::Value*> func_args;
	func_args.push_back(Codegen::expression(f, f_list, size));
	func_args.push_back(llvm::ConstantInt::get(builder.getInt32Ty(), 4));
	llvm::Value *ret = builder.CreateCall(stdfunc["create_array"].func, func_args);
	ty->change(T_ARRAY, new ExprType(alloc_type));
	return builder.CreateBitCast(ret, 
			alloc_type == T_STRING ? 
			builder.getInt8Ty()->getPointerTo() : 
			builder.getInt32Ty()->getPointerTo(), "bitcast_tmp");
}

llvm::Value * VariableAsgmtAST::codegen(Function &f, Program &f_list, ExprType *ty) {
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
		ExprType expr_ty;
		llvm::Value *elem = llvm::GetElementPtrInst::CreateInBounds(
				Codegen::expression(f, f_list, vidx->var, &expr_ty),
				llvm::ArrayRef<llvm::Value *>(Codegen::expression(f, f_list, vidx->idx)), "elem_tmp", builder.GetInsertBlock());
		llvm::Value *val = Codegen::expression(f, f_list, src);
		if(expr_ty.eql_type(T_STRING)) {
			val = builder.CreateZExt(val, builder.getInt8Ty());
		}
		return builder.CreateStore(val, elem);
	}

	// single assignment
	ExprType v_ty;
	llvm::Value *val = Codegen::expression(f, f_list, src, &v_ty);
	ty->change(new ExprType(v_ty));
	if(v->is_global) {
		if(first_decl) {
			mod->getOrInsertGlobal(v->name, builder.getInt32Ty());
			llvm::GlobalVariable *gbl = mod->getNamedGlobal(v->name);
			// gbl->setLinkage(llvm::GlobalValue::CommonLinkage);
			gbl->setAlignment(4);
			gbl->setInitializer(llvm::ConstantInt::get(builder.getInt32Ty(), 0));
			v->val = gbl;
		}
		builder.CreateStore(val, v->val);
	} else {
		if(var->get_type() == AST_VARIABLE) v->type = v_ty;
		if(first_decl) {
			llvm::AllocaInst *ai;
			if(v_ty.eql_type(T_STRING)) { // string
				ai = builder.CreateAlloca(llvm::Type::getInt8PtrTy(context), 0, v->name);
			} else if(v_ty.eql_type(T_DOUBLE)) {
				ai = builder.CreateAlloca(llvm::Type::getFloatTy(context), 0, v->name);
			} else if(v_ty.eql_type(T_ARRAY)) { // integer array TODO:FIX
				ExprType *elem_ty = &v_ty;
				while(elem_ty->eql_type(T_ARRAY)) 
					elem_ty = elem_ty->next;
				if(elem_ty->eql_type(T_INT))
					ai = builder.CreateAlloca(llvm::Type::getInt32PtrTy(context), 0, v->name);
				else if(elem_ty->eql_type(T_STRING)) 
					ai = builder.CreateAlloca(llvm::Type::getInt8Ty(context)->getPointerTo()->getPointerTo(), 0, v->name);
			} else { // integer
				ai = builder.CreateAlloca(llvm::Type::getInt32Ty(context), 0, v->name);
			}
			v->val = ai;
		}
		if(v->type.is_array() || v->type.eql_type(T_STRING)) {
			std::vector<llvm::Value*> func_args;
			func_args.push_back(builder.CreateBitCast(v->val, v->val->getType()->getPointerTo())); // addr
			builder.CreateCall(stdfunc["append_addr_for_gc"].func, func_args);
		}
		// if(v->type.eql_type(T_STRING)) {
		// 	val = builder.CreateCall(stdfunc["str_copy"].func, val);
		// }
		builder.CreateStore(val, v->val);
	}
	return NULL;
}

var_t *VariableDeclAST::get(Function &f, Program &f_list) {
	if(info.is_global) return f_list.var_global.get(info.name, info.mod_name);
	return f.var.get(info.name, info.mod_name);
}

var_t *VariableDeclAST::append(Function &f, Program &f_list) {
	if(info.is_global) return f_list.var_global.append(info.name, new ExprType(info.type),true);
	return f.var.append(info.name, new ExprType(info.type));
}

llvm::Value * VariableIndexAST::codegen(Function &f, Program &f_list, ExprType *ret_ty) {
	ExprType ty;
	llvm::Value *ret = llvm::GetElementPtrInst::CreateInBounds(
			Codegen::expression(f, f_list, var, &ty), 
			llvm::ArrayRef<llvm::Value *>(Codegen::expression(f, f_list, idx)), "elem_tmp", builder.GetInsertBlock());
	ret = builder.CreateLoad(ret);
	if(ty.eql_type(T_STRING))
		ret_ty->change(T_CHAR);
	else {
		ret_ty->change(ty.next);
	}
	return ret;
}

llvm::Value * VariableAST::codegen(Function &f, Program &f_list, ExprType *ty) {
	var_t *v;
	if(info.is_global) {
		v = f_list.var_global.get(info.name, info.mod_name);
		if(v == NULL) {
			v = f_list.append_global_var(info.name, info.type.get().type); // global variable can be used if didn't declared
			mod->getOrInsertGlobal(v->name, builder.getInt32Ty());
			llvm::GlobalVariable *gbl = mod->getNamedGlobal(v->name);
			gbl->setAlignment(4);
			gbl->setInitializer(llvm::ConstantInt::get(builder.getInt32Ty(), 0));
			v->val = gbl;
		}
	} else 
		v = f.var.get(info.name, info.mod_name);
	if(v == NULL) error("error: '%s' was not declared", info.name.c_str());
	if(info.is_global == false) { // local
		ty->change(&v->type);
		return builder.CreateLoad(v->val, "var_tmp");
	} else { // global
		ty->change(T_INT);
		return builder.CreateLoad(v->val, "var_tmp");
	}
}
var_t *VariableAST::get(Function &f, Program &f_list) {
	if(info.is_global) return f_list.var_global.get(info.name, info.mod_name);
	return f.var.get(info.name, info.mod_name);
}
var_t *VariableAST::append(Function &f, Program &f_list) {
	if(info.is_global) return f_list.append_global_var(info.name, info.type.get().type);
	return f.var.append(info.name, new ExprType(info.type));
}

llvm::Value *ReturnAST::codegen(Function &f, Program &f_list) {
	f.has_br.top() = true;
	return builder.CreateRet(Codegen::expression(f, f_list, expr));
}

llvm::Value * BreakAST::codegen(Function &f, Program &f_list) {
	f.has_br.top() = true;
	builder.CreateBr(f.break_br_list.top());
	return NULL;
}

llvm::Value * ArrayAST::codegen(Function &f, Program &f_list, ExprType *ret_ty) {
	std::vector<llvm::Value*> func_args;
	func_args.push_back(llvm::ConstantInt::get(builder.getInt32Ty(), elems.size()));
	func_args.push_back(llvm::ConstantInt::get(builder.getInt32Ty(), 4));
	llvm::Value *ary = builder.CreateCall(stdfunc["create_array"].func, func_args);
	ExprType ty;
	size_t num = 0;
	for(ast_vector::iterator it = elems.begin(); it != elems.end(); ++num, ++it) {
		llvm::Value *val = Codegen::expression(f, f_list, *it, &ty);
		if(it == elems.begin())
			ary = builder.CreateBitCast(ary, val->getType()->getPointerTo(), "bitcast_tmp");
		llvm::Value *elem = llvm::GetElementPtrInst::CreateInBounds(
				ary,
				llvm::ArrayRef<llvm::Value *>(llvm::ConstantInt::get(builder.getInt32Ty(), num)), "elem_tmp", builder.GetInsertBlock());
		builder.CreateStore(val, elem);
	}
	ret_ty->change(T_ARRAY, new ExprType(ty));
	return ary;
}

llvm::Value *StringAST::codegen(Function &f, ExprType *ty) {
	ty->change(T_STRING);
	char *embed = (char *)LitMemory::alloc_const(str.length() + 1); // TODO: fix!
	replace_escape(strcpy(embed, str.c_str()));
	return builder.CreateGlobalStringPtr(str.c_str());
}

llvm::Value * CharAST::codegen(Function &f, ExprType *ty) {
	ty->change(T_CHAR);
	return llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), ch);
}

llvm::Value * NumberAST::codegen(Function &f, ExprType *ty) {
	ty->change(T_INT);
	return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), number, true);
}
llvm::Value * FloatNumberAST::codegen(Function &f, ExprType *ty) {
	ty->change(T_DOUBLE);
	return llvm::ConstantFP::get(builder.getFloatTy(), number);
}

