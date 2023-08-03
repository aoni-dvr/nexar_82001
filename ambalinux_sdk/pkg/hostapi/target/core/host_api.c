#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <dirent.h>
#include <errno.h>
#include "AmbaHost_api.h"
#include "AmbaHost_msg.h"
#include "AmbaHost_network.h"
#include "private.h"

struct xport_config default_ambahost_daemon = {
	.port = 6969,
	.flags = 0
};

static int    amba_fd;
static char*  mbase;
static size_t msize;

static char *amba_map(uint32_t addr, uint32_t size)
{
	uint32_t aligned = addr & ~0xFFF;

	amba_fd = open("/dev/ambacv", O_DSYNC | O_RDWR );
	if (amba_fd < 0) {
		printf("Can't open device file /dev/ambacv !!!\n");
		return NULL;
	}

	msize = size + (addr - aligned);
	mbase = mmap(NULL, msize, PROT_READ | PROT_WRITE, MAP_SHARED,
		     amba_fd, aligned);

	if (mbase == MAP_FAILED) {
		printf("Can't mmap region [0x%X -- 0x%X], err(%d)\n",
		       addr, addr + size, errno);
		return NULL;
	}
	return (mbase + (addr - aligned));
}

static void amba_unmap(void)
{
	if (mbase != MAP_FAILED) {
		munmap(mbase, msize);
		mbase = MAP_FAILED;
	}

	if (amba_fd > 0) {
		close(amba_fd);
		amba_fd = 0;
	}
}

static int process_loopback(AHMSG_HDR *header, hXport xport)
{
	char message[1024];

	// retrieve loopback message
	if (header->len > sizeof(message)) {
		printf("Loopback message is too long\n");
		return AMBA_HOST_ERR_GENERAL;
	}
	AmbaHost_xport_recv(xport, message, header->len);

	// send the message back
	AmbaHost_xport_lock(xport);
	AmbaHost_xport_send(xport, (const char*)header, sizeof(*header));
	AmbaHost_xport_send(xport, message, header->len);
	AmbaHost_xport_unlock(xport);

	return AMBA_HOST_ERR_OK;
}

static int process_copyfrom(AHMSG_HDR *header, hXport xport)
{
	FILE *fp;
	char buf[4096];
	struct AmbaHost_msg_copyfile copyfile;

	AmbaHost_xport_recv(xport, (char*)&copyfile, sizeof(copyfile));
	AmbaHost_xport_lock(xport);
	AmbaHost_xport_send(xport, (const char*)header, sizeof(*header));

	fp = fopen(copyfile.src_name, "rb");
	if (fp == NULL) {
		/* send reply message with len=0 */
		copyfile.len = 0;
		AmbaHost_xport_send(xport, (const char*)&copyfile,
			sizeof(copyfile));
	} else {
		/* send reply message with acutal length */
		fseek(fp, 0, SEEK_END);
		copyfile.len = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		AmbaHost_xport_send(xport, (const char*)&copyfile,
			sizeof(copyfile));

		/* send the file contents */
		while (copyfile.len > 0) {
			int bytes = copyfile.len;
			if (bytes > sizeof(buf)) bytes = sizeof(buf);
			assert(bytes == fread(buf, 1, bytes, fp));
			AmbaHost_xport_send(xport, buf, bytes);
			copyfile.len -= bytes;
		}
		fclose(fp);
	}

	AmbaHost_xport_unlock(xport);
	return AMBA_HOST_ERR_OK;
}

