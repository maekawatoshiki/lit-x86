#ifndef _VAR_LIT_
#define _VAR_LIT_

#include "lit.h"
#include "asm.h"
#include "expr.h"
#include "util.h"
#include "token.h"
#include "library.h"

typedef struct {
	std::string name, mod_name;
	unsigned int id;
	int type;
	int loctype;
} var_t;

typedef struct {
	var_t var[0xFF];
	int count;
} gblvar_t;

typedef struct {
	var_t var[0xFF][0xFF]; // var[ "funcs.now" ] [ each var ]
	int count, size[0xFF];
} locvar_t;

extern locvar_t locVar;
extern gblvar_t gblVar;

var_t *get_var(std::string , std::string );
var_t *append_var(std::string , int);
var_t *declare_var();

int is_asgmt();
int asgmt();
int asgmt_single(var_t *);
int asgmt_array(var_t *);

#endif // _LIT_VAR_

