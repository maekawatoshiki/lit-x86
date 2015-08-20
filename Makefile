CC = clang
CFLAGS = -Wall -m32

lit: lit.c

clean:
	$(RM) a.out lit *.o
