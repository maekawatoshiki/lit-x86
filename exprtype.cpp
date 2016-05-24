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

bool ExprType::change(ExprType *ty) {
	type.type = ty->type.type;
	type.user_type = ty->type.user_type;
	next = ty->next;
	return true;
}

bool ExprType::change(int ary, ExprType *ty) {
	type.type = ary;
	next = ty;
	return true;
}

bool ExprType::is_array() {
	if(type.type == T_ARRAY) return true;
	else return false;
}

bool ExprType::eql_type(int ty, bool is_ary) {
	if(is_ary) {
		return (type.type & ty) && (type.type & T_ARRAY);
	} else 
		return type.type == ty;
}

void ExprType::show() {
	std::cout << "builtin: " << type.type << " ";
	if(next && type.type == T_ARRAY) next->show();
	std::cout << std::endl;
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
