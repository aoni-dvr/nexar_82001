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
#include "AmbaWrap.h"
#include "AmbaRTSL_GPIO.h"

#define UART_CHAN AMBA_UART_APB_CHANNEL0
//#define UART_CHAN AMBA_UART_AHB_CHANNEL0

void AmbaUserConsole_Write(UINT32 StringSize, const char *StringBuf, UINT32 TimeOut)
{
    UINT32 SentSize;
    UINT32 TxSize = StringSize;
    UINT32 BufIndex = 0U;
    const UINT8 *pTxBuf;

    (void) AmbaWrap_memcpy(&pTxBuf, &StringBuf, sizeof(StringBuf));
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
    (void) AmbaWrap_memcpy(&pRxBuf, &StringBuf, sizeof(StringBuf));
    (void)AmbaUART_Read(UART_CHAN, 0U, StringSize, pRxBuf, &RxSize, TimeOut);

    return RxSize;
}


UINT32 AmbaUserShell_Write(UINT32 StringSize, const char *StringBuf, UINT32 TimeOut)
{
    UINT32 SentSize = 0U;
    const UINT8 *pTxBuf;

    (void) AmbaWrap_memcpy(&pTxBuf, &StringBuf, sizeof(StringBuf));
    (void)AmbaUART_Write(UART_CHAN, 0U, StringSize, pTxBuf, &SentSize, TimeOut);

    return SentSize;
}

extern void AmbaUserGPIO_FpdCtrl(UINT32 FpdFlag);
void AmbaUserGPIO_FpdCtrl(UINT32 FpdFlag)
{
    if (FpdFlag == 0U) {
        (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_19, AMBA_GPIO_LEVEL_LOW);
    } else {
        (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_19, AMBA_GPIO_LEVEL_HIGH);
    }
}

extern void AmbaUserSD_PowerCtrl(UINT32 SdChanNo, UINT32 VddPower);
void AmbaUserSD_PowerCtrl(UINT32 SdChanNo, UINT32 VddPower)
{
    UINT32 GpioLevel;
    UINT32 GpioPinSdHighSpeed;
    UINT32 GpioPinSdPower = 0U;
    UINT32 Delay = AmbaRTSL_PllGetNumCpuCycleUs();

    if (VddPower > AMBA_SD_VDD_1D80V) {
        GpioLevel = AMBA_GPIO_LEVEL_LOW;
    } else {
        GpioLevel = AMBA_GPIO_LEVEL_HIGH;
    }

    /* for CV28 BUB */
    if (SdChanNo == AMBA_SD_CHANNEL0) {
        GpioPinSdHighSpeed = GPIO_PIN_129_SD0_HS_SEL;
        GpioPinSdPower = GPIO_PIN_91_SD0_RESET;
    } else if (SdChanNo == AMBA_SD_CHANNEL1) {
        GpioPinSdHighSpeed = GPIO_PIN_130_SD1_HS_SEL;
        GpioPinSdPower = GPIO_PIN_100_SD1_RESET;
    } else {
        GpioPinSdHighSpeed = GPIO_PIN_132_SD2_HS_SEL;
        GpioPinSdPower = GPIO_PIN_109_SD0_RESET;
    }

    if (VddPower == AMBA_SD_VDD_POWER_OFF) {
        /* Config to 1.8 V before poweroff */
        // if ((SdChanNo == AMBA_SD_CHANNEL0) || (SdChanNo == AMBA_SD_CHANNEL1)) {
        (void)AmbaRTSL_GpioSetFuncGPO(GpioPinSdHighSpeed, AMBA_GPIO_LEVEL_HIGH);
        // }
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_87_SD0_CLK, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_88_SD0_CMD, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_79_SD0_DATA0, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_80_SD0_DATA1, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_81_SD0_DATA2, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_82_SD0_DATA3, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_83_SD0_DATA4, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_84_SD0_DATA5, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_85_SD0_DATA6, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_86_SD0_DATA7, AMBA_GPIO_LEVEL_LOW);
        } else if (SdChanNo == AMBA_SD_CHANNEL1) {
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_96_SD1_CLK, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_97_SD1_CMD, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_92_SD1_DATA0, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_93_SD1_DATA1, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_94_SD1_DATA2, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_95_SD1_DATA3, AMBA_GPIO_LEVEL_LOW);
        } else {
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_105_SD2_CLK, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_106_SD2_CMD, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_101_SD2_DATA0, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_102_SD2_DATA1, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_103_SD2_DATA2, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_104_SD2_DATA3, AMBA_GPIO_LEVEL_LOW);
        }
        (void)AmbaRTSL_GpioSetFuncGPO(GpioPinSdPower, AMBA_GPIO_LEVEL_LOW);
    } else {
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_87_SD0_CLK);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_88_SD0_CMD);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_79_SD0_DATA0);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_80_SD0_DATA1);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_81_SD0_DATA2);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_82_SD0_DATA3);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_83_SD0_DATA4);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_84_SD0_DATA5);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_85_SD0_DATA6);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_86_SD0_DATA7);
        } else if (SdChanNo == AMBA_SD_CHANNEL1) {
            // (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_98_SD1_CD); /* may be altered by Vin diag command */
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_99_SD1_WP);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_96_SD1_CLK);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_97_SD1_CMD);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_92_SD1_DATA0);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_93_SD1_DATA1);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_94_SD1_DATA2);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_95_SD1_DATA3);
        } else {
            //(void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_107_SD2_CD);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_108_SD2_WP);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_105_SD2_CLK);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_106_SD2_CMD);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_101_SD2_DATA0);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_102_SD2_DATA1);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_103_SD2_DATA2);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_104_SD2_DATA3);
        }
        // if ((SdChanNo == AMBA_SD_CHANNEL0) || (SdChanNo == AMBA_SD_CHANNEL1)) {
        (void)AmbaRTSL_GpioSetFuncGPO(GpioPinSdHighSpeed, GpioLevel);
        // }
        (void)AmbaRTSL_GpioSetFuncGPO(GpioPinSdPower, AMBA_GPIO_LEVEL_HIGH);
    }
    AmbaDelayCycles(30000U * Delay);
}

