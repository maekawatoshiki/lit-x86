#include "var.h"
#include "parse.h"
#include "token.h"
#include "asm.h"
#include "util.h"
#include "lit.h"
#include "func.h"


std::vector<var_t> &Variable::focus() { return local[funcs.now]; }

var_t *Variable::get(std::string name, std::string mod_name) {
	// local var
	for(int i = 0; i < local[funcs.now].size(); i++) {
		if(name == local[funcs.now][i].name) {
			return &(local[funcs.now][i]);
		}
	}
	// global var
	for(int i = 0; i < global.size(); i++) {
		if(name == global[i].name && mod_name == global[i].mod_name) {
			return &(global[i]);
		}
	}

	return NULL;
}

var_t * Variable::append(std::string name, int type) {
	if(funcs.inside == true) { // local
		size_t sz = local[funcs.now].size();
		var_t v = {
			.name = name,
			.type = type,
			.id = sz + 2, 
			.loctype = V_LOCAL
		};
		local[funcs.now].push_back(v);
		return &local[funcs.now].back();
	} else if(funcs.inside == false) { // global
		var_t v = {
			.name = name,
			.mod_name = module,
			.type = type,
			.id = (uint32_t)&ntv.code[ntv.count], 
			.loctype = V_GLOBAL
		};
		ntv.count += ADDR_SIZE;
		global.push_back(v);
		return &global.back();
	}
	return NULL;
}

int Parser::is_asgmt() {
	if(tok.is("=", 1)) return 1;
	else if(tok.is("++", 1)) return 1;
	else if(tok.is("--", 1)) return 1;
	else if(tok.is("[", 1)) {
		int i = tok.pos + 2, t = 1;
re:
		while(t) {
			if(tok.at(i).val == "[") t++;
			if(tok.at(i).val == "]") t--;
			if(tok.at(i).val == ";")
				error("index: error: %d: invalid expression", tok.tok[tok.pos].nline);
			i++;
		}
		t = 1;
		if(tok.at(i).val == "[") { i++; goto re; }
#ifdef DEBUG
		std::cout << "> " << tok.tok[i].val << std::endl;
#endif
		if(tok.at(i).val == "=") return 1;
	} else if(tok.is(".", 1) /* module */ || 
			tok.is(":", 1) /* var:type */) {
		if(tok.is("=", 3)) return 1;
	}
	return 0;
}

int Parser::asgmt() {
	std::string name = tok.tok[tok.pos].val, mod_name = "";
	if(tok.is(".", 1)) { // module's func or var?
		mod_name = tok.get().val;
		tok.pos += 2;
		name = tok.get().val;
	}

	int declare = 0;
	var_t *v = var.get(name, mod_name);

	if(v == NULL) v = var.get(name, module);
	if(v == NULL) { declare = 1; v = declare_var(); }
	SKIP_TOK;

	if(v->loctype == V_LOCAL) {
		if(tok.is("[")) { // Array?
			asgmt_array(v);
		} else { // Scalar?
			asgmt_single(v);
		}
	} else if(v->loctype == V_GLOBAL) {
		if(declare) { // declare for global var?
			// asgmt only int32_terger
			if(tok.skip("=")) {
				unsigned *m = (unsigned *)v->id; // v->id is gloval var's address
				*m = atoi(tok.tok[tok.pos++].val.c_str());
			}
		} else {
			if(tok.is("[")) { // Array?
				asgmt_array(v);
			} else asgmt_single(v);
		}
	}
	return 0;
}

