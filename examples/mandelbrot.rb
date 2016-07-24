require "std"


def m c_x:double, c_y:double, n
	x_n = 0.0
	y_n = 0.0
	x_n_1 = 0.0
	y_n_1 = 0.0
	for i in 0...n 
		x_n_1 = x_n*x_n - y_n*y_n + c_x
		y_n_1 = 2.0 * x_n * y_n + c_y
		if x_n_1*x_n_1 + y_n_1*y_n_1 > 4.0 
			return n
		else
			x_n = x_n_1
			y_n = y_n_1
		end
	end
	0
end

$x_max = 1.0
$x_min = 0.0-2
$y_max = 1.0
$y_min = 0.0-1
$dx = 0.1
$dy = 0.1

x = 0.0
y = 0.0
x_length = ($x_max - $x_min) / $dx;
for y = $y_max, y > $y_min, y -= $dy
	for x = $x_min, x < $x_max, x += $dx 
		if m(x, y, 300) == 0 
			print "*"
		else
			print " "
		end
	end
	puts
end
