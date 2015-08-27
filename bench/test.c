#include <stdio.h>

int fibo(int n) {
	if(n < 2) return 1;
	else return fibo(n - 1) + fibo(n - 2);
}

int main() {
	printf("%d\n", fibo(35));

	return 0;
}
