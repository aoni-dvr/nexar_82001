/**
 *  @file AmbaIOUTDiag_CmdVin.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Diagnostic functions for video inputs
 *
 */

#include "AmbaTypes.h"
#include "AmbaWrap.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"

#include "AmbaVIN.h"
#include "AmbaSYS.h"
#include "AmbaGPIO.h"
#ifdef CONFIG_SVC_APPS_IOUT
#include "AmbaIOUTDiag_Priv.h"
#include "AmbaSensor.h"
#include "AmbaPrint.h"
#include "AmbaINT.h"
#include "AmbaKAL.h"
#include "AmbaSensor_IMX290_MIPI.h"
#include "AmbaSensor_IMX383.h"
#include "AmbaSensor_IMX272.h"
#include "AmbaSensor_IMX677.h"
#include "AmbaSensor_IMX490.h"
#include "AmbaSensor_IMX317.h"
#include "AmbaSensor_IMX334.h"
#include "AmbaSensor_IMX586.h"
#include "AmbaSensor_MAX9295_96712_IMX390.h"
#include "AmbaSensor_MAX9295_96712_IMX390_IOUT.h"
#include "AmbaSensor_MAX9295_9296_IMX390.h"
#include "AmbaSensor_MAX9295_9296_IMX390_REMAP.h"
#include "AmbaYuv.h"
#include "AmbaYuv_LT6911.h"
#include "AmbaSensor_IMX577.h"

//#define VC_1ch

static UINT32 IrqCount[AMBA_NUM_VIN_CHANNEL] = {0};
#endif

#define IODIAG_VIN_VSDLY_VS0               0U
#define IODIAG_VIN_VSDLY_VS1               1U
#define IODIAG_VIN_VSDLY_VS2               2U
#define IODIAG_VIN_VSDLY_VS3               3U
#define IODIAG_VIN_VSDLY_HS                4U    /* Hsync0 and Hsync1 share the same pin */
#define IODIAG_VIN_VSDLY_OUTPUT_NUM        5U

static void IoDiag_PrintFormattedInt(const char *pFmtString, UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[64];
    UINT32 ArgUINT32[2];

    ArgUINT32[0] = Value;
    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgUINT32);
    PrintFunc(StrBuf);
}

#ifdef CONFIG_SVC_APPS_IOUT
static void IRQ_Vin0(UINT32 args, UINT32 UserArg)
{
    (void) args;
    (void) UserArg;
    IrqCount[0]++;
}

static void IRQ_Vin1(UINT32 args, UINT32 UserArg)
{
    (void) args;
    (void) UserArg;
    IrqCount[1]++;
}

static void IRQ_Vin2(UINT32 args, UINT32 UserArg)
{
    (void) args;
    (void) UserArg;
    IrqCount[2]++;
}

static void IRQ_Vin3(UINT32 args, UINT32 UserArg)
{
    (void) args;
    (void) UserArg;
    IrqCount[3]++;
}

static void IRQ_Vin4(UINT32 args, UINT32 UserArg)
{
    (void) args;
    (void) UserArg;
    IrqCount[4]++;
}

static void IRQ_Vin5(UINT32 args, UINT32 UserArg)
{
    (void) args;
    (void) UserArg;
    IrqCount[5]++;
}

static void IRQ_Vin6(UINT32 args, UINT32 UserArg)
{
    (void) args;
    (void) UserArg;
    IrqCount[6]++;
}

static void IRQ_Vin7(UINT32 args, UINT32 UserArg)
{
    (void) args;
    (void) UserArg;
    IrqCount[7]++;
}

static void IRQ_Vin8(UINT32 args, UINT32 UserArg)
{
    (void) args;
    (void) UserArg;
    IrqCount[8]++;
}

static void IRQ_Vin9(UINT32 args, UINT32 UserArg)
{
    (void) args;
    (void) UserArg;
    IrqCount[9]++;
}

static void IRQ_Vin10(UINT32 args, UINT32 UserArg)
{
    (void) args;
    (void) UserArg;
    IrqCount[10]++;
}

static void IRQ_Vin11(UINT32 args, UINT32 UserArg)
{
    (void) args;
    (void) UserArg;
    IrqCount[11]++;
}

static void IRQ_Vin12(UINT32 args, UINT32 UserArg)
{
    (void) args;
    (void) UserArg;
    IrqCount[12]++;
}

static void IRQ_Vin13(UINT32 args, UINT32 UserArg)
{
    (void) args;
    (void) UserArg;
    IrqCount[13]++;
}


#define IODIAG_COMM_INTERFACE_I2C    0U
#define IODIAG_COMM_INTERFACE_SPI    1U

static void IoDiag_VinSetCommInterface(UINT32 CommInterface, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (CommInterface == IODIAG_COMM_INTERFACE_I2C) {
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_8_I2C3_CLK);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_9_I2C3_DATA);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_6_I2C4_CLK);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_7_I2C4_DATA);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_114_I2C0_CLK);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_115_I2C0_DATA);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_3_I2C1_CLK);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_4_I2C1_DATA);
    } else if (CommInterface == IODIAG_COMM_INTERFACE_SPI) {
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_6_I2C4_CLK);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_7_SPI2_MOSI);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_8_SPI2_MISO);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_9_SPI2_SS0);
#if defined(CONFIG_BSP_CV5BUB_OPTION_A_V100)
        /* Cnfigure GPIO92~95 as SSI4 function (GPIO96~99 are reserved for SD card monitor task) */
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_92_SPI4_SCLK);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_93_SPI4_MOSI);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_94_SPI4_MISO);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_95_SPI4_SS0);
#else
        /* Configure GPIO92~95 as SSI4 function and GPIO96~99 as SSI5 function */
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_98_SPI5_SCLK);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_96_SPI5_MOSI);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_99_SPI5_MISO);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_97_SPI5_SS0);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_92_SPI4_SCLK);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_93_SPI4_MOSI);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_94_SPI4_MISO);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_95_SPI4_SS0);
#endif
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_1_SPI0_SCLK);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_2_SPI0_MOSI);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_3_SPI0_MISO);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_4_SPI0_SS0);
    } else {
        IoDiag_PrintFormattedInt("Unknown communication interface:%d\n", CommInterface, PrintFunc);
    }
}
#define IODIAG_INTERFACE_MIPI           0U
#define IODIAG_INTERFACE_SLVS           1U
#define IODIAG_INTERFACE_LVCMOS         2U
#define IODIAG_INTERFACE_MIPI_PIP       3U
#define IODIAG_INTERFACE_MIPI_8LANE     4U
#define IODIAG_INTERFACE_MIPI_VC_HDR    5U
#define IODIAG_INTERFACE_MIPI_4K        6U
#define IODIAG_INTERFACE_MIPI_CPHY      7U
#define IODIAG_INTERFACE_SLVSEC         8U
#define IODIAG_INTERFACE_SLVSEC_2LINK   9U
#define IODIAG_INTERFACE_SLVSEC_2P3G    10U
#define IODIAG_INTERFACE_SLVSEC_4P6G    11U
#define IODIAG_NUM_INTERFACE            12U

