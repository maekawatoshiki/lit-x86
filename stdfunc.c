#include "stdfunc.h"

typedef struct {
	char *name;
	int args, addr;
} std_function;

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
	{"freeMem", 0, 52}
}; // len = sizeof(stdf) / sizeof(stdf[0])

int make_stdfunc(char *name) {
	for(int i = 0; i < sizeof(stdfunc) / sizeof(stdfunc[0]); i++) {
		if(strcmp(stdfunc[i].name, name) == 0) {
			if(strcmp(name, "Array") == 0) {
				relExpr(); // get array size
				genas("shl eax 2");
				genCode(0x89); genCode(0x04); genCode(0x24); // mov [esp], eax
				genCode(0xff); genCode(0x56); genCode(12); // call malloc
				genas("push eax");
				genCode(0x89); genCode(0x04); genCode(0x24); // mov [esp], eax
				genCode(0xff); genCode(0x56); genCode(24); // call appendMem
				genas("pop eax");
			} else {
				if(stdfunc[i].args == -1) { // vector
					uint32_t a = 0;
					do {
						relExpr();
						genCode(0x89); genCode(0x44); genCode(0x24); genCode(a); // mov [esp+a], eax
						a += 4;
					} while(skip(","));
				} else {
					for(int arg = 0; arg < stdfunc[i].args; arg++) {
						relExpr();
						genCode(0x89); genCode(0x44); genCode(0x24); genCode(arg * 4); // mov [esp+arg*4], eax
						skip(",");
					}
				}
				genCode(0xff); genCode(0x56); genCode(stdfunc[i].addr); // call $function
			}
			return 1;
		}
	}

	return 0;
}
