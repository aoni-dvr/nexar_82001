#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "schdr_util.h"

#define OPTIONS         "l:h"

static void print_help(const char *exe)
{
	printf("\nusage: %s [args]\n", exe);
	printf("\t-l arg: directory holding visorc binaries\n");

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
	int c;

	optind = 1;
	while ((c = getopt(argc, argv, OPTIONS)) != -1) {
		switch (c) {
		case 'l':
			load_binary(optarg);
			break;
		default:
			break;
		}
	}

	return 0;
}
