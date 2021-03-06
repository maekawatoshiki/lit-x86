#include "func.h"
#include "parse.h"
#include "lit.h"
#include "exprtype.h"
#include "stdfunc.h"
#include "util.h"

Function *Program::lookup(std::string name, std::vector<ExprType *> args_type) {
	return lookup(name, std::vector<std::string>(), args_type);
}

Function *Program::lookup(std::string name, std::vector<std::string> mod_name, std::vector<ExprType *> args_type) {
  auto is_eql_args_type = [&](Function f, bool temp=false) -> bool {
    if(f.info.args_type.size() == 0 && args_type.size() == 0) return true;
    if(f.info.args_type.size() != args_type.size()) return false;
    if(temp && f.info.is_template) return true;
    auto caller_it = args_type.begin();
    for(auto it = f.info.args_type.begin(); it != f.info.args_type.end() && caller_it != args_type.end(); ++it) {
      if(!(*it)->eql_type((*caller_it))) return false;
      caller_it++;
    }
    return true;
  };
  for(std::vector<Function>::iterator it = func.begin(); it != func.end(); it++) 
    if(it->info.name == name && is_eql_args_type(*it) && it->info.mod_name == mod_name)
      return &(*it);
  for(std::vector<Function>::iterator it = func.begin(); it != func.end(); it++) // search the template function
    if(it->info.name == name && is_eql_args_type(*it, true) && it->info.mod_name == mod_name) 
      return &(*it);
  return NULL;
}

Function *Program::add(Function f) {
  func.push_back(f);
  return &func.back();
}

var_t *Program::append_global_var(std::string name, int type) {
  var_t *v = var_global.append(name, new ExprType(type));
  v->is_global = true;
  return v;
}

