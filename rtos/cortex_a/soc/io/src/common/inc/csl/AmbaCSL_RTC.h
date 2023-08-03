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

#ifndef AMBA_CSL_RTC_H
#define AMBA_CSL_RTC_H

#if defined(CONFIG_SOC_CV2)
#ifndef AMBA_REG_MISC_H
#include "AmbaReg_MISC.h"
#endif
#else
#ifndef AMBA_REG_SCRATCHPAD_S_H
#include "AmbaReg_ScratchpadS.h"
#endif
#endif

#define AMBA_RTC_TICKS_PER_SECOND   (1U)

/*
 * Inline Function Definitions
 */
#if defined(CONFIG_SOC_CV2)
static inline UINT32 AmbaCSL_RtcGetCurTime(void)
{
    return pAmbaMISC_Reg->CurTimeRead;
}

static inline void AmbaCSL_RtcSetResetReg(UINT32 d)
{
    pAmbaMISC_Reg->RtcReset.Reset = (UINT8)d;
}
static inline void AmbaCSL_RtcSetCurTime(UINT32 d)
{
    pAmbaMISC_Reg->CurTimeWrite = d;
}
static inline void AmbaCSL_RtcSetAlarmTime(UINT32 d)
{
    pAmbaMISC_Reg->AlarmWrite = d;
}
static inline void AmbaCSL_RtcSetCurTimeLatch(UINT32 d)
{
    pAmbaMISC_Reg->CurTimerLatch = d;
}

static inline UINT32 AmbaCSL_RtcGetAlarmWakeupState(void)
{
    return pAmbaMISC_Reg->RtcStatus.AlarmWakeUp;
}
static inline UINT32 AmbaCSL_RtcGetPwcWakeupState(void)
{
    return pAmbaMISC_Reg->RtcStatus.PwcWakeUp;
}
#else
static inline UINT32 AmbaCSL_RtcGetCurTime(void)
{
    return pAmbaScratchpadS_Reg->CurTimeRead;
}

static inline void AmbaCSL_RtcSetResetReg(UINT32 d)
{
    pAmbaScratchpadS_Reg->RtcReset.Reset = (UINT8)d;
}
static inline void AmbaCSL_RtcSetCurTime(UINT32 d)
{
    pAmbaScratchpadS_Reg->CurTimeWrite = d;
}
#if defined(CONFIG_SOC_CV22)
static inline void AmbaCSL_RtcSetAlarmTime(UINT32 d)
{
    pAmbaScratchpadS_Reg->AlarmWrite = d;
}
static inline void AmbaCSL_RtcSetCurTimeLatch(UINT32 d)
{
    pAmbaScratchpadS_Reg->CurTimerLatch = d;
}

static inline UINT32 AmbaCSL_RtcGetAlarmWakeupState(void)
{
    return pAmbaScratchpadS_Reg->RtcStatus.AlarmWakeUp;
}
static inline UINT32 AmbaCSL_RtcGetPwcWakeupState(void)
{
    return pAmbaScratchpadS_Reg->RtcStatus.PwcWakeUp;
}
#endif
#endif

/*
 * Defined in AmbaCSL_RTC.c
 */
void AmbaCSL_RtcSetSystemTime(UINT32 Seconds);
void AmbaCSL_RtcTogglePCRST(void);

#endif /* AMBA_CSL_RTC_H */
