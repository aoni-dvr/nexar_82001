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
#include "AmbaRTSL_GPIO.h"

#define UART_CHAN AMBA_UART_APB_CHANNEL0
//#define UART_CHAN AMBA_UART_AHB_CHANNEL0

void AmbaUserConsole_Write(UINT32 StringSize, const char *StringBuf, UINT32 TimeOut)
{
    UINT32 SentSize;
    UINT32 TxSize = StringSize;
    UINT32 BufIndex = 0U;
    const UINT8 *pTxBuf;

    AmbaMisra_TypeCast(&pTxBuf, &StringBuf);
    while (AmbaUART_Write(UART_CHAN, 0U, TxSize, &pTxBuf[BufIndex], &SentSize, TimeOut) == UART_ERR_NONE) {
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
    AmbaMisra_TypeCast(&pRxBuf, &StringBuf);
    (void)AmbaUART_Read(UART_CHAN, 0U, StringSize, pRxBuf, &RxSize, TimeOut);

    return RxSize;
}


UINT32 AmbaUserShell_Write(UINT32 StringSize, const char *StringBuf, UINT32 TimeOut)
{
    UINT32 SentSize = 0U;
    const UINT8 *pTxBuf;

    AmbaMisra_TypeCast(&pTxBuf, &StringBuf);
    (void)AmbaUART_Write(UART_CHAN, 0U, StringSize, pTxBuf, &SentSize, TimeOut);

    return SentSize;
}

extern void AmbaUserGPIO_FpdCtrl(UINT32 FpdFlag);
void AmbaUserGPIO_FpdCtrl(UINT32 FpdFlag)
{
    if (FpdFlag == 0U) {
        (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_73, AMBA_GPIO_LEVEL_LOW);
    } else {
        (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_73, AMBA_GPIO_LEVEL_HIGH);
    }
}

extern void AmbaUserSD_PowerCtrl(UINT32 SdChanNo, UINT32 VddPower);
void AmbaUserSD_PowerCtrl(UINT32 SdChanNo, UINT32 VddPower)
{
//    UINT32 GpioLevel;
//    UINT32 GpioPinSdHighSpeed;
    UINT32 GpioPinSdPower = 0U;
    UINT32 Delay = AmbaRTSL_PllGetNumCpuCycleUs();

//    if (VddPower > AMBA_SD_VDD_1D80V) {
//        GpioLevel = AMBA_GPIO_LEVEL_LOW;
//    } else {
//        GpioLevel = AMBA_GPIO_LEVEL_HIGH;
//    }

    /* for CV28 BUB */
    if (SdChanNo == AMBA_SD_CHANNEL0) {
//        GpioPinSdHighSpeed = GPIO_PIN_86;//GPIO_PIN_85;
        GpioPinSdPower = GPIO_PIN_63;
    } else if (SdChanNo == AMBA_SD_CHANNEL1) {
//        GpioPinSdHighSpeed = GPIO_PIN_86;// SDIO0 HS_SEL not work in CV28
        GpioPinSdPower = GPIO_PIN_0;
    } else {
//        GpioPinSdHighSpeed = GPIO_PIN_10;
        GpioPinSdPower = GPIO_PIN_62;
    }

    if (VddPower == AMBA_SD_VDD_POWER_OFF) {
        /* Config to 3.3 V before poweroff */
        // if ((SdChanNo == AMBA_SD_CHANNEL0) || (SdChanNo == AMBA_SD_CHANNEL1)) {
        //     (void)AmbaRTSL_GpioSetFuncGPO(GpioPinSdHighSpeed, AMBA_GPIO_LEVEL_HIGH);
        // }

        if (SdChanNo == AMBA_SD_CHANNEL0) {
            AmbaPrint_PrintUInt5("AMBA_SD_CHANNEL0 AMBA_SD_VDD_POWER_OFF",0,0,0,0,0);

            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_38_SD0_CLK, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_39_SD0_CMD, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_46_SD0_DATA0, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_47_SD0_DATA1, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_48_SD0_DATA2, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_49_SD0_DATA3, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_50_SD0_DATA4, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_51_SD0_DATA5, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_52_SD0_DATA6, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_53_SD0_DATA7, AMBA_GPIO_LEVEL_LOW);
        } else if (SdChanNo == AMBA_SD_CHANNEL1) {
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_54_SD1_CLK, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_55_SD1_CMD, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_56_SD1_DATA0, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_57_SD1_DATA1, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_58_SD1_DATA2, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_59_SD1_DATA3, AMBA_GPIO_LEVEL_LOW);
        } else {
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_77_SD2_CLK, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_78_SD2_CMD, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_73_SD2_DATA0, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_74_SD2_DATA1, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_75_SD2_DATA2, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_76_SD2_DATA3, AMBA_GPIO_LEVEL_LOW);
        }
        (void)AmbaRTSL_GpioSetFuncGPO(GpioPinSdPower, AMBA_GPIO_LEVEL_LOW);

    } else {
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_38_SD0_CLK);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_39_SD0_CMD);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_46_SD0_DATA0);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_47_SD0_DATA1);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_48_SD0_DATA2);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_49_SD0_DATA3);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_50_SD0_DATA4);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_51_SD0_DATA5);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_52_SD0_DATA6);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_53_SD0_DATA7);
        } else if (SdChanNo == AMBA_SD_CHANNEL1) {
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_54_SD1_CLK);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_55_SD1_CMD);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_56_SD1_DATA0);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_57_SD1_DATA1);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_58_SD1_DATA2);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_59_SD1_DATA3);
        } else {
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_77_SD2_CLK);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_78_SD2_CMD);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_73_SD2_DATA0);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_74_SD2_DATA1);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_75_SD2_DATA2);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_76_SD2_DATA3);
        }
        // (void)AmbaRTSL_GpioSetFuncGPO(GpioPinSdHighSpeed, GpioLevel);
        (void)AmbaRTSL_GpioSetFuncGPO(GpioPinSdPower, AMBA_GPIO_LEVEL_HIGH);
    }
    AmbaDelayCycles(30000U * Delay);
}

