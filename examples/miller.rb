def modPow(b, p, m)
	res = 1
	while p > 0	
		if p % 2 == 1
			res = (res * b) % m
		end
		b = (b * b) % m
		p = p / 2
	end
	res
end

def prime(n)
	if n < 2
		return 0
	elsif n == 2
		return 1
	elsif n % 2 == 0
		return 0
	end

	d = n - 1
	s = 0
	while d % 2 == 0; d = d / 2; s++ end
	for q = 0, q < 30, q++
		a = (rand() % (n - 2)) + 1
		y = modPow(a, d, n)
		if y == 1 return 1 end
		if y == n - 1 return 1 end
		
		for j = 0, j < s, j++
			y = modPow(y, 2, n)
			if y == n - 1 
				return 1
			end
		end
		return 0
	end
end

isp = 0
while isp < 10000
	r = rand() % 60000
	if prime(r) == 1
		puts r, " is prime"
		isp++
	end
end

