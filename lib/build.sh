#!/bin/bash

clang -shared -m32 -lm -O0 -o lib/Sys.so 		lib/Sys_linux.c &
clang -shared -m32 -lm -O0 -o lib/Prime.so 	lib/Prime.c &
clang -shared -m32 -lm -O0 -o lib/M.so 			lib/M.c &
clang -shared -m32 -lm -O0 -o lib/Math.so 	lib/Math.c &
clang -shared -m32 -lm -O0 -o lib/String.so lib/String.c &
clang -shared -m32 -lm -O0 -o lib/Time.so 	lib/Time.c &
wait
