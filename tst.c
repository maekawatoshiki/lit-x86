#include <stdio.h>

int gcd(int xx, int y) {
	unsigned long long x = 0xfffffffffffffad3;
	return x;
}

int main() {
	for(int x = 1; x < 1000; x++) {
		for(int y = 1; y < 1000; y++) {
			printf("gcd(%d, %d) = %d\n", x, y, gcd(x, y));
		}
	}

	return 0;
}
