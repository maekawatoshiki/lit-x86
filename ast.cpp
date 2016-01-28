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
