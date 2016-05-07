#include "lit.h"
#include "asm.h"
#include "lex.h"
#include "expr.h"
#include "parse.h"
#include "token.h"
#include "util.h"
#include "option.h"

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
		bool const_mem;
	public:
		bool marked;
		MemoryInfo(void *a, size_t sz, bool const_m = false) : 
			addr(a), size(sz), marked(false), const_mem(const_m) { 
		}
		void *get_addr() { return addr; }
		uint32_t get_size() { return size; }
		bool is_const() { return const_mem; }
		void mark() { marked = true; }
		void free_mem() { free(addr); }
	};

	const size_t max_mem = 256 * 1024; // 256KB
	size_t current_mem = 0;
	std::map<void *, MemoryInfo *> mem_list;
	std::map<void *, bool> root_ptr;

	void *alloc_const(uint32_t size) { // allocate constant memory(for string)
		void *addr = alloc(size, 1);
		mem_list[(void *)addr] = new MemoryInfo(addr, size, true);
		return addr;	
	}
	void *alloc(uint32_t size, uint32_t byte) {
		if(current_mem >= max_mem) {
			gc();
		}
		void *addr = calloc(size, byte);
		current_mem += size;
		mem_list[(void *)addr] = new MemoryInfo(addr, size);
		return addr;	
	}

	uint32_t get_size(void *addr) {
		MemoryInfo *m = mem_list[(void *)addr];
		if(m == NULL) return 0;
		return m->get_size();
	}

	void append_ptr(void *ptr) {
		root_ptr[(void *)ptr] = true;
	}
	void delete_ptr(void *ptr) {
		root_ptr[(void *)ptr] = false;
	}
	void gc_mark() {
		for(std::map<void *, bool>::iterator it = root_ptr.begin(); it != root_ptr.end(); ++it) {
			if(it->second == true) {
				int *ptr = (int *)it->first;
				MemoryInfo *m = mem_list[(void *)*ptr];
				if(m != NULL) {
					if(m->is_const()) continue;
					m->mark();
					// std::cout << "marked success: " << (uint32_t)m->get_addr() << std::endl;
				} else {
					mem_list.erase(mem_list.find((void *)*ptr));
				}
			}
		} 
	}
	void gc_sweep() {
		for(std::map<void *, MemoryInfo *>::iterator it = mem_list.begin(); it != mem_list.end(); ++it) {
			if(it->second->marked == false) {
				if(it->second->is_const()) continue;
				std::cout << "*** freed success: " << (uint64_t)it->second->get_addr() << ", size: " << it->second->get_size() << "bytes ***" << std::endl;
				it->second->free_mem();
				current_mem -= it->second->get_size();
				mem_list.erase(it);
			}
		} 
		for(std::map<void *, MemoryInfo *>::iterator it = mem_list.begin(); it != mem_list.end(); ++it) 
			it->second->marked = false;
	}
	void gc() {
		gc_mark();
		gc_sweep();
	}

	void free_all_mem() {
		for(std::map<void *, MemoryInfo *>::iterator it = mem_list.begin(); it != mem_list.end(); ++it) {
			it->second->free_mem();
			// std::cout << "freed success: " << (uint32_t)it->second->get_addr() << std::endl;
		}
	}
};

char *rea_concat(char *a, char *b) {
	char *t = (char *)LitMemory::alloc(strlen(a) + strlen(b) + 1, 1);
	strcpy(t, a);
	return strcat(t, b);
}
char *rea_concat_char(char *a, int b) {
	char *t = (char *)LitMemory::alloc(strlen(a) + 2, 1);
	strcpy(t, a);
	t[strlen(t)] = b;
	return t;
}
char *str_replace(char *s, char *b, char *a) { // replace from b to a in s
	std::string str = s;
	std::string from = b;
	std::string to = a;
	std::string::size_type pos = str.find(from);
	while(pos != std::string::npos){
		str.replace(pos, from.size(), to);
		pos = str.find(from, pos + to.size());
	}
	char *ret = (char *)LitMemory::alloc(str.size() + 1, sizeof(char));
	strcpy(ret, str.c_str());
	return ret;
}
uint32_t str_split(char *s, char *t) { // divide s by t, and make them array  
	std::vector<std::string> splited;
	char *str = (char *)malloc(strlen(s) + 1);
	strcpy(str, s);
	char *p = strtok(str, t);
	splited.push_back(p);
	while(p != NULL) {
		p = strtok(NULL, t);
		if(p != NULL)
			splited.push_back(p);
	}
	uint64_t *ary = (uint64_t *)LitMemory::alloc(splited.size(), sizeof(int64_t));
	for(int i = 0; i < splited.size(); i++) {
		char *ss = (char *)LitMemory::alloc(splited[i].size() + 1, sizeof(char));
		strcpy(ss, splited[i].c_str());
		ary[i] = (uint64_t)ss;
	}
	return (uint64_t)ary;
}
char *gets_stdin() {
	char *str;	
	std::string input;
	std::getline(std::cin, input);
	str = (char *)LitMemory::alloc(input.size() + 1, ADDR_SIZE);
	strcpy(str, input.c_str());
	return str;
}
int streql(char *a, char *b) { return strcmp(a, b) == 0 ? 1 : 0; }
int strne(char *a, char *b) { return strcmp(a, b) != 0 ? 1 : 0; }
char *str_copy(char *_s) { return strcpy((char *)LitMemory::alloc(strlen(_s) + 1, sizeof(char)), _s); }

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
	(void *) putchar,			// 60
	(void *) strlen,		// 64
	(void *) LitMemory::get_size, // 68
	(void *) LitMemory::gc,	// 72
	(void *) rea_concat_char, // 76
	(void *) streql,		// 80
	(void *) strne, 		// 84
	(void *) str_replace,	// 88
	(void *) str_split,		// 92
	(void *) str_copy,		// 96
};


// Lit class is from here

Lit::Lit(int ac, char **av)
	:lex(tok), parser(tok), argc(ac), argv(av) {
	tok.pos = 0; tok.size = 0xfff;
}

Lit::~Lit() {
	LitMemory::free_all_mem();
	// freeAddr();
}

int Lit::execute(char *source) {
	lex.lex(source);
	parser.parser();
	// if((fork()) == 0) run();
	// int status = 0;
	// wait(&status);
	// if(!WIFEXITED(status)) {
	// 	puts("LitRuntimeError: *** the process was terminated abnormally ***");
	// }
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
