#include "stdfunc.h"

std_function stdfunc[] = {
	{"Array", 1, 12},
	{"rand", 0, 16},
	{"printf", -1, 20},
	{"sleep", 1, 28},
	{"fopen", 2, 32},
	{"fprintf", -1, 36},
	{"fclose", 1, 40},
	{"fgets", 3, 44},
	{"free", 1, 48},
	{"freeLocal", 0, 52}
}; // len = sizeof(stdf) / sizeof(stdf[0])

int make_stdfunc(char *name) {
	for(int i = 0; i < sizeof(stdfunc) / sizeof(stdfunc[0]); i++) {
		if(strcmp(stdfunc[i].name, name) == 0) {
			if(strcmp(name, "Array") == 0) {
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
					} while(skip(","));
				} else {
					for(int arg = 0; arg < stdfunc[i].args; arg++) {
						expr_compare();
						gencode(0x89); gencode(0x44); gencode(0x24); gencode(arg * 4); // mov [esp+arg*4], eax
						skip(",");
					}
				}
				gencode(0xff); gencode(0x56); gencode(stdfunc[i].addr); // call $function
			}
			return 1;
		}
	}

	return 0;
}
