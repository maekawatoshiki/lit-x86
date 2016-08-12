require "std"

def template_join(x, y):string
	"{}+{}={}" % x % y % (x+y)
end

puts template_join 1 2
puts template_join 1.2 3.8
puts template_join "hel" "lo"

# output
# 1+2=3
# 1.2+3.8=5
# hel+lo=hello
