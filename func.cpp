#include "func.h"
#include "parse.h"
#include "asm.h"
#include "lit.h"

uint32_t Function::call(NativeCode_x86 &ntv) {
	if(info.is_lib) {
		ntv.gencode(0xe8); ntv.gencode_int32(info.address - (uint32_t)&ntv.code[ntv.count] - ADDR_SIZE);
	} else {
		ntv.gencode(0xe8); ntv.gencode_int32(0xFFFFFFFF - (ntv.count - info.address) - 3); // call Function
	}
	return info.address;
}

bool Module::is(std::string name, std::string mod_name) {
	return get(name, mod_name) == NULL ? false : true;
}

Function *Module::get(std::string name, std::string mod_name) {
	for(std::vector<Function>::iterator it = func.begin(); it != func.end(); it++) {
		if(it->info.name == name/* && it->mod_name == mod_name*/) {
			return &(*it);
		}
	}
	return NULL;
}


Function *Module::append(Function f) {
	func.push_back(f);
	return &func.back();
}

Function *Module::append_undef(std::string name, std::string mod_name, int ntvc_pos) {
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

bool Module::rep_undef(std::string name, int ntvc) {
	bool replaced = false;
	for(std::vector<Function>::iterator it = undef_func.begin(); it != undef_func.end(); it++) {
		if(it->info.name == name && it->info.mod_name == module) {
			ntv.gencode_int32_insert(ntvc - it->info.address - 4, it->info.address);
			replaced = true;
		}
	}
	return replaced;
}

var_t *Module::append_global_var(std::string name, int type) {
	var_t *v = var_global.append(name, type);
	v->is_global = true;
	return v;
}

void Module::append_addr_of_global_var(std::string name, int ntv_pos) {
	var_t *v = var_global.get(name, "");
	if(v == NULL) puts("error: nullptr");
	v->used_location.push_back(ntv_pos);
}

void Module::insert_global_var() {
	for(std::vector<var_t>::iterator it = var_global.local.begin(); it != var_global.local.end(); ++it) {
		uint32_t g_addr = (uint32_t)LitMemory::alloc(ADDR_SIZE);
		for(std::vector<int>::iterator pos = it->used_location.begin(); pos != it->used_location.end(); ++pos) {
			ntv.gencode_int32_insert(g_addr, *pos);
		}
		ntv.gencode_int32(0x00000000);
	}
}
