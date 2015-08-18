#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <iso646.h>
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

static unsigned char *jitCode;
int jitCount = 0;

struct Token {
  char val[32];
  int nline;
};
struct Token token[0xFFF] = { 0 }, formula[0xFFF] = { 0 };
int tkpos = 0, tksize, formpos = 0;

struct {
	char name[32];
	int size;
} varNames[0xFF][0x7F] = { 0 };
int varSize[0xFF] = { 0 }, varCounter = 0;

int nowFunc = 0;

int breaks[0xFF] = {0}; int breaksCount = 0;

int blocksCount = 0; // for while ~ end and if ~ end error check

struct {
	char val[0xFF];
} strings[0xFF] = { 0 };
int *stringsPos, stringsCount = 0;

static int addSubExpr();
static int mulDivExpr();
static int relExpr();
static int primExpr();
static int isassign();
static int assignment();
static int getString();
static int getNumOfVar(char *, int);
static void genCode(unsigned char);
static void genCodeInt32(unsigned int);
static void genCodeInt32Insert(unsigned int, int);
static void init();
static int skip(char *);
static int error(char *, ...);
static int lex(char *);
static int eval(int, int);
static int parser();

static int getString() {
	strcpy(strings[stringsCount].val, token[tkpos++].val);
	*stringsPos++ = jitCount;
	return stringsCount++;
}

static int getNumOfVar(char *name, int arraySize) {
	int i;
	for(i = 0; i < varCounter; i++)
	{
		if(strcmp(name, varNames[nowFunc][i].name) == 0)
			return varNames[nowFunc][i].size;
	}
	strcpy(varNames[nowFunc][varCounter].name, name);
	int sz = 1 + (varSize[nowFunc] += arraySize + 1);
	varNames[nowFunc][varCounter++].size = sz;
	return sz;
}

struct Function {
	int address;
	char name[0xFF];
};
struct Function functions[0xFF] = { 0 };
int funcCount = 0;

static int getFunction(char *name, int address) {
	int i;
	for(i = 0; i < funcCount; i++) {
		if(strcmp(functions[i].name, name) == 0) {
			return functions[i].address;
		}
	}
	functions[funcCount].address = address;
	strcpy(functions[funcCount].name, name);
	return functions[funcCount++].address;
}

static void genCode(unsigned char val) { jitCode[jitCount++] = (val); }
static void genCodeInt32(unsigned int val) {
	// for little endian
	genCode(val << 24 >> 24);
	genCode(val << 16 >> 24);
	genCode(val << 8 >> 24);
	genCode(val << 0 >> 24);
}
static void genCodeInt32Insert(unsigned int val, int pos) {
	// for little endian
	jitCode[pos] = (val << 24 >> 24);
	jitCode[pos+1] = (val << 16 >> 24);
	jitCode[pos+2] = (val << 8 >> 24);
	jitCode[pos+3] = (val << 0 >> 24);
}

static void init() {
	tkpos = jitCount = 0;
	memset(jitCode, 0, 0xFFF);
	memset(token, 0, sizeof(token));
}

static int skip(char *s) {
  if(strcmp(s, token[tkpos].val) == 0) {
  	tkpos++;
  	return 1;
  } else return 0;
}

static int error(char *errs, ...) {
	va_list args;
	va_start(args, errs);
	vprintf(errs, args); puts("");
	va_end(args);
	exit(0);
}

