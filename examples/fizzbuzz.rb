def fizzbuzz i
	if i % 15 == 0
		puts "FizzBuzz" 
	elsif i % 5 == 0
		puts "Buzz" 
	elsif i % 3 == 0
		puts "Fizz" 
	else
		puts i 
	end
end

for i in 1...20
	i.fizzbuzz()
end
