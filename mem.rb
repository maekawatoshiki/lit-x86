Array a[1000000]

N = 1000000
for i = 0, i < N
  a[i] = 0
  i = i + 1
end
for i = 2, i < N
  if a[i] == 0
    k = i * 2
    while k < N
      a[k] = 1
      k = k + i
    end
  end
  i = i + 1
end
for i = 2, i < N
  if a[i] == 0
    puts i
  end
  i = i + 1
end
