#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void swap(int *a, int *b) { int t = *a; *a = *b; *b = t; }

int Math_gcd(int a, int b) {
	if(a < b) swap(&a, &b);
	while(b > 0) {
		int t = a % b;
		a = b;
		b = t;
	}
	return a;
}

int Math_lcm(int a, int b) {
	return a / Math_gcd(a, b) * b;
}

int Math_fact(int n) {
	int r = n;
	while(--n) r *= n;
	return r;
}

int Math_even(int n) { return n & 1; }
int Math_odd(int n) { return !Math_even(n); }
int Math_abs(int n) { return n < 0 ? -n : n; }
int Math_rand() { return rand(); }
