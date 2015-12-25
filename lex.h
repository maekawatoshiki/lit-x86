#ifndef _LEXER_LIT_
#define _LEXER_LIT_

#include "lit.h"
#include "asm.h"
#include "expr.h"
#include "parse.h"
#include "stdfunc.h"
#include "util.h"

#define SKIP_TOK tok.pos++;

enum {
	TOK_SYMBOL,
	TOK_IDENT,
	TOK_NUMBER,
	TOK_STRING,
	TOK_CHAR,
	TOK_END
};

int lex(char *);

#endif
