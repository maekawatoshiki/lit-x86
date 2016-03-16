#include "codegen.h"
#include "asm.h"
#include "expr.h"
#include "exprtype.h"
#include "util.h"
#include "parse.h"

int codegen_entry(ast_vector &program) {
	uint32_t main_address;
	std::string module = "";
	ntv.gencode(0xe9); main_address = ntv.count; ntv.gencode_int32(0);
	Module list(module);
	for(ast_vector::iterator it = program.begin(); it != program.end(); ++it) {
		if((*it)->get_type() == AST_FUNCTION) {
			Function f = ((FunctionAST *)*it)->codegen(list);
		} else if((*it)->get_type() == AST_VARIABLE_ASGMT) { // global variable assignment
		}
	}
	list.insert_global_var();
	Function *main = list.get("main");
		if(main == NULL) error("error: not found function: 'main'");
	ntv.gencode_int32_insert(main->info.address - ADDR_SIZE - 1, main_address);
	
	return 0;
}

int codegen_expression(Function &f, Module &f_list, AST *ast) {
	switch(ast->get_type()) {
	case AST_NUMBER:
		((NumberAST *)ast)->codegen(f, ntv);
		return T_INT;
	case AST_STRING:
		((StringAST *)ast)->codegen(f, ntv);
		return T_STRING;
	case AST_VARIABLE:
		((VariableAST *)ast)->codegen(f, f_list, ntv);
		return ((VariableAST *)ast)->get(f, f_list)->type;
	case AST_VARIABLE_ASGMT:
		((VariableAsgmtAST *)ast)->codegen(f, f_list, ntv);
		return T_VOID;
	case AST_FUNCTION_CALL:
		((FunctionCallAST *)ast)->codegen(f, f_list, ntv);
		return T_INT;
	case AST_BINARY:
		return ((BinaryAST *)ast)->codegen(f, f_list, ntv);
	case AST_ARRAY:
		return ((ArrayAST *)ast)->codegen(f, f_list, ntv);
	case AST_VARIABLE_INDEX:
		return ((VariableIndexAST *)ast)->codegen(f, f_list, ntv);
	case AST_IF:
		((IfAST *)ast)->codegen(f, f_list, ntv);
		return T_VOID;
	case AST_WHILE:
		((WhileAST *)ast)->codegen(f, f_list, ntv);
		return T_VOID;
	case AST_FOR:
		((ForAST *)ast)->codegen(f, f_list, ntv);
		return T_VOID;
	case AST_BREAK:
		((BreakAST *)ast)->codegen(f, f_list, ntv);
		return T_VOID;
	case AST_RETURN:
		((ReturnAST *)ast)->codegen(f, f_list, ntv);
		return T_VOID;
	}
	return T_VOID;
}

Function FunctionAST::codegen(Module &f_list) {
	Function f;
	f.info.name = info.name;
	f.info.mod_name = "";
	f.info.address = ntv.count;
	f.info.params = args.size();
	uint32_t func_bgn = ntv.count;

	ntv.genas("push ebp");
	ntv.genas("mov ebp esp");
	uint32_t esp_ = ntv.count + 2; ntv.genas("sub esp 0");
	if(info.name == "main") {
		ntv.gencode(0x8b); ntv.gencode(0x75); ntv.gencode(0x0c); // mov esi, 0xc(%ebp)
	}

	// append argument  variables to function
	for(ast_vector::iterator it = args.begin(); it != args.end(); ++it) {
		if((*it)->get_type() == AST_VARIABLE)
			((VariableAST *)*it)->append(f, f_list);
		else if((*it)->get_type() == AST_VARIABLE_DECL)
			((VariableDeclAST *)*it)->append(f, f_list);
	}

	f_list.append(f);
	f_list.rep_undef(f.info.name, func_bgn);

	for(ast_vector::iterator it = statement.begin(); it != statement.end(); ++it) {
		codegen_expression(f, f_list, *it);
	}

	// generate return code
	for(std::vector<int>::iterator it = f.return_list.begin(); it != f.return_list.end(); ++it) {
		ntv.gencode_int32_insert(ntv.count - *it - ADDR_SIZE, *it);
	}
	ntv.genas("add esp %u", f.var.total_size() + 6 * ADDR_SIZE); // add esp nn
	ntv.gencode(0xc9);// leave
	ntv.gencode(0xc3);// ret

	ntv.gencode_int32_insert(f.var.total_size() + ADDR_SIZE * 6, esp_);
	return f;
}

