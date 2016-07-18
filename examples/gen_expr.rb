require "std"

def gen_num_to_str:string
	(Math::random % $MAX_NUM).to_string()
end

def gen_expr(len):string
	expr = gen_num_to_str
	for i in 0...len
		op = $OP[Math::random % length($OP)]
		expr += op
		if Math::random % 5 == 0 # 1/5%
			expr += "(" + gen_expr(len/2) + ")"
		else 
			expr += gen_num_to_str
		end
	end
	expr
end

$OP = "+-*/"
$MAX_NUM = 1000
puts gen_expr 50

