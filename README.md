# lit
- lit is jit compile calculator.
so, very fast!

# How to use
- syntax

```ruby
# output the number
# string is surport!
puts "Hello world!!\n"
print 12345
puts "number = ", 488569363, "\n"

# calculate & variable
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
def sum(n)
  i = 1
  sm = 0
  while i <= n
    sm = sm + i
    i = i + 1
  end
return sm # don't use "return" 2 times in 1 function.
```

- compile
```
$ make (need clang)
$ ./lit  
<expression>
```

# Version Info
- 2015/7/22 : First Commit
