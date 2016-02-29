#include "func.h"
#include "parse.h"
#include "asm.h"

bool FunctionList::is(std::string name, std::string mod_name) {
	return get(name, mod_name) == NULL ? false : true;
}

Function *FunctionList::get(std::string name, std::string mod_name) {
	for(std::vector<Function>::iterator it = func.begin(); it != func.end(); it++) {
		if(it->info.name == name/* && it->mod_name == mod_name*/) {
			return &(*it);
		}
	}
	return NULL;
}


Function *FunctionList::append(Function f) {
	func.push_back(f);
#ifdef DEBUG
	std::cout << func[func.size() - 1].mod_name << " : " << name << std::endl;
#endif
	return &func.back();
}

Function *FunctionList::append_undef(std::string name, std::string mod_name, int ntvc_pos) {
	Function f = {
		.info = {
			.address = (uint32_t)ntvc_pos,
			.mod_name = module,
			.name = name
		}
	};
	func.push_back(f);
	return 0;
}

bool FunctionList::rep_undef(std::string name, int ntvc) {
	for(std::vector<Function>::iterator it = func.begin(); it != func.end(); it++) {
		if(it->info.name == name && it->info.mod_name == module) {
			ntv.gencode_int32_insert(ntvc - it->info.address - 4, it->info.address);
		}
	}
	return true;
}
