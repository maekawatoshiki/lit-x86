#include "lit.h"

void init() {
#if defined(WIN32) || defined(WINDOWS)
	ntvCode = VirtualAlloc(NULL, 0xFFF, MEMORY_COMMIT, PAGE_EXECUTE_READWRITE);
#else
	long memsize = 0xFFFF + 1;
	if(posix_memalign((void **)&ntvCode, memsize, memsize))
		perror("posix_memalign");
	if(mprotect(ntvCode, memsize, PROT_READ | PROT_WRITE | PROT_EXEC))
		perror("mprotect");
#endif
	tok.pos = ntvCount = 0; tok.size = 0xfff;
	set_xor128();
	tok.tok = calloc(sizeof(Token), tok.size);
	brks.addr = calloc(sizeof(uint32_t), 1);
	rets.addr = calloc(sizeof(uint32_t), 1);
}

void dispose() {
	free(ntvCode);
	free(brks.addr);
	free(rets.addr);
	free(tok.tok);
	freeAddr();
}


int32_t lex(char *code) {
  int32_t codeSize = strlen(code), line = 1;
	int32_t iswindows = 0;

	for(int32_t i = 0; i < codeSize; i++) {
 		if(tok.size <= i)
			tok.tok = realloc(tok.tok, (tok.size += 512 * sizeof(Token)));
		if(isdigit(code[i])) { // number?
      for(; isdigit(code[i]); i++)
				strncat(tok.tok[tok.pos].val, &(code[i]), 1);
			tok.tok[tok.pos].nline = line;
      i--; tok.pos++;
    } else if(isalpha(code[i])) { // ident?
			char *str = tok.tok[tok.pos].val;
      for(; isalpha(code[i]) || isdigit(code[i]) || code[i] == '_'; i++)
        *str++ = code[i];
      tok.tok[tok.pos].nline = line;
      i--; tok.pos++;
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
			tok.pos++;
		} else if(code[i] == '\n' ||
			(iswindows=(code[i] == '\r' && code[i+1] == '\n'))) {
			i += iswindows;
			strcpy(tok.tok[tok.pos].val, ";");
			tok.tok[tok.pos].nline = line++; tok.pos++;
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


char *replaceEscape(char *str) {
	int32_t i;
	char *pos;
	char escape[12][3] = {
		"\\a", "\a",
		"\\r", "\r",
		"\\f", "\f",
		"\\n", "\n",
		"\\t", "\t",
		"\\b", "\b"
	};
	for (i = 0; i < 12; i += 2)
	{
		while ((pos = strstr(str, escape[i])) != NULL)
		{
			*pos = escape[i + 1][0];
			memmove(pos + 1, pos + 2, strlen(str) - 2 + 1);
		}
	}
	return str;
}

void putNumber(int32_t n) {
	printf("%d", n);
}
void putString(int32_t *n) {
	printf("%s", (char *)n);
}
void putln() { printf("\n"); }

void ssleep(uint32_t t) {
#if defined(WIN32) || defined(WINDOWS)
	Sleep(t * CLOCKS_PER_SEC / 1000);
#else
	usleep(t * CLOCKS_PER_SEC / 1000);
#endif
}

void appendAddr(int32_t addr) {
	mem.addr[mem.count++] = addr;
}

void freeAddr() {
	if(mem.count > 0) {
		for(--mem.count; mem.count >= 0; --mem.count) {
			free((void *)mem.addr[mem.count]);
		}
		mem.count = 0;
	}
}

void set_xor128() {
#if defined(WIN32) || defined(WINDOWS)
#else
	w = 1234 + getpid() ^ 0xFFBA9285;
	puts("set_xor128()");
#endif
}

int xor128() {
  static uint32_t x = 123456789;
  static uint32_t y = 362436069;
  static uint32_t z = 521288629;
  uint32_t t;
  t = x ^ (x << 11); x = y; y = z; z = w;
  w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
  return ((int32_t)w < 0) ? -(int32_t)w : (int32_t)w;
}

void *funcTable[] = { 
	(void *) putNumber, // 0
	(void *) putString, // 4
	(void *) putln,			// 8
	(void *) malloc, 		// 12
	(void *) xor128, 		// 16
	(void *) printf, 		// 20
	(void *) appendAddr,// 24
	(void *) ssleep, 		// 28
	(void *) fopen, 		// 32
	(void *) fprintf, 	// 36
	(void *) fclose,		// 40
	(void *) fgets,			// 44
	(void *) free, 			// 48
	(void *) freeAddr,	// 52
};

int run() {
	printf("size: %dbyte, %.2lf%%\n", ntvCount, ((double)ntvCount / 0xFFFF) * 100.0);
	return ((int (*)(int *, void**))ntvCode)(0, funcTable);
}

int execute(char *source) {
	init();
	lex(source);
	parser();
	run();
	dispose();
	return 0;
}

