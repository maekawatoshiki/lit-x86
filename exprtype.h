#ifndef _EXPRTYPE_LIT_
#define _EXPRTYPE_LIT_

#include "common.h"

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
	int mode, type;
	std::string user_type;
	expr_type_t *expr_type;
};

class ExprType {
	expr_type_t type;
public:
	ExprType() {};
	ExprType(int ty) { type.type = ty; }
	ExprType(std::string ty) { type.type = T_USER_TYPE; type.user_type = ty; }
	ExprType(ExprType *et) { 
		type = et->type; 
		next = et->next;
	}
	~ExprType() {  }
	expr_type_t &get();
	ExprType *next = NULL;
	bool change(int);
	bool change(ExprType *);
	bool change(int, ExprType *);
	bool change(std::string);
	bool is_array();
	bool eql_type(int, bool = false);
	bool eql_type(ExprType *);

	std::string to_string();
};

namespace Type {
	ExprType *str_to_type(std::string);
	std::string type_to_str(ExprType *);
	llvm::Type *type_to_llvmty(ExprType *);
};
#endif