/*   @VinID     Vin channel to be tested,
*    @Interface     0U: MIPI,
*                   1U: SLVS,
*                   2U: LVCMOS
*/
static UINT32 IoDiag_CheckVinStatus(UINT32 VinID, UINT32 Interface, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = VIN_ERR_NONE;
    IODIAG_VIN_STATUS_s VinStatus;

    IoDiag_PrintFormattedInt("Vin %d, ", VinID,  PrintFunc);
    IoDiag_PrintFormattedInt("Interface %d Check--------\n", Interface, PrintFunc);

    (void) AmbaDiag_VinGetStatus(VinID, &VinStatus);

    //general case
    if (VinStatus.GotActSof != 1U) {
        RetVal = 1U;
        PrintFunc("GotActSof NG\n");
    } else {
        //
    }
    if (VinStatus.GotWinSof != 1U) {
        RetVal = 1U;
        PrintFunc("GotWinSof NG\n");
    } else {
        //
    }
    if (VinStatus.GotWinEof != 1U) {
        RetVal = 1U;
        PrintFunc("GotWinEof NG\n");
    } else {
        //
    }
    if (VinStatus.GotVsync != 1U) {
        RetVal = 1U;
        PrintFunc("GotVsync NG\n");
    } else {
        //
    }
    if(VinStatus.AfifoOverflow != 0U) {
        RetVal = 1U;
        PrintFunc("AfifoOverflow NG\n");
    } else {
        //
    }
    if(VinStatus.SfifoOverflow != 0U) {
        RetVal = 1U;
        PrintFunc("SfifoOverFlow NG\n");
    } else {
        //
    }
    if(VinStatus.ShortLine != 0U) {
        RetVal = 1U;
        PrintFunc("ShortLine NG\n");
    } else {
        //
    }
    if(VinStatus.ShortFrame != 0U) {
        RetVal = 1U;
        PrintFunc("ShortFrame NG\n");
    } else {
        //
    }
    if(VinStatus.WatchdogTimeout != 0U) {
        RetVal = 1U;
        PrintFunc("WatchdogTimeout NG\n");
    } else {
        //
    }

    //mipi
    if((Interface == IODIAG_INTERFACE_MIPI) || (Interface == IODIAG_INTERFACE_MIPI_CPHY) || (Interface == IODIAG_INTERFACE_MIPI_VC_HDR)) {

        if(VinStatus.EccError2Bit != 0U) {
            RetVal = 1U;
            PrintFunc("EccError2Bit NG\n");
        } else {
            //
        }
        if(VinStatus.EccError1Bit != 0U) {
            RetVal = 1U;
            PrintFunc("EccError1Bit NG\n");
        } else {
            //
        }

        if(Interface != IODIAG_INTERFACE_MIPI_CPHY) {
            if(VinStatus.CrcError!= 0U) {   /* the bit is invalid for cphy due to chip bug */
                RetVal = 1U;
                PrintFunc("CrcError NG\n");
            } else {
                //
            }
        } else {
            if(VinStatus.CrcPhError!= 0U) {
                RetVal = 1U;
                PrintFunc("CrcPhError NG\n");
            } else {
                //
            }
        }
        if(VinStatus.FrameSyncError!= 0U) {
            RetVal = 1U;
            PrintFunc("FrameSyncError NG\n");
        } else {
            //
        }
        if(VinStatus.SofPacketReceived!= 1U) {
            RetVal = 1U;
            PrintFunc("SofPacketReceived NG\n");
        } else {
            //
        }
        if(VinStatus.EofPacketReceived!= 1U) {
            RetVal = 1U;
            PrintFunc("EofPacketReceived NG\n");
        } else {
            //
        }
        if(VinStatus.SerialDeskewError!= 0U) {
            RetVal = 1U;
            PrintFunc("SerialDeskewError NG\n");
        } else {
            //
        }
    } else if(Interface == IODIAG_INTERFACE_SLVS) {

        if(VinStatus.SerialDeskewError!= 0U) {
            RetVal = 1U;
            PrintFunc("SerialDeskewError NG\n");
        } else {
            //
        }
        if(VinStatus.Uncorrectable656Error!= 0U) {
            RetVal = 1U;
            PrintFunc("Uncorrectable656Error NG\n");
        } else {
            //
        }
        if(VinStatus.PartialSyncDetected!= 0U) {
            RetVal = 1U;
            PrintFunc("PartialSyncDetected NG\n");
        } else {
            //
        }
        if(VinStatus.UnknownSyncCode!= 0U) {
            RetVal = 1U;
            PrintFunc("UnknownSyncCode NG\n");
        } else {
            //
        }
    } else if((Interface == IODIAG_INTERFACE_SLVSEC) || (Interface == IODIAG_INTERFACE_SLVSEC_2LINK) ||
              (Interface == IODIAG_INTERFACE_SLVSEC_2P3G) || (Interface == IODIAG_INTERFACE_SLVSEC_4P6G)) {
        if(VinStatus.SlvsecCrcError!= 0U) {
            RetVal = 1U;
            PrintFunc("SlvsecCrcError NG\n");
        } else {
            //
        }
        if(VinStatus.SlvsecLineBoundaryErr!= 0U) {
            RetVal = 1U;
            PrintFunc("SlvsecLineBoundaryErr NG\n");
        } else {
            //
        }
        if(VinStatus.SlvsecDelayedLineEndErr!= 0U) {
            RetVal = 1U;
            PrintFunc("SlvsecDelayedLineEndErr NG\n");
        } else {
            //
        }
        if(VinStatus.SlvsecTruncatedPacketErr!= 0U) {
            RetVal = 1U;
            PrintFunc("SlvsecTruncatedPacketErr NG\n");
        } else {
            //
        }
        if(VinStatus.SlvsecIllegalStandbyErr!= 0U) {
            RetVal = 1U;
            PrintFunc("SlvsecIllegalStandbyErr NG\n");
        } else {
            //
        }
        PrintFunc("Slvsec TBD, will check status later\n");
        IoDiag_PrintFormattedInt("SlvsecL0EccCorrect 0x%x\n", VinStatus.SlvsecL0EccCorrect, PrintFunc);
        IoDiag_PrintFormattedInt("SlvsecL0EccCorreactable 0x%x\n", VinStatus.SlvsecL0EccCorreactable, PrintFunc);
        IoDiag_PrintFormattedInt("SlvsecL0EccUncorrectable 0x%x\n", VinStatus.SlvsecL0EccUncorrectable, PrintFunc);
        if (Interface == IODIAG_INTERFACE_SLVSEC_2LINK) {
            IoDiag_PrintFormattedInt("SlvsecL1EccCorrect 0x%x\n", VinStatus.SlvsecL1EccCorrect, PrintFunc);
            IoDiag_PrintFormattedInt("SlvsecL1EccCorreactable 0x%x\n", VinStatus.SlvsecL1EccCorreactable, PrintFunc);
            IoDiag_PrintFormattedInt("SlvsecL1EccUncorrectable 0x%x\n", VinStatus.SlvsecL1EccUncorrectable, PrintFunc);
        }
    } else {
        PrintFunc("Please check Interface value. 0U:MIPI,1U:SLVS,3:MIPI_PIP,4:MIPI_8lane,7:MIPI_CPHY,8:SLVSEC,9:2LinkSLVSEC,10:2P3G_SLVSEC,11:4P6G_SLVSEC\n");
    }

    return RetVal;
}

/*
*  Return     :: 0U: OK, 1U: NG
*/
static UINT32 IoDiag_CheckFrameRate(UINT32 VinID, const AMBA_VIN_FRAME_RATE_s *pTargetFrmRate, UINT32 *pActualFrmRate, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;
    UINT32 IrqCnt;
    UINT32 IrqTol = 1U; /* tolerance: 1 */
    DOUBLE TargetFrmRate;
    UINT32 TargetIrqCnt;
    UINT32 Cnt = 0U;

    (void)AmbaWrap_floor((((DOUBLE)pTargetFrmRate->TimeScale / (DOUBLE)pTargetFrmRate->NumUnitsInTick) + 0.5), &TargetFrmRate);
    TargetIrqCnt = (UINT32) TargetFrmRate;

    /* Wait 8 seconds for YUV Irq start */
    while (Cnt < 8000U) {
        if(IrqCount[VinID] != 0U) {
            break;
        } else {
            if ((Cnt % 1000U) == 0U) {
                IoDiag_PrintFormattedInt("Waiting for vin irq ... %ds\n", (8000U - Cnt) / 1000U, PrintFunc);
            }
            (void)AmbaKAL_TaskSleep(1);
            Cnt++;
        }
    }

    IrqCnt = IrqCount[VinID];
    (void)AmbaKAL_TaskSleep(3000U);
    IrqCnt = IrqCount[VinID] - IrqCnt;
    IrqCnt = (UINT32)(IrqCnt / 3U);

    *pActualFrmRate = IrqCnt;
    if ((IrqCnt > (TargetIrqCnt + IrqTol)) || (IrqCnt < (TargetIrqCnt - IrqTol))) {
        RetVal = 1U;
        IoDiag_PrintFormattedInt("VIN%d ", VinID, PrintFunc);
        IoDiag_PrintFormattedInt("%d frames during 1s ", IrqCnt, PrintFunc);
        IoDiag_PrintFormattedInt("NG, should be %d frames during 1s\n", TargetIrqCnt, PrintFunc);
    } else {
        RetVal = 0U;
        IoDiag_PrintFormattedInt("VIN%d ", VinID, PrintFunc);
        IoDiag_PrintFormattedInt("%d frames during 1s\n", IrqCnt, PrintFunc);
    }

    return RetVal;
}