void IfAST::codegen(Function &f, Module &f_list, NativeCode_x86 &ntv) {
	codegen_expression(f, f_list, cond);
	ntv.gencode(0x83); ntv.gencode(0xf8); ntv.gencode(0x00);// cmp eax, 0
	ntv.gencode(0x75); ntv.gencode(0x05); // jne 5
	ntv.gencode(0xe9); int end = ntv.count; ntv.gencode_int32(0);// jmp
	for(ast_vector::iterator it = then_block.begin(); it != then_block.end(); ++it)
		codegen_expression(f, f_list, *it);
	ntv.gencode(0xe9); int end1 = ntv.count; ntv.gencode_int32(0);// jmp end
	ntv.gencode_int32_insert(ntv.count - end - 4, end);

	for(ast_vector::iterator it = else_block.begin(); it != else_block.end(); ++it)
		codegen_expression(f, f_list, *it);
	ntv.gencode_int32_insert(ntv.count - end1 - 4, end1);
}

void WhileAST::codegen(Function &f, Module &f_list, NativeCode_x86 &ntv) {
	ntv.gencode(0x90); int loop_bgn = ntv.count;
		std::vector<int> *break_list = new std::vector<int>;
		f.break_list.push(break_list);
	codegen_expression(f, f_list, cond);
	ntv.gencode(0x83); ntv.gencode(0xf8); ntv.gencode(0x00);// cmp eax, 0
	ntv.gencode(0x75); ntv.gencode(0x05); // jne 5
	ntv.gencode(0xe9); int end = ntv.count; ntv.gencode_int32(0);// jmp while end

	for(ast_vector::iterator it = block.begin(); it != block.end(); ++it) {
		codegen_expression(f, f_list, *it);
	}

	ntv.gencode(0xe9); ntv.gencode_int32(0xFFFFFFFF - ntv.count + loop_bgn - ADDR_SIZE); // jmp n
	ntv.gencode_int32_insert(ntv.count - end - ADDR_SIZE, end);

	for(std::vector<int>::iterator it = f.break_list.top()->begin(); it != f.break_list.top()->end(); ++it) {
		ntv.gencode_int32_insert(ntv.count - *it - ADDR_SIZE, *it);
	}
	f.break_list.pop();
}

void ForAST::codegen(Function &f, Module &f_list, NativeCode_x86 &ntv) {
	codegen_expression(f, f_list, asgmt);
	ntv.gencode(0x90); int loop_bgn = ntv.count;
		std::vector<int> *break_list = new std::vector<int>;
		f.break_list.push(break_list);
	codegen_expression(f, f_list, cond);
	ntv.gencode(0x83); ntv.gencode(0xf8); ntv.gencode(0x00);// cmp eax, 0
	ntv.gencode(0x75); ntv.gencode(0x05); // jne 5
	ntv.gencode(0xe9); int end = ntv.count; ntv.gencode_int32(0x00000000);// jmp while end

	for(ast_vector::iterator it = block.begin(); it != block.end(); ++it) {
		codegen_expression(f, f_list, *it);
	}
	codegen_expression(f, f_list, step);

	ntv.gencode(0xe9); ntv.gencode_int32(0xFFFFFFFF - ntv.count + loop_bgn - ADDR_SIZE); // jmp n
	ntv.gencode_int32_insert(ntv.count - end - ADDR_SIZE, end);
	ntv.gencode(0x90); ntv.gencode(0x90);
	for(std::vector<int>::iterator it = f.break_list.top()->begin(); it != f.break_list.top()->end(); ++it) {
		ntv.gencode_int32_insert(ntv.count - *it - ADDR_SIZE, *it);
	}
}

