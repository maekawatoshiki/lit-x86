require "std"

def prime n
	if n < 2
		return 0
	elsif n == 2
		return 1
	elsif Math::even( n )
		return 0
	end

	d = n - 1
	while Math::even( d )
		d /= 2
	end

	for q in 0...30
		a = (Math::random() % (n - 2)) + 1
		t = d
		y = Math::pow( <int64>a, <int64>t, <int64>n )
		while (t != n - 1) & (y != 1) & (y != n - 1)
			y = (y * y) % n
			t *= 2
		end
		if (y != n - 1) & Math::even( d )
			return 0
		end
	end
	1
end

Math::random_init()
isp = 0
while isp < 1000000
	r = (Math::random() % 2100000000)
	if r.prime()
		puts(r)
		isp += 1
	end
end

