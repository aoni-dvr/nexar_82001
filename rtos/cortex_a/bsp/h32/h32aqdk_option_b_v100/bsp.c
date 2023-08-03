/**
 *  @file bsp.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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

#include "AmbaGPIO.h"
#include "AmbaUART.h"
#include "AmbaSD.h"
#include "AmbaVIN.h"
#include "AmbaRTSL_PLL.h"

#include "bsp.h"
#include "AmbaMisraFix.h"
#include "AmbaRTSL_GPIO.h"


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
    /* v100 need to be rework, then BackLight will use PWM0(GPIO12) */
    if (FpdFlag == 0U) {
        (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_12, AMBA_GPIO_LEVEL_LOW);
    } else {
        (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_12, AMBA_GPIO_LEVEL_HIGH);
    }
}

extern void AmbaUserSD_PowerCtrl(UINT32 SdChanNo, UINT32 VddPower);
void AmbaUserSD_PowerCtrl(UINT32 SdChanNo, UINT32 VddPower)
{
    UINT32 GpioLevel;
    UINT32 GpioPinSdHighSpeed;
    UINT32 GpioPinSdPower = 0U;

    if (VddPower > AMBA_SD_VDD_1D80V) {
        GpioLevel = AMBA_GPIO_LEVEL_LOW;
    } else {
        GpioLevel = AMBA_GPIO_LEVEL_HIGH;
    }
    /* for H32 DK */
    if (SdChanNo == AMBA_SD_CHANNEL0) {
        GpioPinSdHighSpeed = GPIO_PIN_85;
        GpioPinSdPower = GPIO_PIN_0;
    } else if (SdChanNo == AMBA_SD_CHANNEL1) {
        GpioPinSdHighSpeed = GPIO_PIN_41;
        GpioPinSdPower = GPIO_PIN_82;
    } else {
        /* H32 DK has no this card slot */
    }

    if (VddPower == AMBA_SD_VDD_POWER_OFF) {
        if (SdChanNo == AMBA_SD_CHANNEL0) {
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
            /* H32 DK has no this card slot */
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
            /* H32 DK has no this card slot */
        }
        (void)AmbaRTSL_GpioSetFuncGPO(GpioPinSdHighSpeed, GpioLevel);
        (void)AmbaRTSL_GpioSetFuncGPO(GpioPinSdPower, AMBA_GPIO_LEVEL_HIGH);
    }
    (void)AmbaRTSL_RctTimerWait(30000);
}

extern UINT32 AmbaUserSD_VoltSwitchSupportChk(UINT32 SdChanNo);
UINT32 AmbaUserSD_VoltSwitchSupportChk(UINT32 SdChanNo)
{
    UINT32 VoltageSwitchSupport;

    if (SdChanNo == AMBA_SD_CHANNEL0) {
        VoltageSwitchSupport = 0U; /* no power switch IC for SD_VDDO */
    } else if (SdChanNo == AMBA_SD_CHANNEL1) {
        VoltageSwitchSupport = 0U; /* no power switch IC for SDIO_VDDO */
    } else {
        VoltageSwitchSupport = 0U; /* H32 DK has no this card slot */
    }
    (void) SdChanNo;

    return VoltageSwitchSupport;
}

extern void AmbaUserSD_DelayCtrl(UINT32 SdChanNo, AMBA_SD_INFO_CID_INFO_s *pCardID);
void AmbaUserSD_DelayCtrl(UINT32 SdChanNo, AMBA_SD_INFO_CID_INFO_s *pCardID)
{
    UINT8  Mid[] = {0x9C, 0x74};
    UINT16 Oid[] = {0x534F, 0x4A45};
    UINT32 i, Count = sizeof(Mid) / sizeof(pCardID->ManufacturerID);
    AMBA_SD_SETTING_s SdSetting = {
        .ClockDrive    = AMBA_SD_DRIVE_STRENGTH_12MA,
        .DataDrive     = AMBA_SD_DRIVE_STRENGTH_12MA,
        .CMDDrive      = AMBA_SD_DRIVE_STRENGTH_12MA,
        .CDDrive       = AMBA_SD_DRIVE_STRENGTH_12MA,
        .WPDrive       = AMBA_SD_DRIVE_STRENGTH_12MA,

        .DetailDelay   = 0U,         /* 0U: no change */
        .HsRdLatency   = 0U,         /* 0U: no change */

        .InitFrequency = 0U,         /* 0U: no change */
        .MaxFrequency  = 44000000U,
    };

    for (i = 0U; i < Count; i++) {
        if ((Mid[i] == pCardID->ManufacturerID) && (Oid[i] == pCardID->OemID)) {
            switch(i) {
            case 0: /* KINGMAX PRO Extreme microSDXC U3 I Class10 64GB */
                (void) AmbaSD_SetSdSetting(SdChanNo, &SdSetting);
                break;
            case 1: /* Transcend microSDHC Class10 8GB (9173BA 8G 01DS2) */
                (void) AmbaSD_SetSdSetting(SdChanNo, &SdSetting);
                break;
            default:
                AmbaMisra_TouchUnused(&SdChanNo);
                break;
            }
        }
    }

    AmbaMisra_TouchUnused(pCardID); // to avoid misraC error
}

