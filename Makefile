CC = clang
CFLAGS = -Wall -m32

lit: lit.c

clean:
	rm lit *.o
