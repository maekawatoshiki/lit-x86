CFLAGS = -m32 -O0 -std=c99
CC = clang $(CFLAGS) 


lit: asm.c lit.c
	$(CC) -c lit.c
	$(CC) -c asm.c
	$(CC) -o lit lit.o asm.o

clean:
	$(RM) a.out lit *.o

