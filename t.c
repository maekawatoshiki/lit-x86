#include <stdio.h>

int main()
{
	int i, isprime;
	for(i = 2; i < 100000; i++) {
		int k;
		isprime = 1;
		for(k = 2; k * k <= i; k++) {
			if(i % k == 0) {
				isprime = 0;
				break;
			}
		}
		if(isprime == 1) {
			printf("%d\n", i);
		}
	}
	return 0;
}
