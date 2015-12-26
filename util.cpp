#include "util.h"

int streql(char *s1, char *s2) {
	return (strcmp(s1, s2) == 0) ? 1 : 0;
}

bool skip(std::string s) {
  if(s == tok.tok[tok.pos].val) {
  	tok.pos++; return true;
  } else return false;
}

int error(char *errs, ...) {
	va_list args;
	va_start(args, errs);
		vprintf(errs, args); puts("");
	va_end(args);
	exit(0);
	return 0;
}
