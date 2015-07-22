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

unsigned char *jitcode;
int jitCount = 0;

struct Token {
  char val[32];
} token[0xFFF] = { 0 };
int tkpos = 0, tksize;

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
    } else if(code[i] == ' ' || code[i] == '\t') {
    } else {
      strncat(token[tkpos++].val, &(code[i]), 1);
    }
  }
  tksize = tkpos;
}

int skip(char *s)
{
  if(strcmp(s, token[tkpos].val) == 0) { tkpos++; return 1; }
  else return 0;
}

int parser()
{
	tkpos = jitCount = 0;
	genCode(0x55);// push   %ebp
	genCode(0x89); genCode(0xe5);// mov    %esp,%ebp
	addSubExpr();
	genCode(0x5d);// pop %ebp
	genCode(0xc3);// ret
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
  int mul;
  primExpr();
  while((mul = skip("*")) || skip("/"))
  {
    genCode(0x50); // push %eax
    primExpr();
    genCode(0x89); genCode(0xc3);  // mov %ebx %eax
    genCode(0x58); // pop %eax f7f3
    if(mul) { genCode(0x0f); genCode(0xaf); genCode(0xc3); }// mul %eax %ebx
    else {
      genCode(0xba); // mov %edx 0
      genCode(0); genCode(0); genCode(0); genCode(0);
      genCode(0xf7); genCode(0xf3); // div %ebx
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
  } else if(skip("(")) {
    addSubExpr();
    skip(")");
  }
}
int run()
{
	return ((int (*)(int))jitcode)(0);
}

int main()
{
	long psize;

#if defined(WIN32) || defined(WINDOWS)
	#include <windows.h>
#else
	psize = sysconf(_SC_PAGE_SIZE);
	if((posix_memalign((void **)&jitcode, psize, psize)))
		err(1, "posix_memalign");
	if(mprotect((void*)jitcode, psize, PROT_READ | PROT_WRITE | PROT_EXEC))
		err(1, "mprotect");
#endif

	memset(jitcode, 0, psize);

	char input[0xFFF];
	fgets(input, 0xFFF, stdin);

	lex(input);
	parser();
	clock_t bgn = clock();
	printf("%d\n", run());
	printf("time: %lfsec\n", (double)(clock() - bgn) / CLOCKS_PER_SEC);
}
