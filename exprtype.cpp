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

bool ExprType::eql_type(ExprType *ty) {
	if(ty->eql_type(type.type)) {
		if(next && ty->next) {
			ExprType *_next = this;
			ty = ty->next;
			bool res = false;
			res = ty->eql_type(next);
			return res;
		}
		return true;
	} else return false;
}

std::string ExprType::to_string() {
	return Type::type_to_str(this);
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

	std::string type_to_str(ExprType *type) {
		std::string str_ty;
		switch(type->get().type) {
			case T_INT:
				str_ty = "int";
				break;
			case T_CHAR:
				str_ty = "char";
				break;
			case T_BOOL:// bool is exist but unsupport now..
				str_ty = "bool";
				break;
			case T_STRING:
				str_ty = "string";
				break;
			case T_DOUBLE:
				str_ty = "double";
				break;
			case T_ARRAY:
				str_ty = "[" + (type->next ? type_to_str(type->next) : "") + "]";
				break;
		}
		return str_ty;
	}
};