static int relExpr() {
	int lt=0, gt=0, diff=0, eql=0, fle=0;
	addSubExpr();
	if((lt=skip("<")) || (gt=skip(">")) || (diff=skip("!=")) ||
			(eql=skip("==")) || (fle=skip("<=")) || skip(">=")) {
		genCode(0x50); // push %eax
		addSubExpr();
		genCode(0x89); genCode(0xc3);  // mov %ebx %eax
		genCode(0x58); // pop %eax
		genCode(0x39); genCode(0xd8); // cmp %eax, %ebx
		return lt ? JB : gt ? JA : diff ? JNE : eql ? JE : fle ? JBE : JAE;
	}
}
static int addSubExpr() {
	int add;
	mulDivExpr();
	while((add = skip("+")) || skip("-")) {
		genCode(0x50); // push %eax
		mulDivExpr();
		genCode(0x89); genCode(0xc3);  // mov %ebx %eax
		genCode(0x58); // pop %eax
		if(add) { genCode(0x01); genCode(0xd8); }// add %eax %ebx
		else { genCode(0x29); genCode(0xd8); } // sub %eax %ebx
	}
}
static int mulDivExpr() {
  int mul, div;
  primExpr();
  while((mul = skip("*")) || (div=skip("/")) || skip("%")) {
    genCode(0x50); // push %eax
    primExpr();
    genCode(0x89); genCode(0xc3);  // mov %ebx %eax
    genCode(0x58); // pop %eax f7f3
    if(mul) {
			genCode(0x0f); genCode(0xaf); genCode(0xc3); // mul %eax %ebx
    } else if(div) {
      genCode(0xba); genCodeInt32(0); //mov edx 0
      genCode(0xf7); genCode(0xf3); // div %ebx
    } else { //mod
		  genCode(0xba); genCodeInt32(0); //mov edx 0
		  genCode(0xf7); genCode(0xf3); // div %ebx
      genCode(0x89); genCode(0xd0); // mov eax, edx
		}
  }
}
static int primExpr()
{
  if(isdigit(token[tkpos].val[0])) { // number?
    int n = atoi(token[tkpos].val);
      genCode(0xb8);
			genCodeInt32(n); // mov %eax num
    tkpos++;
  } else if(isalpha(token[tkpos].val[0])) { // variable?
		int varn = getNumOfVar(token[tkpos].val, 0);
		char *name = token[tkpos].val;
		printf("ar> %s\n", token[tkpos].val);
	  tkpos++;
		if(skip("[")) {
			relExpr();
			genCode(0x89); genCode(0xc2); // mov edx, eax
			genCode(0x8b); genCode(0x44); genCode(0x95);
			genCode(256 - sizeof(int) * varn); //mov eax, [ebp - 8 + (edx * 8)]
			if(!skip("]")) error("error: %d: expected expression ']'", token[tkpos].nline);
		} else if(skip("(")) {
			int address = getFunction(name, 0);
			printf("addr: %d\n", address);
			do {
				relExpr();
				genCode(0x50);
			} while(skip(","));
			genCode(0xe8); genCodeInt32(0xFFFFFFFF - (jitCount - address) - 3); // call func
			skip(")");
		} else {
			genCode(0x8b); genCode(0x45);
			genCode(256 - sizeof(int) * varn); // mov %eax variable
			printf("size: OK: %d\n", jitCount);
		}
	} else if(skip("]")) {
	   error("error: %d: invalid expression", token[tkpos].nline);
  } else if(skip("(")) {
    if(isassign()) assignment(); else relExpr();
    skip(")");
  } else if(!skip(";")) {
  	printf("prim>%s\n", token[tkpos].val);
		error("error: %d: invalid expression", token[tkpos].nline);
  }

	return 0;
}

