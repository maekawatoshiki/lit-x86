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

AST *visit(AST *ast) {
	if(ast->get_type() == AST_BINARY) {
		std::cout << "(" << ((BinaryAST *)ast)->op << " ";
			visit(((BinaryAST *)ast)->left);
			visit(((BinaryAST *)ast)->right);
			std::cout << ")";
	} else if(ast->get_type() == AST_IF) {
		IfAST *ia = (IfAST *)ast;
		std::cout << "(if ("; visit(ia->cond); std::cout << ")\n(";
		for(int i = 0; i < ia->then_block.size(); i++) 
			visit(ia->then_block[i]);
		std::cout << ")\n(";
		for(int i = 0; i < ia->else_block.size(); i++) 
			visit(ia->else_block[i]);
		std::cout << "))";
	} else if(ast->get_type() == AST_VARIABLE_DECL) {
		std::cout << "(vardecl "
			<< ((VariableDeclAST *)ast)->info.mod_name << "::"
			<< ((VariableDeclAST *)ast)->info.name << " "
			<< ((VariableDeclAST *)ast)->info.type << ")";
	} else if(ast->get_type() == AST_ARRAY) {
		ArrayAST *ary = (ArrayAST *)ast;
		std::cout << "(array ";
		for(int i = 0; i < ary->elems.size(); i++) 
			visit(ary->elems[i]);
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
	return ast;
}

AST *Parser::expr_entry() { 
	AST *ast = expr_asgmt();
	// visit(ast); std::cout << std::endl;
	return ast;
}

AST *Parser::expr_asgmt() {
	AST *l, *r;
	l = expr_compare();
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
	l = expr_index();
	while((mul = tok.skip("*")) || (div=tok.skip("/")) || tok.skip("%")) {
		r = expr_index();
		l = new BinaryAST(mul ? "*" : div ? "/" : "%", l, r);
	}
	return l;
}

AST *Parser::expr_index() {
	AST *l, *r;
	l = expr_primary();
	while(tok.skip("[")) {
		r = expr_primary();
		l = new BinaryAST("[]", l, r);
		if(!tok.skip("]"))
			error("error: %d: expected expression ']'", tok.get().nline);
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
		name = tok.next().val; mod_name = "";
		int type, is_ary; bool is_vardecl = false; std::string class_name;

		if(tok.skip("::")) { // module?
			mod_name = tok.next().val;
			swap(mod_name, name);
		} else if(tok.skip(":")) {
			if(tok.skip("int")) { 
				if(tok.skip("[]")) { is_ary = T_ARRAY; }
				type = T_INT | is_ary;
			} else if(tok.skip("string")) { 
				if(tok.skip("[]")) { is_ary = T_ARRAY; }
				type = T_STRING | is_ary;
			} else if(tok.skip("double")) { 
				if(tok.skip("[]")) { is_ary = T_ARRAY; }
				type = T_DOUBLE | is_ary;
			} else {
				class_name = tok.next().val;
				if(tok.skip("[]")) { is_ary = T_ARRAY; }
				type = T_USER_TYPE | is_ary;
			}
			is_vardecl = true;
		} else { 
			type = T_INT;
		}
		
		{	
			if(tok.skip("(")) { // function
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
			} else { // variable
				var_t v = {
					.name = name,
					.mod_name = mod_name == "" ? module : mod_name,
					.type = type,
					.class_type = class_name
				};
				if(is_vardecl)
					return new VariableDeclAST(v);
				else
					return new VariableAST(v);
			}
		}
	} else if(tok.skip("(")) {
		AST *e = expr_asgmt();
		if(!tok.skip(")"))
			error("error: %d: expected expression ')'", tok.get().nline);
		return e;
	} else {
		AST *ary = expr_array();
		if(ary == NULL)
			error("error: %d: invalid expression", tok.get().nline);
		else return ary;
	}

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

AST *Parser::expr_array() {
	if(tok.skip("[")) {
		ast_vector elems;
		while(!tok.skip("]")) {
			AST *elem = expr_asgmt();
			elems.push_back(elem);
			tok.skip(",");
		}
		return new ArrayAST(elems);
	}
	return NULL;
}

