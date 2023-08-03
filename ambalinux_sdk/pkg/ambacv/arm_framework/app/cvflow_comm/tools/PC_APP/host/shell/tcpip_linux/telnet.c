/**
*  @file telnet.c
*
* Copyright (c) [2020] Ambarella International LP
*
* This file and its contents ("Software") are protected by intellectual
* property rights including, without limitation, U.S. and/or foreign
* copyrights. This Software is also the confidential and proprietary
* information of Ambarella International LP and its licensors. You may not use, reproduce,
* disclose, distribute, modify, or otherwise prepare derivative works of this
* Software or any portion thereof except pursuant to a signed license agreement
* or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
* In the absence of such an agreement, you agree to promptly notify and return
* this Software to Ambarella International LP.
*
* This file includes sample code and is only for internal testing and evaluation.  If you
* distribute this sample code (whether in source, object, or binary code form), it will be
* without any warranty or indemnity protection from Ambarella International LP or its affiliates.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
* MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*  @detail xxxx
*
*/
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
        EMSG("Failed to connect to %s:%d, err=%d\n", addr, port, err);
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

    send(socket, buf, len, 0);
}

static int recv_until(int sid, unsigned char c, int pid, printer_func printer)
{
    unsigned char buf[3];

    while (1) {
        recv(sid, buf, 1, 0);
        if (buf[0] == IAC_CMD) {
            recv(sid, buf+1, 2, 0);
            negociate(sid, buf, 3);
        } else if (buf[0] != c) {
            if (printer != NULL) {
                printer(pid, (const char *)buf, 1);
            }
        } else {
            break;
        }
    }

    return 0;
}

static int recv_skip(int sid, int size)
{
    unsigned char buf[1024];
    int bytes;

    if (size != 0) {
        /* skip fix-length of bytes */
        while (size > 0) {
            bytes = recv(sid, buf, size, 0);
            size -= bytes;
        }
    } else {
        /* skip anything left in the buffer */
        while (1) {
            bytes = recv(sid, buf, sizeof(buf), MSG_DONTWAIT);
            if (bytes <= 0)
                break;
        }
    }
    return 0;
}

static int telnet_open(hccb *handle)
{
    int  sid;

    sid = connect_to(handle->addr, 23);
    if (sid > 0) {
        recv_until(sid, ':', 0, NULL);
        send(sid, "root\n", 5, 0);
        recv_until(sid, '#', 0, NULL);
    }
    return sid;
}

static void *shell_rx(void *arg)
{
    hscb *shell = (hscb*)arg;
    unsigned char buf[3];
    int bytes, socket, pid;
    printer_func printer;
    fd_set fds;

    pid = shell->sid;
    socket = shell->socket;
    printer = shell->printer;

    FD_ZERO(&fds);
    FD_SET(socket, &fds);

    //DMSG("start shell rx %d %d\n", pid, socket);
    while (1) {
        int ready = select(socket+1, &fds, NULL, NULL, NULL);
        if (ready <= 0) {
            EMSG("select error.\n");
            break;
        }
        bytes = recv(socket, buf, 1, 0);
        if (bytes < 0) {
            EMSG("\r\nrecv error.\n\r");
            break;
        } else if (bytes == 0) {
            EMSG("\n\rConnection closed by remote end\n\r");
            printer(pid, NULL, 0);
            break;
        }

        if (buf[0] == IAC_CMD) {
            bytes = recv(socket, buf+1, 2, 0);
            if (bytes <= 0) break;
            negociate(socket, buf, 3);
        } else if (printer != NULL) {
            printer(pid, (char*)buf, 1);
        }
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
    recv_skip(sid, 0);

    // send the command then request remote to echo '010'
    len0 = strlen(cmd);
    send(sid, cmd, len0, 0);
    len1 = strlen(marker);
    send(sid, marker, len1, 0);

    // process incoming message until we get '010'
    // this char indicates that the target is done with execution.
    recv_skip(sid, len0+len1);
    recv_until(sid, '\010', cid, printer);

    return AMBA_HOST_ERR_OK;
}

int AmbaHost_shell_open(int cid, printer_func printer, uint32_t flags)
{
    GET_AND_CHECK_HCCB(cid);
    hscb *shell;
    int i, ret = AMBA_HOST_ERR_NO_SHELL;

    LOCK();
    for (i = 0; i < MAX_SHELL_NUM; i++) {
        if (handle->shell[i].socket != 0) {
            ret = connect_to(handle->addr, 23);
            if (ret < 0)
                break;
            shell = &handle->shell[i];
            shell->socket = ret;
            shell->sid = (1<<16) | cid;
            shell->printer = printer;
            pthread_create(&shell->thread, NULL, shell_rx, shell);
            break;
        }
    }
    UNLOCK();
    return ret;
}

int AmbaHost_shell_close(int sid)
{
    GET_AND_CHECK_HCCB(sid & 0xFFFF);
    hscb *shell = &handle->shell[sid >> 16];

    LOCK();
    shutdown(shell->socket, 2);
    pthread_join(shell->thread, NULL);
    memset(shell, 0, sizeof(*shell));
    UNLOCK();

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
