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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#if defined(WIN32) || defined(WINDOWS)
	#include <windows.h>
#else
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/mman.h>
#endif

#define JA 0x7f
#define JB 0x7c
#define JE 0x74
#define JNE 0x75
#define JBE 0x7e
#define JAE 0x7d

#define NON 0

#define IN_FUNC 1
#define IN_GLOBAL 0

#define BLOCK_LOOP 1
#define BLOCK_FUNC 2

static void init();
static void dispose();

static int lex(char *);
static int eval(int, int);
static int parser();

static int addSubExpr();
static int mulDivExpr();
static int relExpr();
static int primExpr();

static int isassign();
static int assignment();

static int ifStmt();
static int whileStmt();
static int functionStmt();

static int getString();
static int getFunction(char *, int);
static int getVariable(char *);
static int appendVariable(char *);
static int appendBreak();

static int skip(char *);
static int error(char *, ...);
static char *replaceEscape(char *);

unsigned char *ntvCode;
int ntvCount;

typedef struct {
  char val[32];
  int nline;
} Token;

Token *tok;
int tkpos, tksize;

typedef struct {
	char name[32];
	int id;
} VarNameTable;

VarNameTable varNames[0xFF][0xFF];
int varSize[0xFF], varCounter;

int nowFunc; // number of function
int isFunction;

int *breaks; int brkCount;

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

#endif
