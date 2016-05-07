#ifndef _CODEGEN_LIT_
#define _CODEGEN_LIT_

#include "common.h"
#include "func.h"
#include "ast.h"
#include "exprtype.h"

int codegen_entry(ast_vector &);
llvm::Value *codegen_expression(Function &f, Program &f_list, AST *ast, int * = NULL);
bool const_folding(AST *, int *);

struct {
	std::string name, mod_name;
	int args, addr, type; // if args is -1, the function has vector args.
	llvm::Function *func;
} stdfunc[] = {
	{"Array", "", 1, 12, T_INT | T_ARRAY},
	{"printf", "", -1, 16, T_VOID},
	{"sleep", "Time", 1, 24, T_VOID},
	{"open", "File", 2, 28, T_INT},
	{"write", "File", -1, 32, T_INT},
	{"read", "File", 3, 40, T_INT},
	{"close", "File", 1, 36, T_INT},
	{"gets", "File", 0, 52, T_STRING},
	{"free", "Sys", 1, 44, T_VOID},
	{"strlen", "", 1, 64, T_INT},
	{"len", "", 1, 68, T_INT},
	{"GC", "", 0, 72, T_INT},
	{"replace", "", 3, 88, T_STRING},
	{"split", "", 2, 92, T_STRING | T_ARRAY},
	{"puts", "", -1, -1, T_VOID} // special
};


#endif

