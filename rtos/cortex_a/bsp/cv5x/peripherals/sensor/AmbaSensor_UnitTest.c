/**
 *  @file AmbaSensor_UnitTest.c
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
 *  @details Sensor unit test implementation
 *
 */

#include "AmbaTypes.h"
#include "AmbaWrap.h"
#include "AmbaSensor.h"
#include "AmbaSensor_UnitTest.h"
#include "AmbaUtility.h"

#include "AmbaGPIO.h"
#include "AmbaKAL.h"
#include "AmbaI2C.h"

#include "bsp.h"

static void AmbaSensorUT_PrintFormattedInt(const char *pFmtString, UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[64];
    UINT32 ArgUINT32[2];

    ArgUINT32[0] = Value;
    (void)AmbaUtility_StringPrintUInt32(StrBuf, (UINT32)sizeof(StrBuf), pFmtString, 1U, ArgUINT32);
    PrintFunc(StrBuf);
}

static void AmbaSensorUT_PrintHexUInt32(UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char HexString[UTIL_MAX_HEX_STR_LEN];
    HexString[0] = '0';
    HexString[1] = 'x';
    HexString[2] = '\0';
    (void)AmbaUtility_UInt32ToStr(&HexString[2], UTIL_MAX_HEX_STR_LEN-2, Value, 16);
    PrintFunc(HexString);
}

static void AmbaSensorUT_PrintUInt32(UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char IntString[UTIL_MAX_INT_STR_LEN];
    (void)AmbaUtility_UInt32ToStr(IntString, UTIL_MAX_INT_STR_LEN, Value, 10);
    PrintFunc(IntString);
}

static void AmbaSensorUT_PrintFloat(FLOAT Value, UINT32 Precision, AMBA_SHELL_PRINT_f PrintFunc)
{
    char FloatString[UTIL_MAX_FLOAT_STR_LEN];
    (void)AmbaUtility_FloatToStr(FloatString, UTIL_MAX_FLOAT_STR_LEN, Value, Precision);
    PrintFunc(FloatString);
}

static INT32 AmbaSensorUT_StringCompare(const char *pString1, const char *pString2)
{
    INT32 RetVal = 0;
    SIZE_t StringLength1, StringLength2;

    if ((pString1 == NULL) || (pString2 == NULL)) {
        RetVal = -1;
    } else {
        SIZE_t i;
        StringLength1 = AmbaUtility_StringLength(pString1);
        StringLength2 = AmbaUtility_StringLength(pString2);

        /* Compare string length first */
        if (StringLength1 != StringLength2) {
            RetVal = -1;
        } else {
            /* Compare string */
            for(i = 0U; i < StringLength1; i++) {
                if (pString1[i] != pString2[i]) {
                    if (pString1[i] < pString2[i]) {
                        RetVal = -1;
                    } else {
                        RetVal = 1;
                    }
                    break;
                }
            }
        }
    }
    return RetVal;
}

