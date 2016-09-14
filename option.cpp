#include "option.h"
#include "lit.h"
#include "lex.h"
#include "expr.h"
#include "parse.h"
#include "var.h"
#include "util.h"

void Lit::run_option() {
  bool opt_version = false, 
      opt_help = false,
      opt_interpret = false,
      opt_eval = false,
      opt_optimize = false,
      opt_emit = false;

  if(argc < 2) {
    interpret();
  } else {
    std::vector<std::string> args(&argv[0], &argv[argc]);
    std::string file_name;

    for(auto arg : args) {
           if(arg == "-v")    opt_version   = true;
      else if(arg == "-h")    opt_help      = true;
      else if(arg == "-i")    opt_interpret = true;   
      else if(arg == "-e")    opt_eval      = true;
      else if(arg == "-emit") opt_emit      = true;
      else                    file_name     = arg;
    }

    if(opt_version) { 
      show_version();
    } else if(opt_help) {
      show_version();
      puts(
          "Usage: lit [options] <file>\n"
          " -v\t\tshow version\n"
          " -h\t\tshow this help\n"
          " -i\t\tinterpret mode(default)\n"
          " -e 'cmd'\tone line of script\n"
          " -emit\t\toutput Bitcode to a file 'mod.bc'"
          );
    } else if(opt_interpret) {
      interpret();
    } else if(opt_eval) {
      if(file_name == "") error("LitSystemError: no input file");
      else execute((char *)file_name.c_str(), opt_emit);
    } else run_from_file((char *)file_name.c_str(), opt_emit);
  }
}

void Lit::show_version() {
  puts("Lit " _LIT_VERSION_ ", Build( " __TIME__ " " __DATE__ " )");
}
