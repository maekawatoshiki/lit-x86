#include "parse.h"

char *module = "";
int blocksCount;
extern unsigned char *ntvCode;
extern int ntvCount;

void skip_tok() { tok.pos++; }

int streql(char *s1, char *s2) {
	return (strcmp(s1, s2) == 0) ? 1 : 0;
}

int getString() {
	strings.text[ strings.count ] = (char *)
		calloc(sizeof(char), strlen(tok.tok[tok.pos].val) + 1);
	strcpy(strings.text[strings.count], tok.tok[tok.pos++].val);

	*strings.addr++ = ntvCount;
	return strings.count++;
}

func_t *get_func(char *name, char *mod_name) {
	for(int i = 0; i < funcs.count; i++) {
		printf("%s : %s >> %s : %s\n", mod_name, name, funcs.func[i].mod_name, funcs.func[i].name);
		if(streql(funcs.func[i].name, name) && streql(funcs.func[i].mod_name, mod_name)) {
			return &funcs.func[i];
		}
	}
	return NULL;
}

func_t *append_func(char *name, int address, int params) {
	funcs.func[funcs.count].address = address;
	funcs.func[funcs.count].params = params;
	strcpy(funcs.func[funcs.count].mod_name, module);
	printf("%s:%s\n", funcs.func[funcs.count].mod_name, name);
	strcpy(funcs.func[funcs.count].name, name);
	return &funcs.func[funcs.count++];
}

int append_undef_func(char *name, char *mod_name, int ntvc) {
	undef_funcs.func[undef_funcs.count].address = ntvc;
	strcpy(undef_funcs.func[undef_funcs.count].mod_name, mod_name);
	strcpy(undef_funcs.func[undef_funcs.count++].name, name);
	return 0;
}

int is_undef_func(char *name, int ntvc) {
	int count = undef_funcs.count;
	func_t *f = undef_funcs.func;
	for(int i = 0; i < undef_funcs.count; i++) {
		if(streql(f[i].name, name) && streql(f[i].mod_name, module)) {
			genCodeInt32Insert(ntvc - f[i].address - 4, f[i].address);
		}
	}
	return 0;
}

int make_break() {
	genCode(0xe9); // jmp
	brks.addr = (uint32_t*)realloc(brks.addr, ADDR_SIZE * (brks.count + 1));
	if(brks.addr == NULL) error("LitSystemError: no enough memory");
	brks.addr[brks.count] = ntvCount;
	genCodeInt32(0);
	return brks.count++;
}

int make_return() {
	expr_entry(); // get argument
	genCode(0xe9); // jmp
	rets.addr = (uint32_t*)realloc(rets.addr, ADDR_SIZE * (rets.count + 1));
	if(rets.addr == NULL) error("LitSystemError: no enough memory");
	rets.addr[rets.count] = ntvCount;
	genCodeInt32(0);
	return rets.count++;
}


int skip(char *s) {
  if(streql(s, tok.tok[tok.pos].val)) {
  	tok.pos++; return 1;
  } else return 0;
}

int error(char *errs, ...) {
	va_list args;
	va_start(args, errs);
		vprintf(errs, args); puts("");
	va_end(args);
	exit(0);
	return 0;
}

