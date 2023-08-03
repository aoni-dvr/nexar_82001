/**
 * @file main.c
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
 *
 * @details Main function for rpmsg daemon.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/tcpip.h>

#include <sys/select.h>
#include <semaphore.h>
#include <pthread.h>
#ifdef CONFIG_QNX
#include <sys/procmgr.h>
#endif
#include <sched.h>

// Debug: dump memory information
#ifdef DUMP_BACKTRACE
#include <backtrace.h>
#include <errno.h>
#endif
#include <time.h>
#include <stdarg.h>

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaIPC_RPMSG.h"
#include "AmbaRPMSG_priv.h"

//#define BUILTIN_G2_ECHO

#define ANSI_RED      "\033""[1;31m"
#define ANSI_RESET    "\033""[0m"

int rpmsg_core_verbose = 0;
int rpmsg_core_close_stdin = 1;

void IpcDebug(const char *fmt, ...)
{
    if (rpmsg_core_verbose == 1U) {
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

#ifdef BUILTIN_G2_ECHO
/******************************************************************************/
#define RPMSG_APP_PROMPT    "\ng2_echo > "

void prompt_loop(AMBA_IPC_HANDLE hdl)
{
    FILE *fp;
    char *p;
    static char g2_buf[1024];

    (void)hdl;
    fp = fopen(ttyname(fileno(stdin)), "r");
    if(fp != NULL) {
        do {
            fprintf(stderr, "%s", RPMSG_APP_PROMPT);
            memset(g2_buf, 0, sizeof(g2_buf));
            p = fgets(g2_buf, sizeof(g2_buf), fp);
            if (p == NULL) {
                break;
            }
            g2_buf[strnlen(g2_buf, sizeof(g2_buf)) - 1] ='\0';
            fprintf(stderr, "%s len %ld\n", g2_buf, strlen(g2_buf) + 1);
            rpdev_send(hdl, g2_buf, strlen(g2_buf) + 1);
        } while (1);

        fclose(fp);
        return;
    } else {
        IpcDebug("IPC %s(%d) Can NOT open terminal", __func__, __LINE__);
    }

    return;
}

INT32 g2_echo_handler(AMBA_IPC_HANDLE IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl)
{
    fprintf(stderr, "%s: %s len %d\n", __func__, (char *)(pMsgCtrl->pMsgData), pMsgCtrl->Length);
    rpdev_send(IpcHandle, (char *)(pMsgCtrl->pMsgData), pMsgCtrl->Length);
    (void)IpcHandle;
    return 0;
}

static void QnxRpmsgCB(struct AmbaIPC_RPDEV_s *rpdev, void *data, UINT32 len, void *priv, UINT32 src)
{
    AMBA_IPC_MSG_CTRL_s msg;

    (void)priv;
    (void)src;
    msg.Length = (UINT32)len;
    msg.pMsgData = data;
    g2_echo_handler(rpdev, &msg);
}

int32_t main(int32_t argc, char *argv[])
{
    struct AmbaIPC_RPDEV_s *rpdev;
    void AmbaLink_LinkCtrlCfg(void);
    void AmbaLink_MemPool_Init(void);
    void AmbaIPC_RpmsgInit(void);

    AmbaLink_LinkCtrlCfg();
    AmbaLink_MemPool_Init();
    AmbaIPC_RpmsgInit();

    (void)argc;
    (void)argv;
    rpmsg_core_verbose = 1;
    rpdev = rpdev_alloc("g2_echo", 0, QnxRpmsgCB, (AMBA_IPC_HANDLE)0xdeadbeef);
    IpcDebug("IPC %s(%d) rpdev %p", __func__, __LINE__, (void *)rpdev);
    rpdev_register(rpdev, "c0_and_c1");
    prompt_loop(rpdev);
}
#else  //BUILTIN_G2_ECHO

/******************************************************************************/
#define MAX_CLIENT_NUM         (10u)
#define CQ_NUM                 MAX_CLIENT_NUM
#define PROTOPORT              7878
#define BUF_SIZE               CONFIG_AMBALINK_G2_RPMSG_SIZE

