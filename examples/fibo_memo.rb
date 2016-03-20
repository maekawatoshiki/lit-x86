def fibo(n, memo:int[])
	if n == 0
		0
	elsif n == 1
		1
	elsif memo[n] != 0
		memo[n]
	else 
		memo[n] = fibo(n - 1, memo) + fibo(n - 2, memo)
	end
end

def main
	n = 46
	memo:int[] = Array(n)
	puts("fibo(", n, ") = ", fibo(n, memo))
end

