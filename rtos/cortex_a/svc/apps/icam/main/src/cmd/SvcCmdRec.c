/**
 *  @file SvcCmdRec.c
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
 *  @details svc record command functions
 *
 */

#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaDef.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaKAL.h"
#include "AmbaSYS.h"
#include "AmbaWrap.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcUtil.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AENC.h"
#include "SvcTask.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "AmbaAEncFlow.h"
#include "SvcRecMain.h"
#include "SvcDataGather.h"
#include "SvcCmdRec.h"
#include "SvcResCfg.h"
#include "SvcBRateCalc.h"
#include "SvcRecQpCtrl.h"
#include "SvcTimeLapse.h"
#include "SvcLiveview.h"
#include "SvcCmd.h"

#define SVC_LOG_CMDREC      "CREC"

static void CmdRecUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("svc_rec commands:\n");
    PrintFunc(" start [strm_bits]                  : start to record\n");
    PrintFunc(" stop [strm_bits]                   : stop to record\n");
    PrintFunc(" coding [strm_bits] [coding]        : configure coding format, 1=H264, 3=H265\n");
    PrintFunc(" tile [strm_bits] [tile_num]        : configure tile number\n");
    PrintFunc(" slice [strm_bits] [slice_num]      : configure slice number\n");
    PrintFunc(" rotate [strm_bits] [rotate_num]    : configure encode rotate\n");
    PrintFunc(" data_coding [strm_bits] [coding]   : configure data coding format, 0=CAN, 1=PTP\n");
    PrintFunc(" brate [strm_bits]                  : show bit rate\n");
    PrintFunc(" brate [strm_bits] [brate]          : configure bit rate (MBit)\n");
    PrintFunc(" stopwatch [strm_bits]              : blend stopwatch\n");
    PrintFunc(" statis [strm_bits] [secs]          : show encode statis during encode in secs, 0 from start\n");
    PrintFunc(" dbg null_write [0|1]               : on/off null_write\n");
    PrintFunc(" dbg rbx_srcin [strm_bits] [in_bits]: set source input of box\n");
    PrintFunc(" dbg rdt_srcin [strm_bits] [in_bits]: set source input of dest\n");
    PrintFunc(" dbg rdt_boxin [strm_bits] [in_bits]: set box input of dest\n");
    PrintFunc(" nmlevt_start [strm_bits] [file_num]: trigger normal event record start\n");
    PrintFunc(" nmlevt_stop [strm_bits] [file_num] : trigger normal event record stop\n");
    PrintFunc(" emgevt [strm_bits] [prev] [after]  : trigger emergency event record start\n");
    PrintFunc(" timelapse [cap_interval]           : in seconds\n");
    PrintFunc(" sch_offset [strm_bits] [frames]    : configure schedule frame offset\n");
    PrintFunc(" bitscmp [strm_bits]                : compare bitstream data\n");
    PrintFunc(" bitscmp report                     : bitstream data comparison report\n");
    PrintFunc(" dump                               : dump dsp encode config\n");
}

