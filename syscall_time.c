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
#define DEF_FREQ 800 // * 10^6 cycles per second
#define DEF_OUTPUT "out.data"

// Outputs time in nano seconds... 
/*FIXME - pin it to a single CPU, figure out why there is a diff b/w tests 11 and 12*/
int verbose = 0;

void print_help ()
{
	printf("Usage -t <test number> -n <num_iterations> -r <num_rounds> -t <test_num> -f <cpu freq in MHz> -o <test output file> -v <verbose>\n");	
}

void test_get_pid_today(int num_rounds, int num_iters, FILE *fp) 
{
	int rnd, i;
	struct timeval st_time, end_time;
	unsigned long mytime;	
	unsigned long avg_time = 0;	

	for (rnd = 0; rnd < num_rounds; rnd++) {
		gettimeofday(&st_time, NULL);
		for (i = 0; i < num_iters; i++) {
			getpid();
		}
		gettimeofday(&end_time, NULL);

		mytime = ((end_time.tv_sec - st_time.tv_sec) * 1000000) +  end_time.tv_usec - st_time.tv_usec;

		mytime = (mytime * 1000) / num_iters;

		avg_time = avg_time + mytime;
		if (verbose) {
			printf("Iter %d time %lu ns\n", rnd, mytime);
		}
		fprintf(fp, "%lu\n", mytime); 
	}

	printf("Average %lu", avg_time / num_rounds);

}

void test_get_pid_rdtsc(int num_rounds, int num_iters, int cpu_freq, FILE *fp) 
{
	int rnd, i;
	unsigned long long start, end;
	unsigned long long mytime;
	unsigned long long total_time = 0;

	for (rnd = 0; rnd < num_rounds; rnd++) {
		start = rdtsc();
		for (i = 0; i < num_iters; i++) {
			getpid();
		}
		end = rdtsc();

		mytime = ((end - start ) * 1000) / (cpu_freq * num_iters);
		total_time = total_time + mytime;

		if (verbose) {
			printf("Iter %d time %llu ns\n", rnd, mytime);
		}
		fprintf(fp, "%llu\n", mytime);
	}

	printf("Avg time :%llu\n", total_time / num_rounds);
}

void test_devnull_today(int num_rounds, int num_iters, FILE *fp) 
{
	int rnd, i;
	struct timeval st_time, end_time;
	unsigned long mytime;	
	unsigned long avg_time = 0;	

	int fid;
	char *buf = "buffer";
	int size = sizeof(buf);

	fid = open("/dev/null", O_RDWR);
	if (fid < 0) {
		perror("/dev/null open failed\n");
		return;
	}

	for (rnd = 0; rnd < num_rounds; rnd++) {
		gettimeofday(&st_time, NULL);
		for (i = 0; i < num_iters; i++) {
			write(fid, buf, size);	
		}
		gettimeofday(&end_time, NULL);

		mytime = ((end_time.tv_sec - st_time.tv_sec) * 1000000) +  end_time.tv_usec - st_time.tv_usec;

		mytime = (mytime * 1000) / num_iters;

		avg_time = avg_time + mytime;
		if (verbose) {
			printf("%d time %lu ns\n", rnd, mytime);
		}
		fprintf(fp, "%lu\n", mytime); 
	}

	printf("gettimeofday - /dev/null write Average %lu", avg_time / num_rounds);
	close(fid);
}

void test_devnull_rdtsc(int num_rounds, int num_iters, int cpu_freq, FILE *fp) 
{
	int rnd, i;
	unsigned long long start, end;
	unsigned long long mytime;
	unsigned long long total_time = 0;

	int fid;
	char *buf = "buffer";
	//char rd_buf[10];
	int size = sizeof(buf);

	fid = open("/dev/null", O_RDWR);
	if (fid < 0) {
		perror("/dev/null open failed\n");
		return;
	}

	for (rnd = 0; rnd < num_rounds; rnd++) {
		start = rdtsc();
		for (i = 0; i < num_iters; i++) {
			write(fid, buf, size);	
			//read(fid, rd_buf, size);	
		}
		end = rdtsc();

		mytime = ((end - start ) * 1000) / (cpu_freq * num_iters);
		total_time = total_time + mytime;


		if (verbose) {
			printf("Iter %d time %llu ns\n", rnd, mytime);
		}

		fprintf(fp, "%llu\n", mytime);
	}

	printf("Avg time :%llu\n", total_time / num_rounds);
	close(fid);
}

int main(int argc, char **argv)
{
	int num_iters = DEF_ITERS;
	int num_rounds = DEF_RNDS;
	int test_num = DEF_TEST;
	char c;
	char *data_file = DEF_OUTPUT;
	int cpu_freq = DEF_FREQ;
	FILE *fid;
	// Get opt setup	

	while ((c = getopt(argc, argv, "r:n:t:f:o:v:h")) != -1) {
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

		switch(test_num) {
		
		case 1:
		test_get_pid_today(num_rounds, num_iters, fid);
		break;

		case 2:
		test_devnull_today(num_rounds, num_iters, fid);
		break;

		case 11:
		test_get_pid_rdtsc(num_rounds, num_iters, cpu_freq, fid);
		break;
		
		case 12:
		test_devnull_rdtsc(num_rounds, num_iters, cpu_freq, fid);
		break;

		default:
		break;
		}

		fclose(fid);
	return 0;
}


