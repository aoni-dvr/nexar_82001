/**
 *  @file network.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Network APIs
 *
 */

#include <stdlib.h>
#include <string.h>
#include "os_api.h"
#include "amalgam.h"
#include "dram_sync.h"

#define DTHREAD_MAX_NUM 32  /* dthread pool + scheduler + extra */
#define MAX_CONN_TRY    16

#define MSG_ARM_ECHO    0
#define MSG_ARM_WRITE   1
#define MSG_ARM_READ    2
#define MSG_ARM_SYNC    3
#ifdef CHIP_CV6
#define MSG_ARM_INTERRUPT 4
#endif /* ?CHIP_CV6 */

#define MSG_ORC_ECHO    0
#define MSG_ORC_READ_R  1
#define MSG_ORC_WRITE   2
#define MSG_ORC_INT     3

#define RX_STACK_LEN    (4*1024)
#define RX_PRIORITY     160

#define TX_COPY_LIMIT   64

#define tx_lock()       thread_mutex_lock(&mutex_tx)
#define tx_unlock()     thread_mutex_unlock(&mutex_tx)

#define rr_lock()       thread_mutex_lock(&sync->lock)
#define rr_unlock()     thread_mutex_unlock(&sync->lock)

#define tx_buf(socket_fd, buf, size) {                                         \
    int pos, bytes;                                                        \
    for (pos = 0; pos < size; pos += bytes) {                              \
        bytes = send(socket_fd, buf + pos, size - pos, 0);             \
        assert(bytes > 0);                                             \
    }}
#define tx_flush(socket_fd) {                                                  \
    int flag = 1;                                                          \
    setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, 4);      \
    }

struct rr_sync {
    pmutex_t        lock;
    sem_t           done;
    tid_t           tid;
    int             seq;

#define RR_SYNC_NO_WAITING 0 /* read task is not waiting */
#define RR_SYNC_IS_WAITING 1 /* read task is waiting for finish */
#define RR_SYNC_ALL_FINISH 2 /* all read replies have been received */
    int                status;
};

static struct rr_sync   reply_sync[DTHREAD_MAX_NUM];
static pmutex_t         mutex_tx;
static int32_t          socket_fd;
static thread_t         rx_thread;

extern void sod_orc_isr(int id);

static void* p2v(uint32_t pa)
{
    void *ptr;

#ifdef  LINUX_BUILD
#ifndef DAG_KERNEL_SUPPORT
    extern void* schdr_p2v(uint32_t pa);
    ptr = schdr_p2v(pa);
    if (ptr != NULL)
        return ptr;
#endif
#endif
    ptr = ambacv_p2v(pa);
    return ptr;
}

static void rx_buf(int fd, char *buf, int32_t size)
{
    int pos, bytes, retval;
    fd_set rfds;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    for (pos = bytes = 0; pos < size; pos += bytes) {
        retval = select(fd+1, &rfds, NULL, NULL, NULL);
        if (retval == -1)
            continue;

        bytes = recv(fd, buf + pos, size - pos, 0);
        if (bytes <= 0) {
            printf("Connection closed!\n");
            exit(0);
        }
    }
}

static struct rr_sync* get_rr_sync(tid_t tid)
{
    int i;
    struct rr_sync *sync;

    for (i = 0, sync = reply_sync; i < DTHREAD_MAX_NUM; i++, sync++) {
        if (sync->tid == tid)
            return sync;
    }

    for (i = 0, sync = reply_sync; i < DTHREAD_MAX_NUM; i++, sync++) {
        if (sync->tid == (tid_t)0) {
            sync->tid = tid;
            return sync;
        }
    }

    assert(0);
}

static void wakeup_rr_waiting_task(int seq)
{
    struct rr_sync *sync = reply_sync;
    int i;

    for (i = 0; i < DTHREAD_MAX_NUM; i++, sync++) {
        rr_lock();
        if (sync->seq == seq) {
            if (sync->status == RR_SYNC_IS_WAITING) {
                // wake up the waiting task
                sem_post(&sync->done);
            } else {
                // indicate that all reads are done
                sync->status = RR_SYNC_ALL_FINISH;
            }
        }
        rr_unlock();
    }
}

