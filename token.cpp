#include "token.h"
#include "lit.h"
#include "asm.h"
#include "lex.h"
#include "expr.h"
#include "parse.h"
#include "stdfunc.h"
#include "util.h"

bool Token::skip(std::string s) {
	if(s == tok[pos].val) {
		pos++; return true;
	}
	return false;
}
bool Token::is(std::string s) { return s == tok[pos].val; }
bool Token::is(std::string s, int n) { return s == tok[pos + n].val; }
bool Token::skip() { pos++; return true; }
bool Token::prev() { pos--; return true; }

token_t Token::get() { return tok[pos]; }
token_t Token::next() { return tok[pos++]; }
token_t Token::at(int n) { return tok[n]; }
