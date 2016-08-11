#include "expr.h"
#include "lit.h"
#include "lex.h"
#include "token.h"
#include "parse.h"
#include "util.h"
#include "var.h"
#include "func.h"
#include "ast.h"
#include "exprtype.h"

int Parser::is_string_tok() { return tok.get().type == TOK_STRING; }
int Parser::is_number_tok() { return tok.get().type == TOK_NUMBER; }
int Parser::is_ident_tok()  { return tok.get().type == TOK_IDENT;  }
int Parser::is_char_tok() { return tok.get().type == TOK_CHAR; } 

AST *visit(AST *ast) {
	auto mod_to_string = [](func_t f) -> std::string {
		std::string out;
		for(auto it = f.mod_name.begin(); it != f.mod_name.end(); ++it) {
			out += *it + "::";
		}
		return out;
	};

	if(ast->get_type() == AST_BINARY) {
		std::cout << "(" << ((BinaryAST *)ast)->op << " ";
			visit(((BinaryAST *)ast)->left);
			visit(((BinaryAST *)ast)->right);
		std::cout << ")";
		std::cout << std::endl;
	} else if(ast->get_type() == AST_STRUCT) {
		StructAST *sa = (StructAST *)ast;
		std::cout << "(struct " << sa->name << "(" << std::endl;
		for(int i = 0; i < sa->var_decls.size(); i++) 
			visit(sa->var_decls[i]);
		std::cout << ")\n)" << std::endl;
	} else if(ast->get_type() == AST_VARIABLE_ASGMT) {
		std::cout << "(=";
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
		std::cout << "(defunc " << mod_to_string(fa->info) << fa->info.name << " ("; 
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
			// << ((VariableDeclAST *)ast)->info.mod_name << "::"
			<< ((VariableDeclAST *)ast)->info.name << " "
			<< ((VariableDeclAST *)ast)->info.type.get().type << ")";
	} else if(ast->get_type() == AST_ARRAY) {
		ArrayAST *ary = (ArrayAST *)ast;
		std::cout << "(array ";
		for(int i = 0; i < ary->elems.size(); i++) 
			visit(ary->elems[i]);
		std::cout << ")";
	} else if(ast->get_type() == AST_NUMBER) {
		std::cout << " " << ((NumberAST *)ast)->number << " ";
	} else if(ast->get_type() == AST_NUMBER_FLOAT) {
		std::cout << " " << ((FloatNumberAST *)ast)->number << " ";
	} else if(ast->get_type() == AST_CHAR) {
		std::cout << " " << ((CharAST *)ast)->ch << " ";
	} else if(ast->get_type() == AST_STRING) {
		std::cout << " \"" << ((StringAST *)ast)->str << "\" ";
	} else if(ast->get_type() == AST_VARIABLE) {
		std::cout << "(var " 
			// << ((VariableAST *)ast)->info.mod_name << "::"
			<< ((VariableAST *)ast)->info.name << ") ";
	} else if(ast->get_type() == AST_FUNCTION_CALL) {
		std::cout << "(call " 
			<< mod_to_string(((FunctionCallAST *)ast)->info) 
			<< ((FunctionCallAST *)ast)->info.name << " ";
		for(int i = 0; i < ((FunctionCallAST *)ast)->args.size(); i++) {
			visit(((FunctionCallAST *)ast)->args[i]);
		}
		std::cout << ")";
		std::cout << std::endl;
	} else if(ast->get_type() == AST_RETURN) {
		std::cout << "(return ("; visit(((ReturnAST *)ast)->expr);
		std::cout << "))" << std::endl;
	} else if(ast->get_type() == AST_LIBRARY) {
		LibraryAST *l = (LibraryAST *)ast;
		std::cout << "(lib " << l->lib_name << "(" << std::endl;
		for(int i = 0; i < l->proto.size(); i++) 
			visit(l->proto[i]);
		std::cout << ")" << std::endl;
	} else if(ast->get_type() == AST_PROTO) {
		PrototypeAST *prt = (PrototypeAST *)ast;
		std::cout << "(proto " << prt->proto.name << " " << prt->proto.params << ")" << std::endl;
	} else if(ast->get_type() == AST_NEW) {
		NewAllocAST *na = (NewAllocAST *)ast;
		std::cout << "(new " << na->type << " ";
		if(na->size) visit(na->size);
		std::cout << ")" << std::endl;
	} else if(ast->get_type() == AST_DOT) {
		DotOpAST *da = (DotOpAST *)ast;
		std::cout << "(dot "; 
			visit(da->var);
			visit(da->member);
		std::cout << ")";
	} else if(ast->get_type() == AST_MODULE) {
		ModuleAST *ma = (ModuleAST *)ast;
		std::cout << "(module " << ma->name << " ";
			for(auto it = ma->statement.begin(); it != ma->statement.end(); ++it) {
				visit(*it);
			}
		std::cout << ")" << std::endl;
	} else if(ast->get_type() == AST_CAST) {
		CastAST *ca = (CastAST *)ast;
		std::cout << "(cast " << ca->type << " ";
			visit(ca->expr);
		std::cout << ")" << std::endl;
	} else if(ast->get_type() == AST_MINUS) {
		UnaryMinusAST *ua = (UnaryMinusAST *)ast;
		std::cout << "(minus ";
			visit(ua->expr);
		std::cout << ")" << std::endl;
	}


	return ast;
}

