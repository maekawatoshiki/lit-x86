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
};

class FunctionList {
public:
	std::vector<func_t> func;
	int now;
	bool inside;
	std::string &module;
	
	FunctionList(std::string &mod): module(mod), now(0), inside(false) { func.reserve(128); }
	bool is(std::string, std::string);
	func_t *focus();
	func_t *get(std::string, std::string);
	func_t *append(std::string, int, int);
	func_t *append_undef(std::string, std::string, int);
	bool rep_undef(std::string, int);	
};


#endif // _FUNC_LIT_
