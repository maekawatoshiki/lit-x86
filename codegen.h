#ifndef _CODEGEN_LIT_
#define _CODEGEN_LIT_

#include "common.h"
#include "func.h"
#include "ast.h"
#include "exprtype.h"

namespace Codegen {
	llvm::Module *codegen(ast_vector &);
	int run(llvm::Module *, bool = false, bool = false);
	llvm::Value *expression(Function &f, Program &f_list, AST *ast, ExprType * = NULL);
};


#endif

