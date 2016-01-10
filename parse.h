#ifndef _PARSE_LIT_
#define _PARSE_LIT_

#include "common.h"
#include "var.h"
#include "token.h"
#include "func.h"
#include "var.h"
#include "library.h"

// The strings embedded in native code
struct embed_string_t {
	std::string text;
	int addr;
};

class EmbedString {
public:
	std::vector<embed_string_t> text;
	int count;
	
	EmbedString()
		:count(0) { }
};

class Parser {
public:
	Token &tok;
	EmbedString embed_str;
	LibraryList lib_list;
	FunctionList undef_funcs, funcs;
	Variable var;
	int blocksCount;
	std::string module;

	Parser(Token &token)
		:tok(token), var(funcs, module), undef_funcs(module), funcs(module) { }

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
	int make_index(var_t *);

	int make_array();

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
