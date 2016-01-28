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
	} else if(ast->get_type() == AST_VARIABLE) {
		std::cout << "(var " 
			<< ((VariableAST *)ast)->info.mod_name << "::"
			<< ((VariableAST *)ast)->info.name << ") ";
	} else if(ast->get_type() == AST_FUNCTION_CALL) {
		std::cout << "(call " 
			<< ((FunctionCallAST *)ast)->info.mod_name << "::"
			<< ((FunctionCallAST *)ast)->info.name << " ";
		for(int i = 0; i < ((FunctionCallAST *)ast)->args.size(); i++) {
			visit(((FunctionCallAST *)ast)->args[i]);
		}
		std::cout << ")";
	}
	return 0;
}

AST *Parser::expr_entry() { 
	AST *ast = expr_asgmt();
	visit(ast);
	return ast;
}

AST *Parser::expr_asgmt() {
	AST *l, *r;
	l = expr_primary();
	while(tok.skip("=")) {
		r = expr_compare();
		l = new BinaryAST("=", l, r);
	}
	return l;
}

AST *Parser::expr_compare() {
	int andop=0, orop=0;
	AST *l, *r;
	l = expr_logic();
	while((andop=tok.skip("and") || tok.skip("&")) || (orop=tok.skip("or") || 
				tok.skip("|")) || tok.skip("xor") || tok.skip("^")) {
		r = expr_logic();
		l = new BinaryAST(andop ? "and" : orop ? "or" : "xor", l, r);
	}

	return l;
}

AST *Parser::expr_logic() {
	int32_t lt=0, gt=0, ne=0, eql=0, fle=0;
	AST *l, *r;
	l = expr_add_sub();
	if((lt=tok.skip("<")) || (gt=tok.skip(">")) || (ne=tok.skip("!=")) ||
			(eql=tok.skip("==")) || (fle=tok.skip("<=")) || tok.skip(">=")) {
		r = expr_add_sub();
		l = new BinaryAST(lt ? "<" : gt ? ">" : ne ? "!=" : eql ? "==" : fle ? "<=" : "!", l, r);
	}

	return l;
}

AST *Parser::expr_add_sub() {
	int add = 0, concat = 0;
	AST *l, *r;
	l = expr_mul_div();
	while((add = tok.skip("+")) || (concat = tok.skip("~")) || tok.skip("-")) {
		r = expr_mul_div();
		l = new BinaryAST(add ? "+" : concat ? "~" : "-", l, r);
	}
	return l;
}

AST *Parser::expr_mul_div() {
	int mul, div;
	AST *l, *r;
	l = expr_primary();
	while((mul = tok.skip("*")) || (div=tok.skip("/")) || tok.skip("%")) {
		r = expr_primary();
		l = new BinaryAST(mul ? "*" : div ? "/" : "%", l, r);
	}
	return l;
}

AST *Parser::expr_primary() {
	int is_get_addr = 0, ispare = 0;
	std::string name, mod_name = "";
	var_t *v = NULL; 
	
	if(tok.skip("&")) is_get_addr = 1;

	if(is_number_tok()) {
	
		return new NumberAST(atoi(tok.next().val.c_str()));
	
	} else if(is_char_tok()) { 
		
		ntv.genas("mov eax %d", (int)tok.next().val[0]);
	
	} else if(is_string_tok()) { 

		return new StringAST(tok.next().val);

	} else if(is_ident_tok()) { // variable or inc or dec
	
		name = tok.get().val; mod_name = "";

		if(tok.is(":", 1)) { // module?
			mod_name = tok.next().val; 
			tok.skip(":");
			name = tok.get().val; 
		}
		
		{	
			tok.skip();

			if(tok.skip("(")) {
				func_t f = {
					.name = name,
					.mod_name = mod_name == "" ? module : mod_name
				};
				std::vector<AST *> args;
				for(int i = 0; !tok.skip(")"); i++) {
					args.push_back(expr_compare());
					tok.skip(",");
				}
				return new FunctionCallAST(f, args);
			} else { // single variable
				var_t v = {
					.name = name,
					.mod_name = mod_name == "" ? module : mod_name
				};
				return new VariableAST(v);
			}
		}
	} else if(tok.skip("(")) {
		AST *e = expr_compare();
		if(!tok.skip(")"))
			error("error: %d: expected expression ')'", tok.get().nline);
		return e;
	}// else if(!make_array(et)) error("error: %d: invalid expression", tok.get().nline);

	// while(tok.skip(".")) {
	// 	name = tok.get().val;
	// 	std::string class_name;
	// 	if(v == NULL) {
	// 		if(et.is_type(T_INT)) class_name = "Math";
	// 		else if(et.is_type(T_STRING)) class_name = "String";
	// 		else class_name = mod_name;
	// 	} else class_name = v->class_type;
	// 	tok.skip();
	// 	if(lib_list.is(class_name)) {
	// 		ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(0x00); // mov [esp], eax
	// 		if(HAS_PARAMS_FUNC) {
	// 			tok.skip("(");
	// 			for(size_t i = 0; !tok.skip(")") && !tok.skip(";"); i++) {
	// 				expr_entry();
	// 				ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode((i + 1) * ADDR_SIZE); // mov [esp+ADDR*(i+1)], eax
	// 				tok.skip(",");
	// 			}
	// 		} 
	// 		ntv.gencode(0xe8); ntv.gencode_int32(lib_list.call(name, class_name) - (uint32_t)&ntv.code[ntv.count] - ADDR_SIZE); // call func
	// 		et.change(T_INT);
	// 	} else {
	// 		func_t *function = funcs.get(name, class_name);
	// 		if(function == NULL) 
	// 			function = funcs.get(name, module);
	// 		if(function == NULL) error("function not found");
	// 		if(function->params > 0) ntv.genas("push eax");
	// 		if(HAS_PARAMS_FUNC) {
	// 			tok.skip("(");
	// 			for(size_t i = 0; i < function->params - 1; i++) {
	// 				expr_entry();
	// 				ntv.genas("push eax");
	// 				tok.skip(",");
	// 			}
	// 		} tok.skip(")");
	// 		ntv.gencode(0xe8); ntv.gencode_int32(0xFFFFFFFF - (ntv.count - function->address) - 3); // call func
	// 		ntv.genas("add esp %d", function->params * ADDR_SIZE);
	// 		et.change(T_INT);
	// 	}
	// } 

	// while(is_index()) make_index(et);
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
			elem_type;// = expr_entry();
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

