#ifndef _CODEGEN_LIT_
#define _CODEGEN_LIT_

#include "common.h"
#include "func.h"
#include "ast.h"
#include "exprtype.h"

int codegen_entry(ast_vector &);
llvm::Value *codegen_expression(Function &f, Program &f_list, AST *ast, ExprType * = NULL);
bool const_folding(AST *, int *);

#endif

