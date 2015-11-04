# Lit
- Lit is JIT Compile language.
So very faster.

# How to use

- Easy to remember. Because syntax is like ruby.

- Syntax

```ruby
# Statement => write in the function.

# output the string and number
# puts is output and \n
# output is output only.
puts "Hello world!!" # => Hello world!!\n
puts "number = ", 65536 # => number = 65536\n
output "not new line" # => not new line

# declaration is not required
# support only integer
i = 10
i = i + 43
i = i % 5
i = i - 32
sum = i * (12 / 5)
12 / 5 # => 2

# loop & if
for i = 1, i < 100, i = i + 1
  if i % 15 == 0
    puts "fizzbuzz"
  elsif i % 5 == 0
    puts "buzz"
  elsif i % 3 == 0
  	puts "fizz"
  else
  	puts i
  end
end

# create function
def sum(n)
  sm = 0
  for i = 1, i <= n, i++
    sm = sm + i
  end
  sm
	# "return sm" is OK
end

# create module(namespace)
module Say
  $v = 100
  def hello
    puts "hello"
  end
  def bye
    puts "bye"
  end
  def get_v
    v
  end
end

Say.hello()
```

- Fibonacci

```ruby
def fibo(n)
  if n < 2
    1
  else
    fibo(n - 1) + fibo(n - 2)
  end
end

puts fibo(39)
```

- quick sort

```
def sort(a, left, right)
	l = left; r = right; pv = a[(l + r) / 2]
	while 1
		while a[l] < pv; l++; end
		while pv < a[r]; r--; end
		if l >= r; break end
		tmp = a[l]; a[l] = a[r]; a[r] = tmp
		l++; r--;
	end
	l--; r++
	if left < l; sort(a, left, l); end
	if r < right; sort(a, r, right); end
end
```

- compile

```
$ make (need clang)
$ ./lit [sourcefile]
```


# Version Info
- 2015/7/22 : First Commit

# License

- Copyright (c) 2015, maekawatoshiki All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of the <ORGANIZATION> nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
