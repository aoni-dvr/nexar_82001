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
#include ".svc_autogen"
#include <AmbaWrap.h>

#define UART_CHAN AMBA_UART_APB_CHANNEL0
//#define UART_CHAN AMBA_UART_AHB_CHANNEL0

static int EnableRtosUart = 1;
void AmbaUser_SetRtosUartEnable(int enable)
{
    EnableRtosUart = enable;
}

int AmbaUser_GetRtosUartEnable(void)
{
    return EnableRtosUart;
}

void AmbaUserConsole_Write(UINT32 StringSize, const char *StringBuf, UINT32 TimeOut)
{
    UINT32 SentSize;
    UINT32 TxSize = StringSize;
    UINT32 BufIndex = 0U;
    const UINT8 *pTxBuf;

    if (EnableRtosUart == 0) {
        return;
    }
    AmbaMisra_TypeCast32(&pTxBuf, &StringBuf);
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
    AmbaMisra_TypeCast32(&pRxBuf, &StringBuf);
    (void)AmbaUART_Read(UART_CHAN, 0U, StringSize, pRxBuf, &RxSize, TimeOut);

    return RxSize;
}


UINT32 AmbaUserShell_Write(UINT32 StringSize, const char *StringBuf, UINT32 TimeOut)
{
    UINT32 SentSize = 0U;
    const UINT8 *pTxBuf;

    AmbaMisra_TypeCast32(&pTxBuf, &StringBuf);
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

    /* for CV25 BUB */
    if (SdChanNo == AMBA_SD_CHANNEL0) {
        GpioPinSdHighSpeed = GPIO_PIN_105;
        GpioPinSdPower = GPIO_PIN_81;
    } else if (SdChanNo == AMBA_SD_CHANNEL1) {
        GpioPinSdHighSpeed = GPIO_PIN_0;
        GpioPinSdPower = GPIO_PIN_80;
    } else {
        GpioPinSdHighSpeed = GPIO_PIN_17;
    }

    if (VddPower == AMBA_SD_VDD_POWER_OFF) {
        /* Config to 1.8 V before poweroff */
        (void)AmbaRTSL_GpioSetFuncGPO(GpioPinSdHighSpeed, AMBA_GPIO_LEVEL_HIGH);
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_48_SD0_CLK, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_49_SD0_CMD, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_64_SD0_DATA0, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_65_SD0_DATA1, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_66_SD0_DATA2, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_67_SD0_DATA3, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_68_SD0_DATA4, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_69_SD0_DATA5, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_70_SD0_DATA6, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_71_SD0_DATA7, AMBA_GPIO_LEVEL_LOW);
        } else if (SdChanNo == AMBA_SD_CHANNEL1) {
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_72_SD1_CLK, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_73_SD1_CMD, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_74_SD1_DATA0, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_75_SD1_DATA1, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_76_SD1_DATA2, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_77_SD1_DATA3, AMBA_GPIO_LEVEL_LOW);
        } else {
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_101_SD2_CLK, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_102_SD2_CMD, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_97_SD2_DATA0, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_98_SD2_DATA1, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_99_SD2_DATA2, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaRTSL_GpioSetFuncGPO(GPIO_PIN_100_SD2_DATA3, AMBA_GPIO_LEVEL_LOW);
        }

        if ((SdChanNo == AMBA_SD_CHANNEL0) || (SdChanNo == AMBA_SD_CHANNEL1)) {
            (void)AmbaRTSL_GpioSetFuncGPO(GpioPinSdPower, AMBA_GPIO_LEVEL_LOW);
        }
    } else {
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_48_SD0_CLK);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_49_SD0_CMD);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_64_SD0_DATA0);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_65_SD0_DATA1);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_66_SD0_DATA2);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_67_SD0_DATA3);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_68_SD0_DATA4);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_69_SD0_DATA5);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_70_SD0_DATA6);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_71_SD0_DATA7);
        } else if (SdChanNo == AMBA_SD_CHANNEL1) {
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_72_SD1_CLK);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_73_SD1_CMD);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_74_SD1_DATA0);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_75_SD1_DATA1);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_76_SD1_DATA2);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_77_SD1_DATA3);
        } else {
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_101_SD2_CLK);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_102_SD2_CMD);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_97_SD2_DATA0);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_98_SD2_DATA1);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_99_SD2_DATA2);
            (void)AmbaRTSL_GpioSetFuncAlt(GPIO_PIN_100_SD2_DATA3);
        }
        (void)AmbaRTSL_GpioSetFuncGPO(GpioPinSdHighSpeed, GpioLevel);
        if ((SdChanNo == AMBA_SD_CHANNEL0) || (SdChanNo == AMBA_SD_CHANNEL1)) {
            (void)AmbaRTSL_GpioSetFuncGPO(GpioPinSdPower, AMBA_GPIO_LEVEL_HIGH);
        }
    }
    AmbaKAL_TaskSleep(200);
    AmbaDelayCycles(30000U * Delay);
}