int amalgam_wait_for_read_done(void)
{
    struct rr_sync  *sync = get_rr_sync(thread_self());

    rr_lock();
    if (sync->status == RR_SYNC_ALL_FINISH) {
        rr_unlock();
        return 0;
    }
    sync->status = RR_SYNC_IS_WAITING;
    rr_unlock();

    sem_wait(&sync->done);
    sync->status = RR_SYNC_NO_WAITING;
    return 0;
}

void amalgam_send_echo(char *str)
{
    int buf[2], len = strlen(str);

    assert(str != 0);

    tx_lock();
    buf[0] = len + sizeof(int);
    buf[1] = MSG_ARM_ECHO;
    tx_buf(socket_fd, (char*)buf, sizeof(buf));
    tx_buf(socket_fd, str, len);
    tx_flush(socket_fd);
    tx_unlock();
}

int amalgam_send_read(uint64_t addr, int32_t size)
{
    static int32_t _seq;
#ifdef CHIP_CV6
    int buf[6];
    struct rr_sync  *sync = get_rr_sync(thread_self());

    tx_lock();
    _seq++;
    buf[0] = 20;
    buf[1] = MSG_ARM_READ;
    buf[2] = ((addr      ) & 0xFFFFFFFFU);
    buf[3] = ((addr >> 32) & 0xFFFFFFFFU);
    buf[4] = size;
    buf[5] = _seq;
#else /* !CHIP_CV6 */
    int buf[5];
    struct rr_sync  *sync = get_rr_sync(thread_self());

    tx_lock();
    _seq++;
    buf[0] = 16;
    buf[1] = MSG_ARM_READ;
    buf[2] = addr;
    buf[3] = size;
    buf[4] = _seq;
#endif /* ?CHIP_CV6 */
    rr_lock();
    sync->seq = _seq;
    sync->status = RR_SYNC_NO_WAITING;
    rr_unlock();
    tx_buf(socket_fd, (char*)buf, sizeof(buf));
    tx_flush(socket_fd);
    tx_unlock();

    return sync->seq;
}

void amalgam_send_write(uint64_t addr, char *data, int32_t size)
{
#ifdef CHIP_CV6
    int buf[5+(TX_COPY_LIMIT>>2)];

    assert(data != 0);

    tx_lock();
    buf[0] = 16 + size;
    buf[1] = MSG_ARM_WRITE;
    buf[2] = ((addr      ) & 0xFFFFFFFFU);
    buf[3] = ((addr >> 32) & 0xFFFFFFFFU);
    buf[4] = size;
    if (size <= TX_COPY_LIMIT) {
        memcpy(&buf[5], data, size);
        tx_buf(socket_fd, (char*)buf, 20+size);
    } else {
        tx_buf(socket_fd, (char*)buf, 20);
        tx_buf(socket_fd, data, size);
    }
#else
    int buf[4+(TX_COPY_LIMIT>>2)];

    assert(data != 0);

    tx_lock();
    buf[0] = 12 + size;
    buf[1] = MSG_ARM_WRITE;
    buf[2] = addr;
    buf[3] = size;
    if (size <= TX_COPY_LIMIT) {
        memcpy(&buf[4], data, size);
        tx_buf(socket_fd, (char*)buf, 16+size);
    } else {
        tx_buf(socket_fd, (char*)buf, 16);
        tx_buf(socket_fd, data, size);
    }
#endif /*? CHIP_CV6 */
    tx_flush(socket_fd);
    tx_unlock();
}

#ifdef CHIP_CV6
void amalgam_send_sync(int counter)
{
    int buf[4];

    tx_lock();
    buf[0] = 12;
    buf[1] = MSG_ARM_INTERRUPT;
    buf[2] = 0;
    buf[3] = 0x20;
    tx_buf(socket_fd, (char*)buf, sizeof(buf));
    tx_unlock();
}

void amalgam_send_idspmsg_sync(int counter)
{
    int buf[4];

    tx_lock();
    buf[0] = 12;
    buf[1] = MSG_ARM_INTERRUPT;
    buf[2] = 3;
    buf[3] = 0x22;
    tx_buf(socket_fd, (char*)buf, sizeof(buf));
    tx_unlock();
}

