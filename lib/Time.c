#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

struct tm *Time_time() {
	time_t t = time(NULL);
	return localtime(&t);
}

void Time_sleep(double sec) {
	usleep(1000000 * sec);
}
