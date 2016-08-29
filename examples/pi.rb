# The calculate of pi program by Maekawa Toshiki.
# Used http://xn--w6q13e505b.jp/program/spigot.html as reference

def pi
  N = 14*5000
  NM = N - 14 # PI LONG
  a = new int N
  d = 0; e = 0; g = 0; h = 0
  f = 10000
  for c = NM, c, c -= 14
    d = d % f
    e = d
    for b = c - 1, b > 0, b -= 1
      g = 2 * b - 1
      if c != NM
        d = d * b + f * a[b]
      else
        d = d * b + f * (f / 5)
      end
      a[b] = d % g
      d = d / g
    end
		printf("%04d ", e + d / f)
		if c % 112 == 0
			puts()
		end
  end
end

puts(pi())
