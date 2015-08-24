CC = clang
CFLAGS = -m32 -O3

lit: asm.o lit.o

clean:
	$(RM) a.out lit *.o
