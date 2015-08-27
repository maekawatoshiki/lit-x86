CFLAGS = -m32 
CC = clang $(CFLAGS)

lit: asm.c lit.c 
	$(CC) -c asm.c
	$(CC) -c lit.c
	$(CC) -o lit lit.o asm.o

clean:
	$(RM) a.out lit *.o 
