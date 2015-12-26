#include "parse.h"

int blocksCount;
std::string module = "";
funclist_t undef_funcs, funcs;
liblist_t lib_list;
string_t strings;

int get_string() {
	strings.text[ strings.count ] = (char *)
		calloc(sizeof(char), tok.tok[tok.pos].val.length() + 1);
	strcpy(strings.text[strings.count], tok.tok[tok.pos++].val.c_str());

	*strings.addr++ = ntvCount;
	return strings.count++;
}

int is_func(std::string name, std::string mod_name) {
	return get_func(name, mod_name) == NULL ? 0 : 1;
}

func_t *get_func(std::string name, std::string mod_name) {
	for(int i = 0; i < funcs.count; i++) {
#ifdef DEBUG
		printf("%s : %s >> %s : %s\n", mod_name.c_str(), name.c_str(), funcs.func[i].mod_name.c_str(), funcs.func[i].name.c_str());
#endif
		if(funcs.func[i].name == name && funcs.func[i].mod_name == mod_name) {
			return &(funcs.func[i]);
		}
	}
	return NULL;
}

func_t *append_func(std::string name, int address, int params) {
	funcs.func[funcs.count].address = address;
	funcs.func[funcs.count].params = params;
	funcs.func[funcs.count].mod_name = module;
#ifdef DEBUG
	std::cout << funcs.func[funcs.count].mod_name << " : " << name << std::endl;
#endif
	funcs.func[funcs.count].name = name;
	return &(funcs.func[funcs.count++]);
}

int append_undef_func(std::string name, std::string mod_name, int ntvc) {
	undef_funcs.func[undef_funcs.count].address = ntvc;
	undef_funcs.func[undef_funcs.count].mod_name = mod_name;
	undef_funcs.func[undef_funcs.count++].name = name;
	return 0;
}

int rep_undef_func(std::string name, int ntvc) {
	func_t *f = undef_funcs.func;

	for(int i = 0; i < undef_funcs.count; i++) {
		if(f[i].name == name && f[i].mod_name == module) {
			gencode_int32_insert(ntvc - f[i].address - 4, f[i].address);
		}
	}
	return 0;
}

int make_break() {
	gencode(0xe9); // jmp
	brks.addr = (uint32_t*)realloc(brks.addr, ADDR_SIZE * (brks.count + 1));
	if(brks.addr == NULL) error("LitSystemError: no enough memory");
	brks.addr[brks.count] = ntvCount;
	gencode_int32(0);
	return brks.count++;
}

int make_return() {
	expr_entry(); // get argument
	gencode(0xe9); // jmp
	rets.addr = (uint32_t*)realloc(rets.addr, ADDR_SIZE * (rets.count + 1));
	if(rets.addr == NULL) error("LitSystemError: no enough memory");
	rets.addr[rets.count] = ntvCount;
	gencode_int32(0);
	return rets.count++;
}

