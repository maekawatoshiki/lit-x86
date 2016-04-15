def fibo n memo:int[]
	if n == 0 or n == 1
		n
	elsif memo[n] != 0
		memo[n]
	else 
		memo[n] = fibo(n - 1 memo) + fibo(n - 2 memo)
	end
end

n = 46
memo = new n int
puts "fibo(" n ") = " fibo n memo

