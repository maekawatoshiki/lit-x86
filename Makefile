CC = clang
CFLAGS = -Wall -m32

lit: lit.c
	$(CC) $(CFLAGS) lit.c -o lit 
	./lit lf.rb

clean:
	rm a.out lit *.o
