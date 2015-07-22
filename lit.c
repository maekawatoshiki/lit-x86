#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>

unsigned char *jitcode;
int j = 0;

struct Token {
  char val[32];
} token[0xFFFF] = { 0 };
int tkpos = 0, tksize;

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

int calc()
{
  tkpos = j = 0;
  jitcode[j++] = 0x55 ;// push   %ebp
  jitcode[j++] = 0x89; jitcode[j++] = 0xe5;// mov    %esp,%ebp
  addSubExpr();
  jitcode[j++] = 0x5d;// pop %ebp
  jitcode[j++] = 0xc3;// ret
}

int addSubExpr()
{
  int add;
  mulDivExpr();
  while((add = skip("+")) || skip("-"))
  {
    jitcode[j++] = 0x50; // push %eax
    mulDivExpr();//89c3
    jitcode[j++] = 0x89; jitcode[j++] = 0xc3;  // mov %ebx %eax
    jitcode[j++] = 0x58; // pop %eax
    if(add) { jitcode[j++] = 0x01; jitcode[j++] = 0xd8; }// add %eax %ebx
    else { jitcode[j++] = 0x29; jitcode[j++] = 0xd8; } // sub %eax %ebx
  }
}
int mulDivExpr()
{
  int mul;
  primExpr();
  while((mul = skip("*")) || skip("/"))
  {
    jitcode[j++] = 0x50; // push %eax
    primExpr();
    jitcode[j++] = 0x89; jitcode[j++] = 0xc3;  // mov %ebx %eax
    jitcode[j++] = 0x58; // pop %eax f7f3
    if(mul) { jitcode[j++] = 0x0f; jitcode[j++] = 0xaf; jitcode[j++] = 0xc3; }// mul %eax %ebx
    else {
      jitcode[j++] = 0xba; // mov %edx 0
      jitcode[j++] = 0; jitcode[j++] = 0; jitcode[j++] = 0; jitcode[j++] = 0;
      jitcode[j++] = 0xf7; jitcode[j++] = 0xf3; // div %ebx
    }
  }
}
int primExpr()
{
  if(isdigit(token[tkpos].val[0]))
  {
    int n = atoi(token[tkpos].val);
      jitcode[j++] = 0xb8; // mov %eax
      jitcode[j++] = n << 24 >> 24;
      jitcode[j++] = n << 16 >> 24;
      jitcode[j++] = n << 8 >> 24;
      jitcode[j++] = n << 0 >> 24; // mov %eax num
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
  int i = 0;
	long psize;
  #ifdef BSD
  	psize = getpagesize();
  #else
  	psize = sysconf(_SC_PAGE_SIZE);
  #endif
  if((posix_memalign((void **)&jitcode, psize, psize)))
  	err(1, "posix_memalign");
  if(mprotect((void*)jitcode, psize, PROT_READ | PROT_WRITE | PROT_EXEC))
  	err(1, "mprotect");
  memset(jitcode, 0, psize);

  char input[0xFFFF];
  fgets(input, 0xFFFF, stdin);
  lex(input);
  calc();
  clock_t bgn = clock();
  printf("%d\n", run());
  printf("time: %lfsec\n", (double)(clock() - bgn) / CLOCKS_PER_SEC);
}
