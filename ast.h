#ifndef _AST_LIT_
#define _AST_LIT_

#include "common.h"
#include "var.h"
#include "func.h"
#include "exprtype.h"

enum {
	AST_NUMBER,
	AST_NUMBER_FLOAT,
	AST_STRING,
	AST_CHAR,
	AST_POSTFIX,
	AST_BINARY,
	AST_VARIABLE,
	AST_VARIABLE_DECL,
	AST_VARIABLE_ASGMT,
	AST_VARIABLE_INDEX,
	AST_FUNCTION_CALL,
	AST_FUNCTION,
	AST_IF,
	AST_WHILE,
	AST_FOR,
	AST_BREAK,
	AST_RETURN,
	AST_ARRAY,
	AST_LIBRARY,
	AST_PROTO,
	AST_NEW,
	AST_STRUCT,
	AST_DOT,
	AST_MODULE,
	AST_CAST
};

class AST {
public:
	virtual int get_type() const = 0;
	virtual ~AST() {};
};

class FloatNumberAST : public AST {
public:
	double number;
	FloatNumberAST(double);
	virtual int get_type() const { return AST_NUMBER_FLOAT; };
	llvm::Value *codegen(Function &, ExprType *);
};
class NumberAST : public AST {
public:
	int32_t number;
	NumberAST(int);
	virtual int get_type() const { return AST_NUMBER; };
	llvm::Value *codegen(Function &, ExprType *);
};

class CharAST : public AST {
public:
	int ch;
	CharAST(int);
	virtual int get_type() const { return  AST_CHAR; }
	llvm::Value *codegen(Function &, ExprType *);
};

class StringAST : public AST {
public:
	std::string str;
	StringAST(std::string);
	virtual int get_type() const { return AST_STRING; };
	llvm::Value *codegen(Function &, ExprType *);
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
	llvm::Value *codegen(Function &, Program &, ExprType *); // ret is type of expr.
};

class CastAST : public AST {
public:
	std::string type;
	AST *expr;
	CastAST(std::string, AST *);
	virtual int get_type() const { return AST_CAST; }
	llvm::Value *codegen(Function &, Program &, ExprType *);
};

class NewAllocAST : public AST {
public:
	std::string type;
	AST * size;
	NewAllocAST(std::string, AST *size);
	virtual int get_type() const { return AST_NEW; }
	llvm::Value *codegen(Function &, Program &, ExprType *);
};

class VariableAST : public AST {
public:
	var_t info;
	VariableAST(var_t v);
	virtual int get_type() const { return AST_VARIABLE; }
	llvm::Value *codegen(Function &, Program &, ExprType *);
	var_t *get(Function &, Program &);
	var_t *append(Function &, Program &);
};

class VariableDeclAST : public AST {
public:
	var_t info;
	VariableDeclAST(var_t);
	virtual int get_type() const { return AST_VARIABLE_DECL; }
	var_t *get(Function &, Program &);
	var_t *append(Function &, Program &);
};

class VariableAsgmtAST : public AST {
public:
	AST *var, *src;
	VariableAsgmtAST(AST *, AST *);
	virtual int get_type() const { return AST_VARIABLE_ASGMT; }
	llvm::Value * codegen(Function &, Program &, ExprType *);
};

class VariableIndexAST : public AST {
public:
	AST *var, *idx;
	VariableIndexAST(AST *, AST *);
	virtual int get_type() const { return AST_VARIABLE_INDEX; }
	llvm::Value * codegen(Function &, Program &, ExprType *);
	llvm::Value *get_elem(Function &, Program &, ExprType *);
};

class LibraryAST : public AST {
public:
	std::string lib_name;
	std::vector<AST *> proto;
	LibraryAST(std::string, std::vector<AST *>);
	virtual int get_type() const { return AST_LIBRARY; }
	llvm::Value * codegen(Program &);
};

class PrototypeAST : public AST {
public:
	func_t proto;
	std::string name;
	std::vector<AST *> args_type;
	PrototypeAST(func_t, std::vector<AST *>, std::string = "");	
	virtual int get_type() const { return AST_PROTO; }
	void append(llvm::Module *, Program &);
};

class FunctionCallAST : public AST {
public:
	func_t info;
	std::vector<AST *> args;
	FunctionCallAST(func_t f, std::vector<AST *> a);
	virtual int get_type() const { return AST_FUNCTION_CALL; }
	llvm::Value * codegen(Function &, Program &, ExprType *);
};

class FunctionAST : public AST {
public:
	func_t info;
	std::vector<AST *> args, statement;
	FunctionAST(func_t f, std::vector<AST *> a, std::vector<AST *>);
	virtual int get_type() const { return AST_FUNCTION; }
	Function codegen(Program &);
};

class ModuleAST : public AST {
public:
	std::string name;
	std::vector<AST *> statement;
	ModuleAST(std::string name, std::vector<AST *>);
	virtual int get_type() const { return AST_MODULE; }
	void codegen(Program &);
};

class StructAST : public AST {
public:
	std::string name;
	std::vector<AST *> var_decls;
	StructAST(std::string, std::vector<AST *>);
	virtual int get_type() const { return AST_STRUCT; }
	llvm::Value *codegen(Program &);
};

class DotOpAST : public AST {
public:
	AST *var, *member;
	DotOpAST(AST *, AST *);
	virtual int get_type() const { return AST_DOT; }
	llvm::Value *codegen(Function &, Program &, ExprType *);
};

class ArrayAST : public AST {
public:
	std::vector<AST *> elems;
	ExprType *type = NULL;
	ArrayAST(std::vector<AST *>);
	ArrayAST(ExprType *);
	virtual int get_type() const { return AST_ARRAY; }
	llvm::Value * codegen(Function &, Program &, ExprType *);
};

class IfAST : public AST {
public:
	AST *cond;
	std::vector<AST *> then_block, else_block;
	IfAST(AST *, std::vector<AST*>, std::vector<AST *>);
	virtual int get_type() const { return AST_IF; }
	llvm::Value * codegen(Function &, Program &);
};

class WhileAST : public AST {
public:
	AST *cond;
	std::vector<AST *> block;
	WhileAST(AST *, std::vector<AST *>);
	virtual int get_type() const { return AST_WHILE; }
	llvm::Value * codegen(Function &, Program &);
};

class ForAST : public AST {
public:
	bool is_range_for;
	AST *asgmt, *cond, *step, *range;
	std::vector<AST *> block;
	ForAST(AST *, AST *, AST *, std::vector<AST *>); // assignment, condition, step, body
	virtual int get_type() const { return AST_FOR; }
	llvm::Value * codegen(Function &, Program &);
};

class BreakAST : public AST {
public:
	virtual int get_type() const { return AST_BREAK; }
	llvm::Value * codegen(Function &, Program &);
};

class ReturnAST : public AST {
public:
	AST *expr;
	ReturnAST(AST *);
	virtual int get_type() const { return AST_RETURN; }
	llvm::Value * codegen(Function &, Program &);
};

typedef std::vector<AST *> ast_vector;


#endif
