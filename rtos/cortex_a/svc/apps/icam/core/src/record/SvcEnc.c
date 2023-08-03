/**
 *  @file SvcEnc.c
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
 *  @details svc dsp control
 *
 */

#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_Event.h"
#include "AmbaPrint.h"

#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcErrCode.h"
#include "SvcResCfg.h"
#include "SvcBRateCalc.h"
#include "SvcRecQpCtrl.h"
#if defined(CONFIG_ICAM_BIST_UCODE)
#include "AmbaShell.h"
#include "SvcUcBIST.h"
#endif
#include "SvcEnc.h"

#define SVC_LOG_ENC                 "ENC"

static UINT32                            g_NumEnc GNU_SECTION_NOZEROINIT;
static AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s  g_EncStrmCfg[AMBA_DSP_MAX_STREAM_NUM] GNU_SECTION_NOZEROINIT;
#if defined(CONFIG_ICAM_ENCMV_SUPPORTED)
static AMBA_DSP_VIDEO_ENC_MV_CFG_s       g_EncStrmMV[AMBA_DSP_MAX_STREAM_NUM] GNU_SECTION_NOZEROINIT;
#endif

static UINT8 CalcLevelIDC(const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pEnc)
{
    typedef struct {
        UINT8   Level;
        UINT32  SamplesPerSec;
    } ENC_LEVEL_s;


    UINT8              Rval;
    UINT32             i, LevelNum;
    UINT64             Samples;
    const ENC_LEVEL_s  *pLevel;

    const ENC_LEVEL_s  AVCLevel[] = {
        {11U,     3000U}, {12U,     6000U},
        {20U,    11880U}, {21U,    19800U}, {22U,    20250U},
        {30U,    40500U}, {31U,   108000U}, {32U,   216000U},
        {41U,   245760U}, {42U,   522240U},
        {50U,   589824U}, {51U,   983040U},
    };

    const ENC_LEVEL_s  HEVCLevel[] = {
        {10U,     552960U},
        {20U,    3686400U}, {21U,    7372800U},
        {30U,   16588800U}, {31U,   33177600U},
        {40U,   66846720U}, {41U,  133693440U},
        {50U,  267386880U}, {51U,  534773760U},
        {60U, 1069547520U}, {61U, 2139095040U}, {62U, 4278190080U},
    };

    Samples = pEnc->Window.Width;
    Samples *= pEnc->Window.Height;
    Samples *= pEnc->FrameRate.TimeScale;
    Samples /= pEnc->FrameRate.NumUnitsInTick;
    if (pEnc->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264) {
        pLevel = AVCLevel;
        LevelNum = (UINT32)sizeof(AVCLevel) / (UINT32)sizeof(ENC_LEVEL_s);

        Samples /= 256U;
    } else {
        pLevel = HEVCLevel;
        LevelNum = (UINT32)sizeof(HEVCLevel) / (UINT32)sizeof(ENC_LEVEL_s);
    }

    Rval = pLevel[LevelNum - 1U].Level;
    for (i = 0U; i < LevelNum; i++) {
        if ((UINT32)Samples < pLevel[i].SamplesPerSec) {
            Rval = pLevel[i].Level;
            break;
        }
    }

    return Rval;
}

/**
* get encode info
* @param [out] pInfo encode info
*/
void SvcEnc_InfoGet(SVC_ENC_INFO_s *pInfo)
{
    pInfo->pNumStrm = &g_NumEnc;
    pInfo->pStrmCfg = g_EncStrmCfg;
#if defined(CONFIG_ICAM_ENCMV_SUPPORTED)
    pInfo->pStrmMV  = g_EncStrmMV;
#endif
}

