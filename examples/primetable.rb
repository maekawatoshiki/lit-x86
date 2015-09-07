def table(a, max)
	for i = 0, i < max, i = i + 1
  	a[i] = 0
	end
	for i = 2, i * i < max, i = i + 1
	  if a[i] == 0
			for k = i * 2, k < max, k = k + i
	      a[k] = 1
	    end
	  end
	end
end

def isprime(a, n)
	if a[n] == 0
		1
	else
		0
	end
end

N = 100000
Array a[N]

table(a, N)

for i = 2, i < N, i = i + 1
  if isprime(a, i) == 1
    puts i
  end
end


