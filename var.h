#ifndef _VAR_LIT_
#define _VAR_LIT_

#include <string>
#include <vector>
#include "common.h"

typedef struct {
	std::string name, mod_name;
	unsigned int id;
	int type;
	std::string class_type;
	bool is_global;
} var_t;

enum {
	V_LOCAL,
	V_GLOBAL
};

class Variable {
public:
	std::vector<var_t> local;
	std::vector<var_t> global;

	var_t *get(std::string, std::string);
	var_t *append(std::string, int, std::string = "");
	size_t total_size();
};

#endif // _LIT_VAR_

