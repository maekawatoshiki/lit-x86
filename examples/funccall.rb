module Calc
	def calc(x, y)
		x * y
	end
end

def one
	1
end

def calc x y
	x + y + one()
end

def no_use
	puts('A')
end

a = calc(1, 2)
a = a + calc(2, 3)
puts(a - Calc::calc(2, 3))
