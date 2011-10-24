#include <stdio.h>
#include "rdtsc.h"

void measurement_overhead(int N){
	unsigned long long a,b,c;
	
	int i;
	
	a = rdtsc();
	for(i = 0; i < N; i++){
		i = i;
	}
	c = rdtsc();
	printf("Without rdtsc: %f\n", (c-a)/(double)N);
	
	a = rdtsc();
	for(i = 0; i < N; i++){
		i = i;
		rdtsc();
	}
	c = rdtsc();
	printf("Without rdtsc: %f\n", (c-a)/(double)N);
}

void measure_forloop_overhead(int N){
	unsigned long long a,b,c;
	a = rdtsc();
	rdtsc();
	rdtsc();
	rdtsc();
	rdtsc();
	rdtsc();
	rdtsc();
	rdtsc();
	rdtsc();
	rdtsc();
	c = rdtsc();
	printf("Without for loop: %f\n", (c-a)/10.0);
	
	a = rdtsc();
	int i;
	for(i = 0; i < 10; i++){
		rdtsc();
	}
	c = rdtsc();
	printf("With for loop: %f\n", (c-a)/10.0);
}

void foo(){
	int x = 1+1;
	int y = x;
}

void foo1(int a1){
	int x = 1+1;
	int y = x;
}

void foo2(int a1,int a2){
	int x = 1+1;
	int y = x;
}

void foo3(int a1,int a2, int a3){
	int x = 1+1;
	int y = x;
}

void foo4(int a1,int a2, int a3, int a4){
	int x = 1+1;
	int y = x;
}

void measure_procedure_call(int N){
	unsigned long long a,b,c;
	int i;
	
	a = rdtsc();
	for(i = 0; i < N; i++){
		foo();
	}
	c = rdtsc();
	printf("foo(): %f\n", (c-a)/(double)N);
	
	a = rdtsc();
	for(i = 0; i < N; i++){
		foo1(i);
	}
	c = rdtsc();
	printf("foo1(): %f\n", (c-a)/(double)N);
	
	a = rdtsc();
	for(i = 0; i < N; i++){
		foo2(i,i);
	}
	c = rdtsc();
	printf("foo2(): %f\n", (c-a)/(double)N);
	
	a = rdtsc();
	for(i = 0; i < N; i++){
		foo3(i,i,i);
	}
	c = rdtsc();
	printf("foo3(): %f\n", (c-a)/(double)N);
}

int main(int argc, char* argv[]){
	measurement_overhead(100000);
	measure_forloop_overhead(100000);
	measure_procedure_call(100000);
	return 0;
}
