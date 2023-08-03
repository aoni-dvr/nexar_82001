#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "schdr_util.h"

int main(int argc, char **argv)
{
	int fd, ret;
	uint64_t base, size;
        uint32_t *ptr;

	fd = open("/dev/ambacv", O_SYNC | O_RDONLY);
	if (fd < 0) {
		printf("Can't open device file /dev/ambacv !!!\n");
		exit(-1);
	}

	if (schdr_get_cvshm_info(&base, &size) != 0) {
		printf("Can't get cv shared memory info !!!\n");
		exit(-1);
	}
	printf("CVMEM is [%X -- %X]\n", base, base + size);

	// test cv share memory mmap
	ptr = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, base);
	if (ptr == MAP_FAILED) {
		printf("Can't mmap cv shared memory !!!\n");
	}

	printf("Virtual add is %p, first uint32 is %X\n", ptr, ptr[0]);
	munmap(ptr, size);

#if 0
	// test debug port mmap
	size = 0x1000;
	ptr = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0xED030000);
	printf("Audio clock counter is %X\n", ptr[0x44 >> 2]);
	munmap(ptr, size);
#endif
	close(fd);
	return 0;
}