void FunctionCallAST::codegen(Function &f, Module &f_list, NativeCode_x86 &ntv) {
	struct {
		std::string name, mod_name;
		int args, addr; // if args is -1, the function has vector args.
	} stdfunc[] = {
		{"Array", "", 1, 12},
		{"printf", "", -1, 16},
		{"sleep", "Time", 1, 24},
		{"open", "File", 2, 28},
		{"write", "File", -1, 32},
		{"read", "File", 3, 40},
		{"close", "File", 1, 36},
		{"gets", "File", 3, 52},
		{"free", "Sys", 1, 44},
		{"puts", "", -1, -1} // special
	};
	bool is_std_func = false;

	for(int i = 0; i < sizeof(stdfunc) / sizeof(stdfunc[0]); i++) {
		if(stdfunc[i].name == info.name /* && stdfunc[i].mod_name == mod_name */) {
			if(info.name == "Array") {
				codegen_expression(f, f_list, args[0]);
				ntv.genas("shl eax 2");
				ntv.gencode(0x89); ntv.gencode(0x04); ntv.gencode(0x24); // mov [esp], eax
				ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(12); // call malloc
			} else if(info.name == "puts") {
				for(int i = 0; i < args.size(); i++) {
					int ty = codegen_expression(f, f_list, args[i]);
					ntv.gencode(0x89); ntv.gencode(0x04); ntv.gencode(0x24); // mov [esp], eax
					if(ty == T_STRING) {
						ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(4);// call *4(esi) putString
					} else if(ty == T_CHAR) {
						ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(60);// call *60(esi) putc
					} else {
						ntv.gencode(0xff); ntv.gencode(0x16); // call (esi) putNumber
					}
				}
				ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(8);// call *0x08(esi) putLN
			} else { // user function
				if(stdfunc[i].args == -1) { // vector
					uint32_t a = 0;
					for(ast_vector::iterator it = args.begin(); it != args.end(); ++it) {
						codegen_expression(f, f_list, *it);
						ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(ADDR_SIZE * a++); // mov [esp+a*ADDR_SIZE], eax
					}
				} else { // normal function
					for(int arg = 0; arg < stdfunc[i].args; arg++) {
						codegen_expression(f, f_list, args[arg]);
						ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(arg * ADDR_SIZE); // mov [esp+arg*ADDR_SIZE], eax
					}
				}
				ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(stdfunc[i].addr); // call $function
			}
			is_std_func = true;
		}
	}
	if(!is_std_func) { // user's Function
		Function *function = f_list.get(info.name, info.mod_name);
		if(function == NULL) { // undefined
			uint32_t a = 3;
			for(ast_vector::iterator it = args.begin(); it != args.end(); ++it) {
				codegen_expression(f, f_list, *it);
				ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(256 - a++ * ADDR_SIZE); // mov [esp+ADDR*a], eax
			}
			ntv.gencode(0xe8); f_list.append_undef(info.name, info.mod_name, ntv.count);
				ntv.gencode_int32(0x00000000); // call Function
		} else { // defined
			uint32_t a = 3;
			if(args.size() != function->info.params) error("error: the number of arguments is not same");
			for(ast_vector::iterator it = args.begin(); it != args.end(); ++it) {
				codegen_expression(f, f_list, *it);
				ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(256 - a++ * ADDR_SIZE); // mov [esp+ADDR*a], eax
			}
			ntv.gencode(0xe8); ntv.gencode_int32(0xFFFFFFFF - (ntv.count - function->info.address) - 3); // call Function
		}
	}
}

