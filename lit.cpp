#include "lit.h"
#include "lex.h"
#include "expr.h"
#include "parse.h"
#include "token.h"
#include "util.h"
#include "codegen.h"
#include "option.h"
#include "stdfunc.h"

// ---- for native code --- //

char *File_read(char *s, int len, FILE *fp) { fread(s, 1, len, fp); return s; }


// Lit class is from here

Lit::Lit(int ac, char **av)
  :lex(tok), parser(tok), argc(ac), argv(av) {
  tok.pos = 0; tok.size = 0xfff;
}

Lit::~Lit() {
  LitMemory::free_all_mem();
}

int Lit::execute(char *source, bool enable_emit_llvm) {
  lex.lex(source);
  llvm::Module *program = parser.parser();
  Codegen::run(program, false/*optimize*/, enable_emit_llvm);
  // if((fork()) == 0) run();
  // int status = 0;
  // wait(&status);
  // if(!WIFEXITED(status)) {
  //   puts("LitRuntimeError: *** the process was terminated abnormally ***");
  // }
  return 0;
}

void Lit::interpret() {
  std::string line, all;

  while(std::getline(std::cin, line)) {
    all += line + " ; ";
    line.clear();
  }

  clock_t bgn = clock();
    execute((char *)all.c_str(), false);
  clock_t end = clock();
#ifdef DEBUG
  printf("time: %.3lf\n", (double)(end - bgn) / CLOCKS_PER_SEC);
#endif
}

void Lit::run_from_file(char *source, bool enable_emit_llvm) {
  std::ifstream ifs_src(source);
  if(!ifs_src) ::error("LitSystemError: cannot open file '%s'", source);
  std::istreambuf_iterator<char> it(ifs_src);
  std::istreambuf_iterator<char> last;
  std::string src_all(it, last);
  
  execute((char *)src_all.c_str(), enable_emit_llvm);
}

int Lit::start() {
  show_option();
  return 0;
}
