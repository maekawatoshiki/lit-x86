require "std"

def isalpha ch:char
	if ('A' <= ch & ch <= 'Z') | ('a' <= ch & ch <= 'z')
		1
	else
		0
	end
end

module Calc 
	def prim(input:string):string
		str = ""
		if isalpha input[$pos]
			fname = ""
			while isalpha input[$pos]
				fname += input[$pos]
				$pos += 1
			end
			$pos += 1 # (
			str = addsub(input)
			$pos += 1 # )
			str + fname + " "
		elsif input[$pos] == '('
			$pos += 1
			str = addsub(input)
			$pos += 1
			str
		else
			while ('0' <= input[$pos] & input[$pos] <= '9') | input[$pos] == '.'
				str += input[$pos]
				$pos += 1
			end
			str + " "
		end
	end

	def muldiv(input:string):string
		str = prim input
		while input[$pos] == '*' | input[$pos] == '/'
			op = input[$pos]
			$pos += 1
			str += prim(input) + op + " "
		end
		str
	end

	def addsub(input:string):string
		str = muldiv input
		while input[$pos] == '+' | input[$pos] == '-'
			op = input[$pos]
			$pos += 1
			str += muldiv(input) + op + " "
		end
		str
	end

	def run(expr:string):double
		tok_str = addsub expr
		puts "reverse polish: ", tok_str
		tok_ary = tok_str.split(' ')
		# VM ( calculate )
		cur_stack_sz = 256
		stack = new cur_stack_sz double
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
			elsif s == "sqrt"
				stack[sp-1] = Math::sqrt stack[sp-1]
			else
				stack[sp] = <double> str_to_float s
				sp += 1
			end
		end
		stack[0]
	end
end

$pos = 0
print "expression: "
input = gets
puts Calc::run input

