/**
 *  @file bsp.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Implementation of Board Support Package
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaSYS.h"
#include "AmbaDef.h"

#include "AmbaGPIO.h"
#include "AmbaUART.h"
#include "AmbaSD.h"
#include "AmbaVIN.h"
#include "AmbaRTSL_PLL.h"

#include "AmbaPrint.h"
#include "AmbaShell.h"
#include "AmbaShell_Commands.h"
#include "bsp.h"
#include "AmbaMisraFix.h"

void AmbaUserConsole_Write(UINT32 StringSize, const char *StringBuf, UINT32 TimeOut)
{
    UINT32 SentSize;
    UINT32 TxSize = StringSize;
    UINT32 BufIndex = 0U;
    const UINT8 *pTxBuf;

    AmbaMisra_TypeCast32(&pTxBuf, &StringBuf);
    while (AmbaUART_Write(AMBA_UART_APB_CHANNEL0, 0U, TxSize, &pTxBuf[BufIndex], &SentSize, TimeOut) == UART_ERR_NONE) {
        TxSize -= SentSize;
        BufIndex += SentSize;
        if (TxSize == 0U) {
            break;
        }
    }
}

UINT32 AmbaUserShell_Read(UINT32 StringSize, char *StringBuf, UINT32 TimeOut)
{
    UINT32 RxSize = 0U;
    UINT8 *pRxBuf;

    (void)StringBuf;
    AmbaMisra_TypeCast32(&pRxBuf, &StringBuf);
    (void)AmbaUART_Read(AMBA_UART_APB_CHANNEL0, 0U, StringSize, pRxBuf, &RxSize, TimeOut);

    return RxSize;
}


UINT32 AmbaUserShell_Write(UINT32 StringSize, const char *StringBuf, UINT32 TimeOut)
{
    UINT32 SentSize = 0U;
    const UINT8 *pTxBuf;

    AmbaMisra_TypeCast32(&pTxBuf, &StringBuf);
    (void)AmbaUART_Write(AMBA_UART_APB_CHANNEL0, 0U, StringSize, pTxBuf, &SentSize, TimeOut);

    return SentSize;
}

extern void AmbaUserGPIO_FpdCtrl(UINT32 FpdFlag);
void AmbaUserGPIO_FpdCtrl(UINT32 FpdFlag)
{
    if (FpdFlag == 0U) {
        (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_137, AMBA_GPIO_LEVEL_LOW);
    } else {
        (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_137, AMBA_GPIO_LEVEL_HIGH);
    }
}

extern void AmbaUserSD_PowerCtrl(UINT32 SdChanNo, UINT32 VddPower);
void AmbaUserSD_PowerCtrl(UINT32 SdChanNo, UINT32 VddPower)
{
    UINT32 GpioLevel;
    UINT32 GpioPinSdHighSpeed = (SdChanNo == AMBA_SD_CHANNEL0) ? GPIO_PIN_156_SD0_HS_SEL : GPIO_PIN_157_SD1_HS_SEL;
    UINT32 GpioPinSdPower = (SdChanNo == AMBA_SD_CHANNEL0) ? GPIO_PIN_109_SD0_RESET : GPIO_PIN_110_SD1_RESET;
    UINT32 Delay = AmbaRTSL_PllGetNumCpuCycleUs();

    if (VddPower > AMBA_SD_VDD_1D80V) {
        GpioLevel = AMBA_GPIO_LEVEL_LOW;
    } else {
        GpioLevel = AMBA_GPIO_LEVEL_HIGH;
    }

    if (VddPower == AMBA_SD_VDD_POWER_OFF) {
        /* Config to 1.8 V before poweroff */
        (void)AmbaGPIO_SetFuncGPO(GpioPinSdHighSpeed, AMBA_GPIO_LEVEL_HIGH);
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_73_SD0_CLK, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_74_SD0_CMD, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_89_SD0_DATA0, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_90_SD0_DATA1, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_91_SD0_DATA2, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_92_SD0_DATA3, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_93_SD0_DATA4, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_94_SD0_DATA5, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_95_SD0_DATA6, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_96_SD0_DATA7, AMBA_GPIO_LEVEL_LOW);
        } else {
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_97_SD1_CLK, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_98_SD1_CMD, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_99_SD1_DATA0, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_100_SD1_DATA1, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_101_SD1_DATA2, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_102_SD1_DATA3, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_105_SD1_DATA4, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_106_SD1_DATA5, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_107_SD1_DATA6, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_108_SD1_DATA7, AMBA_GPIO_LEVEL_LOW);
        }
        (void)AmbaGPIO_SetFuncGPO(GpioPinSdPower, AMBA_GPIO_LEVEL_LOW);
    } else {
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_73_SD0_CLK);
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_74_SD0_CMD);
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_89_SD0_DATA0);
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_90_SD0_DATA1);
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_91_SD0_DATA2);
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_92_SD0_DATA3);
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_93_SD0_DATA4);
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_94_SD0_DATA5);
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_95_SD0_DATA6);
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_96_SD0_DATA7);
        } else {
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_97_SD1_CLK);
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_98_SD1_CMD);
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_99_SD1_DATA0);
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_100_SD1_DATA1);
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_101_SD1_DATA2);
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_102_SD1_DATA3);
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_105_SD1_DATA4);
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_106_SD1_DATA5);
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_107_SD1_DATA6);
            (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_108_SD1_DATA7);
        }
        (void)AmbaGPIO_SetFuncGPO(GpioPinSdHighSpeed, GpioLevel);
        (void)AmbaGPIO_SetFuncGPO(GpioPinSdPower, AMBA_GPIO_LEVEL_HIGH);
    }
    AmbaDelayCycles(30000U * Delay);
}

