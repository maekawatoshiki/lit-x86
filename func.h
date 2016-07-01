#ifndef _FUNC_LIT_
#define _FUNC_LIT_

#include "var.h"
#include "exprtype.h"

struct func_t {
	uint32_t address, params;
	ExprType type;
	std::vector<ExprType *> args_type;
	std::string name;
	std::vector<std::string> mod_name;
	llvm::Function *func_addr;
};

class Function {
public:
	func_t info;
	Variable var;
	llvm::BasicBlock *bb_return;
	std::stack<llvm::BasicBlock *> break_br_list;
	std::stack<bool> has_br;
};

class Program {
public:
	std::string name;
	std::vector<Function> func, undef_func;
	std::vector<std::string> cur_mod;
	Variable var_global; 
	Struct structs;
	std::string &module;
	
	Program(std::string &mod): module(mod) { func.reserve(128); }
	bool is(std::string, std::string);
	Function *get(std::string, std::vector<ExprType *>, std::string = "");
	Function *get(std::string, std::vector<std::string>, std::vector<ExprType*>);
	Function *append(Function);
	
	Function *append_undef(std::string, std::string, int);
	bool rep_undef(std::string, int);	

	var_t *append_global_var(std::string, int);
	void append_addr_of_global_var(std::string, int);
	void insert_global_var();
};


#endif // _FUNC_LIT_
