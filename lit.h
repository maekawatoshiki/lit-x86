#ifndef _LIT_MAIN_HEAD_
#define _LIT_MAIN_HEAD_

#include "lex.h"
#include "common.h"
#include "parse.h"
#include "token.h"

#if defined(WIN32) || defined(WINDOWS)
	#include <windows.h>
#else
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/mman.h>
	#include <sys/wait.h>
#endif

#define _LIT_VERSION_ "0.1.1"


class Lit {
	Token tok;
	Lexer lex;
	Parser parser;
	int argc;
	char **argv;

	int execute(char *); // execute(<source code>)
	int run();
	
	void interpret();
	void run_from_file(char *);

	void show_option();
	void show_version();

public:

	Lit(int, char**);
	~Lit();

	int start();
};

/* for native(JIT) code. */
namespace LitMemory {
	void *alloc(uint32_t, uint32_t);
	void *alloc_const(uint32_t);
	void gc();
	void gc_mark();
	void gc_sweep();
	void append_ptr(void *);
	void delete_ptr(void *);
	uint32_t get_size(void *);
	bool is_allocated_addr(void *);
};

void putNumber(int);
void putString(int *);
void putln();
#endif
