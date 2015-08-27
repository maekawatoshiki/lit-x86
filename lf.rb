def gcd(x, y)
	while x != y
		if x > y
			x = x - y
		else
			y = y - x
		end
	end
	x
end

def lcm(x, y)
	(x / gcd(x, y)) * y
end

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
	ret
end

def collatz(n)
	output n, " "
	if n % 2 == 0
		if n > 1 collatz(n / 2) end
	else
		if n > 1 collatz(3 * n + 1) end
	end
end

def fact_rec(n)
	if n < 2
		1
	else
		fact_rec(n - 1) * n
	end
end


def htest(x, y)
	puts "x = ", x
	puts "y = ", y
end


puts "lcm = ", lcm(13, 3)
i = 1
while i <= 40
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
	puts "collatz", i, " = ", collatz(i)
	i = i + 1
end
i = 1
puts "i = i + 10 => ", (i = i + 10)
puts "i = ", i
htest(10, 20)