/* @Interface 0U: MIPI,
*             1U: SLVS,
*             2U: LVCMOS,
*             3U: PIP MIPI,
*             4U: 8-lane MIPI,
*  @VinID     Vin channel to be tested
*  @ModeID    0xFFFFFFFF: default configuration
*/
typedef struct {
    AMBA_SENSOR_OBJ_s   *pAmbaSensorObj;
    UINT32              ModeID;
    UINT32              CommInterface;
} IODIAG_SENSOR_CONFIG_s;
static void IoDiag_VinVerifyInterface(UINT32 Interface, UINT32 VinID, UINT32 ModeID, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_SENSOR_CHANNEL_s Chan = {0};
    AMBA_SENSOR_CONFIG_s Config = {0};
    AMBA_SENSOR_MODE_INFO_s ModeInfo;
    UINT32 ActualFrmRate;
    UINT32 RetVal = 0U;
    UINT32 i;

    static IODIAG_SENSOR_CONFIG_s IoDiagDefaultSensorConfig[IODIAG_NUM_INTERFACE] = {
        [IODIAG_INTERFACE_MIPI] = {
            .pAmbaSensorObj = &AmbaSensor_IMX290_MIPIObj,
            .ModeID         = IMX290_MIPI_1920_1080_60P,
            .CommInterface  = IODIAG_COMM_INTERFACE_I2C,
        },
        [IODIAG_INTERFACE_SLVS] = {
            .pAmbaSensorObj = &AmbaSensor_IMX317Obj,
            .ModeID         = AMBA_SENSOR_IMX317_3840_2160_30P,
            .CommInterface  = IODIAG_COMM_INTERFACE_SPI,
        },
#if 0 //LVCMOS is not supported
        [IODIAG_INTERFACE_LVCMOS] = {
            .pAmbaSensorObj = &AmbaSensor_AR0238_PARALLELObj,
            .ModeID         = AMBA_SENSOR_AR0238_PARALLEL_1920_1080_30P,
            .CommInterface  = IODIAG_COMM_INTERFACE_I2C,
        },
#endif
        [IODIAG_INTERFACE_MIPI_PIP] = {
            .pAmbaSensorObj = &AmbaSensor_IMX490Obj,
            .ModeID         = IMX490_2880_1860_A30P_HDR,
            .CommInterface  = IODIAG_COMM_INTERFACE_I2C,
        },
        [IODIAG_INTERFACE_MIPI_8LANE] = {
            .pAmbaSensorObj = &AmbaSensor_IMX334Obj,
            .ModeID         = IMX334_3840_2160_30P_1,
            .CommInterface  = IODIAG_COMM_INTERFACE_I2C,
        },
        [IODIAG_INTERFACE_MIPI_VC_HDR] = {
            .pAmbaSensorObj = &AmbaSensor_IMX334Obj,
            .ModeID         = IMX334_3840_2160_30P_HDR_0,
            .CommInterface  = IODIAG_COMM_INTERFACE_I2C,
        },
        [IODIAG_INTERFACE_MIPI_CPHY] = {
            .pAmbaSensorObj = &AmbaSensor_IMX586Obj,
            .ModeID         = IMX586_7680_4320_30P,
            .CommInterface  = IODIAG_COMM_INTERFACE_I2C,
        },
        [IODIAG_INTERFACE_MIPI_4K] = {
            .pAmbaSensorObj = &AmbaSensor_IMX577Obj,
            .ModeID         = IMX577_3840_2160_30P,
            .CommInterface  = IODIAG_COMM_INTERFACE_I2C,
        },
        [IODIAG_INTERFACE_SLVSEC] = {
            .pAmbaSensorObj = &AmbaSensor_IMX383Obj,
            .ModeID         = IMX383_5472_3648_60P,
            .CommInterface  = IODIAG_COMM_INTERFACE_SPI,
        },
        [IODIAG_INTERFACE_SLVSEC_2LINK] = {
            .pAmbaSensorObj = &AmbaSensor_IMX272Obj,
            .ModeID         = IMX272_5280_3044_30P,
            .CommInterface  = IODIAG_COMM_INTERFACE_SPI,
        },
        [IODIAG_INTERFACE_SLVSEC_2P3G] = {
            .pAmbaSensorObj = &AmbaSensor_IMX677Obj,
            .ModeID         = IMX677_5599_4223_30P,
            .CommInterface  = IODIAG_COMM_INTERFACE_SPI,
        },
        [IODIAG_INTERFACE_SLVSEC_4P6G] = {
            .pAmbaSensorObj = &AmbaSensor_IMX677Obj,
            .ModeID         = IMX677_5599_4223_30P_1,
            .CommInterface  = IODIAG_COMM_INTERFACE_SPI,
        },
    };

    Chan.VinID = VinID;
    Chan.SensorID = 0U;

    if (ModeID == 0xFFFFFFFFU) {
        Config.ModeID = IoDiagDefaultSensorConfig[Interface].ModeID;
    } else {
        Config.ModeID = ModeID;
    }

    IoDiag_VinSetCommInterface(IoDiagDefaultSensorConfig[Interface].CommInterface, PrintFunc);

    AmbaSensor_Hook(&Chan, IoDiagDefaultSensorConfig[Interface].pAmbaSensorObj);

    (void)AmbaSensor_Init(&Chan);
    (void)AmbaSensor_Enable(&Chan);
    (void)AmbaSensor_Config(&Chan, &Config);
    (void)AmbaSensor_GetModeInfo(&Chan, &Config, &ModeInfo);

    if (Interface == IODIAG_INTERFACE_MIPI_VC_HDR) {
        for (i = 0U; i < ModeInfo.HdrInfo.ActiveChannels; i++) {
            (void)AmbaDiag_VinConfigThrDbgPort(VinID + i, ModeInfo.OutputInfo.RecordingPixels.Width, ModeInfo.OutputInfo.RecordingPixels.Height, PrintFunc);
        }
    } else {
        (void)AmbaDiag_VinConfigThrDbgPort(VinID, ModeInfo.OutputInfo.RecordingPixels.Width, ModeInfo.OutputInfo.RecordingPixels.Height, PrintFunc);
    }

    (void)AmbaSensor_ConfigPost(&Chan);

    if (Interface == IODIAG_INTERFACE_MIPI_VC_HDR) {
        RetVal = 0U;

        for (i = 0U; i < ModeInfo.HdrInfo.ActiveChannels; i++) {
            RetVal |= IoDiag_CheckFrameRate(VinID + i, &ModeInfo.FrameRate, &ActualFrmRate, PrintFunc);
            RetVal |= IoDiag_CheckVinStatus(VinID + i, Interface, PrintFunc);

            if (RetVal == 0U) {
                IoDiag_PrintFormattedInt("[PASS] Vin %d \n", VinID + i, PrintFunc);
            } else {
                IoDiag_PrintFormattedInt("[NG] Vin %d \n", VinID + i, PrintFunc);
            }
        }
    } else {
        RetVal |= IoDiag_CheckFrameRate(VinID, &ModeInfo.FrameRate, &ActualFrmRate, PrintFunc);
        if ((Interface > IODIAG_INTERFACE_LVCMOS) && (Interface < IODIAG_INTERFACE_MIPI_CPHY)) {
            RetVal |= IoDiag_CheckVinStatus(VinID, IODIAG_INTERFACE_MIPI, PrintFunc);
        } else if ((Interface == IODIAG_INTERFACE_SLVSEC) || (Interface == IODIAG_INTERFACE_SLVSEC_2LINK) ||
                   (Interface == IODIAG_INTERFACE_SLVSEC_2P3G) || (Interface == IODIAG_INTERFACE_SLVSEC_4P6G)) {
            PrintFunc("\n### check status ###\n");
            (void) IoDiag_CheckVinStatus(VinID, Interface, PrintFunc);
            PrintFunc("\n### reset status ###\n");
            (void) AmbaDiag_VinResetStatus(VinID, PrintFunc);
            (void) AmbaKAL_TaskSleep(1000U);
            PrintFunc("\n### check status (1 sec after reset status) ###\n");
            RetVal |= IoDiag_CheckVinStatus(VinID, Interface, PrintFunc);
        } else {
            RetVal |= IoDiag_CheckVinStatus(VinID, Interface, PrintFunc);
        }

        if (RetVal == 0U) {
            IoDiag_PrintFormattedInt("[PASS] Vin %d \n", VinID, PrintFunc);
        } else {
            IoDiag_PrintFormattedInt("[NG] Vin %d \n", VinID, PrintFunc);
        }
    }
}

#define DPHY_VC     0U
#define DPHY_VCX    1U
#define CPHY_VC     2U
#define CPHY_VCX    3U

