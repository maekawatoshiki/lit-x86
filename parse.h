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
	Variable var[0xFF][0xFF]; // var[ "funcs.now" ] [ each var ]
	int count, size[0xFF];
} locVar;

typedef struct {
	int address, params;
	char name[0xFF], mod_name[0xff];
} func_t;

struct {
	func_t func[0xff];
	int count;
} undefined_funcs;

struct {
	func_t func[0xff];
	int count, inside, now;
} funcs;

// strings embedded in native code
struct {
	char *text[0xff];
	int *addr;
	int count;
} strings;

void skip_tok();

int streql(char *, char *);

int make_if();
int make_while();
int make_func();
int make_break();
int make_return();

int eval(int, int);
int expression(int, int);

int parser();
int getString();

func_t *get_func(char *, char *);
func_t *append_func(char *, int, int);

Variable *get_var(char *, char *);
Variable *append_var(char *, int);

int append_undefined_func(char *, char *, int);
int is_undefined_func(char *, int);

int isassign();
int assignment();
int assignment_single(Variable *);
int assignment_array(Variable *);
Variable *declare_var();

char *replaceEscape(char *);

#endif