typedef struct {
    UINT16  OemID;       /* OID (OEM/Application ID) */
    UINT8   MID;         /* MID (Manufacturer ID) */
    UINT8   Name[5];     /* PNM (Product name) */
    AMBA_SD_SETTING_s SdSetting; /* SD signal setting */
} AMBA_SD_DELAY_CASE_s;

static AMBA_SD_DELAY_CASE_s AmbaSD_DelayCaseCh0[] = {
    [0] = {
        .OemID = 0x4150U,
        .MID   = 0x9U,
        .Name  = "MB68B",
        .SdSetting = {
            .DetailDelay = SVCAG_SD0_DELAY,
            .InitFrequency = 300000U,
            .MaxFrequency = 80000000U,
        }
    },
};

static int sd_phyctrl_debug = 1;
void AmbaUserSD_PhyCtrl_SetDebug(int enable)
{
    sd_phyctrl_debug = enable;
}

int AmbaUserSD_PhyCtrl_GetDebug(void)
{
    return sd_phyctrl_debug;
}

void AmbaUserSD_PhyCtrl(UINT32 SdChanNo, AMBA_SD_INFO_CID_INFO_s *pCardID)
{
    UINT32 i, Found = 0U;

    if (sd_phyctrl_debug) {
        AmbaPrint_PrintUInt5("AmbaUserSD_PhyCtrl: CH %d, OemID 0x%x, MID 0x%x", SdChanNo, pCardID->OemID, pCardID->ManufacturerID, 0U, 0U);
        AmbaPrint_PrintStr5("AmbaUserSD_PhyCtrl: %s", (char *)(pCardID->ProductName), NULL, NULL, NULL, NULL);
    }
    for (i = 0; i < sizeof(AmbaSD_DelayCaseCh0) / sizeof(AMBA_SD_DELAY_CASE_s); i++) {
        if ((AmbaSD_DelayCaseCh0[i].MID == pCardID->ManufacturerID)
            && (AmbaSD_DelayCaseCh0[i].OemID == pCardID->OemID)) {
            INT32 CmpResult = 0;
            if ((AmbaWrap_memcmp((char *)pCardID->ProductName, (char *)AmbaSD_DelayCaseCh0[i].Name, 5, &CmpResult) == 0)
                && (CmpResult == 0)) {
                if (sd_phyctrl_debug) {
                    AmbaPrint_PrintUInt5("Card Full Matched. Index=%d", i, 0U, 0U, 0U, 0U);
                }
                AmbaSD_DelayCaseCh0[i].SdSetting.ClockDrive    = AMBA_SD_DRIVE_STRENGTH_12MA;
                AmbaSD_DelayCaseCh0[i].SdSetting.DataDrive     = AMBA_SD_DRIVE_STRENGTH_12MA;
                AmbaSD_DelayCaseCh0[i].SdSetting.CMDDrive      = AMBA_SD_DRIVE_STRENGTH_12MA;
                AmbaSD_DelayCaseCh0[i].SdSetting.CDDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
                AmbaSD_DelayCaseCh0[i].SdSetting.WPDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
                (void) AmbaSD_SetSdSetting(SdChanNo, &AmbaSD_DelayCaseCh0[i].SdSetting);
                Found = 1U;
                break;
            }
        }
    }

    if (Found == 0U) {
        AMBA_SD_SETTING_s SdDefaultSetting;
        SdDefaultSetting.ClockDrive    = AMBA_SD_DRIVE_STRENGTH_12MA;
        SdDefaultSetting.DataDrive     = AMBA_SD_DRIVE_STRENGTH_12MA;
        SdDefaultSetting.CMDDrive      = AMBA_SD_DRIVE_STRENGTH_12MA;
        SdDefaultSetting.CDDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
        SdDefaultSetting.WPDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
        SdDefaultSetting.DetailDelay   = SVCAG_SD0_DELAY;
        SdDefaultSetting.InitFrequency = 300000U;
        SdDefaultSetting.MaxFrequency  = SVCAG_SD0_CLOCK;
        (void) AmbaSD_SetSdSetting(SdChanNo, &SdDefaultSetting);
    }
}

