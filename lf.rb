def gcd(x, y)
	while x != y
		if x > y x = x - y
		else y = y - x end
	end
return x

def lcm(x, y) return ((y / gcd(x, y)) * x)

def fibo(n)
	if n < 2 ret = n
	else ret = fibo(n - 1) + fibo(n - 2) end
return ret

def main
	puts "lcm = ", lcm(20, 3)
	i = 1
	while i < 30
		puts "fibo", i, " = ", fibo(i)
		i = i + 1
	end
return 0
