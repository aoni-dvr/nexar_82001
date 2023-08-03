/**
*  @file AmbaDSP_ApiResource.c
*
 * Copyright (c) 2020 Ambarella International LP
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
*  @details DSP API Resource Check.
*
*/

#include "AmbaDSP_Def.h"
#include "AmbaDSP_ApiResource.h"
#include "AmbaDSP_CommonAPI.h"
#include "AmbaDSP_ContextUtility.h"
#include "AmbaDSP_Log.h"

static Dsp_ResourceApi_t *pDspApiResourceFunc = NULL;

static UINT32 IsFeatureEnabled(const UINT32 CheckFeature, const UINT32 FeatureBit, UINT32 *ErrLine, UINT32 *ErrFeature)
{
    UINT32 Rval = OK;
    UINT32 Enable;
    UINT32 FeatureID;

    DSP_Bit2Idx(FeatureBit, &FeatureID);

    if ((CheckFeature & FeatureBit) == FeatureBit) {
        Rval = dsp_osal_dbgport_get_clk_enable_status(FeatureID, &Enable);
        if (Rval != DSP_ERR_NONE) {
            Rval = DSP_ERR_0001; *ErrLine = __LINE__;
            *ErrFeature = FeatureBit;
        } else {
            if (Enable != 1U) {
                Rval = DSP_ERR_0001; *ErrLine = __LINE__;
                *ErrFeature = FeatureBit;
            }
        }
    }

    return Rval;
}

