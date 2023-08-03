/**
 *  @file sys_arch.h
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Architecture environment, some compiler specific, some environment specific.
 *           Refer to doc/sys_arch.txt for detail.
 *
 */

#ifndef LWIP_AMBA_SYS_ARCH_H
#define LWIP_AMBA_SYS_ARCH_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <AmbaKAL.h>

/** Define LWIP_COMPAT_MUTEX if the port has no mutexes and binary semaphores
      should be used instead */
#define LWIP_COMPAT_MUTEX (0)

typedef AMBA_KAL_TASK_t  *      sys_thread_t;
typedef AMBA_KAL_MUTEX_t        sys_mutex_t;
typedef AMBA_KAL_SEMAPHORE_t    sys_sem_t;
typedef AMBA_KAL_MSG_QUEUE_t    sys_mbox_t;


extern void sys_arch_msleep(u32_t ms);
#define sys_msleep(ms)              sys_arch_msleep(ms)

#if defined(__cplusplus)
}
#endif

#endif /* LWIP_AMBA_SYS_ARCH_H */

