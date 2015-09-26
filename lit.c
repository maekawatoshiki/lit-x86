#include "debug.h"
#include "lit.h"
#include "asm.h"

static void init() {
#if defined(WIN32) || defined(WINDOWS)
	ntvCode = (unsigned char*) calloc(sizeof(unsigned char), 0xFFFF);
#else
	long memsize = 0xFFFF + 1;
	if((posix_memalign((void **)&ntvCode, memsize, memsize)))
		perror("posix_memalign");
	if(mprotect((void*)ntvCode, memsize, PROT_READ | PROT_WRITE | PROT_EXEC))
		perror("mprotect");
#endif
	tkpos = ntvCount = 0; tksize = 0xfff;
	set_xor128();
	tok = calloc(sizeof(Token), tksize);
	brks.addr = calloc(sizeof(unsigned int), 1);
	rets.addr = calloc(sizeof(unsigned int), 1);
	memset(ntvCode, 0, 0xFFFF);
	memset(tok, 0, sizeof(Token));
}

static void dispose() {
	free(ntvCode);
	free(brks.addr);
	free(rets.addr);
	free(tok);
	freeMem();
}

static int getString() {
	strcpy(strings[strCount], tok[tkpos++].val);
	*strAddr++ = ntvCount;
	return strCount++;
}

static Variable *getVariable(char *name) {
	int i;
	// loval variable
	for(i = 0; i < varCounter; i++) {
		if(strcmp(name, locVar[nowFunc][i].name) == 0)
			return &locVar[nowFunc][i];
	}
	// global variable
	for(i = 0; i < gblVar.count; i++) {
		if(strcmp(name, gblVar.var[i].name) == 0) {
			return &gblVar.var[i];
		}
	}

	return NULL;
}

static Variable *appendVariable(char *name, int type) {
	if(isFunction == IN_FUNC) {
		int sz;
		sz = 1 + ++varSize[nowFunc];
		strcpy(locVar[nowFunc][varCounter].name, name);
		locVar[nowFunc][varCounter].type = type;
		locVar[nowFunc][varCounter].id = sz;
		locVar[nowFunc][varCounter].loctype = V_LOCAL;

		return &locVar[nowFunc][varCounter++];
	} else if(isFunction == IN_GLOBAL) {
		// global varibale
		strcpy(gblVar.var[gblVar.count].name, name);
		gblVar.var[gblVar.count].type = type;
		gblVar.var[gblVar.count].loctype = V_GLOBAL;
		gblVar.var[gblVar.count].id = (unsigned int)&ntvCode[ntvCount];
		ntvCount += sizeof(int); // type

		return &gblVar.var[gblVar.count++];
	}

	return NULL;
}

static int getFunction(char *name, int address) {
	for(int i = 0; i < funcCount; i++) {
		if(strcmp(functions[i].name, name) == 0) {
			return functions[i].address;
		}
	}
	functions[funcCount].address = address;
	strcpy(functions[funcCount].name, name);
	return functions[funcCount++].address;
}

static int appendBreak() {
	genCode(0xe9); // jmp
	brks.addr = realloc(brks.addr, 4 * (brks.count + 1));
	brks.addr[brks.count] = ntvCount;
	genCodeInt32(0);
	return brks.count++;
}

static int appendReturn() {
	relExpr(); // get argument
	//genas("push eax");
	genCode(0xe9); // jmp
	rets.addr = realloc(rets.addr, 4 * (rets.count + 1));
	if(rets.addr == NULL) error("LitSystemError: no enough memory");
	rets.addr[rets.count] = ntvCount;
	genCodeInt32(0);
	return rets.count++;
}


