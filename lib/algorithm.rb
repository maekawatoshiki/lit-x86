def template qsort(a left right):T
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
		a = qsort(a left l-1)
	end
	if r + 1 < right
		a = qsort(a r+1 right)
	end
	a
end

def template sort(a):T
	qsort(a 0 builtinlength(a)-1)
	a
end
