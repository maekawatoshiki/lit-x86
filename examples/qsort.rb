require "Math"

def qsort a:int[] left right
	l = left; r = right
	pv = l
	while 1
		while a[l] < a[pv]; l += 1; end
		while a[pv] < a[r]; r -= 1; end
		if l >= r 
			break	
		end
		t = a[l]; a[l] = a[r]; a[r] = t;
		l += 1; r -= 1
	end
	if left < l - 1
		qsort(a left l-1)
	end
	if r + 1 < right
		qsort(a r+1 right)
	end
end

max = 5
a = new int max

for i in 0..max
	a[i] = Math::random() % 100
end

puts(a)

qsort(a 0 max-1)

puts(a)
