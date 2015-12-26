CFLAGS = -O0 -m32 -Wno-strict-aliasing
CC = clang++ $(CFLAGS) 
LIB_PATH = lib

lit: main.o lit.o asm.o lex.o var.o expr.o parse.o stdfunc.o option.o util.o
	$(CC) -o lit -rdynamic -ldl main.o lit.o asm.o lex.o var.o expr.o parse.o stdfunc.o option.o util.o

main.o: main.cpp
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

option.o: option.h option.cpp
	$(CC) -c option.cpp

util.o: util.h util.cpp
	$(CC) -c util.cpp

lib: lit
	clang -shared -m32 -lm -O0 -o $(LIB_PATH)/Sys.so $(LIB_PATH)/Sys_linux.c
	clang -shared -m32 -lm -O0 -o $(LIB_PATH)/Prime.so $(LIB_PATH)/Prime.c
	clang -shared -m32 -lm -O0 -o $(LIB_PATH)/M.so $(LIB_PATH)/M.c
	clang -shared -m32 -lm -O0 -o $(LIB_PATH)/Math.so $(LIB_PATH)/Math.c
	clang -shared -m32 -lm -O0 -o $(LIB_PATH)/String.so $(LIB_PATH)/String.c
	clang -shared -m32 -lm -O0 -o $(LIB_PATH)/Time.so $(LIB_PATH)/Time.c

test: lit lib
	./test/test.sh

install: lit
	mkdir ~/.lit
	cp -p lit ~/.lit

clean:
	$(RM) a.out lit *.o lib/*.so

