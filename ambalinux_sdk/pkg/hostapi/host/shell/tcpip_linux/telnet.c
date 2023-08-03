#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include "AmbaHost_api.h"
#include "AmbaHost_msg.h"
#include "AmbaHost_connection.h"
#include "AmbaHost_util.h"

#define IAC_CMD         0xff
#define IAC_WILL        0xFB
#define IAC_WONT        0xFC
#define IAC_DO          0xFD
#define IAC_DONT        0xFE

#define LOCK()         pthread_mutex_lock(&mutex)
#define UNLOCK()       pthread_mutex_unlock(&mutex)
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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
		EMSG("Failed to connect to %s:%d, err=%d\n\r", addr, port, err);
		return AMBA_HOST_ERR_NO_CONNECTION;
	}

	return fd;
}

void negociate(int socket, unsigned char *buf, int len)
{
	int i;

	for (i = 0; i < len; i++) {
		if (buf[i] == IAC_DO)
			buf[i] = IAC_WONT;
		else if (buf[i] == IAC_WILL)
			buf[i] = IAC_DO;
	}

	send(socket, buf, len , 0);
}

static int do_recv(hccb *handle, void *_buf, int size)
{
        int pos, fd = handle->shell_socket;
        char *buf = (char *)_buf;

        for (pos = 0; pos < size; ) {
                int nbytes = recv(fd, buf+pos, size-pos, 0);
                if (nbytes <= 0)
                        return -1;
                pos += nbytes;
        }

        return AMBA_HOST_ERR_OK;
}

static int recv_until(
        hccb *handle,
        unsigned char c,
        int pid,
        printer_func printer)
{
	unsigned char buf[3];
        int sid = handle->shell_socket;

	while (1) {
		if (do_recv(handle, buf, 1) != AMBA_HOST_ERR_OK) {
                        return -1;
                }

		if (buf[0] == IAC_CMD) {
			if (do_recv(handle, buf+1, 2) != AMBA_HOST_ERR_OK) {
                                return -1;
                        }
			negociate(sid, buf, 3);
		} else if (buf[0] != c) {
			if (printer != NULL) {
				printer(pid, (const char *)buf, 1);
			}
		} else {
			break;
		}
	}

	return AMBA_HOST_ERR_OK;
}

static int recv_until2(
        hccb *handle,
        unsigned char c,
        void *arg,
        printer_func2 printer)
{
	unsigned char buf[3];
        int sid = handle->shell_socket;

	while (1) {
		if (do_recv(handle, buf, 1) != AMBA_HOST_ERR_OK) {
                        return -1;
                }

		if (buf[0] == IAC_CMD) {
			if (do_recv(handle, buf+1, 2) != AMBA_HOST_ERR_OK) {
                                return -1;
                        }
			negociate(sid, buf, 3);
		} else if (buf[0] != c) {
			if (printer != NULL) {
				printer(arg, (const char *)buf, 1);
			}
		} else {
			break;
		}
	}

	return AMBA_HOST_ERR_OK;
}

static int recv_skip(hccb *handle, int size)
{
	unsigned char buf[1024];
	int bytes, sid = handle->shell_socket;

	if (size != 0) {
                return do_recv(handle, buf, size);
	} else {
		/* skip anything left in the buffer */
		while (1) {
			bytes = recv(sid, buf, sizeof(buf), MSG_DONTWAIT);
			if (bytes <= 0)
				break;
		}
	}
	return AMBA_HOST_ERR_OK;
}

static int telnet_open(hccb *handle)
{
	int  sid;

	sid = connect_to(handle->addr, 23);
	if (sid > 0) {
                handle->shell_socket = sid;
		recv_until(handle, ':', 0, NULL);
		send(sid, "root\n", 5, 0);
		recv_until(handle, '#', 0, NULL);
	}
	return sid;
}

static void *shell_rx(void *arg)
{
	hscb *shell = (hscb*)arg;
	unsigned char buf[3];
	int bytes, socket, pid, pipefd;
	printer_func printer;
	fd_set fds;

	pid = shell->sid;
	socket = shell->socket;
	printer = shell->printer;
	pipefd = shell->pfd[1];

	//DMSG("start shell rx %d %d\n", pid, socket);
	while (1) {
		FD_ZERO(&fds);
		FD_SET(socket, &fds);
		int ready = select(socket+1, &fds, NULL, NULL, NULL);
		if (ready <= 0) {
			EMSG("select error.\n\r");
			break;
		}
		bytes = recv(socket, buf, 1, 0);
		if (bytes < 0) {
			EMSG("recv error.\n\r");
			break;
		} else if (bytes == 0) {
			EMSG("Connection closed by remote end\n\r");
			if (printer != NULL) {
				printer(pid, NULL, 0);
			}
			break;
		}

		if (buf[0] == IAC_CMD) {
			bytes = recv(socket, buf+1, 2, 0);
			if (bytes <= 0) break;
			negociate(socket, buf, 3);
		} else if (printer != NULL) {
			printer(pid, (char*)buf, 1);
		} else if (pipefd != 0) {
			write(pipefd, buf, 1);
		}
	}

	if (shell->pfd[1] != 0) {
		// close write-end of the pipe to wake up the thread
		// waiting on read
		close(shell->pfd[1]);
	}

	return NULL;
}

