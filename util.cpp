#include "util.h"
#include "common.h"

int streql(char *s1, char *s2) {
	return (strcmp(s1, s2) == 0) ? 1 : 0;
}

int error(const char *errs, ...) {
	va_list args;
	va_start(args, errs);
		vprintf(errs, args); puts("");
	va_end(args);
	exit(0);
	return 0;
}