static void IoDiag_VinVerifyVC(AMBA_SHELL_PRINT_f PrintFunc, UINT32 VinID, UINT32 ModeID, UINT8 Type)
{
    AMBA_SENSOR_CHANNEL_s Chan = {0U};
    AMBA_SENSOR_CONFIG_s Mode = {0U};
    static AMBA_SENSOR_MODE_INFO_s ModeInfo = {0U};

    UINT32 ActualFrmRate;
    UINT32 RetVal = 0U;

    Chan.VinID = VinID;
    if ((VinID == 0U) || (VinID == 4U)) {
#ifndef VC_1ch
        Chan.SensorID = 0x11110;
#else
        Chan.SensorID = 0x10;
#endif
    } else if ((VinID == 8U) || (VinID == 11U)) {
#ifndef VC_1ch
        Chan.SensorID = 0x1110;
#else
        Chan.SensorID = 0x10;
#endif
    } else {
        IoDiag_PrintFormattedInt("[NG] VinID %d is not supported, return.", VinID, PrintFunc);
        return;
    }

    if (ModeID == 0xFFFFFFFFU) {
        Mode.ModeID = 6U;
        Mode.ModeID_1 = 6U;
        Mode.ModeID_2 = 6U;
        Mode.ModeID_3 = 6U;
    } else {
        Mode.ModeID = ModeID;
        Mode.ModeID_1 = ModeID;
        Mode.ModeID_2 = ModeID;
        Mode.ModeID_3 = ModeID;
    }


    IoDiag_VinSetCommInterface(IODIAG_COMM_INTERFACE_I2C, PrintFunc);

    if (Type == DPHY_VC) {
        AmbaSensor_Hook(&Chan, &AmbaSensor_MX01_IMX390_VCObj);
    } else if (Type == DPHY_VCX) {
        AmbaSensor_Hook(&Chan, &AmbaSensor_MX01_IMX390_VCXObj);
    } else if (Type == CPHY_VC) {
        AmbaSensor_Hook(&Chan, &AmbaSensor_MX01_IMX390_VCCObj);
    } else {
        AmbaSensor_Hook(&Chan, &AmbaSensor_MX01_IMX390_VCXCObj);
    }
    (void) AmbaSensor_Init(&Chan);
    (void) AmbaSensor_Enable(&Chan);
    (void) AmbaSensor_Config(&Chan, &Mode);
    (void) AmbaSensor_GetModeInfo(&Chan, &Mode, &ModeInfo);
    (void) AmbaDiag_VinConfigThrDbgPort(VinID, 1920U, 1080U, PrintFunc);
#ifndef VC_1ch
    (void) AmbaDiag_VinConfigThrDbgPort(VinID+1U, 1920U, 1080U, PrintFunc);
    (void) AmbaDiag_VinConfigThrDbgPort(VinID+2U, 1920U, 1080U, PrintFunc);
    if ((VinID == 0U) || (VinID == 4U)) {
        (void) AmbaDiag_VinConfigThrDbgPort(VinID+3U, 1920U, 1080U, PrintFunc);
    }
#endif
    (void) AmbaSensor_ConfigPost(&Chan);


    RetVal |= IoDiag_CheckFrameRate(VinID, &ModeInfo.FrameRate, &ActualFrmRate, PrintFunc);
#ifndef VC_1ch
    RetVal |= IoDiag_CheckFrameRate(VinID+1U, &ModeInfo.FrameRate, &ActualFrmRate, PrintFunc);
    RetVal |= IoDiag_CheckFrameRate(VinID+2U, &ModeInfo.FrameRate, &ActualFrmRate, PrintFunc);
    if ((VinID == 0U) || (VinID == 4U)) {
        RetVal |= IoDiag_CheckFrameRate(VinID+3U, &ModeInfo.FrameRate, &ActualFrmRate, PrintFunc);
    }
#endif
    if ((Type == DPHY_VC) || (Type == DPHY_VCX)) {
        PrintFunc("DPhy\n");
        RetVal |= IoDiag_CheckVinStatus(VinID, IODIAG_INTERFACE_MIPI, PrintFunc);
#ifndef VC_1ch
        RetVal |= IoDiag_CheckVinStatus(VinID+1U, IODIAG_INTERFACE_MIPI, PrintFunc);
        RetVal |= IoDiag_CheckVinStatus(VinID+2U, IODIAG_INTERFACE_MIPI, PrintFunc);
        if ((VinID == 0U) || (VinID == 4U)) {
            RetVal |= IoDiag_CheckVinStatus(VinID+3U, IODIAG_INTERFACE_MIPI, PrintFunc);
        }
#endif
    } else {
        PrintFunc("CPhy\n");
        RetVal |= IoDiag_CheckVinStatus(VinID, IODIAG_INTERFACE_MIPI_CPHY, PrintFunc);
#ifndef VC_1ch
        RetVal |= IoDiag_CheckVinStatus(VinID+1U, IODIAG_INTERFACE_MIPI_CPHY, PrintFunc);
        RetVal |= IoDiag_CheckVinStatus(VinID+2U, IODIAG_INTERFACE_MIPI_CPHY, PrintFunc);
        if ((VinID == 0U) || (VinID == 4U)) {
            RetVal |= IoDiag_CheckVinStatus(VinID+3U, IODIAG_INTERFACE_MIPI_CPHY, PrintFunc);
        }
#endif
    }
    if (RetVal == 1U) {
        IoDiag_PrintFormattedInt("VinID %d", VinID, PrintFunc);
#ifndef VC_1ch
        IoDiag_PrintFormattedInt("/%d", VinID+1U, PrintFunc);
        if ((VinID == 0U) || (VinID == 4U)) {
            IoDiag_PrintFormattedInt("/%d", VinID+2U, PrintFunc);
            IoDiag_PrintFormattedInt("/%d [NG - VC]\n", VinID+3U, PrintFunc);
        } else {
            IoDiag_PrintFormattedInt("/%d [NG - VC]\n", VinID+2U, PrintFunc);
        }
#else
        PrintFunc("[NG - VC]\n");
#endif
    } else if (RetVal == 0U) {
        IoDiag_PrintFormattedInt("VinID %d", VinID, PrintFunc);
#ifndef VC_1ch
        IoDiag_PrintFormattedInt("/%d", VinID+1U, PrintFunc);
        if ((VinID == 0U) || (VinID == 4U)) {
            IoDiag_PrintFormattedInt("/%d", VinID+2U, PrintFunc);
            IoDiag_PrintFormattedInt("/%d [OK - VC]\n", VinID+3U, PrintFunc);
        } else {
            IoDiag_PrintFormattedInt("/%d [OK - VC]\n", VinID+2U, PrintFunc);
        }
#else
        PrintFunc("[OK - VC]\n");
#endif
    } else {
        IoDiag_PrintFormattedInt("return value NG %d \n", RetVal, PrintFunc);
    }

}

static void IoDiag_VinVerifyRemap(AMBA_SHELL_PRINT_f PrintFunc, UINT32 VinID)
{
    AMBA_SENSOR_CHANNEL_s Chan = {0U};
    AMBA_SENSOR_CONFIG_s Mode = {0U};
    static AMBA_SENSOR_MODE_INFO_s ModeInfo = {0U};

    UINT32 ActualFrmRate;
    UINT32 RetVal = 0U;

    Chan.VinID = VinID;
    Chan.SensorID = 0x10;
    Mode.ModeID = MX00_IMX390_1920_1080_30P_HDR1; //MX00_IMX390_1920_1080_30P_HDR1: 1-2lane, MX00_IMX390_1920_1080_60P_HDR1: 4lane

    IoDiag_VinSetCommInterface(IODIAG_COMM_INTERFACE_I2C, PrintFunc);

    AmbaSensor_Hook(&Chan, &AmbaSensor_MX00_IMX390_REMAPObj);
    (void) AmbaSensor_Init(&Chan);
    (void) AmbaSensor_Enable(&Chan);
    (void) AmbaSensor_Config(&Chan, &Mode);
    (void) AmbaSensor_GetModeInfo(&Chan, &Mode, &ModeInfo);
    (void) AmbaDiag_VinConfigThrDbgPort(VinID, 1920U, 1080U, PrintFunc);
    (void) AmbaSensor_ConfigPost(&Chan);


    RetVal |= IoDiag_CheckFrameRate(VinID, &ModeInfo.FrameRate, &ActualFrmRate, PrintFunc);
    RetVal |= IoDiag_CheckVinStatus(VinID, IODIAG_INTERFACE_MIPI, PrintFunc);

    if (RetVal == 1U) {
        PrintFunc("[NG - REAMP] \n");
    } else if (RetVal == 0U) {
        PrintFunc("[OK - REAMP] \n");
    } else {
        IoDiag_PrintFormattedInt("return value NG %d \n", RetVal, PrintFunc);
    }
}

static void IoDiag_VinVerifyYuv(AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_YUV_CHANNEL_s YuvChan = {0U};
    AMBA_YUV_CONFIG_s YuvCfg = {0U};
    static AMBA_YUV_MODE_INFO_s YuvMode = {0U};

    UINT32 ActualFrmRate;
    UINT32 RetVal = 0U;
    DOUBLE FrmTime;

    YuvChan.VinID = 8U;
    YuvCfg.ModeID = LT6911_1920_1080_60P;

    IoDiag_VinSetCommInterface(IODIAG_COMM_INTERFACE_I2C, PrintFunc);

    AmbaYuv_Hook(&YuvChan, &AmbaYuv_LT6911Obj);
    (void) AmbaYuv_Init(&YuvChan);
    (void) AmbaYuv_Enable(&YuvChan);
    (void) AmbaYuv_Config(&YuvChan, &YuvCfg);
    (void) AmbaYuv_GetModeInfo(&YuvChan, &YuvCfg, &YuvMode);
    (void) AmbaDiag_VinConfigThrDbgPort(8U, 1920U, 1080U, PrintFunc);

    (void)AmbaWrap_floor((((DOUBLE)YuvMode.OutputInfo.FrameRate.NumUnitsInTick * 1000 / (DOUBLE)YuvMode.OutputInfo.FrameRate.TimeScale) + 0.5), &FrmTime);
    RetVal |= IoDiag_CheckFrameRate(8U, &YuvMode.OutputInfo.FrameRate, &ActualFrmRate, PrintFunc);
    //reset Vin reg since LT6911 not support post config maybe hit streaming before vin cmd issue
    (void) AmbaDiag_VinResetStatus(8U, PrintFunc);
    (void) AmbaKAL_TaskSleep(FrmTime*2);
    RetVal |= IoDiag_CheckVinStatus(8U, IODIAG_INTERFACE_MIPI, PrintFunc);

    if (RetVal == 1U) {
        PrintFunc("[NG - YUV] \n");
    } else if (RetVal == 0U) {
        PrintFunc("[OK - YUV] \n");
    } else {
        IoDiag_PrintFormattedInt("return value NG %d \n", RetVal, PrintFunc);
    }
}

