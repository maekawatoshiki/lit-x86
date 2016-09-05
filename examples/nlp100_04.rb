# No.04 of natural language processing 100
#  ref. http://www.cl.ecei.tohoku.ac.jp/nlp100/#sec00

require "std"

def in?(n, a:int[])
	for i in 0...a.length()
		if n == a[i]
			return true
		end
	end
	false
end

str = "Hi He Lied Because Boron Could Not Oxidize Fluorine. New Nations Might Also Sign Peace Security Clause. Arthur King Can."
str.replace_all!(".", "")
a = str.split(' ')
single = [1, 5, 6, 7, 8, 9, 15, 16, 19]
lst = []:string
for i in 0...a.length()
	if i.in?(single)
		lst += "" + a[i][0]
	else
		lst += "" + a[i][0] + a[i][1]
	end
end
puts(lst)
