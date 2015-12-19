#include <stdio.h>

int M_fact(int n, int j) {
	if(n == 1) return 1;
	else return M_fact(n - 1, 0) * n;
}

int main() {
	M_fact(1, 3);
}


