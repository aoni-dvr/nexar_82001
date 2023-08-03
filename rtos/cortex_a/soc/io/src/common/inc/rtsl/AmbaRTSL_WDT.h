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

#ifndef AMBA_RTSL_WDT_H
#define AMBA_RTSL_WDT_H

#ifndef AMBA_WDT_DEF_H
#include "AmbaWDT_Def.h"
#endif

/*
 * Defined in AmbaRTSL_WDT.c
 */
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
UINT32 AmbaRTSL_WdtHookTimeOutHandler(AMBA_WDT_ISR_f ExpireFunc, UINT32 ExpireFuncArg);
UINT32 AmbaRTSL_WdtHookHandler(UINT32 ChId, AMBA_WDT_ISR_f ExpireFunc, UINT32 ExpireFuncArg);
UINT32 AmbaRTSL_WdtStart(UINT32 CountDown, UINT32 ResetIrqPulseWidth);
UINT32 AmbaRTSL_WdtStartByChId(UINT32 ChId, UINT32 CountDown, UINT32 ResetIrqPulseWidth);
UINT32 AmbaRTSL_WdtFeed(void);
UINT32 AmbaRTSL_WdtFeedByChId(UINT32 ChId);
UINT32 AmbaRTSL_WdtStop(void);
UINT32 AmbaRTSL_WdtStopByChId(UINT32 ChId);
UINT32 AmbaRTSL_WdtSetPattern(void);
UINT32 AmbaRTSL_WdtGetStatusById(UINT32 ChId);

UINT32 AmbaRTSL_WdtGetTimeOutAction(UINT32 ChId);
UINT32 AmbaRTSL_WdtGetTimeOutStatus(void);
UINT32 AmbaRTSL_WdtGetResetStatus(void);
void AmbaRTSL_WdtClearTimeOutStatus(void);
void AmbaRTSL_WdtCoreFreqChg(UINT32 ChId);
#else
UINT32 AmbaRTSL_WdtHookTimeOutHandler(AMBA_WDT_ISR_f ExpireFunc, UINT32 ExpireFuncArg);
UINT32 AmbaRTSL_WdtStart(UINT32 CountDown, UINT32 ResetIrqPulseWidth);
UINT32 AmbaRTSL_WdtFeed(void);
UINT32 AmbaRTSL_WdtStop(void);

UINT32 AmbaRTSL_WdtGetTimeOutAction(void);
UINT32 AmbaRTSL_WdtGetTimeOutStatus(void);
UINT32 AmbaRTSL_WdtGetResetStatus(void);
void AmbaRTSL_WdtClearTimeOutStatus(void);
void AmbaRTSL_WdtCoreFreqChg(void);
#endif

void AmbaRTSL_Wdt_Init(void);

#endif  /* AMBA_RTSL_WDT_H */

