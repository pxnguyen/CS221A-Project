#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include "rdtsc.h"
#include "conv.h"
#define DEF_RNDS 1000
#define DEF_TEST 1
#define DEF_FREQ 800 // * 10^6 cycles per second
#define DEF_OUTPUT "out.data"

// Outputs time in nano seconds... 
/*FIXME - pin it to a single CPU, figure out why there is a diff b/w tests 11 and 12*/
int verbose = 0;
void print_help ()
{
	printf("Usage -t <test number> -n <num_iterations> -r <num_rounds> -f <cpu freq in MHz> -o <test output file> -v <verbose>\n");	
}

void test_mult_proc_exec(int cpu_freq, FILE *fp, int num_rounds, int max_procs, int test_num)
{
	uint64_t start, end, mytime;
	uint64_t start2, end2;
	uint64_t total_time = 0;
	int pid, result, rnd, i;
	int num_iters = 1;
	char *pathname = "./";
	char *argv[2] = {"test", 0};
	
	for (rnd = 0; rnd < num_rounds; rnd++) {

		start = rdtsc();
		for (i = 0; i < max_procs; i++) {
		
			pid = fork();

			/*Child Process*/		
			if (pid == 0) {
				execve(pathname, argv, NULL);
			}
		}
	
		wait(&result);

		for (i = 0; i < max_procs; i++) {
		
			pid = fork();

			/*Child Process*/		
			if (pid == 0) {
				execve(pathname, argv, NULL);
			}
		}

		wait(&result);
		end = rdtsc();

		start2 = rdtsc();
		for (i = 0; i < max_procs; i++) {
		
			pid = fork();

			/*Child Process*/		
			if (pid == 0) {
				execve(pathname, argv, NULL);
			}
		}
	
		wait(&result);
		end2 = rdtsc();

		mytime = convert_cycles_to_ns(cpu_freq, (end - start) - (end2 - start2), max_procs);
		if (verbose) {
			printf("%d %llu\n", rnd, mytime);

		}	
	    	fprintf(fp, "%llu\n", mytime);
		total_time += mytime;	
	}
	
	printf("Average time for testnum %d : %llu ns\n", test_num, total_time / num_rounds);
}

void test_mult_proc_wloop(int cpu_freq, FILE *fp, int num_rounds, int max_procs, int test_num)
{
	uint64_t start, end, mytime;
	uint64_t start2, end2;
	uint64_t total_time = 0;
	int pid, result, rnd, i;
	int num_iters = 1;

	
	for (rnd = 0; rnd < num_rounds; rnd++) {

		start = rdtsc();
		for (i = 0; i < max_procs; i++) {
		
			pid = fork();

			/*Child Process*/		
			if (pid == 0) {
				exit(0);
			}
		}
	
		wait(&result);

		for (i = 0; i < max_procs; i++) {
		
			pid = fork();

			/*Child Process*/		
			if (pid == 0) {
				exit(0);
			}
		}

		wait(&result);
		end = rdtsc();

		start2 = rdtsc();
		for (i = 0; i < max_procs; i++) {
		
			pid = fork();

			/*Child Process*/		
			if (pid == 0) {
				exit(0);
			}
		}
	
		wait(&result);
		end2 = rdtsc();

		mytime = convert_cycles_to_ns(cpu_freq, (end - start) - (end2 - start2), max_procs);
		if (verbose) {
			printf("%d %llu\n", rnd, mytime);

		}	
	    	fprintf(fp, "%llu\n", mytime);
		total_time += mytime;	
	}
	
	printf("Average time for testnum %d : %llu ns\n", test_num, total_time / num_rounds);
}

void test_mult_proc(int cpu_freq, FILE *fp, int num_rounds, int max_procs, int test_num)
{
	uint64_t start, end, mytime;
	uint64_t total_time = 0;
	int pid, result, rnd, i;
	int num_iters = 1;
	
	for (rnd = 0; rnd < num_rounds; rnd++) {

		start = rdtsc();
		for (i = 0; i < max_procs; i++) {
		
			pid = fork();

			/*Child Process*/		
			if (pid == 0) {
				exit(0);
			}
		}
	
		wait(&result);
		end = rdtsc();

		mytime = convert_cycles_to_ns(cpu_freq, (end - start), max_procs);
		if (verbose) {
			printf("%d %llu\n", rnd, mytime);

		}	
	    	fprintf(fp, "%llu\n", mytime);
		total_time += mytime;	
	}
	
	printf("Average time for testnum %d : %llu ns\n", 1, total_time / num_rounds);
}

void test_single_proc(int cpu_freq, FILE *fp, int num_rounds, int test_num)
{
	uint64_t start, end, mytime;
	uint64_t total_time = 0;
	int pid, result, rnd;
	int num_iters = 1;

	for (rnd = 0; rnd < num_rounds; rnd++) {
		start = rdtsc();
		
		pid = fork();

		/*Child Process*/		
		if (pid == 0) {
			exit(0);
		}

		wait(&result);

		end = rdtsc();

		mytime = convert_cycles_to_ns(cpu_freq, (end - start), num_iters);
		if (verbose) {
			printf("%d %llu\n", rnd, mytime);

		}		
	
	    	fprintf(fp, "%llu\n", mytime);
		total_time += mytime;	
	}
	
	printf("Average time for testnum %d : %llu ns\n", 1, total_time / num_rounds);
}
	
int find_max_num_proc()
{
	int max_proc = 0;
	int pid;
	int status;
	int i;

	while (1) {
		pid = fork();
		
		if (pid == 0) {
			exit(0);
		} 
	
		else if (pid == -1) {
			break;
		}
		max_proc++;
	}		

	for(i = 0; i < max_proc; i++) {
		wait(&status);
	}

	return max_proc;
}

int main(int argc, char **argv)
{
	int num_rounds = DEF_RNDS;
	int test_num = DEF_TEST;
	char c;
	char *data_file = DEF_OUTPUT;
	int cpu_freq = DEF_FREQ;
	int max_proc;
	FILE *fid;
	// Get opt setup	

	while ((c = getopt(argc, argv, "r:t:f:o:v:h")) != -1) {
		switch(c) {

			case 'r':
			num_rounds = atoi(optarg);
			break;

			case 't' :
			test_num = atoi(optarg);
			break;
			
			case 'f':
			cpu_freq = atoi(optarg);
			break;

			case 'o':
			data_file = optarg;
			break;

			case 'v':
			verbose = 1;
			break;

			case 'h':
			print_help();
			break;

			case '?' :
			fprintf(stderr, "Unknown option");
			break;	
			}
	
		}

		fid = fopen(data_file, "w");
		//max_proc = find_max_num_proc();	

		max_proc = 1;

		printf("Max procs %d\n", max_proc);

		switch(test_num) {
			case 1:
			test_single_proc(cpu_freq, fid, num_rounds, test_num);
			break;

			case 2:
			test_mult_proc(cpu_freq, fid, num_rounds, max_proc, test_num);
			break;

			case 3:
			test_mult_proc_wloop(cpu_freq, fid, num_rounds, max_proc, test_num);
			break;

			case 4:
			test_mult_proc_exec(cpu_freq, fid, num_rounds, max_proc, test_num);
			break;
		}

		fclose(fid);
	return 0;
}


