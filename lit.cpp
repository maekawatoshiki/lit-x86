#include "lit.h"
#include "asm.h"
#include "lex.h"
#include "expr.h"
#include "parse.h"
#include "token.h"
#include "util.h"
#include "option.h"
#include "library.h"

MemoryList mem;
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

void *manage_alloc(uint32_t size) {
	void *addr = calloc(sizeof(int), size);
	appendAddr((uint32_t) addr);
	return addr;
}

void appendAddr(uint32_t addr) {
	mem_info mi = {
		.addr = addr,
		.isfree = false
	};
	mem.mem.push_back(mi);
}

void free_addr_in_program(uint32_t addr) {
	for(int i = 0; i < mem.mem.size(); i++) {
		if(mem.mem[i].addr == addr) {
			free((void *)mem.mem[i].addr);
			mem.mem[i].isfree = false;
		}
	}
}

void freeAddr() {
	if(mem.count() > 0) {
		for(size_t i = 0; i < mem.count(); i++) {
			if(mem.mem[i].isfree == 0) {
				free((void *)mem.mem[i].addr);
				mem.mem.pop_back();
			}
		}
	}
}

char *rea_concat(char *a, char *b) {
	char *t = (char *)malloc(strlen(a) + strlen(b) + 2);
	strcpy(t, a);
	return strcat(t, b);
}

char *gets_stdin() {
	char *str = (char *)manage_alloc(256);
	fgets(str, 256, stdin);
	str[strlen(str) - 1] = '\0';
	return str;
}

void *funcTable[] = {
	(void *) putNumber, // 0
	(void *) putString, // 4
	(void *) putln,			// 8
	(void *) manage_alloc, 		// 12
	(void *) printf, 		// 16
	(void *) appendAddr,// 20
	(void *) ssleep, 		// 24
	(void *) fopen, 		// 28
	(void *) fprintf, 	// 32
	(void *) fclose,		// 36
	(void *) File_read,	// 40
	(void *) free_addr_in_program,	// 44
	(void *) freeAddr,	// 48
	(void *) gets_stdin, 		// 52
	(void *) rea_concat,// 56
	(void *) putc,			// 60
	(void *) strlen,		// 64
};


// Lit class is from here

Lit::Lit(int ac, char **av)
	:lex(tok), parser(tok), argc(ac), argv(av) {
	tok.pos = 0; tok.size = 0xfff;
	return_list.addr_list = (uint32_t *)calloc(sizeof(uint32_t), 1);
	break_list.addr_list = (uint32_t *)calloc(sizeof(uint32_t), 1);
}

Lit::~Lit() {
	freeAddr();
}

int Lit::execute(char *source) {
	lex.lex(source);
	parser.parser();
	run();
	return 0;
}

int Lit::run() {
	printf(""); 
	// this block writes out native code to file
	// {
	// 	std::ofstream bin("out", std::ios::binary);
	// 	bin.write((const char *)ntv.code, ntv.count);
	// 	bin.close();
	// }
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

int Lit::start() {
	show_option();
	return 0;
}
