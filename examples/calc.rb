def strlen(d:string)
  for len = 0, d[len] != 0, len = len + 1
  end
  len
end

def strcpy(d:string, s:string)
  for i = 0, s[i] != 0, i = i + 1
    d[i] = s[i]
  end
  d[i] = 0
end

def strcat(d:string, s:string)
  i = 0
  len = strlen(d)
  for j = len, s[i] != 0, i = i + 1
    d[j] = s[i]
    j = j + 1
  end
end

def isdigit(n)
  res = 0
  if '0' < n if n <= '9'
      res = 1
  end end
  res
end

def calc(a:string)
  num = Array(64); sp = 0
  len = strlen(a)
  for i = 0, i < len, i = i + 1
    if a[i] == '+'
      num[sp - 2] = num[sp - 2] + num[sp - 1]
      sp = sp - 1
    elsif a[i] == '-'
      num[sp - 2] = num[sp - 2] - num[sp - 1]
      sp = sp - 1
    elsif a[i] == '*'
      num[sp - 2] = num[sp - 2] * num[sp - 1]
      sp = sp - 1
    elsif a[i] == '/'
      num[sp - 2] = num[sp - 2] / num[sp - 1]
      sp = sp - 1
    elsif isdigit(a[i]) == 1
      num[sp] = a[i] - '0'
      sp = sp + 1
    end
  end
  num[0]
end

a:string = Array(256)

strcpy(a, "1 3 + 4 * ")
strcat(a, "2 / 1 +")

puts calc(a) # => 9
