require "std"

struct HASH
	use
	key
	val
	height
	a:HASH
	b:HASH
end

def set(a:HASH, key, val):HASH
	if a.use == false
		a.val = val
		a.key = key
		a.height = 1
		a.use = true
		return a
	elsif a.key < key
		if a.b == nil
			a.b = new HASH
		end
		a.b = set(a.b, key, val)
	elsif key < a.key
		if a.a == nil
			a.a = new HASH
		end
		a.a = set(a.a, key, val)
	end
	balance(a)
end
def get(a:HASH, key)
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
def diff_height(a:HASH)
	get_height(a.b) - get_height(a.a)
end
def get_height(a:HASH)
	if a
		a.height
	else
		0
	end
end
def fix_height(a:HASH)
	l = get_height(a.a)
	r = get_height(a.b)
	a.height = util::max l + 1, r + 1
end

def balance(a:HASH):HASH
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

def rotateL(a:HASH):HASH
	l = a.b
	a.b = l.a
	l.a = a
	fix_height(a)
	fix_height(l)
	l
end
def rotateR(a:HASH):HASH
	r = a.a
	a.a = r.b ##err
	r.b = a
	fix_height(a)
	fix_height(r)
	r
end

def show(a:HASH, n)
	puts a.key, "({})" % (a.height)
	if a.a
		print " " * n, "L"
		show(a.a, n+1)
	end
	if a.b
		print " " * n, "R"
		show(a.b, n+1)
	end
end


a = new HASH
Math::random_init
for i in 0...20000
	a = a.set(Math::random % 10000, Math::random % 1000)
end
show(a, 1)


