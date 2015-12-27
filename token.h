#ifndef _TOKEN_LIT_
#define _TOKEN_LIT_

#include "lit.h"
#include "asm.h"
#include "lex.h"
#include "expr.h"
#include "parse.h"
#include "stdfunc.h"
#include "util.h"

typedef struct {
	std::string val;
	int type;
	int nline;
} token_t;

class Token {
public:
	std::vector<token_t> tok;
	int size, pos;

	bool skip(std::string);
	bool skip();
	bool is(std::string);
	bool is(std::string, int);
	bool prev();

	token_t get();
	token_t next();
	token_t at(int);
};

extern Token tok;

#endif // _TOKEN_LIT_
