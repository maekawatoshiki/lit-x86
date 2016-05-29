#include "parse.h"
#include "lit.h"
#include "asm.h"
#include "lex.h"
#include "expr.h"
#include "token.h"
#include "util.h"
#include "var.h"
#include "codegen.h"
#include "exprtype.h"
#include "func.h"

AST *Parser::make_break() {
	if(tok.skip("break")) return new BreakAST();
	return NULL;
}

AST *Parser::make_return() {
	if(tok.skip("return")) {
		AST *expr = expr_entry();
		return new ReturnAST(expr);
	}
	return NULL;
}

AST *Parser::expression() {
	if(tok.skip("def")) return make_func();
	else if(tok.is("proto")) return make_proto();
	else if(tok.is("struct")) return make_struct();
	// else if(tok.skip("module")) { blocksCount++;
	// 	module = tok.tok[tok.pos++].val;
	// 	eval();
	// 	module = "";
	// }
	else if(tok.is("lib")) return make_lib();
	else if(tok.is("for")) return make_for();
	else if(tok.is("while"))  return make_while();
	else if(tok.is("return"))  return make_return();
	else if(tok.is("if")) return make_if();
	else if(tok.is("break")) return make_break();
	else if(tok.is("else") || tok.is("elsif") || tok.is("end")) return NULL;
	else return expr_entry();
	
	return NULL;
}

ast_vector Parser::eval() {
	ast_vector block;
	while(tok.pos < tok.size) {
		while(tok.skip(";"))
			if(tok.pos >= tok.size) break;
		AST *b = expression();
		while(tok.skip(";"))
			if(tok.pos >= tok.size) break;
		if(b == NULL) break;
		block.push_back(b);
	}
	return block;
}

int Parser::parser() {
	tok.pos = ntv.count = 0;
	blocksCount = 0;
	op_prec[".."] = 50;
	op_prec["..."] =50;
	op_prec["="] =  100;
	op_prec["+="] = 100;
	op_prec["-="] = 100;
	op_prec["*="] = 100;
	op_prec["/="] = 100;
	op_prec["%="] = 100;
	op_prec["=="] = 200;
	op_prec["!="] = 200;
	op_prec["<="] = 200;
	op_prec[">="] = 200;
	op_prec["<"] =  200;
	op_prec[">"] =  200;
	op_prec["&"] =  150;
	op_prec["|"] =  150;
	op_prec["^"] =  150;
	op_prec["+"] =  300;
	op_prec["-"] =  300;
	op_prec["*"] =  400;
	op_prec["/"] =  400;
	op_prec["%"] =  400;

	// append standard functions to a list of declared functions
	append_func("Array");
	append_func("printf");
	append_func("gets");
	append_func("strlen");
	append_func("len");
	append_func("puts");
	append_func("print"); // 'print' is almost the same as 'puts' but 'print' doesn't new line
	append_func("gets");
	append_func("str_to_int");

	ast_vector a = eval();
	// std::cout << "\n---------- abstract syntax tree ----------" << std::endl;
	// for(int i = 0; i < a.size(); i++)
	// 	visit(a[i]), std::cout << std::endl;
	codegen_entry(a); // start code generating
	// std::cout << "\n---------- end of abstract syntax tree --" << std::endl;
	return 1;
}

AST *Parser::make_lib() {
	if(tok.skip("lib")) {
		std::string name = tok.next().val;
		ast_vector proto = eval();
		if(!tok.skip("end")) error("error: %d: expected expression 'end'", tok.get().nline);
		return new LibraryAST(name, proto);
	}
	return NULL;
}

AST *Parser::make_proto() {
	if(tok.skip("proto")) {
		std::string func_name = tok.next().val;
		ast_vector args;
		func_t function = { .name = func_name, .type = T_INT };

		bool is_parentheses = false;
		if((is_parentheses=tok.skip("(")) || is_ident_tok()) { // get params
			do { args.push_back(expr_primary()); } while(tok.skip(",") || is_ident_tok());
			if(is_parentheses) {
				if(!tok.skip(")"))
					error("error: %d: expected expression ')'", tok.get().nline);
			}
		}
		function.params = args.size();
		if(tok.skip(":")) { 
			int type = Type::str_to_type(tok.next().val);
			if(tok.skip("[]")) type |= T_ARRAY;
			function.type = type;
		}
		std::string new_name;
		if(tok.skip("|")) {
			new_name = tok.next().val;
		}

		append_func(new_name.empty() ? func_name : new_name);

		return new PrototypeAST(function, args, new_name);	
	}
	return NULL;
}

