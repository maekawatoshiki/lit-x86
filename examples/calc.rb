def strlen(d:string)
  for len = 0, d[len] != 0, len++; end
  len
end

def strcpy(d:string, s:string)
  i = 0
  while s[i] != 0
    d[i] = s[i++]
  end
end

def strcat(d:string, s:string)
  i = 0
  len = strlen(d)
  j = len
  while s[i] != 0
    d[j++] = s[i++]
  end
end

def isdigit(n)
  res = 0
  if '0' <= n if n <= '9'
      return 1
  end end
  0
end

def atoi(s:string, p)
  sum = 0
  n = 1
  for l = 0, isdigit(s[p+l]) == 1, l++; n = n * 10; end
  for i = 0, i < l, i++
    n = n / 10
    sum = sum + n * (s[p+i] - '0')
  end
  sum
end

def prim(a:string, pos, out:string)
  while isdigit(a[pos]) == 1
    out[strlen(out)] = a[pos++]
  end
  out[strlen(out)] = ','
  pos
end

def muldiv(a:string, pos, out:string)
  pos = prim(a, pos, out)
  while 1 == 1
    if a[pos] == '*'
      pos++
      pos = prim(a, pos, out)
      strcat(out, "*")
    elsif a[pos] == '/'
      pos++
      pos = prim(a, pos, out)
      strcat(out, "/")
    else
      break
    end
  end
  pos
end


def addsub(a:string, pos, out:string) 
  pos = muldiv(a, pos, out)
  while 1 == 1
    if a[pos] == '+'
      pos++
      pos = muldiv(a, pos, out)
      strcat(out, "+")
    elsif a[pos] == '-'
      pos++
      pos = muldiv(a, pos, out)
      strcat(out, "-")
    else
      break
    end
  end
end

def len(n)
  len = 0
  while n > 0
    n = n / 10
    len++
  end
  len
end

def calc(a:string)
  out:string = Array(100)
  addsub(a, 0, out)
  printf "parse=> %s\n", out
  strcpy(a, out)

  num = Array(64); sp = 0
  len = strlen(a)
  for i = 0, i < len, i = i + 1
    if a[i] == '+'
      num[sp - 2] = num[sp - 2] + num[sp - 1]
      sp--
    elsif a[i] == '-'
      num[sp - 2] = num[sp - 2] - num[sp - 1]
      sp--
    elsif a[i] == '*'
      num[sp - 2] = num[sp - 2] * num[sp - 1]
      sp--
    elsif a[i] == '/'
      num[sp - 2] = num[sp - 2] / num[sp - 1]
      sp--
    elsif isdigit(a[i]) == 1
      num[sp] = atoi(a, i)
      i = i + len(num[sp])
      sp++
    end
  end
  num[0]
end

a:string = Array(256)

strcpy(a, "1+3*30/5-10")

printf "expr=> %s\n", a

puts calc(a) # => 9
