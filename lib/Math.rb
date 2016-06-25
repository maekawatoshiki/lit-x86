lib Math
proto math_gcd a b
proto math_lcm a b
proto math_fact n
proto math_even n
proto math_odd n
proto math_abs n
proto math_rand 
end

lib Prime
proto Prime_is p | prime_is 
proto Prime_table tbl:int[] max | prime_table
end

module Math
	def gcd a b
		math_gcd a b
	end
	def lcm a b
		math_lcm a b
	end
	def factorial i
		math_fact i
	end
	def even i
		math_even i
	end
	def odd i
		math_odd i
	end
	def abs i
		math_abs i
	end
	def abs f:double :double
		if f < 0.0
			0.0 - f
		else
			f
		end
	end
	def random 
		math_rand
	end

	def sqrt f:double :double
		x = 1.0
		delta = 1.0
		eps = 0.0001
		while Math::abs(delta) > eps
			x = x - (x*x-f)/(2.0*x)
			delta = x * x - f
		end
		x
	end
end

