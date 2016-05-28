lib Math
	proto math_gcd a b | gcd
	proto math_lcm a b | lcm
	proto math_fact n | fact
	proto math_even n | even
	proto math_odd n | odd
	proto math_abs n | abs
	proto math_rand | random
end

lib Prime
	proto Prime_is p | prime_is 
	proto Prime_table tbl:int[] max | prime_table
end

