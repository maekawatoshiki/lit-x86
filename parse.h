#ifndef _PARSE_LIT_
#define _PARSE_LIT_

#include "common.h"
#include "var.h"
#include "token.h"
#include "func.h"
#include "var.h"
#include "library.h"

struct expr_type_t {
	expr_type_t():type(T_INT) {};
	int type;
	std::string user_type;
};

class ExprType {
public:
	expr_type_t type;

	expr_type_t &get();
	bool change(int);
	bool change(std::string);
};

class Parser {
public:
	Token &tok;
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

	ExprType expr_entry();
	int expr_compare(ExprType &);
	int expr_logic(ExprType &);
	int expr_add_sub(ExprType &);
	int expr_mul_div(ExprType &);
	int expr_primary(ExprType &);

	int is_index();
	int make_index(ExprType &);

	int make_array(ExprType &);

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
