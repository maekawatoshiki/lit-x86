def main
	a:int[] = Array(1000)
	puts("first alloc")
	a = Array(1000)
	puts("second alloc")
	Array(1000)
	puts("third alloc")
end