#endif
static void IoDiag_VinMsyncEnable(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RefClk, HSyncPeriod, HSyncPulseWidth, HSyncPolarity, VSyncPeriod, VSyncPulseWidth, VSyncPolarity, HSyncDelayCycles, VSyncDelayCycles, ToggleHsyncInVblank;
    AMBA_VIN_MASTER_SYNC_CONFIG_s MasterSyncCfg = {0};
    UINT32 MSyncID = 0xffffffffU;

    if (ArgCount != 13U) {
        PrintFunc("[msync_enable] Incorrect argument count!\n");
    } else {
        (void)AmbaUtility_StringToUInt32(pArgVector[2], &MSyncID);

        if (MSyncID >= AMBA_NUM_VIN_MSYNC) {
            PrintFunc("[msync_enable] Incorrect MSyncID!\n");
        } else {
            (void)AmbaUtility_StringToUInt32(pArgVector[3], &RefClk);
            (void)AmbaUtility_StringToUInt32(pArgVector[4], &HSyncPeriod);
            (void)AmbaUtility_StringToUInt32(pArgVector[5], &HSyncPulseWidth);
            (void)AmbaUtility_StringToUInt32(pArgVector[6], &HSyncPolarity);
            (void)AmbaUtility_StringToUInt32(pArgVector[7], &VSyncPeriod);
            (void)AmbaUtility_StringToUInt32(pArgVector[8], &VSyncPulseWidth);
            (void)AmbaUtility_StringToUInt32(pArgVector[9], &VSyncPolarity);
            (void)AmbaUtility_StringToUInt32(pArgVector[10], &HSyncDelayCycles);
            (void)AmbaUtility_StringToUInt32(pArgVector[11], &VSyncDelayCycles);
            (void)AmbaUtility_StringToUInt32(pArgVector[12], &ToggleHsyncInVblank);

            MasterSyncCfg.RefClk               = RefClk;
            MasterSyncCfg.HSync.Period         = HSyncPeriod;
            MasterSyncCfg.HSync.PulseWidth     = HSyncPulseWidth;
            MasterSyncCfg.HSync.Polarity       = (UINT8)HSyncPolarity;
            MasterSyncCfg.VSync.Period         = VSyncPeriod;
            MasterSyncCfg.VSync.PulseWidth     = VSyncPulseWidth;
            MasterSyncCfg.VSync.Polarity       = (UINT8)VSyncPolarity;
            MasterSyncCfg.HSyncDelayCycles     = (UINT16)HSyncDelayCycles;
            MasterSyncCfg.VSyncDelayCycles     = (UINT16)VSyncDelayCycles;
            MasterSyncCfg.ToggleHsyncInVblank  = (UINT8)ToggleHsyncInVblank;

            (void) AmbaVIN_MasterSyncEnable(MSyncID, &MasterSyncCfg);
        }
    }
}

static void IoDiag_VinMsyncDisable(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 MSyncID = 0xffffffffU;

    if (ArgCount != 3U) {
        PrintFunc("[msync_disable] Incorrect argument count!\n");
    } else {
        (void)AmbaUtility_StringToUInt32(pArgVector[2], &MSyncID);

        if (MSyncID >= AMBA_NUM_VIN_MSYNC) {
            PrintFunc("[msync_disable] Incorrect MSyncID!\n");
        } else {
            (void) AmbaVIN_MasterSyncDisable(MSyncID);
        }
    }
}

static void IoDiag_VinSetLvdsTermination(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 VinChan = 0U;
    UINT32 Value = 0U;

    if (ArgCount != 4U) {
        PrintFunc("[lvds_term_set] Incorrect argument count!\n");
    } else {
        (void)AmbaUtility_StringToUInt32(pArgVector[1], &VinChan);
        (void)AmbaUtility_StringToUInt32(pArgVector[3], &Value);

        if ((Value > AMBA_VIN_TERMINATION_VALUE_MAX) ||
            (Value < AMBA_VIN_TERMINATION_VALUE_MIN)) {
            PrintFunc("[lvds_term_set] Incorrect Termination Value!\n");
        } else {
            (void)AmbaVIN_SetLvdsTermination(VinChan, Value);
        }
    }
}

static void IoDiag_VinSensorClkEnable(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 ClkID = 0xffffffffU, Frequency = 0U;

    if (ArgCount != 4U) {
        PrintFunc("[sensor_clk_enable] Incorrect argument count!\n");
    } else {
        (void)AmbaUtility_StringToUInt32(pArgVector[2], &ClkID);
        (void)AmbaUtility_StringToUInt32(pArgVector[3], &Frequency);

        if (ClkID >= AMBA_NUM_VIN_SENSOR_CLOCK) {
            PrintFunc("[sensor_clk_enable] Incorrect ClkID!\n");
        } else {
            (void)AmbaVIN_SensorClkEnable(ClkID, Frequency);
        }
    }
}

static void IoDiag_VinSensorClkDisable(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 ClkID = 0xffffffffU;

    if (ArgCount != 3U) {
        PrintFunc("[sensor_clk_disable] Incorrect argument count!\n");
    } else {
        (void)AmbaUtility_StringToUInt32(pArgVector[2], &ClkID);

        if (ClkID >= AMBA_NUM_VIN_SENSOR_CLOCK) {
            PrintFunc("[sensor_clk_disable] Incorrect ClkID!\n");
        } else {
            (void)AmbaVIN_SensorClkDisable(ClkID);
        }
    }
}

static void IoDiag_VinSensorClkSetDrvStrength(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Value = 0xffffffffU;

    if (ArgCount != 3U) {
        PrintFunc("[sensor_clk_drv_set] Incorrect argument count!\n");
    } else {
        (void)AmbaUtility_StringToUInt32(pArgVector[2], &Value);

        if (Value > AMBA_VIN_DRIVE_STRENGTH_12MA) {
            PrintFunc("[sensor_clk_drv_set] Incorrect Drive Strength Value!\n");
        } else {
            (void)AmbaVIN_SensorClkSetDrvStrength(Value);
        }
    }
}

static void IoDiag_VinVsdelayCtrlStatus(const AMBA_VIN_DELAYED_VSYNC_CONFIG_s *pVsDlyConfigCtrl, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i;

    PrintFunc("VsDlyConfigCtrl: \n");
    IoDiag_PrintFormattedInt("  DelayedHVsyncSource: %d\n", pVsDlyConfigCtrl->DelayedHVsyncSource, PrintFunc);
    for (i = 0U; i < AMBA_VIN_DELAYED_VSYNC_NUM; i++) {
        IoDiag_PrintFormattedInt("  VsDlyPeriod[%d]:", i, PrintFunc);
        IoDiag_PrintFormattedInt("      %d\n", pVsDlyConfigCtrl->VsDlyPeriod[i], PrintFunc);
    }

    PrintFunc("  Polarity: \n");
    IoDiag_PrintFormattedInt("    HsPolInput:        %d\n", pVsDlyConfigCtrl->Polarity.HsPolInput, PrintFunc);
    IoDiag_PrintFormattedInt("    VsPolInput:        %d\n", pVsDlyConfigCtrl->Polarity.VsPolInput, PrintFunc);
    IoDiag_PrintFormattedInt("    HsPolOutput:       %d\n", pVsDlyConfigCtrl->Polarity.HsPolOutput, PrintFunc);
    for (i = 0U; i < AMBA_VIN_DELAYED_VSYNC_NUM; i++) {
        IoDiag_PrintFormattedInt("  VsPolOutput[%d]:", i, PrintFunc);
        IoDiag_PrintFormattedInt("      %d\n", pVsDlyConfigCtrl->Polarity.VsPolOutput[i], PrintFunc);
    }

    PrintFunc("  FineAdjust: \n");
    IoDiag_PrintFormattedInt("    RefClk:            %d\n", pVsDlyConfigCtrl->FineAdjust.RefClk, PrintFunc);
    IoDiag_PrintFormattedInt("    HsDlyClk:          %d\n", pVsDlyConfigCtrl->FineAdjust.HsDlyClk, PrintFunc);
    for (i = 0U; i < AMBA_VIN_DELAYED_VSYNC_NUM; i++) {
        IoDiag_PrintFormattedInt("    VsDlyClk[%d]:", i, PrintFunc);
        IoDiag_PrintFormattedInt("       %d\n", pVsDlyConfigCtrl->FineAdjust.VsDlyClk[i], PrintFunc);
    }
    IoDiag_PrintFormattedInt("    HsPulseWidth:      %d\n", pVsDlyConfigCtrl->FineAdjust.HsPulseWidth, PrintFunc);
    for (i = 0U; i < AMBA_VIN_DELAYED_VSYNC_NUM; i++) {
        IoDiag_PrintFormattedInt("    VsPulseWidth[%d]:", i, PrintFunc);
        IoDiag_PrintFormattedInt("   %d\n", pVsDlyConfigCtrl->FineAdjust.VsPulseWidth[i], PrintFunc);
    }
}