static int lex(char *code)
{
  int i, codeSize = strlen(code), line = 1;
  for(i = 0; i < codeSize; i++) {
    if(isdigit(code[i])) { // number?
      for(; isdigit(code[i]); i++)
				strncat(token[tkpos].val, &(code[i]), 1);
			token[tkpos].nline = line;
      i--; tkpos++;
    } else if(isalpha(code[i])) { // ident?
      for(; isalpha(code[i]) || isdigit(code[i]) || code[i] == '_'; i++)
        strncat(token[tkpos].val, &(code[i]), 1);
      token[tkpos].nline = line;
      i--; tkpos++;
    } else if(code[i] == ' ' || code[i] == '\t') { // space char?
    } else if(code[i] == '#') { // comment?
			for(i++; code[i] != '\n'; i++); line++;
		} else if(code[i] == '"') { // string?
			strcpy(token[tkpos].val, "\"");
			token[tkpos++].nline = line;
			for(i++; code[i] != '"' && code[i] != '\0'; i++)
				strncat(token[tkpos].val, &(code[i]), 1);
			token[tkpos].nline = line;
			if(code[i] == '\0') error("error: %d: expected expression '\"'", token[tkpos].nline);
			tkpos++;
		} else { // symbol?
      if(code[i] == '\n') {
      	strcpy(token[tkpos].val, ";"); line++;
      } else strncat(token[tkpos].val, &(code[i]), 1);
			if(code[i+1] == '=')  strncat(token[tkpos].val, &(code[++i]), 1);
			token[tkpos].nline = line;
			tkpos++;
    }
  }
  token[tkpos].nline = line;
	for(i = 0; i < tkpos; i++) {
		printf("tk> %s\n", token[i].val);
	}
  tksize = tkpos;
}

static int isassign() {
	if(strcmp(token[tkpos+1].val, "=") == 0) return 1;
	else if(strcmp(token[tkpos+1].val, "[") == 0) {
		int i = tkpos + 1;
		while(strcmp(token[i].val, "]")) {
			if(strcmp(token[i].val, ";") == 0)
				error("error: %d: invalid expression", token[tkpos].nline);
			i++;
		}
		printf(">%s\n", token[i].val); i++;
		if(strcmp(token[i].val, "=") == 0) return 1;
	}
	return 0;
}

static int assignment() {
	int n = getNumOfVar(token[tkpos++].val, 0);
	if(skip("[")) {
		relExpr();
		genCode(0x89); genCode(0xc1); // mov ecx, eax
		if(!skip("]") || !skip("="))
			 error("error: %d: invalid assignment", token[tkpos].nline);
		relExpr();
		genCode(0x89); genCode(0x44); genCode(0x8d); genCode(256 - sizeof(int) * n); //mov [ebp - n + (ecx * n)], eax
	} else {
		skip("=");
		relExpr();
		genCode(0x89); genCode(0x45);
		genCode(256 - sizeof(int) * n); // mov var %eax
		printf("%d\n", tkpos);
	}
}

