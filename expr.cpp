#include "expr.h"
#include "lit.h"
#include "asm.h"
#include "lex.h"
#include "token.h"
#include "parse.h"
#include "stdfunc.h"
#include "util.h"
#include "library.h"
#include "var.h"
#include "func.h"
#include "ast.h"

int Parser::is_string_tok() { return tok.get().type == TOK_STRING; }
int Parser::is_number_tok() { return tok.get().type == TOK_NUMBER; }
int Parser::is_ident_tok()  { return tok.get().type == TOK_IDENT;  }
int Parser::is_char_tok() { return tok.get().type == TOK_CHAR; } 

int visit(AST *ast) {
	if(ast->get_type() == AST_BINARY) {
		std::cout << "(" << ((BinaryAST *)ast)->op << " ";
			visit(((BinaryAST *)ast)->left);
			visit(((BinaryAST *)ast)->right);
			std::cout << ")";
	} else if(ast->get_type() == AST_NUMBER) {
		std::cout << " " << ((NumberAST *)ast)->number << " ";
	} else if(ast->get_type() == AST_STRING) {
		std::cout << " " << ((StringAST *)ast)->str << " ";
	}
	return 0;
}

ExprType Parser::expr_entry() { 
	ExprType et;
	AST *node = expr_compare(et);
	visit(node);
	return et;
}

AST *Parser::expr_compare(ExprType &et) {
	int andop=0, orop=0;
	AST *l, *r;
	l = expr_logic(et);
	while((andop=tok.skip("and") || tok.skip("&")) || (orop=tok.skip("or") || 
				tok.skip("|")) || tok.skip("xor") || tok.skip("^")) {
		r = expr_logic(et);
		l = new BinaryAST(andop ? "and" : orop ? "or" : "xor", l, r);
	}

	return l;
}

AST *Parser::expr_logic(ExprType &et) {
	int32_t lt=0, gt=0, ne=0, eql=0, fle=0;
	AST *l, *r;
	l = expr_add_sub(et);
	if((lt=tok.skip("<")) || (gt=tok.skip(">")) || (ne=tok.skip("!=")) ||
			(eql=tok.skip("==")) || (fle=tok.skip("<=")) || tok.skip(">=")) {
		r = expr_add_sub(et);
		l = new BinaryAST(lt ? "<" : gt ? ">" : ne ? "!=" : eql ? "==" : fle ? "<=" : "!", l, r);
	}

	return l;
}

AST *Parser::expr_add_sub(ExprType &et) {
	int add = 0, concat = 0;
	AST *l, *r;
	l = expr_mul_div(et);
	while((add = tok.skip("+")) || (concat = tok.skip("~")) || tok.skip("-")) {
		r = expr_mul_div(et);
		l = new BinaryAST(add ? "+" : concat ? "~" : "-", l, r);
	}
	return l;
}

AST *Parser::expr_mul_div(ExprType &et) {
	int mul, div;
	AST *l, *r;
	l = expr_primary(et);
	while((mul = tok.skip("*")) || (div=tok.skip("/")) || tok.skip("%")) {
		r = expr_primary(et);
		l = new BinaryAST(mul ? "*" : div ? "/" : "%", l, r);
	}
	return l;
}

