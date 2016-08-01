#include "codegen.h"
#include "lit.h"
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

struct func_body_t { // To define the function that is used before defining
	Function *info;
	std::vector<std::string> arg_names, cur_mod;
	std::vector<llvm::Type *> arg_types;
	ast_vector body;
	llvm::Function *func;
	llvm::Type *ret_type;
};

std::vector<func_body_t> funcs_body;
std::map<std::string, stdfunc_t> stdfunc;

extern "C" {
	void put_num(int n) {
		printf("%d", n);
	}
	void put_num64(long long int n) {
		printf("%lld", n);
	}
	void put_num_float(double n) {
		printf("%.10g", n);
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
			if(LitMemory::is_allocated_addr(reinterpret_cast<void *>(ary[i])))
				put_array(reinterpret_cast<int *>(ary[i]));
			else
				printf("%d ", ary[i]);
		}
		printf("] ");
	}
	void put_array_float(double *ary) {
		int size = LitMemory::get_size(ary);
		if(size == -1) return;
		printf("[ ");
		for(int i = 0; i < size; i++) {
			printf("%.10g ", ary[i]);
		}
		printf("] ");
	}
	void put_array_str(char *ary[]) {
		int size = LitMemory::get_size(ary);
		if(size == -1) return;
		printf("[ ");
		for(int i = 0; i < size; i++) {
			// if(LitMemory::is_allocated_addr((void *)ary[i]))
			// 	put_array_str((char **)ary[i]);
			// else
				printf("%s ", ary[i]);
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
	double str_to_float(char *str) {
		return atof(str);
	}
	char *int_to_str(int n) {
		char buf[16]; sprintf(buf, "%d", n);
		return strcpy((char *)LitMemory::alloc(strlen(buf)+1, sizeof(char)), buf);
	}
	char *int64_to_str(long long int n) {
		char buf[32]; sprintf(buf, "%lld", n);
		return strcpy((char *)LitMemory::alloc(strlen(buf)+1, sizeof(char)), buf);
	}
	char *float_to_str(double f) {
		char buf[16], *ret;
		sprintf(buf, "%.10g", f);
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
	void run_gc() {
		LitMemory::gc();
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
			stdfunc["put_num64"] = {"put_num64", 1, T_VOID};
			stdfunc["put_num_float"] = {"put_num_float", 1, T_VOID};
			stdfunc["put_char"] = {"put_char", 1, T_VOID};
			stdfunc["put_array"] = {"put_array", 1, T_VOID};
			stdfunc["put_array_float"] = {"put_array_float", 1, T_VOID};
			stdfunc["put_array_str"] = {"put_array_str", 1, T_VOID};
			stdfunc["put_string"] = {"put_string", 1, T_VOID};
			stdfunc["strcat"] = {"strcat", 2, T_STRING};
			stdfunc["concat_char_str"] = {"concat_char_str", 2, T_STRING};
			stdfunc["str_to_int"] = {"str_to_int", 1, T_INT};
			stdfunc["str_to_float"] = {"str_to_float", 1, T_DOUBLE};
			stdfunc["int_to_str"] = {"int_to_str", 1, T_STRING};
			stdfunc["float_to_str"] = {"float_to_str", 1, T_STRING};
			stdfunc["builtinlength"] = {"builtinlength", 1, T_INT};
			stdfunc["str_copy"] = {"str_copy", 1, T_STRING};
			stdfunc["GC"] = {"GC", 0, T_VOID};

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
					llvm::FunctionType::get(/*ret*/builder.getInt32Ty(), func_args, true),
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
			// create put_num64 function
			func_args.push_back(builder.getInt64Ty());
			func = llvm::Function::Create(
					llvm::FunctionType::get(/*ret*/builder.getVoidTy(), func_args, false),
					llvm::GlobalValue::ExternalLinkage,
					"put_num64", mod);
			stdfunc["put_num64"].func = func;
			func_args.clear();
			// create put_array function
			func_args.push_back(builder.getInt32Ty()->getPointerTo());
			func = llvm::Function::Create(
					llvm::FunctionType::get(/*ret*/builder.getVoidTy(), func_args, false),
					llvm::GlobalValue::ExternalLinkage,
					"put_array", mod);
			stdfunc["put_array"].func = func;
			func_args.clear();
			// create put_array_float function
			func_args.push_back(builder.getInt32Ty()->getPointerTo());
			func = llvm::Function::Create(
					llvm::FunctionType::get(/*ret*/builder.getVoidTy(), func_args, false),
					llvm::GlobalValue::ExternalLinkage,
					"put_array_float", mod);
			stdfunc["put_array_float"].func = func;
			func_args.clear();
			// create put_array_str function
			func_args.push_back(builder.getInt8PtrTy()->getPointerTo());
			func = llvm::Function::Create(
					llvm::FunctionType::get(/*ret*/builder.getVoidTy(), func_args, false),
					llvm::GlobalValue::ExternalLinkage,
					"put_array_str", mod);
			stdfunc["put_array_str"].func = func;
			func_args.clear();
			// create put_num_float function
			func_args.push_back(builder.getDoubleTy());
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
			// create str_to_float function
			func_args.push_back(builder.getInt8PtrTy());
			func = llvm::Function::Create(
					llvm::FunctionType::get(/*ret*/builder.getDoubleTy(), func_args, false),
					llvm::GlobalValue::ExternalLinkage,
					"str_to_float", mod);
			stdfunc["str_to_float"].func = func;
			func_args.clear();
			// create int_to_str function
			func_args.push_back(builder.getInt32Ty());
			func = llvm::Function::Create(
					llvm::FunctionType::get(/*ret*/builder.getInt8Ty()->getPointerTo(), func_args, false),
					llvm::GlobalValue::ExternalLinkage,
					"int_to_str", mod);
			stdfunc["int_to_str"].func = func;
			func_args.clear();
			// create int64_to_str function
			func_args.push_back(builder.getInt64Ty());
			func = llvm::Function::Create(
					llvm::FunctionType::get(/*ret*/builder.getInt8Ty()->getPointerTo(), func_args, false),
					llvm::GlobalValue::ExternalLinkage,
					"int64_to_str", mod);
			stdfunc["int64_to_str"].func = func;
			func_args.clear();
			// create float_to_str function
			func_args.push_back(builder.getDoubleTy());
			func = llvm::Function::Create(
					llvm::FunctionType::get(/*ret*/builder.getInt8Ty()->getPointerTo(), func_args, false),
					llvm::GlobalValue::ExternalLinkage,
					"float_to_str", mod);
			stdfunc["float_to_str"].func = func;
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
			stdfunc["builtinlength"].func = func;
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
			// create GC Function
			func = llvm::Function::Create(
					llvm::FunctionType::get(/*ret*/builder.getVoidTy(), func_args, false),
					llvm::GlobalValue::ExternalLinkage,
					"run_gc", mod);
			stdfunc["GC"].func = func;
		}
		std::string module = "";
		Program list(module);

		Function main;
		std::vector<AST *> main_code, other_code, gvar_code;
		for(ast_vector::iterator it = program.begin(); it != program.end(); ++it) {
			if((*it)->get_type() == AST_FUNCTION) {
				other_code.push_back(*it);
			} else if((*it)->get_type() == AST_LIBRARY) {
				other_code.push_back(*it);
			} else if((*it)->get_type() == AST_MODULE) {
				other_code.push_back(*it);
			} else if((*it)->get_type() == AST_STRUCT) {
				other_code.push_back(*it);
			} else if((*it)->get_type() == AST_VARIABLE_ASGMT) {
				if(((VariableAsgmtAST *)(*it))->var->get_type() == AST_VARIABLE) {
					VariableAST *var = (VariableAST *)((VariableAsgmtAST *)(*it))->var;
					if(var->info.is_global == true) 
						gvar_code.push_back(*it);
					else
						main_code.push_back(*it);
				} else 
					main_code.push_back(*it);
			} else {
				main_code.push_back(*it);
			}
		}
		llvm::Function *func_init_gvar = llvm::Function::Create(
				llvm::FunctionType::get(builder.getVoidTy(), std::vector<llvm::Type *>(), false),
				llvm::Function::ExternalLinkage, "init_gvar", mod);
		{
			Function init_gvar;
			// create entry point of main function
			llvm::BasicBlock *entry = llvm::BasicBlock::Create(context, "entry", func_init_gvar);
			builder.SetInsertPoint(entry);

			for(auto g = gvar_code.begin(); g != gvar_code.end(); ++g) {
				ExprType ty;
				((VariableAsgmtAST *)*g)->codegen(init_gvar, list, &ty);
			}
			builder.CreateRetVoid();
		}
		for(auto code = other_code.begin(); code != other_code.end(); ++code) {
			switch((*code)->get_type()) {
				case AST_FUNCTION:
					((FunctionAST *)*code)->codegen(list);
					break;
				case AST_LIBRARY: 
					((LibraryAST *)*code)->codegen(list);
					break;
				case AST_MODULE:
					((ModuleAST *)*code)->codegen(list);
					break;
				case AST_STRUCT:
					((StructAST *)*code)->codegen(list);
				default:
					error("what happened?");
			}
		}

		// create functions' body
		for(auto fn = funcs_body.begin(); fn != funcs_body.end(); ++fn) {
			list.cur_mod = fn->cur_mod;
			llvm::BasicBlock *entry = llvm::BasicBlock::Create(context, "entry", fn->func);
			builder.SetInsertPoint(entry);

			// set argment variable names and store
			auto arg_types_it = fn->arg_types.begin();
			auto arg_names_it = fn->arg_names.begin();
			for(auto arg_it = fn->func->arg_begin(); arg_it != fn->func->arg_end(); ++arg_it) {
				arg_it->setName(*arg_names_it);
				llvm::AllocaInst *ainst = create_entry_alloca(fn->func, *arg_names_it, (*arg_types_it));
				builder.CreateStore(arg_it, ainst);
				var_t *v = fn->info->var.get(*arg_names_it, "");
				if(v) v->val = ainst;
				arg_types_it++; arg_names_it++;
			}

			for(auto v = fn->info->var.local.begin(); v != fn->info->var.local.end(); v++) {
				if(v->type.is_array() || v->type.eql_type(T_STRING)) {
					std::vector<llvm::Value*> func_args;
					func_args.push_back(builder.CreateBitCast(v->val, v->val->getType()->getPointerTo())); // get address of var
					builder.CreateCall(stdfunc["append_addr_for_gc"].func, func_args);
				}
			}

			llvm::Value *ret_value = llvm::ConstantInt::get(builder.getInt32Ty(), 0); // default return code 0
			for(ast_vector::iterator it = fn->body.begin(); it != fn->body.end(); ++it) { // function body
				ret_value = Codegen::expression(*(fn->info), list, *it);
			}

			for(auto v = fn->info->var.local.begin(); v != fn->info->var.local.end(); v++) {
				if(v->type.is_array() || v->type.eql_type(T_STRING)) {
					std::vector<llvm::Value*> func_args;
					func_args.push_back(builder.CreateBitCast(v->val, v->val->getType()->getPointerTo())); // get address of var
					builder.CreateCall(stdfunc["delete_addr_for_gc"].func, func_args);
				}
			}

			if(ret_value) {
				if(ret_value->getType()->getTypeID() != fn->ret_type->getTypeID()) {
					ret_value = llvm::ConstantInt::get(builder.getInt32Ty(), 0);
					// fprintf(stderr, "warning: type of expression that evaluated last is not match\n");
				}
			} else if(fn->ret_type->getTypeID() == builder.getInt32Ty()->getTypeID()) {

				ret_value = llvm::ConstantInt::get(builder.getInt32Ty(), 0);
			} else error("error: return code of function is incorrect '%s'", fn->info->info.name.c_str());

			builder.CreateRet(ret_value);
		}
		list.append(main);

		// create main function
		llvm::Function *func_main = llvm::Function::Create(
				llvm::FunctionType::get(builder.getInt32Ty(), std::vector<llvm::Type *>(), false),
				llvm::Function::ExternalLinkage, "main", mod);
		// create entry point of main function
		llvm::BasicBlock *entry = llvm::BasicBlock::Create(context, "entry", func_main);
		builder.SetInsertPoint(entry);

		builder.CreateCall(func_init_gvar);

		for(std::vector<AST *>::iterator it = main_code.begin(); it != main_code.end(); ++it) {
			expression(main, list, *it);
		}

		builder.CreateRet(llvm::ConstantInt::get(builder.getInt32Ty(), 0));
		// llvm::verifyModule(*mod);
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
			/// Provide basic AliasAnalysis support for GVN.
			pass_mgr.add(llvm::createBasicAliasAnalysisPass());
			// Do simple "peephole" optimizations and bit-twiddling optzns.
			pass_mgr.add(llvm::createInstructionCombiningPass());
			//
			pass_mgr.add(llvm::createReassociatePass());
			//
			pass_mgr.add(llvm::createCFGSimplificationPass());
			// Reassociate expressions.
			pass_mgr.add(llvm::createReassociatePass());
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
			case AST_DOT:
				return ((DotOpAST *)ast)->codegen(f, f_list, ty);
			case AST_CAST:
				return ((CastAST *)ast)->codegen(f, f_list, ty);
		}
		return NULL;
	}
};

