// ci0.c  2013.10.03  Hatada
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//----------------------------------------------------------------------------//
//                               字句解析                                     //
//----------------------------------------------------------------------------//
#define  SIZETOKEN      1000    // トークン配列のサイズ
#define  OPERATOR2      "== != <= >= |= += -= *= /= >> << ++ -- && || -> "

int  fToken, fTrace, fCode;       // コマンド引数オプション
char *Token[SIZETOKEN];                 // トークン配列
int  nToken;                            // トークン数
int  tix = -1;  //カレントトークン.構文解析で使用.エラー関数共用のためここに置く

void error(const char *format, ...) {
    vfprintf(stderr, format, (char*)&format + sizeof(char*));
    if (tix >= 0) fprintf(stderr, "\n  Token[%d] = \"%s\"\n", tix, Token[tix]);
    exit(1);
}   // 移植性のために、一般には stdarg.h のマクロを使う方がよい。
int isAlpha(int c) { return (isalpha(c) || c == '_'); }
int isAlNum(int c) { return isAlpha(c) || isdigit(c); }
int isKanji(int c) { return (c>=0x81 && c<=0x9F) || (c>=0xE0 && c<=0xFC); }

void lex(const char *srcfile) {
    char linebuf[256], buf[128], op2[4], *p, *pBgn;
    FILE *fpSrc = fopen(srcfile, "r");
    if (fpSrc == NULL) error("file '%s' can't open", srcfile);
    while (fgets(linebuf, sizeof(linebuf), fpSrc) != NULL) {
        for (p = linebuf; *p != '\0'; ) {       // 一行分の字句解析
            if (*p <= ' ') { p++; continue; }   // 空白文字をスキップする
            if (p[0]=='/' && p[1]=='/') break;  // コメント. 行末までスキップ
            pBgn = p;
            if (*p == '"') {   // 文字列. escape文字,2byte文字のとき2byte目をスキップ
                for (p++; *p != '\0' && *p != '"'; p++)
                    if (*p == '\\' || isKanji(*p)) p++;
                if (*p++ != '"') error("後ろの引用符がありません<%s>", pBgn);
            } else if (isdigit(*p)) {                   // 数値(10進)
                for (p++; *p != '\0' && isdigit(*p); p++) ;
            } else if (strncmp(pBgn,"char*",5) == 0) {  // char*
                p += 5;
            } else if (isAlpha(*p)) {                   // 識別子, 予約語
                for (p++; *p != '\0' && isAlNum(*p); ) p++;
            } else {                                    // 演算子
                sprintf(op2, "%c%c ", p[0], p[1]);
                p += strstr(OPERATOR2, op2) != NULL ? 2 : 1;
            }
            sprintf(buf, "%.*s", p - pBgn, pBgn);
            if (nToken == SIZETOKEN) error("トークン配列オーバフロー");
            Token[nToken++] = strdup(buf);
            if (fToken) printf(strchr(";{}",*buf)!=NULL ? "%s\n" : "%s ", buf);
        }
    }
    fclose(fpSrc);
}

//----------------------------------------------------------------------------//
//                               名前管理                                     //
//----------------------------------------------------------------------------//
#define  SIZEGLOBAL     1000    // グローバル名前管理表のサイズ
#define  SIZELOCAL      1000    // ローカル名前管理表のサイズ
enum { NM_VAR = 0x01, NM_FUNC = 0x02 };         // 名前の種別
enum { AD_DATA = 0, AD_STACK, AD_CODE };        // アドレスの種別

typedef struct _Name {
    int  type;                  // NM_VAR: 変数、NM_FUNC: 関数
    char *dataType, *name;      // データ型, 変数名または関数名
    int  addrType, address;     // アドレスの種別, 相対アドレス
} Name;

Name *GName[SIZEGLOBAL], *LName[SIZELOCAL];    // 名前管理表.
int  nGlobal, nLocal;                          // 同上現在の登録数

// 関数または変数情報を名前管理表に登録する
Name *appendName(int nB, int type, char *dataType, char *name, int addrType, int addr) {
    Name nm = { type, dataType, name, addrType, addr };
    Name *pNew = calloc(1, sizeof(Name));
    if (pNew == NULL) error("appendName: メモリが足りません");
    memcpy(pNew, &nm, sizeof(Name));
    if (nB == 0 && nGlobal < SIZEGLOBAL-1) GName[nGlobal++] = pNew;
    else if (nB == 1 && nLocal < SIZELOCAL-1) LName[nLocal++] = pNew;
    else error("appendName: 引数エラーまたは配列オーバーフロー");
    return pNew;        // 新しいエントリのアドレスを返す
}

