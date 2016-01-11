require String

module calc
	$srcLen = 0
	$pos = 0

	def atoi(s:string)
		sum = 0; n = 1
		for l = 0, String.isdigit(s[pos + l]) == 1, l++; 
			n = n * 10
		end
		for i = 0, i < l, i++
			n = n / 10
			sum = sum + n * (s[pos++] - '0')
		end
		sum
	end

	def prim(a:string, out:string)
		if a[pos] == '('
			pos++
				addsub(a, out)
			pos++ # ')'
		else
			while String.isdigit(a[pos])
				out[String.len(out)] = a[pos++]
			end
		end
		out = out ~ ","
	end

	def muldiv(a:string, out:string)
		prim(a, out)
		while 1
			if a[pos] == '*'
				pos++
					prim(a, out)
				out = out ~ "*"
			elsif a[pos] == '/'
				pos++
					prim(a, out)
				out = out ~ "/"
			elsif a[pos] == ' '
				pos++
			else
				break
			end
		end
	end


	def addsub(a:string, out:string) 
		muldiv(a, out)
		while pos < srcLen
			if a[pos] == '+'
				pos++
					muldiv(a, out)
				out = out ~ "+"
			elsif a[pos] == '-'
				pos++
					muldiv(a, out)
				out = out ~ "-"
			elsif a[pos] == ' '
				pos++
			else
				break
			end
		end
	end

	def calc(a:string)
		out:string = Array(100)
		for i = 0, i < 100, i++; out[i] = 0; end
		srcLen = String.len(a)
		addsub(a, out)
		String.copy(a, out)

		num = Array(128); sp = 0
		len = String.len(a)
		for pos = 0, pos < len, pos++
			if a[pos] == '+'
				num[sp - 2] = num[sp - 2] + num[sp - 1]
				sp--
			elsif a[pos] == '-'
				num[sp - 2] = num[sp - 2] - num[sp - 1]
				sp--
			elsif a[pos] == '*'
				num[sp - 2] = num[sp - 2] * num[sp - 1]
				sp--
			elsif a[pos] == '/'
				num[sp - 2] = num[sp - 2] / num[sp - 1]
				sp--
			elsif String.isdigit(a[pos])
				num[sp++] = atoi a
			end
		end
		num[0]
	end
end

def input(str:string)
	f = File.open("/dev/stdin", "w+")
	File.gets(str, 100, f)
	str[ String.len(str) - 1 ] = 0
	File.close(f)
	str
end

expr:string = Array(100)

input(expr)

puts calc.calc(expr) 
