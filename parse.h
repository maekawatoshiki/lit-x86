#ifndef _PARSE_LIT_
#define _PARSE_LIT_

#include "lit.h"
#include "asm.h"
#include "lex.h"
#include "expr.h"
#include "token.h"
#include "util.h"
#include "library.h"
#include "var.h"

typedef struct {
	uint32_t address, params;
	std::string name, mod_name;
} func_t;

class FunctionList {
public:
	std::vector<func_t> func;
	int now;
	bool inside;
	
	FunctionList() { func.reserve(128); }
	bool is(std::string, std::string);
	func_t *focus();
	func_t *get(std::string, std::string);
	func_t *append(std::string, int, int);
	func_t *append_undef(std::string, std::string, int);
	bool rep_undef(std::string, int);	
};

extern FunctionList undef_funcs, funcs;

// The strings embedded in native code
typedef struct {
	char *text[0xff];
	int *addr;
	int count;
} string_t;

extern string_t strings;
extern std::string module;

void using_require();

int make_if();
int make_while();
int make_func();
int make_break();
int make_return();

int eval(int, int);
int expression(int, int);

int parser();
int get_string();

void replaceEscape(char *);

#endif
