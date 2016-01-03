CFLAGS = -O0 -m32 -Wno-strict-aliasing
CC = clang++ $(CFLAGS) 

lit: main.o lit.o asm.o lex.o var.o expr.o parse.o stdfunc.o token.o option.o util.o library.o
	$(CC) -o lit -rdynamic -ldl main.o lit.o asm.o lex.o var.o expr.o parse.o token.o stdfunc.o option.o util.o library.o

main.o: main.cpp common.h
	$(CC) -c main.cpp

lit.o: lit.h lit.cpp
	$(CC) -c lit.cpp

asm.o: asm.h asm.cpp
	$(CC) -c asm.cpp

lex.o: lex.h lex.cpp
	$(CC) -c lex.cpp

var.o: var.h var.cpp
	$(CC) -c var.cpp

expr.o: expr.h expr.cpp
	$(CC) -c expr.cpp

parse.o: parse.h parse.cpp
	$(CC) -c parse.cpp

stdfunc.o: stdfunc.h stdfunc.cpp
	$(CC) -c stdfunc.cpp

token.o: token.h token.cpp
	$(CC) -c token.cpp

option.o: option.h option.cpp
	$(CC) -c option.cpp

util.o: util.h util.cpp
	$(CC) -c util.cpp

library.o: library.h library.cpp
	$(CC) -c library.cpp

lib: lit
	make -j -f lib/Makefile

test: lit lib
	./test/test.sh

install: lit
	mkdir ~/.lit
	cp -p lit ~/.lit

clean:
	$(RM) a.out lit *.o lib/*.so