AST *Parser::expr_primary(ExprType &et) {
	int is_get_addr = 0, ispare = 0;
	std::string name, mod_name = "";
	var_t *v = NULL; 
	
	if(tok.skip("&")) is_get_addr = 1;

	if(is_number_tok()) {
	
		return new NumberAST(atoi(tok.next().val.c_str()));
	
	} else if(is_char_tok()) { 
		
		ntv.genas("mov eax %d", (int)tok.next().val[0]);
		et.change(T_INT);	
	
	} else if(is_string_tok()) { 

		return new StringAST(tok.next().val);

	} else if(is_ident_tok()) { // variable or inc or dec
	
		name = tok.get().val; mod_name = "";

		if(tok.is(":", 1)) { // module?
			mod_name = tok.next().val; 
			tok.skip(":");
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
				et.change(T_INT);
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
				et.change(v->type);
			}
		}
	} else if(tok.skip("(")) {
		if(is_asgmt()) asgmt(); else expr_entry();
		if(!tok.skip(")"))
			error("error: %d: expected expression ')'", tok.get().nline);
	} else if(!make_array(et)) error("error: %d: invalid expression", tok.get().nline);

	while(tok.skip(".")) {
		name = tok.get().val;
		std::string class_name;
		if(v == NULL) {
			if(et.is_type(T_INT)) class_name = "Math";
			else if(et.is_type(T_STRING)) class_name = "String";
			else class_name = mod_name;
		} else class_name = v->class_type;
		tok.skip();
		if(lib_list.is(class_name)) {
			ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(0x00); // mov [esp], eax
			if(HAS_PARAMS_FUNC) {
				tok.skip("(");
				for(size_t i = 0; !tok.skip(")") && !tok.skip(";"); i++) {
					expr_entry();
					ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode((i + 1) * ADDR_SIZE); // mov [esp+ADDR*(i+1)], eax
					tok.skip(",");
				}
			} 
			ntv.gencode(0xe8); ntv.gencode_int32(lib_list.call(name, class_name) - (uint32_t)&ntv.code[ntv.count] - ADDR_SIZE); // call func
			et.change(T_INT);
		} else {
			func_t *function = funcs.get(name, class_name);
			if(function == NULL) 
				function = funcs.get(name, module);
			if(function == NULL) error("function not found");
			if(function->params > 0) ntv.genas("push eax");
			if(HAS_PARAMS_FUNC) {
				tok.skip("(");
				for(size_t i = 0; i < function->params - 1; i++) {
					expr_entry();
					ntv.genas("push eax");
					tok.skip(",");
				}
			} tok.skip(")");
			ntv.gencode(0xe8); ntv.gencode_int32(0xFFFFFFFF - (ntv.count - function->address) - 3); // call func
			ntv.genas("add esp %d", function->params * ADDR_SIZE);
			et.change(T_INT);
		}
	} 

	while(is_index()) make_index(et);
	return 0;
}

int Parser::is_index() {
	if(tok.is("[")) {
		return 1;
	}
	return 0;
}

int Parser::make_index(ExprType &et) {
	ntv.genas("mov edx eax");
	tok.skip("["); expr_entry(); tok.skip("]");
	ntv.genas("mov ecx eax");
	if(et.is_array()) { // TODO: create ExprType::is
		ntv.gencode(0x8b); ntv.gencode(0x04); ntv.gencode(0x8a);// mov eax, [edx + ecx * 4]
		if(et.is_type(T_INT)) et.change(T_INT);
		else if(et.is_type(T_STRING)) et.change(T_STRING);
	} else if(et.is_type(T_STRING)) {
		ntv.gencode(0x0f); ntv.gencode(0xb6); ntv.gencode(0x04); ntv.gencode(0x0a);// movzx eax, [edx + ecx]
		et.change(T_INT); // TODO: create type "char"
	}
	return 0;
}

int Parser::make_array(ExprType &et) {
	if(tok.skip("[")) {
		int elems = 0, pos = tok.pos;
		{ // count elements
			int pare = 1;
			while(pare) {
				if(tok.at(pos).val == "[") pare++;
				if(tok.at(pos).val == "]") pare--;
				if(tok.at(pos++).val == "," && pare == 1) elems++;
			} elems++;
		}
		{ // allocate memory
			ntv.genas("mov eax %d", elems * ADDR_SIZE + ADDR_SIZE);
			ntv.gencode(0x89); ntv.gencode(0x04); ntv.gencode(0x24); // mov [esp], eax
			ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(12); // call malloc
			ntv.genas("push eax");
			ntv.gencode(0x89); ntv.gencode(0x04); ntv.gencode(0x24); // mov [esp], eax
			ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(20); // call append_addr
		} // stack top is allocated address
		ExprType elem_type;	
		for(int elem = 0; elem < elems; elem++)  {
			elem_type = expr_entry();
			ntv.genas("pop ecx"); // mem address 
			ntv.genas("mov edx %d", elem);
			ntv.gencode(0x89); ntv.gencode(0x04); ntv.gencode(0x91); // mov [ecx+edx*4], eax
			ntv.genas("push ecx");
			if(elem < elems - 1) tok.skip(",");
		} 
		et.change(elem_type.get().type | T_ARRAY); // TODO: refactoring
		ntv.genas("pop eax");
		tok.skip("]");
		return 1;
	} else return 0;
}

