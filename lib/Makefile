CFLAGS = -shared -m32 -lm -O0 
CC = clang $(CFLAGS) 

all: lib/M.so lib/Math.so lib/Prime.so lib/String.so lib/Sys.so lib/Time.so lib/File.so

lib/M.so: lib/M.c
	$(CC) -o lib/M.so lib/M.c

lib/Math.so: lib/Math.c
	$(CC) -o lib/Math.so lib/Math.c

lib/Prime.so: lib/Prime.c
	$(CC) -o lib/Prime.so lib/Prime.c

lib/String.so: lib/String.c
	$(CC) -o lib/String.so lib/String.c

lib/Sys.so: lib/Sys_linux.c
	$(CC) -o lib/Sys.so lib/Sys_linux.c

lib/Time.so: lib/Time.c
	$(CC) -o lib/Time.so lib/Time.c

lib/File.so: lib/File.c
	$(CC) -o lib/File.so lib/File.c

clean: all
	$(RM) *.so
