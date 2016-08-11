require "std"

module Calc 
	def skip
		$pos += 1
	end
	def prim(input:string):string
		str = ""
		if input[$pos].alpha? # function (sqrt..)
			fname = ""
			while input[$pos].alpha?
				fname += input[$pos]
				skip
			end
			skip # (
			str = addsub(input)
			skip # )
			str + fname + " "
		elsif input[$pos] == '('
			skip
			str = addsub(input)
			skip
			str
		else
			while input[$pos].digit? | input[$pos] == '.'
				str += input[$pos]
				skip
			end
			str + " "
		end
	end

	def muldiv(input:string):string
		str = prim input
		while input[$pos] == '*' | input[$pos] == '/' | input[$pos] == '^'
			op = input[$pos]
			skip
			str += prim(input) + op + " "
		end
		str
	end

	def addsub(input:string):string
		str = muldiv input
		while input[$pos] == '+' | input[$pos] == '-'
			op = input[$pos]
			skip
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
			elsif s == "^"
				stack[sp-2] = Math::pow stack[sp-2], stack[sp-1]
				sp -= 1
			elsif s == "sqrt"
				stack[sp-1] = Math::sqrt stack[sp-1]
			else
				stack[sp] = s.to_float
				sp += 1
			end
		end
		stack[0]
	end
end

$pos = 0
print "expression: "
puts Calc::run gets