static int eval(int pos, int isloop) {
	int isputs = 0;
	while(tkpos < tksize) {
		if(isassign()) {
			assignment();
		} else if(skip("!")) {
			char *varname = token[tkpos++].val; if(!skip("["));
			int asize = atoi(token[tkpos++].val); if(!skip("]"));
			getNumOfVar(varname, asize); // add array variable
		} else if((isputs=skip("puts")) || skip("output")) {
			do {
				int isstring = 0;
				if(skip("\"")) {
					genCode(0xb8); getString();
					genCodeInt32(0x00); // mov eax string_address
					isstring = 1;
				} else {
					relExpr();
				}
				genCode(0x50); // push eax
				if(isstring) {
					genCode(0xff); genCode(0x56); genCode(0x04);// call *0x04(esi) putString
				} else {
					genCode(0xff); genCode(0x16);// call (esi) putNumber
				}
				genCode(0x58); // pop eax
			} while(skip(","));
			// for new line
			if(isputs) {
				genCode(0xff); genCode(0x56); genCode(0x08);// call *0x08(esi) putLN
			}
		} else if(skip("def")) {
			int espBgn, address, argsc = 0;
			char *funcName = token[tkpos++].val;
			nowFunc++;
			if(skip("(")) {
				do { getNumOfVar(token[tkpos++].val, 0); argsc++; } while(skip(","));
				skip(")");
			}
			getFunction(funcName, jitCount); // append function
			if(strcmp("main", funcName) == 0) {
				genCode(0x55);// push   %ebp
				genCode(0x89); genCode(0xe5);// mov    %esp,%ebp
				genCode(0x81); genCode(0xec); espBgn = jitCount; genCodeInt32(0); // sub %esp nn
				genCode(0x8b); genCode(0x75); genCode(0x0c); // mov 0xc(%ebp), esi
					eval(0, 0);
				genCode(0x81); genCode(0xc4); genCodeInt32(sizeof(int) * varSize[nowFunc] + 0x18); // add %esp nn
				genCode(0xc9);// leave
				genCode(0xc3);// ret
				genCodeInt32Insert(sizeof(int) * varSize[nowFunc] + 0x18, espBgn);
			} else {
				genCode(0x55);// push   %ebp
				genCode(0x89); genCode(0xe5);// mov    %esp,%ebp
				genCode(0x81); genCode(0xec); espBgn = jitCount; genCodeInt32(0x00); // sub %esp nn
				int argpos = jitCount, i; for(i = 0; i < argsc; i++) {
					genCode(0x8b); genCode(0x45); genCode(0x08 + i * 4);
					genCode(0x89); genCode(0x44); genCode(0x24); genCode(256 - 4 + 4);
				}
					eval(0, 0);
				genCode(0x81); genCode(0xc4); genCodeInt32(sizeof(int) * varSize[nowFunc]); // add %esp nn
				genCode(0xc9);// leave
				genCode(0xc3);// ret
				genCodeInt32Insert(sizeof(int) * varSize[nowFunc], espBgn);
				for(i = 0; i < argsc; i++) {
					jitCode[(argpos + 6) * (i+1)] = 256 - 4 * (i + 1) + (varSize[nowFunc] * 4 - 4);
				}
			}
			printf("%s() has %d byte\n", funcName, varSize[nowFunc] << 2);
		} else if(skip("return")) {
			relExpr();
			return 1;
		} else if(skip("while")) { blocksCount++;
			int loopBgn = jitCount, end;
			int type = relExpr();
			genCode(type); genCode(0x05);
			genCode(0xe9); end = jitCount; genCodeInt32(0);// jmp while end
			if(eval(0, 1)) {
				unsigned int n = 0xFFFFFFFF - jitCount + loopBgn - 4;
				genCode(0xe9);
				genCodeInt32(n);
				genCodeInt32Insert(jitCount - end - 4, end);
				for(--breaksCount; breaksCount >= 0; breaksCount--) {
					genCodeInt32Insert(jitCount - breaks[breaksCount] - 4, breaks[breaksCount]);
				} breaksCount = 0;
			}
		} else if(skip("if")) { blocksCount++;
			int loopBgn = jitCount, end;
			int type = relExpr();
			genCode(type); genCode(0x05);
			genCode(0xe9); end = jitCount; genCodeInt32(0);// jmp while end
			eval(end, 0);
		} else if(skip("else")) {
			int end;
			genCode(0xe9); end = jitCount; genCodeInt32(0);// jmp while end
			genCodeInt32Insert(jitCount - pos - 4, pos);
			eval(end, 0);
			return 1;
		} else if(skip("elsif")) {
			int endif, end;
			genCode(0xe9); endif = jitCount; genCodeInt32(0);// jmp while end
			genCodeInt32Insert(jitCount - pos - 4, pos);
			int type = relExpr();
			genCode(type); genCode(0x05);
			genCode(0xe9); end = jitCount; genCodeInt32(0);// jmp while end
			eval(end, 0);
			genCodeInt32Insert(jitCount - endif - 4, endif);
			return 1;
		} else if(skip("break")) {
			genCode(0xe9);
			breaks[breaksCount] = jitCount; genCodeInt32(0);
			breaksCount++;
		} else if(skip("end")) { blocksCount--;
			if(isloop == 0) {
				genCodeInt32Insert(jitCount - pos - 4, pos);
			}
			return 1;
		} else if(!skip(";")) {
			//error("error: %d: invalid expression", token[tkpos].nline);
			relExpr();
		}
	}
	if(blocksCount != 0)
		error("error: expected 'end' before %d line", token[tkpos].nline);
	return 0;
}

