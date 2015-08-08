i = 2
while i < 1000000
	isprime = 1
	if i == 2
		print i
	elsif i % 2 != 0
		k = 3
		while k * k <= i
			if i % k == 0
				isprime = 0
				break
			end
			k = k + 2
		end
		if isprime == 1
			print i
		end
	end
	i = i + 1
end