// 指定された名前表から指定された名前を探す
Name *getNameFromTable(int nB, int type, char *name, int fErr) {
    int  n;
    int  nEntry = nB==0 ? nGlobal : nLocal;
    for (n = 0; n < nEntry; n++) {
        Name *e = nB==0 ? GName[n] : LName[n];
        if (strcmp(e->name,name) == 0 && (e->type&type) != 0) return e;
    }
    if (fErr && nB==0) error("'%s' undeclared", name);
    return NULL;        // 見つからなかった。
}

// 最初にローカル名前表、なければグローバル名前表から指定された名前を探す
Name *getNameFromAllTable(int type, char *name, int fErr) {
    Name *pName = getNameFromTable(1, type, name, fErr);
    if (pName != NULL) return pName;
    return getNameFromTable(0, type, name, fErr);
}

//----------------------------------------------------------------------------//
//                               構文解析                                     //
//----------------------------------------------------------------------------//
void expression(int mode);                      // 式の構文解析
void statement(int locBreak, int locContinue);  // 文の構文解析

typedef struct _INSTRUCT { int opcode, type, val; } INSTRUCT;
enum { PUSH = 0, ENTRY, POP, MOV, ADD, ADDSP, SUB, MUL, DIV, MOD, RET, CALL,
       JZ, JMP, CMP, LT, GT, LE, GE, EQ, NE, FUNC, LABEL };
char *OPCODE[] = { "push", "entry", "pop", "mov", "add", "addsp", "sub", "mul", "div", "mod",
         "ret", "call", "jz", "jmp", "cmp", "lt", "gt", "le", "ge", "eq", "ne", "func", "label" };
enum { NIL, IMM, STR, MEM, REF, SKV, SKR, LOC, SP };
char *TYPE[] = { "nil", "int", "str", "mem", "ref", "stack-val", "stack-ref", "loc", "sp" };
#define SIZEDATASECT    1000
enum { VAL=0, ADDR };                   // VAL: 変数の内容, ADDR: 変数のアドレス
enum { ST_FUNC=0, ST_GVAR };            // ST_FUNC: 関数定義中, ST_GVAR: 変数宣言中
char *DataSection[SIZEDATASECT];        // グローバル変数、文字列管理配列
int  ixData;                            // データの登録数
int  baseSpace;                         // スタック上の相対アドレス
int  numLabel = 1;      // ラベルの通し番号(ラベル配列の添え字)
INSTRUCT Inst[1000];    // 命令配列
int  nInst = 0;         // 命令数
int  entryPoint;        // 命令を格納するとき main関数の位置を記録しておく

int loc() { return numLabel++; }
int is(char *s) { return strcmp(Token[tix],s) == 0; }
int ispp(char *s) { if (is(s)) { tix++; return 1; } return 0; }
int isTypeSpecifier() { return is("void") || is("int") || is("char*"); }
int isFunctionDefinition() { return *Token[tix+2] == '('; }
void skip(char *s) { if (!ispp(s)) error("'%s' expected", s); }

void printInst(int n, INSTRUCT *pI) {
    printf("%d: %s ", n, OPCODE[pI->opcode]);
    if (pI->type == NIL) printf("\n");
    else printf(pI->type==STR ? "%s %s\n" : "%s %d\n", TYPE[pI->type], pI->val);
}

INSTRUCT *outInst2(int opcode, int type, int val) {
    INSTRUCT inst = { opcode, type, val };
    return memcpy(&Inst[nInst++], &inst, sizeof(inst));
}
INSTRUCT *outInst(int code){ return outInst2(code, 0,0); }

//================================== 式の構文解析 ==============================
/// PrimExpression ::= Identifier | "(" Expr ")" | Constant | FunctionCall
void functionCall() {   // 優先順位 #1
    int n, nParam, posParam[20], nInstSave, ixDataSave, ixNext, rtn;
    char *name  = Token[tix++];
    Name *pName = getNameFromTable(0, NM_FUNC, name, 1);
    skip("(");
    // 引数は最後から順にスタックに積むため、まず位置を求める
    nInstSave  = nInst;
    ixDataSave = ixData;
    for (nParam = 0; !is(")"); nParam++) {
        posParam[nParam] = tix;         // 各引数の先頭トークンの位置
        expression(VAL);
        ispp(",");
    }
    ixNext = tix + 1;                       // ")" の次
    nInst  = nInstSave;
    ixData = ixDataSave;                // 生成したデータを廃棄する
    for (n = nParam; --n >= 0; ) {
        tix = posParam[n];
        expression(VAL);     // 引数を最後から順に評価して、スタックに積む
    }
    tix = ixNext;
    if (pName->address > 0) outInst2(CALL, IMM, pName->address);
    else outInst2(CALL, STR, (int)pName->name);
    outInst2(ADDSP, IMM, nParam);               // スタックポインタ回復
}