static void IoDiag_VinVsdelayEnable(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;
    UINT32 OutputChan = 0U, MsyncSrc = 0U, DlyVPeriod = 0U;
    UINT32 PolVInput = 0U, PolVOutput = 0U, PolHInput = 0U, PolHOutput = 0U;
    UINT32 Refclk = 0U, DlyHClk = 0U, DlyVClk = 0U, HWidth = 0U, VWidth = 0U;

    static AMBA_VIN_DELAYED_VSYNC_FINE_ADJUST_s  VsDlyAdjCtrl     = {0};
    static AMBA_VIN_DELAYED_VSYNC_POLARITY_s     VsDlyPolCtrl     = {0};
    static AMBA_VIN_DELAYED_VSYNC_CONFIG_s       VsDlyConfigCtrl  = {0};

    (void) AmbaUtility_StringToUInt32(pArgVector[2], &MsyncSrc);
    (void) AmbaUtility_StringToUInt32(pArgVector[3], &OutputChan);
    if (OutputChan >= IODIAG_VIN_VSDLY_OUTPUT_NUM) {
        PrintFunc("[vsdelay] Incorrect output channel number!\n");
    } else {

        if (OutputChan == IODIAG_VIN_VSDLY_HS) {
            (void) AmbaUtility_StringToUInt32(pArgVector[4], &PolHInput);
            (void) AmbaUtility_StringToUInt32(pArgVector[5], &PolHOutput);
            (void) AmbaUtility_StringToUInt32(pArgVector[6], &Refclk);
            (void) AmbaUtility_StringToUInt32(pArgVector[7], &DlyHClk);
            (void) AmbaUtility_StringToUInt32(pArgVector[8], &HWidth);

            VsDlyConfigCtrl.DelayedHVsyncSource = MsyncSrc;
            VsDlyPolCtrl.HsPolInput = PolHInput;
            VsDlyPolCtrl.HsPolOutput = PolHOutput;
            (void)AmbaWrap_memcpy(&VsDlyConfigCtrl.Polarity, &VsDlyPolCtrl, sizeof(AMBA_VIN_DELAYED_VSYNC_POLARITY_s));

            if (Refclk != 0U) {
                VsDlyAdjCtrl.RefClk = Refclk;
                VsDlyAdjCtrl.HsDlyClk = DlyHClk;
                VsDlyAdjCtrl.HsPulseWidth = HWidth;
                (void)AmbaWrap_memcpy(&VsDlyConfigCtrl.FineAdjust, &VsDlyAdjCtrl, sizeof(AMBA_VIN_DELAYED_VSYNC_FINE_ADJUST_s));
            }

        } else {  /* OutputChan == IODIAG_VIN_VSDLY_VS0-3 */
            (void) AmbaUtility_StringToUInt32(pArgVector[4], &DlyVPeriod);
            (void) AmbaUtility_StringToUInt32(pArgVector[5], &PolVInput);
            (void) AmbaUtility_StringToUInt32(pArgVector[6], &PolVOutput);
            (void) AmbaUtility_StringToUInt32(pArgVector[7], &Refclk);
            (void) AmbaUtility_StringToUInt32(pArgVector[8], &DlyVClk);
            (void) AmbaUtility_StringToUInt32(pArgVector[9], &VWidth);

            VsDlyConfigCtrl.DelayedHVsyncSource = MsyncSrc;
            VsDlyConfigCtrl.VsDlyPeriod[OutputChan] = DlyVPeriod;
            VsDlyPolCtrl.VsPolInput = PolVInput;
            VsDlyPolCtrl.VsPolOutput[OutputChan] = PolVOutput;
            (void)AmbaWrap_memcpy(&VsDlyConfigCtrl.Polarity, &VsDlyPolCtrl, sizeof(AMBA_VIN_DELAYED_VSYNC_POLARITY_s));

            if (Refclk != 0U) {
                VsDlyAdjCtrl.RefClk = Refclk;
                VsDlyAdjCtrl.VsDlyClk[OutputChan] = DlyVClk;
                VsDlyAdjCtrl.VsPulseWidth[OutputChan] = VWidth;
                (void)AmbaWrap_memcpy(&VsDlyConfigCtrl.FineAdjust, &VsDlyAdjCtrl, sizeof(AMBA_VIN_DELAYED_VSYNC_FINE_ADJUST_s));
            }
        }
    }

    RetVal = AmbaVIN_DelayedVSyncEnable(&VsDlyConfigCtrl);

    if (RetVal != VIN_ERR_NONE) {
        PrintFunc("[vsdelay] Enable Failed!\n");
    } else {
        IoDiag_VinVsdelayCtrlStatus(&VsDlyConfigCtrl, PrintFunc);
        PrintFunc("[vsdelay] Enable Done!\n");
    }
}

static void IoDiag_VinVsdelayDisable(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;

    RetVal = AmbaVIN_DelayedVSyncDisable();

    if (RetVal != VIN_ERR_NONE) {
        PrintFunc("[vsdelay] Disable Failed!\n");
    } else {
        PrintFunc("[vsdelay] Disable Done!\n");
    }
}

static void IoDiag_VinCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    (void) pArgVector;
    PrintFunc("Usage: \n");

    PrintFunc(pArgVector[0]);
    PrintFunc(" <channel> info                      - check VIN status \n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <channel> reset                     - reset VIN status register \n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <channel> reconfig                  - reconfig VIN main \n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <channel> reconfig <Width> <Height> - reconfig VIN main and capture window \n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" msync_enable <MSyncID> <RefClk> <HSPeriod> <HSPulseWidth> <HSPol>"
              " <VSPeriod> <VSPulseWidth> <VSPol> <HSDlyCycles> <VSDlyCycles>"
              " <ToggleHsyncInVB> - config VIN master sync generation \n");
    PrintFunc(" Ex1. diag vin msync_enable 0 72000000 264 8 0 4550 8 0 0 0 1\n"
              "      (RefClk=72000000, "
              "HSPeriod=264, HSPulseWidth=8, HSPol=0 (Act Low), "
              "VSPeriod=4550, VSPulseWidth=8, VSPol=0 (Act Low), "
              "HSDlyCycles=0, VSDlyCycles=0, ToggleHsyncInVB=1)\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" msync_disable <MSyncID>\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <channel> lvds_term_set <Value>     - adjust the impendance of LVDS pad\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" sensor_clk_enable <ClkID> <Freq>    - Enable clock as the sensor clock source\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" sensor_clk_disable <ClkID>          - disable the clock\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" sensor_clk_drv_set <Value>          - adjust the driving strength of sensor clock\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" vsdelay_enable <MsyncSrc> <OutputChan> (<DlyVPeriod>) <PolH/VInput>"
              " <PolH/VOutput> <Refclk> <DlyH/VClk> <H/VWidth> - config and enable vsdelay output \n");
    PrintFunc(" Ex1. diag vin vsdelay_enable 0 0 2 0 0 72000000 3 4 \n"
              "      (DelayVsync0 delays 2 periods and 3 clock cycles,"
              " with pulse width=4 clock cycles and RefClk=72000000)\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" vsdelay_disable - disable vsdelay output \n");
#ifdef CONFIG_SVC_APPS_IOUT
    PrintFunc(pArgVector[0]);
    PrintFunc("Sensor Verification Usage: \n");
    PrintFunc("vin verify MIPI (<VinID> <ModeID>)    - MIPI       (Vin0) IMX586, (Vin8) IMX290, (Vin9) IMX490 \n");
    PrintFunc("vin verify SLVS                       - SLVS       (Vin8) IMX317 \n");
    PrintFunc("vin verify 8LaneMIPI                  - 8-Lane MIPI(Vin8) IMX334 \n");
    PrintFunc("vin verify VC_HDR                     - MIPI VC_HDR(Vin8) IMX334 \n");
    PrintFunc("vin verify SLVSEC                     - SLVSEC     (Vin0) IMX383 \n");
    PrintFunc("vin verify 2LinkSLVSEC                - 2LinkSLVSEC(Vin0) IMX272 \n");
    PrintFunc("vin verify 2P3G_SLVSEC                - 2P3G_SLVSEC(Vin0) IMX677 \n");
    PrintFunc("vin verify 4P6G_SLVSEC                - 4P6G_SLVSEC(Vin0) IMX677 \n");
    PrintFunc("vin verify VC (<VinID> <ModeID>)      - 4chABCD IMX390+MAX9295/96712 VC DPHY \n");
    PrintFunc("vin verify VCC (<VinID>)              - 4chABCD IMX390+MAX9295/96712 VC CPHY \n");
    PrintFunc("vin verify VCX (<VinID> <ModeID>)     - 4chABCD IMX390+MAX9295/96712 VCX DPHY \n");
    PrintFunc("vin verify VCXC (<VinID>)             - 4chABCD IMX390+MAX9295/96712 VCX CPHY \n");
    PrintFunc("vin verify REMAP (<VinID>)            - 1ch IMX390+MAX9295/9296 \n");
    PrintFunc("vin verify YUV                        - LT6911 \n");
    PrintFunc("vin verify 4K                         - (Vin8) IMX577 \n");
    PrintFunc("vin verify IRQ <VinID>                - VinID: 0-13 \n");
    PrintFunc("vin verify STATUS <VinID> <Interface> - 0U:MIPI,1U:SLVS,7:CPHY,8:SLVSEC,9:2LinkSLVSEC,10:2P3G_SLVSEC,11:4P6G_SLVSEC\n");
#endif
}

