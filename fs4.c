#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#include "rdtsc.h"
#define BLK_SIZE 4096
#define MAX_FILES 20
#define FILE_SIZE (100 << 10) 

int keep_time = 1;

void drop_page_cache(char *fname)
{
  int fid = open(fname, O_RDONLY);
  posix_fadvise(fid, 0, 0, POSIX_FADV_DONTNEED);
  close(fid);
}

int main(int argc, char **argv)
{
	unsigned long st, end, j;
	char *fname;
	unsigned long num_file_blks = FILE_SIZE / BLK_SIZE;
	char cmd[100];
	int fid, r, rnds = 100;
	char buf[BLK_SIZE];

	if (argc >= 2) {
		keep_time = atoi(argv[1]);
		fname = argv[2];
		if (argc == 4) {
			rnds = atoi(argv[3]);
		}
	}	

	drop_page_cache(fname);
	fid = open(fname, O_RDONLY | O_DIRECT); 	

	if (fid == -1) {
		printf("error opening file %s\n", fname);
		perror("open");
		return 1;
	}

	if (keep_time) {
	  st = rdtsc();
	}

	for (r = 0; r < rnds; r++) {
	  for (j = 0; j < num_file_blks; j++) {
	    unsigned long off = j * BLK_SIZE;
	    pread(fid, buf, BLK_SIZE, off);
	  }
	}
	
	if (keep_time) {
	  end = rdtsc();
	  sprintf(cmd, "echo %lu >> res.time", end - st);
	  system(cmd);	
	}

	return 0;
}
