/**
*  @file AmbaDSP_ApiTiming.c
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
*  @details DSP API Timing Check.
*
*/

#include "AmbaDSP_Log.h"
#include "AmbaDSP_ApiTiming.h"
#include "AmbaDSP_ContextUtility.h"

static Dsp_TimingApi_t *pDspApiTimingFunc = NULL;

/* VideoEncode */
static UINT32 Timing_VideoEncConfig(const UINT16 NumStream,
                                    const UINT16 *pStreamIdx,
                                    const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pStreamConfig)
{
    UINT32 Rval = OK;
(void)NumStream;
(void)pStreamIdx;
(void)pStreamConfig;
    return Rval;
}

static UINT32 Timing_VideoEncStart(const UINT16 NumStream,
                                   const UINT16 *pStreamIdx,
                                   const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStartConfig,
                                   const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 Mask = 0U;
    UINT16 i;
    UINT8 EncState;
(void)pStartConfig;
(void)pAttachedRawSeq;

    HL_GetResourcePtr(&Resource);
    if (Resource->ParLoadEn == 1U) {
        Mask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
        if (Mask == 0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    /* Prevent user to stop start cmd during encode busy status */
    for (i = 0U; i < NumStream; i++) {
        EncState = DSP_GetEncState(pStreamIdx[i]);
        if (EncState == DSP_ENC_STATUS_BUSY) {
            Rval = DSP_ERR_0004; ErrLine = __LINE__; break;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncStart Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoEncStop(const UINT16 NumStream,
                                  const UINT16 *pStreamIdx,
                                  const UINT8 *pStopOption,
                                  const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 Mask = 0U;
    UINT16 i;
    UINT8 EncState;
(void)pStopOption;
(void)pAttachedRawSeq;

    HL_GetResourcePtr(&Resource);
    if (Resource->ParLoadEn == 1U) {
        Mask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
        if (Mask == 0U) {
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        }
    }

    /* Prevent user to stop encode cmd during encode idle status */
    for (i = 0U; i < NumStream; i++) {
        EncState = DSP_GetEncState(pStreamIdx[i]);
        if (EncState == DSP_ENC_STATUS_IDLE) {
            Rval = DSP_ERR_0004; ErrLine = __LINE__; break;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncStop Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoEncControlFrameRate(const UINT16 NumStream,
                                              const UINT16 *pStreamIdx,
                                              const UINT32 *pDivisor,
                                              const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 Mask = 0U;
(void)NumStream;
(void)pStreamIdx;
(void)pDivisor;
(void)pAttachedRawSeq;

    HL_GetResourcePtr(&Resource);
    if (Resource->ParLoadEn == 1U) {
        Mask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
        if (Mask == 0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncCtrlFrate Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoEncControlRepeatDrop(const UINT16 NumStream,
                                               const UINT16* pStreamIdx,
                                               const AMBA_DSP_VIDEO_REPEAT_DROP_CONFIG_s *pRepeatDropCfg,
                                               const UINT64* pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 Mask = 0U;
(void)NumStream;
(void)pStreamIdx;
(void)pRepeatDropCfg;
(void)pAttachedRawSeq;

    HL_GetResourcePtr(&Resource);
    if (Resource->ParLoadEn == 1U) {
        Mask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
        if (Mask == 0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncCtrlRepeatDrop Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoEncControlQuality(const UINT16 NumStream,
                                            const UINT16 *pStreamIdx,
                                            const AMBA_DSP_VIDEO_ENC_QUALITY_s *pQCtrl)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 Mask;
(void)NumStream;
(void)pStreamIdx;
(void)pQCtrl;
    HL_GetResourcePtr(&Resource);
    if (Resource->ParLoadEn == 1U) {
        Mask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
        if (Mask == 0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncCtrlQt Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoEncControlBlend(const UINT16 NumStream,
                                          const UINT16 *pStreamIdx,
                                          const AMBA_DSP_VIDEO_BLEND_CONFIG_s *pBlendCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 Mask = 0U;
(void)NumStream;
(void)pStreamIdx;
(void)pBlendCfg;
    HL_GetResourcePtr(&Resource);
    if (Resource->ParLoadEn == 1U) {
        Mask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
        if (Mask == 0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncControlBlend Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 Timing_VideoEncExecIntervalCap(const UINT16 NumStream,
                                             const UINT16 *pStreamIdx)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 Mask = 0U;
(void)NumStream;
(void)pStreamIdx;
    HL_GetResourcePtr(&Resource);
    if (Resource->ParLoadEn == 1U) {
        Mask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
        if (Mask == 0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncExecIntCap Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoEncFeedYuvData(const UINT16 NumStream,
                                         const UINT16 *pStreamIdx,
                                         const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 Mask = 0U;
(void)NumStream;
(void)pStreamIdx;
(void)pExtYuvBuf;
    HL_GetResourcePtr(&Resource);
    if (Resource->ParLoadEn == 1U) {
        Mask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
        if (Mask == 0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncFeedYuvData Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoEncGrpConfig(const AMBA_DSP_VIDEO_ENC_GROUP_s *pGrpCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 Mask = 0U;
(void)pGrpCfg;
    HL_GetResourcePtr(&Resource);
    if (Resource->ParLoadEn == 1U) {
        Mask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
        if (Mask == 0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncRcGrpConfig Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoEncMvConfig(const UINT16 StreamIdx, const AMBA_DSP_VIDEO_ENC_MV_CFG_s *pMvCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 Mask = 0U;
(void)StreamIdx;
(void)pMvCfg;
    HL_GetResourcePtr(&Resource);
    if (Resource->ParLoadEn == 1U) {
        Mask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
        if (Mask == 0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncMvConfig Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoEncDescFmtConfig(const UINT16 StreamIdx, const UINT16 CatIdx, const UINT32 OptVal)
{
    UINT32 Rval = OK;
(void)StreamIdx;
(void)CatIdx;
(void)OptVal;
    return Rval;
}

/* StillCapture */
static UINT32 Timing_StillYuv2Yuv(const AMBA_DSP_YUV_IMG_BUF_s *pYuvIn,
                                  const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                                  const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                  const UINT32 Opt,
                                  const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT16 VprocIdx;
    CTX_STILL_INFO_s StlInfo = {0};
(void)pYuvIn;
(void)pYuvOut;
(void)pIsoCfg;
(void)Opt;
(void)pAttachedRawSeq;

    HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
#ifdef SUPPORT_VPROC_RT_RESTART
    VprocIdx = StlInfo.YuvInVprocId;
    if ((StlInfo.RawInVprocId == VprocIdx) && (StlInfo.RawInVprocStatus == STL_VPROC_STATUS_RUN)) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    } else if ((StlInfo.YuvInVprocId == VprocIdx) && (StlInfo.YuvInVprocStatus == STL_VPROC_STATUS_RUN)) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    } else {
        //TBD
    }
#else
    VprocIdx = (pYuvIn->DataFmt == AMBA_DSP_YUV420) ? StlInfo.YuvInVprocId: StlInfo.Yuv422InVprocId);
    if ((StlInfo.RawInVprocId == VprocIdx) && (StlInfo.RawInVprocStatus == STL_VPROC_STATUS_RUN)) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    } else if ((StlInfo.YuvInVprocId == VprocIdx) && (StlInfo.YuvInVprocStatus == STL_VPROC_STATUS_RUN)) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    } else if ((StlInfo.Yuv422InVprocId == VprocIdx) && (StlInfo.Yuv422InVprocStatus == STL_VPROC_STATUS_RUN)) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    } else {
        //TBD
    }
#endif

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillYuv2Yuv Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_StillRaw2Yuv(const AMBA_DSP_RAW_BUF_s *pRawIn, const AMBA_DSP_BUF_s *pAuxBufIn, const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, const UINT32 Opt, const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT16 VprocIdx ;
    CTX_STILL_INFO_s StlInfo = {0};
(void)pRawIn;
(void)pAuxBufIn;
(void)pYuvOut;
(void)pIsoCfg;
(void)Opt;
(void)pAttachedRawSeq;

    HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
    VprocIdx = StlInfo.RawInVprocId;

    if ((StlInfo.RawInVprocId == VprocIdx) && (StlInfo.RawInVprocStatus == STL_VPROC_STATUS_RUN)) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    } else if ((StlInfo.YuvInVprocId == VprocIdx) && (StlInfo.YuvInVprocStatus == STL_VPROC_STATUS_RUN)) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
#ifndef SUPPORT_VPROC_RT_RESTART
    } else if ((StlInfo.Yuv422InVprocId == VprocIdx) && (StlInfo.Yuv422InVprocStatus == STL_VPROC_STATUS_RUN)) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
#endif
    } else {
        //TBD
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillRaw2Yuv Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_StillRaw2Raw(const AMBA_DSP_RAW_BUF_s *pRawIn, const AMBA_DSP_BUF_s *pAuxBufIn, const AMBA_DSP_RAW_BUF_s *pYuvOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, const UINT32 Opt, const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT16 VprocIdx;
    CTX_STILL_INFO_s StlInfo = {0};
(void)pRawIn;
(void)pAuxBufIn;
(void)pYuvOut;
(void)pIsoCfg;
(void)Opt;
(void)pAttachedRawSeq;

    HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
    VprocIdx = StlInfo.RawInVprocId;

    if ((StlInfo.RawInVprocId == VprocIdx) && (StlInfo.RawInVprocStatus == STL_VPROC_STATUS_RUN)) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    } else if ((StlInfo.YuvInVprocId == VprocIdx) && (StlInfo.YuvInVprocStatus == STL_VPROC_STATUS_RUN)) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
#ifndef SUPPORT_VPROC_RT_RESTART
    } else if ((StlInfo.Yuv422InVprocId == VprocIdx) && (StlInfo.Yuv422InVprocStatus == STL_VPROC_STATUS_RUN)) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
#endif
    } else {
        //TBD
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillRaw2Raw Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_StillEncodeCtrl(const UINT16 StreamIdx,
                                     const AMBA_DSP_STLENC_CTRL_s *pStlEncCtrl,
                                     const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_STILL_INFO_s StlInfo = {0};
(void)StreamIdx;
(void)pStlEncCtrl;
(void)pAttachedRawSeq;

    HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
    if ((StlInfo.EncStatus != STL_ENC_STATUS_IDLE) && (StlInfo.EncStatus != STL_ENC_STATUS_COMPLETE)) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillEncodeCtrl Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_LiveviewFeedRawData(const UINT16 NumViewZone,
                                         const UINT16 *pViewZoneId,
                                         const AMBA_DSP_LIVEVIEW_EXT_RAW_s *pExtBuf)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT16 Idx, VinId;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    for (Idx = 0U; Idx < NumViewZone; Idx++) {
        HL_GetViewZoneInfoPtr(pViewZoneId[Idx], &ViewZoneInfo);

        if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM) {
            VinId = (UINT16)DSP_GetU16Bit(pExtBuf[Idx].VinId, 0U, 15U);
            if (1U == DSP_GetU16Bit(pExtBuf[Idx].VinId, VIN_VIRT_IDX, 1U)) {
                VinId += AMBA_DSP_MAX_VIN_NUM;
            }
            if (DSP_CheckCmdBufLock(VinId) != OK) {
                Rval = DSP_ERR_0004; ErrLine = __LINE__;
                break;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewFeedRawData Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_LiveviewFeedYuvData(const UINT16 NumViewZone,
                                         const UINT16 *pViewZoneId,
                                         const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT16 Idx, VinId, DecIdx = 0U;
    UINT8 RescChanged = (HL_GetRescState() == HL_RESC_CONFIGED)? 1U: 0U;
    UINT8 VprocState;
    CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VPROC_INFO_s VprocInfo = {0};
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    UINT8 ExitLoop = (UINT8)0U;
(void)pExtYuvBuf;

    for (Idx = 0U; Idx < NumViewZone; Idx++) {
        HL_GetViewZoneInfoPtr(pViewZoneId[Idx], &pViewZoneInfo);
        if ((pViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420) ||
            (pViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422)) {
            HL_GetVprocInfo(HL_MTX_OPT_ALL, pViewZoneId[Idx], &VprocInfo);

            if ((VprocInfo.Status != DSP_VPROC_STATUS_IDLE2RUN) &&
                ((RescChanged == 1U) ||
                 (DSP_VPROC_STATUS_ACTIVE != VprocInfo.Status))) {
                // DO NOTHING
            } else {
                DSP_Bit2U16Idx(pViewZoneInfo->SourceVin, &VinId);

                if (DSP_CheckCmdBufLock(VinId) != OK) {
                    ExitLoop = (UINT8)1U; ErrLine = __LINE__;
                }
            }
        } else if (pViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {
            VprocState = DSP_GetVprocState((UINT8)pViewZoneId[Idx]);

            if ((RescChanged == 1U) ||
                (DSP_VPROC_STATUS_ACTIVE != VprocState)) {
                // DO NOTHING
            } else {
                DSP_Bit2U16Idx(pViewZoneInfo->SourceVin, &DecIdx);
                HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecIdx, &VidDecInfo);
                VinId = VidDecInfo.YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM;

                if (DSP_CheckCmdBufLock(VinId) != OK) {
                    ExitLoop = (UINT8)1U; ErrLine = __LINE__;
                }
            }
        } else {
            // DO NOTHING
        }

        if (ExitLoop == (UINT8)1U) {
            Rval = DSP_ERR_0004;
            break;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("Timing_LiveviewFeedYuvData Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

void TimingApiReg(const UINT8 Disable, const UINT32 Mask, const UINT8 IsAutoInit)
{
    static UINT8 IsUserSetup = 0U;
    static Dsp_TimingApi_t DspApiTimingFunc = {
        /* VideoEncode */
        .pVideoEncConfig = Timing_VideoEncConfig,
        .pVideoEncStart = Timing_VideoEncStart,
        .pVideoEncStop = Timing_VideoEncStop,
        .pVideoEncControlFrameRate = Timing_VideoEncControlFrameRate,
        .pVideoEncControlRepeatDrop = Timing_VideoEncControlRepeatDrop,
        .pVideoEncControlQuality = Timing_VideoEncControlQuality,
        .pVideoEncControlBlend = Timing_VideoEncControlBlend,
        .pVideoEncExecIntervalCap = Timing_VideoEncExecIntervalCap,
        .pVideoEncFeedYuvData = Timing_VideoEncFeedYuvData,
        .pVideoEncGrpConfig = Timing_VideoEncGrpConfig,
        .pVideoEncMvConfig = Timing_VideoEncMvConfig,
        .pVideoEncDescFmtConfig = Timing_VideoEncDescFmtConfig,

        /* StillCapture */
        .pStillYuv2Yuv = Timing_StillYuv2Yuv,
        .pStillRaw2Yuv = Timing_StillRaw2Yuv,
        .pStillRaw2Raw = Timing_StillRaw2Raw,
        .pStillEncodeCtrl = Timing_StillEncodeCtrl,

        /* Liveview */
        .pLiveviewFeedRawData = Timing_LiveviewFeedRawData,
        .pLiveviewFeedYuvData = Timing_LiveviewFeedYuvData,
    };
//FIXME, Misra
(void)Mask;

    if (IsAutoInit == 0U) {
        IsUserSetup = 1U;
        if (Disable == 0U) {
            pDspApiTimingFunc = &DspApiTimingFunc;
        } else {
            pDspApiTimingFunc = NULL;
        }
    } else if (IsUserSetup == 0U) {
        if (Disable == 0U) {
            pDspApiTimingFunc = &DspApiTimingFunc;
        } else {
            pDspApiTimingFunc = NULL;
        }
    } else {
        //
    }
}

Dsp_TimingApi_t* AmbaDSP_GetTimingApiFunc(void){
    return pDspApiTimingFunc;
}
