# The prime number search program by maekawa toshiki.

def main
	i = 2
	while i < 100000
		if i == 2 puts i, " is prime"
		elsif i % 2 != 0
			k = 3
			isprime = 1
			while k * k <= i
				if i % k == 0
					isprime = 0
					break
				end
				k = k + 2
			end
			if isprime == 1
				puts i, " is prime"
			end
		end
		i = i + 1
	end
return