int expression(int pos, int status) {
	int isputs = 0;

	if(skip("$")) { // global varibale

		if(is_asgmt()) asgmt();

	} else if(skip("require")) {
	
		using_require();
	
	} else if(skip("def")) { blocksCount++;

		make_func();

	} else if(skip("module")) { blocksCount++;
		module = tok.tok[tok.pos++].val;
		eval(0, NON);
		module = "";
	} else if(funcs.inside == FALSE && tok.tok[tok.pos+1].val != "def" &&
			tok.tok[tok.pos+1].val != "module" && tok.tok[tok.pos+1].val != "$" &&
			tok.tok[tok.pos+1].val != ";" && module == "") {	// main func entry

		funcs.inside = TRUE;
		funcs.now++;
		append_func("main", ntvCount, 0); // append funcs
		genas("push ebp");
		genas("mov ebp esp");
		uint32_t espBgn = ntvCount + 2; genas("sub esp 0");
		gencode(0x8b); gencode(0x75); gencode(0x0c); // mov esi, 0xc(%ebp)
		
		eval(0, BLOCK_NORMAL);
		
		gencode(0x81); gencode(0xc4); gencode_int32(ADDR_SIZE * (locVar.size[funcs.now] + 6)); // add %esp nn
		gencode(0xc9);// leave
		gencode(0xc3);// ret
		gencode_int32_insert(ADDR_SIZE * (locVar.size[funcs.now] + 6), espBgn);
		funcs.inside = FALSE;

	} else if(is_asgmt()) {

		asgmt();

	} else if((isputs=skip("puts")) || skip("output")) {

		do {
			int isstring = 0;
			if((isstring = is_string_tok())) {
				gencode(0xb8); get_string(); gencode_int32(0x00); // mov eax string_address
			} else {
				expr_entry();
			}
			genas("push eax");
			if(isstring) {
				gencode(0xff); gencode(0x56); gencode(4);// call *0x04(esi) putString
			} else {
				gencode(0xff); gencode(0x16); // call (esi) putNumber
			}
			genas("add esp 4");
		} while(skip(","));
		// for new line
		if(isputs) {
			gencode(0xff); gencode(0x56); gencode(8);// call *0x08(esi) putLN
		}

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

		uint32_t end;
		gencode(0xe9); end = ntvCount; gencode_int32(0);// jmp while end
		gencode_int32_insert(ntvCount - pos - 4, pos);
		eval(end, BLOCK_NORMAL);
		return 1;

	} else if(skip("elsif")) {

		uint32_t endif, end;
		gencode(0xe9); endif = ntvCount; gencode_int32(0);// jmp while end
		gencode_int32_insert(ntvCount - pos - 4, pos);
		expr_entry(); // if condition
		gencode(0x83); gencode(0xf8); gencode(0x00);// cmp eax, 0
		gencode(0x75); gencode(0x05); // jne 5
		skip(";");
		gencode(0xe9); end = ntvCount; gencode_int32(0);// jmp while end
		eval(end, BLOCK_NORMAL);
		gencode_int32_insert(ntvCount - endif - 4, endif);
		return 1;

	} else if(skip("break")) {

		make_break();

	} else if(skip("end")) { blocksCount--;

		if(status == NON) return 1;
		if(status == BLOCK_NORMAL) {
			gencode_int32_insert(ntvCount - pos - 4, pos);
		} else if(status == BLOCK_FUNC) funcs.inside = FALSE;
		return 1;

	} else if(!skip(";")) {
		expr_entry();
	}
	
	return 0;
}

int eval(int pos, int status) {
	while(tok.pos < tok.size) 
		if(expression(pos, status)) return 1;
	return 0;
}

int parser() {
	tok.pos = ntvCount = 0;
	strings.addr = (int32_t*)calloc(0xFF, sizeof(int32_t));
	uint32_t main_address;
	gencode(0xe9); main_address = ntvCount; gencode_int32(0);

	blocksCount = 0;
	eval(0, BLOCK_NORMAL);
#ifdef DEBUG
	printf("blocks: %d\n", blocksCount);
#endif
	if(blocksCount != 0) error("error: 'end' is not enough");
	uint32_t addr = get_func("main", "")->address;
	gencode_int32_insert(addr - 5, main_address);

	for(strings.addr--; strings.count; strings.addr--) {
		gencode_int32_insert((uint32_t)&ntvCode[ntvCount], *strings.addr);
		replaceEscape(strings.text[--strings.count]);
		for(int i = 0; strings.text[strings.count][i]; i++) {
			gencode(strings.text[strings.count][i]);
		} gencode(0); // '\0'
	}
#ifdef DEBUG
	for(int i = 0; i < ntvCount; i++)
		printf("%02x", ntvCode[i]);
	puts("");
	printf("memsz: %d\n", locVar.size[funcs.now]);
#endif

	return 1;
}

void using_require() {
	append_lib(tok.tok[tok.pos++].val);
}

