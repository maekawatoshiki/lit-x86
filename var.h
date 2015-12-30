#ifndef _VAR_LIT_
#define _VAR_LIT_

#include <string>
#include <vector>

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

	Variable() { local.resize(100); }
	std::vector<var_t> &focus();
	var_t *get(std::string, std::string);
	var_t *append(std::string, int);
};

extern Variable var;

var_t *declare_var();

int is_asgmt();
int asgmt();
int asgmt_single(var_t *);
int asgmt_array(var_t *);

#endif // _LIT_VAR_