int Parser::asgmt_single(var_t *v) {
	int inc = 0, dec = 0;

	if(v->loctype == V_LOCAL) { // local single
		if(tok.skip("=")) {
			expr_entry();
		} else if((inc=tok.skip("++")) || (dec=tok.skip("--"))) {
			ntv.gencode(0x8b); ntv.gencode(0x45);
			ntv.gencode(256 -
					(v->type == T_INT ? ADDR_SIZE :
					 v->type == T_STRING ? ADDR_SIZE :
					 v->type == T_DOUBLE ? sizeof(double) : 4) * v->id); // mov eax varaible
			ntv.genas("push eax");
			if(inc) ntv.gencode(0x40); // inc eax
			else if(dec) ntv.gencode(0x48); // dec eax
		}
		ntv.gencode(0x89); ntv.gencode(0x45);
		ntv.gencode(256 -
				(v->type == T_INT ? ADDR_SIZE :
				 v->type == T_STRING ? ADDR_SIZE :
				 v->type == T_DOUBLE ? sizeof(double) : 4) * v->id); // mov var eax
		if(inc || dec) ntv.genas("pop eax");
	} else if(v->loctype == V_GLOBAL) { // global single
		if(tok.skip("=")) {
			expr_entry();
			ntv.gencode(0xa3); ntv.gencode_int32(v->id); // mov GLOBAL_ADDR eax
		} else if((inc=tok.skip("++")) || (dec=tok.skip("--"))) {
			ntv.gencode(0xa1); ntv.gencode_int32(v->id);// mov eax GLOBAL_ADDR
			ntv.genas("push eax");
			if(inc) ntv.gencode(0x40); // inc eax
			else if(dec) ntv.gencode(0x48); // dec eax
			ntv.gencode(0xa3); ntv.gencode_int32(v->id); // mov GLOBAL_ADDR eax
			ntv.genas("pop eax");
		}
	}
	return 0;
}

int Parser::asgmt_array(var_t *v) {
	int inc = 0, dec = 0;

	if(!tok.skip("[")) error("error: %d: expected '['", tok.tok[tok.pos].nline);
	if(v->loctype == V_LOCAL) {
		expr_entry();
		ntv.genas("push eax");
		if(!tok.skip("]")) error("error: %d: ']' except", tok.tok[tok.pos].nline);
		while(is_index()) make_index();

		if(tok.skip("=")) {
			expr_entry();
			ntv.gencode(0x8b); ntv.gencode(0x4d);
			ntv.gencode(256 -
					(v->type == T_INT ? ADDR_SIZE :
					 v->type == T_STRING ? ADDR_SIZE :
					 v->type == T_DOUBLE ? sizeof(double) : 4) * v->id); // mov ecx [ebp-n]
			ntv.genas("pop edx");
			if(v->type == T_INT) {
				ntv.gencode(0x89); ntv.gencode(0x04); ntv.gencode(0x91); // mov [ecx+edx*4], eax
			} else {
				ntv.gencode(0x89); ntv.gencode(0x04); ntv.gencode(0x11); // mov [ecx+edx], eax
			}
		} else if((inc=tok.skip("++")) || (dec=tok.skip("--"))) {

		} else 
			error("error: %d: invalid asgmt", tok.tok[tok.pos].nline);
	} else if(v->loctype == V_GLOBAL) {
		expr_entry();
		ntv.genas("push eax");
		tok.skip("]");
		if(tok.skip("=")) {

			expr_entry();
			ntv.gencode(0x8b); ntv.gencode(0x0d); ntv.gencode_int32(v->id); // mov ecx GLOBAL_ADDR
			ntv.genas("pop edx");
			if(v->type == T_INT) {
				ntv.gencode(0x89); ntv.gencode(0x04); ntv.gencode(0x91); // mov [ecx+edx*4], eax
			} else {
				ntv.gencode(0x89); ntv.gencode(0x04); ntv.gencode(0x11); // mov [ecx+edx], eax
			}
		
		} else if((inc=tok.skip("++")) || (dec=tok.skip("--"))) {

		} else
			error("error: %d: invalid asgmt", tok.tok[tok.pos].nline);
	}

	return 0;
}

var_t *Parser::declare_var() {
	int npos = tok.pos;

	if(isalpha(tok.tok[tok.pos].val[0])) {
		tok.pos++;
		if(tok.skip(":")) {
			if(tok.skip("int")) { --tok.pos; return var.append(tok.tok[npos].val, T_INT); }
			if(tok.skip("string")) { --tok.pos; return var.append(tok.tok[npos].val, T_STRING); }
			if(tok.skip("double")) { --tok.pos; return var.append(tok.tok[npos].val, T_DOUBLE); }
		} else { --tok.pos; return var.append(tok.tok[npos].val, T_INT); }
	} else error("error: %d: can't declare var", tok.tok[tok.pos].nline);
	return NULL;
}


