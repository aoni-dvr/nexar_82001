/**
 *  @file SvcCmdSys.c
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details svc sys command functions
 *
 */

#include "AmbaShell.h"
#include "AmbaKAL.h"
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaSensor.h"
#include "AmbaYuv.h"
#if defined(CONFIG_BUILD_COMSVC_DRAMSHMOO)
#include "AmbaDramAdjust.h"
#endif

#include "SvcPref.h"
#include "SvcClock.h"
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcNvm.h"
#include "SvcWrap.h"
#include "SvcVinSrc.h"
#include "SvcTiming.h"
#include "SvcLiveview.h"
#include "SvcLoadCtrl.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcCmdSys.h"
#include "SvcCmd.h"
#include "SvcResCfg.h"
#include "SvcDramUtil.h"
#include "SvcTaskList.h"

static void CmdSysUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc(" svc_sys commands: \n");
    PrintFunc(" pref [save|erase]\n");
    PrintFunc(" clk\n");
    PrintFunc(" vin_stat\n");
    PrintFunc(" time_zone [+/-][mins]\n");
    PrintFunc(" rom [list (0: DSP rom file, 1: user rom file)]\n");
    PrintFunc(" cpu_load [cpu_bits] [loading (0~100, 0 is disabled)]\n");
    PrintFunc(" mem\n");
#if defined(SVC_DRAMUTIL_CMD_USED)
    PrintFunc(" dram_traffic [msec]\n");
    PrintFunc("              [dram_type (0: lpddr4-32b, 1: lpddr4-16b, 2: ddr4-32b, 3: lpddr5)]\n");
    PrintFunc("              [mode (0:ideal, 1:real)]\n");
    PrintFunc("              [dram_piece]\n");
#endif
#if defined(CONFIG_BUILD_COMSVC_DRAMSHMOO) && defined(CONFIG_DRAM_TYPE_LPDDR5)
    PrintFunc(" dram_adj start (default duration is 10000 ms)\n");
    PrintFunc(" dram_adj start [adjust_duration (ms)]\n");
    PrintFunc(" dram_adj stop\n");
    PrintFunc(" dram_adj show\n");
#endif
}