/**
* hook encoder event call-back function
* @param [in] Enable enable/disable(1/0)
* @param [in] pEvtCB encode call-back function
*/
void SvcEnc_HookEvent(UINT32 Enable, const SVC_ENC_EVTCB_s *pEvtCB)
{
    UINT32  Rval;

    if (Enable == 1U) {
        Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VIDEO_ENC_START, pEvtCB->pCBStart);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ENC, "## fail to register enc_start, rval(%u)", Rval, 0U);
        }

        Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VIDEO_ENC_STOP, pEvtCB->pCBStop);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ENC, "## fail to register enc_stop, rval(%u)", Rval, 0U);
        }

        Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VIDEO_DATA_RDY, pEvtCB->pCBDataRdy);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ENC, "## fail to register enc_data_rdy, rval(%u)", Rval, 0U);
        }
#if defined(CONFIG_ICAM_ENCMV_SUPPORTED)
        Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VIDEO_MV_DATA_RDY, pEvtCB->pCBMVData);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ENC, "## fail to register MV_data_rdy, rval(%u)", Rval, 0U);
        }
#endif
    } else {
        Rval = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_VIDEO_ENC_START, pEvtCB->pCBStart);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ENC, "## fail to unregister enc_start, rval(%u)", Rval, 0U);
        }

        Rval = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_VIDEO_ENC_STOP, pEvtCB->pCBStop);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ENC, "## fail to unregister enc_stop, rval(%u)", Rval, 0U);
        }

        Rval = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_VIDEO_DATA_RDY, pEvtCB->pCBDataRdy);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ENC, "## fail to unregister enc_data_rdy, rval(%u)", Rval, 0U);
        }
#if defined(CONFIG_ICAM_ENCMV_SUPPORTED)
        Rval = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_VIDEO_MV_DATA_RDY, pEvtCB->pCBMVData);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ENC, "## fail to unregister MV_data_rdy, rval(%u)", Rval, 0U);
        }
#endif
    }
}

/**
* config encoder
*/
void SvcEnc_Config(void)
{
    static AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s  LastStrmCfg[AMBA_DSP_MAX_STREAM_NUM] GNU_SECTION_NOZEROINIT;

    UINT32                            i;
    UINT16                            StrmIdxArr[AMBA_DSP_MAX_STREAM_NUM] = {0};
    UINT32                            Rval;
    AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s  *pEnc;

    AmbaSvcWrap_MisraMemset(LastStrmCfg, 0, sizeof(LastStrmCfg));
    for (i = 0; i < g_NumEnc; i++) {
        LastStrmCfg[i] = g_EncStrmCfg[i];
    }

    /* configure MV */
#if defined(CONFIG_ICAM_ENCMV_SUPPORTED) && defined(AMBA_DSP_ENC_CODING_TYPE_H265)
    for (i = 0; i < g_NumEnc; i++) {
        pEnc = &(LastStrmCfg[i]);
        if (0U < g_EncStrmMV[i].BufSize) {
            /* just support HEVC now */
            if (pEnc->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H265) {
                Rval = AmbaDSP_VideoEncMvConfig(i, &(g_EncStrmMV[i]));
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_ENC, "## fail to AmbaDSP_VideoEncMvConfig, rval(%u)", Rval, 0U);
                }
            }
        }
    }