int BinaryAST::codegen(Function &f, Module &f_list, NativeCode_x86 &ntv) {
	int ty1 = codegen_expression(f, f_list, left);
	ntv.genas("push eax");
	int ty2 = codegen_expression(f, f_list, right);
	ntv.genas("mov ebx eax");
	ntv.genas("pop eax");
	if(ty1 != ty2)
		if(op != "+") error("error: type error"); // except string concat
	if(op == "+") {
		if(ty1 == T_STRING) {
			ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(ADDR_SIZE * 0); // mov [esp+0*ADDR_SIZE], eax
			ntv.gencode(0x89); ntv.gencode(0x5c); ntv.gencode(0x24); ntv.gencode(ADDR_SIZE * 1); // mov [esp+1*ADDR_SIZE], ebx
			ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(56); // call rea_concat
		} else
			ntv.genas("add eax ebx");
	} else if(op == "-") ntv.genas("sub eax ebx");
	else if(op == "*") ntv.genas("mul ebx");
	else if(op == "/") {
		ntv.genas("mov edx 0");
		ntv.genas("div ebx");
	} else if(op == "%") {
		ntv.genas("mov edx 0");
		ntv.genas("div ebx");
		ntv.genas("mov eax edx");
	} else if(op == "<" || op == ">" || op == "!=" ||
			op == "==" || op == "<=" || op == ">=") {
		bool lt = op == "<", gt = op == ">", ne = op == "!=", eql = op == "==", fle = op == "<=";
		ntv.gencode(0x39); ntv.gencode(0xd8); // cmp %eax, %ebx
		ntv.gencode(0x0f); ntv.gencode(lt ? 0x9c : gt ? 0x9f : ne ? 0x95 : eql ? 0x94 : fle ? 0x9e : 0x9d); ntv.gencode(0xc0); // setX al
		ntv.gencode(0x0f); ntv.gencode(0xb6); ntv.gencode(0xc0); // movzx eax al
	} else if(op == "and" || op == "&" || op == "or" ||
			op == "|" || op == "xor" || op == "^") {
		bool andop = op == "and", orop = op == "or";
		ntv.gencode(andop ? 0x21 : orop ? 0x09 : 0x31); ntv.gencode(0xd8); // and eax ebx
	}
	return ty1;
}

void VariableAsgmtAST::codegen(Function &f, Module &f_list, NativeCode_x86 &ntv) {
	var_t *v;
	bool first_decl = false;

	if(var->get_type() == AST_VARIABLE) {
		if((v = ((VariableAST *)var)->get(f, f_list)) == NULL) {
			v = ((VariableAST *)var)->append(f, f_list);
			first_decl = true;
		}
	} else if(var->get_type() == AST_VARIABLE_DECL) {
		if((v = ((VariableDeclAST *)var)->get(f, f_list)) == NULL) {
			v = ((VariableDeclAST *)var)->append(f, f_list);
			first_decl = true;
		}
	} else if(var->get_type() == AST_VARIABLE_INDEX) {
		VariableIndexAST *via = (VariableIndexAST *)var;
			if(via->var->get_type() != AST_VARIABLE) error("error: variable");
		// while(via->var->get_type() == AST_VARIABLE_INDEX) {
		// 	via = ((VariableIndexAST *)via->var);
		// }
		v = ((VariableAST *)via->var)->get(f, f_list);
			if(v == NULL) error("error: the var was not declared");
		codegen_expression(f, f_list, via->idx);
			ntv.genas("push eax");
			ntv.gencode(0x8b); ntv.gencode(0x4d);
				ntv.gencode(256 - ADDR_SIZE * v->id); // mov ecx [ebp-n]
		
		codegen_expression(f, f_list, src);
		ntv.genas("pop edx");
		if(IS_ARRAY(v->type)) {
			ntv.gencode(0x89); ntv.gencode(0x04); ntv.gencode(0x91); // mov [ecx+edx*4], eax
		} else if(IS_TYPE(v->type, T_STRING)) {
			ntv.gencode(0x89); ntv.gencode(0x04); ntv.gencode(0x11); // mov [ecx+edx], eax
		}
	}
	if(var->get_type() != AST_VARIABLE_INDEX) {
		int ty = codegen_expression(f, f_list, src);
		if(op != "=") { 
			ntv.genas("push eax");
			codegen_expression(f, f_list, var);
			ntv.genas("pop ebx");
			ntv.genas("%s eax ebx", op == "+=" ? "add" : op == "-=" ? "sub" : "");
		}
		if(v->is_global) {
			ntv.gencode(0xa3); f_list.append_addr_of_global_var(v->name, ntv.count); 
				ntv.gencode_int32(0x00000000); // GLOBAL_INSERT
		} else {
			ntv.gencode(0x89); ntv.gencode(0x45);
				ntv.gencode(256 - ADDR_SIZE * v->id); // mov var eax
			if(first_decl && var->get_type() == AST_VARIABLE) v->type = ty; // for type inference
		}
	}
}

var_t *VariableDeclAST::get(Function &f, Module &f_list) {
	if(info.is_global) return f_list.var_global.get(info.name, info.mod_name);
	return f.var.get(info.name, info.mod_name);
}

