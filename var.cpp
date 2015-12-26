#include "var.h"

extern int blocksCount;
locvar_t locVar;
gblvar_t gblVar;

Variable *get_var(std::string name, std::string mod_name) {
	// local var
	for(int i = 0; i < locVar.count; i++) {
		if(name == locVar.var[funcs.now][i].name)
			return &locVar.var[funcs.now][i];
	}
	// global var
	for(int i = 0; i < gblVar.count; i++) {
		if(name == gblVar.var[i].name && mod_name == gblVar.var[i].mod_name) {
			return &gblVar.var[i];
		}
	}

	return NULL;
}

Variable *append_var(std::string name, int type) {
	if(funcs.inside == TRUE) {
		// local var
		uint32_t sz = 1 + ++locVar.size[funcs.now];
		locVar.var[funcs.now][locVar.count].name = name;
		locVar.var[funcs.now][locVar.count].type = type;
		locVar.var[funcs.now][locVar.count].id = sz;
		locVar.var[funcs.now][locVar.count].loctype = V_LOCAL;

		return &locVar.var[funcs.now][locVar.count++];
	} else if(funcs.inside == FALSE) {
		// global varibale
		gblVar.var[gblVar.count].name = name;
		gblVar.var[gblVar.count].mod_name = module;
		gblVar.var[gblVar.count].type = type;
		gblVar.var[gblVar.count].loctype = V_GLOBAL;
		gblVar.var[gblVar.count].id = (uint32_t)&ntvCode[ntvCount];
		ntvCount += ADDR_SIZE;

		return &gblVar.var[gblVar.count++];
	}

	return NULL;
}

int is_asgmt() {
	if(tok.tok[tok.pos+1].val == "=") return 1;
	else if(tok.tok[tok.pos+1].val == "++") return 1;
	else if(tok.tok[tok.pos+1].val == "--") return 1;
	else if(tok.tok[tok.pos+1].val == "[") {
		int i = tok.pos + 2, t = 1;
re:
		while(t) {
			if(tok.tok[i].val == "[") t++;
			if(tok.tok[i].val == "]") t--;
			if(tok.tok[i].val == ";")
				error("index: error: %d: invalid expression", tok.tok[tok.pos].nline);
			i++;
		}
		t = 1;
		if(tok.tok[i].val == "[") { i++; goto re; }
#ifdef DEBUG
		std::cout << "> " << tok.tok[i].val << std::endl;
#endif
		if(tok.tok[i].val == "=") return 1;
	} else if(tok.tok[tok.pos + 1].val =="." /* module */ || 
			tok.tok[tok.pos + 1].val == ":" /* var:type */) {
		if(tok.tok[tok.pos + 3].val == "=") return 1;
	}
	return 0;
}

int asgmt() {
	std::string name = tok.tok[tok.pos].val, mod_name = "";
	if(tok.tok[tok.pos+1].val == ".") { // module's func or var?
		mod_name = tok.tok[tok.pos].val;
		tok.pos += 2;
		name = tok.tok[tok.pos].val;
	}

	int declare = 0;
	Variable *v = get_var(name, mod_name);
	if(v == NULL) v = get_var(name, module);
	if(v == NULL) { declare = 1; v = declare_var(); }
	SKIP_TOK;
	
	if(v->loctype == V_LOCAL) {
		if(tok.tok[tok.pos].val == "[") { // Array?
			asgmt_array(v);
		} else { // Scalar?
			asgmt_single(v);
		}
	} else if(v->loctype == V_GLOBAL) {
		if(declare) { // declare for global var?
			// asgmt only int32_terger
			if(skip("=")) {
				unsigned *m = (unsigned *)v->id; // v->id is gloval var's address
				*m = atoi(tok.tok[tok.pos++].val.c_str());
			}
		} else {
			if(tok.tok[tok.pos].val == "[") { // Array?
				asgmt_array(v);
			} else asgmt_single(v);
		}
	}
	return 0;
}

