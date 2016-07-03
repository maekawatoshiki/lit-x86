#!/bin/bash

function check() {
	if [[ $($1) = $($2) ]] ; then
		printf "$1 ... \e[32mOK\e[00m\n"
	else
		printf "$1 ... \e[31mNO\e[00m\n"
	fi
}

check './lit ./test/io.rb' 'cat ./test/io' &
check './lit ./examples/fibo.rb' 'echo 165580141' &
check './lit ./examples/pi.rb' 'cat test/pi' &
check './lit ./examples/primetable.rb' "cat ./test/primetable" &
check './lit ./examples/vararg.rb' 'echo 31' &
check './lit ./examples/funccall.rb' 'echo 4' &
check './lit ./examples/math.rb' 'cat test/math' &
check './lit ./test/require.rb' 'echo 2' &
check './lit ./examples/array.rb' 'echo 12345' &
wait
exit 0
