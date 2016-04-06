lib Math
	proto Math_gcd a b
	proto Math_fact n
	proto Math_even n
	proto Math_odd n
	proto Math_abs n
	proto Math_lcm a b
	proto Math_rand 
end

def normal x
	puts x
end

def main
	puts Math_gcd 6, 4
	puts Math_fact 10
	puts Math_even 7
	puts Math_odd 7
	puts Math_abs 10 - 20
	puts Math_lcm 2 3
	puts Math_rand
	normal 1223 
end
