# The calculate of pi program by Maekawa Toshiki.
# Used http://xn--w6q13e505b.jp/program/spigot.html as reference

def format(n, long)
  l = 0
  h = 1
  while n / h != 0
    l = l + 1; h = h * 10
  end
  for i = long - l, i > 0, i = i - 1
    output "0"
  end
  output n
end

def pi
  N = 14*100
  NM = N - 14 # PI LONG
  a = Array(N)
  d = 0; e = 0; g = 0; h = 0
  f = 10000
  for c = NM, c > 0, c = c - 14
    d = d % f
    e = d
    for b = c - 1, b > 0, b = b - 1
      g = 2 * b - 1
      if c != NM
        d = d * b + f * a[b]
      else
        d = d * b + f * (f / 5)
      end
      a[b] = d % g
      d = d / g
    end
    format(e + d / f, 4);
    puts ""
  end
end

puts pi()
