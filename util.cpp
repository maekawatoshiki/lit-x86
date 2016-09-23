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
  int l = strlen(str);
  for(int i = 0; i < l; i++) {
    if(str[i] == '\\') {
      i++; switch(str[i]) {
        case 'a': str[i]='\a'; break;
        case 'n': str[i]='\n'; break;
        case 't': str[i]='\t'; break;
        case 'f': str[i]='\f'; break;
        case 'r': str[i]='\r'; break;
        case 'b': str[i]='\b'; break;
        case '\\':str[i]='\\'; break;
        case '\'':str[i]='\''; break;
        case '\"':str[i]='\"'; break;
      }
      memmove(&str[i-1], &str[i], l - 2 + 1);
    }
  }
  return str;
}