void primaryExpression(int mode) {      // 優先順位 #1
    if (isdigit(*Token[tix])) {                 // Constant 数値
        outInst2(PUSH, IMM, atoi(Token[tix++]));
    } else if (*Token[tix] == '"') {            // Constant 文字列
        outInst2(PUSH, STR, (int)Token[tix++]);
    } else if (ispp("(")) {                     // "(" Expr ")"
        expression(VAL);
        skip(")");
    } else if (*Token[tix+1] == '(') {          // FuctionCall
        functionCall();
    } else if (isAlpha(*Token[tix])) {          // Identifier
        Name *pName = getNameFromAllTable(NM_VAR, Token[tix], 1);
        int type = (pName->addrType==AD_STACK)? (mode==VAL?SKV:SKR):(mode==VAL?MEM:REF);
        outInst2(PUSH, type, pName->address);
        tix++;
    } else {
        error("primExpr: <%s>", Token[tix]);
    }
}

/// MulExpression ::= PrimaryExpression ( ("*" | "/" | "%") PrimaryExpression )*
void mulExpression(int mode) {  // #4
    int fMul=0, fDiv=0;
    primaryExpression(mode);    // [左辺の値]
    while ((fMul=ispp("*")) || (fDiv=ispp("/")) || ispp("%")) {
        primaryExpression(mode); // [右辺の値,左辺の値]
        outInst(fMul ? MUL : fDiv ? DIV : MOD);
    }
}

/// AddExpression ::= MulExpression ( ("+" | "-") MulExpression )*
void addExpression(int mode) {  // #5
    int fAdd;
    mulExpression(mode);                // [左辺の値]
    while ((fAdd=ispp("+")) || ispp("-")) {
        mulExpression(mode);            // [右辺の値,左辺の値]
        outInst(fAdd ? ADD : SUB);
    }
}

/// RelationalExpression ::= addExpr [("<" | ">" | "<=" | ">=") addExpr]
void relationalExpression(int mode) {    // #7    // 符号付き整数に対応
    int fLT=0, fGT=0, fLE=0;
    addExpression(mode);        // [左辺の値]
    if ((fLT=ispp("<")) || (fGT=ispp(">")) || (fLE=ispp("<=")) || ispp(">=")){
        addExpression(mode);    // [右辺の値,左辺の値]
        outInst(fLT ? LT : fGT ? GT : fLE ? LE : GE);
    }
}

/// EqualityExpression ::= RelationalExpr [ ("==" | "!=") RelationalExpr ]
void equalityExpression(int mode) { // #8
    int fEQ;
    relationalExpression(mode);         // [左辺の値]
    if ((fEQ=ispp("==")) || ispp("!=")) {
        relationalExpression(mode);     // [右辺の値,左辺の値]
        outInst(fEQ ? EQ : NE);
    }
}

/// expression ::= [Identifier "="] addExpression
void assign() {
    primaryExpression(ADDR);     // [左辺変数アドレス]
    skip("=");
    addExpression(VAL);         // [右辺の値,左辺変数アドレス]
    outInst(MOV);               // Mem[st1] = st0
}

void expression(int mode) {             // #15
    if (strcmp(Token[tix+1],"=") == 0) assign();
    else equalityExpression(mode);
}

//================================ 文の構文解析 ================================
/// TypeSpecifier ::= "void" | "char*" | "int"
char *typeSpecifier() {
    if (!isTypeSpecifier()) error("'%s' not typespecfier", Token[tix]);
    return Token[tix++];        // データ型
}

/// VarDeclarator ::= Identifier
char *varDeclarator() { return Token[tix++]; }  // 変数名または関数名

/// VariableDeclaration ::= TypeSpecifier VarDeclarator ["=" Initializer]
///                             ("," VarDeclarator ["=" Initializer])*
void variableDeclaration(int status) {
    char *varType = typeSpecifier();
    do {
        char *varName = varDeclarator();
        if (status == ST_FUNC) {
            appendName(1, NM_VAR, varType, varName, AD_STACK, --baseSpace);
            if (is("=")) { tix--; assign(); }
        } else {
            appendName(0, NM_VAR, varType, varName, AD_DATA, ixData);
            if (ispp("=")) DataSection[ixData++] = Token[tix++];
        }
    } while (ispp(","));
}