static amba_rpmsg_cq_t cq_pool[CQ_NUM];
static pthread_mutex_t cq_mutex  = PTHREAD_MUTEX_INITIALIZER;

static sem_t sem_recv;

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

static void init_cq(void)
{
    uint32_t i;

    memset(cq_pool, 0, sizeof(cq_pool));
    for (i = 0u; i < CQ_NUM; i++) {
        cq_pool[i].handle = (AMBA_IPC_HANDLE)-1;
    }
}

static void cq_enq(AMBA_IPC_HANDLE handle, int32_t cbfd)
{
    uint32_t i;

    pthread_mutex_lock(&cq_mutex);
    for (i = 0u; i < CQ_NUM; i++) {
        if (cq_pool[i].handle == (AMBA_IPC_HANDLE)-1) {
            cq_pool[i].handle = handle;
            cq_pool[i].cbfd = cbfd;
            IpcDebug("IPC %s(%d) handle 0x%p cbfd: %d", __func__, __LINE__, handle, cbfd);
            break;
        }
    }
    pthread_mutex_unlock(&cq_mutex);
}

static int32_t cq_get_cbfd(AMBA_IPC_HANDLE handle)
{
    uint32_t i;
    int32_t cbfd = -1;

    pthread_mutex_lock(&cq_mutex);
    for (i = 0u; i < CQ_NUM; i++) {
        if (cq_pool[i].handle == handle) {
            cbfd = cq_pool[i].cbfd;
            break;
        }
    }
    pthread_mutex_unlock(&cq_mutex);

    return cbfd;
}

static void cq_dump(void)
{
    uint32_t i;

    pthread_mutex_lock(&cq_mutex);
    for (i = 0u; i < CQ_NUM; i++) {
        if (cq_pool[i].handle != (AMBA_IPC_HANDLE)-1) {
            IpcDebug("%s(%d) handle 0x%p cbfd %d", __func__, __LINE__, cq_pool[i].handle, cq_pool[i].cbfd);
        }
    }
    pthread_mutex_unlock(&cq_mutex);
}

// Debug: dump memory information
#ifdef DUMP_BACKTRACE
static int print_memmap(void)
{
    char out[2048];

    bt_accessor_t acc;
    bt_memmap_t memmap;

    if (bt_init_accessor(&acc, BT_SELF) == -1)
    {
        fprintf(stderr, "%s:%i %s (%i)%s\n", __func__, __LINE__,
                "bt_init_accessor", errno, strerror(errno));
        return EXIT_FAILURE;
    }

    if (bt_load_memmap(&acc, &memmap) == -1)
    {
        fprintf(stderr, "%s:%i %s (%i)%s\n", __func__, __LINE__,
                "bt_load_memmap", errno, strerror(errno));
        return EXIT_FAILURE;
    }

    if (bt_sprn_memmap(&memmap, out, sizeof(out)) == -1)
    {
        fprintf(stderr, "%s:%i %s (%i)%s\n", __func__, __LINE__,
                "bt_sprn_memmap", errno, strerror(errno));
        return EXIT_FAILURE;
    }

    /* Make sure that the string is null-terminated. */
    out[sizeof(out) - 1] = '\0';
    puts(out);

    bt_unload_memmap(&memmap);

    if (bt_release_accessor(&acc) == -1)
    {
        fprintf(stderr, "%s:%i %s (%i)%s\n", __func__, __LINE__,
                "bt_release_accessor", errno, strerror(errno));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
#endif // DUMP_BACKTRACE

#define REMOVECLIENT(void) do { \
    c_bmap[fd_i] = 0; \
    close(fd_i); \
    FD_CLR(fd_i, &readfd); \
    IpcDebug("IPC %s(%d) client%d offline", __func__, __LINE__, fd_i); \
} while(0);

#define FREEBUF(void) do { \
    if(buf_ptr != NULL) { \
        free(buf_ptr); \
        buf_ptr = NULL;    \
    } \
} while(0);

/**
 * tcp listen on specific port
 */
