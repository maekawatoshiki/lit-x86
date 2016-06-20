require "std"

def abs f:double :double
	if f < 0
		0.0 - f
	else
		f
	end
end

def sqrt f:double :double
	x = 1.0
	delta = 1.0
	eps = 0.0001
	while abs(delta) > eps
		x = x - (x*x-f)/(2.0*x)
		delta = x * x - f
	end
	x
end

float = to_float gets
puts "sqrt(" float ") = " sqrt float
