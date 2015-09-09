def dfs(a, size, sum, k, i)
	if i == size
		if k == sum
			1
		else
			0
		end
	else
		if dfs(a, size, sum, k, i + 1) == 1
			1
		elsif dfs(a, size, sum, k + a[i], i + 1) == 1
			1
		else 
			0
		end
	end
end

Array a[5]

for i = 0, i < 5, i = i + 1
	k = rand() % 20
	for j = 0, j < 5, j = j + 1
		a[j] = rand() % 8
		output a[j], " "
	end puts " sum = ", k
	puts dfs(a, 5, k, 0, 0)
end

