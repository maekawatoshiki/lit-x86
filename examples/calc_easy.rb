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
	puts "reverse polish: " tok_stream
	tok = ""
	tok_ary = new 0 string
	for pos in 0...length tok_stream
		if tok_stream[pos] == ' '
			tok_ary += tok
			tok = ""
		else
			tok += tok_stream[pos]
		end
	end

	# VM ( calculate )
	stack = new 256
	sp = 0
	for i in 0...length tok_ary
		s = tok_ary[i]
		if s == "+"
			stack[sp-2] = stack[sp-2] + stack[sp-1]
			sp -= 1
		elsif s == "-"
			stack[sp-2] = stack[sp-2] - stack[sp-1]
			sp -= 1
		elsif s == "*"
			stack[sp-2] = stack[sp-2] * stack[sp-1]
			sp -= 1
		elsif s == "/"
			stack[sp-2] = stack[sp-2] / stack[sp-1]
			sp -= 1
		else
			stack[sp] = str_to_int s
			sp += 1
		end
	end
	puts "answer: " stack[0]
end

calc gets
