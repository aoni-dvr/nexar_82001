/**
 *  @file sc_fs.h
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
 *  @details System-Call, FileSystem implementation.
 *
 */

#include <unistd.h>
#include <string.h>

#include "sc_fs.h"


static sc_fs_t sc_fs_fd_tbl[SC_FS_FD_MAX];

static void sc_fs_init(void) __attribute__((constructor(101)));

static void sc_fs_init(void)
{
    static int inited = 0;

    if (inited) {
        return;
    }

    memset(sc_fs_fd_tbl, 0, sizeof(sc_fs_fd_tbl));
    sc_fs_fd_tbl[STDOUT_FILENO].flag |= SC_FS_FLG_USED;
    sc_fs_fd_tbl[STDIN_FILENO].flag  |= SC_FS_FLG_USED;
    sc_fs_fd_tbl[STDERR_FILENO].flag |= SC_FS_FLG_USED;
    inited                            = 1;

    return;
}


/* Get/Allocate File Descriptor, return -1 as failed. */
int sc_fs_allocFd(void)
{
    int i = STDERR_FILENO + 1;

    for (; i < SC_FS_FD_MAX; i++) {
        if ((sc_fs_fd_tbl[i].flag & SC_FS_FLG_USED) == 0) {
            sc_fs_fd_tbl[i].flag |= SC_FS_FLG_USED;
            return i;
        }
    }
    return -1;
}


/* Set/Free File Descriptor, return -1 as failed. */
int sc_fs_freeFd(int fd)
{
    if ((fd <= STDERR_FILENO) ||
        (fd >= SC_FS_FD_MAX)) {
        return -1;
    }

    memset(&(sc_fs_fd_tbl[fd]), 0, sizeof(sc_fs_t));

    return 0;
}

/* Set type of FD */
int sc_fs_setType(int fd, sc_ftype_t type)
{
    if ((fd <= STDERR_FILENO) ||
        (fd >= SC_FS_FD_MAX) ||
        (type >= SC_FTYPE_MAX)) {
        return -1;
    }

    sc_fs_fd_tbl[fd].type = type;

    return 0;
}