static int process_copyto(AHMSG_HDR *header, hXport xport)
{
	FILE *fp;
	char buf[4096];
	struct AmbaHost_msg_copyfile copyfile;

	AmbaHost_xport_recv(xport, (char*)&copyfile, sizeof(copyfile));
	AmbaHost_xport_lock(xport);
	AmbaHost_xport_send(xport, (const char*)header, sizeof(*header));

	/* check if dst_name is a directory */
	{
		DIR *directory = opendir(copyfile.dst_name);
		if (directory != NULL) {
			/* create the full dst path */
			char *src_name = strrchr(copyfile.src_name, '/');
			if (src_name == NULL)
				src_name = copyfile.src_name;
			else
				src_name++;
			strcat(copyfile.dst_name, "/");
			strcat(copyfile.dst_name, src_name);
			closedir(directory);
		}
	}

	fp = fopen(copyfile.dst_name, "wb");
	if (fp == NULL) {
		AmbaHost_xport_skip(xport, copyfile.len);
		/* set len=0 to notify remote */
		printf("\tCan't open destination file %s!\n", copyfile.dst_name);
		copyfile.len = 0;
	} else {
		/* receive file contents */
		while (copyfile.len > 0) {
			int bytes = copyfile.len;
			if (bytes > sizeof(buf)) bytes = sizeof(buf);
			AmbaHost_xport_recv(xport, buf, bytes);
			assert(bytes == fwrite(buf, 1, bytes, fp));
			copyfile.len -= bytes;
		}
		fclose(fp);
	}

	AmbaHost_xport_send(xport, (const char*)&copyfile, sizeof(copyfile));
	AmbaHost_xport_unlock(xport);
	return AMBA_HOST_ERR_OK;
}

static int process_read(AHMSG_HDR *header, hXport xport)
{
	struct AmbaHost_msg_memops memops;
	char *ptr;

	AmbaHost_xport_recv(xport, (char *)&memops, sizeof(memops));

	/* send header/memops and the data values */
	AmbaHost_xport_lock(xport);

	ptr = amba_map(memops.tpAddr, memops.size);
	if (ptr != NULL) {
		header->ret = AMBA_HOST_ERR_OK;
		AmbaHost_xport_send(xport, (const char*)header, sizeof(*header));
		AmbaHost_xport_send(xport, (const char*)&memops, sizeof(memops));
		AmbaHost_xport_send(xport, ptr, memops.size);
	} else {
		memops.size = 0;
		header->ret = AMBA_HOST_ERR_INVALID_ADDR;
		AmbaHost_xport_send(xport, (const char*)header, sizeof(*header));
		AmbaHost_xport_send(xport, (const char*)&memops, sizeof(memops));
	}
	amba_unmap();

	AmbaHost_xport_unlock(xport);
	return AMBA_HOST_ERR_OK;
}

static int process_write(AHMSG_HDR *header, hXport xport)
{
	struct AmbaHost_msg_memops memops;
	char *ptr;

	AmbaHost_xport_recv(xport, (char *)&memops, sizeof(memops));

	AmbaHost_xport_lock(xport);

	ptr = amba_map(memops.tpAddr, memops.size);
	if (ptr != NULL) {
		header->ret = AMBA_HOST_ERR_OK;
		AmbaHost_xport_recv(xport, ptr, memops.size);
	} else {
		header->ret = AMBA_HOST_ERR_INVALID_ADDR;
		AmbaHost_xport_skip(xport, memops.size);
		memops.size = 0;
	}
	AmbaHost_xport_send(xport, (const char*)header, sizeof(*header));
	AmbaHost_xport_send(xport, (const char*)&memops, sizeof(memops));
	amba_unmap();

	AmbaHost_xport_unlock(xport);
	return AMBA_HOST_ERR_OK;
}

static int process_msg(AHMSG_HDR *header, hXport xport)
{
	switch (header->type) {
	case AMBA_HOST_MSG_LOOPBACK:
		return process_loopback(header, xport);
	case AMBA_HOST_MSG_COPYFROM:
		return process_copyfrom(header, xport);
	case AMBA_HOST_MSG_COPYTO:
		return process_copyto(header, xport);
	case AMBA_HOST_MSG_READ:
		return process_read(header, xport);
	case AMBA_HOST_MSG_WRITE:
		return process_write(header, xport);
	}
	return AMBA_HOST_ERR_UNKNOWN_TYPE;
}

static void* rx_entry(hXport xport)
{
	AHMSG_HDR header;

	printf("Enter rx entry\n");
	while (AmbaHost_xport_recv(xport, (char*)&header, sizeof(header))) {
		if (process_msg(&header, xport) != AMBA_HOST_ERR_OK) {
			break;
		}
	}
	printf("Exit rx entry\n");
	return NULL;
}

int AmbaHost_init(uint32_t flags, printer_func printer)
{
	default_ambahost_daemon.flags = flags;
	AmbaHost_xport_init(&rx_entry, &default_ambahost_daemon);
	return AMBA_HOST_ERR_OK;
}

int AmbaHost_fini(void)
{
	AmbaHost_xport_fini();
	return AMBA_HOST_ERR_OK;
}
