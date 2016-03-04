# The prime number search program by maekawa toshiki.

def prime(i)
	if i == 2 
		return 1
	end
	if i % 2 == 0
		return 0
	end

	for k = 3, k * k <= i, k += 2
		if i % k == 0
			return 0
		end
	end
	1
end

def main
	for i = 2, i < 100, i += 1
		if prime(i)
			puts(i, " is prime")
		end
	end
end

