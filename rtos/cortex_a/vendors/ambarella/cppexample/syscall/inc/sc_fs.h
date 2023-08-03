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

#ifndef SC_FS_H
#define SC_FS_H

#include <stdint.h>

#include <sc.h>
#ifdef CONFIG_SSP_THREADX_NETX
#include <nx_bsd.h>
#endif

/*
 * FD of netx is from NX_BSD_SOCKFD_START with number NX_BSD_MAX_SOCKETS
 * Default of NX_BSD_SOCKFD_START is 32
 * PrFile use pointer of structure.
 * STDOUT_FILENO (0), STDIN_FILENO (1), STDERR_FILENO (2) are occupied by
 * console at begining.
 */
#ifdef CONFIG_SSP_THREADX_NETX
#define SC_FS_FD_MAX   (NX_BSD_SOCKFD_START)
#else
#define SC_FS_FD_MAX   (32)
#endif


#define SC_FS_FLG_USED (0x80000000)

/* File type. */
typedef enum sc_ftype_s {
    SC_FTYPE_FILE = 0,
    SC_FTYPE_DIR,
    SC_FTYPE_EVENT,
    SC_FTYPE_SOCKET,
    SC_FTYPE_EPOLL,
    SC_FTYPE_TIMER,
    SC_FTYPE_MAX
} sc_ftype_t;

/* File operations. */
typedef struct sc_fops_s {
} sc_fops_t;

typedef struct sc_fs_s {
    uint32_t   flag;            /* File flag/state, cf. SC_FS_FLG_XXX */
    uint32_t   ucnt;            /* Use count. */
    uint32_t   type;            /* File type, cf. sc_ftype_t */
    sc_fops_t *fops;            /* File operations. */
    uint32_t   pos;             /* Current position, offset to 0 */
} sc_fs_t;


#ifdef __cplusplus
extern "C"
{
#endif

extern int sc_fs_allocFd(void);
extern int sc_fs_freeFd(int fd);
extern int sc_fs_setType(int fd, sc_ftype_t type);

#ifdef __cplusplus
}
#endif

#endif /* SC_FS_H */

