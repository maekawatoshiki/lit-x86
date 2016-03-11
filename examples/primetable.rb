def table(prime_list:int[], max)
	count = max - 2
	for i = 0, i < max, i = i + 1
		prime_list[i] = 0
	end
	for i = 2, i * i < max, i = i + 1
		if prime_list[i] == 0
			for k = i * 2, k < max, k = k + i
				if prime_list[k] == 0
					prime_list[k] = 1
					count = count - 1
				end
			end
		end
	end
	count
end

def main
	N = 10000000
	prime_list = Array(N)
	puts("found ", table(prime_list, N), " primes between 2 and ", N)

	# for i = 2, i < N, i = i + 1
	# 	if prime_list[i] == 0
	# 		puts(i, " is prime")
	# 	end
	# end
end
