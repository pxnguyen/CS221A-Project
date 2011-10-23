#include <stdio.h>
#include "rdtsc.h"

void measurement_overhead(int N){
	unsigned long long a,b,c;
	unsigned long long sumsofar = 0;
	
	int i;
	a = rdtsc();
	for(i = 0; i < N; i++){
		i = i;
		rdtsc();
	}
	c = rdtsc();
	sumsofar += (c-a);
	printf("%llu\n", (c-a)/N);
	
	a = rdtsc();
	for(i = 0; i < N; i++){
		i = i;
	}
	c = rdtsc();
	printf("%llu\n", (c-a)/N);
}

int main(int argc, char* argv[]){
	measurement_overhead(10000);
	return 0;
}
