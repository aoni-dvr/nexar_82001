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
#include "AmbaWrap.h"

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
        (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_44, AMBA_GPIO_LEVEL_LOW);
    } else {
        (void)AmbaGPIO_SetFuncGPO(GPIO_PIN_44, AMBA_GPIO_LEVEL_HIGH);
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
    AmbaDelayCycles(30000U * Delay);
}


typedef struct {
    UINT16  OemID;       /* OID (OEM/Application ID) */
    UINT8   MID;         /* MID (Manufacturer ID) */
    UINT32  SerialNo;    /* Serial Number */
    char    Name[5];     /* PNM (Product name) */

    AMBA_SD_SETTING_s SdSetting; /* SD signal setting */
} AMBA_SD_DELAY_CASE_s;

#define SD_MAX_DELAYCASE 4U
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUserSD_DelayCtrl
 *
 *  @Description:: SD Delay control
 *
 *  @Input      ::
 *      SdChanNo: SD Channel number
 *      pCardID: Point to the CID structure
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
extern void AmbaUserSD_PhyCtrl(UINT32 SdChanNo, AMBA_SD_INFO_CID_INFO_s *pCardID);

void AmbaUserSD_PhyCtrl(UINT32 SdChanNo, AMBA_SD_INFO_CID_INFO_s *pCardID)
{
    const AMBA_SD_DELAY_CASE_s *pDelayCase;
    AMBA_SD_DELAY_CASE_s AmbaSD_DelayCaseCh0[SD_MAX_DELAYCASE];
    AMBA_SD_SETTING_s SdDefaultSetting;
    UINT32 i, Found = 0U;

    (void) AmbaWrap_memset(AmbaSD_DelayCaseCh0, 0, sizeof(AMBA_SD_DELAY_CASE_s));

    AmbaSD_DelayCaseCh0[0].OemID = 0x3432U;
    AmbaSD_DelayCaseCh0[0].MID   = 0x41U;
    (void) AmbaWrap_memcpy(AmbaSD_DelayCaseCh0[0].Name, "OTHER", 5U);

    AmbaSD_DelayCaseCh0[0].SdSetting.ClockDrive    = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[0].SdSetting.DataDrive     = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[0].SdSetting.CMDDrive      = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[0].SdSetting.CDDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[0].SdSetting.WPDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[0].SdSetting.DetailDelay   = 0x6401U;
    AmbaSD_DelayCaseCh0[0].SdSetting.HsRdLatency   = 0U;
    AmbaSD_DelayCaseCh0[0].SdSetting.InitFrequency = 300000U;
    AmbaSD_DelayCaseCh0[0].SdSetting.MaxFrequency  = 80000000U;

    AmbaSD_DelayCaseCh0[1].OemID = 0x4245U;
    AmbaSD_DelayCaseCh0[1].MID   = 0x28U;
    (void) AmbaWrap_memcpy(AmbaSD_DelayCaseCh0[1].Name, "OTHER", 5U);

    AmbaSD_DelayCaseCh0[1].SdSetting.ClockDrive    = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[1].SdSetting.DataDrive     = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[1].SdSetting.CMDDrive      = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[1].SdSetting.CDDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[1].SdSetting.WPDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[1].SdSetting.DetailDelay   = 0x4601U;
    AmbaSD_DelayCaseCh0[1].SdSetting.HsRdLatency   = 0U;
    AmbaSD_DelayCaseCh0[1].SdSetting.InitFrequency = 300000U;
    AmbaSD_DelayCaseCh0[1].SdSetting.MaxFrequency  = 80000000U;

    AmbaSD_DelayCaseCh0[2].OemID = 0x5048U;
    AmbaSD_DelayCaseCh0[2].MID   = 0x27U;
    (void) AmbaWrap_memcpy(AmbaSD_DelayCaseCh0[2].Name, "OTHER", 5U);

    AmbaSD_DelayCaseCh0[2].SdSetting.ClockDrive    = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[2].SdSetting.DataDrive     = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[2].SdSetting.CMDDrive      = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[2].SdSetting.CDDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[2].SdSetting.WPDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[2].SdSetting.DetailDelay   = 0x4601U;
    AmbaSD_DelayCaseCh0[2].SdSetting.HsRdLatency   = 0U;
    AmbaSD_DelayCaseCh0[2].SdSetting.InitFrequency = 300000U;
    AmbaSD_DelayCaseCh0[2].SdSetting.MaxFrequency  = 80000000U;

    AmbaSD_DelayCaseCh0[3].OemID = 0x5048U;
    AmbaSD_DelayCaseCh0[3].MID   = 0x27U;
    (void) AmbaWrap_memcpy(AmbaSD_DelayCaseCh0[3].Name, "OTHER", 5U);

    AmbaSD_DelayCaseCh0[3].SdSetting.ClockDrive    = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[3].SdSetting.DataDrive     = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[3].SdSetting.CMDDrive      = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[3].SdSetting.CDDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[3].SdSetting.WPDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
    AmbaSD_DelayCaseCh0[3].SdSetting.DetailDelay   = 0x4601U;
    AmbaSD_DelayCaseCh0[3].SdSetting.HsRdLatency   = 0U;
    AmbaSD_DelayCaseCh0[3].SdSetting.InitFrequency = 300000U;
    AmbaSD_DelayCaseCh0[3].SdSetting.MaxFrequency  = 100000000U;

    SdDefaultSetting.ClockDrive    = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdDefaultSetting.DataDrive     = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdDefaultSetting.CMDDrive      = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdDefaultSetting.CDDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdDefaultSetting.WPDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
    SdDefaultSetting.DetailDelay   = 0x6541U;
    SdDefaultSetting.HsRdLatency   = 0U;
    SdDefaultSetting.InitFrequency = 300000U;
    SdDefaultSetting.MaxFrequency  = 100000000U;

    if (SdChanNo == AMBA_SD_CHANNEL0) {
        pDelayCase = AmbaSD_DelayCaseCh0;
    } else {
        /* FIXME: Need to add other channel */
        pDelayCase = AmbaSD_DelayCaseCh0;
    }

    for(i = 0; i < SD_MAX_DELAYCASE; i++) {
        if ((pDelayCase[i].MID   == pCardID->ManufacturerID) &&
            (pDelayCase[i].OemID == pCardID->OemID)) {

            if(0 == AmbaUtility_StringCompare((char*)pCardID->ProductName, pDelayCase[i].Name, 5)) {
                (void) AmbaSD_SetSdSetting(SdChanNo, &pDelayCase[i].SdSetting);
                Found = 1U;
            } else if (0 == AmbaUtility_StringCompare(pDelayCase[i].Name, "OTHER", 5U)) {
                (void) AmbaSD_SetSdSetting(SdChanNo, &pDelayCase[i].SdSetting);
                Found = 1U;
            } else {
                // For misrac check
            }
        }

        if (Found != 0U) {
            break;
        }
    }

    if (Found == 0U) {
        (void) AmbaSD_SetSdSetting(SdChanNo, &SdDefaultSetting);
    }

    (void) pCardID;
}

UINT32 AmbaUserGPIO_SensorResetCtrl(UINT32 VinID, UINT32 PinLevel)
{
    UINT32 RetVal;

    if (VinID == AMBA_VIN_CHANNEL0) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_84, PinLevel);
    } else if (VinID == AMBA_VIN_CHANNEL1) {
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_21, PinLevel);
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
        RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_21, PinLevel);
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
                (void)AmbaGPIO_SetFuncGPI(GPIO_PIN_42);
                RetVal = AmbaVIN_SensorClkDisable(SensorClkChannel[VinID]);
            }
        } else {
            if (SensorClkChannel[VinID] == AMBA_VIN_SENSOR_CLOCK0) {
                RetVal = AmbaVIN_SensorClkEnable(SensorClkChannel[VinID], Frequency);
            } else {
                RetVal = AmbaVIN_SensorClkEnable(SensorClkChannel[VinID], Frequency);
                if (RetVal == VIN_ERR_NONE) {
                    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_42_ENET_ALT_REF_CLK1);
                }
            }
        }
        (void)AmbaKAL_TaskSleep(2);
    }

    return RetVal;
}
