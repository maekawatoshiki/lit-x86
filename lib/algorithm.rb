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
		qsort a left l-1
	end
	if r + 1 < right
		qsort a r+1 right
	end
end

def qsort(a:string[] left right):string[]
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
		a = qsort a left l-1
	end
	if r + 1 < right
		a = qsort a r+1 right
	end
	a
end

def sort a:string[] :string[]
	qsort a 0 builtinlength(a)-1
end

def sort a:int[] :int[]
	qsort a 0 builtinlength(a)-1
	a
end
