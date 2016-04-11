#include "exprtype.h"

expr_type_t &ExprType::get() {
	return type;
}

bool ExprType::change(int ty) {
	type.type = ty;
	return true;
}

bool ExprType::change(std::string ty) {
	type.type = T_USER_TYPE;
	type.user_type = ty;
	return true;
}

bool ExprType::is_array() {
	return type.type & T_ARRAY;
}

bool ExprType::is_type(int ty) {
	return type.type & ty;
}

namespace Type {
	int str_to_type(std::string str) {
		int is_ary = 0;
		if(str == "int") { 
			return T_INT;
		} else if(str == "bool") {
			return T_BOOL;
		} else if(str == "string") { 
			return T_STRING;
		} else if(str == "double") { 
			return T_DOUBLE;
		} else {
			return T_USER_TYPE;
		}
	}
};
