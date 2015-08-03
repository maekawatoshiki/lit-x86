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
		k = k + 1
	end
	if isprime == 1
		print i
	end
	i = i + 1
end
