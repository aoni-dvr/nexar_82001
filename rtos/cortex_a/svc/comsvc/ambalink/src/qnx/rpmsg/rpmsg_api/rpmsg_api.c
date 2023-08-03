/**
 * @file rpmsg_api.c
 *  IPC module
 *
 * @defgroup ipc title
 *
 * Copyright (c) 2020 Ambarella International LP
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
 */

#include <stdio.h>
#include <stdlib.h>

#ifdef CONFIG_LINUX
#include <error.h>
#endif

#include <string.h>

#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcpip.h>

#include <unistd.h>
#include <time.h>
#include <stdarg.h>

#include "AmbaTypes.h"
#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
#include "AmbaSafety_IPC.h"
#endif

#include "AmbaRPMSG_priv.h"

#define ANSI_RED      "\033""[1;31m"
#define ANSI_RESET    "\033""[0m"

int rpmsg_api_verbose = 0;
int rpmsg_api_close_stdin = 1;

void IpcDebug(const char *fmt, ...)
{
    if (rpmsg_api_verbose == 1U) {
        char Buf[512];
        va_list args;
        INT32 n;

        va_start(args, fmt);

        n = vsnprintf(Buf, 512-3, (const char *)fmt, args);
        Buf[n] = (UINT8)'\r';
        Buf[n+1] = (UINT8)'\n';
        Buf[n+2] = (UINT8)'\0';
        va_end(args);
        fprintf(stderr, "%s", Buf);
    }
}

/******************************************************************************/
#define MAX_CLIENT_NUM         (10u)
#define CQ_NUM                 MAX_CLIENT_NUM
#define PROTOPORT   7878
#define BUF_SIZE    CONFIG_AMBALINK_G2_RPMSG_SIZE

static amba_rpmsg_cq_t cq_pool[CQ_NUM];
static pthread_mutex_t cq_mutex  = PTHREAD_MUTEX_INITIALIZER;

static int client_sd, client_init;         /* client socket descriptor & connected status */
static AMBA_IPC_HANDLE g_handle;           /* rpmsg channel handle */
static pthread_t thd_client = 0;           /* client pthread_t */
static char channel_name[32] = "g2_echo";  /* rpmsg channel name */

sem_t sem_alloc;
sem_t sem_reg;
sem_t sem_send;

static void init_cq(void)
{
    uint32_t i;

    memset(cq_pool, 0, sizeof(cq_pool));
    for (i = 0u; i < CQ_NUM; i++) {
        cq_pool[i].handle = (AMBA_IPC_HANDLE)-1;
        cq_pool[i].cbfd = client_sd;
    }
}

static void cq_enq(AMBA_IPC_HANDLE handle, AMBA_IPC_MSG_HANDLER_f MsgHandler)
{
    uint32_t i;

    pthread_mutex_lock(&cq_mutex);
    for (i = 0u; i < CQ_NUM; i++) {
        if (cq_pool[i].handle == (AMBA_IPC_HANDLE)-1) {
            cq_pool[i].handle = handle;
            cq_pool[i].call_back = MsgHandler;
            IpcDebug("IPC %s(%d) handle 0x%p cb 0x%p", __func__, __LINE__, handle, MsgHandler);
            break;
        }
    }
    pthread_mutex_unlock(&cq_mutex);
}

static AMBA_IPC_MSG_HANDLER_f cq_get_cb(AMBA_IPC_HANDLE handle)
{
    uint32_t i;
    AMBA_IPC_MSG_HANDLER_f call_back = NULL;

    pthread_mutex_lock(&cq_mutex);
    for (i = 0u; i < CQ_NUM; i++) {
        if (cq_pool[i].handle == handle) {
            call_back = cq_pool[i].call_back;
            break;
        }
    }
    pthread_mutex_unlock(&cq_mutex);

    return call_back;
}

static void cq_dump(void)
{
    uint32_t i;

    pthread_mutex_lock(&cq_mutex);
    for (i = 0u; i < CQ_NUM; i++) {
        if (cq_pool[i].handle != (AMBA_IPC_HANDLE)-1) {
            IpcDebug("%s(%d) handle 0x%p cb 0x%p", __func__, __LINE__, cq_pool[i].handle, cq_pool[i].call_back);
        }
    }
    pthread_mutex_unlock(&cq_mutex);
}

/**
 * lock before send to server
 */
static void rpmsg_lock(sem_t *pSem)
{
    IpcDebug("%s(%d) 0x%p", __func__, __LINE__, pSem);
    sem_wait(pSem);
}

