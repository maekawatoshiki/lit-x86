#include "codegen.h"
#include "ast.h"
#include "asm.h"
#include "exprtype.h"
#include "parse.h"

Function FunctionAST::codegen() {
	Function f;
	for(ast_vector::iterator it = statement.begin(); it != statement.end(); ++it) {
		int ty = (*it)->get_type();
		if(ty == AST_FUNCTION_CALL) ((FunctionCallAST *)*it)->codegen(f, ntv);
	}
}

void FunctionCallAST::codegen(Function &f, NativeCode_x86 &ntv) {
	if(info.name == "puts") {
		for(int i = 0; i < args.size(); i++) {
			if(args[i]->get_type() == AST_STRING) {
				((StringAST *)args[i])->codegen(f, ntv);
				ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(4);// call *0x04(esi) putString
			}
		}
		ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(8);// call *0x08(esi) putLN
	}
	// do {
	// 	ExprType et; //TODO: fix
	// 	ntv.genas("push eax");
	// 	if(et.is_type(T_STRING)) {
	// 		ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(4);// call *0x04(esi) putString
	// 	} else {
	// 		ntv.gencode(0xff); ntv.gencode(0x16); // call (esi) putNumber
	// 	}
	// 	ntv.genas("add esp 4");
	// } while(tok.skip(","));
	// // for new line
	// if(isputs) {
	// 	ntv.gencode(0xff); ntv.gencode(0x56); ntv.gencode(8);// call *0x08(esi) putLN
	// }
}

void StringAST::codegen(Function &f, NativeCode_x86 &ntv) {
	ntv.gencode(0xb8);
	char *embed = (char *)malloc(str.length() + 1);
	replace_escape(strcpy(embed, str.c_str()));
	ntv.gencode_int32((uint32_t)embed); // mov eax string_address
}
