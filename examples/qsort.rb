def qsort(a, left, right)
	l = left; r = right
	pv = l
	while 1 == 1
		while a[l] < a[pv]
			l = l + 1
		end
		while a[pv] < a[r]
			r = r - 1
		end
		if l >= r break end
		t = a[l]; a[l] = a[r]; a[r] = t;
		l = l + 1; r = r - 1
	end
	if left < l - 1
		qsort(a, left, l - 1)
	end
	if r + 1 < right
		qsort(a, r + 1, right)
	end
end

max = 20
a = Array(max)
for i = 0, i < max, i = i + 1
	a[i] = rand() % 20
end
qsort(a, 0, max - 1)

for i = 0, i < max, i = i + 1
	puts a[i]
end

