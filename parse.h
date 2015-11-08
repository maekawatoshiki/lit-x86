#ifndef _PARSE_LIT_
#define _PARSE_LIT_

#include "lit.h"
#include "asm.h"
#include "lex.h"
#include "expr.h"

typedef struct {
	char name[32], mod_name[32];
	unsigned int id;
	int type;
	int loctype;
} Variable;

struct {
	Variable var[0xFF];
	int count;
} gblVar;

struct {
	Variable var[0xFF][0xFF]; // var[ "functions.now" ] [ each variable ]
	int count, size[0xFF];
} locVar;

typedef struct {
	int address, params;
	char name[0xFF], mod_name[0xff];
} func_t;

struct {
	func_t func[0xff];
	int count, inside, now;
} functions;

// strings embedded in native code
struct {
	char *text[0xff];
	int *addr;
	int count;
} strings;

void skip_tok();

int ifStmt();
int whileStmt();
int functionStmt();

int eval(int, int);
int expression(int, int);

int parser();
int getString();

func_t *getFunction(char *, char *);
func_t *appendFunction(char *, int, int);

Variable *getVariable(char *, char *);
Variable *appendVariable(char *, int);
int make_break();
int make_return();

int isassign();
int assignment();
int assignment_single(Variable *);
int assignment_array(Variable *);
Variable *declareVariable();

char *replaceEscape(char *);

#endif
