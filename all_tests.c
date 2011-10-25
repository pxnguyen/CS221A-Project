#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sched.h>
#include <pthread.h>
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

struct shared_pipes {
  int pipe1[2];
  int pipe2[2];

  FILE *data_fp;
  int num_rounds;
};

static void *thread_start(void *ptr)
{
   uint64_t end = rdtsc();
   uint64_t *pend =  (void *)ptr;

   *pend = end;
}

uint64_t test_kthread_time(void)
{
  pthread_t thread1;
  pthread_attr_t attr;
  uint64_t start, pend;
  int tid;

  pthread_attr_init(&attr);

  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  start = rdtsc();
  if (pthread_create(&thread1, &attr, thread_start, (void *)&pend)) {
	perror("pthread error");
	return 0;
  }
	
  pthread_join(thread1, NULL);
  return (pend - start);
}

void test_kthreads(test_t *test_params) 
{
  int num_rounds = test_params->num_rounds;
  int verbose = test_params->verbose;
  uint64_t mycycles, mytime;
  FILE *data_fp = test_params->data_fp;
 
  uint64_t total_cycles = 0;
  uint64_t total_time = 0;

  int rnd;

  for (rnd = 0; rnd < num_rounds; rnd++) {
	mycycles = test_kthread_time();

        if (0 == mycycles) {
		return;
	}		


	if (verbose) {
	  printf("pid %d Iter %d cycles%llu\n", getpid(), rnd, mycycles);
	}

	total_cycles += mycycles;
	fprintf(data_fp, "%llu \n", mycycles);
  }
 
  printf("Avg cycles %llu \n", (total_cycles / num_rounds)); 
}

void test_creation_time(test_t *test_params)
{
  int num_rnds = test_params->num_rounds;
  int verbose = test_params->verbose;
  FILE *data_fp = test_params->data_fp;
  int test_num = test_params->test_num;

  int i;
  uint64_t start, end;
  uint64_t mycycles;

  int pid;

  for (i = 0; i < num_rnds; i++) {

	start = rdtsc();
	pid = fork();

	/*Child Process*/		
	if (pid == 0) {
	  end = rdtsc();
	  
	  mycycles = (end - start); 

	  if (verbose) {
	    printf("pid %d Iter %d cycles%llu \n", getpid(), i, mycycles);
	  }

	  fprintf(data_fp, "%llu \n", mycycles);

	  exit(0);

	} else {
 	  wait(NULL);
	}
   }
}

void print_my_pipes(struct shared_pipes *mypipes) {
	printf(" p1 :%d %d ; p2 :%d %d\n", mypipes->pipe1[0], mypipes->pipe1[1], mypipes->pipe2[0], mypipes->pipe2[1]);
}

static void *thread_fun2(void *ptr)
{
  char *pvt_buf = "kthread2";
  struct shared_pipes *pipes = (struct shared_pipes *)ptr;
  char buf[10];
  int num_rounds = pipes->num_rounds;
  int rnd;

  read(pipes->pipe1[0], buf, 6);
  write(pipes->pipe2[1], pvt_buf, sizeof(pvt_buf));

  #if 0
  for (rnd = 0; rnd < num_rounds; rnd++) {
       read(pipes->pipe1[0], buf, 6);
       //write(pipes->pipe2[1], pvt_buf, sizeof(pvt_buf));
  }
  #endif
	
  close(pipes->pipe1[0]);
  close(pipes->pipe2[1]);
}

static void *thread_fun1(void *ptr)
{
  char *pvt_buf = "kthread1";
  struct shared_pipes *pipes = (struct shared_pipes *)ptr;
  char buf[10];
  int num_rounds = pipes->num_rounds;
  int rnd;

  uint64_t start, end;

  close(pipes->pipe1[0]);
  close(pipes->pipe2[1]);

  start = rdtsc();

  write(pipes->pipe1[1], pvt_buf, sizeof(pvt_buf));
  read(pipes->pipe2[0], buf, 6);
  end = rdtsc();

  fprintf(pipes->data_fp, "%llu \n", (end - start));
  close(pipes->pipe1[1]);
  close(pipes->pipe2[0]);
}

