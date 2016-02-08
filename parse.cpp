#include "parse.h"
#include "lit.h"
#include "asm.h"
#include "lex.h"
#include "expr.h"
#include "token.h"
#include "util.h"
#include "library.h"
#include "var.h"
#include "func.h"

expr_type_t &ExprType::get() {
	return type;
}

bool ExprType::change(int ty) {
	type.type = ty;
	return true;
}

bool ExprType::change(std::string ty) {
	type.type = T_USER_TYPE;
	type.user_type = ty;
	return true;
}

bool ExprType::is_array() {
	return type.type & T_ARRAY;
}

bool ExprType::is_type(int ty) {
	return type.type & ty;
}

int Parser::make_break() {
	ntv.gencode(0xe9); // jmp
	break_list.addr_list = (uint32_t*)realloc(break_list.addr_list, ADDR_SIZE * (break_list.count + 1));
	if(break_list.addr_list == NULL) error("LitSystemError: no enough memory");
	break_list.addr_list[break_list.count] = ntv.count;
	ntv.gencode_int32(0);
	return break_list.count++;
}

int Parser::make_return() {
	expr_entry(); // get argument
	ntv.gencode(0xe9); // jmp
	return_list.addr_list = (uint32_t*)realloc(return_list.addr_list, ADDR_SIZE * (return_list.count + 1));
	if(return_list.addr_list == NULL) error("LitSystemError: no enough memory");
	return_list.addr_list[return_list.count] = ntv.count;
	ntv.gencode_int32(0);
	return return_list.count++;
}

AST *Parser::expression() {
	int isputs = 0;

	if(tok.skip("$")) { // global varibale

		if(is_asgmt()) asgmt();

	} else if(tok.skip("require")) {
	
		make_require();
	
	} else if(tok.skip("def")) { 

		return make_func();

	} else if(tok.skip("module")) { blocksCount++;
		module = tok.tok[tok.pos++].val;
		eval();
		module = "";
	} else if((isputs=tok.skip("puts")) || tok.skip("print")) {

		do {
			ExprType et; //TODO: fix
			ntv.genas("push eax");
			if(et.is_type(T_STRING)) {
				ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(4);// call *0x04(esi) putString
			} else {
				ntv.gencode(0xff); ntv.gencode(0x16); // call (esi) putNumber
			}
			ntv.genas("add esp 4");
		} while(tok.skip(","));
		// for new line
		if(isputs) {
			ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(8);// call *0x08(esi) putLN
		}

	} else if(tok.skip("for")) { blocksCount++;

		asgmt();
		if(!tok.skip(",")) error("error: %d: expected ','", tok.tok[tok.pos].nline);
		make_while();

	} else if(tok.is("while")) { return make_while();
	} else if(tok.skip("return")) {

		make_return();

	} else if(tok.is("if")) return make_if();
	else if(tok.is("else")) return NULL;
	else if(tok.skip("break")) {
		make_break();
	} else if(tok.is("end")) return NULL;
	else {
		puts("expression");
		return expr_entry();
	}
	
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
	uint32_t main_address;
	ntv.gencode(0xe9); main_address = ntv.count; ntv.gencode_int32(0);

	blocksCount = 0;

	ast_vector a = eval();
	std::cout << "\n---------- abstract syntax tree ----------" << std::endl;
	for(int i = 0; i < a.size(); i++)
		visit(a[i]), std::cout << std::endl;

#ifdef DEBUG
	printf("blocks: %d\n", blocksCount);
#endif
	if(blocksCount != 0) error("error: 'end' is not enough");
	uint32_t addr = 0;
	func_t *func_main = funcs.get("main", "");
	if(func_main == NULL) error("LitSystemError: not found function 'main'");
	else addr = func_main->address;
	ntv.gencode_int32_insert(addr - ADDR_SIZE - 1, main_address);
#ifdef DEBUG
	for(int i = 0; i < ntv.count; i++)
		printf("%02x", ntv.code[i]);
	puts("");
	printf("memsz: %d\n", funcs.focus()->var.size[funcs.now]);
#endif
// #ifdef DEBUG
// 	printf("blocks: %d\n", blocksCount);
// #endif
// 	if(blocksCount != 0) error("error: 'end' is not enough");
// 	uint32_t addr = 0;
// 	func_t *func_main = funcs.get("main", "");
// 	if(func_main == NULL) error("LitSystemError: not found function 'main'");
// 	else addr = func_main->address;
// 	ntv.gencode_int32_insert(addr - ADDR_SIZE - 1, main_address);
// #ifdef DEBUG
// 	for(int i = 0; i < ntv.count; i++)
// 		printf("%02x", ntv.code[i]);
// 	puts("");
// 	printf("memsz: %d\n", funcs.focus()->var.size[funcs.now]);
// #endif

	return 1;
}

void Parser::make_require() {
	lib_list.append(tok.next().val);
}

AST *Parser::make_if() {
	if(tok.skip("if")) {
		AST *cond = expr_entry();
		ast_vector then = eval(), else_block;
		if(tok.skip("else")) {
			else_block = eval();
			tok.skip("end");
		} else if(tok.skip("end")) {}
		AST *i = new IfAST(cond, then, else_block);
		visit(i);
		return i;
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
	uint32_t espBgn, params = 0;
	std::string func_name = tok.next().val;
	ast_vector args, stmt;
	func_t function = {.name = func_name};

	if(tok.skip("(")) { // get params
		do { args.push_back(expr_entry()); } while(tok.skip(","));
		if(!tok.skip(")"))
			error("error: %d: expected expression ')'", tok.get().nline);
	}

	stmt = eval();
	if(!tok.skip("end")) { error("error: source %d", __LINE__); }

	return new FunctionAST(function, args, stmt);
}

void Parser::replaceEscape(char *str) {
	int i;
	char *pos;
	char escape[12][3] = {
		"\\a", "\a",
		"\\r", "\r",
		"\\f", "\f",
		"\\n", "\n",
		"\\t", "\t",
		"\\b", "\b"
	};
	for (i = 0; i < 12; i += 2) {
		while ((pos = strstr(str, escape[i])) != NULL) {
			*pos = escape[i + 1][0];
			memmove(pos + 1, pos + 2, strlen(str) - 2 + 1);
		}
	}
}
