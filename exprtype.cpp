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
	type= ty->type;
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

bool ExprType::is_ref() {
	return type.ref;
}
void ExprType::set_ref(bool b) {
	type.ref = b;
}

bool ExprType::eql_type(int ty, bool is_ary) {
	if(is_ary) {
		return (type.type & ty) && (type.type & T_ARRAY);
	} else {
		return type.type == ty;
	}
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
	ExprType *str_to_type(std::string str) {
		int is_ary = 0;
		auto ary = [](std::string str, ExprType *ty) -> ExprType * {
			size_t p=0;
			while((p=str.find("[]")) != std::string::npos) {
				str = str.substr(p);
				ty->next = new ExprType(ty->get().type);
				ty->change(T_ARRAY);
			}
			return ty;
		};
		if(str.find("int64") != std::string::npos) { 
			ExprType *ty = new ExprType(T_INT64);
			ty = ary(str, ty);
			return ty;
		} else if(str.find("int") != std::string::npos) { 
			ExprType *ty = new ExprType(T_INT);
			ty = ary(str, ty);
			return ty;
		} else if(str == "char") {
			return new ExprType(T_CHAR);
		} else if(str == "bool") {
			return new ExprType(T_BOOL);
		} else if(str == "string") { 
			return new ExprType(T_STRING);
		} else if(str == "double") { 
			return new ExprType(T_DOUBLE);
		} else {
			return new ExprType(str);
		}
	}

	std::string type_to_str(ExprType *type) {
		std::string str_ty;
		switch(type->get().type) {
			case T_INT:
				str_ty = "int";
				break;
			case T_INT64:
				str_ty = "int64";
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
			case T_USER_TYPE:
				str_ty = type->get().user_type;
				break;
			case T_ARRAY:
				str_ty = "[" + (type->next ? type_to_str(type->next) : "") + "]";
				break;
		}
		return str_ty;
	}

	llvm::Type *type_to_llvmty(ExprType *type) {
		ExprType *ty = type;
		llvm::Type *out;
		int ary_count = 0;
		while(ty->is_array()) {
			ty = ty->next;
			ary_count++;
		}
		if(ty->eql_type(T_STRING)) {
			out = llvm::Type::getInt8PtrTy(llvm::getGlobalContext());
			while(ary_count--) out = out->getPointerTo();	
		} else if(ty->eql_type(T_DOUBLE)) {
			out = llvm::Type::getDoubleTy(llvm::getGlobalContext());
			while(ary_count--) out = out->getPointerTo();	
		} else if(ty->eql_type(T_INT)) {
			out = llvm::Type::getInt32Ty(llvm::getGlobalContext());
			while(ary_count--) out = out->getPointerTo();
		} else if(ty->eql_type(T_INT64)) {
			out = llvm::Type::getInt64Ty(llvm::getGlobalContext());
			while(ary_count--) out = out->getPointerTo();
		} else if(ty->eql_type(T_CHAR)) {
			out = llvm::Type::getInt32Ty(llvm::getGlobalContext());
			while(ary_count--) out = out->getPointerTo();
		} else if(ty->eql_type(T_USER_TYPE)) {
			return nullptr;
			// out = builder.getInt8PtrTy();
			// while(ary_count--) out = out->getPointerTo();	
		}
		// llvm::Type *func_ret_type = 
		// 	info.type.eql_type(T_STRING) ? 
		// 		(llvm::Type *)builder.getInt8PtrTy() : 
		// 		info.type.eql_type(T_DOUBLE) ?
		// 			(llvm::Type *)builder.getDoubleTy() : 
		// 			info.type.eql_type(T_USER_TYPE) ? 
		// 				(llvm::Type *)f_list.structs.get(info.type.get().user_type)->strct->getPointerTo() : 
		// 				(info.type.eql_type(T_ARRAY)) ? 
		// 					(info.type.next->eql_type(T_STRING)) ? 
		// 						(llvm::Type *)builder.getInt8PtrTy()->getPointerTo() : 
		// 						(llvm::Type *)builder.getInt32Ty()->getPointerTo() : 
		// 				(llvm::Type *)builder.getInt32Ty();
		return out;
	}
};
