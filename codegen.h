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

struct func_body_t {
	Function *info;
	std::vector<std::string> arg_names, cur_mod;
	std::vector<llvm::Type *> arg_types;
	ast_vector body;
	llvm::Function *func;
	llvm::Type *ret_type;
};

std::vector<func_body_t> funcs_body;

#endif

