#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

unsigned int Time_time() { return time(NULL); }

char *Time_strtime() { 
	time_t t; char *time_s;
	time(&t); time_s = ctime(&t); 
	time_s[strlen(time_s) - 1] = '\0';
	return time_s;
}

void Time_sleep(double sec) {
	usleep(1000000 * sec);
}
