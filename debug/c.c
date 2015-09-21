#include <stdio.h>

#define N 100000

int table(int *a) {
	for(int i = 0; i < N; i++) {
		a[i] = 0;
	}
	for(int i = 2; i * i < N; i++) {
		if(a[i] == 0) {
			for(int k = i * 2; k < N; k += i) {
				a[k] = 1;
			}
		}
	}
}

int isprime(int *a, int n) {
	if(a[n] == 0) return 1;
	else return 0;
}

int main() {
	int *a = malloc(sizeof(int) * N);
	table(a);
	for(int i = 2; i < N; i++) {
		if(isprime(a, i)) {
			printf("%d\n", i);
		}
	}
}
