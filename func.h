#ifndef _FUNC_LIT_
#define _FUNC_LIT_

#include "common.h"
#include "var.h"

typedef struct {
	uint32_t address, params;
	std::string name, mod_name;
} func_t;

class Function {
public:
	func_t info;
	Variable var;
	std::vector<int> return_list;
	std::stack< std::vector<int> * > break_list;
};

class Module {
public:
	std::string name;
	std::vector<Function> func, undef_func;
	Variable var_global;
	std::string &module;
	
	Module(std::string &mod): module(mod) { func.reserve(128); }
	bool is(std::string, std::string);
	Function *get(std::string, std::string);
	Function *append(Function);
	Function *append_undef(std::string, std::string, int);
	bool rep_undef(std::string, int);	
};


#endif // _FUNC_LIT_
