#include <stdio.h>

void putNumber(int n) { printf("%d\n", n); }

int iff()
{
  void *funcTable[2] = {(void*)putNumber, (void*)getchar};
  funcTable(10);
}

int main()
{
  iff();
}
