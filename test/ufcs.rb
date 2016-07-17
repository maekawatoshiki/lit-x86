require "std"

def sum a:int[]
	all = 0
	for i in 0...length a
		all += a[i]
	end
	all
end

[1, 2, 3].sum().puts()
