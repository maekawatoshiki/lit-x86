#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int Prime_is(int p) {
	if(p == 2) return 1;
	if(!(p & 1)) return 0;

	for(int i = 3; i * i <= p; i += 2) 
		if(p % i == 0) return 0;
	return 1;
}

int Prime_table(int *tbl, int max) {
	if(max < 2) return 0;

	char *list = calloc(max, sizeof(char));
	int count = max - 2, i, k;

	for(i = 2; i < max; i++)
		if(list[i] == 0)
			for(k = i * 2; k < max; k += i)
				if(list[k] == 0)
					list[k] = 1, --count;
		
	int *t = tbl;
	for(i = 2; i < max; i++)
		if(list[i] == 0) *t++ = i;

	free(list);
	return count;
}
