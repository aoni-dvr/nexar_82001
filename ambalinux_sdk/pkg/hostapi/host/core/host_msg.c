#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include "AmbaHost_api.h"
#include "AmbaHost_msg.h"
#include "AmbaHost_connection.h"
#include "AmbaHost_network.h"
#include "AmbaHost_util.h"
#include "private.h"

#define check_filename_length(name) \
	if (strlen(name) > MAX_FILENAME_SIZE - 1) { \
		EMSG("Filename [%s] is too long\n", name); \
		return AMBA_FILENAME_TOO_LONG; \
	}

int tx_loopback(hXport xport, const char *message)
{
	AHMSG_HDR header;
	DMSG("-->loopback [%s]\n", message);

	header.seqnum = AmbaHost_xport_next_seqnum(xport);
	header.len    = strlen(message) + 1;
	header.type   = AMBA_HOST_MSG_LOOPBACK;
	AmbaHost_xport_send(xport, (const char*)&header, sizeof(header));
	AmbaHost_xport_send(xport, message, header.len);

	return AMBA_HOST_ERR_OK;
}

int rx_loopback(hXport xport, AHMSG_HDR *header)
{
	char message[1024];

	assert(header->len <= sizeof(message));
	AmbaHost_xport_recv(xport, message, header->len);
	EMSG("<--loopback [%s]\n", message);

	EMSG("\t[loopback] %s\n", message);
	return AMBA_HOST_ERR_OK;
}

int tx_copyfrom(hXport xport, const char *dst, const char *src)
{
	AHMSG_HDR header;
	struct AmbaHost_msg_copyfile copyfile;
	DMSG("-->copyfrom: dst=%s src=%s\n", dst, src);

	check_filename_length(dst);
	check_filename_length(src);

	header.seqnum = AmbaHost_xport_next_seqnum(xport);
	header.len    = sizeof(copyfile);
	header.type   = AMBA_HOST_MSG_COPYFROM;
	AmbaHost_xport_send(xport, (const char*)&header, sizeof(header));

	strcpy(copyfile.src_name, src);
	strcpy(copyfile.dst_name, dst);
	copyfile.len  = 0;
	copyfile.crc  = 0;
	AmbaHost_xport_send(xport, (const char*)&copyfile, sizeof(copyfile));

	return AMBA_HOST_ERR_OK;
}

int rx_copyfrom(hXport xport, AHMSG_HDR *header)
{
	struct AmbaHost_msg_copyfile copyfile;
	char buf[4096];
	FILE *fp;

	AmbaHost_xport_recv(xport, (char*)&copyfile, sizeof(copyfile));
	DMSG("<--copyfrom: dst=%s src=%s, len=%d\n",
		copyfile.dst_name, copyfile.src_name, copyfile.len);

	if (copyfile.len == 0) {
		EMSG("\tCan't access source file %s!\n", copyfile.src_name);
		return AMBA_HOST_ERR_GENERAL;
	}

	fp = fopen(copyfile.dst_name, "wb");
	if (fp == NULL) {
		EMSG("\tCan't open destination file %s!\n", copyfile.dst_name);
		AmbaHost_xport_skip(xport, copyfile.len);
		return AMBA_HOST_ERR_GENERAL;
	}

	while (copyfile.len > 0) {
		int bytes = copyfile.len;
		if (bytes > sizeof(buf)) bytes = sizeof(buf);
		AmbaHost_xport_recv(xport, buf, bytes);
		assert(bytes == fwrite(buf, 1, bytes, fp));
		copyfile.len -= bytes;
	}
	fclose(fp);

	return AMBA_HOST_ERR_OK;
}