/**
 * unlock after recv from server
 */
static void rpmsg_unlock(sem_t *pSem)
{
    IpcDebug("%s(%d) 0x%p\n", __func__, __LINE__, pSem);
    sem_post(pSem);
}

/**
 * from server: ack
 */
void alloc_acked(int client_sd, amba_rpmsg_pkt_t *pkt)
{
    amba_rpmsg_call_alloc_rsp_t *rsp;
    AMBA_IPC_HANDLE ptr;

    (void)client_sd;
    rsp = (amba_rpmsg_call_alloc_rsp_t *)pkt;

    /* Get handle, saving name */
    ptr = (AMBA_IPC_HANDLE)rsp->handle;

    g_handle = ptr;
    rpmsg_unlock(&sem_alloc);
}

/**
 * from server: ack
 */
void reg_acked(int client_sd, amba_rpmsg_pkt_t *pkt)
{
    amba_rpmsg_call_rsp_t *rsp;

    (void)client_sd;
    rsp = (amba_rpmsg_call_rsp_t *)pkt;
    if (rsp->err != 0) {
        fprintf(stderr, "IPC %s(%d) ret 0x%x\n", __func__, __LINE__, rsp->err);
    }
    rpmsg_unlock(&sem_reg);
}

/**
 * from server: ack
 */
void send_acked(int client_sd, amba_rpmsg_pkt_t *pkt)
{
    amba_rpmsg_call_rsp_t *rsp;

    (void)client_sd;
    rsp = (amba_rpmsg_call_rsp_t *)pkt;
    if (rsp->err != 0) {
        fprintf(stderr, "IPC %s(%d) ret 0x%x\n", __func__, __LINE__, rsp->err);
    }
    rpmsg_unlock(&sem_send);
}

/**
 * msg from server: deliver to callback
 * to server: ack
 */
