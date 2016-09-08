#include "util.h"
#include "common.h"


int error(const char *errs, ...) {
  va_list args;
  va_start(args, errs);
    vprintf(errs, args); puts("");
  va_end(args);
  exit(0);
  return 0;
}

char *replace_escape(char *str) {
  int i;
  char *pos;
  char escape[14][3] = {
    "\\a", "\a",
    "\\r", "\r",
    "\\f", "\f",
    "\\n", "\n",
    "\\t", "\t",
    "\\b", "\b",
    "\\\"",  "\""
  };
  for(i = 0; i < 14; i += 2) {
    while((pos = strstr(str, escape[i])) != NULL) {
      *pos = escape[i + 1][0];
      memmove(pos + 1, pos + 2, strlen(str) - 2 + 1);
    }
  }
  return str;
}