void amalgam_send_vinmsg_sync(int counter)
{
    int buf[4];

    tx_lock();
    buf[0] = 12;
    buf[1] = MSG_ARM_INTERRUPT;
    buf[2] = 2;
    buf[3] = 0x23;
    tx_buf(socket_fd, (char*)buf, sizeof(buf));
    tx_unlock();
}

#else /* !CHIP_CV6 */
void amalgam_send_sync(int counter)
{
    int buf[3];

    tx_lock();
    buf[0] = 8;
    buf[1] = MSG_ARM_SYNC;
    buf[2] = counter;
    tx_buf(socket_fd, (char*)buf, sizeof(buf));
    tx_unlock();
}

void amalgam_send_idspmsg_sync(int counter)
{
    amalgam_send_sync(counter);
}

void amalgam_send_vinmsg_sync(int counter)
{
    amalgam_send_sync(counter);
}

#endif /* ?CHIP_CV6 */

static void handle_echo(int len)
{
    char buf[256];

    assert(sizeof(buf) > len);
    rx_buf(socket_fd, buf, len);
    buf[len] = 0;
    printf("[ECHO]: %s\n", buf);
}

static void handle_read_reply(int len)
{
#ifdef CHIP_CV6
    int buf[4];
    uint64_t phy_daddr;
#else /* !CHIP_CV6 */
    int buf[3];
#endif /* ?CHIP_CV6 */
    char *ptr;

    rx_buf(socket_fd, (char*)buf, sizeof(buf));
#ifdef CHIP_CV6
    phy_daddr   = ((uint64_t)buf[1]) << 32U;
    phy_daddr  |= buf[0];
    ptr = ambacv_p2v(phy_daddr);
    if (ptr != NULL && buf[2] != 0)
        rx_buf(socket_fd, ptr, buf[2]);
    wakeup_rr_waiting_task(buf[3]);
#else /* !CHIP_CV6 */
    ptr = p2v(buf[0]);
    if (ptr != NULL && buf[1] != 0)
        rx_buf(socket_fd, ptr, buf[1]);
    wakeup_rr_waiting_task(buf[2]);
#endif /* ?CHIP_CV6 */
}

static void handle_write(int len)
{
#ifdef CHIP_CV6
    int buf[3];
    char *ptr;

    rx_buf(socket_fd, (char*)buf, sizeof(buf));
    ptr = p2v(buf[0]);
    assert(ptr != 0);
    rx_buf(socket_fd, ptr, buf[2]);
#else /* !CHIP_CV6 */
    int buf[2];
    char *ptr;

    rx_buf(socket_fd, (char*)buf, sizeof(buf));
    ptr = p2v(buf[0]);
    assert(ptr != 0);
    rx_buf(socket_fd, ptr, buf[1]);
#endif /* ?CHIP_CV6 */
}

static void handle_interrupt(int len)
{
    int id;

    rx_buf(socket_fd, (char*)&id, sizeof(id));
    sod_orc_isr(id);
}

void* amalgam_rx_entry(void* arg)
{
    int len, cmd;

    for (;;) {
        rx_buf(socket_fd, (char*)&len, sizeof(len));
        rx_buf(socket_fd, (char*)&cmd, sizeof(cmd));

        len -= 4;
        switch (cmd) {
        case MSG_ORC_ECHO:
            handle_echo(len);
            break;
        case MSG_ORC_READ_R:
            handle_read_reply(len);
            break;
        case MSG_ORC_WRITE:
            handle_write(len);
            break;
        case MSG_ORC_INT:
            handle_interrupt(len);
            break;
        default:
            printf("unhandled cmd=%d, len=%d\n", cmd, len);
        }
    }
    return NULL;
}

void amalgam_start(int fd)
{
    int loop;

    socket_fd = fd;

    /* create tx lock */
    thread_mutex_init(&mutex_tx);

    /* init struct for read reply synchronization */
    for (loop = 0; loop < DTHREAD_MAX_NUM; loop++) {
        sem_init(&(reply_sync[loop].done), 0, 0);
        thread_mutex_init(&(reply_sync[loop].lock));
    }

    thread_create(
        &rx_thread,
        amalgam_rx_entry,
        0,
        RX_PRIORITY,
        0x1,
        RX_STACK_LEN,
        NULL,
        "DAG amalgam rx"
    );
}