#endif

    /* configure encoders */
    for (i = 0; i < g_NumEnc; i++) {
        StrmIdxArr[i] = (UINT16)i;

        pEnc = &(LastStrmCfg[i]);
        pEnc->EncConfig.LevelIDC = CalcLevelIDC(pEnc);

        /* force tile_num = 1 if it isn't HEVC */
        if ((pEnc->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264)
    #if defined(AMBA_DSP_ENC_CODING_TYPE_MJPG)
            || (pEnc->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_MJPG)) {
    #else
            ) {
    #endif
            pEnc->EncConfig.NumTile = 1U;
        }

    #if (1 < CONFIG_ICAM_DSP_ENC_CORE_NUM)
        {
            UINT32  DoHierPChk = 1U;

            if (0U < pEnc->IntervalCapture) {
                if (1U < pEnc->IntervalCapture) {
                    /* force simple gop, don't check */
                    pEnc->IntervalCapture = 1U;
                    DoHierPChk = 0U;
                } else {
                    if (pEnc->Window.Width <= (UINT16)CONFIG_ICAM_ENC_1CORE_MAX_W) {
                        DoHierPChk = 0U;
                    }
                }
            }

            if (0U < DoHierPChk) {
                if (pEnc->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264) {
                    /* for dual-core-temporal, we must use HIER_P and even number of M */
                    if (pEnc->EncConfig.GOPCfg.GOPStruct != AMBA_DSP_VIDEO_ENC_GOP_HIER_P) {
                        pEnc->EncConfig.GOPCfg.GOPStruct = AMBA_DSP_VIDEO_ENC_GOP_HIER_P;
                        SvcLog_DBG(SVC_LOG_ENC, "## Force gop to HIER_P", 0U, 0U);
                    }

                    if ((pEnc->EncConfig.GOPCfg.M % 2U) != 0U) {
                        pEnc->EncConfig.GOPCfg.M = 2U;
                        SvcLog_DBG(SVC_LOG_ENC, "## M isn't even number. Force M to 2", 0U, 0U);
                    }

                    SvcLog_DBG(SVC_LOG_ENC, "## [stream%d]", i, 0U);
                    SvcLog_DBG(SVC_LOG_ENC, "##  For H264 with 2-core encoders, must use", 0U, 0U);
                    SvcLog_DBG(SVC_LOG_ENC, "##    GOP_HIER_P and M is even number", 0U, 0U);
                }
            }
        }
    #endif
    }

    Rval = AmbaDSP_VideoEncConfig((UINT16)g_NumEnc, StrmIdxArr, LastStrmCfg);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_ENC, "## fail to config encoder, rval(%u)", Rval, 0U);
    }
}

/**
* encode start
* @param [in] NumStrm number of stream
* @param [in] pStreamIdx stream index
* @param [in] pStaArr start configuration
*/
void SvcEnc_Start(UINT16 NumStrm,
                  const UINT16 *pStreamIdx,
                  const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStaArr)
{
    UINT32  Rval;
    UINT64  RawSeq[AMBA_DSP_MAX_STREAM_NUM] = {0};

#if defined(CONFIG_ICAM_BIST_UCODE) && defined(CONFIG_ICAM_RECORD_USED)
    /* start enc bist */
    if (SvcUcBIST_Ctrl(UC_BIST_STAGE_ENC) != SVC_OK) {
        SvcLog_NG(SVC_LOG_ENC, "## fail to enable ucode enc BIST", 0U, 0U);
    }
#endif

    Rval = AmbaDSP_VideoEncStart(NumStrm, pStreamIdx, pStaArr, RawSeq);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_ENC, "## fail to start encoder, rval(%u)", Rval, 0U);
    }

}

/**
* encode stop
* @param [in] NumStrm number of stream
* @param [in] pStreamIdx stream index
* @param [in] pStopOpt stop option
*/
void SvcEnc_Stop(UINT16 NumStrm, const UINT16 *pStreamIdx, const UINT8 *pStopOpt)
{
    UINT32  Rval;
    UINT64  RawSeq[AMBA_DSP_MAX_STREAM_NUM] = {0};

    Rval = AmbaDSP_VideoEncStop(NumStrm, pStreamIdx, pStopOpt, RawSeq);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_ENC, "## fail to stop encoder, rval(%u)", Rval, 0U);
    }
}

