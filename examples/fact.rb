def fact(n)
	if n == 1
		1
	else
		fact(n - 1) * n
	end
end

puts(fact(10))


