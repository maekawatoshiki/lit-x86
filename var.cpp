#include "var.h"
#include "parse.h"
#include "token.h"
#include "util.h"
#include "lit.h"
#include "func.h"

var_t *Variable::get(std::string name) {
  // local var
  for(std::vector<var_t>::iterator it = local.begin(); it != local.end(); ++it) {
    if(name == it->name) {
      return &(*it);
    }
  }

  return NULL;
}

var_t * Variable::append(std::string name, ExprType *type, bool is_global, std::string c_name) {
  uint32_t sz = local.size();
  var_t v = {
    .name = name,
    .class_type = c_name,
    .id = sz + 1, 
    .is_global = is_global,
  };
  v.type.change(type);
  local.push_back(v);
  return &local.back();
}

int Struct::get_size(std::string name) {
  return get(name)->members.size() * sizeof(void*);
}

struct_t *Struct::get(std::string strct) {
  for(auto it = structs.begin(); it != structs.end(); ++it) {
    if(strct == it->name) {
      return &(*it);
    }
  }
  return NULL;
}

struct_t *Struct::append(std::string strct, std::vector<var_t> members, llvm::StructType *ty_strct) {
  struct_t z = {
    .name = strct,
    .members = members,
    .strct = ty_strct
  };
  structs.push_back(z);
  return &structs.back();
}
