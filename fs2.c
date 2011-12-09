#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "rdtsc.h"
#define BLK_SIZE 4096
#define RANDOM_BLK_ZONE (4 << 10)

void drop_page_cache(char *fname)
{
  int fid = open(fname, O_RDONLY);
  posix_fadvise(fid, 0, 0, POSIX_FADV_DONTNEED);
  close(fid);
}

int main(int argc, char **argv)
{
	unsigned long st, end, j, st_random, end_random;
	char *fname;
	char cmd[100];
	int fid;
	char buf[BLK_SIZE];
	unsigned long num_file_blks;
	unsigned long num_random_blks;
	unsigned long blks_per_random_blk = RANDOM_BLK_ZONE / BLK_SIZE;

	fname = argv[1];

	drop_page_cache(fname);
	fid = open(fname, O_RDONLY | O_DIRECT); 	

	if (fid == -1) {
		printf("error opening file %s\n", fname);
		perror("open");
		return 1;
	}

	struct stat file_stats;

	fstat(fid, &file_stats);
	num_file_blks = file_stats.st_size / BLK_SIZE;
	num_random_blks = file_stats.st_size / RANDOM_BLK_ZONE;

	st = rdtsc();
	for (j = 0; j < num_file_blks; j++) {
	    read(fid, buf, BLK_SIZE);
	}
        end = rdtsc();

	st_random = rdtsc();
	for (j = 0; j < num_file_blks; j++) {
		//Find the zone first, and then again a random block
		//unsigned long blk = (rdtsc() % num_random_blks) * RANDOM_BLK_ZONE; 
		//unsigned long off = blk + ((rdtsc() % blks_per_random_blk) * BLK_SIZE);
		unsigned long off = (rdtsc() % num_file_blks) * BLK_SIZE;
		pread(fid, buf, BLK_SIZE, off);
	}
	end_random =  rdtsc();

        sprintf(cmd, "echo %lu %lu >> res2.time", end - st, end_random - st_random);
        system(cmd);

	return 0;

}
