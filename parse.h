#ifndef _PARSE_LIT_
#define _PARSE_LIT_

#include "lit.h"
#include "asm.h"
#include "expr.h"

typedef struct {
	char name[32];
	unsigned int id;
	int type;
	int loctype;
} Variable;

struct {
	Variable var[0xFF];
	int count;
} gblVar;

Variable locVar[0xFF][0xFF];
int varSize[0xFF], varCounter;

typedef struct {
	int address;
	char name[0xFF];
} func_t;

struct {
	func_t func[0xff];
	int count;
} functions;

int nowFunc; // number of function
int isFunction; // With in function?

struct {
	char *text[0xff];
	int *addr;
	int count;
} strings;

int ifStmt();
int whileStmt();
int functionStmt();

int eval(int, int);
int expression(int, int);

int parser();
int getString();
int getFunction(char *, int);
Variable *getVariable(char *);
Variable *appendVariable(char *, int);
int appendBreak();
int appendReturn();


int isassign();
int assignment();
Variable *declareVariable();

#endif
