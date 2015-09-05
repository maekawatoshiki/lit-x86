# The prime number search program by maekawa toshiki.

def prime(i)
	isprime = 0
	if i == 2
		isprime = 1
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
	end
	isprime
end


i = 2
while i < 20000
	if prime(i) == 1
		puts i, " is prime!"
	end
	i = i + 1
end
