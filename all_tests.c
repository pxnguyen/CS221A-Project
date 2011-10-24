#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sched.h>
#include "rdtsc.h"

#define DEF_ITERS 10000
#define DEF_RNDS  10000
#define DEF_TEST 1
#define DEF_FREQ 800 // * MHZ
#define DEF_OUTPUT "out.data"

#define _GNU_SOURCE

void print_help ()
{
  printf("Usage -t <test number> -n <num_iterations> -r <num_rounds> -t <test_num> -f <cpu freq in MHz> -o <test output file> -v <verbose>\n");	
}

unsigned long convert_cycles_to_ns(int cpu_freq, unsigned long long cpu_cycles)
{
	unsigned long long mytime = (cpu_cycles * 1000) / (cpu_freq);
	return mytime;	
}

typedef struct test_params {
  int num_rounds;
  int num_iters;
  char *data_fname;
  FILE *data_fp;
  int test_num;
  int verbose;
  int cpu_affinity;
  int cpu_freq;
  int sleep;
  int warmup;
  int append;
} test_t;

void test_creation_time(test_t *test_params)
{
  int num_rounds = test_params->num_rounds;
  int verbose = test_params->verbose;
  FILE *data_fp = test_params->data_fp;
  int test_num = test_params->test_num;
  int cpu_freq = test_params->cpu_freq;

  int rnd, i;
  uint64_t start, end;
  uint64_t mycycles;
  uint64_t mytime;

  int pid;

  for (rnd = 0; rnd < num_rounds; rnd++) {

	start = rdtsc();
	pid = fork();

	/*Child Process*/		
	if (pid == 0) {
	  end = rdtsc();
	  
	  mycycles = (end - start); 

	  mytime = convert_cycles_to_ns(cpu_freq, mycycles);

	  if (verbose) {
	    printf("Iter %d cycles%llu time %llu ns\n", rnd, mycycles, mytime);
	  }

	  fprintf(data_fp, "%llu %llu\n", mycycles, mytime);

	  exit(0);

	} else {
 	  wait(NULL);
	}
   }
}

void test_syscall_time(test_t *test_params)
{
  int num_rounds = test_params->num_rounds;
  int num_iters = test_params->num_iters;
  int verbose = test_params->verbose;
  FILE *data_fp = test_params->data_fp;
  int test_num = test_params->test_num;
  int cpu_freq = test_params->cpu_freq;

  int rnd, i;
  uint64_t start, end;
  uint64_t mycycles;
  uint64_t mytime;
  uint64_t total_time = 0;
  uint64_t total_cycles = 0;

  int fid;
  char c = 'b';

  /* Open Dev null */
  fid = open("/dev/null", O_RDWR);
  if (fid < 0) {
    perror("/dev/null open failed\n");
    return;
  }

  for (rnd = 0; rnd < num_rounds; rnd++) {
    // Measure time it takes to write to dev null

	if (test_params->sleep) {
	  sleep(1);
	}

	if (test_params->warmup) {
	  write(fid, &c, 1);	
	  write(fid, &c, 1);	
	}	

	start = rdtsc();
	for (i = 0; i < num_iters; i++) {
	  write(fid, &c, 1);	
	}
	end = rdtsc();

	mycycles = (end - start) / num_iters;
	total_cycles += mycycles;

	mytime = convert_cycles_to_ns(cpu_freq, mycycles);
	total_time += mytime;

	if (verbose) {
	  printf("Iter %d cycles%llu time %llu ns\n", rnd, mycycles, mytime);
	}
	fprintf(data_fp, "%llu %llu\n", mycycles, mytime);
  }

  printf("Test %d total_cyles %llu total_time %llu avg_cycles %llu avg_time %llu\n", test_num, total_cycles, total_time, (total_cycles / num_iters), (total_time / num_iters));

  close(fid);
}

void init_test_params(test_t *test_params) 
{
  test_params->num_iters = DEF_ITERS;
  test_params->num_rounds = DEF_RNDS;
  test_params->test_num = DEF_TEST;		
  test_params->data_fname = DEF_OUTPUT;
  test_params->cpu_freq = DEF_FREQ;
  test_params->sleep = 0;
  test_params->warmup = 0;
  test_params->append = 0;

  test_params->verbose = 0;
  test_params->cpu_affinity = 0;
}

int setup_test(test_t *test_params)
{
  if (test_params->append) {
	test_params->data_fp = fopen(test_params->data_fname, "a");
  } else {	
	test_params->data_fp = fopen(test_params->data_fname, "w");
  }

  if (NULL == test_params->data_fp) {
	perror("Error opening data file\n");
	return(1);
  }

  if (test_params->cpu_affinity) {
	char cmnd[100];
	int pid = getpid();
	

	sprintf(cmnd, "%s%d", "taskset -p 1 ", pid);

	printf("%s\n", cmnd);

	if (system(cmnd)) {
		perror("taskset");
		return 1;
	}
  }

  return 0;
}

int main(int argc, char **argv)
{
  test_t test_params;
  init_test_params(&test_params);

  char c;

  while ((c = getopt(argc, argv, "r:n:t:f:o:vshcwa")) != -1) {
	switch(c) {
	  case 'r':
	  	test_params.num_rounds = atoi(optarg);
		break;

		case 'n':
		test_params.num_iters = atoi(optarg);
		break;
		
		case 't' :
		test_params.test_num = atoi(optarg);
		break;
		
		case 'f':
		test_params.cpu_freq = atoi(optarg);
		break;

		case 'o':
		test_params.data_fname = optarg;
		break;

		case 'v':
		test_params.verbose = 1;
		break;

		case 's':
		test_params.cpu_affinity = 1;
		break;

		case 'a':
		test_params.append = 1;
		break;

		case 'h':
		print_help();
		break;

		case 'c':
		test_params.sleep = 1;
		break;

		case 'w':
		test_params.warmup = 1;
		break;
		
		case '?' :
		fprintf(stderr, "Unknown option");
		break;	
	}
  }

  if (setup_test(&test_params)) {
	printf("Exiting, error!\n");
	return 1;
  }

  switch(test_params.test_num) {

	case 1:
	  test_syscall_time(&test_params);
	break;

	case 2:
	  test_creation_time(&test_params);
	break;

	default:
	break;
  }

  return 0;
}
