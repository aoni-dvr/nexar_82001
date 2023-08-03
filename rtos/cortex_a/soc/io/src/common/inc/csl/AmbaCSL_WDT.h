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

#ifndef AMBA_CSL_WDT_H
#define AMBA_CSL_WDT_H

#ifndef AMBA_WDT_DEF_H
#include "AmbaWDT_Def.h"
#endif

#ifndef AMBA_REG_RCT_H
#include "AmbaReg_RCT.h"
#endif

#include "AmbaReg_WDT.h"

/*
 * Inline Function Definitions
 */
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
static inline UINT32 AmbaCSL_WdtGetCtrlReg(UINT32 ChId)
{
    return pAmbaWDT_Reg[ChId]->Ctrl;
}
static inline void AmbaCSL_WdtSetCtrlReg(AMBA_WDT_REG_s * pWdtReg, UINT32 Val)
{
    pWdtReg->Ctrl = Val;
}

static inline UINT32 AmbaCSL_WdtGetCounter(const AMBA_WDT_REG_s * pWdtReg)
{
    return pWdtReg->CurCounter;
}
static inline UINT32 AmbaCSL_WdtGetTimeOutStatus(const AMBA_WDT_REG_s * pWdtReg)
{
    return pWdtReg->TimeOutStatus;
}
static inline void AmbaCSL_WdtClearTimeOutStatus(AMBA_WDT_REG_s * pWdtReg)
{
    pWdtReg->Clear = 1U;
}

static inline UINT32 AmbaCSL_WdtGetReload(const AMBA_WDT_REG_s * pWdtReg)
{
    return pWdtReg->Reload;
}
static inline void AmbaCSL_WdtSetReload(AMBA_WDT_REG_s * pWdtReg, UINT32 TimeOut)
{
    pWdtReg->Reload = TimeOut;
}
static inline void AmbaCSL_WdtSetRestart(AMBA_WDT_REG_s * pWdtReg)
{
    pWdtReg->Restart = AMBA_WDT_MAGIC_CODE;
}

static inline UINT32 AmbaCSL_WdtGetResetWidth(const AMBA_WDT_REG_s * pWdtReg)
{
    return pWdtReg->ResetWidth.Width;
}
static inline void AmbaCSL_WdtSetResetWidth(AMBA_WDT_REG_s * pWdtReg, UINT32 Val)
{
    pWdtReg->ResetWidth.Width = Val;
}
#else
static inline UINT32 AmbaCSL_WdtGetCtrlReg(void)
{
    return pAmbaWDT_Reg->Ctrl;
}
static inline void AmbaCSL_WdtSetCtrlReg(UINT32 Val)
{
    pAmbaWDT_Reg->Ctrl = Val;
}

static inline UINT32 AmbaCSL_WdtGetCounter(void)
{
    return pAmbaWDT_Reg->CurCounter;
}
static inline UINT32 AmbaCSL_WdtGetTimeOutStatus(void)
{
    return pAmbaWDT_Reg->TimeOutStatus;
}
static inline void AmbaCSL_WdtClearTimeOutStatus(void)
{
    pAmbaWDT_Reg->Clear = 1U;
}

static inline UINT32 AmbaCSL_WdtGetReload(void)
{
    return pAmbaWDT_Reg->Reload;
}
static inline void AmbaCSL_WdtSetReload(UINT32 TimeOut)
{
    pAmbaWDT_Reg->Reload = TimeOut;
}
static inline void AmbaCSL_WdtSetRestart(void)
{
    pAmbaWDT_Reg->Restart = AMBA_WDT_MAGIC_CODE;
}

static inline UINT32 AmbaCSL_WdtGetResetWidth(void)
{
    return pAmbaWDT_Reg->ResetWidth.Width;
}
static inline void AmbaCSL_WdtSetResetWidth(UINT32 Val)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    pAmbaWDT_Reg->ResetWidth.Width = Val;
#else
    pAmbaWDT_Reg->ResetWidth.Width = (UINT8)Val;
#endif
}
#endif

/*
 * Defined in AmbaCSL_WDT.c
 */
void AmbaCSL_WdtClearWdtResetStatus(void);
void AmbaCSL_WdtSetTimeOutAction(UINT32 TimeOutAction);
#ifndef CONFIG_SOC_CV2
UINT32 AmbaCSL_WdtGetWdtResetStatus(void);
#endif

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
void AmbaCSL_WdtSetAction(UINT32 ChId, UINT32 TimeOutAction);
#endif

#endif /* AMBA_CSL_WDT_H */
