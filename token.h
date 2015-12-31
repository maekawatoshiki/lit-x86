#ifndef _TOKEN_LIT_
#define _TOKEN_LIT_

#include "common.h"

typedef struct {
	std::string val;
	int type;
	int nline;
} token_t;

class Token {
public:
	std::vector<token_t> tok;
	int size, pos;
	
	Token() { tok.reserve(0xffff); }
	bool skip(std::string);
	bool skip();
	bool is(std::string);
	bool is(std::string, int);
	bool prev();

	token_t get();
	token_t next();
	token_t at(int);
};

#endif // _TOKEN_LIT_
