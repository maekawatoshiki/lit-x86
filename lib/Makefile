CFLAGS = -O1 -emit-llvm -c
CC = clang-3.5 $(CFLAGS) 

all: lib/Math.ll lib/Prime.ll  lib/Sys.ll lib/Time.ll lib/File.ll lib/String.ll

lib/Math.ll: lib/Math.c
	$(CC) -o lib/Math.ll lib/Math.c

lib/Prime.ll: lib/Prime.c
	$(CC) -o lib/Prime.ll lib/Prime.c

lib/Sys.ll: lib/Sys_linux.c
	$(CC) -o lib/Sys.ll lib/Sys_linux.c

lib/Time.ll: lib/Time.c
	$(CC) -o lib/Time.ll lib/Time.c

lib/File.ll: lib/File.c
	$(CC) -o lib/File.ll lib/File.c

lib/String.ll: lib/String.c
	$(CC) -o lib/String.ll lib/String.c

clean: all
	$(RM) *.ll
