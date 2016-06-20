require "algorithm"
require "File"

def length ary:int[]
	builtinlength ary
end

def length ary:string[]
	builtinlength ary
end

def length str:string
	strlen str
end

def geti
	str_to_int gets
end

def to_int s:string
	str_to_int s
end

def to_float s:string :double
	0.0 + to_int s
end

# operators for array
def operator+ a:int[] x:int :int[]
	size = length(a) + 1
	cpy = new size
	for i in 0...size-1
		cpy[i] = a[i]
	end
	cpy[size-1] = x
	cpy
end

def operator+ a:string[] x:string :string[]
	size = length(a) + 1
	cpy = new size string
	for i in 0...size-1
		cpy[i] = a[i]
	end
	cpy[size-1] = x
	cpy
end

# operators for string
def str_eql a:string b:string
	a_len = length a
	b_len = length b
	for i = 0, i < a_len & i < b_len, i += 1
		if a[i] != b[i]
			return a[i] - b[i]
		end
	end
	0
end

def operator == a:string b:string
	if str_eql a b; false; else; true; end
end

def operator != a:string b:string
	if str_eql a b; true; else; false; end
end

def operator < a:string b:string
	diff = str_eql a b
	if diff < 0; 1; else; 0; end
end

def operator > a:string b:string
	diff = str_eql a b
	if diff > 0; 1; else; 0; end
end

def operator* a:string n:int :string	
	res = a
	for i in 0...n-1
		res += a
	end
	res
end

