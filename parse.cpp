#include "parse.h"

int blocksCount;
std::string module = "";
FunctionList undef_funcs, funcs;
string_t strings;

int get_string() {
	strings.text[ strings.count ] = (char *)
		calloc(sizeof(char), tok.tok[tok.pos].val.length() + 1);
	strcpy(strings.text[strings.count], tok.tok[tok.pos++].val.c_str());

	*strings.addr++ = ntv.count;
	return strings.count++;
}

bool FunctionList::is(std::string name, std::string mod_name) {
	return get(name, mod_name) == NULL ? false : true;
}

func_t *FunctionList::get(std::string name, std::string mod_name) {
	for(int i = 0; i < func.size(); i++) {
#ifdef DEBUG
		std::cout << mod_name.c_str() << " : " << name.c_str() << " <<< " << func[i].mod_name.c_str() << " : " func[i].name.c_str() << endl;
#endif
		if(func[i].name == name && func[i].mod_name == mod_name) {
			return &(func[i]);
		}
	}
	return NULL;
}

func_t *FunctionList::focus() { return &func[now]; }

func_t *FunctionList::append(std::string name, int address, int params) {
	func_t f = {
		.address = address,
		.params = params,
		.mod_name = module,
		.name = name
	};
	func.push_back(f);
#ifdef DEBUG
	std::cout << func[func.size() - 1].mod_name << " : " << name << std::endl;
#endif
	return &(func[func.size() - 1]);
}

func_t *FunctionList::append_undef(std::string name, std::string mod_name, int ntvc_pos) {
	func_t f = {
		.address = ntvc_pos,
		.mod_name = module,
		.name = name
	};
	func.push_back(f);
	return 0;
}

bool FunctionList::rep_undef(std::string name, int ntvc) {
	std::vector<func_t> &f = func;

	for(int i = 0; i < f.size(); i++) {
		if(f[i].name == name && f[i].mod_name == module) {
			ntv.gencode_int32_insert(ntvc - f[i].address - 4, f[i].address);
		}
	}
	return true;
}

int make_break() {
	ntv.gencode(0xe9); // jmp
	brks.addr = (uint32_t*)realloc(brks.addr, ADDR_SIZE * (brks.count + 1));
	if(brks.addr == NULL) error("LitSystemError: no enough memory");
	brks.addr[brks.count] = ntv.count;
	ntv.gencode_int32(0);
	return brks.count++;
}

int make_return() {
	expr_entry(); // get argument
	ntv.gencode(0xe9); // jmp
	rets.addr = (uint32_t*)realloc(rets.addr, ADDR_SIZE * (rets.count + 1));
	if(rets.addr == NULL) error("LitSystemError: no enough memory");
	rets.addr[rets.count] = ntv.count;
	ntv.gencode_int32(0);
	return rets.count++;
}

