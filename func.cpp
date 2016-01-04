#include "func.h"
#include "parse.h"
#include "asm.h"

bool FunctionList::is(std::string name, std::string mod_name) {
	return get(name, mod_name) == NULL ? false : true;
}

func_t *FunctionList::get(std::string name, std::string mod_name) {
	for(int i = 0; i < func.size(); i++) {
#ifdef DEBUG
		std::cout << mod_name.c_str() << " : " << name.c_str() << " <<< " << func[i].mod_name.c_str() << " : " func[i].name.c_str() << endl;
#endif
		if(func[i].name == name && func[i].mod_name == mod_name) {
			return &(func[i]);
		}
	}
	return NULL;
}

func_t *FunctionList::focus() { return &func[now]; }

func_t *FunctionList::append(std::string name, int address, int params) {
	func_t f = {
		.address = address,
		.params = params,
		.mod_name = module,
		.name = name
	};
	func.push_back(f);
#ifdef DEBUG
	std::cout << func[func.size() - 1].mod_name << " : " << name << std::endl;
#endif
	return &(func[func.size() - 1]);
}

func_t *FunctionList::append_undef(std::string name, std::string mod_name, int ntvc_pos) {
	func_t f = {
		.address = ntvc_pos,
		.mod_name = module,
		.name = name
	};
	func.push_back(f);
	return 0;
}

bool FunctionList::rep_undef(std::string name, int ntvc) {
	std::vector<func_t> &f = func;

	for(int i = 0; i < f.size(); i++) {
		if(f[i].name == name && f[i].mod_name == module) {
			ntv.gencode_int32_insert(ntvc - f[i].address - 4, f[i].address);
		}
	}
	return true;
}
