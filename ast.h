#ifndef _AST_LIT_
#define _AST_LIT_

#include "common.h"

enum {
	AST_NUMBER,
	AST_STRING,
	AST_BINARY,
	AST_VARIABLE
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

class BinaryAST : public AST {
public:
	std::string op;
	AST *left, *right;
	BinaryAST(std::string o, AST *le, AST *re);
	virtual int get_type() const { return AST_BINARY; }
};

class VariableAST : public AST {
public:
	
};

#endif