int expression(int pos, int status) {
	int isputs = 0;

	if(skip("$")) { // global varibale

		if(is_asgmt()) asgmt();

	} else if(skip("def")) { blocksCount++;

		make_func();

	} else if(skip("module")) { blocksCount++;

		module = tok.tok[tok.pos++].val;
		eval(0, NON);
		module = "";
	} else if(funcs.inside == IN_GLOBAL && !streql("def", tok.tok[tok.pos+1].val) && 
			!streql("module", tok.tok[tok.pos+1].val) && !streql("$", tok.tok[tok.pos+1].val) && 
			!streql(";", tok.tok[tok.pos+1].val)) {	// main func entry

		funcs.inside = IN_FUNC;
		funcs.now++;
		append_func("main", ntvCount, 0); // append func
		genas("push ebp");
		genas("mov ebp esp");
		uint32_t espBgn = ntvCount + 2; genas("sub esp 0");
		genCode(0x8b); genCode(0x75); genCode(0x0c); // mov esi, 0xc(%ebp)

		eval(0, BLOCK_NORMAL);

		genCode(0x81); genCode(0xc4); genCodeInt32(ADDR_SIZE * (locVar.size[funcs.now] + 6)); // add %esp nn
		genCode(0xc9);// leave
		genCode(0xc3);// ret
		genCodeInt32Insert(ADDR_SIZE * (locVar.size[funcs.now] + 6), espBgn);
		funcs.inside = IN_GLOBAL;
	
	} else if(is_asgmt()) {
	
		asgmt();
	
	} else if((isputs=skip("puts")) || skip("output")) {
	
		do {
			int isstring = 0;
			if(is_string_tok()) {
				genCode(0xb8); getString();
				genCodeInt32(0x00); // mov eax string_address
				isstring = 1;
			} else {
				expr_entry();
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
	
		if(is_string_tok()) {
			genCode(0xb8); getString();
			genCodeInt32(0x00); // mov eax string_address
			genCode(0x89); genCode(0x44); genCode(0x24); genCode(0x00); // mov [esp+0], eax
		}
		if(skip(",")) {
			uint32_t params = 4;
			do {
				expr_entry();
				genCode(0x89); genCode(0x44); genCode(0x24); genCode(params); // mov [esp+params], eax
				params += 4;
			} while(skip(","));
		}
		genCode(0xff); genCode(0x56); genCode(12 + 8); // call printf
	
	} else if(skip("for")) { blocksCount++;
	
		asgmt(); 
		if(!skip(",")) error("error: %d: expected ','", tok.tok[tok.pos].nline);
		make_while();
	
	} else if(skip("while")) { blocksCount++;
	
		make_while();
	
	} else if(skip("return")) {
	
		make_return();
	
	} else if(skip("if")) { blocksCount++;
	
		make_if();

	} else if(skip("else")) {
	
		int32_t end;
		genCode(0xe9); end = ntvCount; genCodeInt32(0);// jmp while end
		genCodeInt32Insert(ntvCount - pos - 4, pos);
		eval(end, BLOCK_NORMAL);
		return 1;

	} else if(skip("elsif")) {
	
		int32_t endif, end;
		genCode(0xe9); endif = ntvCount; genCodeInt32(0);// jmp while end
		genCodeInt32Insert(ntvCount - pos - 4, pos);
		expr_entry(); // if condition
		genCode(0x83); genCode(0xf8); genCode(0x00);// cmp eax, 0
		genCode(0x75); genCode(0x05); // jne 5
		genCode(0xe9); end = ntvCount; genCodeInt32(0);// jmp while end
		eval(end, BLOCK_NORMAL);
		genCodeInt32Insert(ntvCount - endif - 4, endif);
		return 1;

	} else if(skip("break")) {
	
		make_break();
	
	} else if(skip("end")) { blocksCount--;
	
		if(status == NON) return 1;
		if(status == BLOCK_NORMAL) {
			genCodeInt32Insert(ntvCount - pos - 4, pos);
		} else if(status == BLOCK_FUNC) funcs.inside = IN_GLOBAL;
		return 1;
	
	} else if(!skip(";")) {
		expr_entry();
	}

	return 0;
}

int eval(int pos, int status) {
	while(tok.pos < tok.size) {
		skip(";");
		if(expression(pos, status)) return 1;
	}
	return 0;
}

int32_t parser() {
	tok.pos = ntvCount = 0;
	strings.addr = (int32_t*)calloc(0xFF, sizeof(int32_t));
	uint32_t main_address;
	genCode(0xe9); main_address = ntvCount; genCodeInt32(0);

	blocksCount = 0;
	eval(0, BLOCK_NORMAL);
	printf("blocks: %d\n", blocksCount);
	if(blocksCount != 0) error("error: 'end' is not enough");

	uint32_t addr = get_func("main", "")->address;
	genCodeInt32Insert(addr - 5, main_address);

	for(strings.addr--; strings.count; strings.addr--) {
		genCodeInt32Insert((uint32_t)&ntvCode[ntvCount], *strings.addr);
		replaceEscape(strings.text[--strings.count]);
		for(int i = 0; strings.text[strings.count][i]; i++) {
			genCode(strings.text[strings.count][i]);
		} genCode(0); // '\0'
	}
#ifdef NDEBUG
	// Nothing
#else
	for(int i = 0; i < ntvCount; i++)
		printf("%02x", ntvCode[i]);
	puts("");
#endif
	printf("memsz: %d\n", locVar.size[funcs.now]);

	return 1;
}



int make_if() {
	uint32_t end;
	expr_entry(); // if condition
	genCode(0x83); genCode(0xf8); genCode(0x00);// cmp eax, 0
	genCode(0x75); genCode(0x05); // jne 5
	genCode(0xe9); end = ntvCount; genCodeInt32(0);// jmp
	return eval(end, BLOCK_NORMAL);
}

int make_while() {
	uint32_t loopBgn = ntvCount, end, stepBgn[2], stepOn = 0;
	
	expr_entry(); // condition
	if(skip(",")) {
		stepOn = 1;
		stepBgn[0] = tok.pos;
		for(; tok.tok[tok.pos].val[0] != ';'; tok.pos++);
	}
	if(!skip(";")) error("error");
	genCode(0x83); genCode(0xf8); genCode(0x00);// cmp eax, 0
	genCode(0x75); genCode(0x05); // jne 5
	genCode(0xe9); end = ntvCount; genCodeInt32(0);// jmp while end
	
	eval(0, BLOCK_LOOP);

	if(stepOn) {
		stepBgn[1] = tok.pos;
		tok.pos = stepBgn[0];
		expr_entry();
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

int make_func() {
	uint32_t espBgn, params = 0;
	char *funcName = tok.tok[tok.pos++].val;

	funcs.now++; funcs.inside = IN_FUNC;
	if(skip("(")) { // get params
		do { declare_var(); tok.pos++; params++; } while(skip(","));
		skip(")");
	}
	append_func(funcName, ntvCount, params); // append funcs
	is_undef_func(funcName, ntvCount);
	
	genas("push ebp");
	genas("mov ebp esp");
	espBgn = ntvCount + 2; genas("sub esp 0"); // align
	
	uint32_t pos_save[128], i; 
	
	for(i = 0; i < params; i++) {
		genCode(0x8b); genCode(0x45); 
		genCode(0x08 + (params - i - 1) * ADDR_SIZE);
		genCode(0x89); genCode(0x44); genCode(0x24);
		pos_save[i] = ntvCount; genCode(0x00);
	}

	eval(0, BLOCK_FUNC);

	for(--rets.count; rets.count >= 0; --rets.count) {
		genCodeInt32Insert(ntvCount - rets.addr[rets.count] - 4, rets.addr[rets.count]);
	} rets.count = 0;

	genas("add esp %u", ADDR_SIZE * (locVar.size[funcs.now] + 6)); // add esp nn
	genCode(0xc9);// leave
	genCode(0xc3);// ret

	genCodeInt32Insert(ADDR_SIZE * (locVar.size[funcs.now] + 6), espBgn);
	for(i = 1; i <= params; i++) {
		ntvCode[pos_save[i - 1]] = 
			256 - ADDR_SIZE * i + (((locVar.size[funcs.now] + 6) * ADDR_SIZE) - 4);
	}

	printf("%s() has %u funcs or vars\n", funcName, locVar.size[funcs.now]);

	return 0;
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
