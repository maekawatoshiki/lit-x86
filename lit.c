#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#if defined(WIN32) || defined(WINDOWS)
	#include <windows.h>
#else
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/mman.h>
#endif

#define JA 0x77
#define JB 0x72
#define JE 0x74
#define JNE 0x75
#define JBE 0x76
#define JAE 0x73

static unsigned char *jitCode;
int jitCount = 0;

struct Token {
  char val[32];
};
struct Token token[0xFFF] = { 0 }, formula[0xFFF] = { 0 };
int tkpos = 0, tksize, formpos = 0;

struct {
	char name[32];
} varNames[0x7F] = { 0 };
int varCount = 0;

int breaks[0xFF] = {0}; int breaksCount = 0;

struct Token strings[0xFF] = { 0 };
int *stringsPos, stringsCount = 0;

static int getString() {
	strcpy(strings[stringsCount].val, token[tkpos++].val);
	*stringsPos++ = jitCount;
	return stringsCount++;
}

static int getNumOfVar(char *name) {
	int i;
	for(i = 0; i < varCount; i++)
	{
		if(strcmp(name, varNames[i].name) == 0)
			return i + 1;
	}
	strcpy(varNames[varCount].name, name);
	return 1 + varCount++;
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

static int addSubExpr();
static int mulDivExpr();
static int relExpr();
static int primExpr();

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
  if(isdigit(token[tkpos].val[0])) // number?
  {
    int n = atoi(token[tkpos].val);
      genCode(0xb8);
			genCodeInt32(n); // mov %eax num
    tkpos++;
  } else if(isalpha(token[tkpos].val[0])) { // variable?
	  genCode(0x8b); genCode(0x45);
		genCode(256 - sizeof(int) * getNumOfVar(token[tkpos].val)); // mov %eax variable
	  tkpos++;
		if(skip("[")) {
			relExpr();
			genCode(0x89); genCode(0xc2); // mov edx, eax
			skip("]");
		}
	} else if(skip("(")) {
    addSubExpr();
    skip(")");
  }
}

static int lex(char *code)
{
  int i, codeSize = strlen(code);
  for(i = 0; i < codeSize; i++) {
    if(isdigit(code[i])) { // number?
      for(; isdigit(code[i]); i++)
				strncat(token[tkpos].val, &(code[i]), 1);
      i--; tkpos++;
    } else if(isalpha(code[i])) { // ident?
      for(; isalpha(code[i]) || isdigit(code[i]); i++)
        strncat(token[tkpos].val, &(code[i]), 1);
      i--; tkpos++;
    } else if(code[i] == ' ' || code[i] == '\t' || code[i] == '\n') { // space char?
    } else if(code[i] == '#') { // comment?
			for(i++; code[i] != '\n'; i++);
		} else if(code[i] == '"') { // string?
			strcpy(token[tkpos++].val, "\"");
			for(i++; code[i] != '"'; i++)
				strncat(token[tkpos].val, &(code[i]), 1);
			tkpos++;
		} else { // symbol?
      strncat(token[tkpos].val, &(code[i]), 1);
			if(code[i+1] == '=')  strncat(token[tkpos].val, &(code[++i]), 1);
			tkpos++;
    }
  }
	for(i = 0; i < tkpos; i++) {
		printf("tk> %s\n", token[i].val);
	}
  tksize = tkpos;
}

static int eval(int pos, int isloop) {
	while(tkpos < tksize) {
		skip(";");
		if(strcmp(token[tkpos+1].val, "=") == 0) {
			int  n = getNumOfVar(token[tkpos++].val);
			skip("=");
			relExpr();
			genCode(0x89); genCode(0x45);
			genCode(256 - sizeof(int) * n); // mov var %eax
		} else if(skip("print")) {
			relExpr();
			genCode(0x50); // push eax
			genCode(0xff); genCode(0x16);// call (esi)
			genCode(0x58); // pop eax
		} else if(skip("puts")) {
			do {
				int isstring = 0;
				if(skip("\"")) {
					genCode(0xb8);
					getString();
					genCodeInt32(0); // mov eax string_address
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
		} else if(skip("while")) {
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
		} else if(skip("if")) {
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
		} else if(skip("end")) {
			if(isloop == 0) {
					genCodeInt32Insert(jitCount - pos - 4, pos);
			}
			return 1;
		} else { relExpr(); }
	}

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
	int espBgn;
	tkpos = jitCount = 0;
	memset(jitCode, 0, 0xFFF);
	stringsPos = calloc(0xFF, sizeof(int));

	genCode(0x55);// push   %ebp
	genCode(0x89); genCode(0xe5);// mov    %esp,%ebp
	genCode(0x8b); genCode(0x75); genCode(0x0c);
	genCode(0x83); genCode(0xec); espBgn = jitCount; genCode(sizeof(int)); // sub %esp nn

	eval(0, 0);

	genCode(0x83); genCode(0xc4); genCode(sizeof(int) * varCount + 0x18); // add %esp nn
	genCode(0x5d);// pop %ebp
	genCode(0xc3);// ret
	for(stringsPos--; *stringsPos; stringsPos--) {
		genCodeInt32Insert(jitCount, *stringsPos);
		int i;
		replaceEscape(strings[--stringsCount].val);
		for(i = 0; strings[stringsCount].val[i]; i++) {
			genCode(strings[stringsCount].val[i]);
		} genCode(0); // '\0'
	}
	jitCode[espBgn] = sizeof(int) * varCount + 0x18;
}

static int mem[0xFF]={0}, sp=0;
static int cal_push(int n) { mem[sp++] = n; return n; }
static int cal_pop() { return mem[--sp]; }

static int calculate() {
	int i;
	relExpr();
	for(i = 0; i < formpos; i++) {
		int op = formula[i].val[0];
		char *opc = formula[i].val;
		if(isdigit(op)) {
			cal_push(atoi(opc));
		} else if(isalpha(op)) {

		} else if(op=='+' || op=='-' || op=='*' || op=='/' || op=='%') {

		} else if(strcmp(opc,"<") || strcmp(opc,">") || strcmp(opc,"<=") || strcmp(opc,">=") ||
			strcmp(opc,"==") || strcmp(opc,"!=")) {

		}
	}

}

static int putNumber(int n) { return printf("%d", n); }
static int putString(int n) { return printf("%s", &(jitCode[n])); }

static int run() {
	static void *funcTable[] = { (void *) putNumber, (void*) putString };

	printf("size: %dbyte, %.2lf%%\n", jitCount, ((double)jitCount / 0xFFF) * 100.0);
	return ((int (*)(int *, void**))jitCode)(0, funcTable);
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
