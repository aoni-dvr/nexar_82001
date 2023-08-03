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
#include "AmbaCSL_ADC.h"

/**
 *  AmbaCSL_AdcConfigFifo - Configure adc fifo size
 *  @param[in] FifoID FIFO ID
 *  @param[in] FifoConfig FIFO config
 */
void AmbaCSL_AdcConfigFifo(UINT32 FifoID, UINT32 FifoSize, UINT32 AdcChanNo)
{
    AMBA_ADC_FIFO_CTRL_REG_s AdcFifoCtrl = {0};

    AdcFifoCtrl.FifoDepth = (UINT16)FifoSize;
    AdcFifoCtrl.ChannelId = (UINT8)AdcChanNo;
    AdcFifoCtrl.FifoTheshold = (UINT8)(FifoSize >> 1U);
    AdcFifoCtrl.FifoUnderflowInt = 0U;
    AdcFifoCtrl.FifoOverflowInt = 0U;

    pAmbaADC_Reg->FifoCtrl[FifoID] = AdcFifoCtrl;
}

/**
 *  AmbaCSL_AdcSetTimeSlotPeriod - Configure adc sampling time slot period
 *  @param[in] NumTicks Number of sample clock ticks
 */
void AmbaCSL_AdcSetTimeSlotPeriod(UINT32 NumTicks)
{
    pAmbaADC_Reg->TimeSlotPeriod = NumTicks - 1U;
}

/**
 *  AmbaCSL_AdcConfigTimeSlot - Configure adc channel sampling per time slot
 *  @param[in] NumTimeSlots Number of time slots
 *  @param[in] pChanMask Sampling channels of each time slot
 */
void AmbaCSL_AdcConfigTimeSlot(UINT32 NumTimeSlots, const UINT32 *pChanMask)
{
    UINT32 i;
    const UINT32 *pMask;

    if (pChanMask == NULL) {
        pAmbaADC_Reg->TimeSlotNum = 0U;
    } else {
        AmbaMisra_TypeCast(&pMask, &pChanMask);

        pAmbaADC_Reg->TimeSlotNum = NumTimeSlots - 1U;
        for (i = 0U; i < NumTimeSlots; i ++) {
            pAmbaADC_Reg->TimeSlotCtrl[i] = *pMask;
            pMask ++;
        }
    }
}
