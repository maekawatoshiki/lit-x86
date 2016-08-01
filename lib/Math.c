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
unsigned long w=88675123;
int math_rand() { // xorshift128
	static unsigned long x=123456789,y=362436069,z=521288629;
	unsigned long t; 
	t=(x^(x<<11));x=y;y=z;z=w; return (int)(w=(w^(w>>19))^(t^(t>>8))); 
} 
void math_rand_set(int s) { w = s; }
