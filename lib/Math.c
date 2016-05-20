#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void swap(int *a, int *b) { int t = *a; *a = *b; *b = t; }

int math_gcd(int a, int b) {
	if(a < b) swap(&a, &b);
	while(b > 0) {
		int t = a % b;
		a = b;
		b = t;
	}
	return a;
}

int math_lcm(int a, int b) {
	return a / math_gcd(a, b) * b;
}

int math_fact(int n) {
	int r = n;
	while(--n) r *= n;
	return r;
}

int math_even(int n) { return n % 2 == 0 ? 1 : 0; }
int math_odd(int n) { return !math_even(n); }
int math_abs(int n) { return n < 0 ? -n : n; }
int math_rand() { return rand(); }
