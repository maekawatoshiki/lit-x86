require "std"

def prime_factorization(x):string
	output = ""
	while x >= 4 & math_even x
		output += "2,"
		x /= 2
	end
	d = 3
	q = x / d
	while q >= d 
		if x % d == 0 
			output += int_to_str(d) + ","
			x = q
		else 
			d += 2
		end
		q = x / d
	end
	output += int_to_str x
	output
end

n = str_to_int gets
puts "prime_factorization(" n ") = " prime_factorization n
