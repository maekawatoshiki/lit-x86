#ifndef _EXPR_LIT_
#define _EXPR_LIT_

#include "lit.h"
#include "asm.h"
#include "lex.h"
#include "token.h"
#include "parse.h"
#include "stdfunc.h"
#include "util.h"
#include "library.h"
#include "var.h"

#define HAS_PARAMS_FUNC is_number_tok() || is_ident_tok() || \
							is_string_tok() || is_char_tok() || tok.is("(")

// int is_string_tok();
// int is_number_tok();
// int is_ident_tok();
// int is_char_tok();
//
// int expr_entry();
// int expr_compare();
// int expr_logic();
// int expr_add_sub();
// int expr_mul_div();
// int expr_primary();
//
// int is_index();
// int make_index();


#endif
