/**
 *  @file write.c
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
 *  @details _write()
 *
 */
/* write.c -- write bytes to an output device.
 *
 * Copyright (c) 1995 Cygnus Support
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */
#include <unistd.h>

#include "sc.h"
#include "k_assert.h"

#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaUART.h"

static void _write_init(void) __attribute__((constructor(102)));
static AMBA_KAL_MUTEX_t mutex_writelock;

void _write_init(void)
{
    int err;

    if (mutex_writelock.tx_mutex_id <= 0) {
        err = AmbaKAL_MutexCreate(&mutex_writelock,"sc_write_mutex");
        K_ASSERT(err == OK);
        AmbaKAL_MutexGive(&mutex_writelock);
    }
}

int _write(int file, char *ptr, int len)
{
    int rc;
    int s, i;
    UINT8 *p = (UINT8 *)ptr;

#define WBUF_SIZE (128)
    char wbuf[WBUF_SIZE + 1]; // +1 for insert '\r' at the end

//    AmbaKAL_MutexTake(&mutex_writelock, TX_WAIT_FOREVER);
    rc = AmbaKAL_MutexTake(&mutex_writelock, 100);
    if (rc != 0) {
        AmbaPrint_PrintStr5("%s: ", __func__,NULL,NULL,NULL,NULL);
        AmbaPrint_PrintInt5("rc: %d", rc,0,0,0,0);
        errno = EIO;
        return 0;
    }

    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO)) {
        AmbaKAL_MutexGive(&mutex_writelock);
#if 1
        return len;
#else
        errno = ENOSYS;
        return -1;
#endif
    }
    s = 0;
    while (1) {
        for (i = 0; i < WBUF_SIZE;) {
            if (*p == '\n') {
                wbuf[i++] = '\r';
            }
            wbuf[i++] = *p;
            s++;
            if (s >= len) {
                break;
            }
            p++;
        }
        AmbaUART_Write(AMBA_UART_APB_CHANNEL0, 0U, i, (const UINT8 *)wbuf, &rc, 10);
        K_ASSERT(rc == i);
        if (s >= len) {
            break;
        }
    }
    if (s == len) {
        errno = 0;
    } else {
        errno = EIO;
    }

    AmbaKAL_MutexGive(&mutex_writelock);

    if (s > 0) {
        return s;
    } else {
        return -1;
    }
}

