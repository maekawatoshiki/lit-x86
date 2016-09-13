#ifndef _VAR_LIT_
#define _VAR_LIT_

#include <string>
#include <vector>
#include "common.h"
#include "exprtype.h"

typedef struct {
  std::string name, mod_name;
  unsigned int id;
  ExprType type;
  std::string class_type;
  bool is_global;
  std::vector<int> used_location; // for global variables
  llvm::Value *val; // for codegen
} var_t;

struct struct_t {
  std::string name;
  std::vector<var_t> members;
  llvm::StructType *strct;
  int get_member_num(std::string name) {
    for(int i = 0; i < members.size(); i++) 
      if(members[i].name == name) return i;
    return -1;
  };
};

enum {
  V_LOCAL,
  V_GLOBAL
};

class Variable {
public:
  std::vector<var_t> local;

  var_t *get(std::string);
  var_t *append(std::string, ExprType *,  bool = false, std::string = "");
};

class Struct {
public:
  std::vector<struct_t> structs;

  int get_size(std::string);
  struct_t *get(std::string strct);
  struct_t *append(std::string strct, std::vector<var_t> members, llvm::StructType *);
};

#endif // _LIT_VAR_