static UINT32 Resource_Check(const UINT32 CheckFeature)
{
    UINT32 Rval, ErrLine = 0U, ErrFeature = 0U;
    UINT32 OrcTime1, OrcTime2;

    OrcTime1 = dsp_osal_get_orc_timer();

    Rval = IsFeatureEnabled(CheckFeature, (UINT32)AMBA_DSP_CHK_DSP, &ErrLine, &ErrFeature);

    if (Rval == OK) {
        Rval = IsFeatureEnabled(CheckFeature, (UINT32)AMBA_DSP_CHK_CODEC, &ErrLine, &ErrFeature);
    }

    if (Rval == OK) {
        Rval = IsFeatureEnabled(CheckFeature, (UINT32)AMBA_DSP_CHK_MCTF, &ErrLine, &ErrFeature);
    }

    if (Rval == OK) {
        Rval = IsFeatureEnabled(CheckFeature, (UINT32)AMBA_DSP_CHK_HEVC, &ErrLine, &ErrFeature);
    }

    if (Rval == OK) {
        OrcTime2 = dsp_osal_get_orc_timer();

        if ((OrcTime2 - OrcTime1) == 0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("Resource_Check [%x][%d] FeatureID %d", Rval, ErrLine, ErrFeature, 0U, 0U);
    }

    return Rval;
}

static UINT32 Resource_DspEncHevcCheck(const UINT16 NumStream, const UINT16 *pStreamIdx, UINT32 *pHEVC)
{
    UINT32 Rval = OK, i;
    CTX_STREAM_INFO_s StrmInfo = {0};

    for (i = 0U; i < NumStream; i++) {
        HL_GetStrmInfo(HL_MTX_OPT_ALL, pStreamIdx[i], &StrmInfo);
        if (StrmInfo.CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H265) {
            *pHEVC = 1U;
            break;
        } else {
            *pHEVC = 0U;
        }
    }

    return Rval;
}

static UINT32 Resource_DspDecHevcCheck(const UINT16 NumStream, const UINT16 *pStreamIdx, UINT32 *pHEVC)
{
    UINT32 Rval = OK, i;
    CTX_VID_DEC_INFO_s StrmInfo = {0};

    for (i = 0U; i < NumStream; i++) {
        HL_GetVidDecInfo(HL_MTX_OPT_ALL, pStreamIdx[i], &StrmInfo);
        if (StrmInfo.BitsFormat == AMBA_DSP_DEC_BITS_FORMAT_H265) {
            *pHEVC = 1U;
            break;
        } else {
            *pHEVC = 0U;
        }
    }

    return Rval;
}

/* Main */
static UINT32 Resource_MainInit(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)pDspSysConfig;
    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("MainInit Resource Check Error[%x][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/* Liveview */
static UINT32 Resource_LiveviewCtrl(const UINT16 NumViewZone, const UINT16 *pViewZoneId, const UINT8* pEnable)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)NumViewZone;
    (void)pViewZoneId;
    (void)pEnable;
    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewCtrl Resource Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Resource_LiveviewUpdateIsoCfg(const UINT16 NumViewZone,
                                            const AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s *pIsoCfgCtrl,
                                            const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)NumViewZone;
    (void)pIsoCfgCtrl;
    (void)pAttachedRawSeq;

    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewUpdateIsoCfg Resource Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Resource_LiveviewUpdateVinCfg(const UINT16 VinId,
                                            const UINT16 SubChNum,
                                            const AMBA_DSP_VIN_SUB_CHAN_s *pSubCh,
                                            const AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s *pLvVinCfgCtrl,
                                            const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)VinId;
    (void)SubChNum;
    (void)pSubCh;
    (void)pLvVinCfgCtrl;
    (void)pAttachedRawSeq;

    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewUpdateVinCfg Resource Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Resource_LiveviewUpdatePymdCfg(const UINT16 NumViewZone,
                                             const UINT16 *pViewZoneId,
                                             const AMBA_DSP_LIVEVIEW_PYRAMID_s *pPyramid,
                                             const AMBA_DSP_LIVEVIEW_YUV_BUF_s *pPyramidBuf,
                                             const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)NumViewZone;
    (void)pViewZoneId;
    (void)pPyramid;
    (void)pPyramidBuf;
    (void)pAttachedRawSeq;

    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewUpdatePymdCfg Resource Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Resource_LiveviewFeedRawData(const UINT16 NumViewZone,
                                           const UINT16 *pViewZoneId,
                                           const AMBA_DSP_LIVEVIEW_EXT_RAW_s *pExtBuf)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)NumViewZone;
    (void)pViewZoneId;
    (void)pExtBuf;

    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewFeedRawData Resource Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Resource_LiveviewFeedYuvData(const UINT16 NumViewZone,
                                           const UINT16 *pViewZoneId,
                                           const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)NumViewZone;
    (void)pViewZoneId;
    (void)pExtYuvBuf;

    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewFeedYuvData Resource Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Resource_LiveviewYuvStreamSync(const UINT16 YuvStrmIdx,
                                             const AMBA_DSP_LIVEVIEW_SYNC_CTRL_s *pYuvStrmSyncCtrl,
                                             const UINT32 *pSyncJobId,
                                             const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)YuvStrmIdx;
    (void)pYuvStrmSyncCtrl;
    (void)pSyncJobId;
    (void)pAttachedRawSeq;
    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewYuvStreamSync Resource Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 Resource_LiveviewSideBandUpdate(const UINT16 ViewZoneId, const UINT8 NumBand, const ULONG *pSidebandBufAddr)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)ViewZoneId;
    (void)NumBand;
    (void)pSidebandBufAddr;
    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LivewviewSideBandUpdate Resource Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 Resource_LiveviewUpdateVinState(const UINT16 NumVin, const AMBA_DSP_LV_VIN_STATE_s *pVinState)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)NumVin;
    (void)pVinState;

    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewUpdateVinState Resource Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/* StillCapture */
