def max(a, b)
	if a < b
		b
	else
		a
	end
end

def dfs(a, size, sum, k, i)
	if i == size
		sum
	elsif sum + a[i] > k
		dfs(a, size, sum, k, i + 1)
	else
		max(dfs(a, size, sum, k, i + 1), dfs(a, size, sum + a[i], k, i + 1))
	end
end

N = 20
a = Array(N)

for i = 0, i < N, i = i + 1
	k = rand() % (N * 3)
	for j = 0, j < N, j = j + 1
		a[j] = rand() % N
		output a[j], " "
	end puts " sum = ", k
	if dfs(a, N, 0, k, 0) == k
		puts "true"
	else
		puts "false"
	end
end