void *thread_pipe_ovhead( void *ptr)
{
  char *pvt_buf = "kthread1";
  struct shared_pipes *pipes = (struct shared_pipes *)ptr;
  char buf[10];
  int num_rounds = pipes->num_rounds;
  int rnd;

  uint64_t start, end;

  start = rdtsc();
  write(pipes->pipe1[1], pvt_buf, sizeof(pvt_buf));
  read(pipes->pipe1[0], buf, 6);
  write(pipes->pipe2[1], pvt_buf, sizeof(pvt_buf));
  read(pipes->pipe2[0], buf, 6);
  end = rdtsc();

  fprintf(pipes->data_fp, "%llu \n", (end - start));
}

void measure_kthread_pipe_overhead(test_t * test_params)
{
  struct shared_pipes mypipes;
  pthread_attr_t attr1;
  FILE *data_fp = test_params->data_fp;
  mypipes.data_fp = data_fp;

  if (pipe(mypipes.pipe1) == -1 ) {
	perror("pipe1");
	return;
  }

  //Create pipes
  if (pipe(mypipes.pipe2) == -1 ) {
    perror("pipe2");
    return;
  }

  pthread_t thread1;
  
  pthread_attr_init(&attr1);
  pthread_attr_setscope(&attr1, PTHREAD_SCOPE_SYSTEM);

  if (pthread_create(&thread1, &attr1, thread_pipe_ovhead, (void *)&mypipes)) {
	perror("pthread error");
  }

  pthread_join(thread1, NULL);
}
	
void test_kthread_ctx_switch(test_t *test_params)
{
  int num_rounds = test_params->num_rounds;
  int verbose = test_params->verbose;
  FILE *data_fp = test_params->data_fp;
  int test_num = test_params->test_num;
  int cpu_freq = test_params->cpu_freq;

  int rnd, i, pid;
  uint64_t start, end;
  uint64_t mycycles;
  uint64_t total_cycles = 0;

  struct shared_pipes mypipes;

  mypipes.data_fp = data_fp;
  mypipes.num_rounds = num_rounds;
  
  pthread_t thread1, thread2;
  pthread_attr_t attr1, attr2;

  if (pipe(mypipes.pipe1) == -1 ) {
	perror("pipe1");
	return;
  }

  //Create pipes
  if (pipe(mypipes.pipe2) == -1 ) {
    perror("pipe2");
    return;
  }

  //print_my_pipes(&mypipes);
  pthread_attr_init(&attr1);
  pthread_attr_setscope(&attr1, PTHREAD_SCOPE_SYSTEM);

  pthread_attr_init(&attr2);
  pthread_attr_setscope(&attr2, PTHREAD_SCOPE_SYSTEM);

  if (pthread_create(&thread1, &attr1, thread_fun1, (void *)&mypipes)) {
	perror("pthread error");
  }

  if (pthread_create(&thread2, &attr2, thread_fun2, (void *)&mypipes)) {
	perror("pthread error");
  }

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
}

void measure_pipe_overhead(test_t *test_params)
{
  int pipe1[2];
  int pipe2[2];
  int rnd, i;

  char buf[100];
  char *pvt_buf = "fixed";

  uint64_t start, end;
  FILE *data_fp = test_params->data_fp;

  if (pipe(pipe1) == -1 ) {
	perror("pipe1");
	return;
  }

  //Create pipes
  if (pipe(pipe2) == -1 ) {
    perror("pipe2");
    return;
  }

  for (rnd = 0; rnd < test_params->num_rounds; rnd ++) {

    start = rdtsc();
    for (i = 0; i < test_params->num_iters; i ++) {
      write(pipe1[1], pvt_buf, sizeof(pvt_buf));
      read(pipe1[0], buf, 6);
      write(pipe2[1], pvt_buf, sizeof(pvt_buf));
      read(pipe2[0], buf, 6);
    }
    end = rdtsc();
    fprintf(data_fp, "%llu \n", ((end - start) / test_params->num_iters));
  }
}

