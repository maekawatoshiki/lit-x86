module Test
	def get a, b, c, d, f, g, h
		a
	end
end

def get a, b, c, d, f, g, h
	a + b
end

print(get(1, 2, 3, 4, 5, 6, 7))
print(Test::get(1, 2, 3, 4, 5, 6, 7))

