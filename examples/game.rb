def solve min max
	mid = min + (max - min) / 2
	puts("Is the number smaller than " mid "? or is the " mid " answer? (yes/no/answer)")
	ans = gets()
	if ans[0] == 'y'
		solve(min mid-1)
	elsif ans[0] == 'n'
		solve(mid+1 max)
	elsif ans[0] == 'a'
		puts("you choose " mid "!!")
	end
end

min = 1
max = 100
puts("Imagine a number between " min " and " max ", and answer questions")
solve(min max)
