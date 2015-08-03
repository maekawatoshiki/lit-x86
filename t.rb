i = 2
isprime = 1
while i < 100000
	k = 2
	isprime = 1
	while k * k <= i
		if i % k == 0
			isprime = 0
			break
		end
		k += 1
	end
	puts i if isprime == 1
	i += 1
end
