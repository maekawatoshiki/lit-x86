require "std"

struct HASH
	use
	key
	val
	a:HASH
	b:HASH
end

def set(a:HASH, key, val):HASH
	if a.use == 0
		a.val = val
		a.key = key
		a.use = 1
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
	a
end
def get(a:HASH, key)
	if key == a.key
		return a.val
	elsif a.key < key
		return get(a.b, key)
	elsif key < a.key
		return get(a.a, key)
	end
end

def rotateL(a:HASH):HASH
	l = a.a
	a.a = l.b
	l.b = a
	l
end
def rotateR(a:HASH):HASH
	r = a.b
	a.b = r.a
	r.a = a
	r
end

def show(a:HASH, n)
	puts a.key
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
a = a.set(5, 0)
a = a.set(2, 1)
a = a.set(8, 2)
a = a.set(1, 3)
a = a.set(9, 4)
show(a, 1)
puts a.get(8)
a = rotateR(a)
show a, 1
a = rotateL(a)
show a, 1
