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
#include "AmbaMisraFix.h"

#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_IRIF.h"
#include "AmbaCSL_IRIF.h"

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define IRIF_INT_ID AMBA_INT_SPI_ID81_IR_INTERFACE
#else
#define IRIF_INT_ID AMBA_INT_SPI_ID52_IR_INTERFACE
#endif

/* Call back function when completed transactions */
static void (*AmbaIrIsrCallBack)(void) = NULL;

/**
 *  IR_IntHandler - ISR for Infrared interface
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
#pragma GCC push_options
#pragma GCC target("general-regs-only")
static void IR_IntHandler(UINT32 IntID, UINT32 IsrArg)
{
    AmbaMisra_TouchUnused(&IntID);
    AmbaMisra_TouchUnused(&IsrArg);

    if (pAmbaIR_Reg->Ctrl.FifoOverflow != 0U) {
        /* drop all the FIFO data */
        while (AmbaCSL_IrGetDataCount() != 0U) {
            (void)AmbaCSL_IrGetData();
        }

        /* clear overflow bit */
        AmbaCSL_IrClearFifoOverflow();
    } else {
        if (AmbaIrIsrCallBack != NULL) {
            AmbaIrIsrCallBack();
        }
    }
}
#pragma GCC pop_options

/**
 *  AmbaRTSL_IrInit - Initialize Infrared Remote
 *  @return error code
 */
UINT32 AmbaRTSL_IrInit(void)
{
    AMBA_INT_CONFIG_s IntConfig;
    UINT32 IntID = IRIF_INT_ID;

    IntConfig.TriggerType = INT_TRIG_RISING_EDGE;
    IntConfig.IrqType = INT_TYPE_IRQ;
    IntConfig.CpuTargets = 0x1U;

    (void)AmbaRTSL_GicIntConfig(IntID, &IntConfig, IR_IntHandler, 0U);

    return IRIF_ERR_NONE;
}

/**
 *  AmbaRTSL_IrIntHookHandler - Hook IR interface Interrupt Service Routine (ISR)
 *  @param[in] IntFunc Interrupt Service Routine
 *  @return error code
 */
UINT32 AmbaRTSL_IrIntHookHandler(AMBA_IRIF_ISR_f IntFunc)
{
    AmbaIrIsrCallBack = IntFunc;

    return IRIF_ERR_NONE;
}

/**
 *  AmbaRTSL_IrStart - Start IR interface
 *  @param[in] SampleFreq Sampling frequency (Hz).
 *  @return error code
 */
UINT32 AmbaRTSL_IrStart(UINT32 SampleFreq)
{
    UINT32 RetVal = IRIF_ERR_NONE;

    if (AmbaRTSL_PllSetIrClk(SampleFreq) != PLL_ERR_NONE) {
        RetVal = IRIF_ERR_ARG;
    } else {
        /* Reset and clear interrupt status */
        AmbaCSL_IrReset();

        /* Initialize and enable */
        pAmbaIR_Reg->Ctrl.IntCond = 24U;
        pAmbaIR_Reg->Ctrl.IntEnable = 1U;
        pAmbaIR_Reg->Ctrl.Enable = 1U;

        (void)AmbaRTSL_GicIntEnable(IRIF_INT_ID);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_IrStop - Stop IR interface
 *  @return error code
 */
UINT32 AmbaRTSL_IrStop(void)
{
    (void)AmbaRTSL_GicIntDisable(IRIF_INT_ID);

    /* Reset and clear interrupt status */
    AmbaCSL_IrReset();

    return IRIF_ERR_NONE;
}

/**
 *  AmbaRTSL_IrRead - Read IR Data
 *  @param[in] NumData Number of data to be read
 *  @param[out] pDataBuf pointer to the data buffer
 *  @param[out] pActualNumData number of items successfully read before timeout
 *  @return error code
 */
UINT32 AmbaRTSL_IrRead(UINT32 NumData, UINT16 *pDataBuf, UINT32 *pActualNumData)
{
    static UINT16 AmbaIrData[AMBA_IR_MAX_FIFO_ENTRY];

    UINT32 DataCount = AmbaCSL_IrGetDataCount();
    UINT32 i, RetVal = IRIF_ERR_NONE;

    if ((pDataBuf == NULL) || (pActualNumData == NULL)) {
        RetVal = IRIF_ERR_ARG;
    } else {
        if (DataCount > NumData) {
            DataCount = NumData;
        }

        for (i = 0U; i < DataCount; i ++) {
            AmbaIrData[i] = (UINT16) AmbaCSL_IrGetData();
            pDataBuf[i] = AmbaIrData[i];
        }

        *pActualNumData = DataCount;

        /* clear interrupt bit */
        AmbaCSL_IrClearInterrupt();
    }

    return RetVal;
}
