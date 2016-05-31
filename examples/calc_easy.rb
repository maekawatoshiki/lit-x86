require "std"

def prim input:string :string
	str = ""
	while '0' <= input[$pos] & input[$pos] <= '9'
		str += input[$pos]
		$pos += 1
	end
	str + " "
end

def muldiv input:string :string
	str = prim input
	while input[$pos] == '*' | input[$pos] == '/'
		op = input[$pos]
		$pos += 1
		str += prim(input) + op + " "
	end
	str
end

def addsub input:string :string
	str = muldiv input
	while input[$pos] == '+' | input[$pos] == '-'
		op = input[$pos]
		$pos += 1
		str += muldiv(input) + op + " "
	end
	str
end

def calc expr:string
	$pos = 0
	tok_stream = addsub expr
	puts tok_stream
end

calc gets
