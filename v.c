#include <stdio.h>

float f()
{
  //float n = 2.3, k = 43.425;
  return 1.32*32.3;
}
//80484e0
int main()
{
  float n = 43.425;
  printf("%x\n", *(int*)&n);
  f();
}
