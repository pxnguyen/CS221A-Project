#include <stdio.h>
#include "rdtsc.h"

unsigned long long measure_latency(int size){
	unsigned long long start, end;
	int **a = (int*) malloc(sizeof(int*)*size);
	int *p;
	int i;
	for (i =0; i < size; i++){
		a[i] = (int *) & a[(i + 32) % size];
	}
	p = a[0];
	start = rdtsc();
	for (i =0; i<1000000; i++)
		p= *p;
	end = rdtsc();
	free(a);
	return end-start;
}

int main(int argc, char* argv[]){
	int i;
	int **a = (int*) malloc(sizeof(int*)*2000*128);
	free(a);
	
	for(i=1; i<4000; i++){
		int size = 200*i;
		unsigned long long time = measure_latency(size);
		printf("%d,%llu\n", size, time);
	}
	return 0;
}