#if defined(SVC_DRAMUTIL_CMD_USED)
static void ShowDramTraffic(UINT32 MSec,
                            UINT32 DramType,
                            UINT32 Mode,
                            UINT32 DramPiece,
                            AMBA_SHELL_PRINT_f PrintFunc)
{
    char                StrBuf[256] = {'\0'};
    UINT32              i, BufLen = 256U, Rval;
    DOUBLE              Util;
    SVC_DRAMUTIL_PAR_s  Param;
    SVC_DRAMUTIL_RES_s  Result;

    Param.MSec        = MSec;
    Param.DramType    = DramType;
    Param.DramPiece   = DramPiece;
    Param.ProfileMode = Mode;
    SvcDramUtil_Profile(&Param, &Result);

    PrintFunc("#### dram traffic ####\n");
    {
        const char DramGrp[DRAM_GROUP_NUM][16U] = {
            [DRAM_GROUP_CPU]   = "[CPU  ]",
            [DRAM_GROUP_DSP]   = "[DSP  ]",
            [DRAM_GROUP_PERI]  = "[PERI ]",
            #if defined(AMBA_DRAM_CLIENT_ORCVP)
            [DRAM_GROUP_CVORC] = "[CVORC]",
            [DRAM_GROUP_VPU]   = "[VPU  ]",
            #if defined(DRAM_CLIENT_FEX)
            [DRAM_GROUP_SPU]   = "[SPU  ]",
            #endif
            #endif
        };

        for (i = 0U; i < DRAM_GROUP_NUM; i++) {
            Util = 0.0;
            if (Result.TotalBandWidth != 0U) {
                Util = (DOUBLE)Result.Traffic[i];
                Util *= (DOUBLE)100U;
                Util /= (DOUBLE)Result.TotalBandWidth;
            }
            SVC_WRAP_SNPRINT            "%s %8llu MB (%llu), %.3f percentage\n"
                SVC_SNPRN_ARG_S         StrBuf
                SVC_SNPRN_ARG_CSTR      DramGrp[i] SVC_SNPRN_ARG_POST
                SVC_SNPRN_ARG_UINT64    (Result.Traffic[i] / 1000000U)
                SVC_SNPRN_ARG_UINT64    Result.Traffic[i]
                SVC_SNPRN_ARG_DOUBLE    Util
                SVC_SNPRN_ARG_BSIZE     BufLen
                SVC_SNPRN_ARG_RLEN      &Rval
                SVC_SNPRN_ARG_E
            PrintFunc(StrBuf);
        }
    }

    /* utilization */
    SVC_WRAP_SNPRINT            "\n[Utilization]  %.3f percentage, used/avail(%llu/%llu)\n"
        SVC_SNPRN_ARG_S         StrBuf
        SVC_SNPRN_ARG_DOUBLE    Result.TotalUtil
        SVC_SNPRN_ARG_UINT64    Result.TotalTraffic
        SVC_SNPRN_ARG_UINT64    Result.TotalBandWidth
        SVC_SNPRN_ARG_BSIZE     BufLen
        SVC_SNPRN_ARG_RLEN      &Rval
        SVC_SNPRN_ARG_E
    PrintFunc(StrBuf);

    PrintFunc("\n#### counters ####\n");
    {
        const char DramCli[AMBA_NUM_DRAM_CLIENT][16U] = {
            /* cpu */
            #if defined(DRAM_CLIENT_CR52)
            [DRAM_CLIENT_CR52]          = "[CPU|R52     ]",
            #endif
            #if defined(DRAM_CLIENT_CA53)
            [DRAM_CLIENT_CA53]          = "[CPU|A53     ]",
            #endif
            #if defined(AMBA_DRAM_CLIENT_CA76_0)
            [AMBA_DRAM_CLIENT_CA76_0]   = "[CPU|A76_0   ]",
            #endif
            #if defined(AMBA_DRAM_CLIENT_CA76_1)
            [AMBA_DRAM_CLIENT_CA76_1]   = "[CPU|A76_1   ]",
            #endif
            /* dsp */
            #if defined(AMBA_DRAM_CLIENT_ORCCODE)
            [AMBA_DRAM_CLIENT_ORCCODE]  = "[DSP|ORCCODE ]",
            #endif
            #if defined(AMBA_DRAM_CLIENT_ORCCODE0)
            [AMBA_DRAM_CLIENT_ORCCODE0] = "[DSP|ORCCODE0]",
            #endif
            #if defined(AMBA_DRAM_CLIENT_ORCCODE1)
            [AMBA_DRAM_CLIENT_ORCCODE1] = "[DSP|ORCCODE1]",
            #endif
            #if defined(AMBA_DRAM_CLIENT_ORCME)
            [AMBA_DRAM_CLIENT_ORCME]    = "[DSP|ORCME   ]",
            #endif
            #if defined(AMBA_DRAM_CLIENT_ORCME0)
            [AMBA_DRAM_CLIENT_ORCME0]   = "[DSP|ORCME0  ]",
            #endif
            #if defined(AMBA_DRAM_CLIENT_ORCME1)
            [AMBA_DRAM_CLIENT_ORCME1]   = "[DSP|ORCME1  ]",
            #endif
            #if defined(AMBA_DRAM_CLIENT_SMEM)
            [AMBA_DRAM_CLIENT_SMEM]     = "[DSP|SMEM    ]",
            #endif
            #if defined(AMBA_DRAM_CLIENT_SMEM_WR)
            [AMBA_DRAM_CLIENT_SMEM_WR]  = "[DSP|SMEM_WR ]",
            #endif
            #if defined(AMBA_DRAM_CLIENT_SMEM_RD)
            [AMBA_DRAM_CLIENT_SMEM_RD]  = "[DSP|SMEM_RD ]",
            #endif
            /* vpu */
            #if defined(AMBA_DRAM_CLIENT_ORCVP)
            [AMBA_DRAM_CLIENT_ORCVP]    = "[CVORC|ORCVP ]",
            #endif
            #if defined(AMBA_DRAM_CLIENT_ORCL2)
            [AMBA_DRAM_CLIENT_ORCL2]    = "[CVORC|ORCL2 ]",
            #endif
            #if defined(AMBA_DRAM_CLIENT_VMEM)
            [AMBA_DRAM_CLIENT_VMEM]     = "[VPU|VMEM    ]",
            #endif
            #if defined(AMBA_DRAM_CLIENT_VMEM0)
            [AMBA_DRAM_CLIENT_VMEM0]    = "[VPU|VMEM0   ]",
            #endif
            #if defined(AMBA_DRAM_CLIENT_FEX)
            [AMBA_DRAM_CLIENT_FEX]      = "[SPU|FEX     ]",
            #endif
            #if defined(AMBA_DRAM_CLIENT_BMEM)
            [AMBA_DRAM_CLIENT_BMEM]     = "[SPU|BMEM    ]",
            #endif

        };

        for (i = 0U; i < AMBA_NUM_DRAM_CLIENT; i++) {
            if (SvcWrap_strlen(DramCli[i]) != 0U) {
                SVC_WRAP_SNPRINT            "%s Burst/MaskWrite(%u/%u)\n"
                    SVC_SNPRN_ARG_S         StrBuf
                    SVC_SNPRN_ARG_CSTR      DramCli[i] SVC_SNPRN_ARG_POST
                    SVC_SNPRN_ARG_UINT32    Result.DramStatic.ClientBurstStatis[i] SVC_PRN_ARG_POST
                    SVC_SNPRN_ARG_UINT32    Result.DramStatic.ClientMaskWriteStatis[i] SVC_PRN_ARG_POST
                    SVC_SNPRN_ARG_BSIZE     BufLen
                    SVC_SNPRN_ARG_RLEN      &Rval
                    SVC_SNPRN_ARG_E
                PrintFunc(StrBuf);
            }
        }
    }
}
#endif