AST *Parser::make_struct() {
	if(tok.skip("struct")) {
		std::string name = tok.next().val;
		ast_vector decl_vars = eval();
		if(!tok.skip("end")) error("error: %d: expected expression 'end'", tok.get().nline);
		return new StructAST(name, decl_vars);
	} 
	return NULL;
}

AST *Parser::make_if() {
	if(tok.skip("if")) {
		AST *cond = expr_entry();
		ast_vector then = eval(), else_block;
		if(tok.skip("else")) {
			else_block = eval();
		} else if(tok.is("elsif")) {
			else_block.push_back(make_elsif());
		}
		if(!tok.skip("end")) error("error: %d: expected expression 'end'", tok.get().nline);
		AST *i = new IfAST(cond, then, else_block);
		return i;
	}
	return NULL;
}

AST *Parser::make_elsif() {
	if(tok.skip("elsif")) {
		AST *cond = expr_entry();
		ast_vector then = eval(), else_block;
		if(tok.is("elsif")) {
			else_block.push_back(make_elsif());
		} else if(tok.skip("else")) {
			else_block = eval();
		}
		return new IfAST(cond, then, else_block);
	}
	return NULL;
}

AST *Parser::make_for() {
	if(tok.skip("for")) {
		AST *asgmt = expr_entry();
		if(tok.skip(",")) {
			AST *cond = expr_entry();
			if(!tok.skip(",")) error("error: %d: expect expression ','", tok.get().nline);
			AST *step = expr_entry();
			ast_vector block = eval();
			if(!tok.skip("end")) error("error: %d: expected expression 'end'", tok.get().nline);
			return new ForAST(asgmt, cond, step, block);
		} else if(tok.skip("in")) {
			AST *range = expr_entry(), *cond, *step;
			if(range->get_type() == AST_BINARY) {
				BinaryAST *cond_bin = (BinaryAST *)range;
				AST *var = asgmt;
				asgmt = new VariableAsgmtAST(asgmt, cond_bin->left);
				cond = new BinaryAST(cond_bin->op == ".." ? "<=" : /* op=... */ "<", var, cond_bin->right);
				step = new VariableAsgmtAST(var, 
								new BinaryAST("+", var, new NumberAST(1))
						);
			} else error("error: %d: invalid expression", tok.get().nline);
			ast_vector block = eval();
			if(!tok.skip("end")) error("error: %d: expected expression 'end'", tok.get().nline);
			return new ForAST(asgmt, cond, step, block);
		} else  {
			error("error: %d: unknown syntax", tok.get().nline);
		}
	}
	return NULL;
}

AST *Parser::make_while() {
	if(tok.skip("while")) {
		AST *cond = expr_entry();
		ast_vector block = eval();
		if(!tok.skip("end")) error("error: %d: expected expression 'end'", tok.get().nline);
		return new WhileAST(cond, block);
	}
	return NULL;
}

AST *Parser::make_func() {
	uint32_t params = 0;
	std::string func_name = tok.next().val;
	ast_vector args, stmt;
	func_t function = { .name = func_name, .type = T_INT };
	append_func(func_name);

	bool is_parentheses = false;
	if((is_parentheses=tok.skip("(")) || is_ident_tok()) { // get params
		do { args.push_back(expr_primary()); } while(tok.skip(",") || is_ident_tok());
		if(is_parentheses) {
			if(!tok.skip(")"))
				error("error: %d: expected expression ')'", tok.get().nline);
		}
	}
	if(tok.skip(":")) { 
		int is_ary = 0;
		ExprType type(T_INT);
		type.change(Type::str_to_type(tok.next().val));
		if(tok.skip("[]")) { 
			int elem_ty = type.get().type;
			type.next = new ExprType(elem_ty);
			type.change(T_ARRAY);
		}
		function.type = type;
	}

	stmt = eval();
	if(!tok.skip("end")) { error("error: source %d", __LINE__); }
	return new FunctionAST(function, args, stmt);
}

bool Parser::is_func(std::string name) {
	return function_list.count(name) != 0 ? true : false;
}

void Parser::append_func(std::string name) {
	function_list[name] = true;
}

char *replace_escape(char *str) {
	int i;
	char *pos;
	char escape[14][3] = {
		"\\a", "\a",
		"\\r", "\r",
		"\\f", "\f",
		"\\n", "\n",
		"\\t", "\t",
		"\\b", "\b",
		"\\\"",  "\""
	};
	for(i = 0; i < 14; i += 2) {
		while ((pos = strstr(str, escape[i])) != NULL) {
			*pos = escape[i + 1][0];
			memmove(pos + 1, pos + 2, strlen(str) - 2 + 1);
		}
	}
	return str;
}
