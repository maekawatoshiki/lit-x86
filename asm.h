#ifndef _LIT_ASM_
#define _LIT_ASM_

#include "common.h"

class NativeCode_x86 {
private:
	enum {
		EAX = 0, ECX, EDX, EBX,
		ESP, EBP, ESI, EDI
	};

	int regBit(char *reg);
	int mk_modrm(char *, char *);

public:
	unsigned char *code;
	int count;

	NativeCode_x86();
	~NativeCode_x86();
	void gencode(unsigned char);
	void gencode_int32(unsigned int);
	void gencode_int32_insert(unsigned int, int);

	int genas(const char *, ...);
};

extern NativeCode_x86 ntv;

#endif