int expression(int pos, int status) {
	int isputs = 0;

	if(tok.skip("$")) { // global varibale

		if(is_asgmt()) asgmt();

	} else if(tok.skip("require")) {
	
		using_require();
	
	} else if(tok.skip("def")) { blocksCount++;

		make_func();

	} else if(tok.skip("module")) { blocksCount++;
		module = tok.tok[tok.pos++].val;
		eval(0, NON);
		module = "";
	} else if(funcs.inside == false && !tok.is("def", 1) &&
			!tok.is("module", 1) && !tok.is("$", 1) &&
			!tok.is(";", 1) && module == "") {	// main func entry

		funcs.inside = true;
		funcs.now++;
		funcs.append("main", ntv.count, 0); // append funcs
		ntv.genas("push ebp");
		ntv.genas("mov ebp esp");
		uint32_t espBgn = ntv.count + 2; ntv.genas("sub esp 0");
		ntv.gencode(0x8b); ntv.gencode(0x75); ntv.gencode(0x0c); // mov esi, 0xc(%ebp)
		
		eval(0, BLOCK_NORMAL);

		ntv.gencode(0x81); ntv.gencode(0xc4); ntv.gencode_int32(ADDR_SIZE * (locVar.size[funcs.now] + 6)); // add %esp nn
		ntv.gencode(0xc9);// leave
		ntv.gencode(0xc3);// ret
		ntv.gencode_int32_insert(ADDR_SIZE * (locVar.size[funcs.now] + 6), espBgn);
		funcs.inside = false;

	} else if(is_asgmt()) {

		asgmt();

	} else if((isputs=tok.skip("puts")) || tok.skip("output")) {

		do {
			int isstring = 0;
			if((isstring = is_string_tok())) {
				ntv.gencode(0xb8); get_string(); ntv.gencode_int32(0x00); // mov eax string_address
			} else {
				expr_entry();
			}
			ntv.genas("push eax");
			if(isstring) {
				ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(4);// call *0x04(esi) putString
			} else {
				ntv.gencode(0xff); ntv.gencode(0x16); // call (esi) putNumber
			}
			ntv.genas("add esp 4");
		} while(tok.skip(","));
		// for new line
		if(isputs) {
			ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(8);// call *0x08(esi) putLN
		}

	} else if(tok.skip("for")) { blocksCount++;

		asgmt();
		if(!tok.skip(",")) error("error: %d: expected ','", tok.tok[tok.pos].nline);
		make_while();

	} else if(tok.skip("while")) { blocksCount++;

		make_while();

	} else if(tok.skip("return")) {

		make_return();

	} else if(tok.skip("if")) { blocksCount++;

		make_if();
	
	} else if(tok.skip("else")) {

		uint32_t end;
		ntv.gencode(0xe9); end = ntv.count; ntv.gencode_int32(0);// jmp while end
		ntv.gencode_int32_insert(ntv.count - pos - 4, pos);
		eval(end, BLOCK_NORMAL);
		return 1;

	} else if(tok.skip("elsif")) {

		uint32_t endif, end;
		ntv.gencode(0xe9); endif = ntv.count; ntv.gencode_int32(0);// jmp while end
		ntv.gencode_int32_insert(ntv.count - pos - 4, pos);
		expr_entry(); // if condition
		ntv.gencode(0x83); ntv.gencode(0xf8); ntv.gencode(0x00);// cmp eax, 0
		ntv.gencode(0x75); ntv.gencode(0x05); // jne 5
		tok.skip(";");
		ntv.gencode(0xe9); end = ntv.count; ntv.gencode_int32(0);// jmp while end
		eval(end, BLOCK_NORMAL);
		ntv.gencode_int32_insert(ntv.count - endif - 4, endif);
		return 1;

	} else if(tok.skip("break")) {

		make_break();

	} else if(tok.skip("end")) { blocksCount--;

		if(status == NON) return 1;
		if(status == BLOCK_NORMAL) {
			ntv.gencode_int32_insert(ntv.count - pos - 4, pos);
		} else if(status == BLOCK_FUNC) funcs.inside = false;
		return 1;

	} else if(!tok.skip(";")) {
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
	tok.pos = ntv.count = 0;
	strings.addr = (int32_t*)calloc(0xFF, sizeof(int32_t));
	uint32_t main_address;
	ntv.gencode(0xe9); main_address = ntv.count; ntv.gencode_int32(0);

	blocksCount = 0;
	eval(0, BLOCK_NORMAL);
#ifdef DEBUG
	printf("blocks: %d\n", blocksCount);
#endif
	if(blocksCount != 0) error("error: 'end' is not enough");
	uint32_t addr = funcs.get("main", "")->address;
	ntv.gencode_int32_insert(addr - 5, main_address);

	for(strings.addr--; strings.count; strings.addr--) {
		ntv.gencode_int32_insert((uint32_t)&ntv.code[ntv.count], *strings.addr);
		replaceEscape(strings.text[--strings.count]);
		for(int i = 0; strings.text[strings.count][i]; i++) {
			ntv.gencode(strings.text[strings.count][i]);
		} ntv.gencode(0); // '\0'
	}
#ifdef DEBUG
	for(int i = 0; i < ntv.count; i++)
		printf("%02x", ntv.code[i]);
	puts("");
	printf("memsz: %d\n", funcs.focus()->var.size[funcs.now]);
#endif

	return 1;
}

void using_require() {
	lib_list.append(tok.next().val);
}

int make_if() {
	uint32_t end;
	expr_entry(); // if condition
	ntv.gencode(0x83); ntv.gencode(0xf8); ntv.gencode(0x00);// cmp eax, 0
	ntv.gencode(0x75); ntv.gencode(0x05); // jne 5
	ntv.gencode(0xe9); end = ntv.count; ntv.gencode_int32(0);// jmp
	return eval(end, BLOCK_NORMAL);
}

int make_while() {
	uint32_t loopBgn = ntv.count, end, stepBgn[2], stepOn = 0;

	expr_entry(); // condition
	if(tok.skip(",")) {
		stepOn = 1;
		stepBgn[0] = tok.pos;
		for(; tok.tok[tok.pos].val[0] != ';'; tok.pos++);
	}
	if(!tok.skip(";")) error("error");
	ntv.gencode(0x83); ntv.gencode(0xf8); ntv.gencode(0x00);// cmp eax, 0
	ntv.gencode(0x75); ntv.gencode(0x05); // jne 5
	ntv.gencode(0xe9); end = ntv.count; ntv.gencode_int32(0);// jmp while end

	eval(0, BLOCK_LOOP);

	if(stepOn) {
		stepBgn[1] = tok.pos;
		tok.pos = stepBgn[0];
		expr_entry();
		tok.pos = stepBgn[1];
	}

	uint32_t n = 0xFFFFFFFF - ntv.count + loopBgn - 4;
	ntv.gencode(0xe9); ntv.gencode_int32(n); // jmp n
	ntv.gencode_int32_insert(ntv.count - end - 4, end);

	for(--brks.count; brks.count >= 0; brks.count--) {
		ntv.gencode_int32_insert(ntv.count - brks.addr[brks.count] - 4, brks.addr[brks.count]);
	} brks.count = 0;

	return 0;
}

int make_func() {
	uint32_t espBgn, params = 0;
	std::string funcName = tok.tok[tok.pos++].val;

	funcs.now++; funcs.inside = true;
	if(tok.skip("(")) { // get params
		do { declare_var(); tok.pos++; params++; } while(tok.skip(","));
		tok.skip(")");
	}
	funcs.append(funcName, ntv.count, params);
	undef_funcs.rep_undef(funcName, ntv.count);

	ntv.genas("push ebp");
	ntv.genas("mov ebp esp");
	espBgn = ntv.count + 2; ntv.genas("sub esp 0"); // align

	uint32_t pos_save[128], i;

	for(i = 0; i < params; i++) {
		ntv.gencode(0x8b); ntv.gencode(0x45);
		ntv.gencode(0x08 + (params - i - 1) * ADDR_SIZE);
		ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24);
		pos_save[i] = ntv.count; ntv.gencode(0x00);
	}

	eval(0, BLOCK_FUNC);

	for(--rets.count; rets.count >= 0; --rets.count) {
		ntv.gencode_int32_insert(ntv.count - rets.addr[rets.count] - 4, rets.addr[rets.count]);
	} rets.count = 0;

	ntv.genas("add esp %u", ADDR_SIZE * (locVar.size[funcs.now] + 6)); // add esp nn
	ntv.gencode(0xc9);// leave
	ntv.gencode(0xc3);// ret
	ntv.gencode_int32_insert(ADDR_SIZE * (locVar.size[funcs.now] + 6), espBgn);

	for(i = 1; i <= params; i++) {
		ntv.code[pos_save[i - 1]] =
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
