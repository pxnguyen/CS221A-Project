#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>

#include "rdtsc.h"

#define DEF_ITERS 10000
#define DEF_MAP	  (4096 << 18)
#define PAGE_SIZE 4096

// Test structure
typedef struct test_params {
  int map_fid;
  FILE *out_fid;

  char *map_fname;
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
  tp->map_fid = open(tp->map_fname, O_RDONLY);

  if (tp->map_fid == -1) {
	perror("map file");
	return -1;
   }

  tp->out_fid = fopen(tp->out_fname, "w");
  
  if (NULL == tp->out_fid) {
	perror("Error opening\n");
	return 1;
  }

  posix_fadvise(tp->map_fid, 0, 0, POSIX_FADV_DONTNEED);
  return 0;
}


void page_fault_test_madvise(test_t *tp)
{
  void *map_addr;
  unsigned long long start, end;
  char *c, a;
  unsigned long long addr;	
  int i,j;

  for (i = 0; i < tp->num_iters; i++) {
	map_addr = mmap(NULL, DEF_MAP, PROT_READ, MAP_PRIVATE, tp->map_fid, 0); 
	
	madvise(map_addr, DEF_MAP, MADV_RANDOM);

	j = rdtsc() % tp->num_iters;
	addr = j * (PAGE_SIZE);
	c = (char *) (map_addr + addr);
	start = rdtsc();
	a = *c;	
	end = rdtsc();

	fprintf(tp->out_fid, "%llu\n", end - start);
	munmap(map_addr, DEF_MAP);
  }

}

void page_fault_test(test_t *tp)
{
  void *map_addr;
  unsigned long long start, end;
  char *c, a;
  unsigned long long addr;	
  int i,j;

  for (i = 0; i < tp->num_iters; i++) {
	map_addr = mmap(NULL, DEF_MAP, PROT_READ, MAP_PRIVATE, tp->map_fid, 0); 
	
	j = rdtsc() % tp->num_iters;
	addr = j * (PAGE_SIZE);
	c = (char *) (map_addr + addr);
	start = rdtsc();
	a = *c;	
	end = rdtsc();

	fprintf(tp->out_fid, "%llu\n", end - start);
	munmap(map_addr, DEF_MAP);
  }

}

int main(int argc, char **argv)
{
  test_t test_params;
  init_test_params(&test_params);

  char c;

  while ( (c = getopt(argc, argv, "f:i:m:t:")) != -1) {
	switch(c) {
	  case 'm':
		test_params.map_fname = optarg;
	  break;

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
	  page_fault_test(&test_params);
	break;

	case 2:
	  page_fault_test_madvise(&test_params);
	break;
  }

  return 0;
}
