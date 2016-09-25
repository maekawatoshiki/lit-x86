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
        case 'a': str[i]='\a'; memmove(&str[i-1], &str[i], l); i--; break;
        case 'n': str[i]='\n'; memmove(&str[i-1], &str[i], l); i--; break;
        case 't': str[i]='\t'; memmove(&str[i-1], &str[i], l); i--; break;
        case 'f': str[i]='\f'; memmove(&str[i-1], &str[i], l); i--; break;
        case 'r': str[i]='\r'; memmove(&str[i-1], &str[i], l); i--; break;
        case 'b': str[i]='\b'; memmove(&str[i-1], &str[i], l); i--; break;
				case 'x': {
										char hex[3]={str[i+1], str[i+2]}, *e=nullptr;
										str[i+2]=strtol(hex, &e, 16);
										memmove(&str[i-1], &str[i+2], l - 2 + 1);
										i -= 2;
									} break;
        case '\\': str[i]='\\'; memmove(&str[i-1], &str[i], l); i--; break;
        case '\'': str[i]='\''; memmove(&str[i-1], &str[i], l); i--; break;
        case '\"': str[i]='\"'; memmove(&str[i-1], &str[i], l); i--; break;
      }
    }
  }
  return str;
}