static int skip(char *s) {
  if(strcmp(s, tok[tkpos].val) == 0) {
  	tkpos++; return 1;
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

static int lex(char *code) {
  int i, codeSize = strlen(code), line = 1;
	int iswindows = 0;

	for(i = 0; i < codeSize; i++) {
 		if(tksize <= i)
			tok = realloc(tok, (tksize += 512 * sizeof(Token)));
		if(isdigit(code[i])) { // number?
      for(; isdigit(code[i]); i++)
				strncat(tok[tkpos].val, &(code[i]), 1);
			tok[tkpos].nline = line;
      i--; tkpos++;
    } else if(isalpha(code[i])) { // ident?
      for(; isalpha(code[i]) || isdigit(code[i]) || code[i] == '_'; i++)
        strncat(tok[tkpos].val, &(code[i]), 1);
      tok[tkpos].nline = line;
      i--; tkpos++;
    } else if(code[i] == ' ' || code[i] == '\t') { // space char?
    } else if(code[i] == '#') { // comment?
			for(i++; code[i] != '\n'; i++) { } line++;
		} else if(code[i] == '"') { // string?
			strcpy(tok[tkpos].val, "\"");
			tok[tkpos++].nline = line;
			for(i++; code[i] != '"' && code[i] != '\0'; i++)
				strncat(tok[tkpos].val, &(code[i]), 1);
			tok[tkpos].nline = line;
			if(code[i] == '\0') error("error: %d: expected expression '\"'", tok[tkpos].nline);
			tkpos++;
		} else if(code[i] == '\n' ||
			(iswindows=(code[i] == '\r' && code[i+1] == '\n'))) {
			i += iswindows;
			strcpy(tok[tkpos].val, ";");
			tok[tkpos].nline = line++; tkpos++;
		} else {
			strncat(tok[tkpos].val, &(code[i]), 1);
			if(code[i+1] == '=' ||
				(code[i]=='+' && code[i+1]=='+') ||
				(code[i]=='-' && code[i+1]=='-'))
					strncat(tok[tkpos].val, &(code[++i]), 1);
			tok[tkpos].nline = line;
			tkpos++;
    }
  }
  tok[tkpos].nline = line;
	for(i = 0; i < tkpos; i++) {
		debug("tk: %d > %s\n", i, tok[i].val);
	}
  tksize = tkpos - 1;

	return 0;
}

static int eval(int pos, int isblock) {
	int isputs = 0, is1byte = 0, isformat = 0;
	while(tkpos < tksize) {
		if(skip("$")) { // global varibale?
			if(isassign()) assignment();
		} else if(skip("def")) { blocksCount++;
			functionStmt();
		} else if(isFunction == IN_GLOBAL && strcmp("def", tok[tkpos+1].val) &&
				strcmp("$", tok[tkpos+1].val) && strcmp(";", tok[tkpos+1].val)) {	// main function entry
			isFunction = IN_FUNC;
			nowFunc++;
			int espBgn;
			getFunction("main", ntvCount); // append function
			genas("push ebp");
			genas("mov ebp esp");
			espBgn = ntvCount + 2; genas("sub esp 0");
			genCode(0x8b); genCode(0x75); genCode(0x0c); // mov esi, 0xc(%ebp)

			eval(0, NON);

			genCode(0x81); genCode(0xc4); genCodeInt32(sizeof(int) * (varSize[nowFunc] + 6)); // add %esp nn
			genCode(0xc9);// leave
			genCode(0xc3);// ret
			genCodeInt32Insert(sizeof(int) * (varSize[nowFunc] + 6), espBgn);
			isFunction = IN_GLOBAL;
		} else if(isassign()) {
			assignment();
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
					genCode(0xff); genCode(0x56); genCode(4);// call *0x04(esi) putString
				} else {
					genCode(0xff); genCode(0x16); // call (esi) putNumber
				}
				genas("add esp 4");
			} while(skip(","));
			// for new line
			if(isputs) {
				genCode(0xff); genCode(0x56); genCode(8);// call *0x08(esi) putLN
			}
		} else if(skip("printf")) {
			if(skip("\"")) {
				genCode(0xb8); getString();
				genCodeInt32(0x00); // mov eax string_address
				genCode(0x89); genCode(0x44); genCode(0x24); genCode(0x00); // mov [esp+0], eax
			}
			if(skip(",")) {
				int a = 4;
				do {
					relExpr();
					genCode(0x89); genCode(0x44); genCode(0x24); genCode(a); // mov [esp+a], eax
					a += 4;
				} while(skip(","));
			}
			genCode(0xff); genCode(0x56); genCode(12 + 8); // call printf
		} else if(skip("for")) { blocksCount++;
			assignment(); skip(","); whileStmt();
		} else if(skip("while")) { blocksCount++;
			whileStmt();
		} else if(skip("if")) { blocksCount++;
			ifStmt();
		} else if(skip("return")) {
			appendReturn();
		} else if(skip("else")) {
			int end;
			genCode(0xe9); end = ntvCount; genCodeInt32(0);// jmp while end
			genCodeInt32Insert(ntvCount - pos - 4, pos);
			eval(end, NON);
			return 1;
		} else if(skip("elsif")) {
			int endif, end;
			genCode(0xe9); endif = ntvCount; genCodeInt32(0);// jmp while end
			genCodeInt32Insert(ntvCount - pos - 4, pos);
			int type = relExpr();
			genCode(type); genCode(0x05);
			genCode(0xe9); end = ntvCount; genCodeInt32(0);// jmp while end
			eval(end, NON);
			genCodeInt32Insert(ntvCount - endif - 4, endif);
			return 1;
		} else if(skip("break")) {
			appendBreak();
		} else if(skip("end")) { blocksCount--;
			if(isblock == 0) {
				genCodeInt32Insert(ntvCount - pos - 4, pos);
			} else if(isblock == BLOCK_FUNC) isFunction = IN_GLOBAL;
			return 1;
		} else if(!skip(";")) {
			relExpr();
		}
	}
	if(blocksCount != 0)
		error("error: expected 'end' before %d line", tok[tkpos].nline);
	return 0;
}

