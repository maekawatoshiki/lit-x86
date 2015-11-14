#include "expr.h"

extern char *module;
extern int ntvCount;

int is_string_tok() { return tok.tok[tok.pos].type == TOK_STRING; }
int is_number_tok() { return tok.tok[tok.pos].type == TOK_NUMBER; }

int expr_entry() { return expr_compare(); }

int32_t expr_compare() {
	int andop=0, orop=0;
	expr_logic();
	while((andop=skip("and") || skip("&")) || (orop=skip("or") || skip("|")) || skip("xor") || skip("^")) {
		genas("push eax");
		expr_logic();
		genas("mov ebx eax");
		genas("pop eax");
		genCode(andop ? 0x21 : orop ? 0x09 : 0x31); genCode(0xd8); // and eax ebx
	}

	return 0;
}

int expr_logic() {
	int32_t lt=0, gt=0, ne=0, eql=0, fle=0;
	expr_add_sub();
	if((lt=skip("<")) || (gt=skip(">")) || (ne=skip("!=")) ||
			(eql=skip("==")) || (fle=skip("<=")) || skip(">=")) {
		genas("push eax");
		expr_add_sub();
		genas("mov ebx eax");
		genas("pop eax");
		genCode(0x39); genCode(0xd8); // cmp %eax, %ebx
		/*
		 * < setl 0x9c
		 * > setg 0x9f
		 * <= setle 0x9e
		 * >= setge 0x9d
		 * == sete 0x94
		 * != setne 0x95
		 */
		genCode(0x0f); genCode(lt ? 0x9c : gt ? 0x9f : ne ? 0x95 : eql ? 0x94 : fle ? 0x9e : 0x9d); genCode(0xc0); // setX al
		genCode(0x0f); genCode(0xb6); genCode(0xc0); // movzx eax al
	}

	return 0;
}

int expr_add_sub() {
	int add;
	expr_mul_div();
	while((add = skip("+")) || skip("-")) {
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
  while((mul = skip("*")) || (div=skip("/")) || skip("%")) {
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
  if(is_number_tok()) { // number?
    genas("mov eax %d", atoi(tok.tok[tok.pos++].val));
	} else if(skip("'")) { // char?
		genas("mov eax %d", tok.tok[tok.pos++].val[0]);
		skip("'");
	} else if(is_string_tok()) { // string?
		genCode(0xb8); getString();
		genCodeInt32(0x00); // mov eax string_address
  } else if(isalpha(tok.tok[tok.pos].val[0])) { // variable or inc or dec
		char *name = tok.tok[tok.pos].val, *mod_name = "";
		Variable *v;
		
		if(strcmp(tok.tok[tok.pos + 1].val, ".") == 0) { // module?
			mod_name = tok.tok[tok.pos++].val; 
			skip(".");
			name = tok.tok[tok.pos].val; 
		}

		if(isassign()) {puts("assignmen");assignment();}
		else {
			skip_tok();
			if(skip("[")) { // Array?
				v = getVariable(name , mod_name);
				if(v == NULL) v = getVariable(name, module);
				if(v == NULL)
					error("error: %d: '%s' was not declared", tok.tok[tok.pos].nline, name);
				expr_entry();
				genas("mov ecx eax");

				if(v->loctype == V_LOCAL) {
					genCode(0x8b); genCode(0x55); genCode(256 - sizeof(int32_t) * v->id); // mov edx, [ebp - v*4]
				} else if(v->loctype == V_GLOBAL) {
					genCode(0x8b); genCode(0x15); genCodeInt32(v->id); // mov edx, GLOBAL_ADDR
				}

				if(v->type == T_INT) {
					genCode(0x8b); genCode(0x04); genCode(0x8a);// mov eax, [edx + ecx * 4]
				} else {
					genCode(0x0f); genCode(0xb6); genCode(0x04); genCode(0x0a);// movzx eax, [edx + ecx]
				}
			
				if(!skip("]"))
					error("error: %d: expected expression ']'", tok.tok[tok.pos].nline);
			
			} else if(skip("(")) { // Function?
				int is_stdfunc = make_stdfunc(name); // make standard function
				
				if(!is_stdfunc) {	// user function
					func_t *function = getFunction(name, mod_name);

					if(function == NULL) 
						function = getFunction(name, module);
					if(isalpha(tok.tok[tok.pos].val[0]) || isdigit(tok.tok[tok.pos].val[0]) ||
						is_string_tok() || !strcmp(tok.tok[tok.pos].val, "(")) { // has arg?
						for(size_t i = 0; i < function->params; i++) {
							expr_entry();
							genas("push eax");
							skip(","); //TODO: add error handler 
						}
					}
					genCode(0xe8); genCodeInt32(0xFFFFFFFF - (ntvCount - function->address) - 3); // call func
					genas("add esp %d", function->params * sizeof(int32_t));
				}
				if(!skip(")")) error("func: error: %d: expected expression ')'", tok.tok[tok.pos].nline);
			} else {
				v = getVariable(name, mod_name);
				if(v == NULL) 
					v = getVariable(name, module);
				if(v == NULL)
					error("var: error: %d: '%s' was not declared", tok.tok[tok.pos].nline, name);
				if(v->loctype == V_LOCAL) {
					genCode(0x8b); genCode(0x45);
					genCode(256 - sizeof(uint32_t) * v->id); // mov eax variable
				} else if(v->loctype == V_GLOBAL) {
					genCode(0xa1); genCodeInt32(v->id); // mov eax GLOBAL_ADDR
				}
			}
		}
	} else if(skip("(")) {
    if(isassign()) assignment(); else expr_compare();
		if(!skip(")"))
		 error("error: %d: expected expression ')'", tok.tok[tok.pos].nline);
  } else if(skip(";") || 1) error("error: %d: invalid expression", tok.tok[tok.pos].nline);
	
	while(isIndex()) make_index();

	return 0;
}

int32_t isIndex() {
	if(!strcmp(tok.tok[tok.pos].val, "[")) {
		return 1;
	}
	return 0;
}

int make_index() {
	genas("mov ecx eax");
	skip("["); expr_compare(); skip("]");
	genCode(0x8b); genCode(0x04); genCode(0x81); // mov eax [eax * 4 + ecx]
	return 0;
}

