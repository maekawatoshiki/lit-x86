#!/bin/bash

function check() {
	if [[ $($1) = $($2) ]] ; then
		printf "$1 ... \e[32mOK\e[00m\n"
		return 0
	else
		printf "$1 ... \e[31mNO\e[00m\n"
		return 1
	fi
}

check "./lit ./test/io.rb" "cat ./test/io" &
pid1=$!
check './lit ./examples/fibo.rb' 'echo 165580141' &
pid2=$!
check './lit ./examples/pi.rb' 'cat test/pi' &
pid3=$!
check './lit ./examples/primetable.rb' "cat ./test/primetable" &
pid4=$!
check './lit ./examples/vararg.rb' 'echo 31' &
pid5=$!
check './lit ./examples/funccall.rb' 'echo 4' &
pid6=$!
check './lit ./examples/math.rb' 'cat test/math' &
pid7=$!
check './lit ./test/require.rb' 'echo 2' &
pid8=$!
check './lit ./examples/array.rb' 'echo 12345' &
pid9=$!
check './lit ./test/char.rb' 'echo Aello' &
pid10=$!
check './lit ./test/cast.rb' 'echo A2' &
pid11=$!
check './lit ./test/proto.rb' 'echo 2' &
pid12=$!
status_sum=0
wait $pid1; status_sum=`expr $status_sum + $?`
wait $pid2; status_sum=`expr $status_sum + $?`
wait $pid3; status_sum=`expr $status_sum + $?`
wait $pid4; status_sum=`expr $status_sum + $?`
wait $pid5; status_sum=`expr $status_sum + $?`
wait $pid6; status_sum=`expr $status_sum + $?`
wait $pid7; status_sum=`expr $status_sum + $?`
wait $pid8; status_sum=`expr $status_sum + $?`
wait $pid9; status_sum=`expr $status_sum + $?`
wait $pid10; status_sum=`expr $status_sum + $?`
wait $pid11; status_sum=`expr $status_sum + $?`
wait $pid12; status_sum=`expr $status_sum + $?`
exit $status_sum
