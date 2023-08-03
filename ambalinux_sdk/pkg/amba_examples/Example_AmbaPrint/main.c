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
#include <stdlib.h>
#include "AmbaTypes.h"

#include <mqueue.h>
#include "ambarella_mprint.h"
#include <termios.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>


#define EOK    0

static struct print_region *p_shm;
static int ser_fd = -1;
static UINT8 *p_ring_buf;

//local function for print process
static int p_print_shm_init(int ring_buf_size, int timeout)
{
    int rval = EOK;
    pthread_mutexattr_t attr;
    int p_rfd;

    /* Create shared memory object and set its size */
    p_rfd = shm_open(SHM_PRINT_CTRL, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRWXO);
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

    p_shm->mq_timeout.tv_nsec = timeout;
    p_shm->ring_buf_size = ring_buf_size;
    p_shm->log_buf_size = LOG_BUFFER_LENGTH;
    p_shm->flag_print_stop = 0;

    /* create share mutex */
    if (pthread_mutexattr_init(&attr)) {
        perror("pthread_mutexattr_init");
    }
    if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED)) {
        perror("pthread_mutexattr_setpshared");
    }
    if (pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST)) {
        perror("pthread_mutexattr_setrobust");
    }
    if (pthread_mutex_init(&p_shm->mutex_print, &attr)) {
        perror("pthread_mutex_init: print_mutex");
    }
    if (pthread_mutex_init(&p_shm->mutex_write, &attr)) {
        perror("pthread_mutex_init: write_mutex");
    }

    return rval;
}


static int p_print_ser_init(int uart, char **path)
{
    int rval = 0;

    if (uart == 0xFF) {
        ser_fd = open(*path, O_RDWR);
    } else if (uart == 3) {
        ser_fd = open("/dev/ttyS3", O_RDWR);
    } else if (uart == 2) {
        ser_fd = open("/dev/ttyS2", O_RDWR);
    } else if (uart == 1) {
        ser_fd = open("/dev/ttyS1", O_RDWR);
    } else {
        ser_fd = open("/dev/ttyS0", O_RDWR);
    }

      if (ser_fd == -1) {
          printf("could not open UART %d\n", uart);
          rval = -1;
      } else {
          struct termios tio;
          if (tcgetattr(ser_fd, &tio) == 0) {
              // set baudrate to 115200
              cfsetispeed(&tio, B115200);
              cfsetospeed(&tio, B115200);
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

static mqd_t p_print_msq_init(int rq_num)
{
    amba_print_data_t msg;
    mqd_t msg_queue;
    struct mq_attr attrs;

    /* Create messsage queue for print request */
     memset(&attrs, 0, sizeof attrs);
     attrs.mq_maxmsg = rq_num;
     attrs.mq_msgsize = sizeof(msg);

     msg_queue = mq_open( MQ_PRINT, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, &attrs );
     if (msg_queue == -1) {
        perror ("mq_open()");
     }

    return msg_queue;
}

int main(int argc, char *argv[])
{
    amba_print_data_t msg;
    mqd_t msg_queue;
    ssize_t received_len;
    unsigned int prio;
    int uart, rq_num, ring_buf_size, timeout; // default
    unsigned int path_length;
    char **path;

    if(argc != 5) {
        // set to default
        uart = PRINT_PORT;
        rq_num = PRINT_REQUEST_QUEUE_NUM;
        ring_buf_size = PRINT_BUFFER_LENGTH;
        timeout = PRINT_REQUEST_TIMEOUT; // in nanoseconds
    } else {
        path_length = strlen(argv[1]);

        if (path_length > 1) { // by path
            path = &argv[1];
            uart = 0xFF;
            printf("\nuart port path = %s\n", *path);
        } else {  // by port number
            uart = atoi(argv[1]);
            path = NULL;

            if (uart > PRINT_PORT_MAX) {
                printf("\n[warning] AmbaPrint(): uart > %u, set to default\n", PRINT_PORT_MAX);
                uart = PRINT_PORT;
            }
            printf("\nuart port number = %d\n", uart);
        }

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
        timeout = atoi(argv[4]);
        //printf("\ntimeout = %d\n", timeout);
    }

    p_print_shm_init(ring_buf_size, timeout);
    p_print_ser_init(uart, path);
    msg_queue = p_print_msq_init(rq_num);

    printf("MPrint driver init done\n");

    /* background process for Print and Module Print */
    while (1) {

        received_len = mq_receive (msg_queue, (char*)&msg, sizeof(msg), &prio);
        if (received_len == -1) {
           perror ("mq_receive()");
        }

        //printf(" receive \n");

        //Print_Flush(msg.first_buf_idx, msg.last_buf_idx);

        if (p_ring_buf == NULL) {
            printf("share memory is null");
        } else {

            UINT32 rIdx, wIdx, TxSize;
            rIdx = msg.rIdx;
            wIdx = msg.wIdx;
            //printf("\n begin = 0x%x, end = 0x%x \n ", begin, end);


            while (rIdx != wIdx) {
                if (wIdx > rIdx) {
                    /* No wrap around */
                    TxSize = wIdx - rIdx;
                } else {
                    /* Wrap around circular buffer */
                    TxSize = p_shm->ring_buf_size - rIdx;
                }
                write(ser_fd, &p_ring_buf[rIdx], TxSize);

                if (TxSize > 0U) {
                    rIdx += TxSize;
                    if (rIdx >= p_shm->ring_buf_size) {
                        rIdx = 0;
                    }
                }
            }
        }
    }

    return 0;
}
