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

AST *Parser::expression(int pos, int status) {
	int isputs = 0;

	if(tok.skip("$")) { // global varibale

		if(is_asgmt()) asgmt();

	} else if(tok.skip("require")) {
	
		make_require();
	
	} else if(tok.skip("def")) { 

		make_func();

	} else if(tok.skip("module")) { blocksCount++;
		module = tok.tok[tok.pos++].val;
		eval(0, NON);
		module = "";
	} else if(funcs.inside == false && !tok.is("def", 1) &&
			!tok.is("module", 1) && !tok.is("$", 1) &&
			!tok.is(";", 1) && module == "") {	// main func entry

		funcs.inside = true;
		funcs.now++;
		funcs.append("main", ntv.count, 0); // append funcs
		ntv.genas("push ebp");
		ntv.genas("mov ebp esp");
		uint32_t espBgn = ntv.count + 2; ntv.genas("sub esp 0");
		ntv.gencode(0x8b); ntv.gencode(0x75); ntv.gencode(0x0c); // mov esi, 0xc(%ebp)
		
		ast_vector a = eval(0, BLOCK_NORMAL);
		for(int i = 0; i < a.size(); i++)
			visit(a[i]), std::cout << std::endl;

		ntv.gencode(0x81); ntv.gencode(0xc4); ntv.gencode_int32(ADDR_SIZE * (var.focus().size() + 6)); // add %esp nn
		ntv.gencode(0xc9);// leave
		ntv.gencode(0xc3);// ret
		ntv.gencode_int32_insert(ADDR_SIZE * (var.focus().size() + 6), espBgn);
		funcs.inside = false;

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

	} else if(tok.skip("while")) { blocksCount++;

		make_while();

	} else if(tok.skip("return")) {

		make_return();

	} else if(tok.is("if")) { return make_if();
	
	} else if(tok.is("else")) { return NULL;

	} else if(tok.skip("break")) {

		make_break();

	} else if(tok.is("end")) { return NULL;

	} else {
		puts("expression");
		return expr_entry();
	}
	
	return NULL;
}

ast_vector Parser::eval(int pos, int status) {
	ast_vector block;
	while(tok.pos < tok.size) {
		while(tok.skip(";")) {}
		AST *b = expression(0, 0);
		if(b == NULL) break;
		block.push_back(b);
		std::cout << "type = " << b->get_type() << std::endl;
	}
	return block;
}

int Parser::parser() {
	tok.pos = ntv.count = 0;
	uint32_t main_address;
	ntv.gencode(0xe9); main_address = ntv.count; ntv.gencode_int32(0);

	blocksCount = 0;
	eval(0,0);
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

	return 1;
}

void Parser::make_require() {
	lib_list.append(tok.next().val);
}

AST *Parser::make_if() {
	if(tok.skip("if")) {
		AST *cond = expr_entry();
		ast_vector then = eval(0, 0), else_block;
		if(tok.skip("else")) {
			else_block = eval(0, 0);
			tok.skip("end");
		} else if(tok.skip("end")) {}
		AST *i = new IfAST(cond, then, else_block);
		visit(i);
		return i;
	}
	return NULL;
}

int Parser::make_while() {
	uint32_t loopBgn = ntv.count, end, stepBgn[2], stepOn = 0;

	expr_entry(); // condition
	if(tok.skip(",")) {
		stepOn = 1;
		stepBgn[0] = tok.pos;
		for(; tok.tok[tok.pos].val[0] != ';'; tok.pos++);
	}
	if(!tok.skip(";")) error("error");
	ntv.gencode(0x83); ntv.gencode(0xf8); ntv.gencode(0x00);// cmp eax, 0
	ntv.gencode(0x75); ntv.gencode(0x05); // jne 5
	ntv.gencode(0xe9); end = ntv.count; ntv.gencode_int32(0);// jmp while end

	eval(0, BLOCK_LOOP);

	if(stepOn) {
		stepBgn[1] = tok.pos;
		tok.pos = stepBgn[0];
		expr_entry();
		tok.pos = stepBgn[1];
	}

	uint32_t n = 0xFFFFFFFF - ntv.count + loopBgn - 4;
	ntv.gencode(0xe9); ntv.gencode_int32(n); // jmp n
	ntv.gencode_int32_insert(ntv.count - end - 4, end);

	for(--break_list.count; break_list.count >= 0; break_list.count--) {
		ntv.gencode_int32_insert(ntv.count - break_list.addr_list[break_list.count] - 4, break_list.addr_list[break_list.count]);
	} break_list.count = 0;

	return 0;
}

int Parser::make_func() {
	uint32_t espBgn, params = 0;
	std::string funcName = tok.next().val;

	funcs.now++; funcs.inside = true;
	if(tok.skip("(")) { // get params
		do { declare_var(); tok.skip(); params++; } while(tok.skip(","));
		tok.skip(")");
	}
	funcs.append(funcName, ntv.count, params);
	undef_funcs.rep_undef(funcName, ntv.count);

	ntv.genas("push ebp");
	ntv.genas("mov ebp esp");
	espBgn = ntv.count + 2; ntv.genas("sub esp 0"); // align

	uint32_t pos_save[128], i;

	for(i = 0; i < params; i++) {
		ntv.gencode(0x8b); ntv.gencode(0x45);
		ntv.gencode(0x08 + (params - i - 1) * ADDR_SIZE);
		ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24);
		pos_save[i] = ntv.count; ntv.gencode(0x00);
	}

	for(int i = 0; i < return_list.count; i++) {
		ntv.gencode_int32_insert(ntv.count - return_list.addr_list[i] - 4, return_list.addr_list[i]);
	} return_list.count = 0;

	ntv.genas("add esp %u", ADDR_SIZE * (var.focus().size() + 6)); // add esp nn
	ntv.gencode(0xc9);// leave
	ntv.gencode(0xc3);// ret
	ntv.gencode_int32_insert(ADDR_SIZE * (var.focus().size() + 6), espBgn);


	for(i = 1; i <= params; i++) {
		ntv.code[pos_save[i - 1]] =
			256 - ADDR_SIZE * i + (((var.focus().size() + 6) * ADDR_SIZE) - 4);
	}
#ifdef DEBUG
	printf("%s() has %u funcs or vars\n", funcName.c_str(), var.focus().size());
#endif
	return 0;
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
