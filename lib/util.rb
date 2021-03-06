def length ary:int[]
	builtinlength(ary)
end

def length ary:double[]
	builtinlength(ary)
end

def length ary:string[]
	builtinlength(ary)
end

def length str:string
	strlen(str)
end

module util
	def template max(a, b):T
		if a < b
			b
		else
			a
		end
	end
	def template min(a, b):T
		if a < b
			a
		else
			b
		end
	end
	def geti
		str_to_int(gets())
	end
end

def not x
	if x == 0
		1
	else
		0
	end
end

def to_int s:string
	str_to_int(s)
end

def to_string(s:string):string
	s
end

def to_string(i):string
	int_to_str(i)
end

def to_string(i:int64):string
	int64_to_str(i)
end

def to_string(f:double):string
	float_to_str(f)
end

def to_float s:string :double
	str_to_float(s)
end

def to_int(s:string[]):int[]
	out = []:int
	for i in 0...length(s)
		out += s[i].to_int()
	end
	out
end

def to_float(s:string[]):double[]
	out = []:double
	for i in 0...length(s)
		out += s[i].to_float()
	end
	out
end

def alpha? ch:char
	if ('A' <= ch & ch <= 'Z') | ('a' <= ch & ch <= 'z')
		true
	else
		false
	end
end

def digit? ch:char
	if '0' <= ch & ch <= '9'
		true
	else
		false
	end
end

def reverse(str:string):string
	out = ""
	for i = length(str)-1, i >= 0, i -= 1
		out += str[i]
	end
	out
end

def reverse(a:int[]):int[]
	out = []:int
	for i = length(a)-1, i >= 0, i -= 1
		out += a[i]
	end
	out
end

def reverse(a:double[]):double[]
	out = []:double
	for i = length(a)-1, i >= 0, i -= 1
		out += a[i]
	end
	out
end

def reverse(a:string[]):string[]
	out = []:string
	for i = length(a)-1, i >= 0, i -= 1
		out += a[i]
	end
	out
end

lib String
	proto String_addtoaddr(s:string, add):string | add_addr
end

def substr(str:string, bgn):string
	str.substr(bgn, length(str)-bgn)
end

# def substr(str:string, bgn, last):string
# 	if last < 0; return str; end
# 	ret = "" + str.add_addr(bgn)
# 	ret[last] = 0
# 	"" + ret
# end

def template join(a, s):string
	ret = ""
	for i in 0...length(a)-1
		ret += a[i].to_string() + s
	end
	if 0 < length a
		ret += a[length(a)-1].to_string()
	end
	ret
end

def split(str:string, ch:char):string[]
	str_len = length(str)
	bgn = 0
	chcount = 0
  i = 0
	while i < str_len
		if ch == str[i]
			chcount += 1
      i += 1
      while ch == str[i]
        i += 1
      end
      if i == str_len
        chcount -= 1
      end
		else 
      i += 1
    end
	end
	ret = new string chcount + 1
	# ret = []:string
	pos = 0
	for i in 0...str_len
		if ch == str[i]
      if i - bgn > 0
        ret[pos] = str.substr(bgn, i-bgn)
        pos += 1
      end
      bgn = i + 1
		end
	end
	if bgn < str_len
		ret[pos] = substr(str, bgn)
	end
	ret
end

def find base:string s:string # find s from base. if found, return index of base
	base_len = length(base)
	s_len = length(s)
	for i in 0...base_len
		if base[i] == s[0]
			flg = 1
			for k in 0...s_len
				if base[i+k] != s[k]
					flg = 0
				end
			end
			if flg
				return i
			end
		end
	end
	0-1
end

def replace(base:string from:string to:string):string
	idx = base.find(from)
	if idx < 0; return 0; end
	a = base.substr(0, idx)
	b = base.substr(idx+length(from))
	a + to + b
end

def replace_all(base:string from:string to:string):string
	while (t = base.replace(from, to)) != 0
		base = t
	end
	base
end
def replace_all!(ref base:string, from:string, to:string):string
	base = replace_all(base, from, to)
end

def swap!(ref a, ref b)
	t = a
	a = b
	b = t
end

def back(a:int[])
	a[length(a)-1]
end
def back(a:string[]):string
	a[length(a)-1]
end
def pop_back(ref a:int[])
	aln = length(a)
	c = new int aln-1
	for i in 0...aln-1
		c[i] = a[i]
	end
	a = c
end
def pop_back(ref a:string[])
	aln = length(a)
	c = new int aln-1
	for i in 0...aln-1
		c[i] = a[i]
	end
	a = c
end

# operators for array
# def operator+ a:int[] x:int :int[]
# 	size = length(a) + 1
# 	cpy = new int size
# 	for i in 0...size-1
# 		cpy[i] = a[i]
# 	end
# 	cpy[size-1] = x
# 	cpy
# end

def operator+ a:double[] x:double :double[]
	size = length(a) + 1
	cpy = new double size
	for i in 0...size-1
		cpy[i] = a[i]
	end
	cpy[size-1] = x
	cpy
end

def operator+ a:string[] x:string :string[]
	size = length(a) + 1
	cpy = new string size
	for i in 0...size-1
		cpy[i] = a[i]
	end
	cpy[size-1] = x
	cpy
end

def operator + (a:int[], b:int[]):int[]
	for i in 0...length(b)
		a += b[i]
	end
	a
end

def operator + (a:string[], b:string[]):string[]
	for i in 0...length(b)
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
	a_len = length(a)
	b_len = length(b)
	a_len = util::min(a_len, b_len)
	for i in 0...a_len
		if a[i] != b[i]
			return a[i] - b[i]
		end
	end
	0
end

def operator == a:string b:string
	if str_eql(a b); false; else; true; end
end

def operator != a:string b:string
	if str_eql(a b); true; else; false; end
end

def operator < a:string b:string
	diff = str_eql(a b)
	if diff < 0; 1; else; 0; end
end

def operator > a:string b:string
	diff = str_eql(a b)
	if diff > 0; 1; else; 0; end
end

def operator* a:string n:int :string	
	res = ""
	for i in 0...n
		res += a
	end
	res
end

def operator % (str:string, n):string
	bgn = str.find("{}")
	a = str.substr(0, bgn)
	b = str.substr(bgn+length("{}"))
	a + n.to_string() + b
end

def operator % (str:string, n:int64):string
	bgn = str.find("{}")
	a = str.substr(0, bgn)
	b = str.substr(bgn+length("{}"))
	a + n.to_string() + b
end

def operator % (str:string, n:string):string
	bgn = str.find("{}")
	a = str.substr(0, bgn)
	b = str.substr(bgn+length("{}"))
	a + n + b
end

def operator % (str:string, n:double):string
	bgn = str.find("{}")
	a = str.substr(0, bgn)
	b = str.substr(bgn+length("{}"))
	a + n.to_string() + b
end
