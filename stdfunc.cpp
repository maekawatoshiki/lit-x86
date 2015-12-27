#include "stdfunc.h"

std_function stdfunc[] = {
	{"Array", "", 1, 12},
	{"rand", "Math", 0, 16},
	{"gcd", "Math", 2, 60},
	{"lcm", "Math", 2, 64},
	{"printf", "", -1, 20},
	{"sleep", "Time", 1, 28},
	{"open", "File", 2, 32},
	{"write", "File", -1, 36},
	{"read", "File", 3, 44},
	{"close", "File", 1, 40},
	{"gets", "File", 3, 56},
	{"free", "Sys", 1, 48}
};

int make_stdfunc(std::string name, std::string mod_name) {
	for(int i = 0; i < sizeof(stdfunc) / sizeof(stdfunc[0]); i++) {
		if(stdfunc[i].name == name && stdfunc[i].mod_name == mod_name) {
			if(name == "Array") {
				expr_compare(); // get array size
				genas("shl eax 2");
				gencode(0x89); gencode(0x04); gencode(0x24); // mov [esp], eax
				gencode(0xff); gencode(0x56); gencode(12); // call malloc
				genas("push eax");
				gencode(0x89); gencode(0x04); gencode(0x24); // mov [esp], eax
				gencode(0xff); gencode(0x56); gencode(24); // call appendAddr
				genas("pop eax");
			} else {
				if(stdfunc[i].args == -1) { // vector
					uint32_t a = 0;
					do {
						expr_compare();
						gencode(0x89); gencode(0x44); gencode(0x24); gencode(a); // mov [esp+a], eax
						a += 4;
					} while(tok.skip(","));
				} else { // normal function
					for(int arg = 0; arg < stdfunc[i].args; arg++) {
						expr_compare();
						gencode(0x89); gencode(0x44); gencode(0x24); gencode(arg * 4); // mov [esp+arg*4], eax
						tok.skip(",");
					}
				}
				gencode(0xff); gencode(0x56); gencode(stdfunc[i].addr); // call $function
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
