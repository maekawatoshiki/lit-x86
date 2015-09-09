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
# return is not support...
def sum(n)
  sm = 0
  for i = 1, i <= n, i = i + 1
    sm = sm + i
  end
  sm
end
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

- compile
```
$ make (need clang)
$ ./lit [sourcefile]
```

# Version Info
- 2015/7/22 : First Commit