/// CompoundStatement ::= "{" (Statements)* "}"
void compoundStatement(int locBreak, int locContinue) {
    for (skip("{"); tix < nToken && isTypeSpecifier(); skip(";"))
        variableDeclaration(ST_FUNC);   // ローカル変数の宣言
    while (!ispp("}")) statement(locBreak, locContinue);
}

/// IfStatement ::= "if" "(" Expression ")" Statement [ "else" Statement ]
void ifStatement(int locBreak, int locContinue) {
    int locElse, locEnd;
    skip("(");
    expression(VAL);
    skip(")");
    outInst2(JZ, IMM, locElse=loc());
    statement(locBreak, locContinue);           // then_statement
    if (is("else")) outInst2(JMP, IMM, locEnd=loc());
    outInst2(LABEL, IMM, locElse);
    if (ispp("else")) {
        statement(locBreak, locContinue);       // else_statement
        outInst2(LABEL, IMM, locEnd);
    }
}

/// WhileStatement ::= "while" "(" Expression ")" Statement
void whileStatement(int locBreak, int locContinue) {
    int locExpr, locNext;
    outInst2(LABEL, IMM, locExpr=loc());
    skip("(");
    expression(VAL);
    skip(")");
    outInst2(JZ, IMM, locNext=loc());   // expressionの値が 0 ならば、locNextへジャンプ
    statement(locNext, locExpr);        // locBreak, locContinue
    outInst2(JMP, IMM, locExpr);        // 判定式へ無条件ジャンプ
    outInst2(LABEL, IMM, locNext);      // 次の文を指すラベル
}

/// ReturnStatement ::= "return" [Expression] ";"
void returnStatement() {
    if (!is(";")) expression(VAL);
    skip(";");
    outInst(RET);
}

/// Statement ::= CompoundStmt | IfStmt | ReturnStmt| Expression ";" | ";"
void statement(int locBreak, int locContinue) {
    if (ispp(";")) ; // null statement
    else if (is("{")) compoundStatement(locBreak, locContinue);
    else if (ispp("if")) ifStatement(locBreak, locContinue);
    else if (ispp("while")) whileStatement(locBreak, locContinue);
    else if (ispp("return")) returnStatement();
    else { expression(VAL); skip(";"); }
}

/// FunctionDefinition ::= TypeSpecifier  VarDeclarator
///     "(" [ VarDeclaration ("," VarDeclaration)* ] ")" CompoundStatement
void functionDefinition() {
    int locFunc = loc(); // この関数の識別番号を得る
    char *varType = typeSpecifier();            // 返り値のデータ型
    char *varName = varDeclarator();            // 関数名
    nLocal = 0;         // ローカル変数表初期化(メモリ解放省略)
    outInst2(FUNC, STR, (int)varName);
    outInst2(LABEL, IMM, locFunc);
    if (strcmp(varName,"main") == 0) entryPoint = nInst;
    appendName(0, NM_FUNC, varType, varName, AD_CODE, locFunc);
    baseSpace = 2;      // 戻り番地と旧bpの分.
    outInst(ENTRY);     // push ebp;　mov ebp,espに相当
    INSTRUCT *pSub = outInst2(ADDSP, IMM, 0); // sp -= imm; immの値は後で書き換え
    for (skip("("); !ispp(")"); ispp(",")) {
        varType = typeSpecifier();
        varName = varDeclarator();
        appendName(1, NM_VAR, varType, varName, AD_STACK, baseSpace++);
    }
    baseSpace = 0;              // ここからローカル変数領域
    compoundStatement(0, 0);    // 関数本体
    pSub->val = baseSpace;      // ここで imm の値を変更
    if (Inst[nInst-1].opcode != RET) outInst(RET);
}

//============================ 最上位の構文解析 =============================
/// Program ::= (FunctionDefinition | VariableDeclaration ";")*
void program() {
    for (tix = 0; tix < nToken; ) {
        if (fTrace) printf("%3d: %s\n", tix, Token[tix]);
        if (isFunctionDefinition()) functionDefinition();       // 関数定義
        else { variableDeclaration(ST_GVAR); skip(";"); }       // 外部変数宣言
    }
}

void parser() {
    appendName(0, NM_FUNC, "int", "printf", AD_CODE, 0);
    program();
}

//-----------------------------------------------------------------------------//
//                       命令実行                                              //
//-----------------------------------------------------------------------------//
#define  MAXMEM   1000
int  mem[MAXMEM];       // メモリ
int  sp, bp, pc;        // スタックポインタ、ベースポインタ、プログラムカウンタ
int  pos = 0;           // メモリインデックス. データは上から、スタックは下から
int  location[1000];    // ラベルのアドレス配列

