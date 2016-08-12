require "std"

def template join(n, s):string
	ret = ""
	for i in 0...length n
		ret += n[i].to_string + s
	end
	ret
end

puts [1, 2].join(" ")
puts ["hello", "world"].join(" ")

