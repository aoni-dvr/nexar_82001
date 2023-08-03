#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include "AmbaHost_api.h"
#include "AmbaHost_msg.h"
#include "AmbaHost_connection.h"
#include "AmbaHost_network.h"
#include "AmbaHost_util.h"
#include "private.h"

#define LOCK()         pthread_mutex_lock(&mutex_con)
#define UNLOCK()       pthread_mutex_unlock(&mutex_con)

static hccb            connection[MAX_CONNECTION_NUM];
static pthread_mutex_t mutex_con = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_log = PTHREAD_MUTEX_INITIALIZER;
static int             cid_curr;
static printer_func    printer;
static int             pipe_fd[2];

hccb* get_hccb(int cid)
{
	int  i;

	if (cid <= 0 || cid > cid_curr) {
		EMSG("CID %d is out of range!!!\n", cid);
		return  NULL;
	}

	LOCK();
	for (i = 0; i < MAX_CONNECTION_NUM; i++) {
		if (cid == connection[i].cid) {
			break;
		}
	}
	UNLOCK();

	if (i == MAX_CONNECTION_NUM) {
		EMSG("Invalid CID %d!!!\n", cid);
		return NULL;
	}

	return &connection[i];
}

void free_hccb(hccb *handle)
{
	LOCK();
	memset(handle, 0, sizeof(*handle));
	UNLOCK();
}

int AmbaHost_printf(const char *format, ...)
{
	char buff[1024];
	int len;
	va_list args;

	va_start(args, format);
	len = vsprintf(buff, format, args);
	va_end(args);

	pthread_mutex_lock(&mutex_log);
	if (printer != NULL) {
		printer(HOSTAPI_LIBRARY_PRINTER_ID, buff, len);
	} else if (pipe_fd[1] != 0) {
		write(pipe_fd[1], buff, len);
	}
	pthread_mutex_unlock(&mutex_log);

	return len;
}

int AmbaHost_init(uint32_t flags, printer_func _printer)
{
	int ret = AMBA_HOST_ERR_OK;

	if (_printer != NULL) {
		printer = _printer;
	} else {
		if (pipe(pipe_fd) < 0) {
			printf("Failed to open pipe for Hostapi\n");
		}
		ret = pipe_fd[0];
	}

	AmbaHost_xport_init(flags);
	return ret;
}

int AmbaHost_fini(void)
{
	AmbaHost_xport_fini();

	if (pipe_fd[0] != 0) {
		close(pipe_fd[1]);
		close(pipe_fd[0]);
	}
	return AMBA_HOST_ERR_OK;
}

int AmbaHost_connection_open(const char *addr, uint32_t flags)
{
	int i, ret;
	hccb *handle = NULL;

	LOCK();
	/* acquire a free slot */
	for (i = 0; i < MAX_CONNECTION_NUM; i++) {
		if (connection[i].cid <= 0) {
			handle = &connection[i];
			handle->cid = ++cid_curr;
			if (cid_curr == 0x7FFFFFFF) {
				cid_curr = 0;
			}
                        handle->is_closing = 0;
                        handle->shell_socket = 0;
			break;
		}
	}
	UNLOCK();

	if (handle == NULL) {
		EMSG("No connection slot available at this moment!!\n");
		return AMBA_HOST_ERR_CONNECTION_MAXOUT;
	}

	strcpy(handle->addr, addr);
	ret = AmbaHost_xport_open(&handle->xport, handle->addr, flags);
	if (ret != AMBA_HOST_ERR_OK) {
		/* release the current slot */
		LOCK();
		handle->cid = ret;
		UNLOCK();
	}

	return handle->cid;
}

int AmbaHost_connection_close(int cid)
{
	GET_AND_CHECK_HCCB(cid);
	AmbaHost_xport_close(handle->xport);
        if (handle->shell_socket) {
                handle->is_closing = 1;
                shutdown(handle->shell_socket, 2);
                close(handle->shell_socket);
        }
	free_hccb(handle);
	return AMBA_HOST_ERR_OK;
}

int AmbaHost_loopback(int cid, const char *message)
{
	GET_AND_CHECK_HCCB(cid);
	hXport xport = handle->xport;
	AHMSG_HDR header;

	AmbaHost_xport_lock(xport);
	tx_loopback(xport, message);
	AmbaHost_xport_recv(xport, (char*)&header, sizeof(header));
	rx_loopback(xport, &header);
	AmbaHost_xport_unlock(xport);

	return AMBA_HOST_ERR_OK;
}

int AmbaHost_get_addr(int cid, char *addr, int max_size)
{
	GET_AND_CHECK_HCCB(cid);
	strncpy(addr, handle->addr, max_size);
	return 0;
}

int AmbaHost_pull_file(int cid, const char *dst, const char *src)
{
	GET_AND_CHECK_HCCB(cid);
	hXport xport = handle->xport;
	AHMSG_HDR header;
	int ret;

	AmbaHost_xport_lock(xport);
	tx_copyfrom(xport, dst, src);
	AmbaHost_xport_recv(xport, (char*)&header, sizeof(header));
	ret = rx_copyfrom(xport, &header);
	AmbaHost_xport_unlock(xport);

	return ret;
}

int AmbaHost_push_file(int cid, const char *dst, const char *src)
{
	GET_AND_CHECK_HCCB(cid);
	hXport xport = handle->xport;
	AHMSG_HDR header;
	int ret;

	AmbaHost_xport_lock(xport);
	ret = tx_copyto(xport, dst, src);
	if(ret == AMBA_HOST_ERR_OK) {
		AmbaHost_xport_recv(xport, (char*)&header, sizeof(header));
		rx_copyto(xport, &header);
	}
	AmbaHost_xport_unlock(xport);

	return ret;
}

int AmbaHost_read_mem(int cid, void *hAddr, uint32_t tpAddr, uint32_t size)
{
	GET_AND_CHECK_HCCB(cid);
	hXport xport = handle->xport;
	AHMSG_HDR header;

	AmbaHost_xport_lock(xport);
	tx_read(xport, hAddr, tpAddr, size);
	AmbaHost_xport_recv(xport, (char*)&header, sizeof(header));
	rx_read(xport, &header);
	AmbaHost_xport_unlock(xport);

	return header.ret;
}

int AmbaHost_write_mem(int cid, void *hAddr, uint32_t tpAddr, uint32_t size)
{
	GET_AND_CHECK_HCCB(cid);
	hXport xport = handle->xport;
	AHMSG_HDR header;

	AmbaHost_xport_lock(xport);
	tx_write(xport, hAddr, tpAddr, size);
	AmbaHost_xport_recv(xport, (char*)&header, sizeof(header));
	rx_write(xport, &header);
	AmbaHost_xport_unlock(xport);

	return header.ret;
}
