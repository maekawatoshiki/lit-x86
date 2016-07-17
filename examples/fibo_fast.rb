def mul A:int[] B:int[] :int[]
	C = [0, 0, 0, 0]
	for i in 0...2
		for k in 0...2
			for j in 0...2
				C[2*i+j] = (C[2*i+j] + A[2*i+k] * B[2*k+j])
			end
		end
	end
	C
end

def pow A:int[] n:int :int[]
	B = [1, 0, 1, 0]

	while n > 0
		if n & 1
			B = mul B A
		end
		A = mul A A
		n /= 2
	end
	B
end

max = str_to_int gets
for i in 0...max
	A = [1, 1, 1, 0]
	A = pow A i
	puts "fibo(" i ") = " A[0]
end