UINT32 AmbaUserGPIO_SensorResetCtrl(UINT32 VinID, UINT32 PinLevel)
{
    UINT32 RetVal;

    AmbaMisra_TouchUnused(&VinID);

    RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_113, PinLevel);

    (void)AmbaKAL_TaskSleep(2);

    return RetVal;
}

UINT32 AmbaUserGPIO_YuvResetCtrl(UINT32 VinID, UINT32 PinLevel)
{
    UINT32 RetVal;

    AmbaMisra_TouchUnused(&VinID);

    RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_113, PinLevel);

    (void)AmbaKAL_TaskSleep(2);

    return RetVal;
}

UINT32 AmbaUserGPIO_SerdesPowerCtrl(UINT32 VinID, UINT32 PinLevel)
{
    UINT32 RetVal;
    RetVal = GPIO_ERR_NONE;

    AmbaMisra_TouchUnused(&VinID);

#if 1 //Please modify it as #if 0 if not use MAX9296/96712 power and POC control to shorten delay time
    //MAXIM9296/MAX96712 power
    RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_25, PinLevel);
    //MAXIM96712 POC
    RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_22, PinLevel);
    //MAXIM9296 POC
    RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_23, PinLevel);

    if(PinLevel == AMBA_GPIO_LEVEL_LOW) {
        (void)AmbaKAL_TaskSleep(600U);
    } else if(PinLevel == AMBA_GPIO_LEVEL_HIGH) {
        (void)AmbaKAL_TaskSleep(650U);
    } else {
        RetVal = GPIO_ERR_ARG;
    }
#else
    AmbaMisra_TouchUnused(&PinLevel);

#endif

    return RetVal;
}

UINT32 AmbaUserGPIO_SerdesResetCtrl(UINT32 VinID, UINT32 PinLevel)
{
    UINT32 RetVal = GPIO_ERR_NONE;

    static UINT8 flag0 = 0U; //vin0
    static UINT8 flag1 = 0U; //vin1


    if(VinID == 0U) {
        flag0 = 1U;
    } else if(VinID == 1U) {
        flag1 = 1U;
    } else {
        RetVal = VIN_ERR_ARG;
    }

    if((flag0 & flag1) != 1U) {

        RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_6, PinLevel);
        if (RetVal == GPIO_ERR_NONE) {
            RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_113, PinLevel);

        }
#if 1 //Please modify it as #if 0 if not use MAX9296/96712 power and POC control to shorten delay time
        if(PinLevel == AMBA_GPIO_LEVEL_LOW) {
            (void)AmbaKAL_TaskSleep(250U);
        } else if(PinLevel == AMBA_GPIO_LEVEL_HIGH) {
            (void)AmbaKAL_TaskSleep(50U);
        } else {
            RetVal = GPIO_ERR_ARG;
        }
#endif
    } else {
        //not support reset MAX9296 when use Quad MAX9296
    }


    return RetVal;
}

UINT32 AmbaUserVIN_SensorClkCtrl(UINT32 VinID, UINT32 Frequency)
{
    UINT32 RetVal = VIN_ERR_NONE;
    const UINT32 SensorClkChannel[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = AMBA_VIN_SENSOR_CLOCK0,
        [AMBA_VIN_CHANNEL1] = AMBA_VIN_SENSOR_CLOCK1,
    };

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = VIN_ERR_ARG;
    } else {
        if (Frequency == 0U) {
            RetVal = AmbaVIN_SensorClkDisable(SensorClkChannel[VinID]);
        } else {
            RetVal = AmbaVIN_SensorClkEnable(SensorClkChannel[VinID], Frequency);
        }
        (void)AmbaKAL_TaskSleep(2);
    }

    return RetVal;
}
