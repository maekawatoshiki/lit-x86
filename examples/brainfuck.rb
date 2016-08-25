require "std"

struct Hash
	key
	val
	height
	a:Hash
	b:Hash
end

def insert(ref a:Hash, key, val):Hash
	a = set_hash(a, key, val)
end
def get(a:Hash, key)
	if key == a.key
		return a.val
	elsif a.key < key
		if a.b
			return get(a.b, key)
		end
	elsif key < a.key
		if a.a
			return get(a.a, key)
		end
	end
	nil
end

def set_hash(a:Hash, key, val):Hash
	if a == nil
		a = new Hash
		a.val = val
		a.key = key
		a.height = 1
		return a
	elsif a.key < key
		a.b = set_hash(a.b, key, val)
	elsif key < a.key
		a.a = set_hash(a.a, key, val)
	end
	balance(a)
end
def diff_height(a:Hash)
	get_height(a.b) - get_height(a.a)
end
def get_height(a:Hash)
	if a
		a.height
	else
		0
	end
end
def fix_height(a:Hash)
	l = get_height(a.a)
	r = get_height(a.b)
	a.height = util::max l + 1, r + 1
end

def balance(a:Hash):Hash
	fix_height(a)
	if diff_height(a) == 2
		if diff_height(a.b) < 0
			a.b = rotateR(a.b)
		end
		return rotateL(a)
	elsif diff_height(a) == -2
		if diff_height(a.a) > 0
			a.a = rotateL(a.a)
		end
		return rotateR(a)
	end
	a
end

def rotateL(a:Hash):Hash
	l = a.b
	a.b = l.a
	l.a = a
	fix_height(a)
	fix_height(l)
	l
end
def rotateR(a:Hash):Hash
	r = a.a
	a.a = r.b ##err
	r.b = a
	fix_height(a)
	fix_height(r)
	r
end

struct Token
	ch:char
	num
	jmp
end
def operator + (a:Token[], tok:Token):Token[]
	size = builtinlength(a) + 1
	cpy = new Token size
	for i in 0...size-1
		cpy[i] = a[i]
	end
	cpy[size-1] = tok
	cpy
end

print "file name of brainfuck you want to run: "
s = File::read gets
jmp = new Hash
brackets = []:int
slen = length s
for i in 0...slen
	if s[i] == '['
		brackets += i
	elsif s[i] == ']'
		a = brackets.back()
		brackets.pop_back()
		jmp.insert(i, a)
		jmp.insert(a, i)
	end
end

tape = new int 1000
pos = 0
for i in 0...slen
	if s[i] == '+'
		tape[pos] += 1
	elsif s[i] == '-'
		tape[pos] -= 1
	elsif s[i] == '>'
		pos += 1
	elsif s[i] == '<'
		pos -= 1
	elsif s[i] == '.'
		print <char>tape[pos]
	elsif s[i] == ','
		tape[pos] = <int>getc
	elsif s[i] == '['
		if tape[pos] == 0
			i = jmp.get(i)
			# i = jmplabel[i]
		end
	elsif s[i] == ']'
		if tape[pos] != 0
			i = jmp.get(i)
			# i = jmplabel[i]
			i -= 1
		end
	end
end




