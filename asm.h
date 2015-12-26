#ifndef _LIT_ASM_
#define _LIT_ASM_

#include "lit.h"

enum {
	EAX = 0, ECX, EDX, EBX,
	ESP, EBP, ESI, EDI
};

extern unsigned char *ntvCode;
extern int ntvCount; 

void gencode(unsigned char);
void gencode_int32(unsigned int);
void gencode_int32_insert(unsigned int, int);

int regBit(char *reg);
int mk_modrm(char *, char *);
int genas(char *, ...);

#endif
