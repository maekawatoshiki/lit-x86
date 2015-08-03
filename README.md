# lit
> lit is jit compile calculator.
so, very fast!

# How to use
- syntax
```ruby
# output the number
print 12345
print 488569363

# calculate & variable
# Corresponding only the integer arithmetic
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
```

- compile
```
$ make (need clang)
$ ./lit  
<expression>
```

# Version Info
- 2015/7/22 : First Commit
