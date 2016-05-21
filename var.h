#ifndef _VAR_LIT_
#define _VAR_LIT_

#include <string>
#include <vector>
#include "common.h"
#include "exprtype.h"

typedef struct {
	std::string name, mod_name;
	unsigned int id;
	ExprType type;
	std::string class_type;
	bool is_global;
	std::vector<int> used_location; // for global variables
	llvm::Value *val; // for codegen
} var_t;

enum {
	V_LOCAL,
	V_GLOBAL
};

class Variable {
public:
	std::vector<var_t> local;

	var_t *get(std::string, std::string);
	var_t *append(std::string, int, bool = false, std::string = "");
	size_t total_size();
};

#endif // _LIT_VAR_

