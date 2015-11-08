#ifndef _EXPR_LIT_
#define _EXPR_LIT_

#include "lit.h"
#include "asm.h"
#include "lex.h"
#include "parse.h"
#include "stdfunc.h"

int expr_entry();
int expr_compare();
int expr_logic();
int expr_add_sub();
int expr_mul_div();
int expr_primary();

int isIndex();
int make_index();

#endif
