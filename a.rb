def say2(s1:string, s2:string)
	puts(s1, s2)
end

def fact(i)
	if i == 1
		1
	else
		fact(i - 1) * i
	end
end

def main
	say2("hello", " world!")
	puts("fact(10) = ", fact(10))
end

