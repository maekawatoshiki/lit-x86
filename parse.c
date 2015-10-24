#include "parse.h"

int32_t getString() {
	strings.text[ strings.count ] = 
		calloc(sizeof(char), strlen(tok.tok[tok.pos].val) + 1);
	strcpy(strings.text[strings.count], tok.tok[tok.pos++].val);

	*strings.addr++ = ntvCount;
	return strings.count++;
}

Variable *getVariable(char *name) {
	// loval variable
	for(int i = 0; i < locVar.count; i++) {
		if(strcmp(name, locVar.var[functions.now][i].name) == 0)
			return &locVar.var[functions.now][i];
	}
	// global variable
	for(int i = 0; i < gblVar.count; i++) {
		if(strcmp(name, gblVar.var[i].name) == 0) {
			return &gblVar.var[i];
		}
	}

	return NULL;
}

Variable *appendVariable(char *name, int type) {
	if(functions.inside == IN_FUNC) {
		int32_t sz = 1 + ++locVar.size[functions.now];
		strcpy(locVar.var[functions.now][locVar.count].name, name);
		locVar.var[functions.now][locVar.count].type = type;
		locVar.var[functions.now][locVar.count].id = sz;
		locVar.var[functions.now][locVar.count].loctype = V_LOCAL;

		return &locVar.var[functions.now][locVar.count++];
	} else if(functions.inside == IN_GLOBAL) {
		// global varibale
		strcpy(gblVar.var[gblVar.count].name, name);
		gblVar.var[gblVar.count].type = type;
		gblVar.var[gblVar.count].loctype = V_GLOBAL;
		gblVar.var[gblVar.count].id = (uint32_t)&ntvCode[ntvCount];
		ntvCount += sizeof(int32_t); // type

		return &gblVar.var[gblVar.count++];
	}

	return NULL;
}

func_t *getFunction(char *name) {
	for(int i = 0; i < functions.count; i++) {
		if(strcmp(functions.func[i].name, name) == 0) {
			return &functions.func[i];
		}
	}
	return NULL;
}

func_t *appendFunction(char *name, int address, int args) {
	functions.func[functions.count].address = address;
	functions.func[functions.count].args = args;
	strcpy(functions.func[functions.count].name, name);
	return &functions.func[functions.count++];
}

int32_t make_break() {
	genCode(0xe9); // jmp
	brks.addr = realloc(brks.addr, 4 * (brks.count + 1));
	brks.addr[brks.count] = ntvCount;
	genCodeInt32(0);
	return brks.count++;
}

int32_t make_return() {
	relExpr(); // get argument
	genCode(0xe9); // jmp
	rets.addr = realloc(rets.addr, 4 * (rets.count + 1));
	if(rets.addr == NULL) error("LitSystemError: no enough memory");
	rets.addr[rets.count] = ntvCount;
	genCodeInt32(0);
	return rets.count++;
}


int32_t skip(char *s) {
  if(strcmp(s, tok.tok[tok.pos].val) == 0) {
  	tok.pos++; return 1;
  } else return 0;
}

int32_t error(char *errs, ...) {
	va_list args;
	va_start(args, errs);
		vprintf(errs, args); puts("");
	va_end(args);
	exit(0);
	return 0;
}

int expression(int pos, int status) {
	int isputs = 0;

	if(skip("$")) { // global varibale?
		if(isassign()) assignment();
	} else if(skip("def")) { blocksCount++;
		functionStmt();
	} else if(functions.inside == IN_GLOBAL && strcmp("def", tok.tok[tok.pos+1].val) &&
			strcmp("$", tok.tok[tok.pos+1].val) && strcmp(";", tok.tok[tok.pos+1].val)) {	// main function entry
		functions.inside = IN_FUNC;
		functions.now++;
		appendFunction("main", ntvCount, 0); // append function
		genas("push ebp");
		genas("mov ebp esp");
		uint32_t espBgn = ntvCount + 2; genas("sub esp 0");
		genCode(0x8b); genCode(0x75); genCode(0x0c); // mov esi, 0xc(%ebp)

		eval(0, NON);

		genCode(0x81); genCode(0xc4); genCodeInt32(sizeof(int32_t) * (locVar.size[functions.now] + 6)); // add %esp nn
		genCode(0xc9);// leave
		genCode(0xc3);// ret
		genCodeInt32Insert(sizeof(int32_t) * (locVar.size[functions.now] + 6), espBgn);
		functions.inside = IN_GLOBAL;
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
			uint32_t a = 4;
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
	} else if(skip("return")) {
		make_return();
	} else if(skip("if")) {
		ifStmt();
	} else if(skip("else")) {
		int32_t end;
		genCode(0xe9); end = ntvCount; genCodeInt32(0);// jmp while end
		genCodeInt32Insert(ntvCount - pos - 4, pos);
		eval(end, NON);
		return 1;
	} else if(skip("elsif")) {
		int32_t endif, end;
		genCode(0xe9); endif = ntvCount; genCodeInt32(0);// jmp while end
		genCodeInt32Insert(ntvCount - pos - 4, pos);
		relExpr(); // if condition
		genCode(0x83); genCode(0xf8); genCode(0x00);// cmp eax, 0
		genCode(JNE); genCode(0x05); // jne 5
		genCode(0xe9); end = ntvCount; genCodeInt32(0);// jmp while end
		eval(end, NON);
		genCodeInt32Insert(ntvCount - endif - 4, endif);
		return 1;
	} else if(skip("break")) {
		make_break();
	} else if(skip("end")) { blocksCount--;
		if(status == NON) {
			genCodeInt32Insert(ntvCount - pos - 4, pos);
		} else if(status == BLOCK_FUNC) functions.inside = IN_GLOBAL;
		return 1;
	} else if(!skip(";")) {
		relExpr();
	}

	return 0;
}

