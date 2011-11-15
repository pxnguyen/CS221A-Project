#include <stdio.h>
#include "rdtsc.h"

int main(int argc, char* argv[]){
	unsigned long long start, end;
	int i;
	for(i=1; i<1000; i++){
		int size = 128*i;
		char *source = (char*) malloc(sizeof(char)*size);
		char *dest = (char*) malloc(sizeof(char)*size);

		start = rdtsc();
		bcopy(source,dest,size);
		end = rdtsc();
		printf("%d,%f\n", size, (end-start)/(double)size);
	}
	return 0;
}
