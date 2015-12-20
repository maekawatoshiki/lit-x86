CFLAGS = -O0 -m32 -std=c99 -Wno-strict-aliasing -Wno-strict-aliasing
CC = clang $(CFLAGS) 
LIB_PATH = lib

lit: main.o lit.o asm.o lex.o var.o expr.o parse.o stdfunc.o option.o util.o
	$(CC) -o lit -rdynamic -ldl main.o lit.o asm.o lex.o var.o expr.o parse.o stdfunc.o option.o util.o

main.o: main.c
	$(CC) -c main.c

lit.o: lit.h lit.c
	$(CC) -c lit.c

asm.o: asm.h asm.c
	$(CC) -c asm.c

lex.o: lex.h lex.c
	$(CC) -c lex.c

var.o: var.h var.c
	$(CC) -c var.c

expr.o: expr.h expr.c
	$(CC) -c expr.c

parse.o: parse.h parse.c
	$(CC) -c parse.c

stdfunc.o: stdfunc.h stdfunc.c
	$(CC) -c stdfunc.c

option.o: option.h option.c
	$(CC) -c option.c

util.o: util.h util.c
	$(CC) -c util.c

lib: lit
	clang -shared -m32 -lm -O3 -o $(LIB_PATH)/Sys.so $(LIB_PATH)/Sys_linux.c
	clang -shared -m32 -lm -O3 -o $(LIB_PATH)/Prime.so $(LIB_PATH)/Prime.c
	clang -shared -m32 -lm -O3 -o $(LIB_PATH)/M.so $(LIB_PATH)/M.c

test: lit lib
	./test/test.sh

install: lit
	mkdir ~/.lit
	cp -p lit ~/.lit

clean:
	$(RM) a.out lit *.o lib/*.so

