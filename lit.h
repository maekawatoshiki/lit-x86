#ifndef _LIT_MAIN_HEAD_
#define _LIT_MAIN_HEAD_

// C
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <dlfcn.h>

// C++ 
#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>

#include "asm.h"
#include "lex.h"
#include "expr.h"
#include "parse.h"
#include "token.h"
#include "stdfunc.h"
#include "util.h"
#include "option.h"
#include "library.h"

#if defined(WIN32) || defined(WINDOWS)
	#include <windows.h>
#else
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/mman.h>
	#include <sys/wait.h>
#endif

#define _LIT_VERSION_ "0.9.7"

enum {
	ADDR_SIZE = 4
};

enum {
	BLOCK_NORMAL = 0,
	BLOCK_LOOP,
	BLOCK_FUNC,
	BLOCK_GLOBAL,
	NON
};

class ctrl_t {
public:
	uint32_t *addr_list;
	int count;
};

extern ctrl_t break_list, return_list;

class Lit {
public:
	Lit();
	~Lit();

	int execute(char *); // execute(<source code>)
	int run();
	
	void interpret();
	void run_from_file(char *);
};

/* for native(JIT) code. */

struct mem_info {
	uint32_t addr;
	bool isfree;
};

class MemoryList {
public:
	std::vector<mem_info> mem;

	size_t count() { return mem.size(); }
};

extern MemoryList mem;

void freeAddr();
void freeInProgram(uint32_t);
void putNumber(int);
void putString(int *);
void putln();
void appendAddr(uint32_t);
#endif
