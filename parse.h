#ifndef _PARSE_LIT_
#define _PARSE_LIT_

#include "common.h"
#include "var.h"
#include "token.h"
#include "func.h"
#include "var.h"
#include "ast.h"
#include "exprtype.h"

enum {
  BLOCK_NORMAL = 0,
  BLOCK_LOOP,
  BLOCK_FUNC,
  BLOCK_GLOBAL,
  NON
};

class Parser {
public:
  Token &tok;
  int blocksCount;
  std::vector<std::string> module;

  Parser(Token &token)
    :tok(token) { }

// expr.h
  int is_string_tok();
  int is_number_tok();
  int is_ident_tok();
  int is_char_tok();

  std::map<std::string, int> op_prec;
  int get_op_prec(std::string);
  AST *expr_entry();
  AST *expr_index();
  AST *expr_dot();
  AST *expr_unary();
  AST *expr_primary();
  AST *expr_array();
  AST *expr_rhs(int, AST *);

// parse.h
  AST *make_lib();
  AST *make_proto();
  AST *make_module();
  AST *make_if();
  AST *make_elsif();
  AST *make_while();
  AST *make_for();
  AST *make_func();
  AST *make_break();
  AST *make_return();
  AST *make_struct();

  ast_vector eval();
  AST *expression();

  llvm::Module *parser();
  int get_string();

  std::map<std::vector<std::string>, bool> function_list;
  bool is_func(std::vector<std::string>);
  void append_func(std::vector<std::string>);
  std::map<std::string, bool> local_var_list;
  bool is_local_var(std::string);
  void append_local_var(std::string);
};


#endif
