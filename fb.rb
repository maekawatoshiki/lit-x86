def main
	i = 1
	while i < 20
		if i % 15 == 0
			puts("FizzBuzz")
		elsif i % 5 == 0
			puts("Buzz")
		elsif i % 3 == 0
			puts("Fizz")
		else
			puts(i)
		end
		i = i + 1
	end
	puts("yeih!")
end