static int tcp_listen(int port)
{
    struct sockaddr_in sad; /* structure to hold server's address */
    int sd; /* socket descriptors */
    int k;
    int sock_opt;
    struct timeval tval;
    const int rebind_sleep = 6;
    const int rebind_cnt = 2;

    memset((char *)&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr("127.0.0.1");
    sad.sin_port = htons((unsigned short)(port));

    /* Create a socket */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        system("io-pkt-v6-hc; if_up lo0");
        sd = socket(AF_INET, SOCK_STREAM, 0);
        if (sd < 0) {
            fprintf(stderr, ANSI_RED "IPC %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
            exit(2);
        }
    }

    /* prevent address already in use */
    sock_opt = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &sock_opt, sizeof(sock_opt));

    /* disable Nagle Algorithm */
    //sock_opt = 1;
    //setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, &sock_opt, sizeof(sock_opt));

    /* enable 60 sec TCP keepalive */
    sock_opt = 1;
    setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, &sock_opt, sizeof(sock_opt));
    memset(&tval, 0, sizeof(tval));
    tval.tv_sec = 60;
    setsockopt(sd, IPPROTO_TCP, TCP_KEEPALIVE, (void *) &tval, sizeof(tval));

    /* Bind a local address to the socket */
    IpcDebug("IPC %s(%d) binding port %d", __func__, __LINE__, PROTOPORT);
    for (k = 1; k <= rebind_cnt; k++) {
        if (bind(sd, (struct sockaddr *)&sad, sizeof(sad)) != -1) {
            break;
        }
        if (rebind_cnt == k) {
            close(sd);
            exit(2);
        }
        fprintf(stderr, ANSI_RED "IPC %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
        IpcDebug("IPC %s(%d) sleep %d and rebind", __func__, __LINE__, rebind_sleep);
        sleep(rebind_sleep);
    }

    /* Specify size of request queue */
    if (listen(sd, MAX_CLIENT_NUM) < 0) {
        fprintf(stderr, ANSI_RED "IPC %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
        close(sd);
        exit(2);
    }

    return sd;
}

/**
 * msg from R52 remote
 */
static void QnxRpmsgCB(struct AmbaIPC_RPDEV_s *rpdev, void *data, UINT32 len, void *priv, UINT32 src)
{
    int32_t cbfd;

    IpcDebug("IPC %s(%d) src: %d, len: %d", __func__, __LINE__, src, len);

    cbfd = cq_get_cbfd(rpdev);
    if (cbfd > 0) {
        if (len > 0) {
            ssize_t sk_err, sk_len;
            amba_rpmsg_pkt_t pkt;
            amba_rpmsg_call_send_t *param;

            IpcDebug("%s(%d) lock sd%ld", __func__, __LINE__, (long)priv);
            rpmsg_lock(&sem_recv);

            param = (amba_rpmsg_call_send_t *)pkt.param;
            param->handle = rpdev;
            param->Length = len;

            /*bus error, rpmsg_core got killed*/
            memcpy_isr(param->Data, data, len);

            pkt.hdr.magic = AMBA_RPMSG_MAGIC;
            pkt.hdr.cmd =  DCMD_RPMSG_RECV;
            pkt.hdr.len = sizeof(param->handle) + sizeof(param->Length) + param->Length;;

            sk_len = sizeof(amba_rpmsg_pkt_hdr_t) + pkt.hdr.len;
            sk_err = send(cbfd, (const void *)&pkt, sk_len, MSG_NOSIGNAL);
            if (sk_err != sk_len) {
                fprintf(stderr, ANSI_RED "IPC %s(%d) write %ld bytes, but return %ld \n" ANSI_RESET, __func__, __LINE__, sk_len, sk_err);
            } else {
                IpcDebug("%s(%d) sent %ld bytes \n", __func__, __LINE__, sk_err);
            }
        } else {
            fprintf(stderr, ANSI_RED "IPC %s(%d) Invalid len: %d\n" ANSI_RESET, __func__, __LINE__, len);
        }
    } else {
        fprintf(stderr, ANSI_RED "IPC %s(%d) Can not found cbfd of %p\n" ANSI_RESET, __func__, __LINE__, rpdev);
    }
}

/**
 * from client: AmbaIPC_Alloc
 * to client: ack
 */
void alloc_acking(int sd, amba_rpmsg_pkt_t *pkt)
{
    ssize_t sk_err;
    const struct AmbaIPC_RPDEV_s *rpdev = NULL;
    amba_rpmsg_call_alloc_t *param;
    amba_rpmsg_call_alloc_rsp_t rsp;

    /* Get handle, saving name */
    param = (amba_rpmsg_call_alloc_t *)pkt->param;
    rpdev = rpdev_alloc(param->name, 0, QnxRpmsgCB, (AMBA_IPC_HANDLE)(long)sd);

    rsp.hdr.magic = AMBA_RPMSG_MAGIC;
    rsp.hdr.cmd =  DCMD_RPMSG_ALLOC;
    rsp.hdr.len = sizeof(amba_rpmsg_call_alloc_rsp_t) - sizeof(amba_rpmsg_pkt_hdr_t);

    if (rpdev == NULL) {
        rsp.handle = (AMBA_IPC_HANDLE)-1;
    } else {
        IpcDebug("IPC %s(%d) AmbaIPC_Alloc(%s) rpdev 0x%p", __func__, __LINE__, param->name, (void *)rpdev);
        rsp.handle = (AMBA_IPC_HANDLE)rpdev;
    }

    sk_err = send(sd, (const void*)&rsp, sizeof(rsp), MSG_NOSIGNAL);
    if (sk_err != sizeof(rsp)) {
        fprintf(stderr, ANSI_RED "IPC %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
    } else {
        cq_enq(rsp.handle, sd);
    }
}

/**
 * from client: AmbaIPC_Register
 * to client: ack
 */
void reg_acking(int sd, amba_rpmsg_pkt_t *pkt)
{
    ssize_t sk_err;
    struct AmbaIPC_RPDEV_s *rpdev = NULL;
    amba_rpmsg_call_reg_t *param;
    amba_rpmsg_call_rsp_t rsp;
    const char *name = "c0_and_c1";
    int32_t ret = -1;

    /* Get handle, saving name */
    param = (amba_rpmsg_call_reg_t *)pkt->param;
    rpdev = param->handle;

    ret = rpdev_register(rpdev, name);

    rsp.hdr.magic = AMBA_RPMSG_MAGIC;
    rsp.hdr.cmd =  DCMD_RPMSG_REG;
    rsp.hdr.len = sizeof(amba_rpmsg_call_rsp_t) - sizeof(amba_rpmsg_pkt_hdr_t);
    rsp.err = ret;

    IpcDebug("IPC %s(%d) AmbaIPC_RegisterChannel(0x%p) return %d", __func__, __LINE__, (void *)rpdev, ret);

    sk_err = send(sd, (const void*)&rsp, sizeof(rsp), MSG_NOSIGNAL);
    if (sk_err != sizeof(rsp)) {
        fprintf(stderr, ANSI_RED "IPC %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
    }
}

/**
 * from client: AmbaIPC_Send
 * to client: ack
 */
void send_acking(int sd, amba_rpmsg_pkt_t *pkt)
{
    ssize_t sk_err;
    struct AmbaIPC_RPDEV_s *rpdev = NULL;
    amba_rpmsg_call_send_t *param;
    amba_rpmsg_call_rsp_t rsp;
    int32_t ret = -1;

    /* Get handle, saving name */
    param = (amba_rpmsg_call_send_t *)pkt->param;
    rpdev = param->handle;

    ret = rpdev_send(rpdev, param->Data, param->Length);

    rsp.hdr.magic = AMBA_RPMSG_MAGIC;
    rsp.hdr.cmd =  DCMD_RPMSG_SEND;
    rsp.hdr.len = sizeof(amba_rpmsg_call_rsp_t) - sizeof(amba_rpmsg_pkt_hdr_t);
    rsp.err = ret;

    IpcDebug("IPC %s(%d) AmbaIPC_Send(0x%p) return %d", __func__, __LINE__, (void *)rpdev, ret);

    sk_err = send(sd, (const void*)&rsp, sizeof(rsp), MSG_NOSIGNAL);
    if (sk_err != sizeof(rsp)) {
        fprintf(stderr, ANSI_RED "IPC %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
    }
}

/**
 * from client: ack
 */
void recv_acked(int sd, amba_rpmsg_pkt_t *pkt)
{
    amba_rpmsg_call_rsp_t *rsp;

    (void)sd;
    rsp = (amba_rpmsg_call_rsp_t *)pkt;
    if (rsp->err != 0) {
        fprintf(stderr, "IPC %s(%d) ret 0x%x\n", __func__, __LINE__, rsp->err);
    }
    IpcDebug("%s(%d) unlock AmbaIPC_Recv", __func__, __LINE__);
    rpmsg_unlock(&sem_recv);
}

/**
 * from client
 */
void parse_rpmsg_cmd(int sd, amba_rpmsg_pkt_t *pkt)
{
    IpcDebug("%s(%d) len %d", __func__, __LINE__, pkt->hdr.len);

    if (pkt->hdr.magic == AMBA_RPMSG_MAGIC) {
        switch (pkt->hdr.cmd) {
            case DCMD_RPMSG_ALLOC:
                alloc_acking(sd, pkt);
                break;

            case DCMD_RPMSG_REG:
                reg_acking(sd, pkt);
                break;

            case DCMD_RPMSG_SEND:
                send_acking(sd, pkt);
                break;

            case DCMD_RPMSG_RECV:
                recv_acked(sd, pkt);
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
 * rpmsg MW service
 */
static void *rpmsg_server(void *arg)
{
    int fd_i = 0;
    fd_set readfd;
    fd_set testfds;
    FILE *FILESTDIN = fdopen(0, "r");
    int c_bmap[FD_SETSIZE]; /* c_bmap[clienid]: 0=offline 1=online 2=processing */
    struct sockaddr_in cad;
    struct timeval tv;
    int sd = 0, sd2 = 0;
    socklen_t alen;
    char *buf_ptr = NULL;
    ssize_t nbytes = 0;
    size_t ignore = 0;
    int i;

    (void)arg;

    sem_init(&sem_recv, 1, 1);
    init_cq();

    sd = tcp_listen(PROTOPORT);
    FD_ZERO(&readfd);
    FD_SET(sd, &readfd);
    memset(c_bmap, 0x0, FD_SETSIZE * sizeof(int));

    if (0 == rpmsg_core_close_stdin) {
        FD_SET(0, &readfd);
    }

    for (;;) {
        tv.tv_sec = 120;
        tv.tv_usec = 0;
        testfds = readfd;

        if (select(FD_SETSIZE, &testfds, NULL, NULL, &tv) <= 0) {
            /*clients should sent ping in 60 sec interval*/
            IpcDebug("select %lu sec timeout\n", tv.tv_sec);
            continue;
        }

        /*
         * check server keyboard then Broadcasts message
         */
        if ((0 == rpmsg_core_close_stdin) && (FD_ISSET(0, &testfds))) {
            FREEBUF();
            if ((nbytes = getline(&buf_ptr, &ignore, FILESTDIN)) <= 0) {
                break;
            }
            strtok(buf_ptr, "\n");
            IpcDebug("IPC %s(%d) [%s]", __func__, __LINE__, buf_ptr);
            if (0 == strncmp(buf_ptr, "dump", strlen("dump"))) {
                cq_dump();
            }
            else {
                for(i= 0; i < FD_SETSIZE; i++) {
                    if(c_bmap[i] == 1 && i!= sd){
                        write(i, buf_ptr, strlen(buf_ptr)+1);
                    }
                }
            }
        }

        for(fd_i = 3; fd_i < FD_SETSIZE; fd_i++) {
            if(!FD_ISSET(fd_i, &testfds)) {
                continue;
            }

            /*
             * add a new connection to the descriptor set
             */
            if(fd_i == sd) {
                alen = sizeof(cad);
                sd2 = accept(sd, (struct sockaddr *)&cad, &alen);
                /* clientid = accepted socket descriptor */
                if (sd2 < (FD_SETSIZE - 1)) {
                    IpcDebug("IPC %s(%d) client%d online", __func__, __LINE__, sd2);
                    FD_SET(sd2, &readfd);
                    c_bmap[sd2] = 1;
                }
                else {
                    IpcDebug("IPC %s(%d) drop client%d", __func__, __LINE__, sd2);
                    close(sd2);
                }
            }


            /*
             * serve the client
             */
            else {
                const ssize_t hdr_len = sizeof(amba_rpmsg_pkt_hdr_t);
                amba_rpmsg_pkt_t *pkt;
                uint32_t payload_len;
                ssize_t readed;

                ioctl(fd_i, FIONREAD, &nbytes);
                if(nbytes == 0) {
                    if(c_bmap[fd_i]){
                        REMOVECLIENT();
                    }
                    continue;
                }
                FREEBUF();
                buf_ptr = (char *)malloc(BUF_SIZE);
                pkt = (amba_rpmsg_pkt_t *)buf_ptr;

                if (nbytes > hdr_len) {
                    readed = read(fd_i, buf_ptr, hdr_len);

                    buf_ptr[readed]='\0';
                    IpcDebug("\n%d|%s|", fd_i, buf_ptr);

                    if (pkt->hdr.magic == AMBA_RPMSG_MAGIC) {
                        payload_len = pkt->hdr.len;
                        readed = read(fd_i, buf_ptr + hdr_len, payload_len);
                        if (readed != payload_len) {
                            fprintf(stderr, ANSI_RED "IPC %s(%d) read %d bytes, but return %ld \n" ANSI_RESET, __func__, __LINE__, payload_len, readed);
                        } else {
                            /*rpmsg control msg from remote*/
                            parse_rpmsg_cmd(fd_i, (amba_rpmsg_pkt_t *)pkt);
                        }
                    }
                } else {
                    readed = read(fd_i, buf_ptr, nbytes);
                    buf_ptr[readed]='\0';
                    IpcDebug("%d|%ld|%s|", fd_i, readed, buf_ptr);
                    if (strncmp(buf_ptr, "quit", strlen("quit")) == 0) {
                        REMOVECLIENT();
                    }
                    else if (0 == strncmp(buf_ptr, "ping", strlen("ping"))) {
                        send(fd_i, "pong", strlen("pong") + 1, MSG_NOSIGNAL);
                    }
                }
            }
        }
    }

    for(fd_i = 1; fd_i < FD_SETSIZE; fd_i++) {
        if(c_bmap[fd_i] > 0 && fd_i != sd){
            REMOVECLIENT();
        }
    }
    close(sd);

    return NULL;
}

int32_t main(int32_t argc, char *argv[])
{
    int opt;
    void AmbaLink_LinkCtrlCfg(void);
    void AmbaLink_MemPool_Init(void);
    void AmbaIPC_RpmsgInit(void);

    AmbaLink_LinkCtrlCfg();
    AmbaLink_MemPool_Init();
    AmbaIPC_RpmsgInit();


    while ((opt = getopt(argc, argv, "hfv")) != -1) {
        switch (opt) {
        case 'h':
            fprintf (stderr, "    -f    runs in foreground to allow stdin \n");
            fprintf (stderr, "    -v    enable logging \n");
            opt = -1;
            break;
        case 'f':
            rpmsg_core_close_stdin = 0;
            break;
        case 'v':
            rpmsg_core_verbose = 1;
            break;

        default: /* '?' */
            fprintf (stderr, "    -f    runs in foreground to allow stdin \n");
            fprintf (stderr, "    -v    enable logging \n");
            exit(0);
        }
    }

    if (rpmsg_core_close_stdin == 1) {
        int err = procmgr_daemon(EXIT_SUCCESS, (PROCMGR_DAEMON_NODEVNULL | PROCMGR_DAEMON_NOCLOSE));
        if (err < 0) {
            fprintf(stderr, ANSI_RED "IPC %s(%d) err %d: %s\n" ANSI_RESET, __func__, __LINE__, errno, strerror(errno));
            return EXIT_FAILURE;
        }
    }

    rpmsg_server(NULL);

    return 0;
}
/******************************************************************************/
#endif  // !defined(BUILTIN_G2_ECHO)
