def gcd(x, y)
	while x != y
		if x > y x = x - y
		else y = y - x end
	end
return x

def lcm(x, y) return ((y / gcd(y, x)) * x)

def fibo(n)
	a = 0
	b = 1
	i = 0
	ret = 0
	while i < n
		ret = a + b
		a = b b = ret
		i = i + 1
	end
return ret

def collatz(n)
	output n, " "
	if n % 2 == 0
		if n > 1 collatz(n / 2) end
	else
		if n > 1 collatz(3 * n + 1) end
	end
return 1

def fact_rec(n)
	if n < 2
		ret = 1
	else
		ret = fact_rec(n - 1) * n
	end
return ret

def rand(n)
	ret = n * 143 + 123
	a = ret + ret * 217 / n
	ret = (ret - n) * ret + n + a
	ret = ret % 10000000
return ret

def main
	puts "lcm = ", lcm(20, 3)
	i = 1
	while i < 30
		puts "fibo", i, " = ", fibo(i)
		i = i + 1
	end
	i = 1
	while i < 10
		puts "fact", i, " = ", fact_rec(i)
		i = i + 1
	end
	i = 2
	while i <= 7
		puts "collatz", i, " = ", collatz(i);
		i = i + 1
	end
	puts "50 - 23 = ", 50 - 23
return 0
