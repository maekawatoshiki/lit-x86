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
			return x_n_1*x_n_1 + y_n_1*y_n_1
		else
			x_n = x_n_1
			y_n = y_n_1
		end
	end
	0
end
def print_density d
	if d > 8
		print ' '
	elsif d > 4
		print '.'
	elsif d > 2
		print '*'
	else
		print '+'
	end
end

$x_max = 1.0
$x_min = -2.0
$y_max = 1.0
$y_min = -1.0
$dx = 0.026
$dy = 0.05

x = 0.0
y = 0.0
x_length = ($x_max - $x_min) / $dx;
for y = $y_max, y > $y_min, y -= $dy
	for x = $x_min, x < $x_max, x += $dx 
		print_density m(x, y, 300)
	end
	puts
end
