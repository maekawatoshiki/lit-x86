#ifndef _EXPR_LIT_
#define _EXPR_LIT_

#include "lit.h"
#include "asm.h"
#include "parse.h"

int relExpr();
int condExpr();
int addSubExpr();
int mulDivExpr();
int primExpr();

int isArray();
int genArray();


#endif