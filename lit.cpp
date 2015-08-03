#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <malloc.h>
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

class Lit {
public:
  struct TOKEN {
    char val[32];
  } token[0xFFF];
  int tkpos, tksize;

  struct VARIABENAMES {
  	char name[32];
  } varNames[0x7F];
  int varCount;

  int breaks[0xFF]; int breaksCount;

  Lit():breaksCount(0), tkpos(0), varCount(0) {
    memset(varNames, 0, sizeof(VARIABENAMES)*0x7F);
    memset(token, 0, sizeof(TOKEN)*0xFFF);
    memset(breaks, 0, 0xFF);
  };

  int getNumOfVar(char *);
  void genCode(unsigned char);
  int lex(char *);
  int skip(char *);
  int eval();
  int parser();
  int relExpr();
  int addSubExpr();
  int mulDivExpr();
  int primExpr();
  int run();

  void execute(char *SourceCode);
};

void putNumber(int);

int Lit::getNumOfVar(char *name)
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

void Lit::genCode(unsigned char val) { jitCode[jitCount++] = val; }

int  Lit::lex(char *code)
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

int  Lit::skip(char *s)
{
  if(strcmp(s, token[tkpos].val) == 0) { tkpos++; return 1; }
  else return 0;
}

/*** prime ***
p = 2; while p<100 k = 2; isp = 1; while k*k <= p; if p % k == 0 isp = 0 break end  k = k + 1; end if isp == 1 print p end p = p + 1; end
***
i = 0; while i < 10; k = 0; while k < 10; j = 0; while j < 10; print j j = j + 1 end k = k + 1 end i = i + 1 end
i=0; while i<10 j=0 while j<10 print j j=j+1 end print i i=i+1; end
i = 0; sum = 0; while i < 100000001; sum = sum + i;  i = i + 1 end print sum
n-6
i = 0; while i < 100 if i % 2 == 0; print 100 end print i i = i + 1 end
n = 1482432730 while n != 1 if i % 2 == 0 n = n / 2 end if i % 2 != 0 n = n * 3 + 1 end print n end

*/

int  Lit::eval()
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
			genCode(0x89); genCode(0x04); genCode(0x24);
			genCode(0xff); genCode(0x16);
		} else if(skip("while")) {
			int loopBgn = jitCount, end;
			int type = relExpr();
			genCode(type); genCode(0x05);
			genCode(0xe9); end = jitCount; genCode(0); genCode(0); genCode(0); genCode(0);// jmp while end
			if(eval()) {
				unsigned int n = 0xFFFFFFFF - jitCount + loopBgn - 4;
				genCode(0xe9);
				genCode(n << 24 >> 24);
				genCode(n << 16 >> 24);
				genCode(n << 8 >> 24);
				genCode(n << 0 >> 24);
				n = jitCount - end - 4;
				jitCode[end]   = (n << 24 >> 24);
				jitCode[end+1] = (n << 16 >> 24);
				jitCode[end+2] = (n << 8 >> 24);
				jitCode[end+3] = (n << 0 >> 24);
				for(--breaksCount; breaksCount >= 0; breaksCount--) {
					n = jitCount - breaks[breaksCount] - 4;
					jitCode[breaks[breaksCount]] = (n << 24 >> 24);
					jitCode[breaks[breaksCount]+1] = (n << 16 >> 24);
					jitCode[breaks[breaksCount]+2] = (n << 8 >> 24);
					jitCode[breaks[breaksCount]+3] = (n << 0 >> 24);
				} breaksCount = 0;
			}
		} else if(skip("if")) {
			int loopBgn = jitCount, end;
			int type = relExpr();
			genCode(type); genCode(0x05);
			genCode(0xe9); end = jitCount; genCode(0); genCode(0); genCode(0); genCode(0);// jmp while end
			if(eval()) {
				unsigned int n = jitCount - end - 4;
				jitCode[end]   = (n << 24 >> 24);
				jitCode[end+1] = (n << 16 >> 24);
				jitCode[end+2] = (n << 8 >> 24);
				jitCode[end+3] = (n << 0 >> 24);
			}
		} else if(skip("break")) {
			genCode(0xe9);
			breaks[breaksCount]=jitCount; genCode(0); genCode(0); genCode(0); genCode(0);
			breaksCount++;
		} else if(skip("end")) {
			return 1;
		} else { relExpr(); }
	}

	return 0;
}

int  Lit::parser()
{
	int espBgn;
	tkpos = jitCount = 0;
	genCode(0x55);// push   %ebp
	genCode(0x89); genCode(0xe5);// mov    %esp,%ebp
	genCode(0x8b); genCode(0x75); genCode(0x0c);
	genCode(0x83); genCode(0xec); espBgn = jitCount; genCode(sizeof(int)); // sub %esp nn

	eval();

	genCode(0x83); genCode(0xc4); genCode(sizeof(int) * varCount); // add %esp nn
	genCode(0x5d);// pop %ebp
	genCode(0xc3);// ret
	jitCode[espBgn] = sizeof(int) * varCount;
}
int  Lit::relExpr()
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

int  Lit::addSubExpr()
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
int  Lit::mulDivExpr()
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
int  Lit::primExpr()
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
void putNumber(int n) { printf("%d\n", n); }

int  Lit::run()
{
	printf("size: %dbyte, %.2lf%%\n", jitCount, ((double)jitCount/4098)*100.0);
	getchar();
	void *funcTable[2] = {(void *)putNumber, (void *)getchar};
	return ((int (*)(int *, void**))jitCode)(0, funcTable);
}

void Lit::execute(char *SourceCode)
{
  lex(SourceCode);
	parser();
	run();
}

int main()
{
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

  Lit lit;
	memset(jitCode, 0, 0xFFF);

	char input[0xFFFF] = "";
	fgets(input, 0xFFFF, stdin);

	clock_t bgn = clock();
	lit.execute(input);
	printf("time: %lfsec\n", (double)(clock() - bgn) / CLOCKS_PER_SEC);
}