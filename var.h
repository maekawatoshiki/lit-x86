#ifndef _VAR_LIT_
#define _VAR_LIT_

#include <string>
#include <vector>
#include "common.h"
#include "func.h"

typedef struct {
	std::string name, mod_name;
	unsigned int id;
	int type;
	int loctype;
} var_t;

enum {
	V_LOCAL,
	V_GLOBAL
};
enum {
	T_INT,
	T_STRING,
	T_DOUBLE
};

class Variable {
public:
	std::vector< std::vector<var_t> > local;
	std::vector<var_t> global;
	FunctionList &funcs;
	std::string &module;

	Variable(FunctionList &f, std::string &mod): funcs(f), module(mod) { local.resize(100); }
	std::vector<var_t> &focus();
	var_t *get(std::string, std::string);
	var_t *append(std::string, int);
};

#endif // _LIT_VAR_