int Parser::get_op_prec(std::string op) {
	return op_prec.count(op) != 0 ? op_prec[op] : -1;
}

AST *Parser::expr_rhs(int prec, AST *lhs) {
	while(true) {
		int tok_prec, next_prec;
		if(tok.get().type == TOK_SYMBOL) {
			tok_prec = get_op_prec(tok.get().val);
			if(tok_prec < prec) return lhs;
		} else return lhs;
		std::string op = tok.next().val;
		AST *rhs = expr_dot();
		if(tok.get().type == TOK_SYMBOL) {
			next_prec = get_op_prec(tok.get().val);
			if(tok_prec < next_prec) 
				rhs = expr_rhs(tok_prec + 1, rhs);
		} 
		if(op == "+=" ||
				op == "-=" ||
				op == "*=" ||
				op == "/=" ||
				op == "&=" ||
				op == "|=" ||
				op == "^=" ||
				op == "=") {
			bool add = op == "+=", sub = op == "-=", mul = op == "*=", div = op == "/=", 
					 aand = op == "&=", aor = op == "|=", axor = op == "^=", normal = op == "=";
			lhs = new VariableAsgmtAST(lhs, normal ? rhs :
						new BinaryAST(
							add ? "+" : 
							sub ? "-" : 
							mul ? "*" : 
							div ? "/" : 
							aand? "&" : 
							aor ? "|" :
							axor? "^" : "ERROR", lhs, rhs));	
		} else 
			lhs = new BinaryAST(op, lhs, rhs);		
	}
}

AST *Parser::expr_entry() { 
	AST *lhs = expr_dot();
	return expr_rhs(0, lhs);
}

AST *Parser::expr_dot() {
	AST *l, *r;
	l = expr_index();
	while(tok.skip(".")) {
		std::string name = tok.next().val;
		bool has_pare = false;
		if((has_pare=tok.skip("(")) || is_func(name)) { // UFCS
			func_t f = {
				.name = name,
				.mod_name = std::vector<std::string>(),
			};
			std::vector<AST *> args;
			args.push_back(l);
			if(tok.get().type != TOK_END && 
					(tok.get().type != TOK_SYMBOL || 
					 tok.get().val == "<" || tok.get().val == "(" || 
					 tok.get().val == "[" || tok.get().val == "$")) {
				while(!tok.is(")") && !tok.is(";")) {
					args.push_back(expr_entry());
					tok.skip(",");
				}
			} if(has_pare) tok.skip(")");
			l = new FunctionCallAST(f, args);
		} else { // member of struct
			tok.prev();
			r = expr_primary();
			l = new DotOpAST(l, r);
		}
	}
	return l;
}

AST *Parser::expr_index() {
	AST *l, *r;
	l = expr_unary();
	if(tok.get().type == TOK_STRING) return l; // skip string tok such as "[" 
	while(tok.skip("[")) {
		r = expr_entry();
		l = new VariableIndexAST(l, r);
		if(!tok.skip("]"))
			error("error: %d: expected expression ']'", tok.get().nline);
	}
	return l;
}

