#ifndef _STD_FUNC_LIT_
#define _STD_FUNC_LIT_

#include "lit.h"
#include "asm.h"
#include "lex.h"
#include "expr.h"
#include "parse.h"
#include "util.h"

typedef struct {
	std::string name, mod_name;
	int args, addr;
} std_function;

int make_stdfunc(std::string , std::string ); // func name, module name
int is_stdfunc(std::string , std::string );

#endif
