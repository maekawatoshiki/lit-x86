#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <math.h>

unsigned int Sys_mem_usage() {
	struct sysinfo info;
	sysinfo(&info);

	unsigned long total = (info.totalram * info.mem_unit) / 1024,
		free = (info.freeram * info.mem_unit ) / 1024;
	
	return (unsigned int)floor((double)free / total * 100.0);
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
