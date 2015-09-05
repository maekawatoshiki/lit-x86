#include "lit.h"
#include "asm.h"

static void init() {
	tkpos = jitCount = 0;
	memset(jitCode, 0, 0xFFFF);
	memset(token, 0, sizeof(token));
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
			for(i++; code[i] != '\n'; i++) { } line++;
		} else if(code[i] == '"') { // string?
			strcpy(token[tkpos].val, "\"");
			token[tkpos++].nline = line;
			for(i++; code[i] != '"' && code[i] != '\0'; i++)
				strncat(token[tkpos].val, &(code[i]), 1);
			token[tkpos].nline = line;
			if(code[i] == '\0') error("error: %d: expected expression '\"'", token[tkpos].nline);
			tkpos++;
		} else { // symbol?
			int iswindows = 0;
      if(code[i] == '\n' || (iswindows=(code[i] == '\r' && code[i+1] == '\n'))) {
      	strcpy(token[tkpos].val, ";"); if(iswindows) i++; line++;
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

	return 0;
}

static int eval(int pos, int isblock) {
	int isputs = 0;
	while(tkpos < tksize) {
		if(skip("def")) { blocksCount++;
			functionStmt();
		} else if(isFunction == IN_GLOBAL &&  strcmp("def", token[tkpos+1].val) != 0 &&
				strcmp(";", token[tkpos+1].val) != 0) {	// main function entry
			isFunction = IN_FUNC;
			nowFunc++;
			int espBgn;
			getFunction("main", jitCount); // append function
			genCode(0x56);
			genas("mov ebp esp");
			espBgn = jitCount + 2; genas("sub esp 0");
			genCode(0x8b); genCode(0x75); genCode(0x0c); // mov esi, 0xc(%ebp)
				eval(0, NON);
			printf("tkpos = %d, tksize = %d\n", tkpos, tksize);
			genCode(0x81); genCode(0xc4); genCodeInt32(sizeof(int) * (varSize[nowFunc] + 6)); // add %esp nn
			genCode(0xc9);// leave
			genCode(0xc3);// ret
			genCodeInt32Insert(sizeof(int) * (varSize[nowFunc] + 6), espBgn);
			isFunction = IN_GLOBAL;
		} else if(isassign()) {
			assignment();
		} else if(skip("Array")) {
			char *varname = token[tkpos++].val;
			int n = getNumOfVar(varname, 0); // add array variable
			if(!skip("[")) error("error: %d: invalid expression", token[tkpos].nline);
			relExpr(); // array size
			genas("shl eax 2");
			if(!skip("]")) 	error("error: %d: invalid expression", token[tkpos].nline);
			genCode(0x89); genCode(0x04); genCode(0x24); // mov [esp], eax
			genCode(0xff); genCode(0x56); genCode(12); // call malloc
			genCode(0x89); genCode(0x45); genCode(256 - sizeof(int) * n); // mov [ebp-n], eax
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
				genas("push eax");
				if(isstring) {
					genCode(0xff); genCode(0x56); genCode(0x04);// call *0x04(esi) putString
				} else {
					genCode(0xff); genCode(0x16);// call (esi) putNumber
				}
				genas("pop eax");
			} while(skip(","));
			// for new line
			if(isputs) { isputs = 0;
				genCode(0xff); genCode(0x56); genCode(0x08);// call *0x08(esi) putLN
			}
		} else if(skip("for")) { blocksCount++;
			assignment(); skip(","); whileStmt();
		} else if(skip("while")) { blocksCount++;
			whileStmt();
		} else if(skip("if")) { blocksCount++;
			ifStmt();
		} else if(skip("else")) {
			int end;
			genCode(0xe9); end = jitCount; genCodeInt32(0);// jmp while end
			genCodeInt32Insert(jitCount - pos - 4, pos);
			eval(end, NON);
			return 1;
		} else if(skip("elsif")) {
			int endif, end;
			genCode(0xe9); endif = jitCount; genCodeInt32(0);// jmp while end
			genCodeInt32Insert(jitCount - pos - 4, pos);
			int type = relExpr();
			genCode(type); genCode(0x05);
			genCode(0xe9); end = jitCount; genCodeInt32(0);// jmp while end
			eval(end, NON);
			genCodeInt32Insert(jitCount - endif - 4, endif);
			return 1;
		} else if(skip("break")) {
			genCode(0xe9); // jmp
			breaks[brkCount++] = jitCount; genCodeInt32(0);
		} else if(skip("end")) { blocksCount--;
			if(isblock == 0) {
				genCodeInt32Insert(jitCount - pos - 4, pos);
			} else if(isblock == BLOCK_FUNC) isFunction = IN_GLOBAL;
			return 1;
		} else if(!skip(";")) {
			relExpr();
		}
	}
	if(blocksCount != 0)
		error("error: expected 'end' before %d line", token[tkpos].nline);
	return 0;
}