int tx_copyto(hXport xport, const char *dst, const char *src)
{
	AHMSG_HDR header;
	char buf[4096];
	FILE *fp;
	struct AmbaHost_msg_copyfile copyfile;

	check_filename_length(dst);
	check_filename_length(src);

	fp = fopen(src, "rb");
	if (fp == NULL) {
		EMSG("\tCan't open source file %s!\n", src);
		return AMBA_HOST_ERR_GENERAL;
	}

	/* send the header */
	header.seqnum = AmbaHost_xport_next_seqnum(xport);
	header.len    = sizeof(copyfile);
	header.type   = AMBA_HOST_MSG_COPYTO;
	AmbaHost_xport_send(xport, (const char*)&header, sizeof(header));

	/* send the copyfile request */
	strcpy(copyfile.src_name, src);
	strcpy(copyfile.dst_name, dst);
	fseek(fp, 0, SEEK_END);
	copyfile.len  = ftell(fp);
	copyfile.crc  = 0;
	AmbaHost_xport_send(xport, (const char*)&copyfile, sizeof(copyfile));
	DMSG("-->copyto: dst=%s src=%s, len=%d\n", dst, src, copyfile.len);

	/* send the file contents */
	fseek(fp, 0, SEEK_SET);
	while (copyfile.len > 0) {
		int bytes = copyfile.len;
		if (bytes > sizeof(buf)) bytes = sizeof(buf);
		assert(bytes == fread(buf, 1, bytes, fp));
		AmbaHost_xport_send(xport, buf, bytes);
		copyfile.len -= bytes;
	}
	fclose(fp);

	return AMBA_HOST_ERR_OK;
}

int rx_copyto(hXport xport, AHMSG_HDR *header)
{
	struct AmbaHost_msg_copyfile copyfile;

	AmbaHost_xport_recv(xport, (char*)&copyfile, sizeof(copyfile));
	DMSG("<--copyto: dst=%s src=%s, len=%d\n",
		copyfile.dst_name, copyfile.src_name, copyfile.len);

	return AMBA_HOST_ERR_OK;
}

int tx_read(hXport xport, void *hAddr, uint32_t tpAddr, uint32_t size)
{
	struct AmbaHost_msg_memops memops;
	AHMSG_HDR header;

	/* send the header */
	header.seqnum = AmbaHost_xport_next_seqnum(xport);
	header.len    = sizeof(memops);
	header.type   = AMBA_HOST_MSG_READ;
	AmbaHost_xport_send(xport, (const char*)&header, sizeof(header));

	/* send the "read" message */
	memops.hAddr  = (uint64_t)hAddr;
	memops.tpAddr = tpAddr;
	memops.size   = size;
	AmbaHost_xport_send(xport, (const char*)&memops, sizeof(memops));

	DMSG("-->read: host=%p target=0x%X size=%d\n", hAddr, tpAddr, size);
	return AMBA_HOST_ERR_OK;
}

int rx_read(hXport xport, AHMSG_HDR *header)
{
	struct AmbaHost_msg_memops memops;

	AmbaHost_xport_recv(xport, (char*)&memops, sizeof(memops));
	AmbaHost_xport_recv(xport, (void*)memops.hAddr, memops.size);

	DMSG("<--read: host=%p target=0x%X size=%d\n",
	     (void*)memops.hAddr, memops.tpAddr, memops.size);
	return AMBA_HOST_ERR_OK;
}

int tx_write(hXport xport, void *hAddr, uint32_t tpAddr, uint32_t size)
{
	struct AmbaHost_msg_memops memops;
	AHMSG_HDR header;

	/* send the header */
	header.seqnum = AmbaHost_xport_next_seqnum(xport);
	header.len    = sizeof(memops);
	header.type   = AMBA_HOST_MSG_WRITE;
	AmbaHost_xport_send(xport, (const char*)&header, sizeof(header));

	/* send the "write" message */
	memops.hAddr  = (uint64_t)hAddr;
	memops.tpAddr = tpAddr;
	memops.size   = size;
	AmbaHost_xport_send(xport, (const char*)&memops, sizeof(memops));

	/* send the data values */
	AmbaHost_xport_send(xport, (const char*)hAddr, size);

	DMSG("-->write: host=%p target=0x%X size=%d\n", hAddr, tpAddr, size);
	return AMBA_HOST_ERR_OK;
}

int rx_write(hXport xport, AHMSG_HDR *header)
{
	struct AmbaHost_msg_memops memops;

	AmbaHost_xport_recv(xport, (char*)&memops, sizeof(memops));
	DMSG("<--write: host=%p target=0x%X size=%d\n",
	     (void*)memops.hAddr, memops.tpAddr, memops.size);
	return AMBA_HOST_ERR_OK;
}
