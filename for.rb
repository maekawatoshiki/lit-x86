def gcd(x, y)
	while x != y
		if x > y 
			x = x - y
		else 
			y = y - x 
		end
	end
	x + 1 - 1
end

def main
	for x = 1, x < 1000
		for y = 1, y < 1000
			puts "gcd(", x, ", ", y, ") = ", gcd(x, y)
			y = y + 1
		end
		x = x + 1
	end
end
