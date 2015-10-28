$NULL = 0

# String functions

module String
	def length(d:string)
		for len = 0, d[len] != 0, len++; end
		len
	end

	def copy(d:string, s:string)
		i = 0
		while s[i]
			d[i] = s[i++]
		end
	end

	def cat(d:string, s:string)
		i = 0
		len = length(d)
		j = len
		while s[i]
			d[j++] = s[i++]
		end
	end

	def compare(a:string, b:string)
		diff = 0
		a_len = length(a)
		for i = 0, i < a_len, i++
			diff = diff + a[i] - b[i]
		end
		diff
	end

	def isdigit(n)
		if '0' <= n and n <= '9'
			return 1
		end
		0
	end

	def isalpha(c)
		if 'A' <= c and c <= 'Z'
			return 1
		end
		if 'a' <= c and c <= 'z'
			return 1
		end
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
end

# Memory
module Memory
	def set(mem:string, n, byte)
		for i = 0, i < byte, i++
			mem[i] = n
		end
	end

	def copy(m1:string, m2:string, byte)
		for i = 0, i < byte, i++
			m1[i] = m2[i]
		end
	end
end

# Math

module Math 
	def pow(a, b)
		c = a; b--
		while b-- > 0
			a = a * c
		end
		a
	end

	def abs(a)
		if a < 0; 0 - a else a end
	end

	def fact(n)
		for ret = n--, n > 0, n--
			ret = ret * n
		end
		ret
	end
end

# Secure

def SecureRandomString(str:string, len)
	stdin = fopen("/dev/urandom", "rb")
	
	bytes = 128
	data:string = Array(bytes)
	fgets(data, bytes, stdin)
	chars = 0
	for i = 0, i < bytes and chars < len, i++
		if String.isalpha(data[i]) or String.isdigit(data[i])
			str[chars++] = data[i]
		else
			fgets(data, bytes, stdin)
			i--
		end
	end
	str[chars] = 0
end

# I/O

module IO
	def input(str:string)
		f = fopen("/dev/stdin", "w+")
		fgets(str, 100, f)
		fclose(f)
		str
	end
end

# Main 

buf:string = Array(32)
IO.input(buf)
len = String.atoi(buf)

for i = 0, i < 8, i++
	SecureRandomString(buf, len)
	printf "%s\n", buf
end

