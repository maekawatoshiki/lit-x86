def table(a, max)
	for i = 0, i < max
  	a[i] = 0
  	i = i + 1
	end
	for i = 2, i * i < max
	  if a[i] == 0
	    k = i * 2
	    while k < max
	      a[k] = 1
	      k = k + i
	    end
	  end
	  i = i + 1
	end
end

def isprime(a, n)
	if a[n] == 0
		1
	else
		0
	end
end

N = 10000000
Array a[N]

table(a, N)

for i = 2, i < N
  if isprime(a, i) == 1
    puts i
  end
	i = i + 1
end


