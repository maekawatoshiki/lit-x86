def sum(x, y)
	for total = 0, x < y, x++
		total = total + x
	end
	total
end

def prime(i)
	if i == 2 
	end
	if i % 2 == 0
	end

	for k = 3, k * k <= i, k = k + 2
		if i % k == 0
		end
	end
	1
end

def main
	for i = 2, i < 100, i++
		if prime(i)
			put(i, " is prime")
		end
	end
end

