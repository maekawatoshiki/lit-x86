require "util"

module Base64
	def to_bin(str:string):int[]
		a = []:int
		for i in 0...str.length()
			for b = 128, b >= 1, b /= 2
				a += <int> ((str[i] / b) & 1)
			end
		end
		a
	end
	def split6(bin:int[]):int[]
		ary = []:int
		for i in 0...bin.length()
			n = 0
			p = 1
			for k in 0...6
				n += bin[i + (6-k-1)] * p
				p *= 2
			end
			ary += n
			i += 5
		end
		ary
	end
	def makestr(bin:int[]):string
		dict = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/="
		str = ""
		for i in 0...bin.length()
			str += dict[bin[i]]
		end
		str
	end
	def fixstr(str:string):string
		n = str.length() % 4;
		if n == 0; return str; end
		n = 4 - n
		str + "=" * n
	end

	def encode(str:string):string
		to_bin(str).split6().makestr().fixstr()
	end

	def decode(str:string):string
		str_len = str.length()
		dict = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/="
		decode_table = new int 256
		for i in 0...64; decode_table[<int>dict[i]] = i; end
		out = ""
		block = [0, 0, 0, 0]
		count = 0
		for i in 0...str_len
			tmp = decode_table[str[i]]
			block[count] = tmp
			count += 1
			if count == 4
				out += <char> ( (block[0] * 4) | (block[1] / 16) )
				out += <char> ( (block[1] * 16) | (block[2] / 4) )
				out += <char> ( (block[2] * 64) | (block[3] / 1) )
				count = 0
			end
		end
		out
	end
end


