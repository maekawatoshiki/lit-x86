CC = clang
CFLAGS = -Wall -m32

lit: lit.c

clean:
	-rm a.out lit *.o