int append_lib(std::string name) {
	lib_t lib;

	lib.name = name;
	lib.no = lib_list.count;
	lib.handle = dlopen(("./lib/" + name + ".so").c_str(), RTLD_LAZY | RTLD_NOW);
	if(lib.handle == NULL) error("LitSystemError: cannot load library (%s)", name.c_str());
	lib_list.lib.push_back(lib);
	return lib_list.count++;
}

int is_lib_module(std::string name) {
	return get_lib_module(name) == NULL ? 0 : 1;
}

lib_t *get_lib_module(std::string name) {
	for(int i = 0; i < lib_list.count; i++) {
		if(lib_list.lib[i].name == name) return &lib_list.lib[i];
	}
	return NULL;
}

void free_lib() {
	for(int i = 0; i < lib_list.count; i++) {
		dlclose(lib_list.lib[i].handle);
	}
}

int make_if() {
	uint32_t end;
	expr_entry(); // if condition
	gencode(0x83); gencode(0xf8); gencode(0x00);// cmp eax, 0
	gencode(0x75); gencode(0x05); // jne 5
	gencode(0xe9); end = ntvCount; gencode_int32(0);// jmp
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
	gencode(0x83); gencode(0xf8); gencode(0x00);// cmp eax, 0
	gencode(0x75); gencode(0x05); // jne 5
	gencode(0xe9); end = ntvCount; gencode_int32(0);// jmp while end

	eval(0, BLOCK_LOOP);

	if(stepOn) {
		stepBgn[1] = tok.pos;
		tok.pos = stepBgn[0];
		expr_entry();
		tok.pos = stepBgn[1];
	}

	uint32_t n = 0xFFFFFFFF - ntvCount + loopBgn - 4;
	gencode(0xe9); gencode_int32(n); // jmp n
	gencode_int32_insert(ntvCount - end - 4, end);

	for(--brks.count; brks.count >= 0; brks.count--) {
		gencode_int32_insert(ntvCount - brks.addr[brks.count] - 4, brks.addr[brks.count]);
	} brks.count = 0;

	return 0;
}

int make_func() {
	uint32_t espBgn, params = 0;
	std::string funcName = tok.tok[tok.pos++].val;

	funcs.now++; funcs.inside = TRUE;
	if(skip("(")) { // get params
		do { declare_var(); tok.pos++; params++; } while(skip(","));
		skip(")");
	}
	append_func(funcName, ntvCount, params);
	rep_undef_func(funcName, ntvCount);

	genas("push ebp");
	genas("mov ebp esp");
	espBgn = ntvCount + 2; genas("sub esp 0"); // align

	uint32_t pos_save[128], i;

	for(i = 0; i < params; i++) {
		gencode(0x8b); gencode(0x45);
		gencode(0x08 + (params - i - 1) * ADDR_SIZE);
		gencode(0x89); gencode(0x44); gencode(0x24);
		pos_save[i] = ntvCount; gencode(0x00);
	}

	eval(0, BLOCK_FUNC);

	for(--rets.count; rets.count >= 0; --rets.count) {
		gencode_int32_insert(ntvCount - rets.addr[rets.count] - 4, rets.addr[rets.count]);
	} rets.count = 0;

	genas("add esp %u", ADDR_SIZE * (locVar.size[funcs.now] + 6)); // add esp nn
	gencode(0xc9);// leave
	gencode(0xc3);// ret

	gencode_int32_insert(ADDR_SIZE * (locVar.size[funcs.now] + 6), espBgn);
	for(i = 1; i <= params; i++) {
		ntvCode[pos_save[i - 1]] =
			256 - ADDR_SIZE * i + (((locVar.size[funcs.now] + 6) * ADDR_SIZE) - 4);
	}
#ifdef DEBUG
	printf("%s() has %u funcs or vars\n", funcName.c_str(), locVar.size[funcs.now]);
#endif

	return 0;
}

void replaceEscape(char *str) {
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
	for (i = 0; i < 12; i += 2) {
		while ((pos = strstr(str, escape[i])) != NULL) {
			*pos = escape[i + 1][0];
			memmove(pos + 1, pos + 2, strlen(str) - 2 + 1);
		}
	}
}
