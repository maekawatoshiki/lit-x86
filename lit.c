#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>

unsigned char *program;
int j = 0;

struct Token {
  char val[32];
} token[1024] = { 0 };
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

int isop1, op2;
int calc()
{
  tkpos = j = 0; isop1 = 1;
  program[j++] = 0x55 ;// push   %ebp
  program[j++] = 0x89; program[j++] = 0xe5;// mov    %esp,%ebp
  addSubEpxr();
  program[j++] = 0x5d;// pop %ebp
  program[j++] = 0xc3;// ret
}

int addSubEpxr()
{
  int add;
  mulDivExpr();
  while((add = skip("+")) || skip("-"))
  {
    program[j++] = 0x50; // push %eax
    mulDivExpr();//89c3
    program[j++] = 0x89; program[j++] = 0xc3;  // mov %ebx %eax
    program[j++] = 0x58; // pop %eax
    if(add) { program[j++] = 0x01; program[j++] = 0xd8; }// add %eax %ebx
    else { program[j++] = 0x29; program[j++] = 0xd8; } // sub %eax %ebx
  }
}
int mulDivExpr()
{
  int mul;
  primExpr();
  while((mul = skip("*")) || skip("/"))
  {
    program[j++] = 0x50; // push %eax
    primExpr();
    program[j++] = 0x89; program[j++] = 0xc3;  // mov %ebx %eax
    program[j++] = 0x58; // pop %eax f7f3
    if(mul) { program[j++] = 0x0f; program[j++] = 0xaf; program[j++] = 0xc3; }// mul %eax %ebx
    else {
      program[j++] = 0xba; // mov %edx 0
      program[j++] = 0; program[j++] = 0; program[j++] = 0; program[j++] = 0;
      program[j++] = 0xf7; program[j++] = 0xf3; // div %ebx
    }
  }
}
int primExpr()
{
  if(isdigit(token[tkpos].val[0]))
  {
    int n = atoi(token[tkpos].val);
      program[j++] = 0xb8; // mov %eax
      program[j++] = n << 24 >> 24;
      program[j++] = n << 16 >> 24;
      program[j++] = n << 8 >> 24;
      program[j++] = n << 0 >> 24; // mov %eax num
    tkpos++;
  }
}

int run()
{
	int stack[32768];
  /*
	program[j++] = 0x55;// push   %ebp
	program[j++] = 0x89; program[j++] = 0xe5;// mov    %esp,%ebp
  program[j++] = 0xb8; // mov %eax
  program[j++] = 0x05; program[j++] = 0x00; program[j++] = 0x00; program[j++] = 0x00;// mov 0x05 %eax
	program[j++] = 0x05; // run %eax
  program[j++] = 0x06; program[j++] = 0x00; program[j++] = 0x00;  program[j++] = 0x00;// run 0x0F %eax
  program[j++] = 0x2d; // sub %eax
  program[j++] = 0x03; program[j++] = 0x00; program[j++] = 0x00; program[j++] = 0x00;// sub 0x03 %eax
  program[j++] = 0x69; program[j++] = 0xc0; // mul %eax
  program[j++] = 0x02; program[j++] = 0x00; program[j++] = 0x00; program[j++] = 0x00;// sub 0x03 %eax
	program[j++] = 0x5d;// pop %ebp
  program[j++] = 0xc3;// ret
  */
  // puts("End of JIT Compile");
	return ((int (*)(int *))program)(stack);
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
  if((posix_memalign((void **)&program, psize, psize)))
  	err(1, "posix_memalign");
  if(mprotect((void*)program, psize, PROT_READ | PROT_WRITE | PROT_EXEC))
  	err(1, "mprotect");
  memset(program, 0, 0xFFF);

  char input[0xFFF];
  fgets(input, 0xFFF, stdin);
  lex(input);
  calc();
  clock_t bgn = clock();
  printf("%d\n", run());
  printf("time: %lfsec\n", (double)(clock() - bgn) / CLOCKS_PER_SEC);
}