static UINT32 Resource_UpdateCapBuffer(const UINT16 CapInstance,
                                       const AMBA_DSP_DATACAP_BUF_CFG_s *pCapBuf,
                                       const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)CapInstance;
    (void)pCapBuf;
    (void)pAttachedRawSeq;

    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("UpdateCapBuffer Resource Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 Resource_DataCapCtrl(const UINT16 NumCapInstance,
                                   const UINT16 *pCapInstance,
                                   const AMBA_DSP_DATACAP_CTRL_s *pDataCapCtrl,
                                   const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)NumCapInstance;
    (void)pCapInstance;
    (void)pDataCapCtrl;
    (void)pAttachedRawSeq;

    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("DataCapCtrl Resource Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 Resource_StillYuv2Yuv(const AMBA_DSP_YUV_IMG_BUF_s *pYuvIn,
                                    const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                                    const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                    const UINT32 Opt,
                                    const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)pYuvIn;
    (void)pYuvOut;
    (void)pIsoCfg;
    (void)Opt;
    (void)pAttachedRawSeq;

    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillYuv2Yuv Resource Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 Resource_StillEncodeCtrl(const UINT16 StreamIdx,
                                       const AMBA_DSP_STLENC_CTRL_s *pStlEncCtrl,
                                       const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)StreamIdx;
    (void)pStlEncCtrl;
    (void)pAttachedRawSeq;
    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillEncodeCtrl Resource Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 Resource_StillRaw2Yuv(const AMBA_DSP_RAW_BUF_s *pRawIn,
                                    const AMBA_DSP_BUF_s *pAuxBufIn,
                                    const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                                    const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                    const UINT32 Opt,
                                    const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)pRawIn;
    (void)pAuxBufIn;
    (void)pYuvOut;
    (void)pIsoCfg;
    (void)Opt;
    (void)pAttachedRawSeq;
    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillRaw2Yuv Resource Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 Resource_StillRaw2Raw(const AMBA_DSP_RAW_BUF_s *pRawIn,
                                    const AMBA_DSP_BUF_s *pAuxBufIn,
                                    const AMBA_DSP_RAW_BUF_s *pRawOut,
                                    const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                    const UINT32 Opt,
                                    const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)pRawIn;
    (void)pAuxBufIn;
    (void)pRawOut;
    (void)pIsoCfg;
    (void)Opt;
    (void)pAttachedRawSeq;
    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillRaw2Raw Resource Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/* VideoEncode */
