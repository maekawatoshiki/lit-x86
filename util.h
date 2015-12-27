#ifndef _UTIL_LIT_
#define _UTIL_LIT_

#include "lit.h"
#include "asm.h"
#include "lex.h"
#include "expr.h"
#include "parse.h"
#include "var.h"

int streql(char *, char *);
int error(const char *, ...);

#endif // _UTIL_LIT_
