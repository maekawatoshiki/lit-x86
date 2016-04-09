#include "lit.h"
#include "asm.h"
#include "lex.h"
#include "expr.h"
#include "parse.h"
#include "token.h"
#include "util.h"
#include "option.h"

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

namespace LitMemory {
	class MemoryInfo {
		void *addr;
		size_t size;
	public:
		bool marked;
		MemoryInfo(void *a, size_t sz) : 
			addr(a), size(sz), marked(false) { 
		}
		void *get_addr() { return addr; }
		uint32_t get_size() { return size; }
		void mark() { marked = true; }
		void free_mem() { free(addr); }
	};

	const size_t max_mem = 256 * 1024; // 256KB
	size_t current_mem = 0;
	std::map<uint32_t, MemoryInfo *> mem_list;
	std::map<uint32_t, bool> root_ptr;

	void *alloc(uint32_t size) {
		if(current_mem >= max_mem) {
			gc();
		}
		void *addr = calloc(size, sizeof(int));
		current_mem += size;
		mem_list[(uint32_t)addr] = new MemoryInfo(addr, size);
		return addr;	
	}

	uint32_t get_size(void *addr) {
		MemoryInfo *m = mem_list[(uint32_t)addr];
		if(m == NULL) return 0;
		return m->get_size();
	}

	void append_ptr(void *ptr) {
		root_ptr[(uint32_t)ptr] = true;
	}
	void delete_ptr(void *ptr) {
		root_ptr[(uint32_t)ptr] = false;
	}
	void gc_mark() {
		for(std::map<uint32_t, bool>::iterator it = root_ptr.begin(); it != root_ptr.end(); ++it) {
			if(it->second == true) {
				int *ptr = (int *)it->first;
				MemoryInfo *m = mem_list[(uint32_t)*ptr];
				if(m != NULL) {
					m->mark();
					// std::cout << "marked success: " << (uint32_t)m->get_addr() << std::endl;
				} else {
					mem_list.erase(mem_list.find((uint32_t)*ptr));
				}
			}
		} 
	}
	void gc_sweep() {
		for(std::map<uint32_t, MemoryInfo *>::iterator it = mem_list.begin(); it != mem_list.end(); ++it) {
			if(it->second->marked == false) {
				// std::cout << "freed success: " << (uint32_t)it->second->get_addr() << ", size: " << it->second->get_size() << "bytes" << std::endl;
				it->second->free_mem();
				current_mem -= it->second->get_size();
				mem_list.erase(it);
			}
		} 
		for(std::map<uint32_t, MemoryInfo *>::iterator it = mem_list.begin(); it != mem_list.end(); ++it) 
			it->second->marked = false;
	}
	void gc() {
		gc_mark();
		gc_sweep();
	}

	void free_all_mem() {
		for(std::map<uint32_t, MemoryInfo *>::iterator it = mem_list.begin(); it != mem_list.end(); ++it) {
			it->second->free_mem();
			// std::cout << "freed success: " << (uint32_t)it->second->get_addr() << std::endl;
		}
	}
};

char *rea_concat(char *a, char *b) {
	char *t = (char *)LitMemory::alloc(strlen(a) + strlen(b) + 1);
	strcpy(t, a);
	return strcat(t, b);
}
char *rea_concat_char(char *a, int b) {
	char *t = (char *)LitMemory::alloc(strlen(a) + 1 + 1);
	strcpy(t, a);
	t[strlen(t)] = b;
	return t;
}

char *gets_stdin() {
	char *str;	
	std::string input;
	std::getline(std::cin, input);
	str = (char *)LitMemory::alloc(input.size() + 1);
	strcpy(str, input.c_str());

	return str;
}

void *funcTable[] = {
	(void *) putNumber, // 0
	(void *) putString, // 4
	(void *) putln,			// 8
	(void *) LitMemory::alloc, 		// 12
	(void *) printf, 		// 16
	(void *) LitMemory::delete_ptr,// 20
	(void *) ssleep, 		// 24
	(void *) fopen, 		// 28
	(void *) fprintf, 	// 32
	(void *) fclose,		// 36
	(void *) File_read,	// 40
	(void *) LitMemory::append_ptr,	// 44
	(void *) LitMemory::free_all_mem,	// 48
	(void *) gets_stdin, 		// 52
	(void *) rea_concat,// 56
	(void *) putc,			// 60
	(void *) strlen,		// 64
	(void *) LitMemory::get_size, // 68
	(void *) LitMemory::gc,	// 72
	(void *) rea_concat_char, // 76
};


// Lit class is from here

Lit::Lit(int ac, char **av)
	:lex(tok), parser(tok), argc(ac), argv(av) {
	tok.pos = 0; tok.size = 0xfff;
	return_list.addr_list = (uint32_t *)calloc(sizeof(uint32_t), 1);
	break_list.addr_list = (uint32_t *)calloc(sizeof(uint32_t), 1);
}

Lit::~Lit() {
	LitMemory::free_all_mem();
	// freeAddr();
}

#include <sys/types.h>
#include <sys/wait.h>

int Lit::execute(char *source) {
	lex.lex(source);
	parser.parser();
	if((fork()) == 0) run();
	int status = 0;
	wait(&status);
	if(!WIFEXITED(status)) {
		puts("LitRuntimeError: *** the process was terminated abnormally ***");
	}
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