static void ShowStatis(UINT16 NumStrm,
                       const UINT16 *pStreamIdx,
                       const UINT32 *pSec,
                       AMBA_SHELL_PRINT_f PrintFunc)
{
    typedef AMBA_RSC_STATIS_s  STAT_STRM_t[CONFIG_ICAM_MAX_REC_STRM];

    char                    SvcPrintBuf[256];
    UINT32                  i, j, Rlen, Err, Clk = 0U;
    UINT64                  HwTimeScale, Brate, DiffTime, DiffVal;
    DOUBLE                  Frate;
    STAT_STRM_t             StatisS[AMBA_RSC_TYPE_NUM];
    STAT_STRM_t             StatisE[AMBA_RSC_TYPE_NUM];
    const AMBA_RSC_STATIS_s  *pStatS, *pStatE;

    (void)AmbaSYS_GetClkFreq(AMBA_SYS_CLK_AUD_0, &Clk);
    HwTimeScale = Clk;

    Err = AmbaWrap_memset(StatisS, 0, sizeof(StatisS));
    if (SVC_OK != Err) {
        SvcLog_NG(SVC_LOG_CMDREC, "AmbaWrap_memset with err %d", Err, 0U);
    }

    Err = AmbaWrap_memset(StatisE, 0, sizeof(StatisE));
    if (SVC_OK != Err) {
        SvcLog_NG(SVC_LOG_CMDREC, "AmbaWrap_memset with err %d", Err, 0U);
    }

    SvcRecMain_Control(SVC_RCM_GET_VID_STATIS, (UINT16)NumStrm, pStreamIdx, &(StatisS[AMBA_RSC_TYPE_VIDEO]));
    SvcRecMain_Control(SVC_RCM_GET_AUD_STATIS, (UINT16)NumStrm, pStreamIdx, &(StatisS[AMBA_RSC_TYPE_AUDIO]));
    SvcRecMain_Control(SVC_RCM_GET_DAT_STATIS, (UINT16)NumStrm, pStreamIdx, &(StatisS[AMBA_RSC_TYPE_DATA]));
    if (0U < (*pSec)) {
        (void)AmbaKAL_TaskSleep((*pSec) * 1000U);
        SvcRecMain_Control(SVC_RCM_GET_VID_STATIS, (UINT16)NumStrm, pStreamIdx, &(StatisE[AMBA_RSC_TYPE_VIDEO]));
        SvcRecMain_Control(SVC_RCM_GET_AUD_STATIS, (UINT16)NumStrm, pStreamIdx, &(StatisE[AMBA_RSC_TYPE_AUDIO]));
        SvcRecMain_Control(SVC_RCM_GET_DAT_STATIS, (UINT16)NumStrm, pStreamIdx, &(StatisE[AMBA_RSC_TYPE_DATA]));
    }

    for (i = 0U; i < NumStrm; i++) {
        SVC_WRAP_SNPRINT "[strm_%d]\n"
            SVC_SNPRN_ARG_S         SvcPrintBuf
            SVC_SNPRN_ARG_UINT32    pStreamIdx[i] SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_BSIZE     (UINT32)sizeof(SvcPrintBuf)
            SVC_SNPRN_ARG_RLEN      &Rlen
            SVC_SNPRN_ARG_E
        PrintFunc(SvcPrintBuf);

        for (j = 0U; j < AMBA_RSC_TYPE_NUM; j++) {
            pStatS = &(StatisS[j][i]);
            pStatE = &(StatisE[j][i]);
            if (0U < pStatS->RxCount) {
                if ((*pSec) == 0U) {
                    Frate = ((DOUBLE)pStatS->RxCount - 1.0);
                    Frate *= (DOUBLE)HwTimeScale;
                    Frate /= (DOUBLE)pStatS->RxLtHwTime;
                    Brate = (pStatS->RxSize * 8U);
                    Brate = (Brate * HwTimeScale) / pStatS->RxLtHwTime;
                } else {
                    DiffTime = pStatE->RxLtHwTime - pStatS->RxLtHwTime;
                    DiffVal = pStatE->RxCount - pStatS->RxCount;
                    Frate = (DOUBLE)DiffVal;
                    Frate *= (DOUBLE)HwTimeScale;
                    Frate /= (DOUBLE)DiffTime;
                    DiffVal = pStatE->RxSize - pStatS->RxSize;
                    Brate = (DiffVal * 8U);
                    Brate = (Brate * HwTimeScale) / DiffTime;
                }

                if (pStatS->RxLtExpTime <= pStatS->RxLtHwTime) {
                    DiffTime = (pStatS->RxLtHwTime - pStatS->RxLtExpTime);
                    DiffVal = 1U;
                } else {
                    DiffTime = (pStatS->RxLtExpTime - pStatS->RxLtHwTime);
                    DiffVal = 0U;
                }

                SVC_WRAP_SNPRINT " %.3f fps, %llu bps, %s(%llums), qlen(%u)\n"
                    SVC_SNPRN_ARG_S         SvcPrintBuf
                    SVC_SNPRN_ARG_DOUBLE    Frate
                    SVC_SNPRN_ARG_UINT64    Brate
                    SVC_SNPRN_ARG_CSTR      ((0U < DiffVal) ? "slow" : "fast")                SVC_SNPRN_ARG_POST
                    SVC_SNPRN_ARG_UINT64    GetRoundUpValU64((DiffTime * 1000U), HwTimeScale)
                    SVC_SNPRN_ARG_UINT32    pStatS->QueLen                                    SVC_SNPRN_ARG_POST
                    SVC_SNPRN_ARG_BSIZE     (UINT32)sizeof(SvcPrintBuf)
                    SVC_SNPRN_ARG_RLEN      &Rlen
                    SVC_SNPRN_ARG_E
                PrintFunc(SvcPrintBuf);
            }
        }
    }
}