static UINT32 IsDigit(INT32 c)
{
    UINT32 RetVal;

    if ((c >= (INT32)'0') && (c <= (INT32)'9')) {
        RetVal = 1U;
    } else {
        RetVal = 0U;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIOUTDiag_CmdVIN
 *
 *  @Description:: Vin diagnostic command
 *
 *  @Input      ::
 *      ArgCount:   Argument count
 *      pArgVector: Argument vector
 *      IoDiag_PrintFormattedInt:  function to print messages on shell task
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaIOUTDiag_CmdVIN(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 VinChan;
#ifdef CONFIG_SVC_APPS_IOUT
    UINT32 VinID    = 0x0U;
    UINT32 ModeID   = 0xFFFFFFFFU;
    static UINT8 Init = 0U; //only do it once
    UINT32 Interface = 0U;

    if (Init == 0U) {
        AMBA_INT_CONFIG_s pIntConfig = {0U};

        //enable SLAVE_VSYNC ISR
        pIntConfig.TriggerType = 1U;
        pIntConfig.IrqType = 0U;
        pIntConfig.CpuTargets = 1U;
        (void) AmbaINT_Config(AMBA_INT_SPI_ID143_VIN0_VSYNC, &pIntConfig, IRQ_Vin0, 0U);
        (void) AmbaINT_Enable(AMBA_INT_SPI_ID143_VIN0_VSYNC);
        (void) AmbaINT_Config(AMBA_INT_SPI_ID148_VIN1_VSYNC, &pIntConfig, IRQ_Vin1, 0U);
        (void) AmbaINT_Enable(AMBA_INT_SPI_ID148_VIN1_VSYNC);
        (void) AmbaINT_Config(AMBA_INT_SPI_ID152_VIN2_VSYNC, &pIntConfig, IRQ_Vin2, 0U);
        (void) AmbaINT_Enable(AMBA_INT_SPI_ID152_VIN2_VSYNC);
        (void) AmbaINT_Config(AMBA_INT_SPI_ID156_VIN3_VSYNC, &pIntConfig, IRQ_Vin3, 0U);
        (void) AmbaINT_Enable(AMBA_INT_SPI_ID156_VIN3_VSYNC);
        (void) AmbaINT_Config(AMBA_INT_SPI_ID138_VIN4_VSYNC, &pIntConfig, IRQ_Vin4, 0U);
        (void) AmbaINT_Enable(AMBA_INT_SPI_ID138_VIN4_VSYNC);
        (void) AmbaINT_Config(AMBA_INT_SPI_ID162_VIN5_VSYNC, &pIntConfig, IRQ_Vin5, 0U);
        (void) AmbaINT_Enable(AMBA_INT_SPI_ID162_VIN5_VSYNC);
        (void) AmbaINT_Config(AMBA_INT_SPI_ID166_VIN6_VSYNC, &pIntConfig, IRQ_Vin6, 0U);
        (void) AmbaINT_Enable(AMBA_INT_SPI_ID166_VIN6_VSYNC);
        (void) AmbaINT_Config(AMBA_INT_SPI_ID170_VIN7_VSYNC, &pIntConfig, IRQ_Vin7, 0U);
        (void) AmbaINT_Enable(AMBA_INT_SPI_ID170_VIN7_VSYNC);
        (void) AmbaINT_Config(AMBA_INT_SPI_ID208_VIN8_VSYNC, &pIntConfig, IRQ_Vin8, 0U);
        (void) AmbaINT_Enable(AMBA_INT_SPI_ID208_VIN8_VSYNC);
        (void) AmbaINT_Config(AMBA_INT_SPI_ID212_VIN9_VSYNC, &pIntConfig, IRQ_Vin9, 0U);
        (void) AmbaINT_Enable(AMBA_INT_SPI_ID212_VIN9_VSYNC);
        (void) AmbaINT_Config(AMBA_INT_SPI_ID216_VIN10_VSYNC, &pIntConfig, IRQ_Vin10, 0U);
        (void) AmbaINT_Enable(AMBA_INT_SPI_ID216_VIN10_VSYNC);
        (void) AmbaINT_Config(AMBA_INT_SPI_ID220_VIN11_VSYNC, &pIntConfig, IRQ_Vin11, 0U);
        (void) AmbaINT_Enable(AMBA_INT_SPI_ID220_VIN11_VSYNC);
        (void) AmbaINT_Config(AMBA_INT_SPI_ID224_VIN12_VSYNC, &pIntConfig, IRQ_Vin12, 0U);
        (void) AmbaINT_Enable(AMBA_INT_SPI_ID224_VIN12_VSYNC);
        (void) AmbaINT_Config(AMBA_INT_SPI_ID228_VIN13_VSYNC, &pIntConfig, IRQ_Vin13, 0U);
        (void) AmbaINT_Enable(AMBA_INT_SPI_ID228_VIN13_VSYNC);
        //turn on sensor/serdes/YUV debug log
        (void) AmbaPrint_ModuleSetAllowList(0x301U, 1U);
        (void) AmbaPrint_ModuleSetAllowList(0x302U, 1U);
        (void) AmbaPrint_ModuleSetAllowList(0x303U, 1U);

        //enable DSP clocks
        (void) AmbaSYS_EnableFeature(AMBA_SYS_FEATURE_DSP);

        Init = 1;
    }
#endif

    if (ArgCount < 2U) {
        IoDiag_VinCmdUsage(pArgVector, PrintFunc);
    } else {
        (void) AmbaUtility_StringToUInt32(pArgVector[1], &VinChan);
        if ((IsDigit((INT32)pArgVector[1][0]) != 0U) && (VinChan >= AMBA_NUM_VIN_CHANNEL)) {
            PrintFunc("[Diag][Vin] ERROR: Invalid Vin id\n");
        }
        if (ArgCount >= 3U) {
            if (AmbaUtility_StringCompare(pArgVector[2], "info", 4U) == 0) {
                (void) AmbaDiag_VinShowInfo(VinChan, PrintFunc);
#ifdef CONFIG_SVC_APPS_IOUT
            } else if (AmbaUtility_StringCompare(pArgVector[1], "verify", 6U) == 0) {
                PrintFunc("[Diag][Vin] verify\n");
                if (AmbaUtility_StringCompare(pArgVector[2], "VCXC", 4U) == 0) {
                    if (ArgCount >=4U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[3], &VinID);
                        if ((VinID != 0U) && (VinID != 4U)) {
                            IoDiag_PrintFormattedInt("[NG] Vin %d is not supported on CPHY => use VIN0 instead.\n", VinID, PrintFunc);
                            VinID = 0x0U;
                        }
                    } else {
                        VinID = 0x0U;
                    }
                    PrintFunc("[Diag][Vin] verify VCX CPHY\n");
                    PrintFunc("[Diag][Vin] Please connect to 4chABCD IMX390+MAX9295/96712 with CPHY board\n");
                    IoDiag_VinVerifyVC(PrintFunc, VinID, ModeID, CPHY_VCX);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "VCX", 3U) == 0) {
                    if (ArgCount >=5U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[4], &ModeID);
                    } else {
                        ModeID = 0xFFFFFFFFU;
                    }
                    if (ArgCount >=4U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[3], &VinID);
                    } else {
                        VinID = 0x8U;
                    }
                    PrintFunc("[Diag][Vin] verify VCX DPHY\n");
                    PrintFunc("[Diag][Vin] Please connect to 4chABCD IMX390+MAX9295/96712\n");
                    IoDiag_VinVerifyVC(PrintFunc, VinID, ModeID, DPHY_VCX);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "VCC", 3U) == 0) {
                    if (ArgCount >=4U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[3], &VinID);
                        if ((VinID != 0U) && (VinID != 4U)) {
                            IoDiag_PrintFormattedInt("[NG] Vin %d is not supported on CPHY => use VIN0 instead.\n", VinID, PrintFunc);
                            VinID = 0x0U;
                        }
                    } else {
                        VinID = 0x0U;
                    }
                    PrintFunc("[Diag][Vin] verify VC CPHY\n");
                    PrintFunc("[Diag][Vin] Please connect to 4chABCD IMX390+MAX9295/96712 with CPHY board\n");
                    IoDiag_VinVerifyVC(PrintFunc, VinID, ModeID, CPHY_VC);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "VC_HDR", 6U) == 0) {
                    if (ArgCount >=5U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[4], &ModeID);
                    } else {
                        ModeID = 0xFFFFFFFFU;
                    }
                    if (ArgCount >=4U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[3], &VinID);
                    } else {
                        VinID = 0x8U;
                    }
                    IoDiag_VinVerifyInterface(IODIAG_INTERFACE_MIPI_VC_HDR, VinID, ModeID, PrintFunc);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "VC", 2U) == 0) {
                    if (ArgCount >=5U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[4], &ModeID);
                    } else {
                        ModeID = 0xFFFFFFFFU;
                    }
                    if (ArgCount >=4U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[3], &VinID);
                    } else {
                        VinID = 0x8U;
                    }
                    PrintFunc("[Diag][Vin] verify VC DPHY\n");
                    PrintFunc("[Diag][Vin] Please connect to 4chABCD IMX390+MAX9295/96712 with DPHY board\n");
                    IoDiag_VinVerifyVC(PrintFunc, VinID, ModeID, DPHY_VC);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "MIPI", 4U) == 0) {
                    if (ArgCount >=5U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[4], &ModeID);
                    } else {
                        ModeID = 0xFFFFFFFFU;
                    }
                    if (ArgCount >=4U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[3], &VinID);
                    } else {
                        VinID = 0x0U;
                    }
                    if ((VinID == 0U) || (VinID == 4U)) {
                        IoDiag_VinVerifyInterface(IODIAG_INTERFACE_MIPI_CPHY, VinID, ModeID, PrintFunc);
                    } else if (VinID == 8U) {
                        IoDiag_VinVerifyInterface(IODIAG_INTERFACE_MIPI, VinID, ModeID, PrintFunc);
                    } else if (VinID == 11U) {
                        IoDiag_VinVerifyInterface(IODIAG_INTERFACE_MIPI_PIP, VinID, ModeID, PrintFunc);
                    } else {
                        IoDiag_PrintFormattedInt("Vin %d is not supported on MIPI currently\n", VinID, PrintFunc);
                    }
                } else if (AmbaUtility_StringCompare(pArgVector[2], "SLVSEC", 6U) == 0) {
                    if (ArgCount >=5U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[4], &ModeID);
                    } else {
                        ModeID = 0xFFFFFFFFU;
                    }
                    if (ArgCount >=4U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[3], &VinID);
                    } else {
                        VinID = 0x0U;
                    }
                    IoDiag_VinVerifyInterface(IODIAG_INTERFACE_SLVSEC, VinID, ModeID, PrintFunc);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "2LinkSLVSEC", 11U) == 0) {
                    if (ArgCount >=5U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[4], &ModeID);
                    } else {
                        ModeID = 0xFFFFFFFFU;
                    }
                    if (ArgCount >=4U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[3], &VinID);
                    } else {
                        VinID = 0x0U;
                    }
                    IoDiag_VinVerifyInterface(IODIAG_INTERFACE_SLVSEC_2LINK, VinID, ModeID, PrintFunc);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "2P3G_SLVSEC", 11U) == 0) {
                    if (ArgCount >=5U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[4], &ModeID);
                    } else {
                        ModeID = 0xFFFFFFFFU;
                    }
                    if (ArgCount >=4U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[3], &VinID);
                    } else {
                        VinID = 0x0U;
                    }
                    IoDiag_VinVerifyInterface(IODIAG_INTERFACE_SLVSEC_2P3G, VinID, ModeID, PrintFunc);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "4P6G_SLVSEC", 11U) == 0) {
                    if (ArgCount >=5U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[4], &ModeID);
                    } else {
                        ModeID = 0xFFFFFFFFU;
                    }
                    if (ArgCount >=4U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[3], &VinID);
                    } else {
                        VinID = 0x0U;
                    }
                    IoDiag_VinVerifyInterface(IODIAG_INTERFACE_SLVSEC_4P6G, VinID, ModeID, PrintFunc);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "SLVS", 4U) == 0) {
                    if (ArgCount >=5U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[4], &ModeID);
                    } else {
                        ModeID = 0xFFFFFFFFU;
                    }
                    if (ArgCount >=4U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[3], &VinID);
                    } else {
                        VinID = 0x8U;
                    }
                    IoDiag_VinVerifyInterface(IODIAG_INTERFACE_SLVS, VinID, ModeID, PrintFunc);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "8LaneMIPI", 9U) == 0) {
                    if (ArgCount >=5U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[4], &ModeID);
                    } else {
                        ModeID = 0xFFFFFFFFU;
                    }
                    if (ArgCount >=4U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[3], &VinID);
                    } else {
                        VinID = 0x8U;
                    }
                    IoDiag_VinVerifyInterface(IODIAG_INTERFACE_MIPI_8LANE, VinID, ModeID, PrintFunc);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "IRQ", 3U) == 0) {
                    if (ArgCount >=4U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[3], &VinID);
                    }
                    if (VinID < AMBA_NUM_VIN_CHANNEL) {
                        IoDiag_PrintFormattedInt("Vin%d irq ", VinID, PrintFunc);
                        IoDiag_PrintFormattedInt("= %d\n", IrqCount[VinID], PrintFunc);

                    } else {
                        PrintFunc("not supported. Only support Vin0-13\n");
                    }
                } else if (AmbaUtility_StringCompare(pArgVector[2], "REMAP", 5U) == 0) {
                    PrintFunc("[Diag][Vin] verify REMAP\n");
                    if (ArgCount >= 4U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[3], &VinID);
                    } else {
                        VinID = 0x8U;
                    }
                    IoDiag_VinVerifyRemap(PrintFunc, VinID);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "YUV", 3U) == 0) {
                    PrintFunc("[Diag][Vin] verify YUV\n");
                    IoDiag_VinVerifyYuv(PrintFunc);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "4K", 2U) == 0) {
                    PrintFunc("[Diag][Vin] verify 4K\n");
                    if (ArgCount >=5U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[4], &ModeID);
                    } else {
                        ModeID = 0xFFFFFFFFU;
                    }
                    if (ArgCount >=4U) {
                        (void) AmbaUtility_StringToUInt32(pArgVector[3], &VinID);
                    } else {
                        VinID = 0x8U;
                    }
                    IoDiag_VinVerifyInterface(IODIAG_INTERFACE_MIPI_4K, VinID, ModeID, PrintFunc);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "STATUS", 5U) == 0) {
                    if (ArgCount <=5U) {
                        if (ArgCount ==5U) {
                            (void) AmbaUtility_StringToUInt32(pArgVector[3], &VinID);
                            (void) AmbaUtility_StringToUInt32(pArgVector[4], &Interface);
                        } else if (ArgCount ==4U) {
                            (void) AmbaUtility_StringToUInt32(pArgVector[3], &VinID);
                            Interface = 0U;
                        } else if (ArgCount ==3U) {
                            VinID = 0U;
                            Interface = 0U; //MIPI
                        } else {
                            //
                        }
                        if ((Interface > IODIAG_INTERFACE_LVCMOS) && (Interface < IODIAG_INTERFACE_MIPI_CPHY)) {
                            (void) IoDiag_CheckVinStatus(VinID, IODIAG_INTERFACE_MIPI, PrintFunc);
                        } else {
                            (void) IoDiag_CheckVinStatus(VinID, Interface, PrintFunc);
                        }
                    } else {
                        PrintFunc("vin verify STATUS <VinID> <Interface> - 0U:MIPI,1U:SLVS,7:CPHY,8:SLVSEC,9:2LinkSLVSEC,10:2P3G_SLVSEC,11:4P6G_SLVSEC\n");
                    }
                } else {
                    PrintFunc("Sensor Verification Usage: \n");
                    PrintFunc("vin verify MIPI (<VinID> <ModeID>)    - MIPI       (Vin0) IMX586, (Vin8) IMX290, (Vin9) IMX490 \n");
                    PrintFunc("vin verify SLVS                       - SLVS       (Vin8) IMX317 \n");
                    PrintFunc("vin verify SLVSEC                     - SLVSEC     (Vin0) IMX383 \n");
                    PrintFunc("vin verify 2LinkSLVSEC                - 2LinkSLVSEC(Vin0) IMX272 \n");
                    PrintFunc("vin verify 2P3G_SLVSEC                - 2P3G_SLVSEC(Vin0) IMX677 \n");
                    PrintFunc("vin verify 4P6G_SLVSEC                - 4P6G_SLVSEC(Vin0) IMX677 \n");
                    PrintFunc("vin verify 8LaneMIPI                  - 8-Lane MIPI(Vin8) IMX334 \n");
                    PrintFunc("vin verify VC_HDR                     - MIPI VC_HDR(Vin8) IMX334 \n");
                    PrintFunc("vin verify VC (<VinID> <ModeID>)      - 4chABCD IMX390+MAX9295/96712 VC DPHY \n");
                    PrintFunc("vin verify VCC (<VinID>)              - 4chABCD IMX390+MAX9295/96712 VC CPHY \n");
                    PrintFunc("vin verify VCX (<VinID> <ModeID>)     - 4chABCD IMX390+MAX9295/96712 VCX DPHY \n");
                    PrintFunc("vin verify VCXC (<VinID>)             - 4chABCD IMX390+MAX9295/96712 VCX CPHY \n");
                    PrintFunc("vin verify REMAP (<VinID>)            - 1ch IMX390+MAX9295/9296 \n");
                    PrintFunc("vin verify YUV                        - LT6911 \n");
                    PrintFunc("vin verify 4K                         - (Vin8) IMX577 \n");
                    PrintFunc("vin verify IRQ <VinID>                - VinID: 0-13 \n");
                    PrintFunc("vin verify STATUS <VinID> <Interface> - 0U:MIPI,1U:SLVS,7:CPHY,8:SLVSEC,9:2LinkSLVSEC,10:2P3G_SLVSEC,11:4P6G_SLVSEC\n");
                }
