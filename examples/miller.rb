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
	if n == 2
		1
	elsif n % 2 == 0
		0
	else
		isp = 0
		d = n - 1
		s = 0
		while d % 2 == 0
			d = d / 2
			s = s + 1
		end
		for q = 0, q < 30, q = q + 1
			isp = 0
			a = (rand() % (n - 2)) + 1
			y = modPow(a, d, n)
			if y == 1 isp = 1 end
			if y == n - 1 isp = 1 end
			
			for j = 0, j < s, j = j + 1
				y = modPow(y, 2, n)
				if y == n - 1 isp = 1 break end
			end
			if isp == 0 break end
		end
		isp
	end
end

isp = 0
while isp == 0
	r = rand() % 65536
	isp = 0
	puts r, " = ", (isp=prime(r))
end


