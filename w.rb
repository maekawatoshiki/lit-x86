def main
	a = 1
	while a < 20
		if a % 15 == 0
			puts("FizzBuzz")
		else if a % 5 == 0
			puts("Buzz")
		else if a % 3 == 0
			puts("Fizz")
		else
			# if a % 5 == 0
			# 	puts("Buzz")
			# else
			# 	if a % 3 == 0
			# 		puts("Fizz")
			# 	else 
			# 		puts(a)
			# 	end
			# end
			puts(a)
		end
		a = a + 1
	end
end