UINT32 AmbaUserGPIO_SensorResetCtrl(UINT32 VinID, UINT32 PinLevel)
{
    UINT32 RetVal;

    if (VinID == AMBA_VIN_CHANNEL0) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_64, PinLevel);
    } else if (VinID == AMBA_VIN_CHANNEL1) {
        // RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_4, PinLevel);
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_79, PinLevel);
    } else if (VinID == AMBA_VIN_CHANNEL2) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_60, PinLevel);
    } else {
        RetVal = VIN_ERR_ARG ;
    }

    (void)AmbaKAL_TaskSleep(2);

    return RetVal;
}

UINT32 AmbaUserGPIO_YuvResetCtrl(UINT32 VinID, UINT32 PinLevel)
{
    UINT32 RetVal;

    if (VinID == AMBA_VIN_CHANNEL0) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_64, PinLevel);
    } else if (VinID == AMBA_VIN_CHANNEL1) {
        // RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_4, PinLevel);
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_79, PinLevel);
    } else if (VinID == AMBA_VIN_CHANNEL2) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_60, PinLevel);
    } else {
        RetVal = VIN_ERR_ARG ;
    }

    (void)AmbaKAL_TaskSleep(2);

    return RetVal;
}

UINT32 AmbaUserGPIO_SerdesPowerCtrl(UINT32 VinID, UINT32 PinLevel)
{
    UINT32 RetVal = GPIO_ERR_NONE;

    AmbaMisra_TouchUnused(&VinID);
    AmbaMisra_TouchUnused(&PinLevel);

    return RetVal;
}

UINT32 AmbaUserGPIO_SerdesResetCtrl(UINT32 VinID, UINT32 PinLevel)
{
    UINT32 RetVal;

    if (VinID == AMBA_VIN_CHANNEL0) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_64, PinLevel);
    } else if (VinID == AMBA_VIN_CHANNEL1) {
        // RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_4, PinLevel);
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_79, PinLevel);
    } else if (VinID == AMBA_VIN_CHANNEL2) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_60, PinLevel);
    } else {
        RetVal = VIN_ERR_ARG ;
    }

    (void)AmbaKAL_TaskSleep(2);

    return RetVal;
}

UINT32 AmbaUserVIN_SensorClkCtrl(UINT32 VinID, UINT32 Frequency)
{
    UINT32 RetVal = VIN_ERR_NONE;
    const UINT32 SensorClkChannel[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = AMBA_VIN_SENSOR_CLOCK0,
        [AMBA_VIN_CHANNEL1] = AMBA_VIN_SENSOR_CLOCK0,
        [AMBA_VIN_CHANNEL2] = AMBA_VIN_SENSOR_CLOCK0
    };

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = VIN_ERR_ARG;
    } else {
        if (Frequency == 0U) {
            if (SensorClkChannel[VinID] == AMBA_VIN_SENSOR_CLOCK0) {
                RetVal = AmbaVIN_SensorClkDisable(SensorClkChannel[VinID]);
            } else {
                (void)AmbaGPIO_SetFuncGPI(GPIO_PIN_35);
                RetVal = AmbaVIN_SensorClkDisable(SensorClkChannel[VinID]);
            }
        } else {
            if (SensorClkChannel[VinID] == AMBA_VIN_SENSOR_CLOCK0) {
                RetVal = AmbaVIN_SensorClkEnable(SensorClkChannel[VinID], Frequency);
            } else {
                RetVal = AmbaVIN_SensorClkEnable(SensorClkChannel[VinID], Frequency);
                if (RetVal == VIN_ERR_NONE) {
                    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_35_ENET_ALT_REF_CLK1);
                }
            }
        }
        (void)AmbaKAL_TaskSleep(2);
    }

    return RetVal;
}

UINT32 AmbaUserGPIO_EnetResetCtrl(void)
{
    UINT32 RetVal = VIN_ERR_NONE;

    RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_14, AMBA_GPIO_LEVEL_LOW);
    if (RetVal != OK) {
        /* do nothing */
    }

    RetVal = AmbaKAL_TaskSleep(2U);
    if (RetVal != OK) {
        /* do nothing */
    }

    RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_14, AMBA_GPIO_LEVEL_HIGH);
    if (RetVal != OK) {
        /* do nothing */
    }

    RetVal = AmbaKAL_TaskSleep(15U);
    if (RetVal != OK) {
        /* do nothing */
    }

    return RetVal;
}
