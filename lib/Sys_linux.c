#include <stdio.h>
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
