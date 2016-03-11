def say2(s1:string, s2:string)
	puts(s1, s2)
end

def fact(i)
	if i == 1
		1
	else
		fact(i - 1) * i
	end
end

def fibo(n)
	if n < 2
		1
	else
		fibo(n - 2) + fibo(n - 1)
	end
end


def main
	say2("hello", " world!")
	puts("fact(10) = ", fact(10))
	puts("fibo(30) = ", fibo(30))
	a = 1
	a += 2
	puts("a = ", a)
	a -= 1
	puts("a = ", a)
	puts("ret: ", undef_func())
	z:int[] = [1, 2, 3]
	# z[0] = 1
	# z[1] = 2
	puts(z[0])
	puts(z[1])
end


def undef_func
	sum = 0
	for i = 0, i < 10, i += 1
		sum += i
		if i >= 5
			break
		end
	end

	10000 + sum
end