int eval(int pos, int status) {
	while(tok.pos < tok.size) {
		if(expression(pos, status)) return 1;
	}
	return 0;
}

int32_t parser() {
	tok.pos = ntvCount = 0;
	strings.addr = calloc(0xFF, sizeof(int32_t));
	uint32_t main_address;
	genCode(0xe9); main_address = ntvCount; genCodeInt32(0);
	eval(0, 0);

	uint32_t addr = getFunction("main")->address;
	genCodeInt32Insert(addr - 5, main_address);

	for(strings.addr--; strings.count; strings.addr--) {
		genCodeInt32Insert((uint32_t)&ntvCode[ntvCount], *strings.addr);
		replaceEscape(strings.text[--strings.count]);
		for(int32_t i = 0; strings.text[strings.count][i]; i++) {
			genCode(strings.text[strings.count][i]);
		} genCode(0); // '\0'
	}
#ifdef NDEBUG
	// Nothing
#else
	for(int32_t i = 0; i < ntvCount; i++)
		printf("%02x", ntvCode[i]);
	puts("");
#endif
	printf("memsz: %d\n", locVar.size[functions.now]);

	return 1;
}



int ifStmt() {
	uint32_t end;
	relExpr(); // if condition
	genCode(0x83); genCode(0xf8); genCode(0x00);// cmp eax, 0
	genCode(JNE); genCode(0x05); // jne 5
	genCode(0xe9); end = ntvCount; genCodeInt32(0);// jmp
	return eval(end, NON);
}

int whileStmt() {
	uint32_t loopBgn = ntvCount, end, stepBgn[2], stepOn = 0;
	relExpr(); // condition
	if(skip(",")) {
		stepOn = 1;
		stepBgn[0] = tok.pos;
		for(; tok.tok[tok.pos].val[0] != ';'; tok.pos++);
	}
	genCode(0x83); genCode(0xf8); genCode(0x00);// cmp eax, 0
	genCode(JNE); genCode(0x05); // jne 5
	genCode(0xe9); end = ntvCount; genCodeInt32(0);// jmp while end
	
	if(skip(":")) expression(0, BLOCK_LOOP);
	else eval(0, BLOCK_LOOP);

	if(stepOn) {
		stepBgn[1] = tok.pos;
		tok.pos = stepBgn[0];
		if(isassign()) assignment();
		tok.pos = stepBgn[1];
	}
	uint32_t n = 0xFFFFFFFF - ntvCount + loopBgn - 4;
	genCode(0xe9); genCodeInt32(n); // jmp n
	genCodeInt32Insert(ntvCount - end - 4, end);

	for(--brks.count; brks.count >= 0; brks.count--) {
		genCodeInt32Insert(ntvCount - brks.addr[brks.count] - 4, brks.addr[brks.count]);
	} brks.count = 0;


	return 0;
}

int32_t functionStmt() {
	int32_t espBgn, argsc = 0;
	char *funcName = tok.tok[tok.pos++].val;
	functions.now++; functions.inside = IN_FUNC;
	if(skip("(")) {
		do { declareVariable(); tok.pos++; argsc++; } while(skip(","));
		skip(")");
	}
	appendFunction(funcName, ntvCount, argsc); // append function
	genas("push ebp");
	genas("mov ebp esp");
	espBgn = ntvCount + 2; genas("sub esp 0"); // align
	int32_t argpos[128], i; for(i = 0; i < argsc; i++) {
		genCode(0x8b); genCode(0x45); genCode(0x08 + (argsc - i - 1) * sizeof(int32_t));
		genCode(0x89); genCode(0x44); genCode(0x24);
		argpos[i] = ntvCount; genCode(0x00);
	}
	eval(0, BLOCK_FUNC);

	for(--rets.count; rets.count >= 0; --rets.count) {
		genCodeInt32Insert(ntvCount - rets.addr[rets.count] - 4, rets.addr[rets.count]);
	} rets.count = 0;

	genas("add esp %u", sizeof(int32_t) * (locVar.size[functions.now] + 6)); // add esp nn
	genCode(0xc9);// leave
	genCode(0xc3);// ret

	genCodeInt32Insert(sizeof(int32_t) * (locVar.size[functions.now] + 6), espBgn);
	for(i = 1; i <= argsc; i++) {
		ntvCode[argpos[i - 1]] = 256 - sizeof(int32_t) * i + (((locVar.size[functions.now] + 6) * sizeof(int32_t)) - 4);
	}

	printf("%s() has %u functions or variables\n", funcName, locVar.size[functions.now] * sizeof(int32_t));

	return 0;
}

