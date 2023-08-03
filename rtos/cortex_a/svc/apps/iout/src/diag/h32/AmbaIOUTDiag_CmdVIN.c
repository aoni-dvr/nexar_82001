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
#include <AmbaGPIO.h>
#ifdef CONFIG_SVC_APPS_IOUT
#include "AmbaIOUTDiag_Priv.h"
#include "AmbaSensor.h"
#include "AmbaPrint.h"
#include "AmbaINT.h"
#include "AmbaKAL.h"
#include "AmbaSensor_IMX290_MIPI.h"
#include "AmbaSensor_IMX317.h"
#include "AmbaSensor_AR0238_PARALLEL.h"

static UINT32 IrqCount[AMBA_NUM_VIN_CHANNEL] = {0};

static void IoDiag_PrintFormattedInt(const char *pFmtString, UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[64];
    UINT32 ArgUINT32[2];

    ArgUINT32[0] = Value;
    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgUINT32);
    PrintFunc(StrBuf);
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
#ifdef CONFIG_SVC_APPS_IOUT
    PrintFunc(pArgVector[0]);
    PrintFunc(" verify\n");
    PrintFunc("           MIPI (<VinID> <ModeID>) - MIPI       (Vin0) IMX290, (Vin1/2/..) IMX490 \n");
    PrintFunc("           SLVS                    - SLVS       (Vin0) IMX317 \n");
    PrintFunc("           LVCMOS                  - LVCMOS     (Vin0) AR0238 \n");
    PrintFunc("           irq <VinID>             - VinID: 0/1/2/3/4/8 \n");

#endif
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


#define IODIAG_COMM_INTERFACE_I2C    0U
#define IODIAG_COMM_INTERFACE_SPI    1U

static void IoDiag_VinSetCommInterface(UINT32 CommInterface, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (CommInterface == IODIAG_COMM_INTERFACE_I2C) {
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_61_I2C0_CLK);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_62_I2C0_DATA);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_79_I2C1_CLK);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_80_I2C1_DATA);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_83_I2C2_CLK);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_84_I2C2_DATA);
    } else if (CommInterface == IODIAG_COMM_INTERFACE_SPI) {
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_54_SPI1_SCLK);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_55_SPI1_MOSI);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_56_SPI1_MISO);
        (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_57_SPI1_SS0);
    } else {
        IoDiag_PrintFormattedInt("Unknown communication interface:%d\n", CommInterface, PrintFunc);
    }
}
#define IODIAG_INTERFACE_MIPI         0U
#define IODIAG_INTERFACE_SLVS         1U
#define IODIAG_INTERFACE_LVCMOS       2U
#define IODIAG_NUM_INTERFACE          3U

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
    if(Interface == IODIAG_INTERFACE_MIPI) {

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

        if(VinStatus.CrcError!= 0U) {
            RetVal = 1U;
            PrintFunc("CrcError NG\n");
        } else {
            //
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
    } else if(Interface == IODIAG_INTERFACE_LVCMOS) {
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
    } else {
        PrintFunc("Please check Interface value. 0U:MIPI,1U:SLVS,2U:LVCMOS\n");
    }

    return RetVal;
}

