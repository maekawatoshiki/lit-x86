#include "util.h"

int streql(char *s1, char *s2) {
	return (strcmp(s1, s2) == 0) ? 1 : 0;
}

int skip(char *s) {
  if(streql(s, tok.tok[tok.pos].val)) {
  	tok.pos++; return 1;
  } else return 0;
}

int error(char *errs, ...) {
	va_list args;
	va_start(args, errs);
		vprintf(errs, args); puts("");
	va_end(args);
	exit(0);
	return 0;
}
