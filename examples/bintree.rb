require "std"

struct Tree
	use
	key
	val
	left:Tree
	right:Tree
end

def set(a:Tree, key, val, n):Tree
	puts "n:",n
	puts "key:", key
	puts "use:", a.use
	if a.use == false
		a.key = key
		a.val = val
		a.use = true
	elsif key < a.key 
		if nil == a.left
			a.left = new Tree
		end
		a.left = set(a.left, key, val, n+1)
	elsif a.key < key
		if nil == a.right
			a.right = new Tree
		end
		a.right = set(a.right, key, val, n+1)
	end
	a
end

def get(a:Tree, key)
	if a.key == key
		return a.val
	elsif key < a.key 
		if a.left
			return get(a.left, key)
		end
	elsif a.key < key
		if a.right
			return get(a.right, key)
		end
	end
	nil
end

def show(a:Tree, n)
	puts a.key
	if a.left
		print " " * n, "L:"
		show a.left, n+1
	end
	if a.right
		print " " * n, "R:"
		show a.right, n+1
	end
end

a = new Tree
a = a.set(5, 3, 1)
a = a.set(2, 8, 1)
a = a.set(9, 2, 1)
a = a.set(8, 2, 1)
show a, 1
