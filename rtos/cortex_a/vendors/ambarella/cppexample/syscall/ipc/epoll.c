/**
 *  @file epoll.c
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details epoll functions.
 *
 */

#include <stdio.h>

#include "sc_fs.h"

//#include <ssp/AmbaSSP.h>
//#include <AmbaPrint.h>
//#include <ssp/soc/intrinsics.h>

#ifdef CONFIG_SSP_THREADX_NETX
#include <nx_bsd.h>
#endif


typedef union epoll_data {
    void *   ptr;
    int      fd;
    uint32_t u32;
    uint64_t u64;
} epoll_data_t;

struct epoll_event {
    uint32_t     events;                       /* Epoll events */
    epoll_data_t data;                         /* User data variable */
};

int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
#if 1
    printf("epfd: %d, op: %d, fd: %d \n", epfd, op, fd);
    (void)event;
    return 0;
#else
    K_ASSERT(0);
    return -1;
#endif
}

int epoll_wait(int epfd, struct epoll_event *events, int maxevents,
               int timeout)
{
#if 1
    printf("epfd: %d, events: 0x%08X, maxevents: %d, timeout: %d\n",
           epfd, (unsigned int)events, maxevents, timeout);
    return 0;
#else
    K_ASSERT(0);
    return -1;
#endif
}

int epoll_create(int size)
{
    int fd = sc_fs_allocFd();

    if (fd < 0) {
        errno = ENFILE;
        return -1;
    }

    sc_fs_setType(fd, SC_FTYPE_EPOLL);
    /* TODO: creates an epoll instance. */

    (void)size;

    return fd;
}

int epoll_create1(int flags)
{
    int fd = sc_fs_allocFd();

    if (fd < 0) {
        errno = ENFILE;
        return -1;
    }

    sc_fs_setType(fd, SC_FTYPE_EPOLL);
    /* TODO: creates an epoll instance. */

    (void)flags;

    return fd;
}

