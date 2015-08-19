def gcd(x, y)
	while x != y
		if x > y x = x - y
		else y = y - x end
	end
return x

def lcm(x, y)
return ((y / gcd(x, y)) * x)

def fibo(n)
	a = 0
	b = 1
	i = 0
	ret = 0
	while i < n
		ret = a + b
		a = b
		b = ret
		i = i + 1
	end
return ret

def main
	puts "lcm = ", lcm(20, 3)
	i = 1
	while i < 30
		puts "fibo", i, " = ", fibo(i)
		i = i + 1
	end
return 0
