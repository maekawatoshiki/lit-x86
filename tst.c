#include <stdio.h>

int gcd(int x, int y) {
	while(x != y) {
		if(x > y) x -= y;
		else y -= x;
	}
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
