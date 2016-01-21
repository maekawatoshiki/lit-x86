$LEN = 10000
$MAX = 10000

def bigPrint(a:int[])
	for i = LEN - 1, i >= 0, i--
		if a[i]
			printf "%d", a[i]
		end
	end 
	puts ""
end

def bigSet(a:int[], b)
	c = b
	for i = 0, i < LEN, i++; a[i] = 0; end
	for i = 0, c, i++
		a[i] = c % MAX
		c = c / MAX
	end
end

def bigAdd(a:int[], b)
	c = 0 
	for i = 0, i < LEN or c, i++
		c = c + a[i] + b[i]
		a[i] = c % MAX
		c = c / MAX
	end
end

def bigMul(a:int[], b)
	c = 0
	for i = 0, i < LEN or c, i++
		c = c + a[i] * b
		a[i] = c % MAX
		c = c / MAX
	end
end


def bigFact(a:int[], max)
	bigSet(a, 1)
	for i = 1, i <= max, i++
		bigMul(a, i)
	end
	a
end

A:int[] = Array(LEN)
bigPrint( bigFact(A, 100) )

