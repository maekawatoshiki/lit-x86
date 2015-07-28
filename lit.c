#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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

unsigned char *jitcode;
int jitCount = 0;

struct {
  char val[32];
} token[0xFFF] = { 0 };
int tkpos = 0, tksize;

struct {
	char name[32];
} varNames[0x7F] = { 0 };
int varCount = 0;

int getNumOfVar(char *name)
{
	int i;
	for(i = 0; i < varCount; i++)
	{
		if(strcmp(name, varNames[i].name) == 0)
			return i + 1;
	}
	strcpy(varNames[varCount].name, name);
	return 1 + varCount++;
}

void genCode(int val) { jitcode[jitCount++] = (val); }

int lex(char *code)
{
  int i, codeSize = strlen(code);
  for(i = 0; i < codeSize; i++)
  {
    if(isdigit(code[i]))
    {
      for(; isdigit(code[i]); i++)
        strncat(token[tkpos].val, &(code[i]), 1);
      i--; tkpos++;
    } else if(isalpha(code[i]))
    {
      for(; isalpha(code[i]); i++)
        strncat(token[tkpos].val, &(code[i]), 1);
      i--; tkpos++;
    } else if(code[i] == ' ' || code[i] == '\t' || code[i] == '\n') {
    } else {
      strncat(token[tkpos].val, &(code[i]), 1);
			if(code[i+1] == '=')  strncat(token[tkpos].val, &(code[++i]), 1);
			tkpos++;
    }
		printf("tk>%s\n", token[tkpos-1].val);
  }
  tksize = tkpos;
}

int skip(char *s)
{
  if(strcmp(s, token[tkpos].val) == 0) { tkpos++; return 1; }
  else return 0;
}

int eval()
{
	while(tkpos < tksize)
	{
		skip(";");
		if(strcmp(token[tkpos+1].val, "=") == 0) {
			char varnm[32]="";
			strcpy(varnm, token[tkpos++].val);
			skip("=");
			relExpr();
			genCode(0x89); genCode(0x45);
			genCode(256 - sizeof(int) * getNumOfVar(varnm)); // mov var %eax
		} else if(skip("if")) {
			int type = relExpr(), ifBgn;
			genCode(type); genCode(0x02);// jne jb je ja  label
			genCode(0xeb); ifBgn = jitCount; genCode(0);//todo
			if(eval()) {
				jitcode[ifBgn] = jitCount - ifBgn-1;
			}
		} else if(skip("do")) {
			int loopBgn = jitCount;
			if(eval()) {
				int type = relExpr();
				/* jne jb ja ... label */
				genCode(type); genCode(256 - (jitCount - loopBgn)-1);// jne label
			}
		} else if(skip("while") || skip("end")) { return 1; }
		else { relExpr(); }
	}

	return 0;
}

int parser()
{
	int espBgn;
	tkpos = jitCount = 0;
	genCode(0x55);// push   %ebp
	genCode(0x89); genCode(0xe5);// mov    %esp,%ebp
	genCode(0x83); genCode(0xec); espBgn = jitCount; genCode(sizeof(int) * 16); // sub %esp 0x04

	eval();

	genCode(0x83); genCode(0xc4); genCode(sizeof(int) * varCount); // add %esp 0x04
	genCode(0x5d);// pop %ebp
	genCode(0xc3);// ret
	jitcode[espBgn] = sizeof(int) * varCount;
}

int relExpr()
{
	int lt=0, gt=0, diff;
	addSubExpr();
	if((lt=skip("<")) || (gt=skip(">")) || (diff=skip("!=")) || skip("=="))
	{
		genCode(0x50); // push %eax
		addSubExpr();
		genCode(0x89); genCode(0xc3);  // mov %ebx %eax
		genCode(0x58); // pop %eax
		genCode(0x39); genCode(0xd8); // cmp %eax, %ebx
		return lt ? JB : gt ? JA : diff ? JNE : JE;
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
    if(mul) { genCode(0x0f); genCode(0xaf); genCode(0xc3); }// mul %eax %ebx
    else if(div) {
      genCode(0xba); // mov %edx 0
      genCode(0); genCode(0); genCode(0); genCode(0);
      genCode(0xf7); genCode(0xf3); // div %ebx
    } else { //mod
			genCode(0xba); // mov %edx 0
      genCode(0); genCode(0); genCode(0); genCode(0);
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
      genCode(0xb8); // mov %eax
      genCode(n << 24 >> 24);
      genCode(n << 16 >> 24);
      genCode(n << 8 >> 24);
      genCode(n << 0 >> 24); // mov %eax num
    tkpos++;
  } else if(isalpha(token[tkpos].val[0])) {
	    genCode(0x8b); genCode(0x45);
			genCode(256 - sizeof(int) * getNumOfVar(token[tkpos].val)); // mov %eax variable
	    tkpos++;
	} else if(skip("(")) {
    addSubExpr();
    skip(")");
  }
}
int run()
{
	return ((int (*)(void**))jitcode)(0);
}

int main()
{
	long psize;

#if defined(WIN32) || defined(WINDOWS)
	#include <windows.h>
#else
	psize = sysconf(_SC_PAGE_SIZE);
	if((posix_memalign((void **)&jitcode, psize, psize)))
		perror("posix_memalign");
	if(mprotect((void*)jitcode, psize, PROT_READ | PROT_WRITE | PROT_EXEC))
		perror("mprotect");
#endif

	memset(jitcode, 0, psize);

	char input[0xFFF] = { 0 };
	fgets(input, 0xFFF, stdin);

	lex(input);
	parser();
	clock_t bgn = clock();
	printf("%d\n", run());
	printf("time: %lfsec\n", (double)(clock() - bgn) / CLOCKS_PER_SEC);
}
