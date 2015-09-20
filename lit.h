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
	#include <sys/wait.h>
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
	int type;
	int id;
} Variable;

Variable varNames[0xFF][0xFF];
int varSize[0xFF], varCounter;

enum {
	T_INT,
	T_STRING,
	T_DOUBLE
};

int nowFunc; // number of function
int isFunction;

struct {
	int *addr;
	int count;
} Breaks, Returns;

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
static Variable *declareVariable();

static int ifStmt();
static int whileStmt();
static int functionStmt();

static int getString();
static int getFunction(char *, int);
static Variable *getVariable(char *);
static Variable *appendVariable(char *, int);
static int appendBreak();
static int appendReturn();

static int skip(char *);
static int error(char *, ...);
static char *replaceEscape(char *);


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
