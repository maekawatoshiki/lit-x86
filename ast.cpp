#include "ast.h"

NumberAST::NumberAST(int n) : 
	number(n) {
}
FloatNumberAST::FloatNumberAST(double f) : 
	number(f) {
}

PostfixAST::PostfixAST(std::string o, AST *c) : 
	op(o), expr(c) {
}

BinaryAST::BinaryAST(std::string o, AST *le, AST *re) :
	op(o), left(le), right(re) {
}

CharAST::CharAST(int c) : 
	ch(c) {
}

StringAST::StringAST(std::string s) : 
	str(s) {
}

NewAllocAST::NewAllocAST(std::string ty, AST *sz) : 
	type(ty), size(sz) {
}

VariableAST::VariableAST(var_t v) : 
	info(v) {
}

VariableAsgmtAST::VariableAsgmtAST(AST *a, AST *b) : 
	var(a), src(b) {
}

VariableIndexAST::VariableIndexAST(AST *v, AST *i) : 
	var(v), idx(i) {
}

VariableDeclAST::VariableDeclAST(var_t v) : 
	info(v) {
}

LibraryAST::LibraryAST(std::string nm, std::vector<AST *> p) : 
	lib_name(nm), proto(p) {
}

PrototypeAST::PrototypeAST(func_t fun, std::vector<AST *> arg) : 
	proto(fun), args_type(arg) {
}

FunctionCallAST::FunctionCallAST(func_t f, std::vector<AST *> a) : 
	info(f), args(a) {
}

FunctionAST::FunctionAST(func_t f, std::vector<AST *> a, std::vector<AST *> stmt) :
	info(f), args(a), statement(stmt) {
}

StructAST::StructAST(std::string _name, std::vector<AST *> _vars) : 
	name(_name), var_decls(_vars) {
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

ForAST::ForAST(AST *a, AST *c, AST *s, std::vector<AST *> b) : 
	asgmt(a), cond(c), step(s), block(b), is_range_for(false) {
}

ReturnAST::ReturnAST(AST *e) : 
	expr(e) {
}