static char *replaceEscape(char *str)
{
	int i;
	char *pos;
	char escape[12][3] = {
		"\\a", "\a",
		"\\r", "\r",
		"\\f", "\f",
		"\\n", "\n",
		"\\t", "\t",
		"\\b", "\b"
	};
	for (i = 0; i < 12; i += 2)
	{
		while ((pos = strstr(str, escape[i])) != NULL)
		{
			*pos = escape[i + 1][0];
			memmove(pos + 1, pos + 2, strlen(str) - 2 + 1);
		}
	}
	return str;
}

static int parser() {
	tkpos = jitCount = 0;
	memset(jitCode, 0, 0xFFF);
	stringsPos = calloc(0xFF, sizeof(int));
	int main_address;
	genCode(0x55);// push   %ebp
	genCode(0x89); genCode(0xe5);// mov    %esp,%ebp
		genCode(0xe9); main_address = jitCount; genCodeInt32(0);
	genCode(0xc9);// leave
	genCode(0xc3);// ret

	eval(0, 0);

	int addr = getFunction("main", 0);
	printf(">%u\n", addr);
	genCodeInt32Insert(addr - 5, main_address);

	for(stringsPos--; stringsCount; stringsPos--) {
		genCodeInt32Insert(jitCount, *stringsPos);
		int i;
		replaceEscape(strings[--stringsCount].val);
		for(i = 0; strings[stringsCount].val[i]; i++) {
			genCode(strings[stringsCount].val[i]);
		} genCode(0); // '\0'
		printf("%d\n", stringsPos);
	}
	printf("memsz: %d\n", varSize);

	/*{
		FILE *out = fopen("asm.s", "wb");
			fwrite(jitCode, 1, jitCount, out);
		fclose(out);
	}*/
}

static void putNumber(int n) { printf("%d", n); }
static void putString(int n) { printf("%s", &(jitCode[n])); }
static void putLN() { printf("\n"); }
void *funcTable[] = { (void *) putNumber, (void*) putString, (void*) putLN };

int run() {
	int mem[0xFFFF] = { 0 };

	printf("size: %dbyte, %.2lf%%\n", jitCount, ((double)jitCount / 0xFFF) * 100.0);
	return ((int (*)(int *, void**))jitCode)(mem, funcTable);
}

int main(int argc, char **argv) {
	long psize;

#if defined(WIN32) || defined(WINDOWS)
	jitCode = (unsigned char*) malloc(0xFFF);
#else
	psize = sysconf(_SC_PAGE_SIZE);
	if((posix_memalign((void **)&jitCode, psize, psize)))
		perror("posix_memalign");
	if(mprotect((void*)jitCode, psize, PROT_READ | PROT_WRITE | PROT_EXEC))
		perror("mprotect");
#endif

	init();

	if(argc < 2) {
		char input[0xFFFF] = "";
		char line[0xFF] = "";

		while(strcmp(line, "run\n") != 0) {
			strcat(input, line);
			memset(line, 0, 0xFF);
			fgets(line, 0xFF, stdin);
		}
		lex(input);
	} else {
		FILE *codefp = fopen(argv[1], "rb");
		int sourceSize = 0;

		if(!codefp) { perror("file not found"); exit(0); }
		fseek(codefp, 0, SEEK_END);
		sourceSize = ftell(codefp);
		fseek(codefp, 0, SEEK_SET);
		char *source = calloc(sizeof(char), sourceSize + 2);
		fread(source, sizeof(char), sourceSize, codefp);
		lex(source);
	}

	parser();
	clock_t bgn = clock();
	run();
	clock_t end = clock();
	printf("time: %lfsec\n", (double)(end - bgn) / CLOCKS_PER_SEC);
	free(jitCode);
}

/*
: prime

i = 2
while i < 1000000
	isprime = 1
	if i == 2
		print i
	elsif i % 2 == 0
	else
		k = 3
		while k * k <= i
			if i % k == 0
				isprime = 0
				break
			end
			k = k + 2
		end
		if isprime == 1
			print i
		end
	end
	i = i + 1
end
*/
