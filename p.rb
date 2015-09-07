# The prime number search program by maekawa toshiki.

def prime(i)
	isprime = 0
	if i == 2
		isprime = 1
	elsif i % 2 != 0
		isprime = 1
		for k = 3, k * k <= i, k = k + 2
			if i % k == 0
				isprime = 0
				break
			end
		end
	end
	isprime
end

for i = 2, i < 100, i = i + 1
	if prime(i) == 1
		puts i, " is prime"
	end
end