/*
*  Return     :: 0U: OK, 1U: NG
*/
static UINT32 IoDiag_CheckFrameRate(const UINT32 VinID, const AMBA_VIN_FRAME_RATE_s *pTargetFrmRate, UINT32 *pActualFrmRate, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;
    UINT32 IrqCnt;
    UINT32 IrqTol = 1U; /* tolerance: 1 */
    DOUBLE TargetFrmRate;
    UINT32 TargetIrqCnt;

    (void)AmbaWrap_floor((((DOUBLE)pTargetFrmRate->TimeScale / (DOUBLE)pTargetFrmRate->NumUnitsInTick) + 0.5), &TargetFrmRate);
    TargetIrqCnt = (UINT32) TargetFrmRate;

    IrqCnt = IrqCount[VinID];
    (void)AmbaKAL_TaskSleep(2000U);
    IrqCnt = IrqCount[VinID] - IrqCnt;
    IrqCnt = (UINT32)(IrqCnt / 2U);

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
*             2U: LVCMOS
*  @VinID     Vin channel to be tested
*  @ModeID    0xFFFFFFFF: default configuration
*/
typedef struct {
    AMBA_SENSOR_OBJ_s   *pAmbaSensorObj;
    UINT32              ModeID;
    UINT32              CommInterface;
} IODIAG_SENSOR_CONFIG_s;
static void IoDiag_VinVerifyInterface(const UINT32 Interface, const UINT32 VinID, const UINT32 ModeID, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_SENSOR_CHANNEL_s Chan = {0};
    AMBA_SENSOR_CONFIG_s Config = {0};
    AMBA_SENSOR_MODE_INFO_s ModeInfo;
    UINT32 ActualFrmRate;
    UINT32 RetVal = 0U;

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
        [IODIAG_INTERFACE_LVCMOS] = {
            .pAmbaSensorObj = &AmbaSensor_AR0238_PARALLELObj,
            .ModeID         = AMBA_SENSOR_AR0238_PARALLEL_1920_1080_30P,
            .CommInterface  = IODIAG_COMM_INTERFACE_I2C,
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

    (void)AmbaDiag_VinConfigThrDbgPort(VinID, ModeInfo.OutputInfo.RecordingPixels.Width, ModeInfo.OutputInfo.RecordingPixels.Height, PrintFunc);

    (void)AmbaSensor_ConfigPost(&Chan);

    RetVal |= IoDiag_CheckFrameRate(VinID, &ModeInfo.FrameRate, &ActualFrmRate, PrintFunc);
    RetVal |= IoDiag_CheckVinStatus(VinID, Interface, PrintFunc);

    if (RetVal == 0U) {
        IoDiag_PrintFormattedInt("[PASS] Vin %d \n", VinID, PrintFunc);
    } else {
        IoDiag_PrintFormattedInt("[NG] Vin %d \n", VinID, PrintFunc);
    }


}

#endif
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
        //enable SLAVE_VSYNC ISR
        AMBA_INT_CONFIG_s pIntConfig = {0U};
        pIntConfig.TriggerType = 1U;
        pIntConfig.IrqType = 0U;
        pIntConfig.CpuTargets = 1U;
        (void) AmbaINT_Config(AMBA_INT_SPI_ID142_VIN_SLAVE_VSYNC, &pIntConfig, IRQ_Vin0, 0U);
        (void) AmbaINT_Enable(AMBA_INT_SPI_ID142_VIN_SLAVE_VSYNC);
        (void) AmbaINT_Config(AMBA_INT_SPI_ID167_PIP_SLAVE_VSYNC, &pIntConfig, IRQ_Vin1, 0U);
        (void) AmbaINT_Enable(AMBA_INT_SPI_ID167_PIP_SLAVE_VSYNC);
        (void) AmbaINT_Config(AMBA_INT_SPI_ID146_PIP2_SLAVE_VSYNC, &pIntConfig, IRQ_Vin2, 0U);
        (void) AmbaINT_Enable(AMBA_INT_SPI_ID146_PIP2_SLAVE_VSYNC);
        //turn on sensor/serdes/YUV debug log
        (void) AmbaPrint_ModuleSetAllowList(0x301U, 1U);
        (void) AmbaPrint_ModuleSetAllowList(0x302U, 1U);
        (void) AmbaPrint_ModuleSetAllowList(0x303U, 1U);
        Init = 1;
    }
#endif

    if (ArgCount < 2U) {
        IoDiag_VinCmdUsage(pArgVector, PrintFunc);
    } else {
        (void) AmbaUtility_StringToUInt32(pArgVector[1], &VinChan);
        if (VinChan >= AMBA_NUM_VIN_CHANNEL) {
            PrintFunc("[Diag][Vin] ERROR: Invalid Vin id\n");
        }
        if (ArgCount >= 3U) {
            if (AmbaUtility_StringCompare(pArgVector[2], "info", 4U) == 0) {
                (void) AmbaDiag_VinShowInfo(VinChan, PrintFunc);
#ifdef CONFIG_SVC_APPS_IOUT
            } else if (AmbaUtility_StringCompare(pArgVector[1], "verify", 5U) == 0) {
                PrintFunc("[Diag][Vin] verify\n");
                if (AmbaUtility_StringCompare(pArgVector[2], "MIPI", 4U) == 0) {
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
                    IoDiag_VinVerifyInterface(IODIAG_INTERFACE_MIPI, VinID, ModeID, PrintFunc);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "SLVS", 4U) == 0) {
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
                    IoDiag_VinVerifyInterface(IODIAG_INTERFACE_SLVS, VinID, ModeID, PrintFunc);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "LVCMOS", 6U) == 0) {
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
                    IoDiag_VinVerifyInterface(IODIAG_INTERFACE_LVCMOS, VinID, ModeID, PrintFunc);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "irq", 3U) == 0) {
                    (void) AmbaUtility_StringToUInt32(pArgVector[3], &VinID);
                    IoDiag_PrintFormattedInt("Vin%d irq ", VinID, PrintFunc);
                    if ((VinID == 0U) || (VinID == 1U) || (VinID == 2U)) {
                        IoDiag_PrintFormattedInt("= %d\n", IrqCount[VinID], PrintFunc);
                    } else {
                        PrintFunc("not supported. Only support Vin0/1/2/3/4/8\n");
                    }
                } else if (AmbaUtility_StringCompare(pArgVector[2], "status", 5U) == 0) {
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
                        (void) IoDiag_CheckVinStatus(VinID, Interface, PrintFunc);
                    } else {
                        PrintFunc("vin verify status <VinID> <Interface> - 0U:MIPI,1U:SLVS,2U:LVCMOS,3U:8-lane MIPI\n");
                    }
                } else {
                    PrintFunc("Sensor Verification Usage: \n");
                    PrintFunc("vin verify MIPI (<VinID> <ModeID>) - MIPI       (Vin0) IMX290, (Vin1/2/..) IMX490 \n");
                    PrintFunc("vin verify SLVS                    - SLVS       (Vin0) IMX317 \n");
                    PrintFunc("vin verify LVCMOS                  - LVCMOS     (Vin0) AR0238 \n");
                    PrintFunc("vin verify irq <VinID>             - VinID: 0/1/2/3/4/8 \n");
                    PrintFunc("vin verify status <VinID> <Interface> - Interface 0U:MIPI,1U:SLVS,2U:LVCMOS\n");
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
            } else {
                IoDiag_VinCmdUsage(pArgVector, PrintFunc);
            }
        }
    }
}