static int parser() {
	tkpos = ntvCount = 0;
	strAddr = calloc(0xFF, sizeof(int));
	int main_address;
	genCode(0xe9); main_address = ntvCount; genCodeInt32(0);
	eval(0, 0);

	int addr = getFunction("main", 0);
	debug("main() addr> %u\n", addr);
	genCodeInt32Insert(addr - 5, main_address);

	for(strAddr--; strCount; strAddr--) {
		genCodeInt32Insert((unsigned int)&ntvCode[ntvCount], *strAddr);
		int i;
		replaceEscape(strings[--strCount]);
		for(i = 0; strings[strCount][i]; i++) {
			genCode(strings[strCount][i]);
		} genCode(0); // '\0'
		debug("string addr> %d\n", (int) strAddr);
	}
#ifdef NDEBUG
	// Nothing
#else
	for(int i = 0; i < ntvCount; i++)
		printf("%02x", ntvCode[i]);
	puts("");
#endif
	debug("memsz: %d\n", varSize[nowFunc]);

	return 1;
}


static int relExpr() {
	int lt=0, gt=0, diff=0, eql=0, fle=0;
	addSubExpr();
	if((lt=skip("<")) || (gt=skip(">")) || (diff=skip("!=")) ||
			(eql=skip("==")) || (fle=skip("<=")) || skip(">=")) {
		genas("push eax");
		addSubExpr();
		genas("mov ebx eax");
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
static int primExpr() {
  if(isdigit(tok[tkpos].val[0])) { // number?
    genas("mov eax %d", atoi(tok[tkpos++].val));
	} else if(skip("'")) { // char?
		genas("mov eax %d", tok[tkpos++].val[0]);
		skip("'");
	} else if(skip("\"")) { // string?
		genCode(0xb8); getString();
		genCodeInt32(0x00); // mov eax string_address
  } else if(isalpha(tok[tkpos].val[0])) { // variable or inc or dec
		char *name = tok[tkpos].val;
		Variable *v;

		if(isassign()) {
			assignment();
		} else {
			tkpos++;
			if(skip("[")) { // Array?
				if((v = getVariable(name)) == NULL)
					error("error: %d: '%s' was not declared", tok[tkpos].nline, name);
				relExpr();
				genas("mov ecx eax");

				if(v->loctype == V_LOCAL) {
					genCode(0x8b); genCode(0x55); genCode(256 - sizeof(int) * v->id); // mov edx, [ebp - v*4]
				} else if(v->loctype == V_GLOBAL) {
					genCode(0x8b); genCode(0x15); genCodeInt32(v->id); // mov edx, GLOBAL_ADDR
				}

				if(v->type == T_INT) {
					genCode(0x8b); genCode(0x04); genCode(0x8a);// mov eax, [edx + ecx * 4]
				} else {
					genCode(0x0f); genCode(0xb6); genCode(0x04); genCode(0x0a);// movzx eax, [edx + ecx]
				}
			if(!skip("]"))
				error("error: %d: expected expression ']'", tok[tkpos].nline);
			} else if(skip("(")) { // Function?
				if(strcmp(name, "rand") == 0) {
					genCode(0xff); genCode(0x56); genCode(12 + 4); // call rand
				} else if(strcmp(name, "Array") == 0) {
					relExpr(); // get array size
					genas("shl eax 2");
					genCode(0x89); genCode(0x04); genCode(0x24); // mov [esp], eax
					genCode(0xff); genCode(0x56); genCode(12 + 0); // call malloc
					genas("push eax");
					genas("push eax");
					genCode(0xff); genCode(0x56); genCode(12 + 12); // call appendMem
					genas("add esp 4");
					genas("pop eax");
				} else { // User Function?
					int address = getFunction(name, 0), args = 0;
					debug("addr: %d\n", address);
					if(isalpha(tok[tkpos].val[0]) || isdigit(tok[tkpos].val[0]) ||
						!strcmp(tok[tkpos].val, "\"") || !strcmp(tok[tkpos].val, "(")) { // has arg?
						do {
							relExpr();
							genas("push eax");
							args++;
						} while(skip(","));
					}
					genCode(0xe8); genCodeInt32(0xFFFFFFFF - (ntvCount - address) - 3); // call func
					genas("add esp %d", args * sizeof(int));
				}
				debug("%s\n", tok[tkpos].val);
				if(!skip(")")) error("func: error: %d: expected expression ')'", tok[tkpos].nline);
			} else {
				if((v = getVariable(name)) == NULL)
					error("var: error: %d: '%s' was not declared", tok[tkpos].nline, name);
				if(v->loctype == V_LOCAL) {
					genCode(0x8b); genCode(0x45);
					genCode(256 - sizeof(int) * v->id); // mov eax variable
				} else if(v->loctype == V_GLOBAL) {
					genCode(0xa1); genCodeInt32(v->id); // mov eax GLOBAL_ADDR
				}
			}
		}
	} else if(skip("]")) {
	   error("error: %d: invalid expression", tok[tkpos].nline);
  } else if(skip("(")) {
    if(isassign()) assignment(); else relExpr();
		if(!skip(")"))
		 error("error: %d: expected expression ')'", tok[tkpos].nline);
  }

	return 0;
}

static int ifStmt() {
	int end, type = relExpr(); // if conditions
	genCode(type); genCode(0x05);
	genCode(0xe9); end = ntvCount; genCodeInt32(0);// jmp while's end:
	return eval(end, NON);
}

static int whileStmt() {
	int loopBgn = ntvCount, end, stepBgn[2], stepOn = 0;
	int type = relExpr();
	if(skip(",")) {
		stepOn = 1;
		stepBgn[0] = tkpos;
		for(; tok[tkpos].val[0] != ';'; tkpos++);
	}
	genCode(type); genCode(0x05); // ifjmp 5
	genCode(0xe9); end = ntvCount; genCodeInt32(0);// jmp while end
	if(eval(0, BLOCK_LOOP)) {
		if(stepOn) {
			stepBgn[1] = tkpos;
			tkpos = stepBgn[0];
			if(isassign()) assignment();
			tkpos = stepBgn[1];
		}
		unsigned int n = 0xFFFFFFFF - ntvCount + loopBgn - 4;
		genCode(0xe9); genCodeInt32(n); // jmp n
		genCodeInt32Insert(ntvCount - end - 4, end);

		for(--brks.count; brks.count >= 0; brks.count--) {
			genCodeInt32Insert(ntvCount - brks.addr[brks.count] - 4, brks.addr[brks.count]);
		} brks.count = 0;
	}

	return 0;
}

static int functionStmt() {
	int espBgn, argsc = 0;
	char *funcName = tok[tkpos++].val;
	nowFunc++; isFunction = IN_FUNC;
	if(skip("(")) {
		do { declareVariable(); tkpos++; argsc++; } while(skip(","));
		skip(")");
	}
	getFunction(funcName, ntvCount); // append function
	genas("push ebp");
	genas("mov ebp esp");
	espBgn = ntvCount + 2; genas("sub esp 0"); // align
	int argpos[128], i; for(i = 0; i < argsc; i++) {
		genCode(0x8b); genCode(0x45); genCode(0x08 + (argsc - i - 1) * sizeof(int));
		genCode(0x89); genCode(0x44); genCode(0x24);
		argpos[i] = ntvCount; genCode(0x00);
	}
	eval(0, BLOCK_FUNC);

	for(--rets.count; rets.count >= 0; --rets.count) {
		genCodeInt32Insert(ntvCount - rets.addr[rets.count] - 4, rets.addr[rets.count]);
		//genas("pop eax");
	} rets.count = 0;

	genas("add esp %u", sizeof(int) * (varSize[nowFunc] + 6)); // add esp nn
	genCode(0xc9);// leave
	genCode(0xc3);// ret

	genCodeInt32Insert(sizeof(int) * (varSize[nowFunc] + 6), espBgn);
	for(i = 1; i <= argsc; i++) {
		ntvCode[argpos[i - 1]] = 256 - sizeof(int) * i + (((varSize[nowFunc] + 6) * sizeof(int)) - 4);
	}

	debug("%s() has %u functions or variables\n", funcName, varSize[nowFunc] * sizeof(int));

	return 0;
}

static int isassign() {
	//puts("isassign()");
	if(strcmp(tok[tkpos+1].val, "=") == 0) return 1;
	else if(strcmp(tok[tkpos+1].val, "++") == 0) return 1;
	else if(strcmp(tok[tkpos+1].val, "--") == 0) return 1;
	else if(strcmp(tok[tkpos+1].val, "[") == 0) {
		int i = tkpos + 1;
		while(strcmp(tok[i].val, "]")) {
			if(strcmp(tok[i].val, ";") == 0)
				error("error: %d: invalid expression", tok[tkpos].nline);
			i++;
		}
		debug(">%s\n", tok[i].val); i++;
		if(strcmp(tok[i].val, "=") == 0) return 1;
	} else if(strcmp(tok[tkpos+1].val, ":") == 0) {
		int i = tkpos + 3;
		if(strcmp(tok[i].val, "=") == 0) return 1;
	}
	return 0;
}

static int assignment() {
	Variable *v = getVariable(tok[tkpos].val);
	int inc = 0, dec = 0, declare = 0;
	if(v == NULL) { declare++; v = declareVariable(); }
	tkpos++;

	if(v->loctype == V_LOCAL) {
		if(skip("[")) { // Array?
			relExpr();
			genas("push eax");
			if(skip("]") && skip("=")) {
				relExpr();
				genCode(0x8b); genCode(0x4d);
				genCode(256 -
					(v->type == T_INT ? sizeof(int) :
						v->type == T_STRING ? sizeof(int*) :
						v->type == T_DOUBLE ? sizeof(double) : 4) * v->id); // mov ecx [ebp-n]
				genas("pop edx");
				if(v->type == T_INT) {
					genCode(0x89); genCode(0x04); genCode(0x91); // mov [ecx+edx*4], eax
				} else {
					genCode(0x89); genCode(0x04); genCode(0x11); // mov [ecx+edx], eax
				}
			} else if((inc=skip("++")) || (dec=skip("--"))) {

			} else error("error: %d: invalid assignment", tok[tkpos].nline);
		} else { // Scalar?
			if(skip("=")) {
				relExpr();
			} else if((inc=skip("++")) || (dec=skip("--"))) {
				genCode(0x8b); genCode(0x45);
				genCode(256 -
					(v->type == T_INT ? sizeof(int) :
						v->type == T_STRING ? sizeof(int*) :
						v->type == T_DOUBLE ? sizeof(double) : 4) * v->id); // mov eax varaible
				genas("push eax");
				if(inc) genCode(0x40); // inc eax
				else if(dec) genCode(0x48); // dec eax
			}
			genCode(0x89); genCode(0x45);
			genCode(256 -
				(v->type == T_INT ? sizeof(int) :
					v->type == T_STRING ? sizeof(int*) :
					v->type == T_DOUBLE ? sizeof(double) : 4) * v->id); // mov var eax
			if(inc || dec) genas("pop eax");
		}
	} else if(v->loctype == V_GLOBAL) {
		if(declare) { // first declare for global variable?
			// assignment only interger
			if(skip("=")) {
				unsigned *m = (unsigned *)v->id; *m = atoi(tok[tkpos++].val);
			}
		} else {
			if(skip("[")) { // Array?
				relExpr();
				genas("push eax");
				if(skip("]") && skip("=")) {
					relExpr();
					genCode(0x8b); genCode(0x0d); genCodeInt32(v->id); // mov ecx GLOBAL_ADDR
					genas("pop edx");
					if(v->type == T_INT) {
						genCode(0x89); genCode(0x04); genCode(0x91); // mov [ecx+edx*4], eax
					} else {
						genCode(0x89); genCode(0x04); genCode(0x11); // mov [ecx+edx], eax
					}
				} else error("error: %d: invalid assignment", tok[tkpos].nline);
			} else if(skip("=")) {
				relExpr();
				genCode(0xa3); genCodeInt32(v->id); // mov GLOBAL_ADDR eax
			} else if((inc=skip("++")) || (dec=skip("--"))) {
				genCode(0xa1); genCodeInt32(v->id);// mov eax GLOBAL_ADDR
				genas("push eax");
				if(inc) genCode(0x40); // inc eax
				else if(dec) genCode(0x48); // dec eax
				genCode(0xa3); genCodeInt32(v->id); // mov GLOBAL_ADDR eax
			}
			if(inc || dec) genas("pop eax");
		}
	}

	return 0;
}

static Variable *declareVariable() {
	int npos = tkpos;
	if(isalpha(tok[tkpos].val[0])) {
		tkpos++;
		if(skip(":")) {
			if(skip("int")) { --tkpos; return appendVariable(tok[npos].val, T_INT); }
			if(skip("string")) { --tkpos; return appendVariable(tok[npos].val, T_STRING); }
			if(skip("double")) { --tkpos; return appendVariable(tok[npos].val, T_DOUBLE); }
		} else { --tkpos; return appendVariable(tok[npos].val, T_INT); }
	} else error("error: %d: can't declare variable", tok[tkpos].nline);
	return NULL;
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

void putNumber(int n) {
	printf("%d", n);
}
void putString(int *n) {
	printf("%s", n);
}
void putln() { printf("\n"); }

void appendMem(int addr) {
	memad.addr[memad.count++] = addr;
}

void freeMem() {
	for(--memad.count; memad.count>=0; --memad.count) {
		free((void*)memad.addr[memad.count]);
		printf("Free: %p\n", (void*)memad.addr[memad.count]);
	}
}

void set_xor128() {
#if defined(WIN32) || defined(WINDOWS)
#else
	w = 12345 * getpid() + (time(0) ^ 0xFFBA1235);
#endif
}

int xor128() {
  static unsigned int x = 123456789;
  static unsigned int y = 362436069;
  static unsigned int z = 521288629;
  unsigned int t;
  t = x ^ (x << 11); x = y; y = z; z = w;
  w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
  return ((int)w < 0) ? -(int)w : (int)w;
}

void *funcTable[] = { (void *) putNumber, (void*) putString, (void*) putln,
	 (void*)malloc, (void*) xor128, (void*) printf, (void*) appendMem };

static int run() {
	printf("size: %dbyte, %.2lf%%\n", ntvCount, ((double)ntvCount / 0xFFFF) * 100.0);
	return ((int (*)(int *, void**))ntvCode)(0, funcTable);
}

int main(int argc, char **argv) {

	init();

	if(argc < 2) {
		char input[0xFFFF] = "";
		char line[0xFF] = "";

		puts("Lit 1.0.0 (C) 2015 maekawatoshiki");
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
		fclose(codefp);
	}

	parser();
	volatile clock_t bgn = clock();
	run();
	volatile clock_t end = clock();
	printf("time: %lfsec\n", (double)(end - bgn) / CLOCKS_PER_SEC);

	dispose();
	return 0;
}
