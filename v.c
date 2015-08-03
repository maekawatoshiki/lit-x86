#include <stdio.h>

int func() {
  int array[0xF];
  int i = 1;
  array[i * 2] = 43;
  array[i +2] = 15;
  array[2] = 32*3;
  return array[3];
}

int main()
{
  func();
}
