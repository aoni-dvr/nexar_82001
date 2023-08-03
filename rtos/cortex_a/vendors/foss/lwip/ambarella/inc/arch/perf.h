/**
 *  @file cc.h
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

#ifndef LWIP_AMBA_PERF_H
#define LWIP_AMBA_PERF_H

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(__cplusplus)
}
#endif

#if LWIP_PERF
/*
 * Example code
 */
#include <comsvc/misc/AmbaUtility.h>
#include <comsvc/printf/AmbaPrint.h>
extern void sys_arch_perf_start(const char *f, u32_t l);
extern void sys_arch_perf_stop(const char *f, u32_t l, const char *str);

#define PERF_START      sys_arch_perf_start(__func__, __LINE__)
#define PERF_STOP(x)    sys_arch_perf_stop(__func__, __LINE__, x)
#else /* !LWIP_PERF */
#define PERF_START      /* NO operation. */
#define PERF_STOP(x)    /* NO operation. */
#endif /* LWIP_PERF */

#endif /* LWIP_AMBA_PERF_H */

