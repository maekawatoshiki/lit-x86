#include "option.h"

int show_option(int argc, char **argv) {
	if(argc < 2) {
		lit_interpret();
	} else if(streql(argv[1], "-v")) {
		show_version();
	} else if(streql(argv[1], "-h")) {
		show_version();
		puts(
				"Usage: lit [options] <file>\n\n"
				"	-v	show version\n"
				"	-h	show this help\n"
				);
	} else {
		lit_run(argv[1]);
	}

	return 0;
}

void show_version() {
	puts("Lit " _LIT_VERSION_ " (C) 2015 maekawatoshiki");
	puts("Build( " __TIME__ " " __DATE__ " )");
}
