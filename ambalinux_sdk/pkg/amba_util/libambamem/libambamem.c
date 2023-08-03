#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "libambamem.h"

int writel(const unsigned long phyaddr, const unsigned long value)
{
	int fd_w;
	unsigned long aligned_4k, viraddr, length;
	void *mapped_w;

	//map fd_w offset p_addr to mapped_w, O_SYNC for pgprot_noncached
	if ((fd_w = open(DEV_MEM, O_RDWR | O_SYNC)) == -1) {
		fprintf(stderr, "open:%s\n", strerror(errno));
		return -1;
	}

	aligned_4k = phyaddr;
	aligned_4k >>= 12;
	aligned_4k <<= 12;
	length = phyaddr - aligned_4k + sizeof(long);

	if ((mapped_w = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_w, aligned_4k)) == MAP_FAILED) {
		fprintf(stderr, "mmap:%s\n", strerror(errno));
		close (fd_w);
		return -1;
	}
	close (fd_w);
	//printf("%s 0x%lx mapped to %p\n", DEV_MEM, aligned_4k, mapped_w);

	viraddr = (unsigned long)mapped_w + phyaddr - aligned_4k;
	 *(unsigned int *)viraddr = value;

	if (munmap (mapped_w, length) != 0) {
		fprintf(stderr, "munmap:%s\n", strerror(errno));
	}

	return 0;
}

int loadbin(const char *src_file, const unsigned long phyaddr)
{
	struct stat info;
	int fd_r, fd_w;
	void *mapped_r, *mapped_w;

	if(stat(src_file, &info) != 0) {
		fprintf(stderr, "stat: %s\n", strerror(errno));
		return -1;
	}
	printf("file size %lu\n", info.st_size);

	//map fd_w offset p_addr to mapped_w, , O_SYNC for pgprot_noncached
	if ((fd_w = open(DEV_MEM, O_RDWR | O_SYNC)) == -1) {
		fprintf(stderr, "open:%s\n", strerror(errno));
		return -1;
	}
	if ((mapped_w = mmap(NULL, info.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_w, phyaddr)) == MAP_FAILED) {
		fprintf(stderr, "mmap:%s\n", strerror(errno));
		fprintf(stderr, "0x%lx is %s 4k aligned", phyaddr, (phyaddr & 0xfff) ? "not" : "");
		close (fd_w);
		return -1;
	}
	close (fd_w);
	printf("%s 0x%lx mapped to %p\n", DEV_MEM, phyaddr, mapped_w);

	//map fd_r offset 0 to mapped_r
	if ((fd_r = open (src_file, O_RDONLY)) == -1) {
		fprintf(stderr, "open:%s\n", strerror(errno));
		munmap (mapped_w, info.st_size);
		return -1;
	}
	if ((mapped_r = mmap (NULL, info.st_size, PROT_READ, MAP_SHARED, fd_r, 0)) == MAP_FAILED) {
		fprintf(stderr, "mmap:%s\n", strerror(errno));
		close (fd_r);
		munmap (mapped_w, info.st_size);
		return -1;
	}
	close (fd_r);
	printf("%s mapped to %p\n", src_file, mapped_r);

	printf("memcpy(%p, %p, %lu); \n", mapped_w, mapped_r, info.st_size);
	memcpy(mapped_w, mapped_r, info.st_size);

	if (munmap(mapped_r, info.st_size) != 0) {
		fprintf(stderr, "munmap:%s\n", strerror(errno));
	}
	if (munmap (mapped_w, info.st_size) != 0) {
		fprintf(stderr, "munmap:%s\n", strerror(errno));
	}

	return 0;
}

unsigned int readl(const unsigned long phyaddr, const unsigned long op_size)
{
	int fd_r, i, retval;
	unsigned long length, aligned_4k, viraddr;
	void *mapped_r;

	//map fd_r offset 0 to mapped_r
	if ((fd_r = open (DEV_MEM, O_RDONLY)) == -1) {
		fprintf(stderr, "open:%s\n", strerror(errno));
		return 0xffffffff;
	}

	aligned_4k = phyaddr;
	aligned_4k >>= 12;
	aligned_4k <<= 12;
	length = phyaddr - aligned_4k + op_size;

	if ((mapped_r = mmap (NULL, length, PROT_READ, MAP_SHARED, fd_r, aligned_4k)) == MAP_FAILED) {
		fprintf(stderr, "mmap:%s\n", strerror(errno));
		fprintf(stderr, "0x%lx is %s 4k aligned\n", phyaddr, (phyaddr & 0xfff) ? "not" : "");
		close (fd_r);
		return 0xffffffff;
	}
	close (fd_r);
	//printf("%s 0x%lx mapped to %p\n", DEV_MEM, aligned_4k, mapped_r);

	viraddr = (unsigned long)mapped_r + phyaddr - aligned_4k;
	retval = *(unsigned int *)viraddr;
	for (i = 0; i < op_size; i+=4) {
		printf("0x%08lx: 0x%08x\n", phyaddr + i, *(unsigned int *)(viraddr + i));
	}
	if (munmap(mapped_r, length) != 0) {
		fprintf(stderr, "munmap:%s\n", strerror(errno));
	}

	return retval;
}

int savebin(const char *dst_file, const unsigned long phyaddr, const unsigned long length)
{
	struct stat info;
	int fd_r, fd_w;
	void *mapped_r, *mapped_w;

	if(stat(DEV_MEM, &info) != 0) {
		fprintf(stderr, "stat: %s\n", strerror(errno));
		return -1;
	}
	printf("file length %lu\n", length);

	//map fd_w offset p_addr to mapped_w
	if ((fd_w = open(dst_file, O_RDWR | O_CREAT | O_TRUNC, 777)) == -1) {
		fprintf(stderr, "open:%s\n", strerror(errno));
		return -1;
	}
	lseek (fd_w, length - 1, SEEK_SET);
	if (write (fd_w, "", 1) < 0) {
		fprintf(stderr, "write:%s\n", strerror(errno));
		close (fd_w);
		return -1;
	}
	lseek (fd_w, 0, SEEK_SET);

	if ((mapped_w = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_w, 0)) == MAP_FAILED) {
		fprintf(stderr, "mmap:%s\n", strerror(errno));
		close (fd_w);
		return -1;
	}
	close (fd_w);
	printf("%s mapped to %p\n", dst_file, mapped_w);

	//map fd_r offset 0 to mapped_r
	if ((fd_r = open (DEV_MEM, O_RDONLY)) == -1) {
		fprintf(stderr, "open:%s\n", strerror(errno));
		munmap (mapped_w, length);
		return -1;
	}
	if ((mapped_r = mmap (NULL, length, PROT_READ, MAP_SHARED, fd_r, phyaddr)) == MAP_FAILED) {
		fprintf(stderr, "mmap:%s\n", strerror(errno));
		fprintf(stderr, "0x%lx is %s 4k aligned\n", phyaddr, (phyaddr & 0xfff) ? "not" : "");
		close (fd_r);
		munmap (mapped_w, length);
		return -1;
	}
	close (fd_r);
	printf("%s 0x%lx mapped to %p\n", DEV_MEM, phyaddr, mapped_r);

	printf("memcpy(%p, %p, %lu); \n", mapped_w, mapped_r, length);
	memcpy(mapped_w, mapped_r, length);

	if (munmap(mapped_r, length) != 0) {
		fprintf(stderr, "munmap:%s\n", strerror(errno));
	}
	if ( munmap (mapped_w, length) != 0) {
		fprintf(stderr, "munmap:%s\n", strerror(errno));
	}

	return 0;
}
