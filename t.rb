i = 2
while i < 100000
	if i == 2
		puts i.to_s + " is prime"
	elsif i % 2 != 0
		k = 3
		isprime = 1
		while k * k <= i
			if i % k == 0
				isprime = 0
				break
			end
			k += 2
		end
		puts i.to_s + " is prime" if isprime == 1
	end
	i += 1
end
