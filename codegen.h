#ifndef _CODEGEN_LIT_
#define _CODEGEN_LIT_

#include "common.h"
#include "func.h"
#include "ast.h"

int codegen_entry(ast_vector &);
int codegen_expression(Function &f, Module &f_list, AST *ast);
bool const_folding(AST *, int *);

#endif

