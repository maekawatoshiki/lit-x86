#include <stdio.h>
#include <stdlib.h>

FILE *File_new() {
	return malloc(sizeof(FILE *));
}

FILE *File_open(int *fp, char *name, char *mode) {
	*fp = (int)fopen(name, mode);
}

void File_write(int *fp, char *str) {
	fprintf((FILE*)*fp, "%s", str);
}

char *File_read(int *fp) {
	fseek((FILE*)*fp, 0, SEEK_END);
	size_t sz = ftell((FILE*)*fp); fseek((FILE*)*fp, 0, SEEK_SET);
	char *str = malloc(sz + 1);
	fread(str, sz, 1, (FILE*)*fp);
	return str;
}

void File_close(int *fp) {
	fclose((FILE*)*fp);
}
