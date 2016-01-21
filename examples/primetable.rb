$prime_list:int[] = 0

def table(max)
	count = max - 2
	for i = 0, i < max, i++
		prime_list[i] = 0
	end
	for i = 2, i * i < max, i++
		if prime_list[i] == 0
			for k = i * 2, k < max, k = k + i
				if prime_list[k] == 0
					prime_list[k] = 1
					count--
				end
			end
		end
	end
	count
end

def isprime(n)
	if prime_list[n]
		0
	else
		1
	end
end

N = 10000000
prime_list = Array(N)

puts table(N)

# for i = 2, i < N, i++
#   if isprime(i) == 1
#     puts i, " is prime"
#   end
# end
