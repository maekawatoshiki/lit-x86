#ifndef _EXPR_LIT_
#define _EXPR_LIT_

#include "lit.h"
#include "asm.h"
#include "parse.h"

int addSubExpr();
int mulDivExpr();
int relExpr();
int primExpr();

int isArray();
int genArray();


#endif
