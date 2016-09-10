#include "stdfunc.h"

extern "C" {
	void put_num(int n) {
		printf("%d", n);
	}
	void put_num64(long long int n) {
		printf("%lld", n);
	}
	void put_num_float(double n) {
		printf("%.10g", n);
	}
	void put_char(char ch) {
		putchar(ch);
		fflush(stdout);
	}
	void put_string(char *s) {
		printf("%s", s);
	}
	void put_array(int *ary) {
		int size = LitMemory::get_size(ary);
		if(size == -1) return;
		printf("[ ");
		for(int i = 0; i < size; i++) {
			if(LitMemory::is_allocated_addr(reinterpret_cast<void *>(ary[i])))
				put_array(reinterpret_cast<int *>(ary[i]));
			else
				printf("%d ", ary[i]);
		}
		printf("] ");
	}
	void put_array_float(double *ary) {
		int size = LitMemory::get_size(ary);
		if(size == -1) return;
		printf("[ ");
		for(int i = 0; i < size; i++) {
			printf("%.10g ", ary[i]);
		}
		printf("]");
	}
	void put_array_str(char *ary[]) {
		int size = LitMemory::get_size(ary);
		if(size == -1) return;
		printf("[ ");
		for(int i = 0; i < size; i++) {
			// if(LitMemory::is_allocated_addr((void *)ary[i]))
			// 	put_array_str((char **)ary[i]);
			// else
				printf("%s ", ary[i]);
		}
		printf("]");
	}
	void put_ln() {
		putchar('\n');
	}
	//***********************************************************************************/
	void *create_array(uint32_t size, uint32_t byte) {
		return LitMemory::alloc(size, byte);
	}
	char *str_concat(char *a, char *b) {
		// char *t = (char *)LitMemory::alloc(strlen(a) + strlen(b) + 1, 1);
		// strcpy(t, a);
		// return strcat(t, b);
		int size = LitMemory::get_size(a), realsize = LitMemory::get_real_size(a);
		if(size + strlen(b) > realsize) {
			size = size + strlen(b);
			char *mem = (char *)LitMemory::alloc(size, sizeof(char));
			memcpy(mem, a, sizeof(char) * (size-1));
			// mem[size-2] = n;
			memmove(&(mem[size-strlen(b)-1]), b, strlen(b));
			LitMemory::set_size(mem, size);
			return mem;
		}
		memmove(&(a[size-1]), b, strlen(b));
		LitMemory::set_size(a, size + strlen(b));
		return a;
	}
	char *str_register_to_memmgr(char *a) {
		int alen = strlen(a);
		char *newS = (char *)LitMemory::alloc_const(alen + 1);
		strcpy(newS, a);
		return newS;
	}
	char *str_concat_char(char *a, char n) {
		int size = LitMemory::get_size(a), realsize = LitMemory::get_real_size(a);
		if(size + 1 > realsize) {
			size = size == 0 ? 2 : size + 1;
			char *mem = (char *)LitMemory::alloc(size, sizeof(char));
			memcpy(mem, a, sizeof(char) * (size-1));
			mem[size-2] = n;
			LitMemory::set_size(mem, size);
			return mem;
		}
		a[size-1] = n;
		LitMemory::set_size(a, size + 1);
		return a;
	}
  char *str_substr(char *a, int bgn, int last) {
    char *c = (char *)LitMemory::alloc(last + 1, sizeof(char));
    return (char *)memmove(c, a + bgn, last);
  }
	char *str_concat_char_str(char a, char *b) {
		char *t = (char *)LitMemory::alloc(strlen(b) + 2, 1);
		t[0] = a;
		strcpy(&(t[1]), b);
		return t;
	}
	int *int_array_push_int(int *a, int n) {
		int size = LitMemory::get_size(a), realsize = LitMemory::get_real_size(a);
		if(size + 1 > realsize) {
			size = size + 1;
			int *mem = (int *)LitMemory::alloc(size, sizeof(int));
			memcpy(mem, a, sizeof(int) * (size-1));
			mem[size-1] = n;
			LitMemory::set_size(mem, size );
			return mem;
		}
		a[size] = n;
		LitMemory::set_size(a, size + 1);
		return a;
	}
	char *str_copy(char *a) {
		char *t = (char *)LitMemory::alloc(strlen(a) + 1, sizeof(char));
		if(!t) return nullptr;
		return strcpy(t, a);
	}
	char *get_string_stdin() {
		std::string input_from_stdin;
		std::getline(std::cin, input_from_stdin);
		char *ret = (char *)LitMemory::alloc(input_from_stdin.size() + 1, sizeof(char));
		return strcpy(ret, input_from_stdin.c_str());
	}
	int str_to_int(char *str) {
		return atoi(str);
	}
	double str_to_float(char *str) {
		return atof(str);
	}
	char *int_to_str(int n) {
		char buf[16]; sprintf(buf, "%d", n);
		return strcpy((char *)LitMemory::alloc(strlen(buf)+1, sizeof(char)), buf);
	}
	char *int64_to_str(long long int n) {
		char buf[32]; sprintf(buf, "%lld", n);
		return strcpy((char *)LitMemory::alloc(strlen(buf)+1, sizeof(char)), buf);
	}
	char *float_to_str(double f) {
		char buf[16], *ret;
		sprintf(buf, "%.10g", f);
		ret = (char *)LitMemory::alloc(strlen(buf)+1, sizeof(char));
		return strcpy(ret, buf);
	}
	int get_memory_length(void *ptr) {
		return LitMemory::get_size(ptr);
	}
	void append_addr_for_gc(void *addr) {
		LitMemory::append_ptr(addr);
	}
	void delete_addr_for_gc(void *addr) {
		LitMemory::delete_ptr(addr);
	}
	void run_gc() {
		LitMemory::gc();
	}
}

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
		if(!addr) { puts("LitSystemError: No enough memory"); return nullptr; }
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
