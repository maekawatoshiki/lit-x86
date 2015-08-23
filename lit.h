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

enum {
	EAX = 0, ECX, EDX, EBX,
	ESP, EBP, ESI, EDI
};

static void init();
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

static int getString();
static int getFunction(char *, int);
static int getNumOfVar(char *, int);

static void genCode(unsigned char);
static void genCodeInt32(unsigned int);
static void genCodeInt32Insert(unsigned int, int);
static int regBit(char *reg);
static int mk_modrm(char *, char *);
static int genas(char *, ...);

static int skip(char *);
static int error(char *, ...);
static char *replaceEscape(char *);

static unsigned char *jitCode;
int jitCount = 0;

struct Token {
  char val[32];
  int nline;
};
struct Token token[0xFFF] = { 0 };
int tkpos = 0, tksize = 0;

struct {
	char name[32];
	int size;
} varNames[0xFF][0x7F] = { 0 };
int varSize[0xFF] = { 0 }, varCounter = 0;

int nowFunc = 0; // number of function

int breaks[0xFF] = {0}; int brkCount = 0;

int blocksCount = 0; // for while ~ end and if ~ end error check

struct Function {
	int address;
	char name[0xFF];
};
struct Function functions[0xFF] = { 0 };
int funcCount = 0;

struct {
	char val[0xFF];
} strings[0xFF] = { 0 };
int *stringsPos, stringsCount = 0; // strings in program