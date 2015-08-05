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

unsigned char *jitCode;
int jitCount = 0;

struct {
  char val[32];
} token[0xFFF] = { 0 };
int tkpos = 0, tksize;

struct {
	char name[32];
} varNames[0x7F] = { 0 };
int varCount = 0;

int getNumOfVar(char *name) {
	int i;
	for(i = 0; i < varCount; i++)
	{
		if(strcmp(name, varNames[i].name) == 0)
			return i + 1;
	}
	strcpy(varNames[varCount].name, name);
	return 1 + varCount++;
}

void genCode(unsigned char val) { jitCode[jitCount++] = (val); }
void genCodeInt32(unsigned int val) {
	genCode(val << 24 >> 24);
	genCode(val << 16 >> 24);
	genCode(val << 8 >> 24);
	genCode(val << 0 >> 24);
}
void genCodeInt32Insert(unsigned int val, int pos) {
	jitCode[pos] = (val << 24 >> 24);
	jitCode[pos+1] = (val << 16 >> 24);
	jitCode[pos+2] = (val << 8 >> 24);
	jitCode[pos+3] = (val << 0 >> 24);
}

void init() {
	tkpos = jitCount = 0;
	memset(jitCode, 0, 0xFFF);
	memset(token, 0, sizeof(token));
}

int lex(char *code)
{
  int i, codeSize = strlen(code);
  for(i = 0; i < codeSize; i++) {
    if(isdigit(code[i])) {
      for(; isdigit(code[i]); i++)
				strncat(token[tkpos].val, &(code[i]), 1);
      i--; tkpos++;
    } else if(isalpha(code[i])) {
      for(; isalpha(code[i]); i++)
        strncat(token[tkpos].val, &(code[i]), 1);
      i--; tkpos++;
    } else if(code[i] == ' ' || code[i] == '\t' || code[i] == '\n') {
    } else {
      strncat(token[tkpos].val, &(code[i]), 1);
			if(code[i+1] == '=')  strncat(token[tkpos].val, &(code[++i]), 1);
			tkpos++;
    }
  }
  tksize = tkpos;
}

int skip(char *s) {
  if(strcmp(s, token[tkpos].val) == 0) {
  	tkpos++;
  	return 1;
  } else return 0;
}

/*** prime ***
p = 2; while p < 1000000 k = 2; isp = 1; while k*k <= p; if p % k == 0 isp = 0 break end  k = k + 1; end if isp == 1 print p end p = p + 1; end
***
i = 0; while i < 10; k = 0; while k < 10; j = 0; while j < 10; print j j = j + 1 end k = k + 1 end i = i + 1 end
i=0; while i<10 j=0 while j<10 print j j=j+1 end print i i=i+1; end
i = 0; sum = 0; while i < 100000001; sum = sum + i;  i = i + 1 end print sum
n-6
i = 0; while i < 100 if i % 2 == 0; print 100 end print i i = i + 1 end
n = 1482432730 while n != 1 if i % 2 == 0 n = n / 2 end if i % 2 != 0 n = n * 3 + 1 end print n end
i = 1 sum = 0  while i < 5 print 22  i = i + 1 end
*/

int breaks[0xFF] = {0}; int breaksCount = 0;

int eval(int pos, int isloop)
{
	while(tkpos < tksize)
	{
		skip(";");
		if(strcmp(token[tkpos+1].val, "=") == 0) {
			int  n = getNumOfVar(token[tkpos++].val);
			skip("=");
			relExpr();
			genCode(0x89); genCode(0x45);
			genCode(256 - sizeof(int) * n); // mov var %eax
		} else if(skip("print")) {
			relExpr();
			genCode(0x89); genCode(0x04); genCode(0x24); // mov esp, eax
			genCode(0xff); genCode(0x16);// call (esi)
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

int parser()
{
	int espBgn;
	tkpos = jitCount = 0;
	genCode(0x55);// push   %ebp
	genCode(0x89); genCode(0xe5);// mov    %esp,%ebp
	genCode(0x8b); genCode(0x75); genCode(0x0c);
	genCode(0x83); genCode(0xec); espBgn = jitCount; genCode(sizeof(int)); // sub %esp nn

	eval(0, 0);

	genCode(0x83); genCode(0xc4); genCode(sizeof(int) * varCount + 0x18); // add %esp nn
	genCode(0x5d);// pop %ebp
	genCode(0xc3);// ret
	jitCode[espBgn] = sizeof(int) * varCount + 0x18;
}

int relExpr()
{
	int lt=0, gt=0, diff=0, eql=0, fle=0;
	addSubExpr();
	if((lt=skip("<")) || (gt=skip(">")) || (diff=skip("!=")) ||
			(eql=skip("==")) || (fle=skip("<=")) || skip(">="))
	{
		genCode(0x50); // push %eax
		addSubExpr();
		genCode(0x89); genCode(0xc3);  // mov %ebx %eax
		genCode(0x58); // pop %eax
		genCode(0x39); genCode(0xd8); // cmp %eax, %ebx
		return lt ? JB : gt ? JA : diff ? JNE : eql ? JE : fle ? JBE : JAE;
	}
}

int addSubExpr()
{
	int add;
	mulDivExpr();
	while((add = skip("+")) || skip("-"))
	{
		genCode(0x50); // push %eax
		mulDivExpr();
		genCode(0x89); genCode(0xc3);  // mov %ebx %eax
		genCode(0x58); // pop %eax
		if(add) { genCode(0x01); genCode(0xd8); }// add %eax %ebx
		else { genCode(0x29); genCode(0xd8); } // sub %eax %ebx
	}
}
int mulDivExpr()
{
  int mul, div;
  primExpr();
  while((mul = skip("*")) || (div=skip("/")) || skip("%"))
  {
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
int primExpr()
{
  if(isdigit(token[tkpos].val[0]))
  {
    int n = atoi(token[tkpos].val);
      genCode(0xb8);
			genCodeInt32(n); // mov %eax num
    tkpos++;
  } else if(isalpha(token[tkpos].val[0])) {
	  genCode(0x8b); genCode(0x45);
		genCode(256 - sizeof(int) * getNumOfVar(token[tkpos].val)); // mov %eax variable
	  tkpos++;
		if(skip("[")) {
			// TODO: ...
			relExpr();
			genCode(0x89); genCode(0xc2); // mov edx, eax
			skip("]");
		}
	} else if(skip("(")) {
    addSubExpr();
    skip(")");
  }
}

int putNumber(int n) { printf("%d\n", n); }
void *funcTable[] = {(void *)putNumber};

int run()
{
	printf("size: %dbyte, %.2lf%%\n", jitCount, ((double)jitCount/4098)*100.0);

	void *funcTable[] = {(void *)putNumber};
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
		char *source; int sourceSize = 0;

		fseek(codefp, 0, SEEK_END);
		sourceSize = ftell(codefp);
		fseek(codefp, 0, SEEK_SET);
		source = calloc(sizeof(char), sourceSize + 1);
		fread(source, sizeof(char), sourceSize, codefp);
		lex(source);
	}

	parser();
	clock_t bgn = clock();
	run();
	printf("time: %lfsec\n", (double)(clock() - bgn) / CLOCKS_PER_SEC);
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
