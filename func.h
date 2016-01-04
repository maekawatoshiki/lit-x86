#ifndef _FUNC_LIT_
#define _FUNC_LIT_

#include "common.h"

typedef struct {
	uint32_t address, params;
	std::string name, mod_name;
} func_t;

class FunctionList {
public:
	std::vector<func_t> func;
	int now;
	bool inside;
	
	FunctionList() { func.reserve(128); }
	bool is(std::string, std::string);
	func_t *focus();
	func_t *get(std::string, std::string);
	func_t *append(std::string, int, int);
	func_t *append_undef(std::string, std::string, int);
	bool rep_undef(std::string, int);	
};

extern FunctionList undef_funcs, funcs;

#endif // _FUNC_LIT_
