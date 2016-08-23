require "std"

struct Tree
	use?
	key
	val
	left:Tree
	right:Tree
end

def set(a:Tree, key, val):Tree
	if a.use? == false
		a.key = key
		a.val = val
		a.use? = true
	elsif key < a.key 
		if nil == a.left
			a.left = new Tree
		end
		set(a.left, key, val)
	elsif a.key < key
		if nil == a.right
			a.right = new Tree
		end
		set(a.right, key, val)
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
Math::random_init
for i in 0...20
	a = a.set(Math::random % 100, Math::random % 100)
end
show a, 1