int asgmt_single(Variable *v) {
	int inc = 0, dec = 0;

	if(v->loctype == V_LOCAL) { // local single
		if(skip("=")) {
			expr_entry();
		} else if((inc=skip("++")) || (dec=skip("--"))) {
			gencode(0x8b); gencode(0x45);
			gencode(256 -
					(v->type == T_INT ? ADDR_SIZE :
					 v->type == T_STRING ? ADDR_SIZE :
					 v->type == T_DOUBLE ? sizeof(double) : 4) * v->id); // mov eax varaible
			genas("push eax");
			if(inc) gencode(0x40); // inc eax
			else if(dec) gencode(0x48); // dec eax
		}
		gencode(0x89); gencode(0x45);
		gencode(256 -
				(v->type == T_INT ? ADDR_SIZE :
				 v->type == T_STRING ? ADDR_SIZE :
				 v->type == T_DOUBLE ? sizeof(double) : 4) * v->id); // mov var eax
		if(inc || dec) genas("pop eax");
	} else if(v->loctype == V_GLOBAL) { // global single
		if(skip("=")) {
			expr_entry();
			gencode(0xa3); gencode_int32(v->id); // mov GLOBAL_ADDR eax
		} else if((inc=skip("++")) || (dec=skip("--"))) {
			gencode(0xa1); gencode_int32(v->id);// mov eax GLOBAL_ADDR
			genas("push eax");
			if(inc) gencode(0x40); // inc eax
			else if(dec) gencode(0x48); // dec eax
			gencode(0xa3); gencode_int32(v->id); // mov GLOBAL_ADDR eax
			genas("pop eax");
		}
	}
	return 0;
}

int asgmt_array(Variable *v) {
	int inc = 0, dec = 0;

	if(!skip("[")) error("error: %d: expected '['", tok.tok[tok.pos].nline);
	if(v->loctype == V_LOCAL) {
		expr_entry();
		genas("push eax");
		if(!skip("]")) error("error: %d: ']' except", tok.tok[tok.pos].nline);
		while(is_index()) make_index();

		if(skip("=")) {
			expr_entry();
			gencode(0x8b); gencode(0x4d);
			gencode(256 -
					(v->type == T_INT ? ADDR_SIZE :
					 v->type == T_STRING ? ADDR_SIZE :
					 v->type == T_DOUBLE ? sizeof(double) : 4) * v->id); // mov ecx [ebp-n]
			genas("pop edx");
			if(v->type == T_INT) {
				gencode(0x89); gencode(0x04); gencode(0x91); // mov [ecx+edx*4], eax
			} else {
				gencode(0x89); gencode(0x04); gencode(0x11); // mov [ecx+edx], eax
			}
		} else if((inc=skip("++")) || (dec=skip("--"))) {

		} else 
			error("error: %d: invalid asgmt", tok.tok[tok.pos].nline);
	} else if(v->loctype == V_GLOBAL) {
		expr_entry();
		genas("push eax");
		skip("]");
		if(skip("=")) {

			expr_entry();
			gencode(0x8b); gencode(0x0d); gencode_int32(v->id); // mov ecx GLOBAL_ADDR
			genas("pop edx");
			if(v->type == T_INT) {
				gencode(0x89); gencode(0x04); gencode(0x91); // mov [ecx+edx*4], eax
			} else {
				gencode(0x89); gencode(0x04); gencode(0x11); // mov [ecx+edx], eax
			}
		
		} else if((inc=skip("++")) || (dec=skip("--"))) {

		} else
			error("error: %d: invalid asgmt", tok.tok[tok.pos].nline);
	}

	return 0;
}

Variable *declare_var() {
	int npos = tok.pos;
	
	if(isalpha(tok.tok[tok.pos].val[0])) {
		tok.pos++;
		if(skip(":")) {
			if(skip("int")) { --tok.pos; return append_var(tok.tok[npos].val, T_INT); }
			if(skip("string")) { --tok.pos; return append_var(tok.tok[npos].val, T_STRING); }
			if(skip("double")) { --tok.pos; return append_var(tok.tok[npos].val, T_DOUBLE); }
		} else { --tok.pos; return append_var(tok.tok[npos].val, T_INT); }
	} else error("error: %d: can't declare var", tok.tok[tok.pos].nline);
	return NULL;
}