static void UpdateBitRate(UINT16 NumStrm, const UINT16 *pStreamIdx, const UINT32 *pBitRate)
{
    UINT16         i;
    SVC_ENC_INFO_s EncInfo;

    SvcEnc_InfoGet(&EncInfo);
    for(i = 0U; i < NumStrm; i++) {
        if (*pBitRate != 0U) {
            SvcBRateCalc_Set(pStreamIdx[i], SVC_BR_CALC_MODE_ASSIGN, *pBitRate);
        } else {
            SvcBRateCalc_Set(pStreamIdx[i], SVC_BR_CALC_MODE_DEFAULT, *pBitRate);
        }
        EncInfo.pStrmCfg[pStreamIdx[i]].EncConfig.BitRateCfg.BitRate = SvcBRateCalc_Get(pStreamIdx[i]);
        if (EncInfo.pStrmCfg[pStreamIdx[i]].EncConfig.BitRateCfg.BitRateCtrl == AMBA_DSP_BRC_SMART_VBR) {
            SvcRecQpCtrlCfg(pStreamIdx[i], SVC_REC_QP_CTRL_ENABLE, EncInfo.pStrmCfg[pStreamIdx[i]].EncConfig.BitRateCfg.BitRate);
        }
    }
}

static void ShowBitRate(UINT16 NumStrm, const UINT16 *pStreamIdx, AMBA_SHELL_PRINT_f PrintFunc)
{
    char           SvcPrintBuf[128];
    UINT16         i;
    UINT32         Rlen;
    SVC_ENC_INFO_s EncInfo;

    SvcEnc_InfoGet(&EncInfo);
    for(i = 0U; i < NumStrm; i++) {
        SVC_WRAP_SNPRINT "[strm_%d brate: %d bit/sec]\n"
            SVC_SNPRN_ARG_S         SvcPrintBuf
            SVC_SNPRN_ARG_UINT32    pStreamIdx[i]                                                SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    EncInfo.pStrmCfg[pStreamIdx[i]].EncConfig.BitRateCfg.BitRate SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_BSIZE     (UINT32)sizeof(SvcPrintBuf)
            SVC_SNPRN_ARG_RLEN      &Rlen
            SVC_SNPRN_ARG_E
        PrintFunc(SvcPrintBuf);
    }
}

static void TimeLapseCfg(UINT32 Interval)
{
#if defined(CONFIG_ICAM_TIMELAPSE_USED)
    UINT32                i, Bits = 0U;
    SVC_TLAPSE_CAP_CFG_s  Cfg;
    const SVC_RES_CFG_s   *pResCfg = SvcResCfg_Get();

    for(i = 0U; i < pResCfg->RecNum; i++) {
        if (0U < pResCfg->RecStrm[i].RecSetting.TimeLapse) {
            Bits |= ((UINT32)1U << i);
        }
    }

    if (Bits > 0UL) {
        Cfg.RecStreamBits = Bits;
        if (Interval >= 1000U) {
            Cfg.CapInterval = Interval;
        } else {
            Cfg.CapInterval = 1000U;
            SvcLog_DBG(SVC_LOG_CMDREC, "CapInterval should >= 1 second", 0U, 0U);
        }

        SvcTimeLapse_CapCfg(&Cfg);
    }
#else
    AmbaMisra_TouchUnused(&Interval);
#endif
}

