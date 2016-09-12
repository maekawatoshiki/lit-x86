#ifndef _LIT_MAIN_HEAD_
#define _LIT_MAIN_HEAD_

#include "lex.h"
#include "common.h"
#include "parse.h"
#include "token.h"

#if defined(WIN32) || defined(WINDOWS)
  #include <windows.h>
#else
  #include <unistd.h>
  #include <sys/types.h>
  #include <sys/mman.h>
  #include <sys/wait.h>
#endif

#define _LIT_VERSION_ "0.2.0"


class Lit {
  Token tok;
  Lexer lex;
  Parser parser;
  int argc;
  char **argv;

  int execute(char *, bool = false); // execute(<source code>, flag<emit-llvm>)
  
  void interpret();
  void run_from_file(char *, bool = false);

  void run_option();
  void show_version();

public:

  Lit(int, char**);
  ~Lit();

  int start();
};

#endif
