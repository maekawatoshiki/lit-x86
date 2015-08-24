CFLAGS = -m32
CC = clang $(CFLAGS)

lit: 
	$(CC) -c asm.c
	$(CC) -c lit.c
	$(CC) -o lit lit.o asm.o

clean:
	$(RM) a.out lit *.o
