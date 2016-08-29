require "std"

def set10 ref x
	x = 10
end

a = 1
if a == 1
	set10(a)
	puts(a)
end
