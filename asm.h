#ifndef _LIT_ASM_
#define _LIT_ASM_

#include "lit.h"

enum {
	EAX = 0, ECX, EDX, EBX,
	ESP, EBP, ESI, EDI
};

void genCode(unsigned char);
void genCodeInt32(unsigned int);
void genCodeInt32Insert(unsigned int, int);

int regBit(char *reg);
int mk_modrm(char *, char *);
int genas(char *, ...);

#endif
