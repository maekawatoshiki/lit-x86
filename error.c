#include "error.h"

int stmt_entry() {
	int save_tok_pos = tok.pos;
	tok.pos = 0;

	while(tok.pos < tok.size) {
		if(stmt_if()) {}
		else if(stmt_while()) {}
		else if(stmt_for()) {}
		else if(stmt_func_declare()) {}
		else if(stmt_func_call()) {}
		else if(stmt_assignment()) {}
		else if(stmt_expr()) {}
		else if(stmt_null()) {}
	}

	tok.pos = save_tok_pos;
}

int stmt_if() {
}
