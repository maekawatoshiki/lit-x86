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

ArrayAST::ArrayAST(std::vector<AST *> e) : 
	elems(e) {
}
