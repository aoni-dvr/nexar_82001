#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include "schdr_util.h"

#define OPTIONS "r:w:l:i:o:c:v:h"

static void print_help(const char *exe)
{
	printf("\nusage: %s [options]\n", exe);
	printf("\t-r arg: read address\n");
	printf("\t-l arg: length of read in bytes, default is 4\n");
	printf("\t-w arg: write address\n");
	printf("\t-v arg: write value, defalut is 0\n");
	printf("\t-o arg: store read values in binary format to a file\n");
	printf("\t-c arg: execute a list of commands from a file\n\n");
	printf("\t\t# comments and empty lines are skipped\n");
	printf("\t\t# read a register value\n");
	printf("\t\tr 0xed030044\n\n");
	printf("\t\t# read a memory chunk of length 0x100 to a file\n");
	printf("\t\tr 0x40000000 0x100 memory.bin\n\n");
	printf("\t\t# write 0xbabeface to a register\n");
	printf("\t\tw 0xed030044 0xbabeface\n\n");
}

static void run_command_file(const char* path)
{
	char *buf;
	FILE *cfp;
	int size;

	cfp = fopen(path, "rb");
	if (cfp == NULL) {
		printf("Can not open command file %s\n", path);
		exit(-1);
	}

	fseek(cfp, 0, SEEK_END);
	size = ftell(cfp);
	buf = malloc(size);
	fseek(cfp, 0, SEEK_SET);
	fread(buf, 1, size, cfp);
	schdr_debug_cmd(buf);
	free(buf);
	fclose(cfp);
}

int main(int argc, char **argv)
{
	int c, len = 4, mode = 0;
	char *iofile="", cmd[256];
	uint32_t addr, wval = 0;

	optind = 1;
	memset(cmd, 0, sizeof(cmd));

	while ((c = getopt(argc, argv, OPTIONS)) != -1) {
		switch (c) {
		case 'r':
		case 'w':
			addr = strtoul(optarg, NULL, 0);
			mode = c;
			break;
		case 'i':
		case 'o':
			iofile = optarg;
			break;
		case 'c':
			run_command_file(optarg);
			exit(0);
		case 'l':
			len = strtoul(optarg, NULL, 0);
			break;
		case 'v':
			wval = strtoul(optarg, NULL, 0);
			break;
		default:
			print_help(argv[0]);
			exit(0);
		}
	}

	if (mode == 'r') {
		sprintf(cmd, "%c 0x%X %d %s", mode, addr, len, iofile);
	} else if (mode == 'w') {
		sprintf(cmd, "%c 0x%X %d", mode, addr, wval);
	} else {
		print_help(argv[0]);
		exit(0);
	}

	schdr_debug_cmd(cmd);
	return 0;
}
