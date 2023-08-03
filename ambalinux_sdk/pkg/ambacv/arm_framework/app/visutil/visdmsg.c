#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "schdr_util.h"

#define OPTIONS         "t:o:h"

static void print_help(const char *exe)
{
	printf("\nusage: %s [args]\n", exe);
	printf("\t-o arg: output file name\n");
	printf("\t-t arg: target log mask, default is 5\n");
	printf("\t    0x1:  SOD scheduler log\n");
	printf("\t    0x2:  SOD cvtask log\n");
	printf("\t    0x4:  VP scheduler log\n");
	printf("\t    0x8:  VP cvtask log\n");
}

static void load_binary(const char *_path)
{
	char *path;

	if (_path[0] == '/') {
 		path = realpath(_path, NULL);
	} else {
		char tmp_path[256];
		getcwd(tmp_path, sizeof(tmp_path));
		strcat(tmp_path, "/");
		strcat(tmp_path, _path);
		path = realpath(tmp_path, NULL);
	}

	//printf("Loading binary from %s...\n", path);
	schdr_load_binary(path);
}

int main(int argc, char **argv)
{
	int c, mode = 0x5;
	char *ofn = NULL;

	optind = 1;
	while ((c = getopt(argc, argv, OPTIONS)) != -1) {
		switch (c) {
		case 't':
			mode = strtoul(optarg, NULL, 0);
			break;
		case 'o':
			ofn = optarg;
			break;
		default:
			print_help(argv[0]);
			exit(0);
		}
	}

	schdr_visorc_dmsg(mode, ofn, 0);
	return 0;
}
