
def f(i)
	ret = 0
	while 0 <= i
		ret = i + ret
		i = i - 1
	end
return ret

def main
	i = 2
	while i < 10
		puts f(10)
		i = i + 1
	end
return 0