#endif
            } else if (AmbaUtility_StringCompare(pArgVector[2], "reset", 5U) == 0) {
                (void) AmbaDiag_VinResetStatus(VinChan, PrintFunc);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "reconfig", 8U) == 0) {
                if (ArgCount == 5U) {
                    UINT32 CapW, CapH;
                    (void) AmbaUtility_StringToUInt32(pArgVector[3], &CapW);
                    (void) AmbaUtility_StringToUInt32(pArgVector[4], &CapH);
                    (void) AmbaDiag_VinConfigThrDbgPort(VinChan, CapW, CapH, PrintFunc);
                } else {
                    (void) AmbaDiag_VinConfigThrDbgPort(VinChan, 0U, 0U, PrintFunc);
                }
            } else if (AmbaUtility_StringCompare(pArgVector[1], "msync_enable", 12U) == 0) {
                IoDiag_VinMsyncEnable(ArgCount, pArgVector, PrintFunc);
            } else if (AmbaUtility_StringCompare(pArgVector[1], "msync_disable", 13U) == 0) {
                IoDiag_VinMsyncDisable(ArgCount, pArgVector, PrintFunc);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "lvds_term_set", 13U) == 0) {
                IoDiag_VinSetLvdsTermination(ArgCount, pArgVector, PrintFunc);
            } else if (AmbaUtility_StringCompare(pArgVector[1], "sensor_clk_enable", 17U) == 0) {
                IoDiag_VinSensorClkEnable(ArgCount, pArgVector, PrintFunc);
            } else if (AmbaUtility_StringCompare(pArgVector[1], "sensor_clk_disable", 18U) == 0) {
                IoDiag_VinSensorClkDisable(ArgCount, pArgVector, PrintFunc);
            } else if (AmbaUtility_StringCompare(pArgVector[1], "sensor_clk_drv_set", 18U) == 0) {
                IoDiag_VinSensorClkSetDrvStrength(ArgCount, pArgVector, PrintFunc);
            } else if (AmbaUtility_StringCompare(pArgVector[1], "vsdelay_enable", 14U) == 0) {
                IoDiag_VinVsdelayEnable(pArgVector, PrintFunc);
            } else if (AmbaUtility_StringCompare(pArgVector[1], "vsdelay_disable", 15U) == 0) {
                IoDiag_VinVsdelayDisable(PrintFunc);
            } else {
                IoDiag_VinCmdUsage(pArgVector, PrintFunc);
            }
        }
    }
}

