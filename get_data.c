#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

void print_help()
{
	printf("Usage -f <filename>");
}

int main(int argc, char **argv)
{
	char c;
	char *fname;
	char *out_file;
	FILE *fid_in, *fid_out;
	char tmp[100], tmp1[100], tmp2[100];
	int iter, time;

	while ((c = getopt(argc, argv, "f:o:")) != -1) {
		switch(c) {

		case 'f':
		fname = optarg;
		break;

		case 'o':
		out_file = optarg;
		break;
		}
	}


	fid_in = fopen(fname, "r");
	fid_out = fopen(out_file, "w");

	while(fscanf(fid_in, "%s%d%s%d%s", tmp, &iter, tmp1, &time, tmp2) != EOF) {
		fprintf(fid_out, "%d \n", time);

		if(feof(fid_in)) {
			break;
		}
	}

	fclose(fid_in);
	fclose(fid_out);

	return 0;
}
