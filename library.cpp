#include "library.h"
#include "util.h"

namespace LibraryList {
	void free_libraries() {
		for(int i = 0; i < lib.size(); i++) 
			dlclose(lib[i].handle);
	}

	int append(std::string name) {
		lib_t l = {
			.name = name,
			.no = (int)lib.size(),
			.handle = dlopen(("./lib/" + name + ".so").c_str(), RTLD_LAZY | RTLD_NOW)
		};
		if(l.handle == NULL)
			error("LitSystemError: cannot load library '%s'", name.c_str());
		lib.push_back(l);
		return lib.size() - 1;
	}

	bool is(std::string mod_name) {
		return get(mod_name) == NULL ? 0 : 1;
	}

	lib_t *get(std::string name) {
		for(int i = 0; i < lib.size(); i++) {
			if(lib[i].name == name) return &lib[i];
		}
		return NULL;
	}

	void *call(std::string name, std::string mod_name) {
		void *function = dlsym(get(mod_name)->handle, (mod_name + "_" + name).c_str());
		if(function == NULL) 
			error("LitSystemError: cannot load function '%s'", (mod_name + "_" + name).c_str());
		return function;
	}
}
