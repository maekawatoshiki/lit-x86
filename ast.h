#ifndef _AST_LIT_
#define _AST_LIT_

#include "common.h"
#include "var.h"
#include "func.h"

enum {
	AST_NUMBER,
	AST_STRING,
	AST_POSTFIX,
	AST_BINARY,
	AST_VARIABLE,
	AST_VARIABLE_DECL,
	AST_FUNCTION_CALL,
	AST_FUNCTION,
	AST_IF,
	AST_WHILE,
	AST_FOR,
	AST_ARRAY
};


class AST {
public:
	virtual int get_type() const = 0;
	virtual ~AST() {};
};

class NumberAST : public AST {
public:
	int32_t number;
	NumberAST(int);
	virtual int get_type() const { return AST_NUMBER; };
};

class StringAST : public AST {
public:
	std::string str;
	StringAST(std::string);
	virtual int get_type() const { return AST_STRING; };
};

class PostfixAST : public AST {
public:
	std::string op;
	AST *expr;
	PostfixAST(std::string, AST *);
	~PostfixAST() { delete expr; }
	virtual int get_type() const { return AST_POSTFIX; }
};

class BinaryAST : public AST {
public:
	std::string op;
	AST *left, *right;
	BinaryAST(std::string o, AST *le, AST *re);
	virtual int get_type() const { return AST_BINARY; }
};

class VariableAST : public AST {
public:
	var_t info;
	VariableAST(var_t v);
	virtual int get_type() const { return AST_VARIABLE; }
};

class VariableDeclAST : public AST {
public:
	var_t info;
	VariableDeclAST(var_t);
	virtual int get_type() const { return AST_VARIABLE_DECL; }
};

class FunctionCallAST : public AST {
public:
	func_t info;
	std::vector<AST *> args;
	FunctionCallAST(func_t f, std::vector<AST *> a);
	virtual int get_type() const { return AST_FUNCTION_CALL; }
};

class FunctionAST : public AST {
public:
	func_t info;
	std::vector<AST *> args, statement;
	FunctionAST(func_t f, std::vector<AST *> a, std::vector<AST *>);
	virtual int get_type() const { return AST_FUNCTION; }
};

class ArrayAST : public AST {
public:
	std::vector<AST *> elems;
	ArrayAST(std::vector<AST *>);
	virtual int get_type() const { return AST_ARRAY; }
};

class IfAST : public AST {
public:
	AST *cond;
	std::vector<AST *> then_block, else_block;
	IfAST(AST *, std::vector<AST*>, std::vector<AST *>);
	virtual int get_type() const { return AST_IF; }
};

class WhileAST : public AST {
public:
	AST *cond;
	std::vector<AST *> block;
	WhileAST(AST *, std::vector<AST *>);
	virtual int get_type() const { return AST_WHILE; }
};

class ForAST : public AST {
public:
	AST *asgmt, *cond, *step;
	std::vector<AST *> block;
	ForAST(AST *, AST *, AST *, std::vector<AST *>);
	virtual int get_type() const { return AST_FOR; }
};

typedef std::vector<AST *> ast_vector;

#endif
