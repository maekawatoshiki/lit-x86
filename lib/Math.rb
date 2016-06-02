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

