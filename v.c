#include <stdio.h>

int v(void **f)
{
  int n = 65;
  putchar(n);
  return n;
}

int main()
{
  void *f[1]={(void*)putchar};
  v(f);
}
