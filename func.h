#ifndef _FUNC_LIT_
#define _FUNC_LIT_

#include "var.h"
#include "exprtype.h"
#include "common.h"

struct func_t {
  ExprType type;
  size_t params;
  std::vector<ExprType *> args_type;
  std::vector<std::string> args_name;
  std::string name;
  std::vector<std::string> mod_name;
  llvm::Function *func_addr;
  bool is_template;
};

class Function {
public:
  func_t info;
  Variable var;
  llvm::BasicBlock *bb_return;
  std::stack<llvm::BasicBlock *> break_br_list;
  std::stack<bool> has_br;
};

class Program {
public:
  std::string name;
  std::vector<Function> func, undef_func;
  std::vector<std::string> cur_mod;
  Variable var_global; 
  Struct structs;
  std::string &module;
  
  Program(std::string &mod): module(mod) { func.reserve(1280); }
  bool is(std::string, std::string);
  Function *lookup(std::string, std::vector<ExprType *>);
  Function *lookup(std::string, std::vector<std::string>, std::vector<ExprType*>);
  Function *add(Function);
  
  var_t *append_global_var(std::string, int);
  void append_addr_of_global_var(std::string, int);
  void insert_global_var();
};


#endif // _FUNC_LIT_
