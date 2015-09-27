$X = 50
$Y = 30

$map = 0
$data = 0

def init
	map = Array((X+1)*(Y+1))
	data = Array((X+1)*(Y+1))
	for y = 0, y <= Y, y++
		for x = 0, x <= X, x++
			map[y * Y + x] = (rand() + rand())% 2
		end
	end
end


def show 
	for y = 1, y < Y, y++
		for x = 1, x < X, x++
			if map[y * Y + x] == 0
				output " "
			else
				output "*"
			end
		end
		puts ""
	end
end

def gen
	for y = 1, y <= Y, y++
		for x = 1, x <= X, x++
			data[y * Y + x] = map[y * Y + x]
		end
	end
	
	for y = 1, y < Y, y++
		for x = 1, x < X, x++
			if data[y * Y + x] == 0 
				e = data[y * Y + (x-1)] + data[y * Y + (x+1)] + data[(y-1) * Y + x] + data[(y+1) * Y + x]
				e = e + data[(y-1) * Y + (x-1)] + data[(y+1) * Y + (x-1)] + data[(y-1) * Y + (x+1)] + data[(y+1) * Y + (x+1)]
				if e == 3; map[y * Y + x] = 1; end
			else 
				e = data[y * Y + (x-1)] + data[y * Y + (x+1)] + data[(y-1) * Y + x] + data[(y+1) * Y + x]
				e = e + data[(y-1) * Y + (x-1)] + data[(y+1) * Y + (x-1)] + data[(y-1) * Y + (x+1)] + data[(y+1) * Y + (x+1)]
				if e < 2; map[y * Y + x] = 0; end
				if e > 3; map[y * Y + x] = 0; end
			end
		end
	end

end

# Main

init()

for i = 0, i < 20000, i++
	gen()
	show()
	sleep(100000)
end

