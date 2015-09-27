#include "lit.h"


int main(int argc, char **argv) {
	char *src;

	if(argc < 2) {
		src = calloc(sizeof(char), 0xFFFF);
		char line[0xFF] = "";

		puts("Lit 1.0.0 (C) 2015 maekawatoshiki");
		while(strcmp(line, "run\n") != 0) {
			printf(">> "); strcat(src, line);
			memset(line, 0, 0xFF);
			fgets(line, 0xFF, stdin);
		}
	} else {
		FILE *srcfp = fopen(argv[1], "rb");
		size_t ssz = 0;

		if(!srcfp) { perror("file not found"); exit(0); }
		fseek(srcfp, 0, SEEK_END);
		ssz = ftell(srcfp);
		fseek(srcfp, 0, SEEK_SET);
		src = calloc(sizeof(char), ssz + 2);
		fread(src, sizeof(char), ssz, srcfp);
		fclose(srcfp);
	}

	volatile clock_t bgn = clock();
		execute(src);
	volatile clock_t end = clock();
	printf("time: %lfsec\n", (double)(end - bgn) / CLOCKS_PER_SEC);

	return 0;
}