var_t *VariableDeclAST::append(Function &f, Module &f_list) {
	if(info.is_global) return f_list.var_global.append(info.name, info.type, true);
	return f.var.append(info.name, info.type);
}

int VariableIndexAST::codegen(Function &f, Module &f_list, NativeCode_x86 &ntv) {
	int ty = codegen_expression(f, f_list, var);
	ntv.genas("push eax");
	codegen_expression(f, f_list, idx);
	ntv.genas("mov ecx eax");
	ntv.genas("pop edx");
	if(IS_TYPE(ty, T_STRING)) {
		ntv.gencode(0x0f); ntv.gencode(0xb6); ntv.gencode(0x04); ntv.gencode(0x0a);// movzx eax, [edx + ecx]
		return T_INT; // T_CHAR;
	} else {
		ntv.gencode(0x8b); ntv.gencode(0x04); ntv.gencode(0x8a);// mov eax, [edx + ecx * 4]
		return ((T_STRING | T_ARRAY) == ty) ? T_STRING : T_INT;
	}
}

void VariableAST::codegen(Function &f, Module &f_list, NativeCode_x86 &ntv) {
	var_t *v;
	if(info.is_global) 
		v = f_list.var_global.get(info.name, info.mod_name);
	else 
		v = f.var.get(info.name, info.mod_name);
	if(v == NULL) error("error: '%s' was not declared", info.name.c_str());
	if(info.is_global == false) {
		ntv.gencode(0x8b); ntv.gencode(0x45);
			ntv.gencode(256 - ADDR_SIZE * v->id); // mov eax variable
	} else { // global
		ntv.gencode(0xa1); f_list.append_addr_of_global_var(v->name, ntv.count);
			ntv.gencode_int32(0x00000000); // mov eax GLOBAL_ADDR GLOBAL_INSERT
	}
}
var_t *VariableAST::get(Function &f, Module &f_list) {
	if(info.is_global) return f_list.var_global.get(info.name, info.mod_name);
	return f.var.get(info.name, info.mod_name);
}
var_t *VariableAST::append(Function &f, Module &f_list) {
	if(info.is_global) return f_list.append_global_var(info.name, info.type);
	return f.var.append(info.name, info.type);
}

void ReturnAST::codegen(Function &f, Module &f_list, NativeCode_x86 &ntv) {
	codegen_expression(f, f_list, expr);
	ntv.gencode(0xe9);
		f.return_list.push_back(ntv.count);
		ntv.gencode_int32(0x00000000);
}

void BreakAST::codegen(Function &f, Module &f_list, NativeCode_x86 &ntv) {
	ntv.gencode(0xe9); // jmp
		f.break_list.top()->push_back(ntv.count);
		ntv.gencode_int32(0x00000000);
}

int ArrayAST::codegen(Function &f, Module &f_list, NativeCode_x86 &ntv) {
	ntv.genas("mov eax %d", elems.size() * ADDR_SIZE);
	ntv.gencode(0x89); ntv.gencode(0x04); ntv.gencode(0x24); // mov [esp], eax
	ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(12); // call malloc
	ntv.genas("push eax");
	uint32_t a = 0;
	int ty;
	for(ast_vector::iterator it = elems.begin(); it != elems.end(); ++it) {
		ty = codegen_expression(f, f_list, *it);
		ntv.genas("pop ecx");
		ntv.genas("mov edx %d", a);
		ntv.gencode(0x89); ntv.gencode(0x81); ntv.gencode_int32(a); // mov [ecx+a], eax
		ntv.genas("push ecx");
		a += 4;
	}
	ntv.genas("pop eax");
	return ty | T_ARRAY;
}

void StringAST::codegen(Function &f, NativeCode_x86 &ntv) {
	ntv.gencode(0xb8);
		char *embed = (char *)calloc(sizeof(char), str.length() + 2);
		replace_escape(strcpy(embed, str.c_str()));
	ntv.gencode_int32((uint32_t)embed); // mov eax string_address
}

void NumberAST::codegen(Function &f, NativeCode_x86 &ntv) {
	ntv.genas("mov eax %d", number);
}