static UINT32 Resource_VideoEncStart(const UINT16 NumStream,
                                     const UINT16 *pStreamIdx,
                                     const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStartConfig,
                                     const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U, HEVC = 0U;
    UINT32 CheckFeature = ((UINT32)AMBA_DSP_CHK_DSP | (UINT32)AMBA_DSP_CHK_CODEC);

    (void)pStartConfig;
    (void)pAttachedRawSeq;

    if (Resource_DspEncHevcCheck(NumStream, pStreamIdx, &HEVC) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval == OK) {
        if (HEVC == 1U) {
            CheckFeature |= (UINT32)AMBA_DSP_CHK_HEVC;
        }
        if (Resource_Check(CheckFeature) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncStart Resource Error[%x][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Resource_VideoEncStop(const UINT16 NumStream,
                                    const UINT16 *pStreamIdx,
                                    const UINT8 *pStopOption,
                                    const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U, HEVC = 0U;
    UINT32 CheckFeature = ((UINT32)AMBA_DSP_CHK_DSP | (UINT32)AMBA_DSP_CHK_CODEC);

    (void)pStopOption;
    (void)pAttachedRawSeq;

    if (Resource_DspEncHevcCheck(NumStream, pStreamIdx, &HEVC) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval == OK) {
        if (HEVC == 1U) {
            CheckFeature |= (UINT32)AMBA_DSP_CHK_HEVC;
        }
        if (Resource_Check(CheckFeature) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncStop Resource Error[%x][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Resource_VideoEncControlFrameRate(const UINT16 NumStream,
                                                const UINT16 *pStreamIdx,
                                                const UINT32 *pDivisor,
                                                const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U, HEVC = 0U;
    UINT32 CheckFeature = ((UINT32)AMBA_DSP_CHK_DSP | (UINT32)AMBA_DSP_CHK_CODEC);

    (void)pDivisor;
    (void)pAttachedRawSeq;

    if (Resource_DspEncHevcCheck(NumStream, pStreamIdx, &HEVC) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval == OK) {
        if (HEVC == 1U) {
            CheckFeature |= (UINT32)AMBA_DSP_CHK_HEVC;
        }
        if (Resource_Check(CheckFeature) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncCtrlFrate Resource Error[%x][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Resource_VideoEncControlQuality(const UINT16 NumStream,
                                              const UINT16 *pStreamIdx,
                                              const AMBA_DSP_VIDEO_ENC_QUALITY_s *pQCtrl)
{
    UINT32 Rval = OK, ErrLine = 0U, HEVC = 0U;
    UINT32 CheckFeature = ((UINT32)AMBA_DSP_CHK_DSP | (UINT32)AMBA_DSP_CHK_CODEC);

    (void)pQCtrl;

    if (Resource_DspEncHevcCheck(NumStream, pStreamIdx, &HEVC) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval == OK) {
        if (HEVC == 1U) {
            CheckFeature |= (UINT32)AMBA_DSP_CHK_HEVC;
        }
        if (Resource_Check(CheckFeature) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncCtrlQt Resource Error[%x][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Resource_VideoEncControlBlend(const UINT16 NumStream,
                                            const UINT16 *pStreamIdx,
                                            const AMBA_DSP_VIDEO_BLEND_CONFIG_s *pBlendCfg)
{
    UINT32 Rval = OK, ErrLine = 0U, HEVC = 0U;
    UINT32 CheckFeature = ((UINT32)AMBA_DSP_CHK_DSP | (UINT32)AMBA_DSP_CHK_CODEC);

    (void)pBlendCfg;

    if (Resource_DspEncHevcCheck(NumStream, pStreamIdx, &HEVC) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval == OK) {
        if (HEVC == 1U) {
            CheckFeature |= (UINT32)AMBA_DSP_CHK_HEVC;
        }
        if (Resource_Check(CheckFeature) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncControlBlend Resource Error[%x][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 Resource_VideoEncFeedYuvData(const UINT16 NumStream,
                                           const UINT16 *pStreamIdx,
                                           const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    UINT32 Rval = OK, ErrLine = 0U, HEVC = 0U;
    UINT32 CheckFeature = ((UINT32)AMBA_DSP_CHK_DSP | (UINT32)AMBA_DSP_CHK_CODEC);

    (void)pExtYuvBuf;

    if (Resource_DspEncHevcCheck(NumStream, pStreamIdx, &HEVC) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval == OK) {
        if (HEVC == 1U) {
            CheckFeature |= (UINT32)AMBA_DSP_CHK_HEVC;
        }
        if (Resource_Check(CheckFeature) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncFeedYuvData Resource Error[%x][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

/* Vout */
static UINT32 Resource_VoutReset(const UINT8 VoutIdx)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)VoutIdx;
    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutReset Resource Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Resource_VoutVideoCtrl(const UINT8 VoutIdx,
                                     const UINT8 Enable,
                                     const UINT8 SyncWithVin,
                                     const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)VoutIdx;
    (void)Enable;
    (void)SyncWithVin;
    (void)pAttachedRawSeq;
    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutVideoCtrl Resource Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Resource_VoutOsdCtrl(const UINT8 VoutIdx,
                                   const UINT8 Enable,
                                   const UINT8 SyncWithVin,
                                   const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)VoutIdx;
    (void)Enable;
    (void)SyncWithVin;
    (void)pAttachedRawSeq;
    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutOsdCtrl Resource Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Resource_VoutDisplayCtrl(const UINT8 VoutIdx)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)VoutIdx;
    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutDisplayCtrl Resource Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Resource_VoutMixerCtrl(const UINT8 VoutIdx)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)VoutIdx;
    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutMixerCtrl Resource Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Resource_VoutDisplayControlGamma(const UINT8 VoutIdx, const UINT8 Enable)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)VoutIdx;
    (void)Enable;
    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutDisplayControlGamma Resource Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Resource_VoutDveCtrl(const UINT8 VoutIdx)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)VoutIdx;
    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutDveCtrl Resource Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

/* StillDecode */
static UINT32 Resource_StillDecStart(const UINT16 StreamIdx, const AMBA_DSP_STLDEC_START_CONFIG_s *pDecConfig)
{
    UINT32 Rval = OK;
    UINT32 ErrLine = 0U;

    (void)StreamIdx;
    (void)pDecConfig;
    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (Resource_Check((UINT32)AMBA_DSP_CHK_CODEC) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else {
        if (Resource_Check((UINT32)AMBA_DSP_CHK_HEVC) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillDecStart param resource NG at %d", ErrLine, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Resource_StillDecYuv2Yuv(const AMBA_DSP_YUV_IMG_BUF_s *pSrcYuvBufAddr,
                                       const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                                       const AMBA_DSP_STLDEC_YUV2YUV_s *pOperation)
{
    UINT32 Rval = OK;
    UINT32 ErrLine = 0U;

    (void)pSrcYuvBufAddr;
    (void)pDestYuvBufAddr;
    (void)pOperation;
    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillDecYuv2Yuv param resource NG at %d", ErrLine, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Resource_StillDecYuvBlend(const AMBA_DSP_YUV_IMG_BUF_s *pSrc1YuvBufAddr,
                                        const AMBA_DSP_YUV_IMG_BUF_s *pSrc2YuvBufAddr,
                                        const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                                        const AMBA_DSP_STLDEC_BLEND_s *pOperation)
{
    UINT32 Rval = OK;
    UINT32 ErrLine = 0U;

    (void)pSrc1YuvBufAddr;
    (void)pSrc2YuvBufAddr;
    (void)pDestYuvBufAddr;
    (void)pOperation;
    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillDecYuvBlend param resource NG at %d", ErrLine, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Resource_StillDecDispYuvImg(const UINT8 VoutIdx,
                                       const AMBA_DSP_YUV_IMG_BUF_s *pYuvBufAddr,
                                       const AMBA_DSP_VOUT_VIDEO_CFG_s *pVoutConfig)
{
    UINT32 Rval = OK;
    UINT32 ErrLine = 0U;

    (void)VoutIdx;
    (void)pYuvBufAddr;
    (void)pVoutConfig;
    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillDecDispYuvImg param resource NG at %d", ErrLine, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

/* VideoDecode */
static UINT32 Resource_VideoDecStart(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig)
{
    UINT32 Rval = OK, ErrLine = 0U, HEVC = 0U;
    UINT32 CheckFeature = ((UINT32)AMBA_DSP_CHK_DSP | (UINT32)AMBA_DSP_CHK_CODEC);

    (void)pStartConfig;

    if (Resource_DspDecHevcCheck(NumStream, pStreamIdx, &HEVC) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval == OK) {
        if (HEVC == 1U) {
            CheckFeature |= (UINT32)AMBA_DSP_CHK_HEVC;
        }
        if (Resource_Check(CheckFeature) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoDecStart param resource NG at %d",ErrLine, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 Resource_VideoDecPostCtrl(const UINT16 StreamIdx, const UINT16 NumPostCtrl, const AMBA_DSP_VIDDEC_POST_CTRL_s *pPostCtrl)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)StreamIdx;
    (void)NumPostCtrl;
    (void)pPostCtrl;

    if (Resource_Check((UINT32)AMBA_DSP_CHK_DSP) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoDecPostCtrl param resource NG at %d",ErrLine, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 Resource_VideoDecBitsFifoUpdate(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_BITS_FIFO_s *pBitsFifo)
{
    UINT32 Rval = OK, ErrLine = 0U, HEVC = 0U;
    UINT32 CheckFeature = ((UINT32)AMBA_DSP_CHK_DSP | (UINT32)AMBA_DSP_CHK_CODEC);

    (void)pBitsFifo;

    if (Resource_DspDecHevcCheck(NumStream, pStreamIdx, &HEVC) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval == OK) {
        if (HEVC == 1U) {
            CheckFeature |= (UINT32)AMBA_DSP_CHK_HEVC;
        }
        if (Resource_Check(CheckFeature) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoDecBitsFifoUpdate param resource NG at %d",ErrLine, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 Resource_VideoDecStop(const UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pShowLastFrame)
{
    UINT32 Rval = OK, ErrLine = 0U, HEVC = 0U;
    UINT32 CheckFeature = ((UINT32)AMBA_DSP_CHK_DSP | (UINT32)AMBA_DSP_CHK_CODEC);

    (void)pShowLastFrame;

    if (Resource_DspDecHevcCheck(NumStream, pStreamIdx, &HEVC) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval == OK) {
        if (HEVC == 1U) {
            CheckFeature |= (UINT32)AMBA_DSP_CHK_HEVC;
        }
        if (Resource_Check(CheckFeature) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoDecStop param resource NG at %d",ErrLine, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 Resource_VideoDecTrickPlay(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_TRICKPLAY_s *pTrickPlay)
{
    UINT32 Rval = OK, ErrLine = 0U, HEVC = 0U;
    UINT32 CheckFeature = ((UINT32)AMBA_DSP_CHK_DSP | (UINT32)AMBA_DSP_CHK_CODEC);

    (void)pTrickPlay;

    if (Resource_DspDecHevcCheck(NumStream, pStreamIdx, &HEVC) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval == OK) {
        if (HEVC == 1U) {
            CheckFeature |= (UINT32)AMBA_DSP_CHK_HEVC;
        }
        if (Resource_Check(CheckFeature) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoDecTrickPlay param resource NG at %d", ErrLine, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

void ResourceApiReg(const UINT8 Disable, const UINT32 Mask, const UINT8 IsAutoInit)
{
    static UINT8 IsUserSetup = 0U;
    static Dsp_ResourceApi_t DspApiResourceFunc = {
            /* Main */
            .pMainInit = Resource_MainInit,

            /* Liveview */
            .pLiveviewCtrl = Resource_LiveviewCtrl,
            .pLiveviewUpdateIsoCfg = Resource_LiveviewUpdateIsoCfg,
            .pLiveviewUpdateVinCfg = Resource_LiveviewUpdateVinCfg,
            .pLiveviewUpdatePymdCfg = Resource_LiveviewUpdatePymdCfg,
            .pLiveviewFeedRawData = Resource_LiveviewFeedRawData,
            .pLiveviewFeedYuvData = Resource_LiveviewFeedYuvData,
            .pLiveviewYuvStreamSync = Resource_LiveviewYuvStreamSync,
            .pLiveviewSideBandUpdate = Resource_LiveviewSideBandUpdate,
            .pLiveviewUpdateVinState = Resource_LiveviewUpdateVinState,

            /* StillCapture */
            .pUpdateCapBuffer = Resource_UpdateCapBuffer,
            .pDataCapCtrl = Resource_DataCapCtrl,
            .pStillYuv2Yuv = Resource_StillYuv2Yuv,
            .pStillEncodeCtrl = Resource_StillEncodeCtrl,
            .pStillRaw2Yuv = Resource_StillRaw2Yuv,
            .pStillRaw2Raw = Resource_StillRaw2Raw,

            /* VideoEncode */
            .pVideoEncStart = Resource_VideoEncStart,
            .pVideoEncStop = Resource_VideoEncStop,
            .pVideoEncControlFrameRate = Resource_VideoEncControlFrameRate,
            .pVideoEncControlQuality = Resource_VideoEncControlQuality,
            .pVideoEncControlBlend = Resource_VideoEncControlBlend,
            .pVideoEncFeedYuvData = Resource_VideoEncFeedYuvData,

            /* Vout */
            .pVoutReset = Resource_VoutReset,
            .pVoutVideoCtrl = Resource_VoutVideoCtrl,
            .pVoutOsdCtrl = Resource_VoutOsdCtrl,
            .pVoutDisplayCtrl = Resource_VoutDisplayCtrl,
            .pVoutMixerCtrl = Resource_VoutMixerCtrl,
            .pVoutDisplayControlGamma = Resource_VoutDisplayControlGamma,
            .pVoutDveCtrl = Resource_VoutDveCtrl,

            /* StillDecode */
            .pStillDecStart = Resource_StillDecStart,
            .pStillDecYuv2Yuv = Resource_StillDecYuv2Yuv,
            .pStillDecYuvBlend = Resource_StillDecYuvBlend,
            .pStillDecDispYuvImg = Resource_StillDecDispYuvImg,

            /* VideoDecode */
            .pVideoDecStart = Resource_VideoDecStart,
            .pVideoDecPostCtrl = Resource_VideoDecPostCtrl,
            .pVideoDecBitsFifoUpdate = Resource_VideoDecBitsFifoUpdate,
            .pVideoDecStop = Resource_VideoDecStop,
            .pVideoDecTrickPlay = Resource_VideoDecTrickPlay,
    };
//FIXME, Misra
(void)Mask;

    if (IsAutoInit == 0U) {
        IsUserSetup = 1U;
        if (Disable == 0U) {
            pDspApiResourceFunc = &DspApiResourceFunc;
        } else {
            pDspApiResourceFunc = NULL;
        }
    } else if (IsUserSetup == 0U) {
        if (Disable == 0U) {
            pDspApiResourceFunc = &DspApiResourceFunc;
        } else {
            pDspApiResourceFunc = NULL;
        }
    } else {
        //
    }
}

Dsp_ResourceApi_t* AmbaDSP_GetResourceApiFunc(void){
    return pDspApiResourceFunc;
}
