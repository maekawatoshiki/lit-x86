module Calc
	def calc(x, y)
		x * y
	end
end

def calc(x, y)
	x + y + one()
end

def one
	1
end

a = calc 1, 2
a = a + calc 2, 3
puts a - Calc.calc(2, 5) + 1
