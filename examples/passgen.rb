require String
$NULL = 0

def ishex(c)
	if 'A' <= c and c <= 'F'
		return 1
	end
	if 'a' <= c and c <= 'f'
		return 1
	end
	String:isdigit(c)
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

	def max(a, b)
		if a < b
			b
		else
			a
		end
	end

	def min(a, b)
		if a < b
			a
		else
			b
		end
	end
end

# Secure

module SecureRandom
	def hex(str:string, len)
		gen = File:open("/dev/urandom", "rb")

		bytes = 128
		data:string = Array(bytes)

		File:read(data, bytes, gen)
		chars = 0
		for i = 0, i < bytes and chars < len, i++
			if ishex(data[i])
				str[chars++] = data[i]
			else
				File:read(data, bytes, gen)
				i--
			end
		end
		str[chars] = 0
		File:close(gen)
		str
	end
end
# I/O

module IO
	def input(str:string, len)
		f = File:open("/dev/stdin", "w+")
		File:gets(str, len, f)
		File:close(f)
		str
	end
end

# Main

buf:string = Array(256)

print "input password length: "; length = String:to_i IO:input buf, 256

for i = 0, i < 8, i++
	printf "%s\n",  SecureRandom:hex(buf, length)
end

