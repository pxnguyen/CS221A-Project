#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "rdtsc.h"

#define DEF_ITERS 3000
#define DEF_MAP	  (4096 << 18)
#define PAGE_SIZE 4096

#define L2_SIZE (3 << 20)
#define NUM_L2 32
#define L2_CLINE (1 << 10)

// Test structure
typedef struct test_params {
  FILE *out_fid;

  char *out_fname;
  int num_iters;
  int test_num;
} test_t;

// Defaults
void init_test_params(test_t *tp)
{
  tp->out_fname = "out.data";
  tp->num_iters = DEF_ITERS;
  tp->test_num = 1;
}

int setup_test(test_t *tp)
{
  tp->out_fid = fopen(tp->out_fname, "w");
  
  if (NULL == tp->out_fid) {
	perror("Error opening\n");
	return 1;
  }

  return 0;
}

void test_bcopy(test_t *tp)
{
  void *src = malloc(NUM_L2/2 * L2_SIZE *sizeof(int));
  void *dst = malloc(NUM_L2/2 * L2_SIZE *sizeof(int));
  int i, ind1, ind2;
  unsigned long long start, end;
 
  for (i = 0; i < NUM_L2/2; i = i + 2) {
	ind1 = L2_SIZE * i;	
	ind2 = L2_SIZE * (i + 1);	

	start = rdtsc();
	memcpy(dst + ind1, src + ind1, L2_SIZE * 4);
	memcpy(dst + ind2, src + ind2, L2_SIZE * 4);
	end = rdtsc();

	fprintf(tp->out_fid, "%llu\n", end - start);
  }
}

void test_write_bw(test_t *tp)
{
  
  int *arr;
  int i, j;
  unsigned long long start, end, off;
  unsigned long ind1, ind2, ind3, ind4, ind5, ind6, ind7, ind8;
  //Unroll Loop 4 times, call loop 30 times...


  arr = malloc(L2_SIZE * NUM_L2 * sizeof(int));

  
  for (j = 0; j < tp->num_iters; j++) {
	off = j * L2_CLINE;
	for (i = 0; i < NUM_L2; i = i + 8) {

	  ind1 = L2_SIZE * i + off;	
	  ind2 = L2_SIZE * (i + 1) + off;	
	  ind3 = L2_SIZE * (i + 2) + off;	
	  ind4 = L2_SIZE * (i + 3) + off;	
	  ind5 = L2_SIZE * (i + 4) + off;	
	  ind6 = L2_SIZE * (i + 5) + off;	
	  ind7 = L2_SIZE * (i + 6) + off;	
	  ind8 = L2_SIZE * (i + 7) + off;	

	  start = rdtsc();
	  arr[ind1] = i;
	  arr[ind2] = i + 1;
	  arr[ind3] = i - 1;
	  arr[ind4] = i + 11;
	  arr[ind5] = i + 2;
	  arr[ind6] = i + 8;
	  arr[ind7] = i + 7;
	  arr[ind8] = i + 3;
	  end = rdtsc();

	  fprintf(tp->out_fid, "%llu\n", end - start);
   	}
   }
}

void test_read_bw(test_t *tp)
{
  
  int *arr;
  int i, j;
  int a;
  unsigned long long start, end, off;
  unsigned long ind1, ind2, ind3, ind4, ind5, ind6, ind7, ind8;
  //Unroll Loop 4 times, call loop 30 times...


  arr = malloc(L2_SIZE * NUM_L2 * sizeof(int));

  for (j = 0; j < tp->num_iters; j++) {
	off = j * L2_CLINE;
	for (i = 0; i < NUM_L2; i = i + 8) {

	  ind1 = L2_SIZE * i + off;	
	  ind2 = L2_SIZE * (i + 1) + off;	
	  ind3 = L2_SIZE * (i + 2) + off;	
	  ind4 = L2_SIZE * (i + 3) + off;	
	  ind5 = L2_SIZE * (i + 4) + off;	
	  ind6 = L2_SIZE * (i + 5) + off;	
	  ind7 = L2_SIZE * (i + 6) + off;	
	  ind8 = L2_SIZE * (i + 7) + off;	

	  start = rdtsc();
	  a = arr[ind1] + arr[ind2];
	  a = arr[ind3] + arr[ind4];
	  a = arr[ind5] + arr[ind6];
	  a = arr[ind7] + arr[ind8];
	  end = rdtsc();
	  fprintf(tp->out_fid, "%llu\n", end - start);
  	}
  }
}

void test_cline(test_t *tp)
{

  int *arr;
  int i;
  int a;
  unsigned long long start, end;
  unsigned long ind1, ind2, ind3, ind4, ind5, ind6, ind7, ind8;
  //Unroll Loop 4 times, call loop 30 times...


  arr = malloc(L2_SIZE * NUM_L2 * sizeof(int));

  for (i=0; i < 100000; i = i+4) {
	start = rdtsc();
	a = arr[i];
	a = arr[i + 1];
	a = arr[i + 2];
	a = arr[i + 3];
//	a = arr[i + 4];
//	a = arr[i + 5];
//	a = arr[i + 6];
//	a = arr[i + 7];
	end = rdtsc();	
	fprintf(tp->out_fid, "%d %llu\n", i, end - start);
  }
}

int main(int argc, char **argv)
{
  test_t test_params;
  init_test_params(&test_params);

  char c;

  while ( (c = getopt(argc, argv, "f:i:t:")) != -1) {
	switch(c) {
	  case 'f':
		test_params.out_fname = optarg;
	  break;

	  case 'i':
		test_params.num_iters = atoi(optarg);
	  break;

	  case 't':
		test_params.test_num = atoi(optarg);
	  break;
	}
  }

  if (setup_test(&test_params)) {
	printf("Error, Exit\n");
	return 1;
  }	

  switch(test_params.test_num) {
	case 1:
	  test_read_bw(&test_params);
	break;
	
	case 2:
	  test_write_bw(&test_params);
	break;

	case 3:
	  test_bcopy(&test_params);
	break;

	case 4:
	  test_cline(&test_params);
	break;
  }

  return 0;
}
