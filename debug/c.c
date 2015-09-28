#include <stdio.h>

int main() {
	FILE *fp = fopen("hello", "w");
	fprintf(fp, "Hello %s\n", "world!!!");
	fclose(fp);
}
