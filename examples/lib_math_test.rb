require "Math"

def main
	puts "fact(10) = " Math_fact 10
	puts "lcm(3, 4) = " Math_lcm 3 4
	max = 100
	table = Array(max)
	max = Prime_table table max
	puts "primes between 2 to 100"
	for i in 0..max-1
		puts table[i]
	end
end

