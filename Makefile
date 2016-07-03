CFLAGS = -O3 -Wno-strict-aliasing -rdynamic -std=c++11
CC = clang++ $(CFLAGS) 
LLVM = `llvm-config-3.4 --cppflags --ldflags --libs all`

lit: main.o lit.o lex.o var.o expr.o parse.o  token.o option.o util.o library.o func.o ast.o codegen.o exprtype.o
	$(CC) -o lit -rdynamic -ldl main.o lit.o lex.o var.o expr.o parse.o \
		token.o option.o util.o library.o func.o ast.o codegen.o exprtype.o $(LLVM) 

main.o: main.cpp common.h
	$(CC) -c main.cpp $(LLVM) 

lit.o: lit.h lit.cpp
	$(CC) -c lit.cpp $(LLVM) 

lex.o: lex.h lex.cpp
	$(CC) -c lex.cpp $(LLVM) 

var.o: var.h var.cpp
	$(CC) -c var.cpp $(LLVM) 

expr.o: expr.h expr.cpp
	$(CC) -c expr.cpp $(LLVM) 

parse.o: parse.h parse.cpp
	$(CC) -c parse.cpp $(LLVM) 

ast.o: ast.h ast.cpp
	$(CC) -c ast.cpp $(LLVM) 

codegen.o: codegen.h codegen.cpp
	$(CC) -c codegen.cpp $(LLVM) 

exprtype.o: exprtype.h exprtype.cpp
	$(CC) -c exprtype.cpp $(LLVM) 

func.o: func.h func.cpp
	$(CC) -c func.cpp $(LLVM) 

token.o: token.h token.cpp
	$(CC) -c token.cpp $(LLVM) 

option.o: option.h option.cpp
	$(CC) -c option.cpp $(LLVM) 

util.o: util.h util.cpp
	$(CC) -c util.cpp $(LLVM) 

library.o: library.h library.cpp
	$(CC) -c library.cpp $(LLVM) 

lib: lit
	make -j -f lib/Makefile

test: lit lib
	./test/test.sh

install: test
	mkdir ~/.lit
	cp -p lit ~/.lit
	cp -r ./lib ~/.lit/lib
	echo "please set path yourself:)"

clean:
	$(RM) a.out lit *.o lib/*.so lib/*.ll

