#include <stdio.h>
#include <stdlib.h>

int memory() { 
	int *a;
	a = malloc(sizeof(int) * 100);
	a[300] = 0x11;
}

int main() {
	memory();

	return 0;
}
