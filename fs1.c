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
	int fid;
	char buf[BLK_SIZE];

	fname = argv[1];

        sprintf(cmd, "cat %s > /dev/null", fname);
        system(cmd);	

	st = rdtsc();
        system(cmd);	
        end = rdtsc();

        sprintf(cmd, "echo %lu >> res.time", end - st);
        system(cmd);

	return 0;

}