static int parser() {
	tkpos = jitCount = 0;
	memset(jitCode, 0, 0xFFF);
	strPos = calloc(0xFF, sizeof(int));
	int main_address;
	genCode(0xe9); main_address = jitCount; genCodeInt32(0);
	eval(0, 0);

	int addr = getFunction("main", 0);
	printf("main() addr> %u\n", addr);
	genCodeInt32Insert(addr - 5, main_address);

	for(strPos--; strCount; strPos--) {
		genCodeInt32Insert(jitCount, *strPos);
		int i;
		replaceEscape(strings[--strCount]);
		for(i = 0; strings[strCount][i]; i++) {
			genCode(strings[strCount][i]);
		} genCode(0); // '\0'
		printf("string addr> %d\n", (int) strPos);
	}
	printf("memsz: %d\n", varSize[nowFunc]);

	/*{
		FILE *out = fopen("asm.s", "wb");
			fwrite(jitCode, 1, jitCount, out);
		fclose(out);
	}*/

	return 1;
}


static int getString() {
	strcpy(strings[strCount], token[tkpos++].val);
	*strPos++ = jitCount;
	return strCount++;
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
	return 0;
}

static int relExpr() {
	int lt=0, gt=0, diff=0, eql=0, fle=0;
	addSubExpr();
	if((lt=skip("<")) || (gt=skip(">")) || (diff=skip("!=")) ||
			(eql=skip("==")) || (fle=skip("<=")) || skip(">=")) {
		genas("push eax");
		addSubExpr();
		genas("mov ebx eax");  // mov %ebx %eax
		genas("pop eax");
		genCode(0x39); genCode(0xd8); // cmp %eax, %ebx
		return lt ? JB : gt ? JA : diff ? JNE : eql ? JE : fle ? JBE : JAE;
	}

	return 0;
}
static int addSubExpr() {
	int add;
	mulDivExpr();
	while((add = skip("+")) || skip("-")) {
		genas("push eax");
		mulDivExpr();
		genas("mov ebx eax");  // mov %ebx %eax
		genas("pop eax");
		if(add) { genas("add eax ebx"); }// add %eax %ebx
		else { genas("sub eax ebx"); } // sub %eax %ebx
	}
	return 0;
}
static int mulDivExpr() {
  int mul, div;
  primExpr();
  while((mul = skip("*")) || (div=skip("/")) || skip("%")) {
		genas("push eax");
    primExpr();
    genas("mov ebx eax"); // mov %ebx %eax
    genas("pop eax");
    if(mul) {
			genas("mul ebx");
    } else if(div) {
			genas("mov edx 0");
			genas("div ebx");
    } else { //mod
			genas("mov edx 0");
			genas("div ebx");
			genas("mov eax edx");
		}
  }
	return 0;
}
static int primExpr()
{
  if(isdigit(token[tkpos].val[0])) { // number?
    genas("mov eax %s", token[tkpos++].val);
  } else if(isalpha(token[tkpos].val[0])) { // variable?
		int varn = getNumOfVar(token[tkpos].val, 0);
		char *name = token[tkpos].val; tkpos++;

		if(skip("[")) {
			relExpr();
			genas("mov ecx eax");
			genCode(0x8b); genCode(0x55); genCode(256 - sizeof(int) * varn); // mov edx, [ebp - varn]
			genCode(0x8b); genCode(0x04); genCode(0x8a);// mov eax, [edx + ecx * 4]
			if(!skip("]"))
				error("error: %d: expected expression ']'", token[tkpos].nline);
		} else if(skip("(")) {
			int address = getFunction(name, 0), args = 0;
			printf("addr: %d\n", address);
			if(isalpha(token[tkpos].val[0]) || isdigit(token[tkpos].val[0])) { // has arg?
				do {
					relExpr();
					genas("push eax");
					args++;
				} while(skip(","));
			}
			genCode(0xe8); genCodeInt32(0xFFFFFFFF - (jitCount - address) - 3); // call func
			genas("add esp %d", args * sizeof(int));
			if(!skip(")")) error("error: %d: expected expression ')'", token[tkpos].nline);
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

static int ifStmt() {
	int end, type = relExpr(); // if conditions
	genCode(type); genCode(0x05);
	genCode(0xe9); end = jitCount; genCodeInt32(0);// jmp while's end:
	return eval(end, NON);
}

static int whileStmt() {
	int loopBgn = jitCount, end;
	int type = relExpr();
	genCode(type); genCode(0x05);
	genCode(0xe9); end = jitCount; genCodeInt32(0);// jmp while end
	if(eval(0, BLOCK_LOOP)) {
		unsigned int n = 0xFFFFFFFF - jitCount + loopBgn - 4;
		genCode(0xe9);
		genCodeInt32(n);
		genCodeInt32Insert(jitCount - end - 4, end);
		for(--brkCount; brkCount >= 0; brkCount--) {
			genCodeInt32Insert(jitCount - breaks[brkCount] - 4, breaks[brkCount]);
		} brkCount = 0;
	}

	return 0;
}

static int functionStmt() {
	int espBgn, argsc = 0;
	char *funcName = token[tkpos++].val;
	nowFunc++; isFunction = IN_FUNC;
	if(skip("(")) {
		do { getNumOfVar(token[tkpos++].val, 0); argsc++; } while(skip(","));
		skip(")");
	}
	getFunction(funcName, jitCount); // append function
	genas("push ebp");
	genas("mov ebp esp");
	espBgn = jitCount + 2; genas("sub esp 0");
	int argpos[128], i; for(i = 0; i < argsc; i++) {
		genCode(0x8b); genCode(0x45); genCode(0x08 + (argsc - i - 1) * sizeof(int));
		genCode(0x89); genCode(0x44); genCode(0x24);
		argpos[i] = jitCount; genCode(0x00);
	}
	eval(0, BLOCK_FUNC);
	genas("add esp %d", sizeof(int) * (varSize[nowFunc] + 6)); // add esp nn
	genCode(0xc9);// leave
	genCode(0xc3);// ret
	genCodeInt32Insert(sizeof(int) * (varSize[nowFunc] + 6), espBgn);
	for(i = 1; i <= argsc; i++) {
		jitCode[argpos[i - 1]] = 256 - sizeof(int) * i + ((varSize[nowFunc] + 6) * 4 - 4);
	}

	printf("isFunction = %d\n", isFunction);
	printf("%s() has %d byte\n", funcName, varSize[nowFunc] << 2);

	return 0;
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
		genas("push eax");
		if(!skip("]") || !skip("="))
			 error("error: %d: invalid assignment", token[tkpos].nline);
			relExpr();
		genCode(0x8b); genCode(0x4d);
		genCode(256 - sizeof(int) * n); // mov ecx [ebp-n]
		genas("pop edx");
		// mov [ecx+edx], eax
		genCode(0x89); genCode(0x04); genCode(0x91);
	} else {
		skip("=");
		relExpr();
		genCode(0x89); genCode(0x45);
		genCode(256 - sizeof(int) * n); // mov var eax
		printf("%d\n", tkpos);
	}

	return 0;
}

static char *replaceEscape(char *str) {
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

static void putNumber(int n) { printf("%d", n); }
static void putString(int n) { printf("%s", &(jitCode[n])); }
static void putLN() { printf("\n"); }
void *funcTable[] = { (void *) putNumber, (void*) putString, (void*) putLN, (void*)malloc };

int run() {
	int mem[0xFFFF] = { 0 };

	printf("size: %dbyte, %.2lf%%\n", jitCount, ((double)jitCount / 0xFFFF) * 100.0);
	return ((int (*)(int *, void**))jitCode)(0, funcTable);
}

int main(int argc, char **argv) {

#if defined(WIN32) || defined(WINDOWS)
	jitCode = (unsigned char*) calloc(sizeof(unsigned char), 0xFFFF);
#else
	long psize = 0xFFFF + 1;
	printf("page_size = %ld\n", psize);
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
			printf(">> "); strcat(input, line);
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
	volatile clock_t bgn = clock();
	run();
	clock_t end = clock();
	printf("time: %lfsec\n", (double)(end - bgn) / CLOCKS_PER_SEC);
	free(jitCode);
}