/**
* encode parameter control
* @param [in] ParamType paramater type
* @param [in] NumStrm number of stream
* @param [in] pStreamIdx stream index
* @param [in] pNewVal pointer to new value
*/
void SvcEnc_ParamCtrl(UINT32 ParamType,
                      UINT16 NumStrm,
                      const UINT16 *pStreamIdx,
                      const UINT32 *pNewVal)
{
    UINT16  i;

    switch (ParamType) {
    case SVC_ENC_PMT_CODING:
        for (i = 0; i < NumStrm; i++) {
            if (((*pNewVal) == AMBA_DSP_ENC_CODING_TYPE_H264)
#if defined(AMBA_DSP_ENC_CODING_TYPE_MJPG)
                || ((*pNewVal) == AMBA_DSP_ENC_CODING_TYPE_MJPG)
#endif
#if defined(AMBA_DSP_ENC_CODING_TYPE_H265)
                || ((*pNewVal) == AMBA_DSP_ENC_CODING_TYPE_H265)
#endif
            ) {
                g_EncStrmCfg[pStreamIdx[i]].CodingFmt = (UINT8)(*pNewVal);
            } else {
                SvcLog_NG(SVC_LOG_ENC, "## unknown coding format(%d)", (*pNewVal), 0U);
            }
        }
        break;
    case SVC_ENC_PMT_BRATE:
        for (i = 0; i < NumStrm; i++) {
            g_EncStrmCfg[pStreamIdx[i]].EncConfig.BitRateCfg.BitRate = SvcBRateCalc_Get(pStreamIdx[i]);
            if (g_EncStrmCfg[pStreamIdx[i]].EncConfig.BitRateCfg.BitRateCtrl == AMBA_DSP_BRC_SMART_VBR) {
                SvcRecQpCtrlCfg(pStreamIdx[i], SVC_REC_QP_CTRL_ENABLE, g_EncStrmCfg[pStreamIdx[i]].EncConfig.BitRateCfg.BitRate);
            }
        }
        break;
    case SVC_ENC_PMT_TILE:
        for (i = 0; i < NumStrm; i++) {
            g_EncStrmCfg[pStreamIdx[i]].EncConfig.NumTile = (UINT8)(*pNewVal);
        }
        break;
    case SVC_ENC_PMT_SLICE:
        for (i = 0; i < NumStrm; i++) {
            g_EncStrmCfg[pStreamIdx[i]].EncConfig.NumSlice = (UINT8)(*pNewVal);
        }
        break;
    case SVC_ENC_PMT_ROTATE_FLIP:
        for (i = 0; i < NumStrm; i++) {
            g_EncStrmCfg[pStreamIdx[i]].Rotate = (UINT8)(*pNewVal);
        }
        break;
    case SVC_ENC_PMT_BRATECTRL:
        for (i = 0; i < NumStrm; i++) {
            g_EncStrmCfg[pStreamIdx[i]].EncConfig.BitRateCfg.BitRateCtrl = (UINT8)(*pNewVal);
        }
        break;
    default:
        SvcLog_NG(SVC_LOG_ENC, "## unknown parameter type, type(%u)", ParamType, 0U);
        break;
    }
}

/**
* encode dump
*/
void SvcEnc_Dump(void)
{
    UINT32                                  i;
    const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s  *pEnc;

    AmbaPrint_PrintStr5("## dump encoder parameters", NULL, NULL, NULL, NULL, NULL);
    AmbaPrint_PrintUInt5("num_encoder: %u", g_NumEnc, 0U, 0U, 0U, 0U);

    for (i = 0; i < g_NumEnc; i++) {
        pEnc = &(g_EncStrmCfg[i]);

        AmbaPrint_PrintUInt5("  [encoder_%u]", i, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("    coding    : %u", pEnc->CodingFmt, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("    timelapse : %u", pEnc->IntervalCapture, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("    brate_ctrl: %u", pEnc->EncConfig.BitRateCfg.BitRateCtrl, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("    brate     : %u", pEnc->EncConfig.BitRateCfg.BitRate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("    tile      : %u", pEnc->EncConfig.NumTile, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("    slice     : %u", pEnc->EncConfig.NumSlice, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("    rotate    : %u", pEnc->Rotate, 0U, 0U, 0U, 0U);
    }
}