void push(int val) {
    if (sp <= pos) error("stack overflow!");
    mem[--sp] = val;
}

int pop() {
    if (sp >= MAXMEM) error("stack empty!");
    return mem[sp++];
}

int getStr(char *str) {
    int n = strlen(str);
    if (str[n-1] == '"') str[n-1] = '\0';
    return str+1;
}

void execute() {
    int n, addr, type, val, rtn;
    if (fTrace) printf("entryPoint = %d\n", entryPoint);
    for (n = 0; n < nInst; n++) {
        if (fCode) printInst(n, &Inst[n]);
        if (Inst[n].opcode == LABEL) location[Inst[n].val] = n;
    }
    for (n = 0; n < ixData; n++)
        mem[pos++] = atoi(DataSection[n]);
    sp = MAXMEM;  // スタックポインタの初期化
    for (pc = entryPoint; pc < nInst; ) {
      if (fTrace) printInst(pc, &Inst[pc]);
      type = Inst[pc].type;
      val  = Inst[pc].val;
      switch (Inst[pc++].opcode) {
        case FUNC:  break;
        case LABEL: break;
        case ENTRY:  // 関数の入り口
            push(bp);
            bp = sp;
            break;
        case ADDSP:
            rtn = pop();        // 関数戻り値がスタックのトップに置かれている
            sp += val;          // 引数分を削除
            push(rtn);          // 改めてスタックのトップに関数戻り値を置く
            break;
        case PUSH:
            if (type == SKV) push(mem[bp+val]);    // ローカル変数の値をpush
            else if (type == SKR) push(bp+val);    // ローカル変数のアドレスをpush
            else if (type == STR) push(getStr((char*)val)); // 文字列のアドレスをpush
            else if (type == IMM) push(val);
            else error("exec.PUSH: %d", type);
            break;
        case MOV:
            val  = pop();
            addr = pop();
            mem[addr] = val;
            break;
        case ADD: mem[sp+1] += mem[sp]; sp++; break;
        case SUB: mem[sp+1] -= mem[sp]; sp++; break;
        case MUL: mem[sp+1] *= mem[sp]; sp++; break;
        case DIV: mem[sp+1] /= mem[sp]; sp++; break;
        case MOD: mem[sp+1] %= mem[sp]; sp++; break;
        case GT:  mem[sp+1] = (mem[sp+1] >  mem[sp]); sp++; break;
        case GE:  mem[sp+1] = (mem[sp+1] >= mem[sp]); sp++; break;
        case LT:  mem[sp+1] = (mem[sp+1] <  mem[sp]); sp++; break;
        case LE:  mem[sp+1] = (mem[sp+1] <= mem[sp]); sp++; break;
        case EQ:  mem[sp+1] = (mem[sp+1] == mem[sp]); sp++; break;
        case NE:  mem[sp+1] = (mem[sp+1] != mem[sp]); sp++; break;
        case JZ:  if (mem[sp++] == 0) pc = location[val]; break;// jz xxx
        case JMP: pc = location[val]; break;
        case CMP: mem[sp+1] -= mem[sp]; sp++; break;
        case CALL:
            if (type == STR) {  // native function call
                rtn = printf((char*)mem[sp], mem[sp+1], mem[sp+2], mem[sp+3]);
                push(rtn);
            } else {
                push(pc);           // 戻り番地格納
                pc = location[val]; // 関数先頭番地を pc にセット
            }
            break;
        case RET:       // 関数からの戻り
            rtn = pop();        // 関数の戻り値
            sp = bp;            // スタックポインタ回復
            bp = pop();         // ベースポインタ回復
            if (sp == MAXMEM) break;       // main関数からのリターン
            pc = pop();         // pcにCALL命令の次の命令アドレスをセット
            push(rtn);          // スタックの一番上に関数の戻り値を置く
            break;
        default: error("unknown opcode: %d\n", Inst[pc-1].opcode);
      }
    }
}

void main(int argc, char *argv[]) {
    char *srcfile = NULL;
    int n;
    for (n = 1; n < argc; n++) {
        if (strcmp(argv[n], "-token") == 0) fToken = 1;
        else if (strcmp(argv[n], "-trace") == 0) fTrace = 1;
        else if (strcmp(argv[n], "-code") == 0) fCode = 1;
        else srcfile = argv[n];
    }
    lex(srcfile);       //==== 字句解析. 結果はToken配列に格納 ====//
    parser();           //==== 構文解析・意味解析・中間語コード生成 ====//
    tix = -1;           // エラー表示で Token[tix] の表示をやめるため
    execute();
}
