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
#include <signal.h>
#include "AmbaHost_api.h"
#include "AmbaHost_network.h"

#define MAX_CONNECTIONS         128

typedef struct _xpcb {
	int             flags;
	int             fd;
	int             seqnum;
	pthread_mutex_t lock;
	pthread_t       rx_thread;
} xpcb;

static xpcb            connection[MAX_CONNECTIONS];

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK()         pthread_mutex_lock(&mutex)
#define UNLOCK()       pthread_mutex_unlock(&mutex)

static pthread_t       server_thread;
static int             server_fd;
static thread_entry_t  rx_entry;

static inline int xport_send(int fd, const char *buff, uint32_t size)
{
	int pos, bytes, flag=1;
	for (pos = 0;  pos < size; pos += bytes) {
		bytes = send(fd, &buff[pos], size - pos, 0);
		if (bytes <= 0) {
			return bytes;
		}
	}
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, 4);
	return size;
}

static inline int xport_recv(int fd, char *buff, uint32_t size)
{
	int pos, bytes;
	for (pos = 0; pos < size; pos += bytes) {
		bytes = recv(fd, &buff[pos], size - pos, 0);
		if (bytes <= 0) {
			return bytes;
		}
	}
	return size;
}

static void sig_handler(int signo)
{
	switch (signo) {
	case SIGTERM:
	case SIGINT:
		printf("\nClosing socket ...\n");
		close(server_fd);
		printf("Exit\n");
		exit(0);
	}
}

static xpcb *get_free_xpcb(void)
{
	int i;
	xpcb *conn = NULL;

	LOCK();
	for (i = 0; i < MAX_CONNECTIONS; i++) {
		if (connection[i].flags == 0) {
			conn = &connection[i];
			conn->flags = 1;
			conn->seqnum = 0;
			pthread_mutex_init(&conn->lock, NULL);
			break;
		}
	}
	UNLOCK();

	return conn;
}

static void* server_entry(void *arg)
{
	struct xport_config *p_cfg = (struct xport_config *)arg;
	struct sockaddr_in addr, cli;
	socklen_t len = sizeof(cli);

	signal(SIGTERM, sig_handler);
	signal(SIGINT,  sig_handler);

	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(p_cfg->port);

	if (bind(server_fd, (const struct sockaddr*)&addr, sizeof(addr)) < 0) {
		printf("Can't bind to port %d\n", p_cfg->port);
		exit(1);
	}

	if (listen(server_fd, MAX_CONNECTIONS) < 0) {
		printf("Can't listen to port %d\n", p_cfg->port);
		exit(1);
	}

	printf("listening on port %d...\n\n", p_cfg->port);
	while (1) {
		xpcb *conn = get_free_xpcb();
		conn->fd = accept(server_fd, (struct sockaddr *)&cli, &len);
		pthread_create(&conn->rx_thread, NULL, rx_entry, conn);
	}
}

int AmbaHost_xport_init(thread_entry_t rx, struct xport_config *xport_conf)
{
	rx_entry = rx;
	pthread_create(&server_thread, NULL, server_entry, (void *)xport_conf);
	return AMBA_HOST_ERR_OK;
}

int AmbaHost_xport_fini(void)
{
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
	return xport_send(handle->fd, buff, size);
}

int AmbaHost_xport_recv(hXport xport, char *buff, uint32_t size)
{
	xpcb *handle = (xpcb*)xport;
	return xport_recv(handle->fd, buff, size);
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
