CC = clang
CFLAGS = -m32 

lit: lit.c

clean:
	$(RM) a.out lit *.o
