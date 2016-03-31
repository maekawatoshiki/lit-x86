#include "expr.h"
#include "lit.h"
#include "asm.h"
#include "lex.h"
#include "token.h"
#include "parse.h"
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
		std::cout << std::endl;
	} else if(ast->get_type() == AST_VARIABLE_ASGMT) {
		std::cout << "(" << ((VariableAsgmtAST *)ast)->op << " ";
			visit(((VariableAsgmtAST *)ast)->var);
			visit(((VariableAsgmtAST *)ast)->src);
		std::cout << ")" << std::endl;;
	} else if(ast->get_type() == AST_POSTFIX) {
		std::cout << "(" << ((PostfixAST *)ast)->op << " ";
			visit(((PostfixAST *)ast)->expr);
			std::cout << ")";
	} else if(ast->get_type() == AST_WHILE) {
		WhileAST *wa = (WhileAST *)ast;
		std::cout << "(while ("; visit(wa->cond);
		std::cout << ")\n(\n";
		for(int i = 0; i < wa->block.size(); i++) 
			visit(wa->block[i]);
		std::cout << ")\n)";
		std::cout << std::endl;
	} else if(ast->get_type() == AST_FOR) {
		ForAST *fa = (ForAST *)ast;
		std::cout << "(for ("; visit(fa->asgmt);
		if(fa->is_range_for) {
			std::cout << ") ("; visit(fa->range);
			std::cout << ")\n(" << std::endl;
		} else {
			std::cout << ") ("; visit(fa->cond); 
			std::cout << ") ("; visit(fa->step);
			std::cout << ")\n(" << std::endl;
		}
		for(int i = 0; i < fa->block.size(); i++) 
			visit(fa->block[i]);
		std::cout << ")\n)";
		std::cout << std::endl;
	} else if(ast->get_type() == AST_FUNCTION) {
		FunctionAST *fa = ((FunctionAST *) ast);
		std::cout << "(defunc " << fa->info.mod_name << "::" << fa->info.name << " ("; 
		for(int i = 0; i < fa->args.size(); i++) 
			visit(fa->args[i]);
		std::cout << ")\n(\n";
		for(int i = 0; i < fa->statement.size(); i++) 
			visit(fa->statement[i]);
		std::cout << ")\n)";
		std::cout << std::endl;
	} else if(ast->get_type() == AST_IF) {
		IfAST *ia = (IfAST *)ast;
		std::cout << "(if ("; visit(ia->cond); std::cout << ")\n(";
		for(int i = 0; i < ia->then_block.size(); i++) 
			visit(ia->then_block[i]);
		std::cout << ")\n(\n";
		for(int i = 0; i < ia->else_block.size(); i++) 
			visit(ia->else_block[i]);
		std::cout << ")\n)";
		std::cout << std::endl;
	} else if(ast->get_type() == AST_VARIABLE_INDEX) {
		std::cout << "([] ";
			visit(((VariableIndexAST *)ast)->var);
			visit(((VariableIndexAST *)ast)->idx);
		std::cout << ")";
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
		std::cout << " \"" << ((StringAST *)ast)->str << "\" ";
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
		std::cout << std::endl;
	} else if(ast->get_type() == AST_RETURN) {
		std::cout << "(return ("; visit(((ReturnAST *)ast)->expr);
		std::cout << "))" << std::endl;
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
	bool add = false, sub = false;
	l = expr_compare();
	while((add = tok.skip("+=")) || (sub = tok.skip("-=")) || tok.skip("=")) {
		r = expr_entry();
		l = new VariableAsgmtAST(l, r, 
				add ? "+=" : 
				sub ? "-=" : 
				"=");
	}
	return l;
}

AST *Parser::expr_compare() {
	bool andop=0, orop=0, xorop = false;
	AST *l, *r;
	l = expr_logic();
	while((andop=tok.skip("and") || tok.skip("&")) || (orop=tok.skip("or") || 
				tok.skip("|")) || (xorop=(tok.skip("xor") || tok.skip("^"))) || tok.skip("..")) {
		r = expr_logic();
		l = new BinaryAST(andop ? "and" : orop ? "or" : xorop ? "xor" : "range", l, r);
	}

	return l;
}

AST *Parser::expr_logic() {
	bool lt = false, gt=false, ne=false, eql=false, fle=false;
	AST *l, *r;
	l = expr_add_sub();
	if((lt=tok.skip("<")) || (gt=tok.skip(">")) || (ne=tok.skip("!=")) ||
			(eql=tok.skip("==")) || (fle=tok.skip("<=")) || tok.skip(">=")) {
		r = expr_add_sub();
		l = new BinaryAST(lt ? "<" : gt ? ">" : ne ? "!=" : eql ? "==" : fle ? "<=" : ">=", l, r);
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
	l = expr_postfix();
	while(tok.skip("[")) {
		r = expr_entry();
		l = new VariableIndexAST(l, r);
		if(!tok.skip("]"))
			error("error: %d: expected expression ']'", tok.get().nline);
	}
	return l;
}

AST *Parser::expr_postfix() {
	AST *expr = expr_primary();
	bool inc;
	if((inc = tok.skip("++")) || tok.skip("--")) {
		return new PostfixAST(inc ? "++" : "--", expr);
	}
	return expr;
}

AST *Parser::expr_primary() {
	bool is_get_addr = false, ispare = false, is_global_decl = false;
	std::string name, mod_name = "";
	var_t *v = NULL; 
	
	if(tok.skip("&")) is_get_addr = true;
	if(tok.skip("$")) is_global_decl = true;

	if(is_number_tok()) {
		return new NumberAST(atoi(tok.next().val.c_str()));
	} else if(is_char_tok()) { 
		return new NumberAST(tok.next().val.c_str()[0]);
	} else if(is_string_tok()) {
		return new StringAST(tok.next().val);
	} else if(is_ident_tok()) { // variable or inc or dec
		name = tok.next().val; mod_name = "";
		int type, is_ary; 
		bool is_vardecl = false; 
		std::string class_name;

		if(tok.skip("::")) { // module?
			mod_name = tok.next().val;
			swap(mod_name, name);
		} else if(tok.skip(":")) { // variable declaration
			is_ary = 0;
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
			if(tok.skip("(") || is_func(name)) { // function
				func_t f = {
					.name = name,
					.mod_name = mod_name == "" ? module : mod_name
				};
				std::vector<AST *> args;
				for(int i = 0; !tok.skip(")") && !tok.is(";"); i++) {
					args.push_back(expr_entry());
					tok.skip(",");
				}
				return new FunctionCallAST(f, args);
			} else { // variable
				var_t v = {
					.name = name,
					.mod_name = mod_name == "" ? module : mod_name,
					.type = type,
					.class_type = class_name,
					.is_global = is_global_decl
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

