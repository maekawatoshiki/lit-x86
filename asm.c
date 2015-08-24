#include "asm.h"

void genCode(unsigned char val) { jitCode[jitCount++] = (val); }
void genCodeInt32(unsigned int val) {
	// for little endian
	genCode(val << 24 >> 24);
	genCode(val << 16 >> 24);
	genCode(val << 8 >> 24);
	genCode(val << 0 >> 24);
}
void genCodeInt32Insert(unsigned int val, int pos) {
	// for little endian
	jitCode[pos] = (val << 24 >> 24);
	jitCode[pos+1] = (val << 16 >> 24);
	jitCode[pos+2] = (val << 8 >> 24);
	jitCode[pos+3] = (val << 0 >> 24);
}

int regBit(char *reg) {
	if(strcmp("eax", reg) == 0) return EAX;
	if(strcmp("ebx", reg) == 0) return EBX;
	if(strcmp("ecx", reg) == 0) return ECX;
	if(strcmp("edx", reg) == 0) return EDX;
	if(strcmp("esp", reg) == 0) return ESP;
	if(strcmp("edi", reg) == 0) return EDI;
	if(strcmp("ebp", reg) == 0) return EBP;
	if(strcmp("esi", reg) == 0) return ESI;
	return -1;
}

int mk_modrm(char *r32, char *rm32) {
	int tmp = regBit(r32) * 8;

	if(isalpha(rm32[0])) { // register?
		tmp += 0xc0;
		tmp += regBit(rm32);
	} else if(isdigit(rm32[0])) { // integer?
	} else { }

	return tmp;
}

int genas(char *s, ...) {
	char *src = calloc(sizeof(char), strlen(s) + 0xff);
	va_list args;
	char nem[4][16] = { 0 };
	int i, n = 0, sz;
	va_start(args, s);
	vsprintf(src, s, args);
	sz = strlen(src) + 1;

	printf("src> %s\n", src);

	for(; *src; src++) {
		if(isalpha(*src) || isdigit(*src)) {
			for(; *src != ' ' && *src != 0; src++)
				strncat(nem[n], src, 1);
			src--; n++;
		}
	}// 0 3
	for(i = n = 0; i < 4; i++)
		printf("as> %s<\n", nem[i]);

	if(strcmp(nem[0], "mov") == 0) { // mov?
		if(isalpha(*nem[2])) { // register?
			genCode(0x89);
			genCode(0xc0 + regBit(nem[2]) * 8 + regBit(nem[1]));
		} else if(isdigit(*nem[2])) { // integer?
			genCode(0xb8 + regBit(nem[1]));
			genCodeInt32(atoi(nem[2]));
		} else {
		}
	} else if(strcmp(nem[0], "add") == 0) { // add
		if(isalpha(*nem[2])) { // register?
			genCode(0x03);
			genCode(mk_modrm(nem[1], nem[2]));
		} else if(isdigit(*nem[2])) { // integer?
			genCode(0x81);
			genCode(0xc0 + regBit(nem[1]));
			genCodeInt32(atoi(nem[2]));
		} else {
		}
	} else if(strcmp(nem[0], "sub") == 0) { // sub
		if(isalpha(*nem[2])) { // register?
			genCode(0x2b);
			genCode(mk_modrm(nem[1], nem[2]));
    } else if(isdigit(*nem[2])) { // integer?
  		genCode(0x81);
  		genCode(0xe8 + regBit(nem[1]));
  		genCodeInt32(atoi(nem[2]));
  	}
	} else if(strcmp(nem[0], "mul") == 0) { // mul
		if(isalpha(*nem[1])) { // register?
			genCode(0xf7);
			genCode(0xe0 + regBit(nem[1]));
		}
	} else if(strcmp(nem[0], "div") == 0) { // mul
		if(isalpha(*nem[1])) { // register?
			genCode(0xf7);
			genCode(0xf0 + regBit(nem[1]));
		}
	} else if(strcmp(nem[0], "push") == 0) { // mul
		if(isalpha(*nem[1])) { // register?
			genCode(0x50 + regBit(nem[1]));
		}
	} else if(strcmp(nem[0], "pop") == 0) { // mul
		if(isalpha(*nem[1])) { // register?
			genCode(0x58 + regBit(nem[1]));
		}
	}

	va_end(args);

	return 0;
}
