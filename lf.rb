# the prime number search program by maekawa toshiki.

def isprime(i)
	ret = 0
		k = 2
		ret = 1
		while k * k < i
			if i % k == 0
				ret = 0
				break
			end
			k = k + 1
		end
return ret

def main
	i = 2
	while i < 10
		if isprime(i) == 1
			puts i, " is prime"
		end

		i = i + 1
	end
return 0
