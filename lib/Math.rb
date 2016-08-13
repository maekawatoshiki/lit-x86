require "util"
require "Time"

lib Math
	proto math_gcd a b
	proto math_lcm a b
	proto math_fact n
	proto math_even n
	proto math_odd n
	proto math_abs n
	proto math_rand 
	proto math_rand_set n
	proto math_log x:double :double
	proto math_exp x:double :double
	proto math_pow x:double y:double :double
end

lib Prime
	proto Prime_is p | prime_is 
end

module Math
	module Prime
		def is p
			prime_is p
		end
		def list maxsize:int :int[]
			prime_list = new maxsize int
			for i in 0...maxsize
				prime_list[i] = 0
			end
			for i = 2, i * i < maxsize, i += 1
				if prime_list[i] == 0
					for k = i * 2, k < maxsize, k += i
						if prime_list[k] == 0
							prime_list[k] = 1
						end
					end
				end
			end
			table = new 0 int
			for i in 2...maxsize
				if prime_list[i] == 0
					table += i
				end
			end
			table
		end
	end
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
	def even i:int64
		if i % 2 == 0
			1
		else
			0
		end
	end
	def odd i
		math_odd i
	end
	def odd i:int64
		if i % 2
			1
		else
			0
		end
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
	def floor f:double :double
		<double><int>f
	end
	def random 
		math_rand
	end
	def random_init n
		math_rand_set n
	end
	def random_init
		math_rand_set Time::time
	end
	def log x:double :double
		math_log x
	end
	def exp x:double :double
		math_exp x
	end
	def pow(a:double, b:double):double
		math_pow a b
	end
	def pow(b:int64, p:int64):int64
		pow b p 9223372036854775807
	end
	def pow(b:int64, p:int64, m:int64):int64
		res = <int64> 1
		while p > 0
			if p % 2 == 1
				res = (res * b) % m
			end
			b = (b * b) % m
			p /= 2
		end
		res
	end

	def sqrt f:double :double
		x = 1.0
		delta = 1.0
		eps = 0.000001
		while Math::abs(delta) > eps
			x = x - (x*x-f)/(2.0*x)
			delta = x * x - f
		end
		x
	end
end

