#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <inttypes.h>
#include <assert.h>
#include <errno.h>
#include "AmbaHost_api.h"
#include "AmbaHost_network.h"

typedef struct _xpcb {
	int             fd;
	uint32_t        seqnum;
	pthread_mutex_t lock;
} xpcb;

static int connect_to(const char* addr, int port)
{
	int fd, err;
	struct sockaddr_in serverAddr;

	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	assert(fd >= 0);

	/* open connection to @addr */
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_aton(addr, &serverAddr.sin_addr);

	err = connect(fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (err != 0) {
		printf("Failed to connect to %s:%d, %d\n", addr, port, errno);
		return err;
	}

	return fd;
}

int AmbaHost_xport_init(uint32_t flags)
{
	return AMBA_HOST_ERR_OK;
}

int AmbaHost_xport_fini(void)
{
	return AMBA_HOST_ERR_OK;
}

int AmbaHost_xport_open(hXport *pHandle, const char *addr, uint32_t flags)
{
	xpcb *handle = malloc(sizeof(xpcb));
	int fd;

	fd = connect_to(addr, HOSTAPI_SOCKET_PORT);
	if (fd < 0) {
		free(handle);
		return AMBA_HOST_ERR_NO_CONNECTION;
	}

	handle->fd = fd;
	handle->seqnum = 0;
	pthread_mutex_init(&handle->lock, NULL);

	*pHandle = handle;
	return AMBA_HOST_ERR_OK;
}

int AmbaHost_xport_close(hXport xport)
{
	free(xport);
	return AMBA_HOST_ERR_OK;
}

int AmbaHost_xport_next_seqnum(hXport xport)
{
	xpcb *handle = (xpcb*)xport;
	return handle->seqnum++;
}

int AmbaHost_xport_lock(hXport xport)
{
	xpcb *handle = (xpcb*)xport;
	pthread_mutex_lock(&handle->lock);
	return AMBA_HOST_ERR_OK;
}

int AmbaHost_xport_unlock(hXport xport)
{
	xpcb *handle = (xpcb*)xport;
	pthread_mutex_unlock(&handle->lock);
	return AMBA_HOST_ERR_OK;
}

int AmbaHost_xport_send(hXport xport, const char *buff, uint32_t size)
{
	xpcb *handle = (xpcb*)xport;
	int pos, bytes, fd = handle->fd, flag=1;

	for (pos = 0;  pos < size; pos += bytes) {
		bytes = send(fd, &buff[pos], size - pos, 0);
	}
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, 4);

	return AMBA_HOST_ERR_OK;
}

int AmbaHost_xport_recv(hXport xport, char *buff, uint32_t size)
{
	xpcb *handle = (xpcb*)xport;
	int pos, bytes, fd = handle->fd;

	for (pos = 0; pos < size; pos += bytes) {
		bytes = recv(fd, &buff[pos], size - pos, 0);
	}

	return AMBA_HOST_ERR_OK;
}

int AmbaHost_xport_skip(hXport xport, uint32_t size)
{
	xpcb *handle = (xpcb*)xport;
	int  bytes, fd = handle->fd;
	char buf[4096];

	while (size > 0) {
		bytes = (size < sizeof(buf)) ?  size : sizeof(buf);
		size -= recv(fd, buf, bytes, 0);
	}

	return AMBA_HOST_ERR_OK;
}
