#include "expr.h"

int Parser::is_string_tok() { return tok.get().type == TOK_STRING; }
int Parser::is_number_tok() { return tok.get().type == TOK_NUMBER; }
int Parser::is_ident_tok()  { return tok.get().type == TOK_IDENT;  }
int Parser::is_char_tok() { return tok.get().type == TOK_CHAR; } 

int Parser::expr_entry() { return expr_compare(); }

int Parser::expr_compare() {
	int andop=0, orop=0;
	expr_logic();
	while((andop=tok.skip("and") || tok.skip("&")) || (orop=tok.skip("or") || 
				tok.skip("|")) || tok.skip("xor") || tok.skip("^")) {
		ntv.genas("push eax");
		expr_logic();
		ntv.genas("mov ebx eax");
		ntv.genas("pop eax");
		ntv.gencode(andop ? 0x21 : orop ? 0x09 : 0x31); ntv.gencode(0xd8); // and eax ebx
	}

	return 0;
}

int Parser::expr_logic() {
	int32_t lt=0, gt=0, ne=0, eql=0, fle=0;
	expr_add_sub();
	if((lt=tok.skip("<")) || (gt=tok.skip(">")) || (ne=tok.skip("!=")) ||
			(eql=tok.skip("==")) || (fle=tok.skip("<=")) || tok.skip(">=")) {
		ntv.genas("push eax");
		expr_add_sub();
		ntv.genas("mov ebx eax");
		ntv.genas("pop eax");
		ntv.gencode(0x39); ntv.gencode(0xd8); // cmp %eax, %ebx
		/*
		 * < setl 0x9c
		 * > setg 0x9f
		 * <= setle 0x9e
		 * >= setge 0x9d
		 * == sete 0x94
		 * != setne 0x95
		 */
		ntv.gencode(0x0f); ntv.gencode(lt ? 0x9c : gt ? 0x9f : ne ? 0x95 : eql ? 0x94 : fle ? 0x9e : 0x9d); ntv.gencode(0xc0); // setX al
		ntv.gencode(0x0f); ntv.gencode(0xb6); ntv.gencode(0xc0); // movzx eax al
	}

	return 0;
}

int Parser::expr_add_sub() {
	int add;
	expr_mul_div();
	while((add = tok.skip("+")) || tok.skip("-")) {
		ntv.genas("push eax");
		expr_mul_div();
		ntv.genas("mov ebx eax");  // mov %ebx %eax
		ntv.genas("pop eax");
		if(add) { ntv.genas("add eax ebx"); }// add %eax %ebx
		else { ntv.genas("sub eax ebx"); } // sub %eax %ebx
	}
	return 0;
}

int Parser::expr_mul_div() {
	int mul, div;
	expr_primary();
	while((mul = tok.skip("*")) || (div=tok.skip("/")) || tok.skip("%")) {
		ntv.genas("push eax");
		expr_primary();
		ntv.genas("mov ebx eax"); // mov %ebx %eax
		ntv.genas("pop eax");
		if(mul) {
			ntv.genas("mul ebx");
		} else if(div) {
			ntv.genas("mov edx 0");
			ntv.genas("div ebx");
		} else { //mod
			ntv.genas("mov edx 0");
			ntv.genas("div ebx");
			ntv.genas("mov eax edx");
		}
	}
	return 0;
}

