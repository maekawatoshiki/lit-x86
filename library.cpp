#include "library.h"
#include "lit.h"

LibraryList lib_list;

LibraryList::~LibraryList() {
	for(int i = 0; i < lib.size(); i++) {
		dlclose(lib[i].handle);
	}
}

int LibraryList::append(std::string name) {
	lib_t l = {
		.name = name,
		.no = lib.size(),
		.handle = dlopen(("./lib/" + name + ".so").c_str(), RTLD_LAZY | RTLD_NOW)
	};

	lib.push_back(l);
	return lib.size() - 1;
}

bool LibraryList::is(std::string mod_name) {
	return get(mod_name) == NULL ? 0 : 1;
}

lib_t *LibraryList::get(std::string name) {
	for(int i = 0; i < lib.size(); i++) {
		if(lib[i].name == name) {
			return &lib[i];
		}
	}
	return NULL;
}

uint32_t LibraryList::call(std::string name, std::string mod_name) {
	return (uint32_t)dlsym(get(mod_name)->handle, (mod_name + "_" + name).c_str());
}
