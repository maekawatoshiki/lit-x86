#ifndef _EXPR_LIT_
#define _EXPR_LIT_

#include "common.h"

#define HAS_PARAMS_FUNC is_number_tok() || is_ident_tok() || \
							is_string_tok() || is_char_tok() || tok.is("(")

#endif
