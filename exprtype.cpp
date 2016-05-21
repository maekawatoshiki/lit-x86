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

bool ExprType::change(ExprType &ty) {
	type.type = ty.type.type;
	type.user_type = ty.type.user_type;
	return true;
}

bool ExprType::is_array() {
	return type.type & T_ARRAY;
}

bool ExprType::eql_type(int ty, bool is_ary) {
	if(is_ary) {
		return (type.type & ty) && (type.type & T_ARRAY);
	} else 
		return type.type == ty;
}

namespace Type {
	int str_to_type(std::string str) {
		int is_ary = 0;
		if(str == "int") { 
			return T_INT;
		} else if(str == "char") {
			return T_CHAR;
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
