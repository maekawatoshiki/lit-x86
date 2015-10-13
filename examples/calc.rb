$srcLen = 0
$pos = 0

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
  if '0' <= n and n <= '9'
		return 1
  end
  0
end

def atoi(s:string)
  sum = 0; n = 1
  for l = 0, isdigit(s[pos + l]) == 1, l++; 
    n = n * 10
  end
  for i = 0, i < l, i++
    n = n / 10
    sum = sum + n * (s[pos++] - '0')
  end
  sum
end

def prim(a:string, out:string)
  while isdigit(a[pos])
    out[strlen(out)] = a[pos++]
  end
  strcat(out, ",")
end

def muldiv(a:string, out:string)
  prim(a, out)
  while 1
    if a[pos] == '*'
      pos++
      prim(a, out)
      strcat(out, "*")
    elsif a[pos] == '/'
      pos++
      prim(a, out)
      strcat(out, "/")
		elsif a[pos] == ' '
			pos++
		else
      break
    end
  end
end


def addsub(a:string, out:string) 
  muldiv(a, out)
  while pos < srcLen
    if a[pos] == '+'
      pos++
      muldiv(a, out)
      strcat(out, "+")
    elsif a[pos] == '-'
      pos++
      muldiv(a, out)
      strcat(out, "-")
		elsif a[pos] == ' '
			pos++
		else
      break
    end
  end
end

def input(str:string)
	f = fopen("/dev/stdin", "w+")
	fgets(str, 100, f)
	str[ strlen(str) - 1 ] = 0
	fclose(f)
	str
end

def calc(a:string)
  out:string = Array(100)
  srcLen = strlen(a)
  addsub(a, out)
  printf "parse=> %s\n", out
  strcpy(a, out)

  num = Array(128); sp = 0
  len = strlen(a)
  for pos = 0, pos < len, pos++
    if a[pos] == '+'
      num[sp - 2] = num[sp - 2] + num[sp - 1]
      sp--
    elsif a[pos] == '-'
      num[sp - 2] = num[sp - 2] - num[sp - 1]
      sp--
    elsif a[pos] == '*'
      num[sp - 2] = num[sp - 2] * num[sp - 1]
      sp--
    elsif a[pos] == '/'
      num[sp - 2] = num[sp - 2] / num[sp - 1]
      sp--
    elsif isdigit(a[pos])
      num[sp++] = atoi(a)
    end
  end
  num[0]
end

a:string = Array(100)

output "expression => "; input(a)

printf "expr => %s\n", a

puts calc(a) # => 9