static void AmbaSensorUT_CmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage:\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" read <VinID> <SensorID> <Addr> - read sensor register\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" write <VinID> <SensorID> <Addr> <Data> - write sensor register\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" dump <VinID> <SensorID> <StartAddr> <Count> - dump sensor registers\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" gain <VinID> <SensorID> <LF_Gain*1000> <SF_Gain*1000> <VS_Gain*1000>\n    - set sensor (hdr) gain ('x' to keep current value)\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" gain_test <VinID> <SensorID> <MinGain> <MaxGain> <Step> <Delay(ms)> - sensor gain control test\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" shr <VinID> <SensorID> <LF_ShrTime(us)> <SF_ShrTime(us)> <VS_ShrTime(us)>\n    - set sensor (hdr) shutter ('x' to keep current value)\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" shr_test <VinID> <SensorID> <MinShr(us)> <MaxShr(us)> <Step> <Delay(ms)> - sensor shutter control test\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" clk_si <VinID> <SensorID> <Clock(Hz)> - set sensor input clock\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" hw_reset <VinID> <SensorID> - reset sensor via GPIO, for main VIN only\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" detail <VinID> <SensorID> - print sensor detail information\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" minfo <VinID> <SensorID> - print supported mode information\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" i2cscan 0 0 <I2C MasterID> - scan avalable I2C device \n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" i2c 0 0 <master_id> [slave <write_slave_addr> data0 data1 ..] [slave <read_slave_addr> count]  - read after write test\n");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSensorUT_IOTestCmd
 *
 *  @Description:: Sensor IO test commands
 *
 *  @Input      ::
 *      ArgCount:   Argument count
 *      pArgVector: Argument vector
 *      PrintFunc:  function to print messages on shell task
 *
 *  @Output     :: none
 *
 *  @Return     :: OK(0)/NG(1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AmbaSensorUT_IOTestCmd(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_SENSOR_CHANNEL_s Chan = {0};
    UINT32 Addr = 0U, Data = 0U, i = 0U, Count = 0U;
    UINT16 R_Data = 0U;
    UINT32 RetVal = OK;

    if (AmbaUtility_StringToUInt32(pArgVector[2], &(Chan.VinID))!= 0U) {
        PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
    }
    if (AmbaUtility_StringToUInt32(pArgVector[3], &(Chan.SensorID))!= 0U) {
        PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
    }

    if ((AmbaSensorUT_StringCompare(pArgVector[1], "read") == 0) && (ArgCount == 5U)) {
        /* read sensor register */
        if (AmbaUtility_StringToUInt32(pArgVector[4], &Addr) != 0U) {
            PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
        }

        if (AmbaSensor_RegisterRead(&Chan, (UINT16)Addr, &R_Data) != SENSOR_ERR_NONE) {
            PrintFunc("Register read error\n");
        } else {
            Data = R_Data;
            PrintFunc("Reg");
            AmbaSensorUT_PrintHexUInt32(Addr, PrintFunc);
            PrintFunc(" = ");
            AmbaSensorUT_PrintHexUInt32(Data, PrintFunc);
            PrintFunc(" \n ");
        }

    } else if ((AmbaSensorUT_StringCompare(pArgVector[1], "write") == 0) && (ArgCount == 6U)) {
        /* write sensor register */
        if (AmbaUtility_StringToUInt32(pArgVector[4], &Addr) != 0U) {
            PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
        }
        if (AmbaUtility_StringToUInt32(pArgVector[5], &Data) != 0U) {
            PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
        }

        if (AmbaSensor_RegisterWrite(&Chan, (UINT16)Addr, (UINT16)Data) != SENSOR_ERR_NONE) {
            PrintFunc("Register write error\n");
        }

    } else if ((AmbaSensorUT_StringCompare(pArgVector[1], "dump") == 0) && (ArgCount == 6U)) {
        if (AmbaUtility_StringToUInt32(pArgVector[4], &Addr) != 0U) {
            PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
        }
        if (AmbaUtility_StringToUInt32(pArgVector[5], &Count) != 0U) {
            PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
        }

        for (i = 0U; i < Count; i++) {
            if (AmbaSensor_RegisterRead(&Chan, (UINT16)(Addr + i), &R_Data) != SENSOR_ERR_NONE) {
                PrintFunc("Register read error\n");
            } else {
                Data = R_Data;
                PrintFunc("\nReg");
                AmbaSensorUT_PrintHexUInt32((Addr + i), PrintFunc);
                PrintFunc(" = ");
                AmbaSensorUT_PrintHexUInt32(Data, PrintFunc);
            }
        }
        PrintFunc(" \n ");

    } else {
        RetVal = 1U;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSensorUT_GainTestCmd
 *
 *  @Description:: Sensor Gain test commands
 *
 *  @Input      ::
 *      ArgCount:   Argument count
 *      pArgVector: Argument vector
 *      PrintFunc:  function to print messages on shell task
 *
 *  @Output     :: none
 *
 *  @Return     :: OK(0)/NG(1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AmbaSensorUT_GainTestCmd(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_SENSOR_CHANNEL_s Chan = {0};
    UINT32 i = 0U;

    AMBA_SENSOR_STATUS_INFO_s SensorStatus = {0};
    AMBA_SENSOR_MODE_INFO_s ModeInfo = {0};
    UINT32 ActiveChan;

    AMBA_SENSOR_GAIN_FACTOR_s DesiredFactor, ActualFactor;
    AMBA_SENSOR_GAIN_CTRL_s GainCtrl = {0};
    UINT32 GainFactor_U[AMBA_SENSOR_NUM_MAX_HDR_CHAN] = {0};
    //UINT32 WBGain_R_U, WBGain_Gr_U, WBGain_B_U, WBGain_Gb_U;

    UINT32 MinGain = 0U, MaxGain = 0U, Step = 0U, Delay = 0U;

    UINT32 RetVal = OK;

    if (AmbaUtility_StringToUInt32(pArgVector[2], &(Chan.VinID))!= 0U) {
        PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
    }
    if (AmbaUtility_StringToUInt32(pArgVector[3], &(Chan.SensorID))!= 0U) {
        PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
    }

    if (AmbaSensorUT_StringCompare(pArgVector[1], "gain") == 0) {
        (void)AmbaSensor_GetStatus(&Chan, &SensorStatus);
        (void)AmbaSensor_GetModeInfo(&Chan, &SensorStatus.ModeInfo.Config, &ModeInfo);
        (void)AmbaSensor_GetCurrentGainFactor(&Chan, &DesiredFactor.Gain[0]);

        if (ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ActiveChan = 1U;
            PrintFunc("Linear mode\n");
        } else {
            ActiveChan =  ModeInfo.HdrInfo.ActiveChannels;
            PrintFunc("HDR mode, ActiveChan = ");
            AmbaSensorUT_PrintUInt32(ModeInfo.HdrInfo.ActiveChannels, PrintFunc);
            PrintFunc("\n");
        }

        if (ArgCount != (4U + ActiveChan)) {
            AmbaSensorUT_CmdUsage(pArgVector, PrintFunc);
        } else {
            for (i = 0U; i < ActiveChan; i++) {
                if (AmbaSensorUT_StringCompare(pArgVector[4U + i], "x") != 0) {

                    if (AmbaUtility_StringToUInt32(pArgVector[4U + i], &GainFactor_U[i]) != 0U) {
                        PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
                    }
                    DesiredFactor.Gain[i] = (FLOAT)GainFactor_U[i] / 1000.0f;
                }
            }

            (void)AmbaSensor_ConvertGainFactor(&Chan, &DesiredFactor, &ActualFactor, &GainCtrl);

            for (i = 0U; i < ActiveChan; i++) {
                PrintFunc(" DesiredFactor[");
                AmbaSensorUT_PrintUInt32(i, PrintFunc);
                PrintFunc("]:");
                AmbaSensorUT_PrintFloat(DesiredFactor.Gain[i], 14, PrintFunc);
                PrintFunc(", AnalogGain[");
                AmbaSensorUT_PrintUInt32(i, PrintFunc);
                PrintFunc("]:");
                AmbaSensorUT_PrintHexUInt32(GainCtrl.AnalogGain[i], PrintFunc);
                PrintFunc(", DigitalGain[");
                AmbaSensorUT_PrintUInt32(i, PrintFunc);
                PrintFunc("]:");
                AmbaSensorUT_PrintUInt32(GainCtrl.DigitalGain[i], PrintFunc);
                PrintFunc(", ActualFactor[");
                AmbaSensorUT_PrintUInt32(i, PrintFunc);
                PrintFunc("]:");
                AmbaSensorUT_PrintFloat(ActualFactor.Gain[i], 14, PrintFunc);
                PrintFunc("\n");
            }

            (void)AmbaSensor_SetAnalogGainCtrl(&Chan, GainCtrl.AnalogGain);
            (void)AmbaSensor_SetDigitalGainCtrl(&Chan, GainCtrl.DigitalGain);
        }
#if 0
        /* Have not done yet */
    } else if (AmbaSensorUT_StringCompare(pArgVector[1], "wb") == 0) {
        AmbaUtility_StringToUInt32(pArgVector[4], &WBGain_R_U);
        AmbaUtility_StringToUInt32(pArgVector[5], &WBGain_Gr_U);
        AmbaUtility_StringToUInt32(pArgVector[6], &WBGain_B_U);
        AmbaUtility_StringToUInt32(pArgVector[7], &WBGain_Gb_U);
        DesiredFactor.pWBGain->R = (FLOAT)WBGain_R_U;
        DesiredFactor.pWBGain->Gr = (FLOAT)WBGain_Gr_U;
        DesiredFactor.pWBGain->B = (FLOAT)WBGain_B_U;
        DesiredFactor.pWBGain->Gb = (FLOAT)WBGain_Gb_U;

        (void)AmbaSensor_ConvertWbGainFactor(&Chan, &DesiredFactor, &ActualFactor, &GainCtrl);
        AmbaPrint("DesiredWBGainFactor R:%f, Gr:%f, Gb:%f, B:%f", DesiredFactor.R, DesiredFactor.Gr, DesiredFactor.Gb, DesiredFactor.B);
        AmbaPrint("WBGainCtrl R:%u, Gr:%u, Gb:%u, B:%u", GainCtrl.R, GainCtrl.Gr, GainCtrl.Gb, GainCtrl.B);
        AmbaPrint("ActualWBGainFactor R:%f, Gr:%f, Gb:%f, B:%f", ActualFactor.R, ActualFactor.Gr, ActualFactor.Gb, ActualFactor.B);

        (void)AmbaSensor_SetHdrWbGainCtrl(&Chan, &GainCtrl);
#endif
    } else if ((AmbaSensorUT_StringCompare(pArgVector[1], "gain_test") == 0) && (ArgCount == 8U)) {
        if (AmbaUtility_StringToUInt32(pArgVector[4], &MinGain) != 0U) {
            PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
        }
        if (AmbaUtility_StringToUInt32(pArgVector[5], &MaxGain) != 0U) {
            PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
        }
        if (AmbaUtility_StringToUInt32(pArgVector[6], &Step) != 0U) {
            PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
        }
        if (AmbaUtility_StringToUInt32(pArgVector[7], &Delay) != 0U) {
            PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
        }

        for (i = 0U; i < ((MaxGain - MinGain) * Step); i++) {
            DesiredFactor.Gain[0] = (FLOAT)MinGain + ((FLOAT)i / (FLOAT)Step);
            (void)AmbaSensor_ConvertGainFactor(&Chan, &DesiredFactor, &ActualFactor, &GainCtrl);

            PrintFunc(" DesiredFactor: ");
            AmbaSensorUT_PrintFloat(DesiredFactor.Gain[0], 14, PrintFunc);
            PrintFunc(", AnalogGain: ");
            AmbaSensorUT_PrintHexUInt32(GainCtrl.AnalogGain[0], PrintFunc);
            PrintFunc(", DigitalGain: ");
            AmbaSensorUT_PrintUInt32(GainCtrl.DigitalGain[0], PrintFunc);
            PrintFunc(", ActualFactor:");
            AmbaSensorUT_PrintFloat(ActualFactor.Gain[0], 14, PrintFunc);
            PrintFunc("\n");

            (void)AmbaSensor_SetAnalogGainCtrl(&Chan, &GainCtrl.AnalogGain[0]);
            (void)AmbaSensor_SetDigitalGainCtrl(&Chan, &GainCtrl.DigitalGain[0]);
            (void)AmbaKAL_TaskSleep(Delay);
        }

    } else {
        RetVal = 1U;
    }


    return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSensorUT_ShrTestCmd
 *
 *  @Description:: Sensor Shutter test commands
 *
 *  @Input      ::
 *      ArgCount:   Argument count
 *      pArgVector: Argument vector
 *      PrintFunc:  function to print messages on shell task
 *
 *  @Output     :: none
 *
 *  @Return     :: OK(0)/NG(1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AmbaSensorUT_ShrTestCmd(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_SENSOR_CHANNEL_s Chan = {0};
    UINT32 i = 0U;

    AMBA_SENSOR_STATUS_INFO_s SensorStatus = {0};
    AMBA_SENSOR_MODE_INFO_s ModeInfo = {0};
    UINT32 ActiveChan;

    UINT32 MinshrTime_U, MaxshrTime_U, Step, Delay;
    FLOAT MinshrTime, MaxshrTime;

    UINT32 ExposureTime_U[AMBA_SENSOR_NUM_MAX_HDR_CHAN] = {0};
    FLOAT DesiredExposureTime[AMBA_SENSOR_NUM_MAX_HDR_CHAN] = {0.0f};
    FLOAT ActualExposureTime[AMBA_SENSOR_NUM_MAX_HDR_CHAN] = {0.0f};
    UINT32 ShutterCtrl[AMBA_SENSOR_NUM_MAX_HDR_CHAN] = {0};

    DOUBLE CeilVal;

    UINT32 RetVal = OK;

    if (AmbaUtility_StringToUInt32(pArgVector[2], &(Chan.VinID))!= 0U) {
        PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
    }
    if (AmbaUtility_StringToUInt32(pArgVector[3], &(Chan.SensorID))!= 0U) {
        PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
    }

    if (AmbaSensorUT_StringCompare(pArgVector[1], "shr") == 0) {
        (void)AmbaSensor_GetStatus(&Chan, &SensorStatus);
        (void)AmbaSensor_GetModeInfo(&Chan, &SensorStatus.ModeInfo.Config, &ModeInfo);
        (void)AmbaSensor_GetCurrentShutterSpeed(&Chan, &DesiredExposureTime[0]);

        if (ModeInfo.NumExposureStepPerFrame != 0U) {
            if (ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
                ActiveChan = 1U;
                PrintFunc("Linear mode\n");
            } else {
                ActiveChan =  ModeInfo.HdrInfo.ActiveChannels;
                PrintFunc("HDR mode, ActiveChan = ");
                AmbaSensorUT_PrintUInt32(ModeInfo.HdrInfo.ActiveChannels, PrintFunc);
                PrintFunc("\n");
            }

            if (ArgCount != (4U + ActiveChan)) {
                AmbaSensorUT_CmdUsage(pArgVector, PrintFunc);
            } else {
                for (i = 0U; i < ActiveChan; i++) {
                    if (AmbaSensorUT_StringCompare(pArgVector[4U + i], "x") != 0) {

                        if (AmbaUtility_StringToUInt32(pArgVector[4U + i], &ExposureTime_U[i]) != 0U) {
                            PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
                        }
                        DesiredExposureTime[i] = (FLOAT)ExposureTime_U[i] / 1000000.0f;    //us -> s
                    }
                }

                (void)AmbaSensor_ConvertShutterSpeed(&Chan, &DesiredExposureTime[0], &ActualExposureTime[0], &ShutterCtrl[0]);

                PrintFunc("Exp frame: ");
                AmbaSensorUT_PrintUInt32((((ShutterCtrl[0] - 1U) / ModeInfo.NumExposureStepPerFrame) + 1U), PrintFunc);
                PrintFunc("\n");

                for (i = 0U; i < ActiveChan; i++) {
                    PrintFunc(" DesiredExpTime[");
                    AmbaSensorUT_PrintUInt32(i, PrintFunc);
                    PrintFunc("]:");
                    AmbaSensorUT_PrintFloat(DesiredExposureTime[i], 14, PrintFunc);
                    PrintFunc(" s, ShutterCtrl[");
                    AmbaSensorUT_PrintUInt32(i, PrintFunc);
                    PrintFunc("]:");
                    AmbaSensorUT_PrintUInt32(ShutterCtrl[i], PrintFunc);
                    PrintFunc(", ActualExpTime[");
                    AmbaSensorUT_PrintUInt32(i, PrintFunc);
                    PrintFunc("]:");
                    AmbaSensorUT_PrintFloat(ActualExposureTime[i], 14, PrintFunc);
                    PrintFunc(" s\n");
                }

                (void)AmbaSensor_SetSlowShutterCtrl(&Chan, ((ShutterCtrl[0] - 1U) / ModeInfo.NumExposureStepPerFrame) + 1U);
                (void)AmbaSensor_SetMasterSyncCtrl(&Chan, ((ShutterCtrl[0] - 1U) / ModeInfo.NumExposureStepPerFrame) + 1U);
                (void)AmbaSensor_SetShutterCtrl(&Chan, &ShutterCtrl[0]);
            }
        }
    } else if ((AmbaSensorUT_StringCompare(pArgVector[1], "shr_test") == 0) && (ArgCount == 8U)) {
        if (AmbaUtility_StringToUInt32(pArgVector[4], &MinshrTime_U) != 0U) {
            PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
        }
        if (AmbaUtility_StringToUInt32(pArgVector[5], &MaxshrTime_U) != 0U) {
            PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
        }
        if (AmbaUtility_StringToUInt32(pArgVector[6], &Step) != 0U) {
            PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
        }
        if (AmbaUtility_StringToUInt32(pArgVector[7], &Delay) != 0U) {
            PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
        }

        (void)AmbaSensor_GetStatus(&Chan, &SensorStatus);
        (void)AmbaSensor_GetModeInfo(&Chan, &SensorStatus.ModeInfo.Config, &ModeInfo);

        if (ModeInfo.NumExposureStepPerFrame != 0U) {
            MinshrTime = (FLOAT)MinshrTime_U / 1000000.0f;
            MaxshrTime = (FLOAT)MaxshrTime_U / 1000000.0f;

            if (AmbaWrap_ceil((((DOUBLE)MaxshrTime - (DOUBLE)MinshrTime) * (DOUBLE)Step) * 1000.0, &CeilVal) == ERR_NONE) {
                for (i = 0U; i < (UINT32)CeilVal; i++) {
                    DesiredExposureTime[0] = MinshrTime + (((FLOAT)i / (FLOAT)Step) / 1000.0f);
                    (void)AmbaSensor_ConvertShutterSpeed(&Chan, &DesiredExposureTime[0], &ActualExposureTime[0], &ShutterCtrl[0]);

                    PrintFunc(" DesiredExpTime: ");
                    AmbaSensorUT_PrintFloat(DesiredExposureTime[0], 14, PrintFunc);
                    PrintFunc(" s, ShutterCtrl: ");
                    AmbaSensorUT_PrintUInt32(ShutterCtrl[0], PrintFunc);
                    PrintFunc(", Exp frame: ");
                    AmbaSensorUT_PrintUInt32((((ShutterCtrl[0] - 1U) / ModeInfo.NumExposureStepPerFrame) + 1U), PrintFunc);
                    PrintFunc(", ActualExpTime: ");
                    AmbaSensorUT_PrintFloat(ActualExposureTime[0], 14, PrintFunc);
                    PrintFunc(" s\n");

                    (void)AmbaSensor_SetSlowShutterCtrl(&Chan, ((ShutterCtrl[0] - 1U) / ModeInfo.NumExposureStepPerFrame) + 1U);
                    (void)AmbaSensor_SetMasterSyncCtrl(&Chan, ((ShutterCtrl[0] - 1U) / ModeInfo.NumExposureStepPerFrame) + 1U);
                    (void)AmbaSensor_SetShutterCtrl(&Chan, &ShutterCtrl[0]);
                    (void)AmbaKAL_TaskSleep(Delay);
                }
            }
        }
    } else {
        RetVal = 1U;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSensorUT_DetailTestCmd
 *
 *  @Description:: Sensor Detail test commands
 *
 *  @Input      ::
 *      pArgVector: Argument vector
 *      PrintFunc:  function to print messages on shell task
 *
 *  @Output     :: none
 *
 *  @Return     :: OK(0)/NG(1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AmbaSensorUT_DetailTestCmd(char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_SENSOR_CHANNEL_s Chan = {0};

    AMBA_SENSOR_STATUS_INFO_s SensorStatus = {0};
    AMBA_SENSOR_MODE_INFO_s ModeInfo = {0};
    AMBA_SENSOR_DEVICE_INFO_s DeviceInfo;

    AMBA_SENSOR_GAIN_FACTOR_s ActualFactor;

    FLOAT ActualExposureTime[AMBA_SENSOR_NUM_MAX_HDR_CHAN] = {0.0f};

    FLOAT PixelRate;

    UINT32 RetVal = OK;

    if (AmbaUtility_StringToUInt32(pArgVector[2], &(Chan.VinID))!= 0U) {
        PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
    }
    if (AmbaUtility_StringToUInt32(pArgVector[3], &(Chan.SensorID))!= 0U) {
        PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
    }

    if (AmbaSensorUT_StringCompare(pArgVector[1], "detail") == 0) {
        (void)AmbaSensor_GetStatus(&Chan, &SensorStatus);
        (void)AmbaSensor_GetDeviceInfo(&Chan, &DeviceInfo);
        (void)AmbaSensor_GetModeInfo(&Chan, &SensorStatus.ModeInfo.Config, &ModeInfo);
        (void)AmbaSensor_GetCurrentGainFactor(&Chan, &ActualFactor.Gain[0]);
        (void)AmbaSensor_GetCurrentShutterSpeed(&Chan, &ActualExposureTime[0]);

        if ((SensorStatus.ModeInfo.OutputInfo.NumDataBits != 0U) &&
            (SensorStatus.ModeInfo.FrameRate.NumUnitsInTick != 0U) &&
            (SensorStatus.ModeInfo.LineLengthPck != 0U) &&
            (SensorStatus.ModeInfo.FrameLengthLines != 0U)) {

            PixelRate = (FLOAT)((DOUBLE)SensorStatus.ModeInfo.OutputInfo.DataRate * (DOUBLE)SensorStatus.ModeInfo.OutputInfo.NumDataLanes / (DOUBLE)SensorStatus.ModeInfo.OutputInfo.NumDataBits);
            PrintFunc("\n========== Sensor Device Info ==========");
            PrintFunc("\n full_view_width:\t");
            AmbaSensorUT_PrintUInt32(DeviceInfo.NumEffectivePixelCols, PrintFunc);
            PrintFunc("\n full_view_height:\t");
            AmbaSensorUT_PrintUInt32(DeviceInfo.NumEffectivePixelRows, PrintFunc);
            PrintFunc("\n sensor_cell_width:\t");
            AmbaSensorUT_PrintFloat(DeviceInfo.UnitCellWidth, 2, PrintFunc);
            PrintFunc(" um");
            PrintFunc("\n sensor_cell_height:\t");
            AmbaSensorUT_PrintFloat(DeviceInfo.UnitCellHeight, 2, PrintFunc);
            PrintFunc(" um");

            PrintFunc("\n========== General Info ================");
            PrintFunc("\n Sensor mode:\t\t");
            AmbaSensorUT_PrintUInt32(SensorStatus.ModeInfo.Config.ModeID, PrintFunc);
            PrintFunc("\n Main frame rate:\t");
            AmbaSensorUT_PrintFloat(((FLOAT)SensorStatus.ModeInfo.FrameRate.TimeScale / (FLOAT)SensorStatus.ModeInfo.FrameRate.NumUnitsInTick), 6, PrintFunc);
            PrintFunc(" fps");
            PrintFunc("\n Sampling factor(hori):\t");
            AmbaSensorUT_PrintUInt32(SensorStatus.ModeInfo.InputInfo.HSubsample.FactorNum, PrintFunc);
            PrintFunc(":");
            AmbaSensorUT_PrintUInt32(SensorStatus.ModeInfo.InputInfo.HSubsample.FactorDen, PrintFunc);
            PrintFunc("\n Sampling factor(vert):\t");
            AmbaSensorUT_PrintUInt32(SensorStatus.ModeInfo.InputInfo.VSubsample.FactorNum, PrintFunc);
            PrintFunc(":");
            AmbaSensorUT_PrintUInt32(SensorStatus.ModeInfo.InputInfo.VSubsample.FactorDen, PrintFunc);

            PrintFunc("\n========== Sensor Output Status ========");
            PrintFunc("\n Line_length_pck:\t");
            AmbaSensorUT_PrintUInt32(SensorStatus.ModeInfo.LineLengthPck, PrintFunc);
            PrintFunc(" (HB=");
            AmbaSensorUT_PrintUInt32((SensorStatus.ModeInfo.LineLengthPck - SensorStatus.ModeInfo.OutputInfo.RecordingPixels.Width), PrintFunc);
            PrintFunc(")");
            PrintFunc("\n Frame_length_lines:\t");
            AmbaSensorUT_PrintUInt32(SensorStatus.ModeInfo.FrameLengthLines, PrintFunc);
            PrintFunc(" (VB=");
            AmbaSensorUT_PrintUInt32((SensorStatus.ModeInfo.FrameLengthLines - SensorStatus.ModeInfo.OutputInfo.RecordingPixels.Height), PrintFunc);
            PrintFunc(")");
            PrintFunc("\n Data bits:\t\t");
            AmbaSensorUT_PrintUInt32(SensorStatus.ModeInfo.OutputInfo.NumDataBits, PrintFunc);
            PrintFunc(" bits");

            if (SensorStatus.ModeInfo.OutputInfo.BayerPattern == AMBA_SENSOR_BAYER_PATTERN_RG) {
                PrintFunc("\n Bayer Pattern:\t\tRG");
            } else if (SensorStatus.ModeInfo.OutputInfo.BayerPattern == AMBA_SENSOR_BAYER_PATTERN_BG) {
                PrintFunc("\n Bayer Pattern:\t\tBG");
            } else if (SensorStatus.ModeInfo.OutputInfo.BayerPattern == AMBA_SENSOR_BAYER_PATTERN_GR) {
                PrintFunc("\n Bayer Pattern:\t\tGR");
            } else if (SensorStatus.ModeInfo.OutputInfo.BayerPattern == AMBA_SENSOR_BAYER_PATTERN_GB) {
                PrintFunc("\n Bayer Pattern:\t\tGB");
            } else {
                PrintFunc("\n Bayer Pattern:\t\tUnknown!!");
            }
#if 0
            PrintFunc(" DSP Phase Shift:\t");
            AmbaSensorUT_PrintUInt32(SensorStatus.ModeInfo.OutputInfo.DspPhaseShift, PrintFunc);
#endif
            PrintFunc("\n Source start_x:\t");
            AmbaSensorUT_PrintUInt32((UINT32)SensorStatus.ModeInfo.OutputInfo.RecordingPixels.StartX,PrintFunc);
            PrintFunc("\n Source start_y:\t");
            AmbaSensorUT_PrintUInt32((UINT32)SensorStatus.ModeInfo.OutputInfo.RecordingPixels.StartY, PrintFunc);
            PrintFunc("\n Source end_x:\t\t");
            AmbaSensorUT_PrintUInt32(((UINT32)SensorStatus.ModeInfo.OutputInfo.RecordingPixels.StartX +
                                      (((UINT32)SensorStatus.ModeInfo.OutputInfo.RecordingPixels.Width *
                                        (UINT32)SensorStatus.ModeInfo.InputInfo.HSubsample.FactorDen) /
                                       (UINT32)SensorStatus.ModeInfo.InputInfo.HSubsample.FactorNum)- 1U), PrintFunc);
            PrintFunc("\n Source end_y:\t\t");
            AmbaSensorUT_PrintUInt32(((UINT32)SensorStatus.ModeInfo.OutputInfo.RecordingPixels.StartY +
                                      (((UINT32)SensorStatus.ModeInfo.OutputInfo.RecordingPixels.Height *
                                        (UINT32)SensorStatus.ModeInfo.InputInfo.VSubsample.FactorDen) /
                                       (UINT32)SensorStatus.ModeInfo.InputInfo.VSubsample.FactorNum)- 1U), PrintFunc);
            PrintFunc("\n Source width:\t\t");
            AmbaSensorUT_PrintUInt32((UINT32)SensorStatus.ModeInfo.OutputInfo.RecordingPixels.Width, PrintFunc);
            PrintFunc("\n Source height:\t\t");
            AmbaSensorUT_PrintUInt32((UINT32)SensorStatus.ModeInfo.OutputInfo.RecordingPixels.Height, PrintFunc);

            PrintFunc("\n========== Vin Window in Pixel Array ===");
            PrintFunc("\n start point:\t\t(");
            AmbaSensorUT_PrintUInt32(SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.StartX, PrintFunc);
            PrintFunc(", ");
            AmbaSensorUT_PrintUInt32(SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.StartY, PrintFunc);
            PrintFunc(")");
            PrintFunc("\n end point:\t\t(");
            AmbaSensorUT_PrintUInt32((UINT32)SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.StartX + (UINT32)SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.Width - 1U, PrintFunc);
            PrintFunc(", ");
            AmbaSensorUT_PrintUInt32((UINT32)SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.StartY + (UINT32)SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.Height - 1U, PrintFunc);
            PrintFunc(")");
            PrintFunc("\n center point:\t\t(");
            AmbaSensorUT_PrintUInt32((UINT32)SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.StartX + ((UINT32)SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.Width / 2U), PrintFunc);
            PrintFunc(", ");
            AmbaSensorUT_PrintUInt32((UINT32)SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.StartY + ((UINT32)SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.Height / 2U), PrintFunc);
            PrintFunc(")");

            PrintFunc("\n========== Actual Frame Timing =========");
            PrintFunc("\n Pixel/Bit clock:\t");
            AmbaSensorUT_PrintUInt32(SensorStatus.ModeInfo.InputClk, PrintFunc);
            PrintFunc(" Hz");
            PrintFunc("\n Pixel rate (I/O):\t");
            AmbaSensorUT_PrintFloat(PixelRate, 6, PrintFunc);
            PrintFunc(" pixel/sec");
            PrintFunc("\n Frame rate:\t\t");
            AmbaSensorUT_PrintFloat(PixelRate / ((FLOAT)SensorStatus.ModeInfo.LineLengthPck * (FLOAT)SensorStatus.ModeInfo.FrameLengthLines), 6, PrintFunc);
            PrintFunc(" frame/sec");
            PrintFunc("\n Row time:\t\t");
            AmbaSensorUT_PrintFloat(SensorStatus.ModeInfo.RowTime * 1000000.0f, 6, PrintFunc);
            PrintFunc(" us");
            PrintFunc("\n Vin HB time:\t\t");
            AmbaSensorUT_PrintFloat(1000000.0f * ((FLOAT)SensorStatus.ModeInfo.LineLengthPck - (FLOAT)SensorStatus.ModeInfo.OutputInfo.OutputWidth) / PixelRate, 6, PrintFunc);
            PrintFunc(" us");
            PrintFunc("\n Vin VB time:\t\t");
            AmbaSensorUT_PrintFloat(1000000.0f * (((FLOAT)SensorStatus.ModeInfo.FrameLengthLines - (FLOAT)SensorStatus.ModeInfo.OutputInfo.OutputHeight) * (FLOAT)SensorStatus.ModeInfo.LineLengthPck) / PixelRate, 6, PrintFunc);
            PrintFunc(" us");

            PrintFunc("\n========== Gain Info ====================");
            PrintFunc("\n Gain factor: \t\t");
            AmbaSensorUT_PrintFloat(ActualFactor.Gain[0], 14, PrintFunc);

            PrintFunc("\n========== Shutter Info ================");
            PrintFunc("\n Shutter Exposure Time: ");
            AmbaSensorUT_PrintFloat(ActualExposureTime[0], 14, PrintFunc);
            PrintFunc(" sec \n");
        }
    } else {
        RetVal = 1U;
    }

    return RetVal;

}

static void AmbaSensorUT_I2cScan(char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 I2cSlaveAddr;
    UINT32 I2cMasterID = 0U;
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 RxData;
    UINT32 TxSize;

    I2cRxConfig.DataSize  = 1U;
    I2cRxConfig.pDataBuf  = &RxData;

    PrintFunc("Start I2C scan ...\n");

    if (AmbaUtility_StringToUInt32(pArgVector[4], &I2cMasterID) != 0U) {
        PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
    }

    for (I2cSlaveAddr = 0U; I2cSlaveAddr <= 0xFEU; I2cSlaveAddr += 2U) {
        I2cRxConfig.SlaveAddr = I2cSlaveAddr | 1U;

        if (AmbaI2C_MasterRead(I2cMasterID, AMBA_I2C_SPEED_STANDARD, &I2cRxConfig, &TxSize, 100U) == ERR_NONE) {
            PrintFunc("Device found. SlaveAddr:\t");
            AmbaSensorUT_PrintHexUInt32(I2cSlaveAddr, PrintFunc);
            PrintFunc("\n");
        }
    }
}

#define SENSOR_UT_I2C_TX_TRANSACTION (4U)
static UINT8 I2cDataBuf[2][64];
static void AmbaSensorUT_I2cParseCmd(UINT32 *pNumTxTransaction, AMBA_I2C_TRANSACTION_s *pTxCtrl, UINT32 *pNumRxTransaction, AMBA_I2C_TRANSACTION_s *pRxCtrl, UINT32 ArgCount, char * const *pArgVector)
{
    UINT8 *pTxBuf = I2cDataBuf[0], *pRxBuf = I2cDataBuf[1];
    UINT32 SlaveAddr = 0U, NumTxTransaction = 0U, NumRxTransaction = 0U;
    UINT32 i, Data = 0U, NumTxBytes = 0U, NumArgs = ArgCount;
    char * const *pArgs = pArgVector;

    while (NumArgs >= 2U) {
        if (AmbaUtility_StringCompare(pArgs[0], "slave", 5U) == 0) {
            (void)AmbaUtility_StringToUInt32(pArgs[1], &SlaveAddr);
            if ((SlaveAddr & 0x1U) == 0x0U) {
                /* Tx transaction */
                if (NumTxTransaction < SENSOR_UT_I2C_TX_TRANSACTION) {
                    pTxCtrl[NumTxTransaction].SlaveAddr = SlaveAddr;
                    pTxCtrl[NumTxTransaction].pDataBuf = &pTxBuf[NumTxBytes];

                    for (i = 2U; i < NumArgs; i ++) {
                        if (AmbaUtility_StringCompare(pArgs[i], "slave", 5U) == 0) {
                            break;
                        } else {
                            (void)AmbaUtility_StringToUInt32(pArgs[i], &Data);
                            pTxBuf[NumTxBytes + i - 2U] = (UINT8)Data;
                        }
                    }
                    pTxCtrl[NumTxTransaction].DataSize = i - 2U;
                    NumTxBytes += (i - 2U);
                    NumTxTransaction++;

                    NumArgs -= i;
                    pArgs = &pArgs[i];
                } else {
                    /* The maximum number of tx transaction supported by this diag command. */
                    NumArgs -= 2U;
                    pArgs = &pArgs[2];
                }
            } else {
                /* Rx transaction */
                if (NumArgs >= 3U) {
                    pRxCtrl->SlaveAddr = SlaveAddr;
                    pRxCtrl->pDataBuf = &pRxBuf[0];
                    (void)AmbaUtility_StringToUInt32(pArgs[2], &(pRxCtrl->DataSize));
                    NumRxTransaction = 1U;
                }

                break;  /* At most one rx transaction is allowed. */
            }
        } else {
            /* Wrong command usage */
            NumArgs -= 1U;
            pArgs = &pArgs[1];
        }
    }

    *pNumTxTransaction = NumTxTransaction;
    *pNumRxTransaction = NumRxTransaction;
}


static void AmbaSensorUT_I2cDumpData(UINT32 DataSize, const UINT8 *pDataBuf, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i;

    if (DataSize != 0U) {
        PrintFunc("---------------------------------------------------------\n");
        PrintFunc("Offset(h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
        for (i = 0; i < DataSize; i ++) {
            if ((i & 0xfU) == 0x0U) {
                AmbaSensorUT_PrintFormattedInt("%08X  ", i, PrintFunc);
                AmbaSensorUT_PrintFormattedInt("%.2X ", pDataBuf[i], PrintFunc);
            } else {
                AmbaSensorUT_PrintFormattedInt("%.2X ", pDataBuf[i], PrintFunc);
            }

            if ((i & 0xfU) == 0xfU) {
                PrintFunc("\n");
            }
        }

        if ((i & 0xfU) != 0x0U) {
            PrintFunc("\n");
        }
    }
}

static UINT32 AmbaSensorUT_I2cReadAfterWrite(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_I2C_TRANSACTION_s TxCtrl[SENSOR_UT_I2C_TX_TRANSACTION], RxCtrl;
    UINT32 NumTxTransaction = 0U, NumRxTransaction = 0U;
    UINT32 i, j, ActualSize, RetVal = OK;
    UINT32 I2cChan = 0U;

    if (AmbaUtility_StringToUInt32(pArgVector[4], &I2cChan) != 0U) {
        PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
    }

    for (i = 0U; i < SENSOR_UT_I2C_TX_TRANSACTION; i ++) {
        TxCtrl[i].SlaveAddr = 0U;
        TxCtrl[i].DataSize = 0U;
        TxCtrl[i].pDataBuf = NULL;
    }
    RxCtrl.SlaveAddr = 0U;
    RxCtrl.DataSize = 0U;
    RxCtrl.pDataBuf = NULL;

    AmbaSensorUT_I2cParseCmd(&NumTxTransaction, &TxCtrl[0], &NumRxTransaction, &RxCtrl, (ArgCount - 5U), &pArgVector[5]);

    if (NumRxTransaction != 0U) {
        if (NumTxTransaction == 0U) {
            RetVal = AmbaI2C_MasterRead(I2cChan, I2C_SPEED_STANDARD, &RxCtrl, &ActualSize, AMBA_KAL_WAIT_FOREVER);

            /* The expected waveform on the bus */
            PrintFunc("[Diag][I2C]");
            AmbaSensorUT_PrintFormattedInt("[Master_%u]", I2cChan, PrintFunc);
            AmbaSensorUT_PrintFormattedInt(" S %02x", RxCtrl.SlaveAddr, PrintFunc);
            AmbaSensorUT_PrintFormattedInt(" (%u bytes) P\n", RxCtrl.DataSize, PrintFunc);
        } else {
            RetVal = AmbaI2C_MasterReadAfterWrite(I2cChan, I2C_SPEED_STANDARD, NumTxTransaction, &TxCtrl[0], &RxCtrl, &ActualSize, AMBA_KAL_WAIT_FOREVER);

            /* The expected waveform on the bus */
            PrintFunc("[Diag][I2C]");
            AmbaSensorUT_PrintFormattedInt("[Master_%u]", I2cChan, PrintFunc);
            AmbaSensorUT_PrintFormattedInt(" S %02x", TxCtrl[0].SlaveAddr, PrintFunc);
            for (i = 0U; i < TxCtrl[0].DataSize; i ++) {
                AmbaSensorUT_PrintFormattedInt(" %02x", (UINT32)TxCtrl[0].pDataBuf[i], PrintFunc);
            }
            for (j = 1U; j < NumTxTransaction; j ++) {
                AmbaSensorUT_PrintFormattedInt(" Sr %02x", TxCtrl[j].SlaveAddr, PrintFunc);
                for (i = 0U; i < TxCtrl[j].DataSize; i ++) {
                    AmbaSensorUT_PrintFormattedInt(" %02x", (UINT32)TxCtrl[j].pDataBuf[i], PrintFunc);
                }
            }
            AmbaSensorUT_PrintFormattedInt(" Sr %02x", RxCtrl.SlaveAddr, PrintFunc);
            AmbaSensorUT_PrintFormattedInt(" (%u bytes) P\n", RxCtrl.DataSize, PrintFunc);
        }
    } else {
        if (NumTxTransaction != 0U) {   /* NumTxTransaction = 1U; */
            RetVal = AmbaI2C_MasterWrite(I2cChan, I2C_SPEED_STANDARD, &TxCtrl[0], &ActualSize, AMBA_KAL_WAIT_FOREVER);

            /* The expected waveform on the bus */
            PrintFunc("[Diag][I2C]");
            AmbaSensorUT_PrintFormattedInt("[Master_%u]", I2cChan, PrintFunc);
            AmbaSensorUT_PrintFormattedInt(" S %02x", TxCtrl[0].SlaveAddr, PrintFunc);
            for (i = 0U; i < TxCtrl[0].DataSize; i ++) {
                AmbaSensorUT_PrintFormattedInt(" %02x", (UINT32)TxCtrl[0].pDataBuf[i], PrintFunc);
            }
            PrintFunc(" P\n");
        } else {
            PrintFunc("[Diag][I2C] ERROR: No transaction is exectuted.\n");
            RetVal = ~OK;
        }
    }

    if (RetVal != OK) {
        PrintFunc("[Diag][I2C]");
        AmbaSensorUT_PrintFormattedInt("[Master_%u] Failure. ", I2cChan, PrintFunc);
        AmbaSensorUT_PrintFormattedInt("(0x%08X)\n", RetVal, PrintFunc);
    } else {
        PrintFunc("[Diag][I2C]");
        AmbaSensorUT_PrintFormattedInt("[Master_%u] Success.\n", I2cChan, PrintFunc);

        if (NumRxTransaction != 0U) {
            AmbaSensorUT_I2cDumpData(RxCtrl.DataSize, RxCtrl.pDataBuf, PrintFunc);
        }
    }

    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSensorUT_TestCmd
 *
 *  @Description:: Sensor test commands
 *
 *  @Input      ::
 *      ArgCount:   Argument count
 *      pArgVector: Argument vector
 *      PrintFunc:  function to print messages on shell task
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaSensorUT_TestCmd(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_SENSOR_CHANNEL_s Chan = {0};

    AMBA_SENSOR_CONFIG_s Config;
    AMBA_SENSOR_MODE_INFO_s ModeInfo = {0};

    UINT32 SensorInputClk = 0U;

    if (ArgCount < 4U) {
        AmbaSensorUT_CmdUsage(pArgVector, PrintFunc);
    } else {
        if (AmbaUtility_StringToUInt32(pArgVector[2], &(Chan.VinID))!= 0U) {
            PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
        }
        if (AmbaUtility_StringToUInt32(pArgVector[3], &(Chan.SensorID))!= 0U) {
            PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
        }
        if (AmbaSensorUT_IOTestCmd(ArgCount, pArgVector, PrintFunc) == OK) {
            //AmbaSensorUT_IOTestCmd
        } else if (AmbaSensorUT_GainTestCmd(ArgCount, pArgVector, PrintFunc) == OK) {
            //AmbaSensorUT_GainTestCmd
        } else if (AmbaSensorUT_ShrTestCmd(ArgCount, pArgVector, PrintFunc) == OK) {
            //AmbaSensorUT_ShrTestCmd
        } else if ((AmbaSensorUT_StringCompare(pArgVector[1], "clk_si") == 0) && (ArgCount == 5U)) {
            if (AmbaUtility_StringToUInt32(pArgVector[4], &SensorInputClk) != 0U) {
                PrintFunc("AmbaUtility_StringToUInt32 Error!\n");
            }

            if (AmbaUserVIN_SensorClkCtrl(Chan.VinID, SensorInputClk) != VIN_ERR_NONE) {
                PrintFunc(" Set sensor clk error!\n");
            } else {
                PrintFunc(" Set sensor clk = ");
                AmbaSensorUT_PrintUInt32(SensorInputClk, PrintFunc);
                PrintFunc(" Hz for VinID: ");
                AmbaSensorUT_PrintUInt32(Chan.VinID, PrintFunc);
                PrintFunc("\n");
            }
        } else if ((AmbaSensorUT_StringCompare(pArgVector[1], "i2cscan" )== 0) && (ArgCount == 5U)) {
            AmbaSensorUT_I2cScan(pArgVector, PrintFunc);
        } else if ((AmbaSensorUT_StringCompare(pArgVector[1], "i2c" )== 0)) {
            (void) AmbaSensorUT_I2cReadAfterWrite(ArgCount, pArgVector, PrintFunc);
        } else if (ArgCount == 4U) {

            if (AmbaSensorUT_StringCompare(pArgVector[1], "hw_reset") == 0) {
                /* to be refined with bsp file */
                (void)AmbaUserGPIO_SensorResetCtrl(Chan.VinID, AMBA_GPIO_LEVEL_LOW);
                (void)AmbaKAL_TaskSleep(2);
                PrintFunc(" GPIO Low\n");

                /* to be refined with bsp file */
                //AmbaUserGPIO_SensorResetCtrl(Chan.VinID, 1U);
                (void)AmbaUserGPIO_SensorResetCtrl(Chan.VinID, AMBA_GPIO_LEVEL_HIGH);
                (void)AmbaKAL_TaskSleep(2);
                PrintFunc(" GPIO High\n");

            } else if (AmbaSensorUT_DetailTestCmd(pArgVector, PrintFunc) == OK) {
                //AmbaSensorUT_DetailTestCmd

            } else if (AmbaSensorUT_StringCompare(pArgVector[1], "minfo") == 0) {
                PrintFunc("ID\tViewAngle\tWidth\tHeight\tDataWidth\tFrameRate\tRowTime(ms)\tVBTime(ms)\n");
                Config.ModeID = 0U;

                while (AmbaSensor_GetModeInfo(&Chan, &Config, &ModeInfo) == SENSOR_ERR_NONE) {
                    AmbaSensorUT_PrintUInt32(Config.ModeID, PrintFunc);
                    if (ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
                        PrintFunc("\t");
                    } else {
                        PrintFunc("(HDR)\t");
                    }
                    AmbaSensorUT_PrintUInt32(ModeInfo.InputInfo.PhotodiodeArray.Width, PrintFunc);
                    PrintFunc("x");
                    AmbaSensorUT_PrintUInt32(ModeInfo.InputInfo.PhotodiodeArray.Height, PrintFunc);
                    PrintFunc("\t");
                    AmbaSensorUT_PrintUInt32(ModeInfo.OutputInfo.RecordingPixels.Width, PrintFunc);
                    PrintFunc("\t");
                    AmbaSensorUT_PrintUInt32(ModeInfo.OutputInfo.RecordingPixels.Height, PrintFunc);
                    PrintFunc("\t");
                    AmbaSensorUT_PrintUInt32(ModeInfo.OutputInfo.NumDataBits, PrintFunc);
                    PrintFunc("\t\t");
                    AmbaSensorUT_PrintUInt32(ModeInfo.FrameRate.TimeScale, PrintFunc);
                    PrintFunc("/");
                    AmbaSensorUT_PrintUInt32(ModeInfo.FrameRate.NumUnitsInTick, PrintFunc);
                    PrintFunc("\t");
                    AmbaSensorUT_PrintFloat(ModeInfo.RowTime * 1000.0f, 6, PrintFunc);
                    PrintFunc("\t");
                    AmbaSensorUT_PrintFloat((((FLOAT)ModeInfo.FrameLengthLines - (FLOAT)ModeInfo.OutputInfo.OutputHeight) * ModeInfo.RowTime) * 1000.0f, 6, PrintFunc);
                    PrintFunc("\n");

                    Config.ModeID++;
                }
            } else {
                AmbaSensorUT_CmdUsage(pArgVector, PrintFunc);
            }
#if 0
        } else if (AmbaSensorUT_StringCompare(pArgVector[1], "driververify") == 0) {
            /* DriverVerify */
            extern __PRE_ATTRIB_WEAK__ int DriverVerify_TestCmd(int ArgCount, char *pArgVector[]) __POST_ATTRIB_WEAK__;
            DriverVerify_TestCmd(ArgCount, pArgVector);
        } else if (AmbaSensorUT_StringCompare(pArgVector[1], "debug") == 0) {
#endif
        } else {
            AmbaSensorUT_CmdUsage(pArgVector, PrintFunc);
        }
    }
}
