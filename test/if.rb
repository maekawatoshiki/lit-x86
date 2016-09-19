require "std"

def func(x)
  if x == 1
    10000
  elsif x == 2
    20000
  elsif x == 3
    30000
  end
end

print( func(3) )
print( func(1) )
print( func(2) )
