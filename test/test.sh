#!/bin/bash

function check() {
	if [[ $($1) = $($2) ]] ; then
		echo -e "$1 ... \e[32mOK\e[00m"
	else
		echo -e "$1 ... \e[31mNO\e[00m"
	fi
}

check './lit ./examples/fibo.rb' 'echo 1346269'
check './lit ./examples/pi.rb' 'cat test/pi'
check './lit ./examples/factlong.rb' 'cat test/long_factorial'

