#include "func.h"
#include "parse.h"
#include "asm.h"
#include "lit.h"
#include "exprtype.h"

uint32_t Function::call(NativeCode_x86 &ntv) {
	return info.address;
}

bool Program::is(std::string name, std::string mod_name) {
	auto is_declared_func = [&](std::string name) -> Function * {
		for(std::vector<Function>::iterator it = func.begin(); it != func.end(); it++) {
			if(it->info.name == name/* && it->mod_name == mod_name*/) {
				return &(*it);
			}
		}
		return NULL;
	};
	return is_declared_func(name) == NULL ? false : true;
}

Function *Program::get(std::string name, std::vector<ExprType *> args_type, std::string mod_name) {
	auto is_eql_args_type = [&](Function f) -> bool {
		if(f.info.args_type.size() == 0 && args_type.size() == 0) return true;
		if(f.info.args_type.size() != args_type.size()) return false;
		auto caller_it = args_type.begin();
		for(auto it = f.info.args_type.begin(); it != f.info.args_type.end() && caller_it != args_type.end(); ++it) {
			if(!(*it)->eql_type((*caller_it))) return false;
			caller_it++;
		}
		return true;
	};
	for(std::vector<Function>::iterator it = func.begin(); it != func.end(); it++) {
		if(it->info.name == name && is_eql_args_type(*it)/* && it->mod_name == mod_name*/) {
			return &(*it);
		}
	}
	return NULL;
}


Function *Program::append(Function f) {
	func.push_back(f);
	return &func.back();
}

Function *Program::append_undef(std::string name, std::string mod_name, int ntvc_pos) {
	Function f = {
		.info = {
			.address = (uint32_t)ntvc_pos,
			.mod_name = module,
			.name = name
		}
	};
	undef_func.push_back(f);
	return 0;
}

bool Program::rep_undef(std::string name, int ntvc) {
	bool replaced = false;
	for(std::vector<Function>::iterator it = undef_func.begin(); it != undef_func.end(); it++) {
		if(it->info.name == name && it->info.mod_name == module) {
			ntv.gencode_int32_insert(ntvc - it->info.address - 4, it->info.address);
			replaced = true;
		}
	}
	return replaced;
}

var_t *Program::append_global_var(std::string name, int type) {
	var_t *v = var_global.append(name, new ExprType(type));
	v->is_global = true;
	return v;
}

void Program::append_addr_of_global_var(std::string name, int ntv_pos) {
	var_t *v = var_global.get(name, "");
	if(v == NULL) puts("error: nullptr");
	v->used_location.push_back(ntv_pos);
}

void Program::insert_global_var() {
	for(std::vector<var_t>::iterator it = var_global.local.begin(); it != var_global.local.end(); ++it) {
		uint64_t g_addr = (uint64_t)LitMemory::alloc(1, ADDR_SIZE);
		for(std::vector<int>::iterator pos = it->used_location.begin(); pos != it->used_location.end(); ++pos) {
			ntv.gencode_int32_insert(g_addr, *pos);
		}
		ntv.gencode_int32(0x00000000);
	}
}
