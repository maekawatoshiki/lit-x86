def table max
	count = max - 2
	prime_list = new int max
	for i = 2, i * i < max, i += 1
		if prime_list[i] == 0
			for k = i * 2, k < max, k += i
				if prime_list[k] == 0
					prime_list[k] = 1
					count = count - 1
				end
			end
		end
	end
	count
end

N = 10000000
puts("found ", table(N), " primes between 2 and ", N)
