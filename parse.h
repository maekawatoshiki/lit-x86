#ifndef _PARSE_LIT_
#define _PARSE_LIT_

#include "common.h"
#include "var.h"
#include "token.h"

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

class Parser {
public:
	Token &tok;

	Parser(Token &token)
		:tok(token) { }

// var.h
	var_t *declare_var();

	int is_asgmt();
	int asgmt();
	int asgmt_single(var_t *);
	int asgmt_array(var_t *);

// expr.h
	int is_string_tok();
	int is_number_tok();
	int is_ident_tok();
	int is_char_tok();

	int expr_entry();
	int expr_compare();
	int expr_logic();
	int expr_add_sub();
	int expr_mul_div();
	int expr_primary();

	int is_index();
	int make_index();

// parse.h
	void make_require();
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

// stdfunc.h
	int make_stdfunc(std::string , std::string ); // func name, module name
	int is_stdfunc(std::string , std::string );
};

#endif
