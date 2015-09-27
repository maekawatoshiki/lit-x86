#ifndef _LIT_MAIN_HEAD_
#define _LIT_MAIN_HEAD_

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "asm.h"
#include "expr.h"
#include "parse.h"

#if defined(WIN32) || defined(WINDOWS)
	#include <windows.h>
#else
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/mman.h>
	#include <sys/wait.h>
#endif

#define JA 0x7f
#define JB 0x7c
#define JE 0x74
#define JNE 0x75
#define JBE 0x7e
#define JAE 0x7d

#define NON 0
#define ONE_SENT 1

enum {
	IN_FUNC = 1,
	IN_GLOBAL = 0
};

enum {
	BLOCK_LOOP = 1,
	BLOCK_FUNC ,
	BLOCK_GLOBAL
};

unsigned char *ntvCode;
int ntvCount;

typedef struct {
  char val[32];
  int nline;
} Token;

Token *tok;
int tkpos, tksize;

enum {
	V_LOCAL,
	V_GLOBAL
};

enum {
	T_INT,
	T_STRING,
	T_DOUBLE
};

int nowFunc; // number of function
int isFunction;

struct {
	unsigned int *addr;
	int count;
} brks, rets;

int blocksCount; // for while ~ end and if ~ end error check

typedef struct {
	int address;
	char name[0xFF];
} Function;

Function functions[0xFF];
int funcCount;

char strings[0xFF][0xFF];
int *strAddr, strCount; // strings in program

int isFunction; // With in function?

void init();
void dispose();

int lex(char *);


int skip(char *);
int error(char *, ...);
char *replaceEscape(char *);

int execute(char *);

/* for native(JIT) code. */

struct {
	int addr[0xff];
	int count;
} memad;

void freeMem();
void putNumber(int);
void putString(int *);
void putln();
void appendMem(int);
void freeMem();

unsigned int w;
void set_xor128();
int  xor128();

#endif