int Parser::expr_primary() {
	int is_get_addr = 0, ispare = 0;

	if(tok.skip("&")) is_get_addr = 1;

	if(is_number_tok()) {
	
		ntv.genas("mov eax %d", atoi(tok.next().val.c_str()));
	
	} else if(is_char_tok()) { 
		
		ntv.genas("mov eax %d", (int)tok.next().val[0]);
	
	} else if(is_string_tok()) { 

		ntv.gencode(0xb8); parse.get_string();
		ntv.gencode_int32(0x00000000); // mov eax string_address

	} else if(is_ident_tok()) { // variable or inc or dec
	
		std::string name = tok.get().val, mod_name = "";
		var_t *v; 

		if(tok.is(".", 1)) { // module?
			mod_name = tok.next().val; 
			tok.skip(".");
			name = tok.get().val; 
		}
		
		if(is_asgmt()) {
	
			asgmt();
	
		} else {
		
			SKIP_TOK;

			if((ispare = tok.skip("(")) || is_stdfunc(name, mod_name) || 
					funcs.is(name, mod_name) || funcs.is(name, module) || lib_list.is(mod_name)) { // Function?

				if(lib_list.is(mod_name)) { // library function
					if(HAS_PARAMS_FUNC) {
						for(int i = 0; !tok.is(")") && !tok.skip(";"); i++) {
							expr_entry();
							ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(i * ADDR_SIZE); // mov [esp+ADDR*i], eax
							tok.skip(",");
						} 
					}
					ntv.gencode(0xe8); ntv.gencode_int32(lib_list.call(name, mod_name) - (uint32_t)&ntv.code[ntv.count] - ADDR_SIZE); // call func
				} else if(is_stdfunc(name, mod_name)) {
					
					make_stdfunc(name, mod_name);
				
				} else if(funcs.get(name, mod_name) || funcs.get(name, module) || ispare) {	// user function
				
					func_t *function = funcs.get(name, mod_name);
					if(function == NULL) 
						function = funcs.get(name, module);
					if(function == NULL) { // undefined
						size_t params = 0;
						if(HAS_PARAMS_FUNC) { // has arg?
							for(params = 0; !tok.is(")") && !tok.skip(";"); params++) {
								expr_entry();
								ntv.genas("push eax");
								tok.skip(",");
							}
						}
						ntv.gencode(0xe8); undef_funcs.append_undef(name, module == "" ? mod_name : module, ntv.count);
						ntv.gencode_int32(0x00000000); // call func
						ntv.genas("add esp %d", params * ADDR_SIZE);

					} else { // defined
						if(HAS_PARAMS_FUNC) {
							for(size_t i = 0; i < function->params; i++) {
								expr_entry();
								ntv.genas("push eax");
								if(!tok.skip(",") && function->params - 1 != i) 
									error("error: %d: expected ','", tok.get().nline);
							}
						}
						ntv.gencode(0xe8); ntv.gencode_int32(0xFFFFFFFF - (ntv.count - function->address) - 3); // call func
						ntv.genas("add esp %d", function->params * ADDR_SIZE);
					}
				}
		
				if(ispare) 
					if(!tok.skip(")")) 
						error("func: error: %d: expected expression ')'", tok.tok[tok.pos].nline);

			} else if(tok.skip("[")) { // Array?
			
				v = var.get(name , mod_name);
				if(v == NULL) v = var.get(name, module);
				if(v == NULL)
					error("error: %d: '%s' was not declare", tok.tok[tok.pos].nline, name.c_str());
				expr_entry();
				ntv.genas("mov ecx eax");

				if(v->loctype == V_LOCAL) {
					ntv.gencode(0x8b); ntv.gencode(0x55); ntv.gencode(256 - ADDR_SIZE * v->id); // mov edx, [ebp - v*4]
				} else if(v->loctype == V_GLOBAL) {
					ntv.gencode(0x8b); ntv.gencode(0x15); ntv.gencode_int32(v->id); // mov edx, GLOBAL_ADDR
				}

				if(v->type == T_INT) {
					ntv.gencode(0x8b); ntv.gencode(0x04); ntv.gencode(0x8a);// mov eax, [edx + ecx * 4]
				} else {
					ntv.gencode(0x0f); ntv.gencode(0xb6); ntv.gencode(0x04); ntv.gencode(0x0a);// movzx eax, [edx + ecx]
				}

				if(!tok.skip("]"))
					error("error: %d: expected expression ']'", tok.tok[tok.pos].nline);
			
			} else { // single variable
				
				v = var.get(name, mod_name);
				if(v == NULL) 
					v = var.get(name, module);
				if(v == NULL)
					error("var: error: %d: '%s' was not declare", tok.tok[tok.pos].nline, name.c_str());
				if(v->loctype == V_LOCAL) {
					ntv.gencode(0x8b); ntv.gencode(0x45);
					ntv.gencode(256 - ADDR_SIZE * v->id); // mov eax variable
				} else if(v->loctype == V_GLOBAL) {
					ntv.gencode(0xa1); ntv.gencode_int32(v->id); // mov eax GLOBAL_ADDR
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

int Parser::is_index() {
	if(tok.is("[")) {
		return 1;
	}
	return 0;
}

int Parser::make_index() {
	ntv.genas("mov ecx eax");
	tok.skip("["); expr_entry(); tok.skip("]");
	ntv.gencode(0x8b); ntv.gencode(0x04); ntv.gencode(0x81); // mov eax [eax * 4 + ecx]
	return 0;
}
