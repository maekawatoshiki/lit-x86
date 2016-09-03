#include "lit.h"
#include "lex.h"
#include "expr.h"
#include "parse.h"
#include "token.h"
#include "util.h"
#include "codegen.h"
#include "option.h"

// ---- for native code --- //

char *File_read(char *s, int len, FILE *fp) { fread(s, 1, len, fp); return s; }

namespace LitMemory {
	class MemoryInfo {
		void *addr;
		size_t size, real_size;
		bool const_mem;
	public:
		bool marked;
		MemoryInfo(void *a, size_t sz, size_t realsz, bool const_m = false) : 
			addr(a), size(sz), real_size(realsz), marked(false), const_mem(const_m) { 
		}
		void *get_addr() { return addr; }
		uint32_t get_size() { return size; }
		uint32_t get_real_size() { return real_size; }
		void set_size(int sz) { size = sz; }
		bool is_const() { return const_mem; }
		void mark() { marked = true; }
		void free_mem() { free(addr); }
	};

	const size_t max_mem = 20*1024*1024; // 20MB
	size_t current_mem = 0;
	void *newest_ptr = nullptr;
	std::map<void *, MemoryInfo *> mem_list;
	std::map<void *, bool> root_ptr;

	std::string byte_with_unit(uint32_t byte) {
		std::string unit[] = {"B", "KB", "MB", "GB"};
		uint32_t b = 1;
		std::stringstream ss;
		for(int i = 0; i < 4; i++) {
			if(byte < b*1024) {
				ss << (double)byte/b << unit[i];
				break;
			}
			b *= 1024;
		}
		return ss.str().c_str();
	}

	void *alloc_const(uint32_t size) { // allocate constant memory(for string)
		void *addr = alloc(size, sizeof(char));
		mem_list[addr] = new MemoryInfo(addr, size, size*2, true);
		return addr;	
	}
	void *alloc(uint32_t size, uint32_t byte) {
		if(current_mem >= max_mem) gc(); // if allocated memory is over max_mem, do GC
		void *addr = calloc(size*2, byte);
		if(!addr) error("LitSystemError: No enough memory");
		current_mem += size*2;
		// std::cout << "*** allocated addr: " << addr << std::endl;
		mem_list[addr] = new MemoryInfo(addr, size, size*2);
		newest_ptr = addr;
		return addr;	
	}

	bool is_allocated_addr(void *addr) {
		return mem_list.count(addr);
	}

	uint32_t get_size(void *addr) {
		MemoryInfo *m = mem_list[(void *)addr];
		if(m == NULL) return 0;
		return m->get_size();
	}
	uint32_t get_real_size(void *addr) {
		MemoryInfo *m = mem_list[(void *)addr];
		if(m == NULL) return 0;
		return m->get_real_size();
	}
	void set_size(void *addr, int sz) {
		MemoryInfo *m = mem_list[(void *)addr];
		if(m == NULL) return;
		m->set_size(sz);
	}

	void append_ptr(void *ptr) {
		root_ptr[ptr] = true;
	}
	void delete_ptr(void *ptr) {
		root_ptr.erase(ptr);
	}
	void gc_mark() {
		mem_list[newest_ptr]->mark();

		for(std::map<void *, bool>::iterator it = root_ptr.begin(); it != root_ptr.end(); ++it) {
			if(it->second == true) {
				uint64_t *ptr = (uint64_t *)it->first;
				if(mem_list.count((void*)*ptr)) {
					MemoryInfo *m = mem_list[(void *)*ptr];
					if(m->is_const()) continue;
					m->mark();
					// std::cout << "*** marked success: " << m->get_addr() << std::endl;
					std::function<void(uint64_t *)> rec_mark = [&](uint64_t *ptr) {
						for(;mem_list.count((void*)*ptr); ptr++) {
							m = mem_list[(void*)*ptr];
							if(m->is_const()) continue;
							m->mark();
							rec_mark((uint64_t*)m->get_addr());
						}
					};
					ptr = (uint64_t *)m->get_addr();
					rec_mark(ptr);
				} 
			}
		} 
	}
	void gc_sweep() {
		for(std::map<void *, MemoryInfo *>::iterator it = mem_list.begin(); it != mem_list.end(); ++it) {
			if(it->second->marked == false) {
				if(it->second->is_const()) continue;
				// std::cout << "*** freed success: " << it->second->get_addr() << ", size: " << byte_with_unit(it->second->get_size()) << "bytes ***" << std::endl;
				it->second->free_mem();
				current_mem -= it->second->get_real_size();
				mem_list.erase(it);
			}
		} 
		for(std::map<void *, MemoryInfo *>::iterator it = mem_list.begin(); it != mem_list.end(); ++it) 
			it->second->marked = false;
	}
	void gc() {
		// uint32_t mem_before_gc = current_mem;
		// clock_t bgn = clock();
		gc_mark();
		gc_sweep();
		// std::cerr << "*** GC(" << (((double)clock() - bgn) / CLOCKS_PER_SEC) << "s) after: " 
		// 	<< byte_with_unit(current_mem) 
		// 	<< ", deleted: "
		// 		<< byte_with_unit(mem_before_gc - current_mem)
		// 	<< " ***" << std::endl;
	}

	void free_all_mem() {
		for(std::map<void *, MemoryInfo *>::iterator it = mem_list.begin(); it != mem_list.end(); ++it) {
			if(it->second == nullptr) continue;
			it->second->free_mem();
			// std::cout << "finalize: freed success: " << it->second->get_addr() << std::endl;
		}
	}
};

// Lit class is from here

Lit::Lit(int ac, char **av)
	:lex(tok), parser(tok), argc(ac), argv(av) {
	tok.pos = 0; tok.size = 0xfff;
}

Lit::~Lit() {
	LitMemory::free_all_mem();
}

int Lit::execute(char *source, bool enable_emit_llvm) {
	lex.lex(source);
	llvm::Module *program = parser.parser();
	Codegen::run(program, false/*optimize*/, enable_emit_llvm);
	// if((fork()) == 0) run();
	// int status = 0;
	// wait(&status);
	// if(!WIFEXITED(status)) {
	// 	puts("LitRuntimeError: *** the process was terminated abnormally ***");
	// }
	return 0;
}

void Lit::interpret() {
	std::string line, all;

	while(std::getline(std::cin, line)) {
		all += line + " ; ";
		line.clear();
	}

	clock_t bgn = clock();
		execute((char *)all.c_str(), false);
	clock_t end = clock();
#ifdef DEBUG
	printf("time: %.3lf\n", (double)(end - bgn) / CLOCKS_PER_SEC);
#endif
}

void Lit::run_from_file(char *source, bool enable_emit_llvm) {
	std::ifstream ifs_src(source);
	if(!ifs_src) ::error("LitSystemError: cannot open file '%s'", source);
	std::istreambuf_iterator<char> it(ifs_src);
	std::istreambuf_iterator<char> last;
	std::string src_all(it, last);
	
	execute((char *)src_all.c_str(), enable_emit_llvm);
}

int Lit::start() {
	show_option();
	return 0;
}
