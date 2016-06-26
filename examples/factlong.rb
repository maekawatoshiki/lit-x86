
def bigPrint(a:int[])
	for i = 100000 - 1, i >= 0, i -= 1
		if a[i]
			printf("%d", a[i])
		end
	end 
	puts ""
end

def bigSet(a:int[], b)
	c = b
	for i = 0, i < 100000, i += 1; a[i] = 0; end
	for i = 0, c, i += 1
		a[i] = c % 100000
		c = c / 100000
	end
end

def bigAdd(a:int[], b)
	c = 0 
	for i = 0, (i < 100000) | c, i += 1
		c = c + a[i] + b[i]
		a[i] = c % 100000
		c = c / 100000
	end
end

def bigMul(a:int[], b)
	c = 0
	for i = 0, (i < 100000) | c, i += 1
		c = c + a[i] * b
		a[i] = c % 100000
		c = c / 100000
	end
end


def bigFact(a:int[], max)
	bigSet(a, 1)
	for i = 1, i <= max, i += 1
		bigMul(a, i)
	end
	a
end

A = new 1000
bigPrint( bigFact(A, 20) )

