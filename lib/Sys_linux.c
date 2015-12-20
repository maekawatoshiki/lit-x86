#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void Sys_system(char *cmd) {
	system(cmd);
}

void Sys_exit() {
	exit(0);
}

void Sys_write(char *s, size_t size, size_t count, FILE *fp) {
	fwrite(s, size, count, fp);
}

void Sys_read(char *s, size_t size, size_t count, FILE *fp) {
	fread(s, size, count, fp);
}
