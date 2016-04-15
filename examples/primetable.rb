def table prime_list:int[] max
	count = max - 2
	for i in 0..max-1
		prime_list[i] = 0
	end
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
prime_list = new N int
puts "found " table(prime_list N) " primes between 2 and " N
