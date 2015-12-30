#include "lit.h"

Token tok;
mem_t mem;
ctrl_t break_list, return_list;

// ---- for native code --- //

char *File_read(char *s, int len, FILE *fp) { fread(s, 1, len, fp); return s; }

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

void free_addr(uint32_t addr) {
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

void *funcTable[] = {
	(void *) putNumber, // 0
	(void *) putString, // 4
	(void *) putln,			// 8
	(void *) malloc, 		// 12
	(void *) printf, 		// 16
	(void *) appendAddr,// 20
	(void *) ssleep, 		// 24
	(void *) fopen, 		// 28
	(void *) fprintf, 	// 32
	(void *) fclose,		// 36
	(void *) File_read,	// 40
	(void *) free_addr,	// 44
	(void *) freeAddr,	// 48
	(void *) fgets, 		// 52
};


// Lit class is from here

Lit::Lit() {
	tok.pos = 0; tok.size = 0xfff;
	mem.mem = (mem_info *)calloc(0x7ff, sizeof(mem_info));
}

Lit::~Lit() {
	freeAddr();
}

int Lit::execute(char *source) {
	lex(source);
	parser();
	run();
	return 0;
}

int Lit::run() {
	return ((int (*)(int *, void**))ntv.code)(0, funcTable);
}

void Lit::interpret() {
	std::string line, all;

	while(std::getline(std::cin, line)) {
		all += line + " ; ";
		line.clear();
	}

	clock_t bgn = clock();
		execute((char *)all.c_str());
	clock_t end = clock();
#ifdef DEBUG
	printf("time: %.3lf\n", (double)(end - bgn) / CLOCKS_PER_SEC);
#endif
}

void Lit::run_from_file(char *source) {
	std::ifstream ifs_src(source);
	if(!ifs_src) ::error("LitSystemError: cannot open file '%s'", source);
	std::istreambuf_iterator<char> it(ifs_src);
	std::istreambuf_iterator<char> last;
	std::string src_all(it, last);
	
	execute((char *)src_all.c_str());
}

