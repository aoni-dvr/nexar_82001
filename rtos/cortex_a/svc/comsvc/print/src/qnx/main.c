/**
 *  @file mian.c
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include "AmbaTypes.h"
#include <mqueue.h>
#include <sys/dispatch.h>
#include <ambarella_mprint.h>
#include <AmbaPrint.h>
#include <termios.h>

static struct print_region *p_shm;
static int ser_fd = -1;
static UINT8 *p_ring_buf;
static int next_read = 0;

//local function for print process

static int p_print_shm_init(int ring_buf_size, int burst,  int level, int timeout)
{
    int rval = EOK;
    pthread_mutexattr_t attr;
    int p_rfd;


    /* Create shared memory object and set its size */
    p_rfd = shm_open(SHM_PRINT_CTRL, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (p_rfd == -1) {
    /* Handle error */;
        printf("error");
        rval = -1;
    }

    if (ftruncate(p_rfd, sizeof(struct print_region)) == -1) {
        /* Handle error */;
        printf("error");
        rval = -1;
    }

    /* Map shared memory object */
    p_shm = mmap(NULL, sizeof(struct print_region),
               PROT_READ | PROT_WRITE, MAP_SHARED, p_rfd, 0);
    if (p_shm == MAP_FAILED) {
        /* Handle error */;
        printf("error");
        rval = -1;
    }

    // For ring buffer
    /* Create shared memory object and set its size */
    p_rfd = shm_open(SHM_PRINT_BUF, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (p_rfd == -1) {
    /* Handle error */;
        printf("error");
        rval = -1;
    }

    if (ftruncate(p_rfd, ring_buf_size) == -1) {
        /* Handle error */;
        printf("error");
        rval = -1;
    }

    /* Map shared memory object */
    p_ring_buf = mmap(NULL, ring_buf_size,
               PROT_READ | PROT_WRITE, MAP_SHARED, p_rfd, 0);
    if (p_ring_buf == MAP_FAILED) {
        /* Handle error */;
        printf("error");
        rval = -1;
    }

    memset(p_ring_buf, 0, ring_buf_size);
    memset(p_shm, 0, sizeof(struct print_region));

    p_shm->ring_buf_size = ring_buf_size;
    p_shm->log_buf_size = LOG_BUFFER_LENGTH;
    p_shm->flag_print_stop = PRINT_COND_NO;
    //p_shm->mq_timeout.tv_nsec = timeout;
    p_shm->burst_enable = burst;
    p_shm->ring_buf_flush_timeout = timeout;
    p_shm->ring_buf_flush_level = level;

    /* create share mutex */
    if (pthread_mutexattr_init(&attr)) {
        perror("pthread_mutexattr_init");
    }
    if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED)) {
        perror("pthread_mutexattr_setpshared");
    }
    if (pthread_mutex_init(&p_shm->mutex_print, &attr)) {
        perror("pthread_mutex_init: print_mutex");
    }
    if (pthread_mutex_init(&p_shm->mutex_write, &attr)) {
        perror("pthread_mutex_init: write_mutex");
    }

    return rval;
}

static mqd_t p_print_msq_init(int rq_num)
{
    amba_print_data_t msg;
    mqd_t msg_queue;
    struct mq_attr attrs;

    /* Create messsage queue for print request */
    memset(&attrs, 0, sizeof attrs);
    //attrs.mq_flags = O_NONBLOCK;
    attrs.mq_maxmsg = rq_num;
    attrs.mq_msgsize = sizeof(msg);
    msg_queue = mq_open(MQ_PRINT, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, &attrs );
    if (msg_queue == -1) {
       perror ("mq_open()");
    } else {
        printf ("Successfully opened AmbaPrintQueue:\n");
    }

    return msg_queue;
}

static int p_print_ser_init(int uart)
{
    int rval = 0;

    if (uart == 1) {
        ser_fd = open("/dev/ser2", O_RDWR);
    } else {
        ser_fd = open("/dev/ser1", O_RDWR);
    }

    if (ser_fd == -1) {
        printf("could not open UART %d.\n", uart);
        rval = -1;
    } else {
        struct termios tio;
        if (tcgetattr(ser_fd, &tio) == 0) {
            // we need to use non-canonical mode and no echo to fit
            // ThreadX Shell implementation.
            tio.c_lflag &= ~(ECHO | ECHOE | ICANON);
            tcsetattr(ser_fd, TCSANOW, &tio);
        } else {
            printf("can't get termio info.\n");
        }
    }

    return rval;
}

#define PRINT_SIZE 512

