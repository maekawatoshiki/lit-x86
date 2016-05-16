#ifndef _FUNC_LIT_
#define _FUNC_LIT_

#include "common.h"
#include "var.h"
#include "asm.h"

struct func_t {
	uint32_t address, params;
	int type;
	bool is_lib;
	std::string name, mod_name;
};

class Function {
public:
	func_t info;
	Variable var;
	llvm::BasicBlock *bb_return;
	std::stack<llvm::BasicBlock *> break_br_list;
	uint32_t call(NativeCode_x86 &);
};

class Program {
public:
	std::string name;
	std::vector<Function> func, undef_func;
	Variable var_global; 
	std::string &module;
	
	Program(std::string &mod): module(mod) { func.reserve(128); }
	bool is(std::string, std::string);
	Function *get(std::string, std::string = "");
	Function *append(Function);
	
	Function *append_undef(std::string, std::string, int);
	bool rep_undef(std::string, int);	

	var_t *append_global_var(std::string, int);
	void append_addr_of_global_var(std::string, int);
	void insert_global_var();
};


#endif // _FUNC_LIT_
