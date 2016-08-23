require "std"

print "file name of brainfuck you want to run: "
s = File::read gets
jmplabel = new int length(s)
tape = new int 1000
pos = 0
brackets = []:int
slen = length s
for i in 0...slen
	if s[i] == '['
		brackets += i
	elsif s[i] == ']'
		a = brackets.back()
		brackets.pop_back()
		jmplabel[i] = a
		jmplabel[a] = i
	end
end
puts "running..."

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
	elsif s[i] == '['
		if tape[pos] == 0
			i = jmplabel[i]
		end
	elsif s[i] == ']'
		if tape[pos] != 0
			i = jmplabel[i]
			i -= 1
		end
	end
end




