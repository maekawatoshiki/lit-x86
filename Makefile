CFLAGS = -m32 
CC = clang $(CFLAGS)

lit: asm.o lit.o 
	$(CC) -o lit lit.o asm.o

asm.o: asm.c asm.h
	$(CC) -c asm.c

lit.o: lit.c lit.h
	$(CC) -c lit.c

clean:
	$(RM) a.out lit *.o 
