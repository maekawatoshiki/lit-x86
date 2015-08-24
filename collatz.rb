def collatz(n)
	sum = 0
	while n > 1
		puts n
		if n % 2 == 0
			n = n / 2
		else
			n = 3 * n + 1
		end
		sum = sum + 1
	end
	sum
end

def main
	puts collatz(8379)
end

main()
