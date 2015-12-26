#ifndef _VAR_LIT_
#define _VAR_LIT_

#include "lit.h"
#include "expr.h"
#include "util.h"

typedef struct {
	std::string name, mod_name;
	unsigned int id;
	int type;
	int loctype;
} Variable;

typedef struct {
	Variable var[0xFF];
	int count;
} gblvar_t;

typedef struct {
	Variable var[0xFF][0xFF]; // var[ "funcs.now" ] [ each var ]
	int count, size[0xFF];
} locvar_t;

extern gblvar_t gblVar;
extern locvar_t locVar;

Variable *get_var(std::string , std::string );
Variable *append_var(std::string , int);
Variable *declare_var();

int is_asgmt();
int asgmt();
int asgmt_single(Variable *);
int asgmt_array(Variable *);

#endif // _LIT_VAR_
