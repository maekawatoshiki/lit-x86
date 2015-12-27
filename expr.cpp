#include "expr.h"

int is_string_tok() { return tok.tok[tok.pos].type == TOK_STRING; }
int is_number_tok() { return tok.tok[tok.pos].type == TOK_NUMBER; }
int is_ident_tok()  { return tok.tok[tok.pos].type == TOK_IDENT;  }
int is_char_tok() { return tok.tok[tok.pos].type == TOK_CHAR; } 

int expr_entry() { return expr_compare(); }

int32_t expr_compare() {
	int andop=0, orop=0;
	expr_logic();
	while((andop=tok.skip("and") || tok.skip("&")) || (orop=tok.skip("or") || 
				tok.skip("|")) || tok.skip("xor") || tok.skip("^")) {
		genas("push eax");
		expr_logic();
		genas("mov ebx eax");
		genas("pop eax");
		gencode(andop ? 0x21 : orop ? 0x09 : 0x31); gencode(0xd8); // and eax ebx
	}

	return 0;
}

int expr_logic() {
	int32_t lt=0, gt=0, ne=0, eql=0, fle=0;
	expr_add_sub();
	if((lt=tok.skip("<")) || (gt=tok.skip(">")) || (ne=tok.skip("!=")) ||
			(eql=tok.skip("==")) || (fle=tok.skip("<=")) || tok.skip(">=")) {
		genas("push eax");
		expr_add_sub();
		genas("mov ebx eax");
		genas("pop eax");
		gencode(0x39); gencode(0xd8); // cmp %eax, %ebx
		/*
		 * < setl 0x9c
		 * > setg 0x9f
		 * <= setle 0x9e
		 * >= setge 0x9d
		 * == sete 0x94
		 * != setne 0x95
		 */
		gencode(0x0f); gencode(lt ? 0x9c : gt ? 0x9f : ne ? 0x95 : eql ? 0x94 : fle ? 0x9e : 0x9d); gencode(0xc0); // setX al
		gencode(0x0f); gencode(0xb6); gencode(0xc0); // movzx eax al
	}

	return 0;
}

int expr_add_sub() {
	int add;
	expr_mul_div();
	while((add = tok.skip("+")) || tok.skip("-")) {
		genas("push eax");
		expr_mul_div();
		genas("mov ebx eax");  // mov %ebx %eax
		genas("pop eax");
		if(add) { genas("add eax ebx"); }// add %eax %ebx
		else { genas("sub eax ebx"); } // sub %eax %ebx
	}
	return 0;
}

