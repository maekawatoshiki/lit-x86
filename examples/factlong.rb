
def bigPrint(a:int[])
	bgn = 0
	for i = $max - 1, i >= 0, i -= 1
		if a[i] != 0
			bgn = i
			break
		end
	end

	for i = bgn, i >= 0, i -= 1
		printf "%d", a[i]
	end 
	puts
end

def bigSet(a:int[], b)
	c = b
	for i = 0, c, i += 1
		a[i] = c % 100000
		c = c / 100000
	end
end

# def bigAdd(a:int[], b)
# 	c = b
# 	for i = 0, c != 0, i += 1
# 		c += a[i]
# 		a[i] = c % 100000
# 		c = c / 100000
# 	end
# end

def bigMul(a:int[], b)
	c = 0
	for i = 0, i < 100000, i += 1
		c += a[i] * b
		a[i] = c % 100000
		c /= 100000
	end
end


def bigFact(a:int[], max):int[]
	bigSet(a, 1)
	for i in 1..max
		bigMul(a, i)
	end
	a
end

$max = 100000
A = new $max
bigPrint( bigFact(A, 200) )

