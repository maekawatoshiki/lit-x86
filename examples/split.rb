def c_to_s(c:char):string
	s:string = Array(2)
	s[0] = c
	s
end

def split(str:string):string[]
	len = strlen(str)
	ary:string[] = Array(len)
	for i in 0..len-1
		ary[i] = c_to_s(str[i])
	end
	return ary
end

def main
	hello:string = gets()
	len = strlen(hello)
	ary = split(hello)
	for i in 0..len-1
		puts("\"", ary[i], "\"")
	end
end

