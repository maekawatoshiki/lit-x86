def length ary:int[]
	builtinlength ary
end

def length ary:string[]
	builtinlength ary
end

def length str:string
	strlen str
end

def is_alpha ch:char
	if ('A' <= ch & ch <= 'Z') | ('a' <= ch & ch <= 'z')
		1
	else
		0
	end
end

def geti
	str_to_int gets
end

def to_int s:string
	str_to_int s
end

def to_string(i):string
	int_to_str i
end

def to_float s:string :double
	str_to_float s
end

def substr(str:string, bgn, last):string
	ret = ""
	if last < bgn; last = length(str) - 1; end
	for i in bgn..last
		ret += str[i]
	end
	ret
end

def split(str:string, ch:char):string[]
	str_len = length str
	ret = []:string
	bgn = 0
	for i in 0...str_len
		if ch == str[i]
			ret += substr(str, bgn, i - 1)
			bgn = i + 1
		end
	end
	if bgn < str_len
		ret += substr(str, bgn, 0)
	end
	ret
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

def operator + (a:int[], b:int[]):int[]
	for i in 0...length b
		a += b[i]
	end
	a
end

def operator + (a:string[], b:string[]):string[]
	for i in 0...length b
		a += b[i]
	end
	a
end

def operator * (a:int[], n):int[]
	t = a
	for i in 0...n-1
		a += t
	end
	a
end

def operator * (a:string[], n):string[]
	t = a
	for i in 0...n-1
		a += t
	end
	a
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

