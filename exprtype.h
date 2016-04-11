#ifndef _EXPRTYPE_LIT_
#define _EXPRTYPE_LIT_

#include "common.h"
#include "var.h"

#define IS_ARRAY(x) ((x) & (T_ARRAY))
#define IS_TYPE(x, ty)  ((x) == (ty))

enum {
	T_VOID			 = 1 << 1,
	T_BOOL			 = 1 << 2,
	T_INT 			 = 1 << 3,
	T_STRING		 = 1 << 4,
	T_CHAR			 = 1 << 5,
	T_DOUBLE		 = 1 << 6,
	T_USER_TYPE	 = 1 << 7,
	T_ARRAY			 = 1 << 8
};

struct expr_type_t {
	expr_type_t(int ty=T_INT): type(ty) {};
	int type;
	std::string user_type;
};

namespace Type {
	int str_to_type(std::string);
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

