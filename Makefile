CFLAGS = -O0  -Wno-strict-aliasing -rdynamic -std=c++11
CC = clang++ $(CFLAGS) 

lit: main.o lit.o asm.o lex.o var.o expr.o parse.o  token.o option.o util.o library.o func.o ast.o codegen.o exprtype.o
	$(CC) -o lit -rdynamic -ldl main.o lit.o asm.o lex.o var.o expr.o parse.o \
		token.o option.o util.o library.o func.o ast.o codegen.o exprtype.o `llvm-config --cppflags --ldflags --libs core jit native` 

main.o: main.cpp common.h
	$(CC) -c main.cpp `llvm-config --cppflags --ldflags --libs core jit native` 

lit.o: lit.h lit.cpp
	$(CC) -c lit.cpp `llvm-config --cppflags --ldflags --libs core jit native` 

asm.o: asm.h asm.cpp
	$(CC) -c asm.cpp `llvm-config --cppflags --ldflags --libs core jit native` 

lex.o: lex.h lex.cpp
	$(CC) -c lex.cpp `llvm-config --cppflags --ldflags --libs core jit native` 

var.o: var.h var.cpp
	$(CC) -c var.cpp `llvm-config --cppflags --ldflags --libs core jit native` 

expr.o: expr.h expr.cpp
	$(CC) -c expr.cpp `llvm-config --cppflags --ldflags --libs core jit native` 

parse.o: parse.h parse.cpp
	$(CC) -c parse.cpp `llvm-config --cppflags --ldflags --libs core jit native` 

ast.o: ast.h ast.cpp
	$(CC) -c ast.cpp `llvm-config --cppflags --ldflags --libs core jit native` 

codegen.o: codegen.h codegen.cpp
	$(CC) -c codegen.cpp `llvm-config --cppflags --ldflags --libs core jit native` 

exprtype.o: exprtype.h exprtype.cpp
	$(CC) -c exprtype.cpp `llvm-config --cppflags --ldflags --libs core jit native` 

func.o: func.h func.cpp
	$(CC) -c func.cpp `llvm-config --cppflags --ldflags --libs core jit native` 

token.o: token.h token.cpp
	$(CC) -c token.cpp `llvm-config --cppflags --ldflags --libs core jit native` 

option.o: option.h option.cpp
	$(CC) -c option.cpp `llvm-config --cppflags --ldflags --libs core jit native` 

util.o: util.h util.cpp
	$(CC) -c util.cpp `llvm-config --cppflags --ldflags --libs core jit native` 

library.o: library.h library.cpp
	$(CC) -c library.cpp `llvm-config --cppflags --ldflags --libs core jit native` 

lib: lit
	make -j -f lib/Makefile

test: lit lib
	./test/test.sh

install: lit
	mkdir ~/.lit
	cp -p lit ~/.lit

clean:
	$(RM) a.out lit *.o lib/*.so

