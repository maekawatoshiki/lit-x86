CFLAGS = -lm -O1 -S -emit-llvm -c
CC = clang $(CFLAGS) 

all: lib/M.ll lib/Math.ll lib/Prime.ll lib/String.ll lib/Sys.ll lib/Time.ll lib/File.ll

lib/M.ll: lib/M.c
	$(CC) -o lib/M.ll lib/M.c

lib/Math.ll: lib/Math.c
	$(CC) -o lib/Math.ll lib/Math.c

lib/Prime.ll: lib/Prime.c
	$(CC) -o lib/Prime.ll lib/Prime.c

lib/String.ll: lib/String.c
	$(CC) -o lib/String.ll lib/String.c

lib/Sys.ll: lib/Sys_linux.c
	$(CC) -o lib/Sys.ll lib/Sys_linux.c

lib/Time.ll: lib/Time.c
	$(CC) -o lib/Time.ll lib/Time.c

lib/File.ll: lib/File.c
	$(CC) -o lib/File.ll lib/File.c

clean: all
	$(RM) *.ll
