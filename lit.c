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

