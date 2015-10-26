def add(a, b)
	puts "a = ", a, " , b = ", b
end

module Math
	$PI = 3141592
	def add(a, b)
		a + b
	end
	def sub(a, b)
		a - b
	end
	
	def fact(a)
		if a < 2
			1
		else
			fact(a - 1) * a
		end
	end
endmodule

puts Math:add(1, 2)
puts Math:PI

add(1, 2)

puts Math:fact(10)

