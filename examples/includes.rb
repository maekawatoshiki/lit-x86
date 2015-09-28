$NULL = 0

# String functions

def strlen(d:string)
  for len = 0, d[len] != 0, len++; end
  len
end

def strcpy(d:string, s:string)
  i = 0
  while s[i] != 0
    d[i] = s[i++]
  end
end

def strcat(d:string, s:string)
  i = 0
  len = strlen(d)
  j = len
  while s[i] != 0
    d[j++] = s[i++]
  end
end

def strcmp(a:string, b:string)
	diff = 0
	a_len = strlen(a)
	for i = 0, i < a_len, i++
		diff = diff + a[i] - b[i]
	end
	diff
end

# util

def isdigit(n)
  if '0' <= n; if n <= '9'
      return 1
  end end
  0
end

def isalpha(c)
	if 'A' <= c; if c <= 'Z'
		return 1
	end end
	if 'a' <= c; if c <= 'z'
		return 1
	end end
	0
end

def atoi(s:string)
  sum = 0; n = 1
  for l = 0, isdigit(s[l]) == 1, l++; n = n * 10; end

  for i = 0, i < l, i++
    n = n / 10
    sum = sum + n * (s[i] - '0')
  end
  sum
end

# Memory

def memset(mem:string, n, byte)
	for i = 0, i < byte, i++
		mem[i] = n
	end
end

def memcpy(m1:string, m2:string, byte)
	for i = 0, i < byte, i++
		m1[i] = m2[i]
	end
end

# Math

def pow(a, b)
	c = a; b--
	while b-- > 0
		a = a * c
	end
	a
end

def abs(a)
	if a < 0; return 0 - a; end
	a
end

hello:string = Array(10)

strcpy(hello, "Hello")
puts strcmp(hello, "world")
puts strcmp("abc", "abc")

puts abs(12 - 20), " ", abs(10)

