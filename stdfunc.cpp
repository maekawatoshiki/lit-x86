#include "stdfunc.h"

std_function stdfunc[] = {
	{"Array", "", 1, 12},
	{"printf", "", -1, 16},
	{"sleep", "Time", 1, 24},
	{"open", "File", 2, 28},
	{"write", "File", -1, 32},
	{"read", "File", 3, 40},
	{"close", "File", 1, 36},
	{"gets", "File", 3, 52},
	{"free", "Sys", 1, 44}
};

int make_stdfunc(std::string name, std::string mod_name) {
	for(int i = 0; i < sizeof(stdfunc) / sizeof(stdfunc[0]); i++) {
		if(stdfunc[i].name == name && stdfunc[i].mod_name == mod_name) {
			if(name == "Array") {
				parse.expr_compare(); // get array size
				ntv.genas("shl eax 2");
				ntv.gencode(0x89); ntv.gencode(0x04); ntv.gencode(0x24); // mov [esp], eax
				ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(12); // call malloc
				ntv.genas("push eax");
				ntv.gencode(0x89); ntv.gencode(0x04); ntv.gencode(0x24); // mov [esp], eax
				ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(20); // call append_addr
				ntv.genas("pop eax");
			} else {
				if(stdfunc[i].args == -1) { // vector
					uint32_t a = 0;
					do {
						parse.expr_compare();
						ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(a); // mov [esp+a], eax
						a += 4;
					} while(tok.skip(","));
				} else { // normal function
					for(int arg = 0; arg < stdfunc[i].args; arg++) {
						parse.expr_compare();
						ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(arg * 4); // mov [esp+arg*4], eax
						tok.skip(",");
					}
				}
				ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(stdfunc[i].addr); // call $function
			}
			return 1;
		}
	}

	return 0;
}

int is_stdfunc(std::string name, std::string mod_name) {
	for(int i = 0; i < sizeof(stdfunc) / sizeof(stdfunc[0]); i++) {
		if(stdfunc[i].name == name && stdfunc[i].mod_name == mod_name)
			return 1;
	}

	return 0;
}
