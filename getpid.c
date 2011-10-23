#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include "rdtsc.h"

#define DEF_ITERS 100
#define DEF_RNDS 1
#define DEF_TEST 1

void print_help ()
{
	printf("Usage -n <num_iterations> -r <num_rounds> -t <test_num>\n");	
}


void test_get_pid_today(int num_rounds, int num_iters) {
	int rnd, i;
	struct timeval st_time, end_time;
	unsigned long mytime;	

	for (rnd = 0; rnd < num_rounds; rnd++) {
		gettimeofday(&st_time, NULL);
		for (i = 0; i < num_iters; i++) {
			getpid();
		}
		gettimeofday(&end_time, NULL);

		mytime = ((end_time.tv_sec - st_time.tv_sec) * 1000000) +  end_time.tv_usec - st_time.tv_usec;

		printf("Iter %d time %lu mus\n", rnd, mytime);
	}
}

void test_get_pid_rdtsc(int num_rounds, int num_iters) {
	int rnd, i;
	unsigned long long start, end;	

	for (rnd = 0; rnd < num_rounds; rnd++) {
		start = rdtsc();
		for (i = 0; i < num_iters; i++) {
			getpid();
		}
		end = rdtsc();

		printf("Iter %d time %llu cycles\n", rnd, end - start);
	}
}

void test_devnull_rdtsc(int num_rounds, int num_iters) {
	int rnd, i;
	unsigned long long start, end;	

	for (rnd = 0; rnd < num_rounds; rnd++) {
		start = rdtsc();
		for (i = 0; i < num_iters; i++) {
			system("echo 'hello' > /dev/null");
		}
		end = rdtsc();

		printf("Iter %d time %llu cycles\n", rnd, end - start);
	}
}

void test_devnull_today(int num_rounds, int num_iters) {
	int rnd, i;
	struct timeval st_time, end_time;
	unsigned long mytime;	

	for (rnd = 0; rnd < num_rounds; rnd++) {
		gettimeofday(&st_time, NULL);
		for (i = 0; i < num_iters; i++) {
			system("echo 'hello' > /dev/null");
		}
		gettimeofday(&end_time, NULL);

		mytime = ((end_time.tv_sec - st_time.tv_sec) * 1000000) +  end_time.tv_usec - st_time.tv_usec;

		printf("Iter %d time %lu mus\n", rnd, mytime);
	}
}		

int main(int argc, char **argv)
{
	int num_iters = DEF_ITERS;
	int num_rounds = DEF_RNDS;
	int test_num = DEF_TEST;
	char c;

	// Get opt setup	

	while ((c = getopt(argc, argv, "r:n:t:h")) != -1) {
		switch(c) {

			case 'r':
			num_rounds = atoi(optarg);
			break;

			case 'n':
			num_iters = atoi(optarg);
			break;
			
			case 't' :
			test_num = atoi(optarg);
			break;
			
			case 'h':
			print_help();
			break;

			case '?' :
			fprintf(stderr, "Unknown option");
			break;	
			}
	
		}

		switch(test_num) {
		
		case 1:
		test_get_pid_today(num_rounds, num_iters);
		break;

		case 2:
		test_devnull_today(num_rounds, num_iters);
		break;

		case 11:
		test_get_pid_rdtsc(num_rounds, num_iters);
		break;
		
		case 12:
		test_devnull_rdtsc(num_rounds, num_iters);
		break;

		default:
		break;
		}

	return 0;
}


