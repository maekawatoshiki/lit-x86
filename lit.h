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
#include "lex.h"
#include "expr.h"
#include "parse.h"
#include "stdfunc.h"
#include "option.h"

#if defined(WIN32) || defined(WINDOWS)
	#include <windows.h>
#else
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/mman.h>
	#include <sys/wait.h>
#endif

#define _LIT_VERSION_ "0.9.2"

enum {
	ADDR_SIZE = 4
};

enum {
	FALSE = 0,
	TRUE = 1
};

enum {
	BLOCK_NORMAL = 0,
	BLOCK_LOOP,
	BLOCK_FUNC,
	BLOCK_GLOBAL,
	NON
};

typedef struct {
  char val[32];
	int type;
	int nline;
} Token; 

struct {
	Token *tok;
	int size, pos;
} tok;

enum {
	V_LOCAL,
	V_GLOBAL
};

enum {
	T_INT,
	T_STRING,
	T_DOUBLE
};

struct {
	unsigned int *addr;
	int count;
} brks, rets;

void init();
void dispose();

int skip(char *);
int error(char *, ...);

static int execute(char *);

void lit_interpret();
void lit_run(char *);

/* for native(JIT) code. */

typedef struct {
	uint32_t addr;
	int isfree;
} mem_info;

struct {
	mem_info *mem;
	int count;
} mem;

void freeAddr();
void freeInProgram(uint32_t);
void putNumber(int);
void putString(int *);
void putln();
void appendAddr(uint32_t);

void set_xor128();
int  xor128();

#endif