UINT32 AmbaUserGPIO_SensorResetCtrl(UINT32 VinID, UINT32 PinLevel)
{
    UINT32 RetVal;

    if (VinID == AMBA_VIN_CHANNEL0) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_66, PinLevel);
    } else if (VinID == AMBA_VIN_CHANNEL1) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_81, PinLevel);
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
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_66, PinLevel);
    } else if (VinID == AMBA_VIN_CHANNEL1) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_81, PinLevel);
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

#if 1 //Please modify it as #if 0 if not use MAX9296/96712 power and POC control to shorten delay time
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
        //MAX9296/96712 share same GPIO to control POC power for vin0/vin1
        //vin0 MAX9296/96712 power
        RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_78, PinLevel);
        //vin1 MAX9296/96712 power
        RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_5, PinLevel);
        //vin0/vin1 MAX96712 POC
        RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_1, PinLevel);
        //vin0/vin1 MAXIM9296 POC
        RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_2, PinLevel);

#ifndef CONFIG_SVC_APPS_ICAM
        if(PinLevel == AMBA_GPIO_LEVEL_LOW) {
            (void)AmbaKAL_TaskSleep(600U);
        } else if(PinLevel == AMBA_GPIO_LEVEL_HIGH) {
            (void)AmbaKAL_TaskSleep(650U);
        } else {
            RetVal = GPIO_ERR_ARG;
        }
#endif
    } else {
        //can't power off/on to affect other MAX9296/96712 POC
        RetVal = GPIO_ERR_NONE;
    }
#else
    AmbaMisra_TouchUnused(&VinID);
    AmbaMisra_TouchUnused(&PinLevel);
#endif

    return RetVal;
}

UINT32 AmbaUserGPIO_SerdesResetCtrl(UINT32 VinID, UINT32 PinLevel)
{
    UINT32 RetVal;

    if (VinID == AMBA_VIN_CHANNEL0) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_66, PinLevel);
    } else if (VinID == AMBA_VIN_CHANNEL1) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_81, PinLevel);
    } else if (VinID == AMBA_VIN_CHANNEL2) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_60, PinLevel);
    } else {
        RetVal = VIN_ERR_ARG ;
    }
#if 1 //Please modify it as #if 0 if not use MAX9296/96712 power and POC control to shorten delay time
#ifndef CONFIG_SVC_APPS_ICAM
    if(PinLevel == AMBA_GPIO_LEVEL_LOW) {
        (void)AmbaKAL_TaskSleep(250U);
    } else if(PinLevel == AMBA_GPIO_LEVEL_HIGH) {
        (void)AmbaKAL_TaskSleep(50U);
    } else {
        RetVal = GPIO_ERR_ARG;
    }
#endif
#endif
    return RetVal;
}

UINT32 AmbaUserVIN_SensorClkCtrl(UINT32 VinID, UINT32 Frequency)
{
    UINT32 RetVal = VIN_ERR_NONE;
    const UINT32 SensorClkChannel[AMBA_NUM_VIN_CHANNEL] = {
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
//                (void)AmbaGPIO_SetFuncGPI(GPIO_PIN_33);
                RetVal = AmbaVIN_SensorClkDisable(SensorClkChannel[VinID]);
            }
        } else {
            if (SensorClkChannel[VinID] == AMBA_VIN_SENSOR_CLOCK0) {
                RetVal = AmbaVIN_SensorClkEnable(SensorClkChannel[VinID], Frequency);
            } else {
                RetVal = AmbaVIN_SensorClkEnable(SensorClkChannel[VinID], Frequency);
                if (RetVal == VIN_ERR_NONE) {
                    //                  (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_33_ENET_ALT_REF_CLK1);
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

    RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_12, AMBA_GPIO_LEVEL_LOW);
    if (RetVal != OK) {
        /* do nothing */
    }

    RetVal = AmbaKAL_TaskSleep(2U);
    if (RetVal != OK) {
        /* do nothing */
    }

    RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_12, AMBA_GPIO_LEVEL_HIGH);
    if (RetVal != OK) {
        /* do nothing */
    }

    RetVal = AmbaKAL_TaskSleep(15U);
    if (RetVal != OK) {
        /* do nothing */
    }

    return RetVal;
}
