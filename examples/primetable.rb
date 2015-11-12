$prime_list = 0

def table(max)
	for i = 0, i < max, i++
		prime_list[i] = 0
	end
	for i = 2, i * i < max, i++
		if prime_list[i] == 0
			for k = i * 2, k < max, k = k + i
				prime_list[k] = 1
			end
		end
	end
end

def isprime(n)
	if prime_list[n]
		0
	else
		1
	end
end

N = 1000000
prime_list = Array(N)

table(N)

for i = 2, i < N, i++
  if isprime(i) == 1
    puts i, " is prime"
  end
end
