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

#ifndef AMBA_RTSL_TMR_H
#define AMBA_RTSL_TMR_H

#ifndef AMBA_TMR_DEF_H
#include "AmbaTMR_Def.h"
#endif

typedef void (*AMBA_TMR_CALLBACK_f)(UINT32 TimerID);

/*
 * Defined in AmbaRTSL_TMR.c
 */
void AmbaRTSL_TmrInit(void);
void AmbaRTSL_TmrCoreFreqChg(void);

UINT32 AmbaRTSL_TmrHookTimeOutHandler(UINT32 TimerID, AMBA_TMR_ISR_f ExpireFunc, UINT32 ExpireFuncArg);
UINT32 AmbaRTSL_TmrConfig(UINT32 TimerID, UINT32 TimerFreq, UINT32 NumPeriodicTick);
UINT32 AmbaRTSL_TmrStart(UINT32 TimerID, UINT32 NumRemainTick);
UINT32 AmbaRTSL_TmrStop(UINT32 TimerID);
UINT32 AmbaRTSL_TmrShowTickCount(UINT32 TimerID, UINT32 * pTimeLeft);
UINT32 AmbaRTSL_TmrGetInfo(UINT32 TimerID, AMBA_TMR_INFO_s * pTimerInfo);
UINT32 AmbaRTSL_TmrGetOsTimerId(UINT32 *pTimerID);

#endif /* AMBA_RTSL_TMR_H */
