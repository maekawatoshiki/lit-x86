
def c_to_s(c)
	s:string = "*"
	s[0] = c
	s
end

def prim(e:string, out:string)
	out = out + c_to_s(e[$pos])
	$pos += 1
	out
end

def muldiv(e:string, out:string)
	out = prim(e, out)
	while e[$pos] == '*' or e[$pos] == '/'
		op = e[$pos]
		$pos += 1
		out = prim(e, out)
		out = out + c_to_s(op)
	end
	out
end

def addsub(e:string, out:string)
	out = muldiv(e, out)
	while e[$pos] == '+' or e[$pos] == '-'
		op = e[$pos]
		$pos += 1
		out = muldiv(e, out)
		out = out + c_to_s(op)
	end
	out
end

def calc(e:string)
	out:string = ""
	cpy:string = e
	out = addsub(cpy, out)	
	puts("expr = ", out)
end

def main
	$pos = 0
	expr:string = gets()

	puts(expr)
	calc(expr)
end


