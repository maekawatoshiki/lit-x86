#ifndef _EXPRTYPE_LIT_
#define _EXPRTYPE_LIT_

#include "common.h"
#include "var.h"

#define IS_ARRAY(x) ((x) & (T_ARRAY))
#define IS_TYPE(x, ty)  ((x) & (ty))

struct expr_type_t {
	expr_type_t(int ty=T_INT): type(ty) {};
	int type;
	std::string user_type;
};

class ExprType {
public:
	expr_type_t type;

	expr_type_t &get();
	bool change(int);
	bool change(std::string);
	bool is_array();
	bool is_type(int);
};

#endif