int AmbaHost_exec(int cid, printer_func printer, const char *cmd)
{
	GET_AND_CHECK_HCCB(cid);
	int sid = handle->shell_socket, len0, len1;
	static const char marker[] = " ; echo -e '\\010'\n";

	// open a telnet socket if this is the first time we do exec
	if (sid == 0) {
		sid = telnet_open(handle);
		if (sid <= 0)
			return sid;
		handle->shell_socket = sid;
	}

	// skip any message left in the incoming buffer
	recv_skip(handle, 0);

	// send the command then request remote to echo '010'
	len0 = strlen(cmd);
	send(sid, cmd, len0, 0);
	len1 = strlen(marker);
	send(sid, marker, len1, 0);

	// process incoming message until we get '010'
	// this char indicates that the target is done with execution.
	recv_skip(handle, len0+len1);
	recv_until(handle, '\010', cid, printer);

	return AMBA_HOST_ERR_OK;
}

int AmbaHost_exec2(int cid, printer_func2 printer, void *arg, const char *cmd)
{
	GET_AND_CHECK_HCCB(cid);
	int sid = handle->shell_socket, len0, len1, err;
	static const char marker[] = " ; echo -e '\\e'\n";

        // skip if connection s closing
        if (handle->is_closing) {
                return AMBA_HOST_ERR_NO_CONNECTION;
        }

	// open a telnet socket if this is the first time we do exec
	if (sid == 0) {
		sid = telnet_open(handle);
		if (sid <= 0)
			return sid;
	}

	// skip any message left in the incoming buffer
	err = recv_skip(handle, 0);
        if (err != AMBA_HOST_ERR_OK) {
                return err;
        }

	// send the command then request remote to echo '010'
	len0 = strlen(cmd);
	send(sid, cmd, len0, 0);
	len1 = strlen(marker);
	send(sid, marker, len1, 0);

	// process incoming message until we get 'ESC'
	// this char indicates that the target is done with execution.
	err = recv_skip(handle, len0+len1);
        if (err != AMBA_HOST_ERR_OK) {
                return err;
        }

	err = recv_until2(handle, 0x1B, arg, printer);
        if (err != AMBA_HOST_ERR_OK) {
                return err;
        }

	return AMBA_HOST_ERR_OK;
}

int AmbaHost_shell_init(int cid, uint32_t flags)
{
	GET_AND_CHECK_HCCB(cid);
	hscb *shell;
	int i, ret = AMBA_HOST_ERR_NO_SHELL;

	LOCK();
	for (i = 0; i < MAX_SHELL_NUM; i++) {
		if (handle->shell[i].sid == 0) {
			shell = &handle->shell[i];
			shell->sid = (i<<16) | cid;
			ret = shell->sid;
			break;
		}
	}
	UNLOCK();
	return ret;
}

int AmbaHost_shell_fini(int sid)
{
	GET_AND_CHECK_HCCB(sid & 0xFFFF);
	hscb *shell = &handle->shell[sid >> 16];

	LOCK();
	memset(shell, 0, sizeof(*shell));
	UNLOCK();
	return AMBA_HOST_ERR_OK;
}

int AmbaHost_shell_open(int sid, printer_func printer)
{
	GET_AND_CHECK_HCCB(sid & 0xFFFF);
	hscb *shell = &handle->shell[sid >> 16];

	shell->socket = connect_to(handle->addr, 23);
	if (shell->socket < 0)
		return shell->socket;

	if (printer != 0) {
		shell->printer = printer;
	} else {
		if (pipe(shell->pfd) < 0) {
			EMSG("Can't open pipe for shell %d\n", sid);
		}
	}

	pthread_create(&shell->thread, NULL, shell_rx, shell);
	return shell->pfd[0];
}

int AmbaHost_shell_close(int sid)
{
	GET_AND_CHECK_HCCB(sid & 0xFFFF);
	hscb *shell = &handle->shell[sid >> 16];

	shutdown(shell->socket, 2);
	pthread_join(shell->thread, NULL);

	if (shell->pfd[0] != 0) {
		close(shell->pfd[0]);
	}

	return AMBA_HOST_ERR_OK;
}

int AmbaHost_shell_sendstring(int sid, const char *chars, int size)
{
	GET_AND_CHECK_HCCB(sid & 0xFFFF);
	hscb *shell = &handle->shell[sid >> 16];

	LOCK();
	send(shell->socket, chars, size, 0);
	UNLOCK();

	return AMBA_HOST_ERR_OK;
}
