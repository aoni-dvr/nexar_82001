#include "serial_port.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int fd = -1;

	serial_init(argv[1], &fd, 0, 115200, 1);
	if (fd < 0) {
		printf("open %s failed\n", argv[1]);
		return -1;
	}
	while (1) {
		unsigned char ch = 0x00;
		if (serial_read(fd, &ch, 1, 2000) < 0) {
			printf("serial read error\n");
			break;
		}
		printf("0x%.2x ", ch);
	}
	serial_close(fd);

	return 0;
}

