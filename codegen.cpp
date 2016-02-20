#include "codegen.h"
#include "asm.h"
#include "expr.h"
#include "exprtype.h"
#include "parse.h"

Function FunctionAST::codegen(FunctionList &f_list) {
	Function f;
	f.info.name = info.name;
	f.info.address = ntv.count;

	ntv.genas("push ebp");
	ntv.genas("mov ebp esp");
	uint32_t esp_ = ntv.count + 2; ntv.genas("sub esp 0");
	if(info.name == "main") {
		ntv.gencode(0x8b); ntv.gencode(0x75); ntv.gencode(0x0c); // mov esi, 0xc(%ebp)
	}

	for(ast_vector::iterator it = statement.begin(); it != statement.end(); ++it) {
		codegen_expression(f, f_list, *it);
	}

	ntv.genas("add esp %u", f.var.total_size() + 6 * ADDR_SIZE); // add esp nn
	ntv.gencode(0xc9);// leave
	ntv.gencode(0xc3);// ret

	ntv.gencode_int32_insert(f.var.total_size() + ADDR_SIZE * 6, esp_);

	return f;
}

void IfAST::codegen(Function &f, FunctionList &f_list, NativeCode_x86 &ntv) {
	codegen_expression(f, f_list, cond);	
	ntv.gencode(0x83); ntv.gencode(0xf8); ntv.gencode(0x00);// cmp eax, 0
	ntv.gencode(0x75); ntv.gencode(0x05); // jne 5
	ntv.gencode(0xe9); int end = ntv.count; ntv.gencode_int32(0);// jmp
	for(ast_vector::iterator it = then_block.begin(); it != then_block.end(); ++it)
		codegen_expression(f, f_list, *it);
	ntv.gencode(0xe9); int end1 = ntv.count; ntv.gencode_int32(0);// jmp end
	ntv.gencode_int32_insert(ntv.count - end - 4, end);

	for(ast_vector::iterator it = else_block.begin(); it != else_block.end(); ++it)
		visit(*it), codegen_expression(f, f_list, *it);
	ntv.gencode_int32_insert(ntv.count - end1 - 4, end1);
}

void WhileAST::codegen(Function &f, FunctionList &f_list, NativeCode_x86 &ntv) {
	int loop_bgn = ntv.count;
	codegen_expression(f, f_list, cond);
	ntv.gencode(0x83); ntv.gencode(0xf8); ntv.gencode(0x00);// cmp eax, 0
	ntv.gencode(0x75); ntv.gencode(0x05); // jne 5
	ntv.gencode(0xe9); int end = ntv.count; ntv.gencode_int32(0);// jmp while end

	for(ast_vector::iterator it = block.begin(); it != block.end(); ++it) {
		codegen_expression(f, f_list, *it);
	}

	ntv.gencode(0xe9); ntv.gencode_int32(0xFFFFFFFF - ntv.count + loop_bgn - ADDR_SIZE); // jmp n
	ntv.gencode_int32_insert(ntv.count - end - ADDR_SIZE, end);

	for(std::vector<int>::iterator it = f.break_list.begin(); it != f.break_list.end(); ++it) {
		ntv.gencode_int32_insert(ntv.count - *it - ADDR_SIZE, *it);
	}
}

int codegen_expression(Function &f, FunctionList &f_list, AST *ast) {
	switch(ast->get_type()) {
	case AST_NUMBER:
		((NumberAST *)ast)->codegen(f, ntv);
		return T_INT;
	case AST_STRING:
		((StringAST *)ast)->codegen(f, ntv);
		return T_STRING;
	case AST_VARIABLE:
		((VariableAST *)ast)->codegen(f, ntv);
		return ((VariableAST *)ast)->get(f)->type;
	case AST_VARIABLE_ASGMT:
		((VariableAsgmtAST *)ast)->codegen(f, f_list, ntv);
		return T_INT;
	case AST_FUNCTION_CALL:
		((FunctionCallAST *)ast)->codegen(f, f_list, ntv);
		return T_INT;
	case AST_BINARY: 
		return ((BinaryAST *)ast)->codegen(f, f_list, ntv);
	case AST_VARIABLE_INDEX:
		return ((VariableIndexAST *)ast)->codegen(f, f_list, ntv);
	case AST_IF:
		((IfAST *)ast)->codegen(f, f_list, ntv);
		return T_INT;
	case AST_WHILE:
		((WhileAST *)ast)->codegen(f, f_list, ntv);
		return T_INT;
	case AST_BREAK:
		ntv.gencode(0xe9); // jmp
		f.break_list.push_back(ntv.count); 
		ntv.gencode_int32(0x00000000);
		return T_INT;
	}
	return -1;
}

