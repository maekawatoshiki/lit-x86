#include "option.h"

int show_option(int argc, char **argv) {
	Lit lit;
	bool opt_version = false, 
			opt_help = false,
			opt_interpret = false,
			opt_eval = false;

	if(argc < 2) {
		lit.interpret();
	} else {
		std::string argv_bgn = argv[1];
		std::vector<std::string> args(&argv[0], &argv[argc]);

		for(int i = 0; i < argc; i++) {
			if(args[i] == "-v") opt_version = true;
			else if(args[i] == "-h") opt_help = true;
			else if(args[i] == "-i") opt_interpret = true;
			else if(args[i] == "-e") opt_eval = true;
		}

		if(opt_version) show_version();
		else if(opt_help) {
			show_version();
			puts(
					"Usage: lit [options] <file>\n\n"
					" -v		show version\n"
					" -h		show this help\n"
					" -i		interpret mode\n"
					" -e 'command'	one line of script"
					);
		} else if(opt_interpret) lit.interpret();
		else if(opt_eval) lit.execute((char *)args[2].c_str());
		else lit.run_from_file((char *)args[1].c_str());
	}
	return 0;
}

void show_version() {
	puts("Lit " _LIT_VERSION_ " (C) 2015 maekawatoshiki");
	puts("Build( " __TIME__ " " __DATE__ " )");
}
