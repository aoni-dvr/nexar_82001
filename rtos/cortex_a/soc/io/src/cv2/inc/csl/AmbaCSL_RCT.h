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

#ifndef AMBA_CSL_RCT_H
#define AMBA_CSL_RCT_H

#ifndef AMBA_REG_RCT_H
#include "AmbaReg_RCT.h"
#endif

/*
 * Inline Function Definitions
 */
static inline void AmbaCSL_RctSetSoftReset(void)
{
    pAmbaRCT_Reg->SoftReset0 = 0xfU;
}
static inline void AmbaCSL_RctClearSoftReset(void)
{
    pAmbaRCT_Reg->SoftReset0 = 0xeU;
}
static inline UINT32 AmbaCSL_RctGetSoftReset(void)
{
    return pAmbaRCT_Reg->SoftReset0;
}
static inline UINT32 AmbaCSL_RctGetWdtState(void)
{
    return pAmbaRCT_Reg->WdtResetState;
}

static inline void AmbaCSL_RctTimer0Enable(void)
{
    pAmbaRCT_Reg->RctTimer0Ctrl = AMBA_RCT_TIMER_ENABLE;
}
static inline void AmbaCSL_RctTimer0Reset(void)
{
    pAmbaRCT_Reg->RctTimer0Ctrl = AMBA_RCT_TIMER_RESET;
}
static inline void AmbaCSL_RctTimer0Freeze(void)
{
    pAmbaRCT_Reg->RctTimer0Ctrl = AMBA_RCT_TIMER_FREEZE;
}

static inline void AmbaCSL_RctTimer1Enable(void)
{
    pAmbaRCT_Reg->RctTimer1Ctrl = AMBA_RCT_TIMER_ENABLE;
}
static inline void AmbaCSL_RctTimer1Reset(void)
{
    pAmbaRCT_Reg->RctTimer1Ctrl = AMBA_RCT_TIMER_RESET;
}
static inline void AmbaCSL_RctTimer1Freeze(void)
{
    pAmbaRCT_Reg->RctTimer1Ctrl = AMBA_RCT_TIMER_FREEZE;
}

/*
 * Defined in AmbaCSL_RCT.c
 */
void AmbaCSL_RctChipSoftReset(void);
UINT32 AmbaCSL_RctTimer0GetCounter(void);
UINT32 AmbaCSL_RctTimer1GetCounter(void);

#endif /* AMBA_CSL_RCT_H */
