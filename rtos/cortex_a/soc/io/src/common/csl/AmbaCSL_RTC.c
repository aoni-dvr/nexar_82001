/*
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
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"

#include "AmbaCSL_PLL.h"
#include "AmbaCSL_RTC.h"

/**
 *  AmbaCSL_RtcSetSystemTime - Set system time (defined as the number of seconds since an epoch of the beginning of 1970 GMT.
 *                             Ticking of the second is freerun so that the new system time value might only appear 1~1000 ms.
 *                             NOTE: the beginning of 1970 GMT (Thursday)
 *                                   = 1970-01-01 00:00:00 UTC
 *                                   = 1970-01-01 00:00:10 TAI
 *  @param[in] Seconds Number of seconds that have elapsed since an epoch of the beginning of 1970 GMT
 */
void AmbaCSL_RtcSetSystemTime(UINT32 Seconds)
{
    AmbaCSL_RtcSetCurTime(Seconds);
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22)
    AmbaCSL_RtcSetCurTimeLatch(1);
#endif
    AmbaCSL_RtcTogglePCRST();
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22)
    AmbaCSL_RtcSetCurTimeLatch(0);
#endif
}

/**
 *  AmbaCSL_RtcTogglePCRST - Toggle PCRST
 */
void AmbaCSL_RtcTogglePCRST(void)
{
#ifndef CONFIG_QNX
    UINT32 DelayCycle = AmbaCSL_PllGetNumCpuCycleMs();
#else
#if defined(CONFIG_SOC_CV2)
    UINT32 DelayCycle = 0xffffU; // TODO: replace it.
#else
    UINT32 DelayCycle = 100; //AmbaCSL_PllGetNumCpuCycleMs(); // TODO: replace it.
#endif
#endif
    UINT32 CurTime, Count = 0U;

    AmbaCSL_RtcSetResetReg(0U);     /* PCRST = 0 */
    AmbaDelayCycles(DelayCycle << 1U);  /* Delay 2ms */

#if defined(CONFIG_SOC_CV2)
    CurTime = pAmbaMISC_Reg->CurTimeWrite;
#else
    CurTime = pAmbaScratchpadS_Reg->CurTimeWrite;
#endif
    AmbaCSL_RtcSetResetReg(1U);     /* PCRST = 1: Rising Edge to latch Alarm and Current Time Register */

    while ((AmbaCSL_RtcGetCurTime() != CurTime) && (Count < 2000000U)) {
        Count ++;
    }
    AmbaCSL_RtcSetResetReg(0U);     /* PCRST = 0 */
}
