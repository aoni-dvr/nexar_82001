/*
 * Copyright 2020, Ambarella International LP
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

#ifndef AMBA_CSL_ADC_H
#define AMBA_CSL_ADC_H

#ifndef AMBA_ADC_DEF_H
#include "AmbaADC_Def.h"
#endif

#ifndef AMBA_REG_RCT_H
#include "AmbaReg_RCT.h"
#endif

#include "AmbaReg_ADC.h"

/*
 * ADC Interrupt Status Structures
 */
typedef struct {
    UINT32  InterruptFlags; /* Interrupt type */
    UINT32  FifoIntStatus;  /* Fifo Interrupt status */
    UINT32  DataIntStatus;  /* Channel Interrupt status */
} AMBA_ADC_INT_STATUS_s;

/*
 * Inline Function Definitions
 */
static inline void AmbaCSL_AdcPowerDownDisable(void)
{
    pAmbaRCT_Reg->AdcPowerCtrl.PowerDown = 0U;
}
static inline void AmbaCSL_AdcPowerDownEnable(void)
{
    pAmbaRCT_Reg->AdcPowerCtrl.PowerDown = 1U;
}

#if defined(CONFIG_SOC_CV2) ||  defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV5) ||defined(CONFIG_SOC_CV52)
static inline void AmbaCSL_AdcT2vPowerDownDisable(void)
{
    pAmbaRCT_Reg->T2vCtrl.PowerDown = 0U;
}
static inline void AmbaCSL_AdcT2vPowerDownEnable(void)
{
    pAmbaRCT_Reg->T2vCtrl.PowerDown = 1U;
}
#endif

/*
 * Defined in AmbaCSL_ADC.c
 */
void AmbaCSL_AdcConfigFifo(UINT32 FifoID, UINT32 FifoSize, UINT32 AdcChanNo);
void AmbaCSL_AdcSetTimeSlotPeriod(UINT32 NumTicks);
void AmbaCSL_AdcConfigTimeSlot(UINT32 NumTimeSlots, const UINT32 *pChanMask);

#endif /* AMBA_CSL_ADC_H */
