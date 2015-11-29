#ifndef _PARSE_LIT_
#define _PARSE_LIT_

#include "lit.h"
#include "asm.h"
#include "lex.h"
#include "expr.h"
#include "var.h"

typedef struct {
	int address, params;
	char name[0xFF], mod_name[0xff];
} func_t;

struct {
	func_t func[0xff];
	int count;
} undef_funcs, funcs;

// The strings embedded in native code
struct {
	char *text[0xff];
	int *addr;
	int count;
} strings;

void skip_tok();

int streql(char *, char *);

int make_if();
int make_while();
int make_func();
int make_break();
int make_return();

int eval(int, int);
int expression(int, int);

int parser();
int get_string();

func_t *get_func(char *, char *);
func_t *append_func(char *, int, int);

int append_undef_func(char *, char *, int);
int is_undef_func(char *, int);

char *replaceEscape(char *);

#endif
