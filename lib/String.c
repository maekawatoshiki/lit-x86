#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

char *str_replace(char *str, char *s1, char *s2) {
	char *p = str;
	size_t s1_len = strlen(s1), s2_len = strlen(s2);

	while((p = strstr(p, s1)) != NULL) {
		if(s1_len < s2_len) 
			memmove(p + s2_len, p + s1_len, strlen(p));
		else if(s1_len > s2_len)
			memmove(p, p + s1_len, strlen(p));
		memcpy(p, s2, s2_len);
	}
	return str;
}

char *str_find(char *str, char *s) { return strstr(str, s); }
int str_eql(char *s1, char *s2) { return strcmp(s1, s2) == 0 ? 1 : 0; }
int str_len(char *str) { return strlen(str); }
char *str_concat(char *str, char *s) { return strcat(str, s); }
char *str_copy(char *str, char *s) { return strcpy(str, s); }
int str_isdigit(int ch) { return isdigit(ch) ? 1 : 0; }
int str_isalpha(int ch) { return isalpha(ch) ? 1 : 0; }
int str_to_i(char *s) { return atoi(s); }
