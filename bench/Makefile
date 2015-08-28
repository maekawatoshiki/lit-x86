fibo:
	gcc -o fibo test.c
	javac test.java
	luac test.lua

time:
	
	time -p ./fibo > /dev/null

	time -p java fibo > /dev/null

	time -p ruby test.rb > /dev/null

	time -p python test.py > /dev/null

	time -p lua test.lua > /dev/null

	time -p lua luac.out > /dev/null

	time -p ../lit test.rb > /dev/null

clean:
	$(RM) fibo fibo.class luac.out
