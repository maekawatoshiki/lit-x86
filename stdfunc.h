#include <iostream>
#include <sstream>
#include <functional>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
	void put_num(int n);
	void put_num64(long long int n);
	void put_num_float(double n);
	void put_char(char ch);
	void put_string(char *s);
	void put_array(int *ary);
	void put_array_float(double *ary);
	void put_array_str(char *ary[]);
	void put_ln();
	void *create_array(uint32_t size, uint32_t byte);
	char *str_concat(char *a, char *b);
	char *str_register_to_memmgr(char *a);
	char *str_concat_char(char *a, char n);
	char *str_concat_char_str(char a, char *b);
	int *int_array_push_int(int *a, int n);
	char *str_copy(char *a);
	char *get_string_stdin();
	int str_to_int(char *str);
	double str_to_float(char *str);
	char *int_to_str(int n);
	char *int64_to_str(long long int n);
	char *float_to_str(double f);
	int get_memory_length(void *ptr);
	void append_addr_for_gc(void *addr);
	void delete_addr_for_gc(void *addr);
	void run_gc();
}

namespace LitMemory {
	void *alloc(uint32_t, uint32_t);
	void *alloc_const(uint32_t);
	void gc();
	void gc_mark();
	void gc_sweep();
	void append_ptr(void *);
	void delete_ptr(void *);
	uint32_t get_size(void *);
	uint32_t get_real_size(void *);
	void set_size(void *, int);
	bool is_allocated_addr(void *);
	void free_all_mem();
};
