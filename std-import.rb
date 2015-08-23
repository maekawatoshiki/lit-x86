def gcd(a, b)
	ret = 0
	if a == 0 ret = 0
	elsif b == 0 ret = 0 else
		ret = 0
		while a != b
			if a > b a = a - b
			else b = b - a end
		end
		ret = a
	end
return ret

def main 
	puts gcd(2, 5) # 1
	puts gcd(6, 8) # 2
return