static UINT32 SensorResetPin[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0]  = GPIO_PIN_62,
    [AMBA_VIN_CHANNEL1]  = GPIO_PIN_62,
    [AMBA_VIN_CHANNEL2]  = GPIO_PIN_62,
    [AMBA_VIN_CHANNEL3]  = GPIO_PIN_62,
    [AMBA_VIN_CHANNEL4]  = GPIO_PIN_63,
    [AMBA_VIN_CHANNEL5]  = GPIO_PIN_63,
    [AMBA_VIN_CHANNEL6]  = GPIO_PIN_63,
    [AMBA_VIN_CHANNEL7]  = GPIO_PIN_63,
    [AMBA_VIN_CHANNEL8]  = GPIO_PIN_60,
    [AMBA_VIN_CHANNEL9]  = GPIO_PIN_60,
    [AMBA_VIN_CHANNEL10] = GPIO_PIN_60,
    [AMBA_VIN_CHANNEL11] = GPIO_PIN_61,
    [AMBA_VIN_CHANNEL12] = GPIO_PIN_61,
    [AMBA_VIN_CHANNEL13] = GPIO_PIN_61,
};

UINT32 AmbaUserGPIO_SensorResetCtrl(UINT32 VinID, UINT32 PinLevel)
{
    UINT32 RetVal;

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = VIN_ERR_ARG;
    } else {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_116, PinLevel);
        RetVal = AmbaGPIO_SetFuncGPO(SensorResetPin[VinID], PinLevel);

        (void)AmbaKAL_TaskSleep(2);
    }

    return RetVal;
}

UINT32 AmbaUserGPIO_YuvResetCtrl(UINT32 VinID, UINT32 PinLevel)
{
    UINT32 RetVal;

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = VIN_ERR_ARG;
    } else {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_116, PinLevel);
        RetVal = AmbaGPIO_SetFuncGPO(SensorResetPin[VinID], PinLevel);

        (void)AmbaKAL_TaskSleep(2);
    }

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

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = VIN_ERR_ARG;
    } else {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_116, PinLevel);
        RetVal = AmbaGPIO_SetFuncGPO(SensorResetPin[VinID], PinLevel);

        (void)AmbaKAL_TaskSleep(2);
    }

    return RetVal;
}

UINT32 AmbaUserVIN_SensorClkCtrl(UINT32 VinID, UINT32 Frequency)
{
    UINT32 RetVal = VIN_ERR_NONE;
    const UINT32 SensorClkChannel[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0]  = AMBA_VIN_SENSOR_CLOCK1,
        [AMBA_VIN_CHANNEL1]  = AMBA_VIN_SENSOR_CLOCK1,
        [AMBA_VIN_CHANNEL2]  = AMBA_VIN_SENSOR_CLOCK1,
        [AMBA_VIN_CHANNEL3]  = AMBA_VIN_SENSOR_CLOCK1,
        [AMBA_VIN_CHANNEL4]  = AMBA_VIN_SENSOR_CLOCK0,
        [AMBA_VIN_CHANNEL5]  = AMBA_VIN_SENSOR_CLOCK0,
        [AMBA_VIN_CHANNEL6]  = AMBA_VIN_SENSOR_CLOCK0,
        [AMBA_VIN_CHANNEL7]  = AMBA_VIN_SENSOR_CLOCK0,
        [AMBA_VIN_CHANNEL8]  = AMBA_VIN_SENSOR_CLOCK1,
        [AMBA_VIN_CHANNEL9]  = AMBA_VIN_SENSOR_CLOCK1,
        [AMBA_VIN_CHANNEL10] = AMBA_VIN_SENSOR_CLOCK1,
        [AMBA_VIN_CHANNEL11] = AMBA_VIN_SENSOR_CLOCK1,
        [AMBA_VIN_CHANNEL12] = AMBA_VIN_SENSOR_CLOCK1,
        [AMBA_VIN_CHANNEL13] = AMBA_VIN_SENSOR_CLOCK1,
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