void FunctionCallAST::codegen(Function &f, FunctionList &f_list, NativeCode_x86 &ntv) {
	struct {
		std::string name, mod_name;
		int args, addr;
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
			} else {
				if(stdfunc[i].args == -1) { // vector
					uint32_t a = 1;
					for(ast_vector::iterator it = args.begin(); it != args.end(); ++it) {
						codegen_expression(f, f_list, *it);
						ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(ADDR_SIZE * (a++)); // mov [esp+a], eax
					}
				} else { // normal function
					for(int arg = 0; arg < stdfunc[i].args; arg++) {
						codegen_expression(f, f_list, args[arg]);
						ntv.gencode(0x89); ntv.gencode(0x44); ntv.gencode(0x24); ntv.gencode(arg * ADDR_SIZE); // mov [esp+arg*4], eax
					}
				}
				ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(stdfunc[i].addr); // call $function
			}
		}
	}
}

int BinaryAST::codegen(Function &f, FunctionList &f_list, NativeCode_x86 &ntv) {
	codegen_expression(f, f_list, left);
	ntv.genas("push eax");
	int ty = codegen_expression(f, f_list, right);
	ntv.genas("mov ebx eax");
	ntv.genas("pop eax");
	if(op == "+") ntv.genas("add eax ebx");
	else if(op == "-") ntv.genas("sub eax ebx");
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
		puts(op.c_str());
		bool lt = op == "<", gt = op == ">", ne = op == "!=", eql = op == "==", fle = op == "<=";
		ntv.gencode(0x39); ntv.gencode(0xd8); // cmp %eax, %ebx
		ntv.gencode(0x0f); ntv.gencode(lt ? 0x9c : gt ? 0x9f : ne ? 0x95 : eql ? 0x94 : fle ? 0x9e : 0x9d); ntv.gencode(0xc0); // setX al
		ntv.gencode(0x0f); ntv.gencode(0xb6); ntv.gencode(0xc0); // movzx eax al
	} else if(op == "and" || op == "&" || op == "or" || 
			op == "|" || op == "xor" || op == "^") {
		bool andop = op == "and", orop = op == "or";
		ntv.gencode(andop ? 0x21 : orop ? 0x09 : 0x31); ntv.gencode(0xd8); // and eax ebx
	}
	return ty;
}

void VariableAsgmtAST::codegen(Function &f, FunctionList &f_list, NativeCode_x86 &ntv) {
	var_t *v;
	if(var->get_type() == AST_VARIABLE)
		v = ((VariableAST *)var)->get(f);
	else if(var->get_type() == AST_VARIABLE_DECL) 
		v = ((VariableDeclAST *)var)->get(f);
	if(v == NULL) v = f.var.append(((VariableAST *)var)->info.name, T_INT, "");
	codegen_expression(f, f_list, src);
	ntv.gencode(0x89); ntv.gencode(0x45);
	ntv.gencode(256 - ADDR_SIZE * v->id); // mov var eax
}

var_t *VariableDeclAST::get(Function &f) {
	return f.var.append(info.name, info.type, "");
}

int VariableIndexAST::codegen(Function &f, FunctionList &f_list, NativeCode_x86 &ntv) {
	int ty = codegen_expression(f, f_list, var); 
	ntv.genas("mov edx eax");
	codegen_expression(f, f_list, idx);
	ntv.genas("mov ecx eax");
	if(ty & T_STRING) {
		ntv.gencode(0x0f); ntv.gencode(0xb6); ntv.gencode(0x04); ntv.gencode(0x0a);// movzx eax, [edx + ecx]
		return T_CHAR;
	} else {
		ntv.gencode(0x8b); ntv.gencode(0x04); ntv.gencode(0x8a);// mov eax, [edx + ecx * 4
		return T_INT;
	}
}

void VariableAST::codegen(Function &f, NativeCode_x86 &ntv) {
	var_t *v = f.var.get(info.name, info.mod_name);
	if(v == NULL) puts("error: null");
	if(v->loctype == V_LOCAL) {
		ntv.gencode(0x8b); ntv.gencode(0x45);
		ntv.gencode(256 - ADDR_SIZE * v->id); // mov eax variable
	} else if(v->loctype == V_GLOBAL) {
		ntv.gencode(0xa1); ntv.gencode_int32(v->id); // mov eax GLOBAL_ADDR
	}
}
var_t *VariableAST::get(Function &f) {
	return f.var.get(info.name, info.mod_name);
}

void StringAST::codegen(Function &f, NativeCode_x86 &ntv) {
	ntv.gencode(0xb8);
	char *embed = (char *)malloc(str.length() + 1);
	replace_escape(strcpy(embed, str.c_str()));
	ntv.gencode_int32((uint32_t)embed); // mov eax string_address
}

void NumberAST::codegen(Function &f, NativeCode_x86 &ntv) {
	ntv.genas("mov eax %d", number);
}
