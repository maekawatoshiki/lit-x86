def fibo(n, ref memo:int[])
	if n < 2
		1
	elsif memo[n] != 0
		memo[n]
	else 
		memo[n] = fibo(n - 1, memo) + fibo(n - 2, memo)
	end
end

n = 46
memo = new int n
fibo n, memo
for i in 0...n
	puts memo[i]
end
