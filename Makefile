CC = clang
CFLAGS = -m32 -O0

lit: lit.c

clean: 
	rm lit *.o
