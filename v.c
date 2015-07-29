#include <stdio.h>

int iff()
{
  int i = 0;
  while(i < 100)
  {
    int j = 0, k = 0, sum = 0;
    for(j = 1; j <= 100000; j++)
    {
      for(k = 1; k <= 100000; k++)
      {
        sum += k * j + sum;
      }
      if(sum % 10 == 0) puts("hello");
    }
  }
  while(1){}
  return 1;
}

int main()
{
  iff();
}