int32_t isassign() {
	if(strcmp(tok.tok[tok.pos+1].val, "=") == 0) return 1;
	else if(strcmp(tok.tok[tok.pos+1].val, "++") == 0) return 1;
	else if(strcmp(tok.tok[tok.pos+1].val, "--") == 0) return 1;
	else if(strcmp(tok.tok[tok.pos+1].val, "[") == 0) {
		int32_t i = tok.pos + 2, t = 1;
re:
		while(t) {
			if(strcmp(tok.tok[i].val, "[") == 0) t++;
			if(strcmp(tok.tok[i].val, "]") == 0) t--;
			if(strcmp(tok.tok[i].val, ";") == 0)
				error("index: error: %d: invalid expression", tok.tok[tok.pos].nline);
			i++;
		}
		t = 1;
		if(strcmp(tok.tok[i].val, "[") == 0) { i++; goto re; }
		printf(">%s\n", tok.tok[i].val);
		if(strcmp(tok.tok[i].val, "=") == 0) return 1;
	} else if(strcmp(tok.tok[tok.pos + 1].val, ":") == 0) {
		int32_t i = tok.pos + 3;
		if(strcmp(tok.tok[i].val, "=") == 0) return 1;
	}
	return 0;
}

int32_t assignment() {
	Variable *v = getVariable(tok.tok[tok.pos].val);
	int32_t inc = 0, dec = 0, declare = 0;
	if(v == NULL) { declare++; v = declareVariable(); }
	tok.pos++;
	if(v->loctype == V_LOCAL) {
		if(skip("[")) { // Array?
			relExpr();
			genas("push eax");
			skip("]");
			if(isIndex()) make_index();
			if(skip("=")) {
				relExpr();
				genCode(0x8b); genCode(0x4d);
				genCode(256 -
					(v->type == T_INT ? sizeof(int32_t) :
						v->type == T_STRING ? sizeof(int32_t*) :
						v->type == T_DOUBLE ? sizeof(double) : 4) * v->id); // mov ecx [ebp-n]
				genas("pop edx");
				if(v->type == T_INT) {
					genCode(0x89); genCode(0x04); genCode(0x91); // mov [ecx+edx*4], eax
				} else {
					genCode(0x89); genCode(0x04); genCode(0x11); // mov [ecx+edx], eax
				}
			} else if((inc=skip("++")) || (dec=skip("--"))) {

			} else error("error: %d: invalid assignment", tok.tok[tok.pos].nline);
		} else { // Scalar?
			if(skip("=")) {
				relExpr();
			} else if((inc=skip("++")) || (dec=skip("--"))) {
				genCode(0x8b); genCode(0x45);
				genCode(256 -
					(v->type == T_INT ? sizeof(int32_t) :
						v->type == T_STRING ? sizeof(int32_t*) :
						v->type == T_DOUBLE ? sizeof(double) : 4) * v->id); // mov eax varaible
				genas("push eax");
				if(inc) genCode(0x40); // inc eax
				else if(dec) genCode(0x48); // dec eax
			}
			genCode(0x89); genCode(0x45);
			genCode(256 -
				(v->type == T_INT ? sizeof(int32_t) :
					v->type == T_STRING ? sizeof(int32_t*) :
					v->type == T_DOUBLE ? sizeof(double) : 4) * v->id); // mov var eax
			if(inc || dec) genas("pop eax");
		}
	} else if(v->loctype == V_GLOBAL) {
		if(declare) { // first declare for global variable?
			// assignment only int32_terger
			if(skip("=")) {
				unsigned *m = (unsigned *)v->id; *m = atoi(tok.tok[tok.pos++].val);
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
				} else error("error: %d: invalid assignment", tok.tok[tok.pos].nline);
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

Variable *declareVariable() {
	int32_t npos = tok.pos;
	if(isalpha(tok.tok[tok.pos].val[0])) {
		tok.pos++;
		if(skip(":")) {
			if(skip("int")) { --tok.pos; return appendVariable(tok.tok[npos].val, T_INT); }
			if(skip("string")) { --tok.pos; return appendVariable(tok.tok[npos].val, T_STRING); }
			if(skip("double")) { --tok.pos; return appendVariable(tok.tok[npos].val, T_DOUBLE); }
		} else { --tok.pos; return appendVariable(tok.tok[npos].val, T_INT); }
	} else error("error: %d: can't declare variable", tok.tok[tok.pos].nline);
	return NULL;
}

char *replaceEscape(char *str) {
	int32_t i;
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
