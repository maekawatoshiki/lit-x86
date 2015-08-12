! prime[1000]
MAX = 1000

i = 0
while i < MAX
	prime[i] = 1
	i = i + 1
end
prime[0] = 0

i = 2
while i * i < MAX
	if prime[i] == 1
		j = i
		while j * i <= MAX
			prime[j * i] = 0
			j = j + 1
		end
	end
	i = i + 1
end

i = 2
while i < MAX
	if prime[i] == 1
		puts i, " is prime"
	end
	i = i + 1
end
