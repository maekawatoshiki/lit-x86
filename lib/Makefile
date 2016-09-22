CFLAGS = -O1 -emit-llvm -c
CC = clang-3.5 $(CFLAGS) 

all: lib/Math.bc lib/Prime.bc  lib/Sys.bc lib/Time.bc lib/File.bc lib/String.bc

lib/Math.bc: lib/Math.c
	$(CC) -o lib/Math.bc lib/Math.c

lib/Prime.bc: lib/Prime.c
	$(CC) -o lib/Prime.bc lib/Prime.c

lib/Sys.bc: lib/Sys_linux.c
	$(CC) -o lib/Sys.bc lib/Sys_linux.c

lib/Time.bc: lib/Time.c
	$(CC) -o lib/Time.bc lib/Time.c

lib/File.bc: lib/File.c
	$(CC) -o lib/File.bc lib/File.c

lib/String.bc: lib/String.c
	$(CC) -o lib/String.bc lib/String.c

clean: all
	$(RM) *.bc
