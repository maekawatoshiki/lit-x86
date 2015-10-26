#ifndef _EXPR_LIT_
#define _EXPR_LIT_

#include "lit.h"
#include "asm.h"
#include "lex.h"
#include "parse.h"
#include "stdfunc.h"

extern char *module;

int relExpr();
int condExpr();
int addSubExpr();
int mulDivExpr();
int primExpr();

int isIndex();
int make_index();

#endif