void _AmbaIPC_Recv(int client_sd, amba_rpmsg_pkt_t *pkt)
{
    ssize_t sk_err;
    struct AmbaIPC_RPDEV_s *rpdev = NULL;
    amba_rpmsg_call_send_t *param;
    amba_rpmsg_call_rsp_t rsp;
    AMBA_IPC_MSG_CTRL_s msg;
    AMBA_IPC_MSG_HANDLER_f call_back;

    /* Get handle, saving name */
    param = (amba_rpmsg_call_send_t *)pkt->param;
    rpdev = param->handle;

    rsp.hdr.magic = AMBA_RPMSG_MAGIC;
    rsp.hdr.cmd =  DCMD_RPMSG_RECV;
    rsp.hdr.len = sizeof(amba_rpmsg_call_rsp_t) - sizeof(amba_rpmsg_pkt_hdr_t);
    rsp.err = 0;

    msg.Length = param->Length;
    msg.pMsgData = param->Data;

    call_back = cq_get_cb(rpdev);
    if (call_back != NULL) {
        /*deliver to app callback*/
        call_back(rpdev, &msg);
    } else {
        /*no callback, internal echo test*/
        IpcDebug("IPC no callback %s(%d) %s", __func__, __LINE__, param->Data);
    }

    sk_err = send(client_sd, (const void*)&rsp, sizeof(rsp), MSG_NOSIGNAL);
    if (sk_err != sizeof(rsp)) {
        fprintf(stderr, ANSI_RED "IPC %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
    }
}

/**
 * from server
 */
static void parse_rpmsg_cmd(int client_sd, amba_rpmsg_pkt_t *pkt)
{
    IpcDebug("%s(%d) len %d", __func__, __LINE__, pkt->hdr.len);

    if (pkt->hdr.magic == AMBA_RPMSG_MAGIC) {
        switch (pkt->hdr.cmd) {
            case DCMD_RPMSG_ALLOC:
                alloc_acked(client_sd, pkt);
                break;

            case DCMD_RPMSG_REG:
                reg_acked(client_sd, pkt);
                break;

            case DCMD_RPMSG_SEND:
                send_acked(client_sd, pkt);
                break;
            case DCMD_RPMSG_RECV:
                _AmbaIPC_Recv(client_sd, pkt);
                break;
            default:
                fprintf(stderr, ANSI_RED "IPC %s(%d) Incorrect cmd: 0x%X\n" ANSI_RESET, __func__, __LINE__, pkt->hdr.cmd);
            }
     } else {
         /* Incorrect packets */
         fprintf(stderr, ANSI_RED "IPC %s(%d) Incorrect packet (magic, cmd) = (0x%08X, 0x%08X)\n" ANSI_RESET, __func__, __LINE__, pkt->hdr.magic, pkt->hdr.cmd);
     }
}

/**
 * to server: AmbaIPC_Alloc
 */
static int _AmbaIPC_Alloc(const char *pName)
{
    ssize_t sk_err, sk_len;
    amba_rpmsg_pkt_t pkt;
    amba_rpmsg_call_alloc_t *param;

    memset(&pkt, 0, sizeof(pkt));
    pkt.hdr.magic = AMBA_RPMSG_MAGIC;
    pkt.hdr.cmd = DCMD_RPMSG_ALLOC;
    pkt.hdr.len = sizeof(amba_rpmsg_call_alloc_t);
    param = (amba_rpmsg_call_alloc_t *)pkt.param;
    memcpy(param->name, pName, strlen(pName));

    sk_len = sizeof(amba_rpmsg_pkt_hdr_t) + sizeof(amba_rpmsg_call_alloc_t);
    sk_err = write(client_sd, (const void *)&pkt, sk_len);

    if (sk_err != sk_len) {
        fprintf(stderr, ANSI_RED "IPC %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
        return -1;
    } else {
        IpcDebug("%s(%d) sent %ld bytes", __func__, __LINE__, sk_err);
        return 0;
    }
}

/**
 * to server: AmbaIPC_RegisterChannel
 */
static int _AmbaIPC_RegisterChannel(AMBA_IPC_HANDLE Channel)
{
    ssize_t sk_err, sk_len;
    amba_rpmsg_pkt_t pkt;
    amba_rpmsg_call_reg_t *param;

    memset(&pkt, 0, sizeof(pkt));
    pkt.hdr.magic = AMBA_RPMSG_MAGIC;
    pkt.hdr.cmd =  DCMD_RPMSG_REG;
    pkt.hdr.len = sizeof(amba_rpmsg_call_reg_t);
    param = (amba_rpmsg_call_reg_t *)pkt.param;
    param->handle = Channel;

    sk_len = sizeof(amba_rpmsg_pkt_hdr_t) + sizeof(amba_rpmsg_call_reg_t);
    sk_err = write(client_sd, (const void *)&pkt, sk_len);
    if (sk_err != sk_len) {
        fprintf(stderr, ANSI_RED "IPC %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
        return -1;
    } else {
        IpcDebug("%s(%d) sent %ld bytes", __func__, __LINE__, sk_err);
        return 0;
    }
}

/**
 * to server: AmbaIPC_Send
 */
static int _AmbaIPC_Send(AMBA_IPC_HANDLE Channel, void *pData, INT32 Length)
{
    ssize_t sk_err, sk_len;
    amba_rpmsg_pkt_t pkt;
    amba_rpmsg_call_send_t *param;

    param = (amba_rpmsg_call_send_t *)pkt.param;
    param->handle = Channel;
    param->Length = Length;
    memcpy(param->Data, pData, param->Length);

    pkt.hdr.magic = AMBA_RPMSG_MAGIC;
    pkt.hdr.cmd =  DCMD_RPMSG_SEND;
    pkt.hdr.len = sizeof(param->handle) + sizeof(param->Length) + param->Length;;

    sk_len = sizeof(amba_rpmsg_pkt_hdr_t) + pkt.hdr.len;
    sk_err = write(client_sd, (const void *)&pkt, sk_len);
    if (sk_err != sk_len) {
        fprintf(stderr, ANSI_RED "IPC %s(%d) write %ld bytes, but return %ld \n" ANSI_RESET, __func__, __LINE__, sk_len, sk_err);
        return -1;
    } else {
        IpcDebug("%s(%d) sent %ld bytes", __func__, __LINE__, sk_err);
        return 0;
    }
}

#define FREEBUF(void) do { \
    if(buf_ptr != NULL) { \
        free(buf_ptr); \
        buf_ptr = NULL;    \
    } \
} while(0);

/**
 * this thread talks to server
 */
static void *rpmsg_client(void *arg)
{
    char serverhost[32] = "127.0.0.1";
    struct sockaddr_in sad; /* structure to hold an IP address */
    struct timeval tv; /* select timeout */
    FILE *FILESTDIN = fdopen(0, "r");
    ssize_t nbytes = 0;
    size_t ind = 0;
    fd_set readfd;
    char *buf_ptr = NULL;
    memset((char *)&sad, 0, sizeof(sad));
    sad.sin_family      = AF_INET;
    sad.sin_port        = htons((unsigned short)PROTOPORT);
    sad.sin_addr.s_addr = inet_addr(serverhost);

    sem_init(&sem_alloc, 1, 1);
    sem_init(&sem_reg, 1, 1);
    sem_init(&sem_send, 1, 1);

    (void)arg;
    client_sd = socket(PF_INET, SOCK_STREAM, 0);
    if (client_sd < 0) {
        fprintf(stderr, ANSI_RED "IPC %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
        return NULL;
    }

    IpcDebug("connecting %s %d\n", serverhost, PROTOPORT);
    if (connect(client_sd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
        fprintf(stderr, ANSI_RED "IPC %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
        close(client_sd);
        return NULL;
    }

    init_cq();
    client_init = 1;
    IpcDebug("connected to sd%d\n", client_sd);

    for (;;) {
        tv.tv_sec = 60;
        tv.tv_usec = 0;

        FD_ZERO(&readfd);
        FD_SET(client_sd, &readfd);
        if (0 == rpmsg_api_close_stdin) {
            FD_SET(0, &readfd);
        }
        if ((select(client_sd + 1, &readfd, NULL, NULL, &tv)) <= 0) {
            IpcDebug("select %lu sec timeout\n", tv.tv_sec);
            /*keep-alive*/
            send(client_sd, "ping", strlen("ping")+1, MSG_NOSIGNAL);
        }

        if ((0 == rpmsg_api_close_stdin) && (FD_ISSET(0, &readfd))) {
            FREEBUF();
            if ((nbytes = getline(&buf_ptr, &ind, FILESTDIN)) <= 0) {
                break;
            }
            strtok(buf_ptr, "\n");
            IpcDebug("[%s]", buf_ptr);

            if (0 == strncmp(buf_ptr, "help", strlen("help"))) {
                fprintf (stderr, "    name g2_echo    set channel name\n");
                fprintf (stderr, "    alloc    do _AmbaIPC_Alloc\n");
                fprintf (stderr, "    reg    do AmbaIPC_RegisterChannel\n");
                fprintf (stderr, "    handle 0x19db4e97b8    set AmbaIPC handle\n");
                fprintf (stderr, "    send    do AmbaIPC_Send\n");
            }
            else if (0 == strncmp(buf_ptr, "alloc", strlen("alloc"))) {
                _AmbaIPC_Alloc(channel_name);
            }
            else if (0 == strncmp(buf_ptr, "reg", strlen("reg"))) {
                cq_enq(g_handle, NULL);
                AmbaIPC_RegisterChannel(g_handle, NULL);
            }
            else if (0 == strncmp(buf_ptr, "send", strlen("send"))) {
                AmbaIPC_Send(g_handle, "hello from QNX", strlen("hello from QNX") + 1);
            }
            else if (0 == strncmp(buf_ptr, "handle", strlen("handle"))) {
                char *tokp;
                if ((tokp = strtok(buf_ptr, "\t \n" )) != NULL) {
                    if ((tokp = strtok(NULL, "\t \n" )) != NULL) {
                        g_handle = (void *)strtoll(tokp, NULL, 16);
                        printf("handle %p\n", g_handle);
                    }
                }
            }
            else if (0 == strncmp(buf_ptr, "name", strlen("name"))) {
                char *tokp;
                if ((tokp = strtok(buf_ptr, "\t \n" )) != NULL) {
                    if ((tokp = strtok(NULL, "\t \n" )) != NULL) {
                        strcpy(channel_name, tokp);
                        printf("channel name %s\n", channel_name);
                    }
                }
            }
            else if (0 == strncmp(buf_ptr, "dump", strlen("dump"))) {
                cq_dump();
            }
            else {
                write(client_sd, buf_ptr, strlen(buf_ptr)+1);
            }
        }

        if (FD_ISSET(client_sd, &readfd)) {
            const ssize_t hdr_len = sizeof(amba_rpmsg_pkt_hdr_t);
            amba_rpmsg_pkt_t *pkt;
            uint32_t payload_len;

            FREEBUF();
            buf_ptr = (char *)malloc(BUF_SIZE);
            pkt = (amba_rpmsg_pkt_t *)buf_ptr;

            if ((nbytes = recv(client_sd, buf_ptr, hdr_len, 0)) <= 0) {
                IpcDebug("connection closed\n");
                close(client_sd);
                break;
            }

            buf_ptr[nbytes]='\0';
            IpcDebug("\n|%s|", buf_ptr);

            if (pkt->hdr.magic == AMBA_RPMSG_MAGIC) {
                payload_len = pkt->hdr.len;
                if ((nbytes = recv(client_sd, buf_ptr + hdr_len, payload_len, 0)) <= 0) {
                    IpcDebug("connection closed\n");
                    close(client_sd);
                    break;
                }
                if (nbytes != payload_len) {
                    fprintf(stderr, ANSI_RED "IPC %s(%d) recv %d bytes, but return %ld \n" ANSI_RESET, __func__, __LINE__, payload_len, nbytes);
                } else {
                    /*rpmsg control msg from remote*/
                    parse_rpmsg_cmd(client_sd, (amba_rpmsg_pkt_t *)pkt);
                }
            }
        }
    }

    close(client_sd);
    return NULL;
}

/**
 * @ingroup ipc
 * Allocate an IPC channel and specify received message handler
 *
 * @param [in] rpmsgID Device ID, e.g., RPMSG_DEV_OAMP or RPMSG_DEV_AMBA
 * @param [in] pName channel name
 * @param [in] MsgHandler channel message handler
 * @return AMBA_IPC_HANDLE
 */
AMBA_IPC_HANDLE AmbaIPC_Alloc(UINT32 rpmsgID, const char *pName, AMBA_IPC_MSG_HANDLER_f MsgHandler)
{
    AMBA_IPC_HANDLE ptr = NULL;

    /* arg check */
    if ((rpmsgID != RPMSG_DEV_OAMP) && (rpmsgID != RPMSG_DEV_AMBA)) {
        ptr = NULL;
    } else if (pName == NULL) {
        ptr = NULL;
    } else {
        /* code start */
        int ms;
        if (thd_client == 0) {
            pthread_create(&thd_client, NULL, rpmsg_client, NULL);
        }
        rpmsg_lock(&sem_alloc);

        ms = 0;
        while (client_init == 0) {
            usleep(1000);
            ms++;
            if (ms >= 1000) {
                fprintf(stderr, ANSI_RED "IPC %s(%d) err\n" ANSI_RESET, __func__, __LINE__);
                break;
            }
        }

        g_handle = NULL;
        _AmbaIPC_Alloc(pName);

        ms = 0;
        while (g_handle == NULL) {
            usleep(1000);
            ms++;
            if (ms >= 3000) {
                fprintf(stderr, ANSI_RED "IPC %s(%d) err\n" ANSI_RESET, __func__, __LINE__);
                break;
            }
        }
    }
    ptr = g_handle;
    if (ptr != NULL) {
        cq_enq(ptr, MsgHandler);
    }

    return ptr;
}

/**
 * @ingroup ipc
 * Register a RPMsg channel and send IPC registration message to remote processor
 *
 * @param [in] Channel IPC handle
 * @param [in] pRemote client rpmsg bus name, specify NULL to use system default
 * @return ipc error number
 */
INT32 AmbaIPC_RegisterChannel(AMBA_IPC_HANDLE Channel, const char *pRemote)
{
    INT32 ret = -1;

    /* [MISRA Rule 8.13] could be declared as pointing to const */
    (void)Channel;
    (void)pRemote;

    /* arg check */
    if (Channel == NULL) {
        ret = (INT32)IPC_ERR_EINVAL;
    } else {
        /* code start */
        rpmsg_lock(&sem_reg);
        ret = _AmbaIPC_RegisterChannel(Channel);
    }

    return ret;
}

/**
 * @ingroup ipc
 * Send message to remote processor
 * If no buffer is available, it will be blocked until a buffer becomes available.
 * pData will be copied to buffer.
 *
 * @param [in] Channel IPC handle
 * @param [in] pData message buffer
 * @param [in] Length message length
 * @return ipc error number
 */
INT32 AmbaIPC_Send(AMBA_IPC_HANDLE Channel, void *pData, INT32 Length)
{
    INT32 ret = -1;

    /* [MISRA Rule 8.13] could be declared as pointing to const */
    (void)pData;
    (void)Channel;

    /* arg check */
    if (Channel == NULL) {
        ret = (INT32)IPC_ERR_EINVAL;
    } else if (pData == NULL) {
        ret = (INT32)IPC_ERR_EINVAL;
    } else if (Length == 0) {
        ret = (INT32)IPC_ERR_EINVAL;
    } else {
        /* code start */
        rpmsg_lock(&sem_send);
        ret = _AmbaIPC_Send(Channel, pData, Length);
    }
    return ret;
}
