#include "expr.h"


int32_t relExpr() {
	int32_t lt=0, gt=0, ne=0, eql=0, fle=0;
	addSubExpr();
	if((lt=skip("<")) || (gt=skip(">")) || (ne=skip("!=")) ||
			(eql=skip("==")) || (fle=skip("<=")) || skip(">=")) {
		genas("push eax");
		addSubExpr();
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
		genCode(0x0f); genCode(lt ? 0x9c : gt ? 0x9f : ne ? 0x95 : eql ? 0x94 : fle ? 0x9e : 0x9d); genCode(0xc1); // setX al
		genCode(0x80); genCode(0xe1); genCode(0x01); // and al 1
		genCode(0x0f); genCode(0xb6); genCode(0xc1); // movzx eax al 0f b6 c0
		return lt ? JB : gt ? JA : ne ? JNE : eql ? JE : fle ? JBE : JAE;
	}

	return 0;
}

int32_t addSubExpr() {
	int32_t add;
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

int32_t mulDivExpr() {
  int32_t mul, div;
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

int32_t primExpr() {
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

		if(isassign()) assignment();
		else {
			tkpos++;
			if(skip("[")) { // Array?
		
				if((v = getVariable(name)) == NULL)
					error("error: %d: '%s' was not declared", tok[tkpos].nline, name);
				relExpr();
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
					error("error: %d: expected expression ']'", tok[tkpos].nline);
			
			} else if(skip("(")) { // Function?
				// standard function
				if(strcmp(name, "rand") == 0) {
					genCode(0xff); genCode(0x56); genCode(12 + 4); // call rand
				} else if(strcmp(name, "sleep") == 0) {
					relExpr();
					genCode(0x89); genCode(0x04); genCode(0x24); // mov [esp], eax
					genCode(0xff); genCode(0x56); genCode(28); // call ssleep
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
				} else if(strcmp(name, "fopen") == 0) {
					uint32_t a = 0;
					do {
						relExpr();
						genCode(0x89); genCode(0x44); genCode(0x24); genCode(a); // mov [esp+a], eax
						a += 4;
					} while(skip(","));
					genCode(0xff); genCode(0x56); genCode(32); // call fopen
				} else if(strcmp(name, "fprintf") == 0) {
					uint32_t a = 0;
					do {
						relExpr();
						genCode(0x89); genCode(0x44); genCode(0x24); genCode(a); // mov [esp+a], eax
						a += 4;
					} while(skip(","));
					genCode(0xff); genCode(0x56); genCode(36); // call fprintf
				} else if(strcmp(name, "fgets") == 0) {
					uint32_t a = 0;
					do {
						relExpr();
						genCode(0x89); genCode(0x44); genCode(0x24); genCode(a); // mov [esp+a], eax
						a += 4;
					} while(skip(","));
					genCode(0xff); genCode(0x56); genCode(44); // call fgets
				} else if(strcmp(name, "fclose") == 0) {
					relExpr();
					genCode(0x89); genCode(0x44); genCode(0x24); genCode(0); // mov [esp], eax
					genCode(0xff); genCode(0x56); genCode(40); // call fclose
				} else { // User Function?
					int32_t address = getFunction(name, 0), args = 0;
					printf("addr: %d\n", address);
					if(isalpha(tok[tkpos].val[0]) || isdigit(tok[tkpos].val[0]) ||
						!strcmp(tok[tkpos].val, "\"") || !strcmp(tok[tkpos].val, "(")) { // has arg?
						do {
							relExpr();
							genas("push eax");
							args++;
						} while(skip(","));
					}
					genCode(0xe8); genCodeInt32(0xFFFFFFFF - (ntvCount - address) - 3); // call func
					genas("add esp %d", args * sizeof(int32_t));
				}
				if(!skip(")")) error("func: error: %d: expected expression ')'", tok[tkpos].nline);
			} else {
				if((v = getVariable(name)) == NULL)
					error("var: error: %d: '%s' was not declared", tok[tkpos].nline, name);
				if(v->loctype == V_LOCAL) {
					genCode(0x8b); genCode(0x45);
					genCode(256 - sizeof(int32_t) * v->id); // mov eax variable
				} else if(v->loctype == V_GLOBAL) {
					genCode(0xa1); genCodeInt32(v->id); // mov eax GLOBAL_ADDR
				}
			}
		}
	} else if(skip("(")) {
    if(isassign()) assignment(); else relExpr();
		if(!skip(")"))
		 error("error: %d: expected expression ')'", tok[tkpos].nline);
  }

	while(isArray()) genArray();

	return 0;
}

int32_t isArray() {
	if(strcmp(tok[tkpos].val, "[") == 0) {
		return 1;
	}
	return 0;
}

int genArray() {
	genas("mov ecx eax");
	skip("["); relExpr(); skip("]");
	genCode(0x8b); genCode(0x04); genCode(0x81); // mov eax [eax * 4 + ecx]
	return 0;
}


