#ifndef _PARSE_LIT_
#define _PARSE_LIT_

#include "lit.h"
#include "asm.h"
#include "lex.h"
#include "expr.h"
#include "var.h"
#include "util.h"

typedef struct {
	uint32_t address, params;
	std::string name, mod_name;
} func_t;

typedef struct {
	func_t func[0xff];
	int count, now, inside;
} funclist_t;

extern funclist_t undef_funcs, funcs;

typedef struct {
	std::string name;
	void *handle;
	int no;
} lib_t;

typedef struct {
	lib_t lib[64];
	int count;
} liblist_t;

extern liblist_t lib_list;

// The strings embedded in native code
typedef struct {
	char *text[0xff];
	int *addr;
	int count;
} string_t;

extern string_t strings;
extern std::string module;

void using_require();
int append_lib(std::string);
int is_lib_module(std::string);
lib_t *get_lib_module(std::string);
void free_lib();

int make_if();
int make_while();
int make_func();
int make_break();
int make_return();

int eval(int, int);
int expression(int, int);

int parser();
int get_string();

int is_func(std::string, std::string);
func_t *get_func(std::string, std::string);
func_t *append_func(std::string, int, int);

int append_undef_func(std::string, std::string, int);
int rep_undef_func(std::string, int);

void replaceEscape(char *);

#endif
