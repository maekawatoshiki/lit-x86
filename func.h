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
	Variable var;
	func_t info;
	std::vector<int> break_list, return_list;
};

class Module {
public:
	std::vector<Function> func;
	Variable var_global;
	int now;
	bool inside;
	std::string &module;
	
	Module(std::string &mod): module(mod), now(0), inside(false) { func.reserve(128); }
	bool is(std::string, std::string);
	Function *get(std::string, std::string);
	Function *append(Function);
	Function *append_undef(std::string, std::string, int);
	bool rep_undef(std::string, int);	
};


#endif // _FUNC_LIT_
