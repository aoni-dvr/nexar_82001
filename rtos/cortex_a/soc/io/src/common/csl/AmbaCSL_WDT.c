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

#include "AmbaCSL_WDT.h"
#include "AmbaCSL_RCT.h"

#if defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52) || defined(CONFIG_SOC_H32)
/**
 *  AmbaCSL_WdtGetWdtResetStatus - Get Watchdog timer reset status
 */
UINT32 AmbaCSL_WdtGetWdtResetStatus(void)
{
    UINT32 RetVal;

    if (pAmbaRCT_Reg->WdtResetState == 0U) { /* Low-active */
        RetVal = 1U;
    } else {
        RetVal = 0U;
    }

    return RetVal;
}
#endif

/**
 *  AmbaCSL_WdtClearWdtResetStatus - Clear Watchdog timer reset status
 */
void AmbaCSL_WdtClearWdtResetStatus(void)
{
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52) || defined(CONFIG_SOC_H32)
    pAmbaRCT_Reg->UnlockWdtClear = 1U;      /* Unlock to clear status */
    pAmbaRCT_Reg->WdtResetState = 1U;       /* Clear WDT Reset Status */
    pAmbaRCT_Reg->UnlockWdtClear = 0U;      /* Lock to make status be updated by WDT reset event */
#else
    pAmbaRCT_Reg->WdtStateClearProtect = 0x3fU;
    pAmbaRCT_Reg->WdtResetState = 0x3fU;
    pAmbaRCT_Reg->WdtStateClearProtect = 0U;
#endif
}

/**
 *  AmbaCSL_WdtSetTimeOutAction - Enable WDT timeout action
 *  @param[in] TimeOutAction Action selection when WDT is occupied
 */
void AmbaCSL_WdtSetTimeOutAction(UINT32 TimeOutAction)
{
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52) || defined(CONFIG_SOC_H32)
    if (TimeOutAction == WDT_ACT_SYS_RESET) {
        AmbaCSL_WdtClearWdtResetStatus();
        AmbaCSL_RctClearSoftReset();    /* Clear soft reset before doing WDT system reset, otherwise reset will fail */
        AmbaCSL_WdtSetCtrlReg(0x03U);
    } else if (TimeOutAction == WDT_ACT_IRQ) {
        AmbaCSL_WdtSetCtrlReg(0x05U);
    } else if (TimeOutAction == WDT_ACT_EXT) {
        AmbaCSL_WdtSetCtrlReg(0x09U);
    } else {
        AmbaCSL_WdtSetCtrlReg(0x00U);
    }
#else
    if (TimeOutAction == WDT_ACT_SYS_RESET) {
        AmbaCSL_WdtClearWdtResetStatus();
        AmbaCSL_RctClearSoftReset();    /* Clear soft reset before doing WDT system reset, otherwise reset will fail */
        AmbaCSL_WdtSetCtrlReg(pAmbaWDT_Reg[0], 0x03U);
    } else if (TimeOutAction == WDT_ACT_IRQ) {
        AmbaCSL_WdtSetCtrlReg(pAmbaWDT_Reg[0], 0x05U);
    } else if (TimeOutAction == WDT_ACT_EXT) {
        AmbaCSL_WdtSetCtrlReg(pAmbaWDT_Reg[0], 0x09U);
    } else {
        AmbaCSL_WdtSetCtrlReg(pAmbaWDT_Reg[0], 0x00U);
    }
#endif
}

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
/**
 *  AmbaCSL_WdtSetTimeOutAction - Enable WDT timeout action
 *  @param[in] ChId WDT channel id
 *  @param[in] TimeOutAction Action selection when WDT is occupied
 */
void AmbaCSL_WdtSetAction(UINT32 ChId, UINT32 TimeOutAction)
{
    if (TimeOutAction == WDT_ACT_SYS_RESET) {
        AmbaCSL_WdtClearWdtResetStatus();
        AmbaCSL_RctClearSoftReset();    /* Clear soft reset before doing WDT system reset, otherwise reset will fail */
        AmbaCSL_WdtSetCtrlReg(pAmbaWDT_Reg[ChId], 0x03U);
    } else if (TimeOutAction == WDT_ACT_IRQ) {
        AmbaCSL_WdtSetCtrlReg(pAmbaWDT_Reg[ChId], 0x05U);
    } else if (TimeOutAction == WDT_ACT_EXT) {
        AmbaCSL_WdtSetCtrlReg(pAmbaWDT_Reg[ChId], 0x09U);
    } else {
        AmbaCSL_WdtSetCtrlReg(pAmbaWDT_Reg[ChId], 0x00U);
    }
}
#endif
