/**
*  @file AmbaDSP_Int.h
*
 * Copyright (c) 2020 Ambarella International LP
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
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
*  @details Definitions and APIs for SSP interrupts Module.
*
*/

#ifndef AMBA_DSP_INT_H
#define AMBA_DSP_INT_H

#include "AmbaTypes.h"

#ifdef CONFIG_KAL_THREADX_SMP
#define INT_TARGET_CORE     (0x1U) /* core 0 */
#else
#define INT_TARGET_CORE     (0x1U) /* core 0 */
#endif

/*---------------------------------------------------------------------------*\
 * Definitions of DSP Event Flags
\*---------------------------------------------------------------------------*/
/** ms to wait DSP interrupt */
#define AMBA_LL_INT_TIME_OUT                (3000U)

/**
 * Init LL_InterruptInit\n
 * 1. Create related flags
 * 2. Hooked DSP ISR
 * @return 0 - OK, other - ErrorCode
 */
UINT32 LL_InterruptInit(void);

/**
 * Wait Vin event flag gernered by ISR
 * It is called by AmbaDSP_MainWaitVinInterrupt
 * @param [in] Flag Vin event flags to wait
 * @param [out] ActualFlag Actual flag
 * @param [in] Timeout Wait option (timer-ticks)
 * @return 0 - OK, other - ErrorCode
 */
UINT32 LL_WaitVinInterrupt(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut);

/**
 * Wait Vout event flag gernered by ISR
 * It is called by AmbaDSP_MainWaitVoutInterrupt
 * @param [in] Flag Vout event flags to wait
 * @param [out] ActualFlag Actual flag
 * @param [in] Timeout Wait option (timer-ticks)
 * @return 0 - OK, other - ErrorCode
 */
UINT32 LL_WaitVoutInterrupt(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut);

#endif  //AMBA_DSP_INT_H
