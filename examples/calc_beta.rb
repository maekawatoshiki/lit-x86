def get_num e:string : string
	num = ""
	while '0' <= e[$pos] and e[$pos] <= '9'
		num += e[$pos]
		$pos += 1
	end
	$pos -= 1
	num 
end

def prim e:string out:string : string
	if e[$pos] == '('
		$pos += 1
		out = addsub(e out)
	else 
		out += get_num e
	end
	$pos += 1
	out + "|"
end

def muldiv e:string out:string : string
	out = prim e out
	while e[$pos] == '*' or e[$pos] == '/'
		op = e[$pos]
		$pos += 1
		out = prim e out
		out = out + op + "|"
	end
	out
end

def addsub e:string out:string : string
	out = muldiv e out
	while e[$pos] == '+' or e[$pos] == '-'
		op = e[$pos]
		$pos += 1
		out = muldiv e out
		out = out + op + "|"
	end
	out
end

def calc e:string
	out = ""
	puts "input = " e
	out = addsub e out
	puts "expr = " out
end

$pos = 0
calc gets

