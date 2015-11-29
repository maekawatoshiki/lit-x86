#include "lit.h"

extern unsigned char *ntvCode;
extern int ntvCount;

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
	mem.mem = (mem_info *)calloc(0x7ff, sizeof(mem_info));
	set_xor128();
	tok.tok = (Token *)calloc(sizeof(Token), tok.size);
	brks.addr = (uint32_t *)calloc(sizeof(uint32_t), 1);
	rets.addr = (uint32_t *)calloc(sizeof(uint32_t), 1);
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

void appendAddr(uint32_t addr) {
	// mem.mem = realloc(mem.mem, sizeof(mem_info) * (1 + mem.count));
	mem.mem[mem.count].addr = addr;
	mem.mem[mem.count++].isfree = 0;
}

void freeInProgram(uint32_t addr) {
	for(int i = 0; i < mem.count; i++) {
		if(mem.mem[i].addr == addr) {
			free((int *)mem.mem[i].addr);
			mem.mem[i].addr = 0;
			mem.mem[i].isfree = 1;
		}
	}
}

void freeAddr() {
	if(mem.count > 0) {
		for(--mem.count; mem.count >= 0; --mem.count) {
			if(mem.mem[mem.count].isfree == 0) {
				free((void *)mem.mem[mem.count].addr);
			}
		}
		mem.count = 0;
	}
}

unsigned int w;
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
	(void *) freeInProgram,// 48
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

size_t get_file_size(FILE *fp) {
	size_t sz;

	fseek(fp, 0, SEEK_END);
	sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	return sz;
}

void lit_interpret() {
	char *src;
	src = (char*)calloc(sizeof(char), 0xFFFF);
	char line[0xFF] = "";

	while(strcmp(line, "run\n") != 0) {
		printf(">> "); strcat(src, line);
		memset(line, 0, 0xFF);
		fgets(line, 0xFF, stdin);
	}

	clock_t bgn = clock();
		execute(src);
	clock_t end = clock();
	printf("time: %.3lf\n", (double)(end - bgn) / CLOCKS_PER_SEC);
}

void lit_run(char *file) {
	char *src;
	FILE *srcfp = fopen(file, "rb");

	{
		if(!srcfp) { perror("file not found"); exit(0); }
		size_t size = get_file_size(srcfp);
		src = (char*)calloc(sizeof(char), size + 2);
		fread(src, sizeof(char), size, srcfp);
		fclose(srcfp);
	}

	clock_t bgn = clock();
		execute(src);
	clock_t end = clock();
	printf("time: %.3lf\n", (double)(end - bgn) / CLOCKS_PER_SEC);
}
