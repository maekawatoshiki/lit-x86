require "std"

def mul(A:int64[], B:int64[]):int64[]
	C = [<int64>0, <int64>0, <int64>0, <int64>0]
	for i in 0...2
		for k in 0...2
			for j in 0...2
				C[2*i+j] = (C[2*i+j] + A[2*i+k] * B[2*k+j])
			end
		end
	end
	C
end

def pow(A:int64[], n):int64[]
	B = [<int64>1, <int64>0, <int64>1, <int64>0]
	while n > 0
		if n & 1
			B = mul B A
		end
		A = mul A A
		n /= 2
	end
	B
end

max = gets.to_int()
for i in 0...max
	A = [<int64>1, <int64>1, <int64>1, <int64>0]
	A = pow A, i
	puts "fibo(" i ") = " A[0]
end