uint64_t test_ctx_switch(test_t *test_params)
{
  int num_iters = test_params->num_iters;
  int num_rounds = test_params->num_rounds;
  int verbose = test_params->verbose;
  int test_num = test_params->test_num;
  int cpu_freq = test_params->cpu_freq;

  int rnd, i, pid;
  uint64_t start, end;
  uint64_t mycycles;
  uint64_t total_cycles = 0;

  char buf[100];
  char *parent_buf = "fixed";

  int pipe1[2];
  int pipe2[2];

  FILE *data_fp = test_params->data_fp;
  //Create pipes
  if (pipe(pipe1) == -1 ) {
	perror("pipe1");
	return;
  }

  //Create pipes
  if (pipe(pipe2) == -1 ) {
    perror("pipe2");
    return;
  }

  pid = fork();

  if (pid == -1) {
	perror("fork");
	return;
  }

  if (pid == 0) {
	char *pvt_buf = "child";
	close(pipe1[1]);
	close(pipe2[0]);

	//Warmup
	read(pipe1[0], buf, 6);
	write(pipe2[1], pvt_buf, sizeof(pvt_buf));

  	for (rnd = 0; rnd < test_params->num_rounds; rnd++) {
	  for (i = 0; i < num_iters; i++) {
		read(pipe1[0], buf, 6);
		write(pipe2[1], pvt_buf, sizeof(pvt_buf));
	  }
	}

	close(pipe1[0]);
	close(pipe2[1]);

  } else {
	close(pipe1[0]);
	close(pipe2[1]);

	//Warmup
	write(pipe1[1], parent_buf, sizeof(parent_buf));
	read(pipe2[0], buf, 5);


  	for (rnd = 0; rnd < num_rounds; rnd++) {
	  start = rdtsc();
	  for (i = 0; i < num_iters; i++) {
		write(pipe1[1], parent_buf, sizeof(parent_buf));
		read(pipe2[0], buf, 5);
	  }
	  end = rdtsc();
  	  mycycles = (end - start);

	  fprintf(data_fp, "%llu \n", mycycles / num_iters);
	}

	wait(NULL);

	close(pipe1[1]);
	close(pipe2[0]);


	if (verbose) {
	  printf("Iter %d cycles%llu \n", rnd, mycycles);
	}
	return mycycles;
  }
}

void test_syscall_getpid(test_t *test_params)
{
  int num_rounds = test_params->num_rounds;
  int num_iters = test_params->num_iters;
  int verbose = test_params->verbose;
  FILE *data_fp = test_params->data_fp;
  int test_num = test_params->test_num;

  int rnd, i;
  uint64_t start, end;
  uint64_t mycycles;
  uint64_t total_cycles = 0;

  for (rnd = 0; rnd < num_rounds; rnd++) {
    // Measure time it takes to write to dev null

	if (test_params->sleep) {
	  sleep(1);
	}

	if (test_params->warmup) {
		getpid();
		getpid();
	}	

	start = rdtsc();
	for (i = 0; i < num_iters; i++) {
	  getpid();
	}
	end = rdtsc();

	mycycles = (end - start) / num_iters;
	total_cycles += mycycles;

	if (verbose) {
	  printf("Iter %d cycles%llu \n", rnd, mycycles);
	}
	fprintf(data_fp, "%llu \n", mycycles);
  }
}

void test_syscall_devnull(test_t *test_params)
{
  int num_rounds = test_params->num_rounds;
  int num_iters = test_params->num_iters;
  int verbose = test_params->verbose;
  FILE *data_fp = test_params->data_fp;
  int test_num = test_params->test_num;

  int rnd, i;
  uint64_t start, end;
  uint64_t mycycles;
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

	if (verbose) {
	  printf("Iter %d cycles%llu ns\n", rnd, mycycles);
	}
	fprintf(data_fp, "%llu \n", mycycles);
  }

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
  int i;

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

	case 0:
	  test_syscall_getpid(&test_params);
	break;

	case 1:
	  test_syscall_devnull(&test_params);
	break;

	case 2:
	  test_creation_time(&test_params);
	break;

	case 3:
	  test_ctx_switch(&test_params);
 	break;
	
	case 4:
	  test_kthreads(&test_params);
 	break;

	case 5:
	  test_kthread_ctx_switch(&test_params);
 	break;

	case 6:
	  measure_kthread_pipe_overhead(&test_params);
	break;

	case 7:
	  measure_pipe_overhead(&test_params);
	break;

	default:
	break;
  }

  return 0;
}
