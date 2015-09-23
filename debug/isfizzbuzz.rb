$NUMBER = 0
$FIZZ = 1
$BUZZ = 2
$FIZZBUZZ = 3

def fizzbuzz(i)
  if i % 15 == 0
    return FIZZBUZZ
  elsif i % 5 == 0
    return BUZZ
  elsif i % 3 == 0
    return FIZZ
  else
    return NUMBER
  end
end

puts fizzbuzz(15) # => 3 (fizzbuzz)
