/**
 *  @file AmbaB8_PWM.c
 *
 *  @copyright Copyright (c) 2017 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details B8 PWM APIs
 *
 */
#include "AmbaB8.h"

#include "AmbaB8_Communicate.h"
#include "AmbaB8_PLL.h"
#include "AmbaB8_PWM.h"
#include "AmbaB8_GPIO.h"

#include "AmbaB8CSL_PLL.h"
#include "AmbaB8CSL_PWM.h"

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PwmInit
 *
 *  @Description:: PWM device driver initialization
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 : B8 ERROR CODE
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PwmInit(UINT32 ChipID, B8_PWM_CHANNEL_e PwmChanNo)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 DataBuf32;

    /* Set IOMUX */
    if (PwmChanNo == B8_PWM_CHANNEL0) {
        (void) AmbaB8_GpioSetAltFunc(ChipID, B8_GPIO_PIN13_PWM0);

    } else if (PwmChanNo == B8_PWM_CHANNEL1) {
        (void) AmbaB8_GpioSetAltFunc(ChipID, B8_GPIO_PIN14_PWM1);

    } else {
        RetVal = B8_ERR_ARG;
    }

    /* Set clk_ref as reference clock for PWM */
    DataBuf32 = 1;
    RetVal |= AmbaB8_RegWrite(ChipID, & pAmbaB8_PllReg->PwmClkSel, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);

    /* Let gclk_pwm to be clk_ref/ 1 */
    DataBuf32 = 1;
    RetVal |= AmbaB8_RegWrite(ChipID, & pAmbaB8_PllReg->CgPwm, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PwmStart
 *
 *  @Description:: Start generating signal pulse
 *
 *  @Input      ::
 *      ChipID:      B8 chip id
 *      PwmChanNo:   PWM Channel Number
 *      pPwmConfig:  Pointer to PWM configuration
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 : B8 ERROR CODE
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PwmStart(UINT32 ChipID, B8_PWM_CHANNEL_e PwmChanNo, B8_PWM_CONFIG_s *pPwmConfig)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 PulseDivider = 0;
    UINT32 PwmFrequency;
    B8_PWM_ENABLE_REG_s PwmEnable = {0};
    B8_PWM_DATA_REG_s PwmConfig = {0};

    if ((PwmChanNo >= B8_NUM_PWM_CHANNEL) || (pPwmConfig == NULL)) {
        RetVal = B8_ERR_ARG;

    } else {
        /* Set PWM Frequency */
        PwmFrequency = AmbaB8_PllGetPwmClkRef(ChipID);
        PulseDivider = AmbaB8_GetRoundUpValU32(PwmFrequency, pPwmConfig->SampleFreq);
        PulseDivider = (PulseDivider > 1U) ? (PulseDivider) : 1U;
        //PulseDivider = max(GetRoundUp(PwmFrequency, pPwmConfig->SampleFreq), 1);
        pPwmConfig->SampleFreq = PwmFrequency / PulseDivider;
        PwmEnable.Divider = PulseDivider - 1U;

        /* Config PWM channel */
        PwmConfig.Xoff = pPwmConfig->OffTicks - 1U;
        PwmConfig.Xon = pPwmConfig->OnTicks - 1U;
        if (PwmChanNo == B8_PWM_CHANNEL0) {
            RetVal |=AmbaB8_RegWrite(ChipID, & pAmbaB8_PwmReg->Pwm0Data, 0, B8_DATA_WIDTH_32BIT, 1, &PwmConfig);
        } else if (PwmChanNo == B8_PWM_CHANNEL1) {
            RetVal |=AmbaB8_RegWrite(ChipID, & pAmbaB8_PwmReg->Pwm1Data, 0, B8_DATA_WIDTH_32BIT, 1, &PwmConfig);
        } else {
            /* Shall not happen! */
        }

        /* Enable PWM channel */
        PwmEnable.Enable = 1;

        if (PwmChanNo == B8_PWM_CHANNEL0) {
            RetVal |=AmbaB8_RegWrite(ChipID, & pAmbaB8_PwmReg->Pwm0Enable, 0, B8_DATA_WIDTH_32BIT, 1, &PwmEnable);
        } else if (PwmChanNo == B8_PWM_CHANNEL1) {
            RetVal |=AmbaB8_RegWrite(ChipID, & pAmbaB8_PwmReg->Pwm1Enable, 0, B8_DATA_WIDTH_32BIT, 1, &PwmEnable);
        } else {
            /* Shall not happen! */
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PwmStop
 *
 *  @Description:: Stop generating signal pulse
 *
 *  @Input      ::
 *      ChipID:     B8 chip id
 *      PwmChanNo:  PWM Channel Number
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 : B8 ERROR CODE
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PwmStop(UINT32 ChipID, B8_PWM_CHANNEL_e PwmChanNo)
{
    UINT32 RetVal = B8_ERR_NONE;
    B8_PWM_ENABLE_REG_s PwmEnable = {0};

    if (PwmChanNo >= B8_NUM_PWM_CHANNEL) {
        RetVal = B8_ERR_ARG;
    } else {
        /* Disable PWM channel */
        PwmEnable.Enable = 0;

        if (PwmChanNo == B8_PWM_CHANNEL0) {
            RetVal |= AmbaB8_RegWrite(ChipID, & pAmbaB8_PwmReg->Pwm0Enable, 0, B8_DATA_WIDTH_32BIT, 1, &PwmEnable);
        } else if (PwmChanNo == B8_PWM_CHANNEL1) {
            RetVal |= AmbaB8_RegWrite(ChipID, & pAmbaB8_PwmReg->Pwm1Enable, 0, B8_DATA_WIDTH_32BIT, 1, &PwmEnable);
        } else {
            /* Shall not happen! */
        }
    }

    return RetVal;
}