static UINT32 CmdRecDbgEntry(UINT32 ArgCount, char * const *pArgVector)
{
    UINT16  StrmIdxArr[CONFIG_ICAM_MAX_REC_STRM];
    UINT32  Rval = SVC_NG, NumStrm, Value;

    if (4U < ArgCount) {
        if (0 == SvcWrap_strcmp("rbx_srcin", pArgVector[2U])) {
            if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &Value)) {
                SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, Value, &NumStrm, StrmIdxArr);
                if (SVC_OK == SvcWrap_strtoul(pArgVector[4U], &Value)) {
                    SvcRecMain_Control(SVC_RCM_RBX_IN, (UINT16)NumStrm, StrmIdxArr, &Value);
                    Rval = SVC_OK;
                }
            }
        } else if (0 == SvcWrap_strcmp("rdt_srcin", pArgVector[2U])) {
            if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &Value)) {
                SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, Value, &NumStrm, StrmIdxArr);
                if (SVC_OK == SvcWrap_strtoul(pArgVector[4U], &Value)) {
                    SvcRecMain_Control(SVC_RCM_RDT_SRCIN, (UINT16)NumStrm, StrmIdxArr, &Value);
                    Rval = SVC_OK;
                }
            }
        } else if (0 == SvcWrap_strcmp("rdt_boxin", pArgVector[2U])) {
            if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &Value)) {
                SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, Value, &NumStrm, StrmIdxArr);
                if (SVC_OK == SvcWrap_strtoul(pArgVector[4U], &Value)) {
                    SvcRecMain_Control(SVC_RCM_RDT_BOXIN, (UINT16)NumStrm, StrmIdxArr, &Value);
                    Rval = SVC_OK;
                }
            }
        } else {
            /* do nothing */
        }
    } else if (3U < ArgCount) {
        if (0 == SvcWrap_strcmp("null_write", pArgVector[2U])) {
            if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &Value)) {
                SvcRecMain_Control(SVC_RCM_PMT_NLWR_SET, 0, NULL, &Value);
                Rval = SVC_OK;
            }
        }
    } else {
        /* do nothing */
    }

    return Rval;
}