void ModuleAST::codegen(Program &f_list) {
	f_list.cur_mod.push_back(name);
	for(auto &stmt : statement) {
		if(stmt->get_type() == AST_FUNCTION) {
			((FunctionAST *)stmt)->codegen(f_list);
		} else if(stmt->get_type() == AST_MODULE) {
			((ModuleAST *)stmt)->codegen(f_list);
		}
	}
	f_list.cur_mod.pop_back();
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

Function FunctionAST::codegen(Program &f_list) { // create a prototype of function, its body will create in Codegen
	Function f;
	f.info.name = info.name;
	f.info.mod_name = f_list.cur_mod;
	f.info.params = args.size(); 	
	f.info.func_addr = NULL;
	f.info.type.change(new ExprType(info.type));

	// append arguments 
	std::vector<llvm::Type *> arg_types;
	std::vector<std::string> arg_names;
	std::vector<ExprType *> args_type_for_overload;
	for(ast_vector::iterator it = args.begin(); it != args.end(); ++it) {
		if((*it)->get_type() == AST_VARIABLE) {
			var_t *v = ((VariableAST *)*it)->append(f, f_list);
			llvm::Type *llvm_type = builder.getInt32Ty();
			ExprType *type = new ExprType(T_INT);
			if(v->type.is_ref()) {
				llvm_type = llvm_type->getPointerTo();
				type->set_ref();
			}
			arg_types.push_back(llvm_type);
			arg_names.push_back(v->name);
			args_type_for_overload.push_back(type);
		} else if((*it)->get_type() == AST_VARIABLE_DECL) {
			var_t *v = ((VariableDeclAST *)*it)->append(f, f_list);
			llvm::Type *llvm_type = Type::type_to_llvmty(&v->type);
			ExprType *type = new ExprType(v->type);
			if(v->type.is_ref()) {
				llvm_type = llvm_type->getPointerTo();
				type->set_ref();
			}
			arg_types.push_back(llvm_type);
			arg_names.push_back(v->name);
			args_type_for_overload.push_back(type);
		}
	}
	f.info.args_type = args_type_for_overload;

	Function *function = f_list.append(f);

	// definition the Function
	
	// set function return type
	llvm::Type *func_ret_type = Type::type_to_llvmty(&info.type);

	llvm::FunctionType *func_type = llvm::FunctionType::get(func_ret_type, arg_types, false);
	llvm::Function *func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, f.info.name, mod);

	funcs_body.push_back(func_body_t {
		.info = function,
			.arg_names = arg_names,
			.arg_types = arg_types,
			.body = statement,
			.func = func,
			.cur_mod = f_list.cur_mod,
			.ret_type = func_ret_type,
	});

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
				} else if(ty.is_array() && ty.next->eql_type(T_INT)) {
					builder.CreateCall(stdfunc["put_array"].func, func_args)->setCallingConv(llvm::CallingConv::C);
				} else if(ty.is_array() && ty.next->eql_type(T_DOUBLE)) {
					builder.CreateCall(stdfunc["put_array_float"].func, func_args)->setCallingConv(llvm::CallingConv::C);
				} else if(ty.is_array() && ty.next->eql_type(T_STRING)) {
					builder.CreateCall(stdfunc["put_array_str"].func, func_args)->setCallingConv(llvm::CallingConv::C);
				} else if(ty.eql_type(T_INT64)) {
					builder.CreateCall(stdfunc["put_num64"].func, func_args)->setCallingConv(llvm::CallingConv::C);
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
	Function *function = f_list.get(info.name, info.mod_name, args_type);
	if(!function) function = f_list.get(info.name, f_list.cur_mod, args_type);
	if(!function) { // not found
		std::string args_type_str; 
			// args to string
			for(auto arg : args_type) 
				args_type_str += arg->to_string() + ", ";
			args_type_str.erase(args_type_str.end() - 2, args_type_str.end()); // erase ", "
			error("error: undefined function: '%s(%s)'", info.name.c_str(), args_type_str.c_str());
	}
	auto func_args = function->info.args_type;
	for(auto arg = args.begin(); arg != args.end(); ++arg) { // reference?
		int count = arg - args.begin();
		if(func_args[count]->is_ref()) {
			VariableAST *vast = (VariableAST *)*arg;
			var_t *v = vast->get(f, f_list);
			callee_args[count] = (v->val);
		}
	}
	llvm::Function *callee = (function->info.func_addr) ? function->info.func_addr : mod->getFunction(info.name);
	if(!callee) error("no function: %s", info.name.c_str());
	if(function == NULL) { // undefined
		uint32_t a = 3;
		for(ast_vector::iterator it = args.begin(); it != args.end(); ++it) {
			Codegen::expression(f, f_list, *it);
		}
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

	{ // cast instructions
		if(ty_l.eql_type(T_INT) && ty_r.eql_type(T_DOUBLE)) {
			lhs = builder.CreateSIToFP(lhs, builder.getDoubleTy());
			ty->change(new ExprType(T_DOUBLE));
			ty_l = T_DOUBLE;
		} else if(ty_l.eql_type(T_DOUBLE) && ty_r.eql_type(T_INT)) {
			rhs = builder.CreateSIToFP(rhs, builder.getDoubleTy());
		} else if(ty_l.eql_type(T_INT64) && ty_r.eql_type(T_INT)) {
			rhs = builder.CreateSExt(rhs, builder.getInt64Ty());
		} else if(ty_l.eql_type(T_INT) && ty_r.eql_type(T_INT64)) {
			lhs = builder.CreateSExt(lhs, builder.getInt64Ty());
		} else if(ty_l.eql_type(T_INT) && !ty_r.eql_type(T_INT)) {
			rhs = builder.CreateZExt(rhs, builder.getInt32Ty());
		}
		// char is the same as int 
		if(ty_l.eql_type(T_CHAR))
			lhs = builder.CreateZExt(lhs, builder.getInt32Ty());
		if(ty_r.eql_type(T_CHAR))
			rhs = builder.CreateZExt(rhs, builder.getInt32Ty());
	}

	// if(!ty_l.eql_type(&ty_r)) {
	// 	std::cout << "warning: different types to each other: " << ty_l.to_string() << ", " << ty_r.to_string() << std::endl;
	// }
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
			if(ty_l.eql_type(T_DOUBLE))
				lhs = builder.CreateFCmpULT(lhs, rhs, tmp_name);
			else
				lhs = builder.CreateICmpSLT(lhs, rhs, tmp_name);
		} else if(op == ">") {
			if(ty_l.eql_type(T_DOUBLE))
				lhs = builder.CreateFCmpUGT(lhs, rhs, tmp_name);
			else
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
		ty->change(T_INT);	
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

llvm::Value *CastAST::codegen(Function &f, Program &f_list, ExprType *ret_ty) {
	ret_ty->change(Type::str_to_type(type));
	llvm::Type *to_type = Type::type_to_llvmty(ret_ty);
	ExprType exp_ty;
	llvm::Value *exp = Codegen::expression(f, f_list, expr, &exp_ty);
	if(exp_ty.eql_type(T_INT) && ret_ty->eql_type(T_DOUBLE)) {
		return builder.CreateSIToFP(exp, builder.getDoubleTy());
	} else if(exp_ty.eql_type(T_DOUBLE) && !ret_ty->eql_type(T_DOUBLE)) {
		exp = builder.CreateFPToSI(exp, builder.getInt32Ty());
	} else if(ret_ty->eql_type(T_INT64)) {
		return builder.CreateSExt(exp, builder.getInt64Ty());
	}
	return builder.CreateBitCast(exp, to_type);
}

llvm::Value * NewAllocAST::codegen(Function &f, Program &f_list, ExprType *ty) {
	ExprType *alloc_type = Type::str_to_type(type);
	bool is_user_object = size == NULL;
	std::vector<llvm::Value*> func_args;
	func_args.push_back(
			is_user_object ? 
				llvm::ConstantInt::get(builder.getInt32Ty(), 1) : 
				Codegen::expression(f, f_list, size));
	func_args.push_back(llvm::ConstantInt::get(builder.getInt32Ty(), 
				alloc_type->eql_type(T_USER_TYPE) && !is_user_object? 
					f_list.structs.get_size(alloc_type->get().user_type) : 
					sizeof(void*)));
	llvm::Value *ret = builder.CreateCall(stdfunc["create_array"].func, func_args);
	if(is_user_object)
		ty->change(alloc_type);
	else
		ty->change(T_ARRAY, alloc_type);
	
	llvm::Type *ret_val;
	if(is_user_object) {
		ret_val = (llvm::PointerType *)f_list.structs.get(alloc_type->get().user_type)
					->strct->getPointerTo();
	} else if(alloc_type->eql_type(T_USER_TYPE)) {
		ret_val = (llvm::PointerType *)f_list.structs.get(alloc_type->get().user_type)
			->strct->getPointerTo()->getPointerTo();
	} else {
		llvm::Type *ltype = Type::type_to_llvmty(alloc_type);
		ret_val = ltype->getPointerTo();
	}
	return builder.CreateBitCast(ret, ret_val, "bitcast_tmp");
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
	} else if(var->get_type() == AST_DOT) {
		DotOpAST *dot = (DotOpAST *)var;
		ExprType ty;
		llvm::Value *parent = NULL;
		AST *obj = dot->var;
		if(obj->get_type() == AST_VARIABLE_INDEX) {
			VariableIndexAST *viast = (VariableIndexAST *)obj;
			parent = viast->get_elem(f, f_list, &ty);
		} else {
			parent = Codegen::expression(f, f_list, obj, &ty);
		}
		struct_t *strct = f_list.structs.get(ty.get().user_type);
		if(!strct) error("err_index");
		int a = 0;
		if(dot->member->get_type() != AST_VARIABLE) puts("NG");
		for(auto it = strct->members.begin(); it != strct->members.end(); ++it){ 
			if(it->name == ((VariableAST *)dot->member)->info.name)
				break;
			a++;
		}
		llvm::Value *memb = builder.CreateConstGEP2_32(parent, 0, a, "gep");
		llvm::Value *val = Codegen::expression(f, f_list, src);
		return builder.CreateStore(val, memb);
	} else if(var->get_type() == AST_VARIABLE_INDEX) {
		VariableIndexAST *vidx = (VariableIndexAST *)var;
		ExprType expr_ty;
		llvm::Value *elem = llvm::GetElementPtrInst::CreateInBounds(
				Codegen::expression(f, f_list, vidx->var, &expr_ty),
				llvm::ArrayRef<llvm::Value *>(Codegen::expression(f, f_list, vidx->idx)), "elem_tmp", builder.GetInsertBlock());
		llvm::Value *val = Codegen::expression(f, f_list, src);
		if(expr_ty.eql_type(T_STRING))
			val = builder.CreateZExt(val, builder.getInt8Ty());
		return builder.CreateStore(val, elem);
	}

	// single assignment
	ExprType v_ty;
	llvm::Value *val = Codegen::expression(f, f_list, src, &v_ty);
	ty->change(new ExprType(v_ty));
	if(v->is_global) {
		if(first_decl) {
			v->type = v_ty;
			mod->getOrInsertGlobal(v->name, Type::type_to_llvmty(&v_ty));
			llvm::GlobalVariable *gbl = mod->getNamedGlobal(v->name);
			// gbl->setLinkage(llvm::GlobalValue::CommonLinkage);
			gbl->setAlignment(4);
			gbl->setInitializer(llvm::ConstantInt::get(builder.getInt32Ty(), 0));
			v->val = gbl;
		}
		builder.CreateStore(val, v->val);
	} else {
		if(var->get_type() == AST_VARIABLE) {
			bool ref = v->type.is_ref();
			v->type = v_ty;
			v->type.set_ref(ref);
		}
		if(first_decl) {
			llvm::AllocaInst *ai;
			ai = builder.CreateAlloca(Type::type_to_llvmty(&v_ty));
			v->val = ai;
		}
		if(v->type.is_array() || v->type.eql_type(T_STRING)) {
			std::vector<llvm::Value*> func_args;
			func_args.push_back(builder.CreateBitCast(v->val, v->val->getType()->getPointerTo())); // addr
			builder.CreateCall(stdfunc["append_addr_for_gc"].func, func_args);
		}
		if(v->type.is_ref()) {
			builder.CreateStore(val, builder.CreateLoad(v->val));
			return builder.CreateLoad(builder.CreateLoad(v->val));
		} else {
			builder.CreateStore(val, v->val);
			return builder.CreateLoad(v->val);
		}
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
	llvm::Value *ret = get_elem(f, f_list, ret_ty);
	ret = builder.CreateLoad(ret);
	return ret;
}

llvm::Value *VariableIndexAST::get_elem(Function &f, Program &f_list, ExprType *ret_ty) {
	ExprType ty;
	llvm::Value *ret = llvm::GetElementPtrInst::CreateInBounds(
			Codegen::expression(f, f_list, var, &ty), 
			llvm::ArrayRef<llvm::Value *>(Codegen::expression(f, f_list, idx)), "elem_tmp", builder.GetInsertBlock());
	if(ty.eql_type(T_STRING)) {
		ret_ty->change(T_CHAR);
	} else if(ty.next->eql_type(T_USER_TYPE)) {
		ret_ty->change(ty.next->get().user_type);
	} else {
		ret_ty->change(ty.next);
	}
	return ret;
}

llvm::Value * VariableAST::codegen(Function &f, Program &f_list, ExprType *ty) {
	var_t *v;
	if(info.is_global) {
		v = f_list.var_global.get(info.name, info.mod_name);
		if(v == NULL) error("error: undefined global variable '%s'", info.name.c_str());
	} else 
		v = f.var.get(info.name, info.mod_name);
	if(v == NULL) error("error: '%s' was not declared", info.name.c_str());
	if(info.is_global == false) { // local
		ty->change(&v->type);
		if(ty->is_ref()) 
			return builder.CreateLoad(builder.CreateLoad(v->val), "var_tmp");
		else
			return builder.CreateLoad(v->val, "var_tmp");
		return builder.CreateLoad(v->val, "var_tmp");
	} else { // global
		ty->change(&v->type);
		return builder.CreateLoad(v->val, "var_tmp");
	}
}
var_t *VariableAST::get(Function &f, Program &f_list) {
	if(info.is_global) return f_list.var_global.get(info.name, info.mod_name);
	return f.var.get(info.name, info.mod_name);
}
var_t *VariableAST::append(Function &f, Program &f_list) {
	if(info.is_global) return f_list.append_global_var(info.name, info.type.get().type);
	return f.var.append(info.name, new ExprType(&info.type));
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

llvm::Value *StructAST::codegen(Program &f_list) {
	std::vector<var_t> members;
	std::vector<llvm::Type *> field;
	for(auto it = var_decls.begin(); it != var_decls.end(); it++) {
		AST *member = *it;
		if(member->get_type() != AST_VARIABLE &&
				member->get_type() != AST_VARIABLE_DECL) {
			error("error: syntax error on struct");
		} else {
			members.push_back(
				member->get_type() == AST_VARIABLE ? 
					((VariableAST *)member)->info : 
					((VariableDeclAST *)member)->info);
			ExprType member_ty = 
				member->get_type() == AST_VARIABLE ? 
					((VariableAST *)member)->info.type : 
					((VariableDeclAST *)member)->info.type;
			auto create_array_ty = [&]() -> llvm::Type * {
				int count = 1;
				llvm::Type *ty;
				ExprType *next_ = member_ty.next;
				while(next_ && next_->is_array()) {
					next_ = next_->next;	
					count++;
				}
				ty =
					next_->eql_type(T_STRING) ? 
						(llvm::Type *)builder.getInt8PtrTy() : 
						(llvm::Type *)builder.getInt32Ty();
				while(count--) ty = ty->getPointerTo();
				return ty;
			};
			field.push_back(
					member_ty.eql_type(T_STRING) ? 
						builder.getInt8PtrTy() : 
						member_ty.is_array() ? 
							create_array_ty() : 
							builder.getInt32Ty());
		}
	}
	llvm::StructType *mystruct = llvm::StructType::create(context, field, "struct."+name);

	f_list.structs.append(name, members, mystruct);
	return NULL;
}

llvm::Value *DotOpAST::codegen(Function &f, Program &f_list, ExprType *ret_ty) {
	ExprType ty;
	llvm::Value *parent = NULL; 
	if(var->get_type() == AST_VARIABLE_INDEX) {
		VariableIndexAST *viast = (VariableIndexAST *)var;
		parent = viast->get_elem(f, f_list, &ty);
	} else parent = Codegen::expression(f, f_list, var, &ty);

	struct_t *strct = f_list.structs.get(ty.get().user_type);
	if(!strct) error("error in DotOpAST");
	int a = 0;
	ExprType member_ty;
	if(member->get_type() != AST_VARIABLE) puts("error in DotOpAST");
	for(auto it = strct->members.begin(); it != strct->members.end(); ++it){ 
		if(it->name == ((VariableAST *)member)->info.name) {
			member_ty.change(new ExprType(it->type));
			break;
		}
		a++;
	}
	llvm::Value *ret = builder.CreateStructGEP(parent, a);
	ret = builder.CreateLoad(ret, "load_tmp");
	ret_ty->change(new ExprType(member_ty));
	return ret;
}

llvm::Value * ArrayAST::codegen(Function &f, Program &f_list, ExprType *ret_ty) {
	std::vector<llvm::Value*> func_args;
	bool zero_ary = type != NULL;
	func_args.push_back(llvm::ConstantInt::get(builder.getInt32Ty(), zero_ary ? 0 : elems.size()));
	func_args.push_back(llvm::ConstantInt::get(builder.getInt32Ty(), sizeof(void*)));
	llvm::Value *ary = builder.CreateCall(stdfunc["create_array"].func, func_args);
	ExprType ty;
	if(zero_ary) {
		ary = builder.CreateBitCast(ary, Type::type_to_llvmty(type)->getPointerTo(), "bitcast_tmp");
		ty = *type;
	} else {
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
	}
	ret_ty->change(T_ARRAY, new ExprType(ty));
	return ary;
}

llvm::Value *StringAST::codegen(Function &f, ExprType *ty) {
	ty->change(T_STRING);
	char *embed = (char *)LitMemory::alloc_const(str.length() * 2); // TODO: fix!
	embed = strcpy(embed, str.c_str());
	embed = replace_escape(embed); // TODO: fix implementation
	return builder.CreateGlobalStringPtr(embed);
}

llvm::Value * CharAST::codegen(Function &f, ExprType *ty) {
	ty->change(T_CHAR);
	return llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), ch);
}

llvm::Value * NumberAST::codegen(Function &f, ExprType *ty) {
	long long int n = std::atoll(number.c_str());
	if(n > INT_MAX) {
		ty->change(T_INT64);
		return llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), n, true);
	} else {
		ty->change(T_INT);
		return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), (int)n, true);
	}
}
llvm::Value * FloatNumberAST::codegen(Function &f, ExprType *ty) {
	ty->change(T_DOUBLE);
	return llvm::ConstantFP::get(builder.getDoubleTy(), number);
}

