#include "lex.h"
#include "lit.h"
#include "asm.h"
#include "expr.h"
#include "parse.h"
#include "util.h"

int Lexer::lex(char *code) {
	size_t code_sz = strlen(code), line = 1;
	int iswindows = 0;
	std::string str;
	token_t tmp_tok;

	for(size_t i = 0; i < code_sz; i++) {

		str.clear();
		if(isdigit(code[i])) { // number?

			for(; isdigit(code[i]); i++)
				str += code[i];
			tmp_tok.val = str;
			tmp_tok.nline = line;
			tmp_tok.type = TOK_NUMBER;
			tok.tok.push_back(tmp_tok);
			i--; SKIP_TOK;

		} else if(isalpha(code[i])) { // ident?

			for(; isalpha(code[i]) || isdigit(code[i]) || code[i] == '_'; i++)
				str += code[i];
			tmp_tok.val = str;
			tmp_tok.nline = line;
			tmp_tok.type = TOK_IDENT;
			tok.tok.push_back(tmp_tok);
			i--; SKIP_TOK;

		} else if(code[i] == ' ' || code[i] == '\t') { // space char?
		} else if(code[i] == '#') { // comment?
			for(i++; code[i] != '\n'; i++) { ; } line++;
		} else if(code[i] == '\"') { // string?

			for(i++; ; i++) {
				if(code[i] == '\\' && code[i+1] == '"') {
					str += code[i++]; str += code[i];
				} else if(code[i] == '"') break;
				else 
					str += code[i];
			}
			tmp_tok.val = str;
			tmp_tok.nline = line;
			tmp_tok.type = TOK_STRING;
			tok.tok.push_back(tmp_tok);
			if(code[i] == '\0') 
				error("error: %d: expected expression '\"'", tok.tok[tok.pos].nline);
			SKIP_TOK;

		} else if(code[i] == '\'') {

			tmp_tok.val = code[++i];
			tmp_tok.nline = line;
			tmp_tok.type = TOK_CHAR;
			tok.tok.push_back(tmp_tok);
			SKIP_TOK; i++;

		} else if(code[i] == '\n' || (iswindows=(code[i] == '\r' && code[i+1] == '\n'))) {

			i += iswindows;
			tmp_tok.val = ";";
			tmp_tok.nline = line++;
			tmp_tok.type = TOK_END;
			tok.tok.push_back(tmp_tok);
			SKIP_TOK; 

		} else {

			str = code[i];
			if(code[i+1] == '=' || 
					(code[i]==':' && code[i+1]==':') || 
					(code[i]=='[' && code[i+1]==']') ||
					(code[i]=='+' && code[i+1]=='+') ||
					(code[i]=='-' && code[i+1]=='-') ||
					(code[i]=='.' && code[i+1]=='.'))
				str += code[++i];
			tmp_tok.val = str;
			tmp_tok.nline = line;
			tmp_tok.type = TOK_SYMBOL;
			tok.tok.push_back(tmp_tok);
			SKIP_TOK;

		}
	}
#ifdef DEBUG
	for(int32_t i = 0; i < tok.pos; i++) {
		printf("tk: %d:%d > %s\n", i, tok.tok[i].nline, tok.tok[i].val.c_str());
	}
#endif
	tmp_tok.val = ";";
	tmp_tok.nline = line++;
	tmp_tok.type = TOK_END;
	tok.tok.push_back(tmp_tok);
	SKIP_TOK; 
	tok.size = tok.tok.size() - 1;

	return 0;
}
