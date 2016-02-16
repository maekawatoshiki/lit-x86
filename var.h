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
	int loctype;
} var_t;

enum {
	V_LOCAL,
	V_GLOBAL
};

enum {
	T_INT 			 = 1 << 1,
	T_STRING		 = 1 << 2,
	T_DOUBLE		 = 1 << 3,
	T_USER_TYPE	 = 1 << 4,
	T_ARRAY			 = 1 << 5
};

class Variable {
public:
	std::vector<var_t> local;
	std::vector<var_t> global;

	var_t *get(std::string, std::string);
	var_t *append(std::string, int, std::string = "");
};

#endif // _LIT_VAR_

