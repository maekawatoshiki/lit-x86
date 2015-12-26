#ifndef _UTIL_LIT_
#define _UTIL_LIT_

#include "lit.h"
#include "asm.h"
#include "lex.h"
#include "expr.h"
#include "parse.h"
#include "var.h"

int streql(char *, char *);
bool skip(std::string);
int error(char *, ...);

#endif // _UTIL_LIT_
