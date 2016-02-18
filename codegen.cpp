#include "codegen.h"
#include "ast.h"
#include "asm.h"
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
		int ty = (*it)->get_type();
		if(ty == AST_FUNCTION_CALL) ((FunctionCallAST *)*it)->codegen(f, f_list, ntv);
	}

	ntv.genas("add esp %u", f.var.total_size() + 6 * ADDR_SIZE); // add esp nn
	ntv.gencode(0xc9);// leave
	ntv.gencode(0xc3);// ret
	ntv.gencode_int32_insert(f.var.total_size() + ADDR_SIZE * 6, esp_);

	return f;
}

int codegen_expression(Function &f, AST *ast) {
	if(ast->get_type() == AST_NUMBER) {
		((NumberAST *)ast)->codegen(f, ntv);
		return AST_NUMBER;
	} else if(ast->get_type() == AST_STRING) { 
		((StringAST *)ast)->codegen(f, ntv);
		return AST_STRING;
	} else if(ast->get_type() == AST_BINARY) {
		((BinaryAST *)ast)->codegen(f, ntv);
		return AST_BINARY;
	}
	return -1;
}

void FunctionCallAST::codegen(Function &f, FunctionList &f_list, NativeCode_x86 &ntv) {
	if(info.name == "puts") {
		for(int i = 0; i < args.size(); i++) {
			int ty = codegen_expression(f, args[i]);
			if(ty == AST_STRING) {
				ntv.gencode(0x89); ntv.gencode(0x04); ntv.gencode(0x24); // mov [esp], eax
				ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(4);// call *0x04(esi) putString
			} else if(ty == AST_NUMBER || ty == AST_BINARY) {
				ntv.gencode(0x89); ntv.gencode(0x04); ntv.gencode(0x24); // mov [esp], eax
				ntv.gencode(0xff); ntv.gencode(0x16); // call (esi) putNumber
			}
		}
		ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(8);// call *0x08(esi) putLN
	}
}

void BinaryAST::codegen(Function &f, NativeCode_x86 &ntv) {
	codegen_expression(f, left);
	ntv.genas("push eax");
	codegen_expression(f, right);
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
		bool lt = op == "<", gt = op == ">", ne = op == "!=", eql = op == "==", fle = "<=";
		ntv.gencode(0x39); ntv.gencode(0xd8); // cmp %eax, %ebx
		ntv.gencode(0x0f); ntv.gencode(lt ? 0x9c : gt ? 0x9f : ne ? 0x95 : eql ? 0x94 : fle ? 0x9e : 0x9d); ntv.gencode(0xc0); // setX al
		ntv.gencode(0x0f); ntv.gencode(0xb6); ntv.gencode(0xc0); // movzx eax al
	} else if(op == "and" || op == "&" || op == "or" || 
			op == "|" || op == "xor" || op == "^") {
		bool andop = op == "and", orop = op == "or";
		ntv.gencode(andop ? 0x21 : orop ? 0x09 : 0x31); ntv.gencode(0xd8); // and eax ebx
	}
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
