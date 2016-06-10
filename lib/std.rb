require "algorithm"
require "File"

def length ary:int[]
	len ary
end

def length str:string
	strlen str
end

def geti
	str_to_int gets
end

def append a:int[] x:int :int[]
	size = length(a) + 1
	cpy = new size
	for i in 0...size-1
		cpy[i] = a[i]
	end
	cpy[size-1] = x
	cpy
end

def operator+ a:int[] x:int :int[]
	append a x
end