static void CmdSysEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (1U < ArgCount) {
        if (0 == SvcWrap_strcmp("pref", pArgVector[1U])) {
            if (2U < ArgCount) {
                if (0 == SvcWrap_strcmp("save", pArgVector[2U])) {
                    UINT32  PrefBufSize;
                    ULONG   PrefBufAddr;

                    SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);

                    if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                        SvcLog_NG("CMDSYS", "Pref save failed!!", 0U, 0U);
                    } else {
                        SvcLog_OK("CMDSYS", "Pref save successfully!!", 0U, 0U);
                    }

                } else if (0 == SvcWrap_strcmp("erase", pArgVector[2U])) {
                    UINT32  PrefBufSize;
                    ULONG   PrefBufAddr;
                    void    *pBuf;

                    SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                    AmbaMisra_TypeCast(&pBuf, &PrefBufAddr);

                    if (SVC_OK != AmbaWrap_memset(pBuf, 0, PrefBufSize)) {
                        SvcLog_NG("CMDSYS", "AmbaWrap_memset failed!!", 0U, 0U);
                    }
                    if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                        SvcLog_NG("CMDSYS", "Pref erase failed!!", 0U, 0U);
                    } else {
                        SvcLog_OK("CMDSYS", "Pref erase successfully!!", 0U, 0U);
                    }
                } else {
                    CmdSysUsage(PrintFunc);
                }
            }
        } else if (0 == SvcWrap_strcmp("rom", pArgVector[1U])) {
            UINT32 RegionID = 1U; /* default: Rom user data */
            if (2U < ArgCount) {
                (void)SvcWrap_strtoul(pArgVector[2U], &RegionID);
            }
            SvcNvm_ShowRomList(RegionID);
            SvcLog_OK("CMDSYS", "rom file list dump done!!", 0U, 0U);
        } else if (0 == SvcWrap_strcmp("ptb", pArgVector[1U])) {
            SvcNvm_ShowPtb();
            SvcLog_OK("CMDSYS", "partition layout dump done!!", 0U, 0U);
        } else if (0 == SvcWrap_strcmp("clk", pArgVector[1U])) {
            SvcClock_Dump();
            SvcLog_OK("CMDSYS", "clk dump done!!", 0U, 0U);
        } else if (0 == SvcWrap_strcmp("vin_stat", pArgVector[1U])) {
            UINT32                   i;
            SVC_LIV_INFO_s           Info;
            const AMBA_DSP_WINDOW_s  *pCapWin;
            const SVC_RES_CFG_s      *pCfg = SvcResCfg_Get();
            UINT32 VinID = 0U, VinSrc = 0U;
            UINT32 Rval;
            const void *pDefRawBuf, *pDefAuxBuf;

            SvcLiveview_InfoGet(&Info);
            for (i = 0U; i < (*Info.pNumVin); i++) {
                VinID = Info.pVinCapWin[i].VinID;
                pCapWin = &(Info.pVinCapWin[i].SubChCfg[0].CaptureWindow);

                SvcLog_OK("CMDSYS", "[vin%u]", VinID, 0U);
                SvcLog_OK("CMDSYS", " cap_win", 0U, 0U);
                SvcLog_OK("CMDSYS", "  offset(%u/%u)", pCapWin->OffsetX, pCapWin->OffsetY);
                SvcLog_OK("CMDSYS", "  width/height(%u/%u)", pCapWin->Width, pCapWin->Height);

                Rval = SvcResCfg_GetVinSrc(VinID, &VinSrc);
                if (SVC_OK == Rval) {
                    if ((VinSrc == SVC_VIN_SRC_SENSOR) && (pCfg->VinCfg[VinID].pDriver != NULL)) {
                        AMBA_SENSOR_CHANNEL_s      Chan = {0};
                        AMBA_SENSOR_STATUS_INFO_s  Stat = {0};

                        Chan.VinID = VinID;
                        Chan.SensorID = 1U;
                        if (AmbaSensor_GetStatus(&Chan, &Stat) == SVC_OK) {
                            SvcLog_OK("CMDSYS", "  frate(%u/%u)"
                                            , Stat.ModeInfo.FrameRate.TimeScale
                                            , Stat.ModeInfo.FrameRate.NumUnitsInTick);
                        } else {
                            SvcLog_NG("CMDSYS", "get sensor status failed", 0U, 0U);
                        }
                    }

                    if ((VinSrc == SVC_VIN_SRC_YUV) && (pCfg->VinCfg[VinID].pDriver != NULL)) {
                        AMBA_YUV_CHANNEL_s      Chan = {0};
                        AMBA_YUV_STATUS_INFO_s  Stat = {0};

                        Chan.VinID = VinID;
                        if (AmbaYuv_GetStatus(&Chan, &Stat) == SVC_OK) {
                            SvcLog_OK("CMDSYS", "  frate(%u/%u)"
                                            , Stat.ModeInfo.OutputInfo.FrameRate.TimeScale
                                            , Stat.ModeInfo.OutputInfo.FrameRate.NumUnitsInTick);
                        } else {
                            SvcLog_NG("CMDSYS", "get yuv status failed", 0U, 0U);
                        }
                    }

                    SvcLog_OK("CMDSYS", " timeout", 0U, 0U);
                    SvcLog_OK("CMDSYS", "  boot/liv(%u/%u)", Info.pDspRes->LiveviewResource.MaxVinBootTimeout[VinID], Info.pDspRes->LiveviewResource.MaxVinTimeout[VinID]);

                    AmbaMisra_TypeCast(&pDefRawBuf, &(Info.pDspRes->LiveviewResource.DefaultRawBuf[VinID][0].BaseAddr));
                    AmbaMisra_TypeCast(&pDefAuxBuf, &(Info.pDspRes->LiveviewResource.DefaultAuxRawBuf[VinID][0].BaseAddr));
                    SvcLog_OK("CMDSYS", " def_raw", 0U, 0U);
                    SVC_WRAP_PRINT "  raw/aux(%p/%p)"
                        SVC_PRN_ARG_S "CMDSYS"
                        SVC_PRN_ARG_PROC SvcLog_OK
                        SVC_PRN_ARG_CPOINT pDefRawBuf SVC_PRN_ARG_POST
                        SVC_PRN_ARG_CPOINT pDefAuxBuf SVC_PRN_ARG_POST
                        SVC_PRN_ARG_E

                } else {
                    SvcLog_NG("CMDSYS", "Cannot get VinSrc", 0U, 0U);
                }
            }
        } else if (0 == SvcWrap_strcmp("time_zone", pArgVector[1U])) {
#if defined(CONFIG_THREADX)
            if (3U < ArgCount) {
                extern INT32 AMBA_set_UtcOffsetMinutes(INT32 UtcOffsetMinutes);
                UINT32  UMin, Sign = 0U, Err = SVC_OK;
                INT32   Min;

                if (0 == SvcWrap_strcmp("+", pArgVector[2U])) {
                    Sign = 1U;
                } else if (0 == SvcWrap_strcmp("-", pArgVector[2U])) {
                    Sign = 2U;
                } else {
                    SvcLog_NG("CMDPBK", "Get signed failed!!", 0U, 0U);
                    Err = SVC_NG;
                }

                if (SVC_OK != SvcWrap_strtoul(pArgVector[3U], &UMin)) {
                    SvcLog_NG("CMDPBK", "Get Minutes failed!!", 0U, 0U);
                    Err = SVC_NG;
                }

                if (Err == SVC_OK) {
                    if (Sign == 1U) {
                        Min = (INT32)UMin;
                    } else {
                        Min = 0 - (INT32)UMin;
                    }
                    (void)AMBA_set_UtcOffsetMinutes(Min);
                }
            }
#endif
        } else if (0 == SvcWrap_strcmp("cpu_load", pArgVector[1U])) {
            if (3U < ArgCount) {
                UINT32  CpuBits = 0U, Loading = 0U;

                (void)SvcWrap_strtoul(pArgVector[2U], &CpuBits);
                (void)SvcWrap_strtoul(pArgVector[3U], &Loading);

                if (0U < Loading) {
                    (void)SvcLoadCtrl_Create(CpuBits, Loading);
                } else {
                    (void)SvcLoadCtrl_Destroy(CpuBits);
                }
            }
            SvcLoadCtrl_Dump(PrintFunc);
#if defined(CONFIG_ICAM_TIMING_LOG)
        } else if (0 == SvcWrap_strcmp("timing", pArgVector[1U])) {
            SvcTime_ForcePrint = 1U;
            SvcTime_PrintBootTime();
#endif
        } else if (0 == SvcWrap_strcmp("mem", pArgVector[1U])) {
            SvcBuffer_Show();
#if defined(CONFIG_BUILD_COMSVC_DRAMSHMOO) && defined(CONFIG_DRAM_TYPE_LPDDR5)
        } else if (AmbaUtility_StringCompare(pArgVector[1], "dram_adj", 6U) == 0) {
            if (AmbaUtility_StringCompare(pArgVector[2], "start", 5U) == 0) {
                UINT32  Duration = 10000U;

                if (3U < ArgCount) {
                    (void)AmbaUtility_StringToUInt32(pArgVector[3], &Duration);
                }
                SvcDramUtil_Adjust(SVC_DRAM_ADJ_TASK_PRI, SVC_DRAM_ADJ_TASK_CPU_BITS, Duration);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "stop", 4U) == 0) {
                AmbaDramAdjust_Stop();
            } else if (AmbaUtility_StringCompare(pArgVector[2], "show", 4U) == 0) {
                AmbaDramAdjust_Show();
            } else {
                /* do nothing */
            }