int expr_mul_div() {
	int mul, div;
	expr_primary();
	while((mul = tok.skip("*")) || (div=tok.skip("/")) || tok.skip("%")) {
		genas("push eax");
		expr_primary();
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

int expr_primary() {
	int is_get_addr = 0, ispare = 0;

	if(tok.skip("&")) is_get_addr = 1;

	if(is_number_tok()) {
	
		genas("mov eax %d", atoi(tok.tok[tok.pos++].val.c_str()));
	
	} else if(is_char_tok()) { 
		
		genas("mov eax %d", (int)tok.tok[tok.pos++].val[0]);
	
	} else if(is_string_tok()) { 

		gencode(0xb8); get_string();
		gencode_int32(0x00000000); // mov eax string_address

	} else if(is_ident_tok()) { // variable or inc or dec
	
		std::string name = tok.tok[tok.pos].val, mod_name = "";
		Variable *v; 

		if(tok.is(".", 1)) { // module?
			mod_name = tok.tok[tok.pos++].val; 
			tok.skip(".");
			name = tok.tok[tok.pos].val; 
		}
		
		if(is_asgmt()) {
	
			asgmt();
	
		} else {
		
			SKIP_TOK;

			if((ispare = tok.skip("(")) || is_stdfunc(name, mod_name) || 
					is_func(name, mod_name) || is_func(name, module) || is_lib_module(mod_name)) { // Function?

				if(is_lib_module(mod_name)) { // library function
					if(HAS_PARAMS_FUNC) {
						for(int i = 0; tok.tok[tok.pos].val != ")" && !tok.skip(";"); i++) {
							expr_entry();
							gencode(0x89); gencode(0x44); gencode(0x24); gencode(i * ADDR_SIZE); // mov [esp+ADDR*i], eax
							tok.skip(",");
						} 
					}
					gencode(0xe8); gencode_int32(call_lib_func(name, mod_name) - (uint32_t)&ntvCode[ntvCount] - ADDR_SIZE); // call func
				} else if(is_stdfunc(name, mod_name)) {
					
					make_stdfunc(name, mod_name);
				
				} else if(get_func(name, mod_name) || get_func(name, module) || ispare) {	// user function
				
					func_t *function = get_func(name, mod_name);
					if(function == NULL) 
						function = get_func(name, module);
					if(function == NULL) { // undefined
						size_t params = 0;
						if(HAS_PARAMS_FUNC) { // has arg?
							for(params = 0; tok.tok[tok.pos].val != ")" && !tok.skip(";"); params++) {
								expr_entry();
								genas("push eax");
								tok.skip(",");
							}
						}
						gencode(0xe8); append_undef_func(name, module == "" ? mod_name : module, ntvCount);
						gencode_int32(0x00000000); // call func
						genas("add esp %d", params * ADDR_SIZE);

					} else { // defined
						if(HAS_PARAMS_FUNC) {
							for(size_t i = 0; i < function->params; i++) {
								expr_entry();
								genas("push eax");
								if(!tok.skip(",") && function->params - 1 != i) 
									error("error: %d: expected ','", tok.tok[tok.pos].nline);
							}
						}
						gencode(0xe8); gencode_int32(0xFFFFFFFF - (ntvCount - function->address) - 3); // call func
						genas("add esp %d", function->params * ADDR_SIZE);
					}
				}
		
				if(ispare) 
					if(!tok.skip(")")) 
						error("func: error: %d: expected expression ')'", tok.tok[tok.pos].nline);

			} else if(tok.skip("[")) { // Array?
			
				v = get_var(name , mod_name);
				if(v == NULL) v = get_var(name, module);
				if(v == NULL)
					error("error: %d: '%s' was not declare", tok.tok[tok.pos].nline, name.c_str());
				expr_entry();
				genas("mov ecx eax");

				if(v->loctype == V_LOCAL) {
					gencode(0x8b); gencode(0x55); gencode(256 - ADDR_SIZE * v->id); // mov edx, [ebp - v*4]
				} else if(v->loctype == V_GLOBAL) {
					gencode(0x8b); gencode(0x15); gencode_int32(v->id); // mov edx, GLOBAL_ADDR
				}

				if(v->type == T_INT) {
					gencode(0x8b); gencode(0x04); gencode(0x8a);// mov eax, [edx + ecx * 4]
				} else {
					gencode(0x0f); gencode(0xb6); gencode(0x04); gencode(0x0a);// movzx eax, [edx + ecx]
				}

				if(!tok.skip("]"))
					error("error: %d: expected expression ']'", tok.tok[tok.pos].nline);
			
			} else { // single variable
				
				v = get_var(name, mod_name);
				if(v == NULL) 
					v = get_var(name, module);
				if(v == NULL)
					error("var: error: %d: '%s' was not declare", tok.tok[tok.pos].nline, name.c_str());
				if(v->loctype == V_LOCAL) {
					gencode(0x8b); gencode(0x45);
					gencode(256 - ADDR_SIZE * v->id); // mov eax variable
				} else if(v->loctype == V_GLOBAL) {
					gencode(0xa1); gencode_int32(v->id); // mov eax GLOBAL_ADDR
				}

			}
		}
	} else if(tok.skip("(")) {
		if(is_asgmt()) asgmt(); else expr_compare();
		if(!tok.skip(")"))
			error("error: %d: expected expression ')'", tok.tok[tok.pos].nline);
	} else if(tok.skip(";") || true) error("error: %d: invalid expression", tok.tok[tok.pos].nline);

	while(is_index()) make_index();

	return 0;
}

int is_index() {
	if(tok.is("[")) {
		return 1;
	}
	return 0;
}

int make_index() {
	genas("mov ecx eax");
	tok.skip("["); expr_entry(); tok.skip("]");
	gencode(0x8b); gencode(0x04); gencode(0x81); // mov eax [eax * 4 + ecx]
	return 0;
}

uint32_t call_lib_func(std::string name, std::string mod_name) {
	char lib_func_name[64];

	sprintf(lib_func_name, "%s_%s", mod_name.c_str(), name.c_str());
	return (uint32_t)dlsym(lib_list.lib[get_lib_module(mod_name)->no].handle, lib_func_name);
}
