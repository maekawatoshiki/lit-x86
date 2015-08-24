def fibo(n)
	if n < 2
		1
	else 
		fibo(n - 1) + fibo(n - 2)
	end
end

def main
	puts fibo(39)
end

main()
