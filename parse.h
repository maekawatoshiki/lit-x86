#ifndef _PARSE_LIT_
#define _PARSE_LIT_

#include "common.h"
#include "var.h"
#include "token.h"
#include "func.h"
#include "var.h"
#include "library.h"
#include "ast.h"
#include "exprtype.h"

class Parser {
public:
	Token &tok;
	LibraryList lib_list;
	FunctionList undef_funcs, funcs;
	Variable var;
	int blocksCount;
	std::string module;

	Parser(Token &token)
		:tok(token), undef_funcs(module), funcs(module) { }

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

	AST *expr_entry();
	AST *expr_asgmt();
	AST *expr_compare();
	AST *expr_logic();
	AST *expr_add_sub();
	AST *expr_mul_div();
	AST *expr_index();
	AST *expr_postfix();
	AST *expr_primary();
	AST *expr_array();

// parse.h
	void make_require();
	AST *make_if();
	AST *make_while();
	AST *make_for();
	AST *make_func();
	int make_break();
	AST *make_return();

	ast_vector eval();
	AST *expression();

	int parser();
	int get_string();

// stdfunc.h
	int make_stdfunc(std::string , std::string ); // func name, module name
	int is_stdfunc(std::string , std::string );
};

char *replace_escape(char *);

#endif