UINT32 AmbaUserGPIO_SensorResetCtrl(UINT32 VinID, UINT32 PinLevel)
{
    UINT32 RetVal;

    if (VinID == AMBA_VIN_CHANNEL0) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_84, PinLevel);
    } else if (VinID == AMBA_VIN_CHANNEL1) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_74, PinLevel);
    } else if (VinID == AMBA_VIN_CHANNEL2) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_107, PinLevel);
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
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_84, PinLevel);
    } else if (VinID == AMBA_VIN_CHANNEL1) {
#if defined(CONFIG_SVC_APPS_ICAM)
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_7, PinLevel);
#else
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_21, PinLevel);
#endif
    } else if (VinID == AMBA_VIN_CHANNEL2) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_107, PinLevel);
    } else {
        RetVal = VIN_ERR_ARG ;
    }

    (void)AmbaKAL_TaskSleep(2);

    return RetVal;
}

UINT32 AmbaUserGPIO_SerdesPowerCtrl(UINT32 VinID, UINT32 PinLevel)
{
    UINT32 RetVal;
    RetVal = GPIO_ERR_NONE;

    AmbaMisra_TouchUnused(&VinID);

#if 1 //Please modify it as #if 0 if not use MAX9296/96712 power and POC control to shorten delay time
    //MAXIM9296/MAX96712 power: comment it since not support alt/GPIO switch in run time in Linux
    //RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_7, PinLevel);
    //MAXIM96712 POC
    RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_4, PinLevel);
    //MAXIM9296 POC
    RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_5, PinLevel);

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



#if defined(CONFIG_SVC_APPS_ICAM)
    if (VinID == AMBA_VIN_CHANNEL0) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_84, PinLevel);
    } else if (VinID == AMBA_VIN_CHANNEL1) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_7, PinLevel);
    } else if (VinID == AMBA_VIN_CHANNEL2) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_107, PinLevel);
    } else {
        RetVal = VIN_ERR_ARG ;
    }
#else
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
        RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_21, PinLevel);
        if (RetVal == GPIO_ERR_NONE) {
            RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_84, PinLevel);
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
#endif

    return RetVal;
}

UINT32 AmbaUserVIN_SensorClkCtrl(UINT32 VinID, UINT32 Frequency)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 SensorClkChannel[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = AMBA_VIN_SENSOR_CLOCK0,
        [AMBA_VIN_CHANNEL1] = AMBA_VIN_SENSOR_CLOCK1,
        [AMBA_VIN_CHANNEL2] = AMBA_VIN_SENSOR_CLOCK1
    };

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = VIN_ERR_ARG;
    } else {
        if (Frequency == 0U) {
            if (SensorClkChannel[VinID] == AMBA_VIN_SENSOR_CLOCK0) {
                RetVal = AmbaVIN_SensorClkDisable(SensorClkChannel[VinID]);
            } else {
                (void)AmbaGPIO_SetFuncGPI(GPIO_PIN_44);
                RetVal = AmbaVIN_SensorClkDisable(SensorClkChannel[VinID]);
            }
        } else {
            if (SensorClkChannel[VinID] == AMBA_VIN_SENSOR_CLOCK0) {
                RetVal = AmbaVIN_SensorClkEnable(SensorClkChannel[VinID], Frequency);
            } else {
                RetVal = AmbaVIN_SensorClkEnable(SensorClkChannel[VinID], Frequency);
                if (RetVal == VIN_ERR_NONE) {
                    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_44_ENET_REF_CLK1);
                }
            }
        }
        (void)AmbaKAL_TaskSleep(2);
    }

    return RetVal;
}
