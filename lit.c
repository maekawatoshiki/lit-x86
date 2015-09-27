#include "lit.h"

void init() {
#if defined(WIN32) || defined(WINDOWS)
	ntvCode = (unsigned char*) calloc(sizeof(unsigned char), 0xFFFF);
#else
	long memsize = 0xFFFF + 1;
	if((posix_memalign((void **)&ntvCode, memsize, memsize)))
		perror("posix_memalign");
	if(mprotect((void*)ntvCode, memsize, PROT_READ | PROT_WRITE | PROT_EXEC))
		perror("mprotect");
	memset(ntvCode, 0, 0xFFFF);
#endif
	tkpos = ntvCount = 0; tksize = 0xfff;
	set_xor128();
	tok = calloc(sizeof(Token), tksize);
	brks.addr = calloc(sizeof(uint32_t), 1);
	rets.addr = calloc(sizeof(uint32_t), 1);
}

void dispose() {
	free(ntvCode);
	free(brks.addr);
	free(rets.addr);
	free(tok);
	freeMem();
}


int32_t lex(char *code) {
  int32_t codeSize = strlen(code), line = 1;
	int32_t iswindows = 0;

	for(int32_t i = 0; i < codeSize; i++) {
 		if(tksize <= i)
			tok = realloc(tok, (tksize += 512 * sizeof(Token)));
		if(isdigit(code[i])) { // number?
      for(; isdigit(code[i]); i++)
				strncat(tok[tkpos].val, &(code[i]), 1);
			tok[tkpos].nline = line;
      i--; tkpos++;
    } else if(isalpha(code[i])) { // ident?
      for(; isalpha(code[i]) || isdigit(code[i]) || code[i] == '_'; i++)
        strncat(tok[tkpos].val, &(code[i]), 1);
      tok[tkpos].nline = line;
      i--; tkpos++;
    } else if(code[i] == ' ' || code[i] == '\t') { // space char?
    } else if(code[i] == '#') { // comment?
			for(i++; code[i] != '\n'; i++) { } line++;
		} else if(code[i] == '"') { // string?
			strcpy(tok[tkpos].val, "\"");
			tok[tkpos++].nline = line;
			for(i++; code[i] != '"' && code[i] != '\0'; i++)
				strncat(tok[tkpos].val, &(code[i]), 1);
			tok[tkpos].nline = line;
			if(code[i] == '\0') error("error: %d: expected expression '\"'", tok[tkpos].nline);
			tkpos++;
		} else if(code[i] == '\n' ||
			(iswindows=(code[i] == '\r' && code[i+1] == '\n'))) {
			i += iswindows;
			strcpy(tok[tkpos].val, ";");
			tok[tkpos].nline = line++; tkpos++;
		} else {
			strncat(tok[tkpos].val, &(code[i]), 1);
			if(code[i+1] == '=' ||
				(code[i]=='+' && code[i+1]=='+') ||
				(code[i]=='-' && code[i+1]=='-'))
					strncat(tok[tkpos].val, &(code[++i]), 1);
			tok[tkpos].nline = line;
			tkpos++;
    }
  }
  tok[tkpos].nline = line;
#ifdef NDEBUG
#else
	for(int32_t i = 0; i < tkpos; i++) {
		printf("tk: %d > %s\n", i, tok[i].val);
	}
#endif
  tksize = tkpos - 1;

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

void appendMem(int32_t addr) {
	memad.addr[memad.count++] = addr;
}

void ssleep(int32_t t) {
	usleep((t / 1000.0 * CLOCKS_PER_SEC));
}

void freeMem() {
	for(--memad.count; memad.count>=0; --memad.count) {
		free((void*)memad.addr[memad.count]);
		printf("Free: %p\n", (void*)memad.addr[memad.count]);
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

void *funcTable[] = { (void *) putNumber, (void*) putString, (void*) putln,
	 (void*)malloc, (void*) xor128, (void*) printf, (void*) appendMem, (void*) usleep };

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

