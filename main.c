#include <stdio.h>
#include "rdtsc.h"

void measurement_overhead(int N){
	unsigned long long a,b,c;
	
	int i;
	int index = 0;
	
	a = rdtsc();
	for(i = 0; i < N; i++){
		index++;
	}
	c = rdtsc();
	printf("Without rdtsc: %f\n", (c-a)/(double)N);
	
	a = rdtsc();
	for(i = 0; i < N; i++){
		index++;
		rdtsc();
	}
	c = rdtsc();
	printf("With rdtsc: %f\n", (c-a)/(double)N);
}

void measure_random(int N){
	unsigned long long a,b,c;
	int i;
        int index = 0;

        a = rdtsc();
        index++;
        c = rdtsc();
        printf("Measuring random: %f\n", (c-a)/(double)N);
}

void measure_forloop_overhead(int N){
	unsigned long long a,b,c;
	int i;
	int index = 0;
	a = rdtsc();
	for(i = 0; i < N; i++){
		index++;
	}
	c = rdtsc();
	printf("With for loop: %f\n", (c-a)/(double)N);
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

void foo5(int a1,int a2, int a3, int a4, int a5){
        int x = 1+1;
        int y = x;
}

void foo6(int a1,int a2, int a3, int a4, int a5, int a6){
        int x = 1+1;
        int y = x;
}

void foo7(int a1,int a2, int a3, int a4, int a5, int a6, int a7){
        int x = 1+1;
        int y = x;
}




void measure_procedure_call(int N){
	unsigned long long a,b,c;
	int i;

	a = rdtsc();
        for(i = 0; i < N; i++){
                int x = 1+1;
		int y = x;
        }
	c = rdtsc();
	printf("No procedure call: %f\n", (c-a)/(double)N);
	
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

	a = rdtsc();
        for(i = 0; i < N; i++){
                foo4(i,i,i,i);
        }
        c = rdtsc();
        printf("foo4(): %f\n", (c-a)/(double)N);

	a = rdtsc();
        for(i = 0; i < N; i++){
                foo5(i,i,i,i,i);
        }
        c = rdtsc();
        printf("foo5(): %f\n", (c-a)/(double)N);


	a = rdtsc();
        for(i = 0; i < N; i++){
                foo6(i,i,i,i,i,i);
        }
        c = rdtsc();
        printf("foo6(): %f\n", (c-a)/(double)N);

	a = rdtsc();
        for(i = 0; i < N; i++){
                foo7(i,i,i,i,i,i,i);
        }
        c = rdtsc();
        printf("foo7(): %f\n", (c-a)/(double)N);
}

int main(int argc, char* argv[]){
	measurement_overhead(100000);
	measure_forloop_overhead(100000);
	measure_procedure_call(100000000);
	measure_random(10);
	return 0;
}
