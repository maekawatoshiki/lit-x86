#include "ast.h"

NumberAST::NumberAST(int n) : 
	number(n) {
}

BinaryAST::BinaryAST(std::string o, AST *le, AST *re) :
	op(o), left(le), right(re) {
}

StringAST::StringAST(std::string s) : 
	str(s) {
}

VariableAST::VariableAST(var_t v) : 
	info(v) {
}

VariableDeclAST::VariableDeclAST(var_t v) : 
	info(v) {
}

FunctionCallAST::FunctionCallAST(func_t f, std::vector<AST *> a) : 
	info(f), args(a) {
}

FunctionAST::FunctionAST(func_t f, std::vector<AST *> a, std::vector<AST *> stmt) :
	info(f), args(a), statement(stmt) {
}

ArrayAST::ArrayAST(std::vector<AST *> e) : 
	elems(e) {
}

IfAST::IfAST(AST *c, std::vector<AST *> t, std::vector<AST *> e) : 
	cond(c), then_block(t), else_block(e) {
}

WhileAST::WhileAST(AST *c, std::vector<AST *> b) : 
	cond(c), block(b) {
}
