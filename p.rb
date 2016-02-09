# The prime number search program by maekawa toshiki.

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

for i = 2, i < 100, i++
	if prime(i)
		puts i, " is prime"
	end
end


