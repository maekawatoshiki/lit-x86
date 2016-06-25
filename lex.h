#ifndef _LEXER_LIT_
#define _LEXER_LIT_

#include "token.h"

#define SKIP_TOK tok.pos++;

enum {
	TOK_SYMBOL,
	TOK_IDENT,
	TOK_NUMBER,
	TOK_STRING,
	TOK_CHAR,
	TOK_END
};

class Lexer {
public:
	Token &tok;
	std::map<std::string, bool> required_files;

	Lexer(Token &token)
		:tok(token) { }

	int lex(char *);
};

#endif
