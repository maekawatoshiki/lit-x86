require "std"

def prime_factorization(x):string
	output = ""
	while x >= 4 & Math::even(x)
		output += "2,"
		x /= 2
	end
	d = 3
	q = x / d
	while q >= d 
		if x % d == 0 
			output += d.to_string() + ","
			x = q
		else 
			d += 2
		end
		q = x / d
	end
	output += x.to_string()
	output
end

n = gets().to_int()
puts("prime_factorization(", n, ") = ", prime_factorization(n))
