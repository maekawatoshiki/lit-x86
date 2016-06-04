require "std"

def mod_pow b p m
	res = 1
	while p > 0
		if math_odd p
			res = (res * b) % m
		end
		b = (b * b) % m
		p /= 2
	end
	res
end

def prime n
	if n < 2
		return 0
	elsif n == 2
		return 1
	elsif math_even n
		return 0
	end

	d = n - 1
	while math_even d
		d /= 2
	end

	for q in 0...30
		a = (random % (n - 2)) + 1
		t = d
		y = mod_pow a t n
		while (t != n - 1) & (y != 1) & (y != n - 1)
			y = (y * y) % n
			t *= 2
		end
		if (y != n - 1) & math_even d
			return 0
		end
	end
	1
end

isp = 0
while isp < 10
	r = random % 65536
	if prime r
		puts r " is prime"
		isp += 1
	end
end

