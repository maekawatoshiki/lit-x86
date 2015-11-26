#ifndef _VAR_LIT_
#define _VAR_LIT_

#include "lit.h"
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

Variable *get_var(char *, char *);
Variable *append_var(char *, int);
Variable *declare_var();

int is_asgmt();
int asgmt();
int asgmt_single(Variable *);
int asgmt_array(Variable *);

#endif // _LIT_VAR_