static void CmdRecEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT16  StrmIdxArr[CONFIG_ICAM_MAX_REC_STRM];
    UINT32  Rval = SVC_NG, Value, NumStrm, Bits;

    if (3U < ArgCount) {
        if (0 == SvcWrap_strcmp("coding", pArgVector[1U])) {
            if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Bits)) {
                SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, Bits, &NumStrm, StrmIdxArr);
                if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &Value)) {
                    SvcEnc_ParamCtrl(SVC_ENC_PMT_CODING, (UINT16)NumStrm, StrmIdxArr, &Value);
                    SvcEnc_ParamCtrl(SVC_ENC_PMT_BRATE, (UINT16)NumStrm, StrmIdxArr, &Value);
                    if (Value == 3U) {
                        Value = AMBA_RSC_VID_SUBTYPE_HEVC;
                    } else if (Value == 2U) {
                        Value = AMBA_RSC_VID_SUBTYPE_MJPG;
                    } else {
                        Value = AMBA_RSC_VID_SUBTYPE_H264;
                    }
                    SvcRecMain_Control(SVC_RCM_PMT_VDCODING, (UINT16)NumStrm, StrmIdxArr, &Value);
                    SvcLog_OK(SVC_LOG_CMDREC, "video coding selection succeeded(%u/%u)", Bits, Value + 1U);
                    Rval = SVC_OK;
                }
            }
        } else if (0 == SvcWrap_strcmp("tile", pArgVector[1U])) {
            if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Bits)) {
                SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, Bits, &NumStrm, StrmIdxArr);
                if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &Value)) {
                    SvcEnc_ParamCtrl(SVC_ENC_PMT_TILE, (UINT16)NumStrm, StrmIdxArr, &Value);
                    SvcLog_OK(SVC_LOG_CMDREC, "video tile config succeeded(%u/%u)", Bits, Value);
                    Rval = SVC_OK;
                }
            }
        } else if (0 == SvcWrap_strcmp("slice", pArgVector[1U])) {
            if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Bits)) {
                SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, Bits, &NumStrm, StrmIdxArr);
                if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &Value)) {
                    SvcEnc_ParamCtrl(SVC_ENC_PMT_SLICE, (UINT16)NumStrm, StrmIdxArr, &Value);
                    SvcLog_OK(SVC_LOG_CMDREC, "video slice config succeeded(%u/%u)", Bits, Value);
                    Rval = SVC_OK;
                }
            }
        } else if (0 == SvcWrap_strcmp("rotate", pArgVector[1U])) {
            if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Bits)) {
                SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, Bits, &NumStrm, StrmIdxArr);
                if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &Value)) {
                    SvcEnc_ParamCtrl(SVC_ENC_PMT_ROTATE_FLIP, (UINT16)NumStrm, StrmIdxArr, &Value);
                    SvcLog_OK(SVC_LOG_CMDREC, "encode rotate config succeeded(%u/%u)", Bits, Value);
                    Rval = SVC_OK;
                }
            }
        } else if (0 == SvcWrap_strcmp("data_coding", pArgVector[1U])) {
            if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Bits)) {
                SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, Bits, &NumStrm, StrmIdxArr);
                if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &Value)) {
                    if (Value < AMBA_RSC_DATA_SUBTYPE_MAX) {
                        SvcRecMain_Control(SVC_RCM_PMT_DACODING, (UINT16)NumStrm, StrmIdxArr, &Value);
                        SvcLog_OK(SVC_LOG_CMDREC, "data coding selection succeeded(%u/%u)", Bits, Value);
                        Rval = SVC_OK;
                    }
                }
            }
        } else if (0 == SvcWrap_strcmp("statis", pArgVector[1U])) {
            UINT32  StartBits = 0U;

            if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Value)) {
                SvcRecMain_Control(SVC_RCM_GET_ENC_STATUS, 0, NULL, &StartBits);

                Value &= StartBits;
                SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, Value, &NumStrm, StrmIdxArr);
                if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &Value)) {
                    ShowStatis((UINT16)NumStrm, StrmIdxArr, &Value, PrintFunc);
                    Rval = SVC_OK;
                }
            }
        } else if (0 == SvcWrap_strcmp("dbg", pArgVector[1U])) {
            Rval = CmdRecDbgEntry(ArgCount, pArgVector);
        } else if (0 == SvcWrap_strcmp("brate", pArgVector[1U])) {
            if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Bits)) {
                SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, Bits, &NumStrm, StrmIdxArr);
                if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &Value)) {
                    Value *= 1000000U;
                    UpdateBitRate((UINT16)NumStrm, StrmIdxArr, &Value);
                    Rval = SVC_OK;
                }
            }
        } else if (0 == SvcWrap_strcmp("nmlevt_start", pArgVector[1U])) {
            UINT32 FileNum;
            if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Bits)) {
                if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &FileNum)) {
                    AmbaRdtNmlEvt_EvtStart(Bits, FileNum);
                    Rval = SVC_OK;
                }
            }
        } else if (0 == SvcWrap_strcmp("nmlevt_stop", pArgVector[1U])) {
            UINT32 FileNum;
            if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Bits)) {
                if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &FileNum)) {
                    AmbaRdtNmlEvt_EvtStop(Bits, FileNum);
                    Rval = SVC_OK;
                }
            }
        } else if (0 == SvcWrap_strcmp("emgevt", pArgVector[1U])) {
            UINT32 Prev = 0U, After = 0U, Err;
            if (4U < ArgCount) {
                Err = SvcWrap_strtoul(pArgVector[2U], &Bits);
                if (Err == SVC_OK) {
                    Err = SvcWrap_strtoul(pArgVector[3U], &Prev);
                }
                if (Err == SVC_OK) {
                    Err = SvcWrap_strtoul(pArgVector[4U], &After);
                }
                if (Err == SVC_OK) {
                    Err = AmbaRdtEmgEvt_EvtStart(Bits, Prev, After);
                }
                if (Err == SVC_OK) {
                    Rval = SVC_OK;
                }
            }
        } else if (0 == SvcWrap_strcmp("sch_offset", pArgVector[1U])) {
            if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Bits)) {
                SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, Bits, &NumStrm, StrmIdxArr);
                if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &Value)) {
                    SvcRecMain_Control(SVC_RCM_SET_SCH_VIDOFF, (UINT16)NumStrm, StrmIdxArr, &Value);
                    Rval = SVC_OK;
                }
            }
        } else {
            AmbaMisra_TouchUnused(&Rval);
        }
    } else if (2U < ArgCount) {
        if (0 == SvcWrap_strcmp("start", pArgVector[1U])) {
            if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Value)) {
                SvcRecMain_Start(Value, 0U);
                Rval = SVC_OK;
            }
        } else if (0 == SvcWrap_strcmp("stop", pArgVector[1U])) {
            if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Value)) {
                SvcRecMain_Stop(Value, 0U);
                Rval = SVC_OK;
            }
        } else if (0 == SvcWrap_strcmp("brate", pArgVector[1U])) {
            if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Bits)) {
                SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, Bits, &NumStrm, StrmIdxArr);
                ShowBitRate((UINT16)NumStrm, StrmIdxArr, PrintFunc);
                Rval = SVC_OK;
            }
        } else if (0 == SvcWrap_strcmp("stopwatch", pArgVector[1U])) {
            if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Bits)) {
                SvcRecMain_Control(SVC_RCM_SET_BLEND, 0U, NULL, &Bits);
                Rval = SVC_OK;
            }
        } else if (0 == SvcWrap_strcmp("timelapse", pArgVector[1U])) {
            UINT32 Interval;

            if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Interval)) {
                TimeLapseCfg(Interval * 1000U);
                Rval = SVC_OK;
            }
        } else if (0 == SvcWrap_strcmp("bitscmp", pArgVector[1U])) {
            if (0 == SvcWrap_strcmp("report", pArgVector[2U])) {
                SvcRecMain_Control(SVC_RCM_BITSCMP_INFO, 0, NULL, NULL);
                Rval = SVC_OK;
            } else if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Value)) {
                UINT32 BrateCtrl = AMBA_DSP_BRC_DISABLE_AND_FIX_QP;

                SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, Value, &NumStrm, StrmIdxArr);
                if (Value > 0U) {
                    SvcEnc_ParamCtrl(SVC_ENC_PMT_BRATECTRL, (UINT16)NumStrm, StrmIdxArr, &BrateCtrl);
                    SvcRecMain_Control(SVC_RCM_BITSCMP_ON, (UINT16)NumStrm, StrmIdxArr, &Value);
                } else {
                    SvcRecMain_Control(SVC_RCM_BITSCMP_OFF, (UINT16)NumStrm, StrmIdxArr, &Value);
                }
                Rval = SVC_OK;
            } else {
                /**/
            }
        } else {
            AmbaMisra_TouchUnused(&Rval);
        }
    } else if (1U < ArgCount) {
        if (0 == SvcWrap_strcmp("dump", pArgVector[1U])) {
            SvcRecMain_Dump();
            SvcEnc_Dump();
            Rval = SVC_OK;
#if defined(CONFIG_ICAM_DATA_GATHER_USED)
        } else if (0 == SvcWrap_strcmp("dump_datg", pArgVector[1U])) {
            SvcDataGather_Dump(PrintFunc);
            Rval = SVC_OK;
#endif
        } else {
            AmbaMisra_TouchUnused(&Rval);
        }
    } else {
        PrintFunc("## too few parameters\n\n");
    }

    if (Rval == SVC_OK) {
        PrintFunc("@@ cmd is done!\n\n");
    } else {
        CmdRecUsage(PrintFunc);
    }
}

/**
* install svc rec command
*/
void SvcCmdRec_Install(void)
{
    AMBA_SHELL_COMMAND_s  SvcCmdRec;

    UINT32  Rval;

    SvcCmdRec.pName    = "svc_rec";
    SvcCmdRec.MainFunc = CmdRecEntry;
    SvcCmdRec.pNext    = NULL;

    Rval = SvcCmd_CommandRegister(&SvcCmdRec);
    if (SHELL_ERR_SUCCESS != Rval) {
        SvcLog_NG(SVC_LOG_CMDREC, "## fail to install svc rec command", 0U, 0U);
    }
}