static void p_print_handler(UINT32 rIdx, UINT32 wIdx)
{
    UINT32 TxSize;

    //printf("\n begin = 0x%x, end = 0x%x \n ", rIdx, wIdx);

    if (rIdx != next_read) {
        //printf("\n message queue overflow? \n ");
    }

    while (rIdx != wIdx) {
        int i;
        UINT32 print_size = 0;

        if (wIdx > rIdx) {
            /* No wrap around */
            TxSize = wIdx - rIdx;
        } else {
            /* Wrap around circular buffer */
            TxSize = (p_shm->ring_buf_size) - rIdx;
        }

        // To avoid data corrupt. FIFO over flow?
        if (TxSize > PRINT_SIZE) {
            for (i = 0; i <= (TxSize - PRINT_SIZE); i+=PRINT_SIZE) {
                write(ser_fd, &p_ring_buf[rIdx+i], PRINT_SIZE);
                print_size+=PRINT_SIZE;
            }
            write(ser_fd, &p_ring_buf[rIdx+print_size], TxSize-print_size);
        } else {
            write(ser_fd, &p_ring_buf[rIdx], TxSize);
        }

        //printf("\n begin = 0x%x, end = 0x%x, TxSize = 0x%x\n ", rIdx, wIdx, TxSize);

        if (TxSize > 0U) {
            rIdx += TxSize;
            if (rIdx == (p_shm->ring_buf_size)) {
                rIdx = 0;
            }
        }
    }
    p_shm->ring_r_idx = wIdx;
    next_read = wIdx;
}

int main(int argc, char *argv[])
{
    amba_print_data_t msg;
    mqd_t msg_queue;
    ssize_t received_len;
    unsigned int prio;
    int uart, rq_num, ring_buf_size, burst, level, timeout; // default

    if(argc != 7) {
        // set to default
        uart = PRINT_PORT;
        ring_buf_size = PRINT_BUFFER_LENGTH;
        rq_num = PRINT_REQUEST_QUEUE_NUM;
        burst = PRINT_BURST_DISABLE;
        timeout = PRINT_FLUSH_TIMEOUT;
        level = PRINT_FLUSH_LEVEL;
    } else {
        uart = atoi(argv[1]);
        if (uart >= PRINT_PORT_MAX) {
            printf("\n[warning] AmbaPrint(): uart > 1, set to default\n");
            uart = PRINT_PORT;
        }
        //printf("\nuart = %d\n", uart);

        rq_num = atoi(argv[2]);
        if(rq_num == 0) {
            printf("\n[warning] AmbaPrint(): rq_num = 0, set to default\n");
            rq_num = PRINT_REQUEST_QUEUE_NUM;
        }
        //printf("\nrq_num = %d\n", rq_num);

        ring_buf_size = atoi(argv[3]);
        if (ring_buf_size == 0) {
            printf("\n[warning] AmbaPrint(): ring_buf_size = 0, set to default\n");
            ring_buf_size = PRINT_BUFFER_LENGTH;
        }
        //printf("\nring_buf_size = %d\n", ring_buf_size);

        burst = atoi(argv[4]);
        //printf("\nburst = %d\n", burst);

        level = atoi(argv[5]);
        //printf("\nlevel = %d\n", level);

        timeout = atoi(argv[6]);
        //printf("\ntimeout = %d\n", timeout);
    }

    p_print_shm_init(ring_buf_size, burst, level, timeout);
    p_print_ser_init(uart);
    msg_queue = p_print_msq_init(rq_num);

    printf("\nMPrint driver init done\n");

    /* background process for Print and Module Print */
    while (1) {

        UINT32 rIdx, wIdx;

        if (p_shm->burst_enable == 1U) {
            struct   timespec tm;
            clock_gettime(CLOCK_MONOTONIC, &tm);

            //tm.tv_sec += 1;  // Set for 1 seconds
            tm.tv_nsec += timeout*1000000; // ms to ns
            tm.tv_sec += (tm.tv_nsec)/1000000000; // ns to s
            tm.tv_nsec %= 1000000000; // tv_nsec was less than 1000 million.
            received_len = mq_timedreceive_monotonic( msg_queue, (char*)&msg, sizeof(msg), &prio, &tm );
        } else {
            received_len = mq_receive( msg_queue, (char*)&msg, sizeof(msg), &prio);
        }

        if( (p_shm->burst_enable == 1U) && (received_len == -1) ) {
            // Timeout? Dump messages stored in ring buffer
            //printf("[Timeout][rIdx = 0x%x, wIdx = 0x%x]\n", p_shm->ring_r_idx, p_shm->ring_w_idx);
            p_shm->msg_index = 0;
            p_shm->msg.rIdx = 0;
            p_shm->msg.wIdx = 0;
            p_shm->ring_buf_accumulate = 0;

            rIdx = p_shm->ring_r_idx;
            wIdx = p_shm->ring_w_idx;

        } else if (received_len != -1) {
            // Normal case
            //printf(" receive \n");
            rIdx = msg.rIdx;
            wIdx = msg.wIdx;
        } else {
            perror ("mq_receive()");
        }

        if (p_ring_buf == NULL) {
            printf("share memory is null");
        } else {
           p_print_handler(rIdx, wIdx);
        }
        continue; // for timeout case
    }

    return EXIT_SUCCESS;
}