AST *Parser::expr_unary() { // TODO: implementation unary minus(-)!!
	if(tok.skip("<")) { // cast
		std::string type = tok.next().val;
		if(!tok.skip(">")) error("error: expected expression '>'");
		AST *expr = expr_entry();
		return new CastAST(type, expr);
	} else if(tok.skip("-")) {
		AST *expr = expr_entry();
		return new UnaryMinusAST(expr);
	}	
	return expr_primary();
}

AST *Parser::expr_primary() {
	bool is_global_decl = false, is_ref = false;
	std::string name;
	std::vector<std::string> mod_name;
	var_t *v = NULL; 
	
	if(tok.skip("$")) is_global_decl = true;
	if(tok.skip("ref")) is_ref = true;

	if(tok.get().val == "new") {
		tok.skip();
		AST *size = NULL;
		if(tok.get().val != "!") {
			size = expr_entry();
		} else tok.skip();
		std::string type = "int";
		if(is_ident_tok()) {
			type = tok.next().val;
			while(tok.skip("[]"))
				type += "[]";
		}
		return new NewAllocAST(type, size);
	} else if(is_number_tok()) {
		if(strstr(tok.get().val.c_str(), ".") != NULL)
			return new FloatNumberAST(atof(tok.next().val.c_str()));
		else
			return new NumberAST(tok.next().val);
	} else if(is_char_tok()) { 
		return new CharAST(tok.next().val.c_str()[0]);
	} else if(is_string_tok()) {
		return new StringAST(tok.next().val);
	} else if(tok.get().val == "true" || tok.get().val == "false" || tok.get().val == "nil") {
		return new NumberAST(tok.next().val == "true" ? "1" :"0");
	} else if(is_ident_tok()) { 
		name = tok.next().val;
		int is_ary; 
		ExprType type;
		bool is_vardecl = false; 
		std::string class_name;

		if(tok.is("::")) { // module?
			mod_name.push_back(name);
			name = "";
			while(tok.skip("::")) mod_name.push_back(tok.next().val);
			name = mod_name.back();
			mod_name.pop_back();
		} else if(tok.skip(":")) { // variable declaration
			is_ary = 0;
			type.change(Type::str_to_type(tok.next().val));
			if(tok.skip("[]")) {
				int elem_ty = type.get().type;
				type.change(T_ARRAY);
				type.next = new ExprType(elem_ty);
			}
			is_vardecl = true;
		} else { 
			type.change(T_INT);
		}
		
		{	
			bool has_pare = false;
			if((has_pare=tok.skip("(")) || is_func(name)) { // function
				func_t f = {
					.name = name,
					.mod_name = mod_name
				};
				std::vector<AST *> args;
				if(tok.get().type != TOK_END && 
						(tok.get().type != TOK_SYMBOL || tok.get().val == "-" || 
						 tok.get().val == "<" || tok.get().val == "(" || 
						 tok.get().val == "[" || tok.get().val == "$")) {
					while(!tok.is(")") && !tok.is(";")) {
						args.push_back(expr_entry());
						tok.skip(",");
					}
				} if(has_pare) tok.skip(")");
				return new FunctionCallAST(f, args);
			} else { // variable
				var_t v = {
					.name = name,
					.mod_name = "",
					.class_type = class_name,
					.is_global = is_global_decl
				};
				v.type.change(new ExprType(type));
				v.type.set_ref(is_ref);
				if(is_vardecl)
					return new VariableDeclAST(v);
				else
					return new VariableAST(v);
			}
		}
	} else if(tok.skip("(")) {
		AST *e = expr_entry();
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
			AST *elem = expr_entry();
			elems.push_back(elem);
			tok.skip(",");
		}
		return new ArrayAST(elems);
	} else if(tok.skip("[]")) { // empty array
		if(tok.skip(":")) {
			std::string type_name = tok.next().val;
			while(tok.skip("[]")) type_name += "[]";
			return new ArrayAST(Type::str_to_type(type_name));
		}
	}
	return NULL;
}

