#include "asm.h"

unsigned char *ntvCode;
int ntvCount;

void gencode(unsigned char val) { ntvCode[ntvCount++] = (val); }
void gencode_int32(unsigned int val) {
	// for little endian
	gencode(val << 24 >> 24);
	gencode(val << 16 >> 24);
	gencode(val << 8 >> 24);
	gencode(val << 0 >> 24);
}
void gencode_int32_insert(unsigned int val, int pos) {
	// for little endian
	ntvCode[pos] = (val << 24 >> 24);
	ntvCode[pos+1] = (val << 16 >> 24);
	ntvCode[pos+2] = (val << 8 >> 24);
	ntvCode[pos+3] = (val << 0 >> 24);
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

int genas(const char *s, ...) {
	char *src = (char *)calloc(strlen(s) + 0xff, sizeof(char));
	char nem[4][16] = { 0 };
	int i, n;
	va_list args;
	va_start(args, s);
	vsprintf(src, s, args);

	for(n = 0; *src; src++) {
		if(isalpha(*src) || isdigit(*src)) {
			for(; *src != ' ' && *src != 0; src++)
				strncat(nem[n], src, 1);
			src--; n++;
		}
	}


	if(strcmp(nem[0], "mov") == 0) { // mov?
		if(isalpha(*nem[2])) { // register?
			gencode(0x89);
			gencode(0xc0 + regBit(nem[2]) * 8 + regBit(nem[1]));
		} else if(isdigit(*nem[2])) { // integer?
			gencode(0xb8 + regBit(nem[1]));
			gencode_int32(atoi(nem[2]));
		} else {
		}
	} else if(strcmp(nem[0], "add") == 0) { // add
		if(isalpha(*nem[2])) { // register?
			gencode(0x03);
			gencode(mk_modrm(nem[1], nem[2]));
		} else if(isdigit(*nem[2])) { // integer?
			gencode(0x81);
			gencode(0xc0 + regBit(nem[1]));
			gencode_int32(atoi(nem[2]));
		} else {
		}
	} else if(strcmp(nem[0], "sub") == 0) { // sub
		if(isalpha(*nem[2])) { // register?
			gencode(0x2b);
			gencode(mk_modrm(nem[1], nem[2]));
    } else if(isdigit(*nem[2])) { // integer?
  		gencode(0x81);
  		gencode(0xe8 + regBit(nem[1]));
  		gencode_int32(atoi(nem[2]));
  	}
	} else if(strcmp(nem[0], "mul") == 0) { // mul
		if(isalpha(*nem[1])) { // register?
			gencode(0xf7);
			gencode(0xe8 + regBit(nem[1]));
		}
	} else if(strcmp(nem[0], "div") == 0) { // div
		if(isalpha(*nem[1])) { // register?
			gencode(0xf7);
			gencode(0xf8 + regBit(nem[1]));
		}
	} else if(strcmp(nem[0], "push") == 0) {
		if(isalpha(*nem[1])) { // register?
			gencode(0x50 + regBit(nem[1]));
		}
	} else if(strcmp(nem[0], "shl") == 0) {
		if(isalpha(*nem[1])) { // register?
			gencode(0xc1); gencode(0xe0 + regBit(nem[1]));
			gencode(atoi(nem[2]));
		}
	} else if(strcmp(nem[0], "shr") == 0) {
		if(isalpha(*nem[1])) { // register?
			gencode(0xc1); gencode(0xe8 + regBit(nem[1]));
			gencode(atoi(nem[2]));
		}
	} else if(strcmp(nem[0], "pop") == 0) {
		if(isalpha(*nem[1])) { // register?
			gencode(0x58 + regBit(nem[1]));
		}
	} else if(strcmp(nem[0], "call") == 0) {
		gencode(0xe8); gencode_int32(atoi(nem[1]));
	}

	va_end(args);

	return 0;
}
