/**
 *  @file getpid.c
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
 *  @details _getpid()
 *
 */
/* getpid.c -- get the current process id.
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
#include "sc.h"
#include "k_assert.h"

#ifdef CONFIG_KAL_THREADX_SMP
#include "tx_api_SMP.h"
#else
#include "tx_api_Uniprocessor.h"
#endif

#include "AmbaKAL.h"
#include "AmbaPrint.h"

/* Should always be successful.
 * return pthreadID if have POSIX wrapper
 * otherwise, return TX_THREAD
 */
int _getpid(void)
{
    int pid;

    pid = (int)tx_thread_identify();

    AmbaPrint_PrintStr5("%s:",__func__,NULL,NULL,NULL,NULL);
    AmbaPrint_PrintInt5("caller pid: %d(0x%X)", pid, pid, 0,0,0);

    AmbaPrint_PrintStr5("TODO: %s:",__func__,NULL,NULL,NULL,NULL);
    AmbaPrint_Flush();
    K_ASSERT(0);

    if (pid == 0) {
        AmbaPrint_PrintStr5("%s: Invalid PID 0.", __func__,NULL,NULL,NULL,NULL);
        AmbaKAL_TaskSleep(10);
        return 0;
    }

    return pid;
}

