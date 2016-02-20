def main
	i = 0
	while i < 10
		puts(i)
		if i >= 5
			puts("break")
			break
		end
		i = i + 1
	end
	puts("success")
end

