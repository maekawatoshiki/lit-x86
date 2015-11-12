#ifndef _LEXER_LIT_
#define _LEXER_LIT_

#include "lit.h"
#include "asm.h"
#include "expr.h"
#include "parse.h"
#include "stdfunc.h"

enum {
	TOK_SYMBOL,
	TOK_IDENT,
	TOK_NUMBER,
	TOK_STRING,
	TOK_END
};

int lex(char *);

#endif
