# lit language spec

## Syntax Summary

## Lexical Syntax

```
white_space  ::= ' ' | '\t'
comment      ::= '#' (not '\n')*
semi         ::= '\n' | "\r\n" | ';'
ident        ::= ('a'|...|'z'|'A'|..|'Z'|'_')('a'|...|'z'|'A'|..|'Z'|'_'|'0'|...|'9')*
number       ::= ('0'|...|'9')+ ('.' ('0'|...|'9')*)?
char         ::= "'" . "'"
string       ::= '"' (not('"'|'\') | '\' .)* '"'
operator     ::= . ('=' | "::" | "[]" | "++" | "--" | "..")? ".."?
```

特にソースを見ずに適当に書いたので書き換えましょう。

## Context-free Syntax

```
type         ::= "int"
               | "char"
               | "bool"
               | "string"
               | "double"
               | ident
program      ::= eval
eval         ::= ";"* (expression (";"*))*
expression   ::= func
               | proto
               | struct
               | module
               | lib
               | for
               | while
               | return
               | if
               | break
               | expr_entry
break        ::= "break"
return       ::= "return" expr_entry
lib          ::= "lib" ident eval "end"
proto        ::= "proto" ident "(" (expr_ident ("," expr_ident)*)? ")" (":" type "[]"?)? ("|" ident)?  
               | "proto" ident     (expr_ident ("," expr_ident)*)?     (":" type "[]"?)? ("|" ident)?  
module       ::= "module" ident eval "end"
struct       ::= "struct" ident eval "end"
if           ::= "if" expr_entry eval "else" eval "end"
               | "if" expr_entry eval "elsif" elsif "end"
elsif        ::= "elsif" expr_entry eval elsif
               | "elsif" expr_entry "else" eval
for          ::= "for" expr_entry "," expr_entry "," expr_entry eval end
               | "for" expr_entry "in" expr_entry eval "end" // todo ast_binary...
while        ::= "while" expr_entry eval "end"
func         ::= "def" ident "(" (expr_ident ("," expr_ident)*)? ")" (":" type "[]"?)? eval "end"
               | "def" ident     (expr_ident ("," expr_ident))?      (":" type "[]"?)? eval "end"
expr_ident   ::= "(" expr_ident ")"
               | ident
expr_rhs     ::= (operator expr_dot)*
expr_entry   ::= expr_dot expr_rhs
expr_dot     ::= expr_index string
               | expr_index ("." expr_primary)*
expr_index   ::= expr_unary string
               | expr_unary "[" expr_entry "]"
               | expr_unary
expr_unary   ::= "<" type ">" expr_entry
               | expr_primary
expr_primary ::= "$"? "new" "!" type?
               | "$"? "new" expr_entry type?
               | "$"? number
               | "$"? char
               | "$"? string
               | "$"? "true"
               | "$"? "false"
               | "$"? ident
               | "$"? "(" expr_entry ")"
               | "$"? expr_array
expr_array   ::= "[" (expr_entry ("," expr_entry)*) "]"
               | "[" "]" ":" ident "[]"*
               | "[]" ":" ident "[]"*
```

## type

```
type         ::= "int"
               | "char"
               | "bool"
               | "string"
               | "double"
               | ident
```

型は"int", "char", "bool", "string", "double" 又は ident です。

## program

```
program      ::= eval
eval         ::= ";"* (expression (";"*))*
```

プログラム(program)は評価(eval)です。評価は0個以上の";"で分離された複数の式です。

todo: evalっておかしくないかなぁ？
expression\_listとかexpr\_list, expressions, exprsとかの法が良いような気がする。

## expression

```
expression   ::= func
               | proto
               | struct
               | module
               | lib
               | for
               | while
               | return
               | if
               | break
               | expr_entry
```

式は、関数式、proto式、struct式、module式、lib式、for式、while式、return式、if式、break式、entry式があります。

todo: entry式の名前はentryでいいような気もする。

## break expression

```
break        ::= "break"
```

break式は"break"と書き、ループ文から抜けます。

## return expression

```
return       ::= "return" expr_entry
```

return式は、return entry式 と書き、関数からリターンします。値を返さない場合はtodoです。


## lib expression

```
lib          ::= "lib" ident eval "end"
```

lib式の詳細はtodoです。

## proto expression

```
proto        ::= "proto" ident "(" (expr_ident ("," expr_ident)*)? ")" (":" type "[]"?)? ("|" ident)?  
               | "proto" ident     (expr_ident ("," expr_ident)*)?     (":" type "[]"?)? ("|" ident)?  
```

proto式は、関数のプロトタイプを宣言します。プロトタイプは関数のインターフェイスを表すものであり関数の本体はありません。

## module expression

```
module       ::= "module" ident eval "end"
```

モジュール式は、モジュールを表します。

## struct expression

```
struct       ::= "struct" ident eval "end"
```

struct式は構造体を定義する式です。

## if expression

```
if           ::= "if" expr_entry eval "else" eval "end"
               | "if" expr_entry eval "elsif" elsif "end"
elsif        ::= "elsif" expr_entry eval elsif
               | "elsif" expr_entry "else" eval
```

if式は条件分岐を行う式です。todo elseとelsifの事を書く。

## for expression

```
for          ::= "for" expr_entry "," expr_entry "," expr_entry eval end
               | "for" expr_entry "in" expr_entry eval "end" // todo ast_binary...
```

for式はループする式です。

## while expression

```
while        ::= "while" expr_entry eval "end"
```

while式はループする式です。１つ目の式が条件を表します。

## function expression

```
func         ::= "def" ident "(" (expr_ident ("," expr_ident)*)? ")" (":" type "[]"?)? eval "end"
               | "def" ident     (expr_ident ("," expr_ident))?      (":" type "[]"?)? eval "end"
expr_ident   ::= "(" expr_ident ")"
               | ident
```

関数式は、関数を定義します。

## entry expression

```
expr_rhs     ::= (operator expr_dot)*
expr_entry   ::= expr_dot expr_rhs
expr_dot     ::= expr_index string
               | expr_index ("." expr_primary)*
expr_index   ::= expr_unary string
               | expr_unary "[" expr_entry "]"
               | expr_unary
expr_unary   ::= "<" type ">" expr_entry
               | expr_primary
```

entry式は様々な演算子を含んだ式です。詳細はtodo

## primary expression

```
expr_primary ::= "$"? "new" "!" type?
               | "$"? "new" expr_entry type?
               | "$"? number
               | "$"? char
               | "$"? string
               | "$"? "true"
               | "$"? "false"
               | "$"? ident
               | "$"? "(" expr_entry ")"
               | "$"? expr_array
```

primary式は基本的な式ですが、$が先頭に着くとグローバルな意味となります。


## array literal expression

```
expr_array   ::= "[" (expr_entry ("," expr_entry)*)? "]"
               | "[]" ":" ident "[]"*
```

array式は配列リテラルです。
空の配列リテラルの場合は、型を指定する必要があります。