#endif
#if defined(SVC_DRAMUTIL_CMD_USED)
        } else if (0 == SvcWrap_strcmp("dram_traffic", pArgVector[1U])) {
            if (4U < ArgCount) {
                UINT32  MSec = 0U, Type = 0U, Mode = 0U, DramPiece = 1U;

                (void)SvcWrap_strtoul(pArgVector[2U], &MSec);
                (void)SvcWrap_strtoul(pArgVector[3U], &Type);
                (void)SvcWrap_strtoul(pArgVector[4U], &Mode);
                if (5U < ArgCount) {
                    (void)SvcWrap_strtoul(pArgVector[5U], &DramPiece);
                }
                ShowDramTraffic(MSec, Type, Mode, DramPiece, PrintFunc);
            }
#endif
        } else {
            SvcLog_NG("CMDSYS", "## no match command", 0U, 0U);
        }
    } else {
        CmdSysUsage(PrintFunc);
    }
}

/**
* install of system shell commands
* @return none
*/
void SvcCmdSys_Install(void)
{
    AMBA_SHELL_COMMAND_s  SvcCmdSys;

    UINT32  Rval;

    SvcCmdSys.pName    = "svc_sys";
    SvcCmdSys.MainFunc = CmdSysEntry;
    SvcCmdSys.pNext    = NULL;

    Rval = SvcCmd_CommandRegister(&SvcCmdSys);
    if (SHELL_ERR_SUCCESS != Rval) {
        SvcLog_NG("CMDPBK", "## fail to install svc sys command", 0U, 0U);
    }
}
