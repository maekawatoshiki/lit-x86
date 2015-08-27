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
i = 0
sum = 0
while i < 100
  if i % 2 == 0
    sum = sum + 2
  elsif i % 3 == 0
    sum = sum + 3
  else
    sum = sum + i
  end
  i = i + 1
end

# create function
# return is not support...
def sum(n)
  i = 1
  sm = 0
  while i <= n
    sm = sm + i
    i = i + 1
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

def main
  puts fibo(39)
end
```

- compile
```
$ make (need clang)
$ ./lit [sourcefile]
```

# Version Info
- 2015/7/22 : First Commit
