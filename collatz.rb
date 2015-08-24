def collatz(n)
	puts n
	if n > 1
		if n % 2 == 0 collatz(n / 2)
		else collatz(3*n+1); end
	end
return

def main
	collatz(4095);
return
