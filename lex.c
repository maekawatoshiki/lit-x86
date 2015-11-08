#include "lex.h"

int lex(char *code) {
  size_t codeSize = strlen(code), line = 1;
	int iswindows = 0;

	for(int i = 0; i < codeSize; i++) {
 		if(tok.size <= i) tok.tok = (Token *)realloc(tok.tok, (tok.size += 512 * sizeof(Token)));
		
		if(isdigit(code[i])) { // number?
    
			for(; isdigit(code[i]); i++)
				strncat(tok.tok[tok.pos].val, &(code[i]), 1);
			tok.tok[tok.pos].nline = line;
      i--; skip_tok();
    
		} else if(isalpha(code[i])) { // ident?
		
			char *str = tok.tok[tok.pos].val;
      for(; isalpha(code[i]) || isdigit(code[i]) || code[i] == '_'; i++)
        *str++ = code[i];
      tok.tok[tok.pos].nline = line;
      i--; skip_tok();
    
		} else if(code[i] == ' ' || code[i] == '\t') { // space char?
    } else if(code[i] == '#') { // comment?
		
			for(i++; code[i] != '\n'; i++) { } line++;
		
		} else if(code[i] == '"') { // string?
		
			strcpy(tok.tok[tok.pos].val, "\"");
			tok.tok[tok.pos++].nline = line;
			for(i++; code[i] != '"' && code[i] != '\0'; i++)
				strncat(tok.tok[tok.pos].val, &(code[i]), 1);
			tok.tok[tok.pos].nline = line;
			if(code[i] == '\0') error("error: %d: expected expression '\"'", tok.tok[tok.pos].nline);
			skip_tok();
	
		} else if(code[i] == '\n' || (iswindows=(code[i] == '\r' && code[i+1] == '\n'))) {
			
			i += iswindows;
			strcpy(tok.tok[tok.pos].val, ";");
			tok.tok[tok.pos].nline = line++; skip_tok();
		
		} else {
		
			strncat(tok.tok[tok.pos].val, &(code[i]), 1);
			if(code[i+1] == '=' ||
				(code[i]=='+' && code[i+1]=='+') ||
				(code[i]=='-' && code[i+1]=='-'))
					strncat(tok.tok[tok.pos].val, &(code[++i]), 1);
			tok.tok[tok.pos].nline = line;
			tok.pos++;

		}
  }
  tok.tok[tok.pos].nline = line;
#ifdef NDEBUG
#else
	for(int32_t i = 0; i < tok.pos; i++) {
		printf("tk: %d > %s\n", i, tok.tok[i].val);
	}
#endif
  tok.size = tok.pos - 1;

	return 0;
}


