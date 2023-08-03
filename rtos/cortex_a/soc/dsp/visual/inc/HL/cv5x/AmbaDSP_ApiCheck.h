/**
*  @file AmbaDSP_ApiCheck.h
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
*  @details DSP API check defines.
*
*/

#ifndef AMBA_CHECK_DSP_API_H
#define AMBA_CHECK_DSP_API_H

#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_VideoDec.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_StillDec.h"
#include "AmbaDSP_StillCapture.h"
#include "ambadsp_ioctl.h"

typedef struct {
    /* Main */
    UINT32 (*pMainInit)(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig);
    UINT32 (*pMainGetDefaultSysCfg)(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig);
    UINT32 (*pMainSetWorkArea)(ULONG WorkAreaAddr, const UINT32 WorkSize);
    UINT32 (*pMainGetDspVerInfo)(const AMBA_DSP_VERSION_INFO_s *Info);
    UINT32 (*pMainMsgParseEntry)(const UINT32 EntryArg);
    UINT32 (*pMainWaitVinInterrupt)(const UINT32 Flag, const UINT32 *ActualFlag, const UINT32 TimeOut);
    UINT32 (*pMainWaitVoutInterrupt)(const UINT32 Flag, const UINT32 *ActualFlag, const UINT32 TimeOut);
    UINT32 (*pMainWaitFlag)(const UINT32 Flag, const UINT32 *ActualFlag, const UINT32 TimeOut);
    UINT32 (*pMainResourceLimit)(const AMBA_DSP_RESOURCE_s *pResource);
    UINT32 (*pGetCmprRawBufInfo)(const UINT16 Width, const UINT16 CmprRate, const UINT16 *pRawWidth, const UINT16 *pRawPitch);
    UINT32 (*pParLoadConfig)(const UINT32 Enable, const UINT32 Data);
    UINT32 (*pParLoadRegionUnlock)(const UINT16 RegionIdx, const UINT16 SubRegionIdx);
    UINT32 (*pCalcEncMvBufInfo)(const UINT16 Width, const UINT16 Height, const UINT32 Option, const UINT32 *pBufSize);
    UINT32 (*pMainGetBufInfo)(const UINT32 Type, const AMBA_DSP_BUF_INFO_s *pBufInfo);
    UINT32 (*pSliceCfgCalc)(const UINT16 Id, const AMBA_DSP_WINDOW_DIMENSION_s *pIn, const AMBA_DSP_WINDOW_DIMENSION_s *pOut, const AMBA_DSP_SLICE_CFG_s *pSliceCfg);

    /* Liveview */
    UINT32 (*pLiveviewConfig)(const UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s* pViewZoneCfg, const UINT16 NumYUVStream, const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg);
    UINT32 (*pLiveviewCtrl)(const UINT16 NumViewZone, const UINT16 *pViewZoneId, const UINT8* pEnable);
    UINT32 (*pLiveviewUpdateConfig)(const UINT16 NumYuvStream, const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYuvStrmCfg, const UINT64 *pAttachedRawSeq);
    UINT32 (*pLiveviewUpdateIsoConfig)(const UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s *pIsoCfgCtrl, const UINT64 *pAttachedRawSeq);
    UINT32 (*pLiveviewUpdateVinCfg)(const UINT16 VinId, const UINT16 SubChNum, const AMBA_DSP_VIN_SUB_CHAN_s *pSubCh, const AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s *pLvVinCfgCtrl, const UINT64 *pAttachedRawSeq);
    UINT32 (*pLiveviewUpdatePymdCfg)(const UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_LIVEVIEW_PYRAMID_s *pPyramid, const AMBA_DSP_LIVEVIEW_YUV_BUF_s *pPyramidBuf, const UINT64 *pAttachedRawSeq);
    UINT32 (*pLiveviewConfigVinCapture)(const UINT16 VinId, const UINT16 SubChNum, const AMBA_DSP_VIN_SUB_CHAN_CFG_s *pSubChCfg);
    UINT32 (*pLiveviewConfigVinPost)(const UINT8 Type, const UINT16 VinId);
    UINT32 (*pLiveviewFeedRawData)(const UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_LIVEVIEW_EXT_RAW_s *pExtBuf);
    UINT32 (*pLiveviewFeedYuvData)(const UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf);
    UINT32 (*pLiveviewGetIDspCfg)(const UINT16 ViewZoneId, const ULONG *CfgAddr);
    UINT32 (*pLiveviewSidebandUpdate)(const UINT16 ViewZoneId, const UINT8 NumBand, const ULONG *pSidebandBufAddr);
    UINT32 (*pLiveviewYuvStreamSync)(const UINT16 YuvStrmIdx, const AMBA_DSP_LIVEVIEW_SYNC_CTRL_s *pYuvStrmSyncCtrl, const UINT32 *pSyncJobId, const UINT64 *pAttachedRawSeq);
    UINT32 (*pLiveviewUpdateGeoCfg)(const UINT16 ViewZoneId, const AMBA_DSP_LIVEVIEW_GEOCFG_CTRL_s *pGeoCfgCtrl, const UINT64 *pAttachedRawSeq);
    UINT32 (*pLiveviewUpdateVZSrcCfg)(const UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_VZ_SRC_CFG_s *pVzSrcCfg, const UINT64 *pAttachedRawSeq);
    UINT32 (*pLiveviewSliceCfg)(const UINT16 ViewZoneId, const AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg);
    UINT32 (*pLiveviewSliceCfgCalc)(const UINT16 ViewZoneId, const AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg);
    UINT32 (*pLiveviewVZPostponeCfg)(const UINT16 ViewZoneId, const AMBA_DSP_LV_VZ_POSTPONE_CFG_s *pVzPostPoneCfg);
    UINT32 (*pLiveviewUpdateVinState)(const UINT16 NumVin, const AMBA_DSP_LV_VIN_STATE_s *pVinState);
    UINT32 (*pLiveviewSlowShutterCtrl)(const UINT16 NumViewZone, const UINT16* pViewZoneId, const UINT32* pRatio);
    UINT32 (*pLiveviewDropRepeatCtrl)(const UINT16 NumViewZone, const UINT16* pViewZoneId, const AMBA_DSP_LIVEVIEW_DROP_REPEAT_s* pDropRptCfg);

    /* StillCapture */
    UINT32 (*pDataCapCfg)(const UINT16 CapInstance, const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg);
    UINT32 (*pUpdateCapBuffer)(const UINT16 CapInstance, const AMBA_DSP_DATACAP_BUF_CFG_s *pCapBuf, const UINT64 *pAttachedRawSeq);
    UINT32 (*pDataCapCtrl)(const UINT16 NumCapInstance, const UINT16 *pCapInstance, const AMBA_DSP_DATACAP_CTRL_s *pDataCapCtrl, const UINT64 *pAttachedRawSeq);
    UINT32 (*pStillYuv2Yuv)(const AMBA_DSP_YUV_IMG_BUF_s *pYuvIn, const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, const UINT32 Opt, const UINT64 *pAttachedRawSeq);
    UINT32 (*pStillEncodeCtrl)(const UINT16 StreamIdx, const AMBA_DSP_STLENC_CTRL_s *pStlEncCtrl, const UINT64 *pAttachedRawSeq);
    UINT32 (*pCalcStillYuvExtBufSize)(const UINT16 StreamIdx, const UINT16 BufType, const UINT16 *pBufPitch, const UINT32 *pBufUnitSize);
    UINT32 (*pStillRaw2Yuv)(const AMBA_DSP_RAW_BUF_s *pRawIn, const AMBA_DSP_BUF_s *pAuxBufIn, const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, const UINT32 Opt, const UINT64 *pAttachedRawSeq);
    UINT32 (*pStillRaw2Raw)(const AMBA_DSP_RAW_BUF_s *pRawIn, const AMBA_DSP_BUF_s *pAuxBufIn, const AMBA_DSP_RAW_BUF_s *pRawOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, const UINT32 Opt, const UINT64 *pAttachedRawSeq);

    /* VideoEncode */
    UINT32 (*pVideoEncConfig)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pStreamConfig);
    UINT32 (*pVideoEncStart)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStartConfig, const UINT64 *pAttachedRawSeq);
    UINT32 (*pVideoEncStop)(const UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pStopOption, const UINT64 *pAttachedRawSeq);
    UINT32 (*pVideoEncControlFrameRate)(const UINT16 NumStream, const UINT16 *pStreamIdx, const UINT32 *pDivisor, const UINT64 *pAttachedRawSeq);
    UINT32 (*pVideoEncControlRepeatDrop)(const UINT16 NumStream, const UINT16* pStreamIdx, const AMBA_DSP_VIDEO_REPEAT_DROP_CONFIG_s *pRepeatDropCfg, const UINT64* pAttachedRawSeq);
    UINT32 (*pVideoEncControlQuality)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_QUALITY_s *pQCtrl);
    UINT32 (*pVideoEncControlBlend)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_BLEND_CONFIG_s *pBlendCfg);
    UINT32 (*pVideoEncExecIntervalCap)(const UINT16 NumStream, const UINT16 *pStreamIdx);
    UINT32 (*pVideoEncFeedYuvData)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf);
    UINT32 (*pVideoEncGrpConfig)(const AMBA_DSP_VIDEO_ENC_GROUP_s *pGrpCfg);
    UINT32 (*pVideoEncMvConfig)(const UINT16 StreamIdx, const AMBA_DSP_VIDEO_ENC_MV_CFG_s *pMvCfg);
    UINT32 (*pVideoEncDescFmtConfig)(const UINT16 StreamIdx, const UINT16 CatIdx, const UINT32 OptVal);

    /* Vout */
    UINT32 (*pVoutReset)(const UINT8 VoutIdx);
    UINT32 (*pVoutVideoConfig)(const UINT8 VoutIdx, const AMBA_DSP_VOUT_VIDEO_CFG_s *pConfig);
    UINT32 (*pVoutVideoCtrl)(const UINT8 VoutIdx, const UINT8 Enable, const UINT8 SyncWithVin, const UINT64 *pAttachedRawSeq);
    UINT32 (*pVoutOsdConfigBuf)(const UINT8 VoutIdx, const AMBA_DSP_VOUT_OSD_BUF_CONFIG_s *pBufConfig);
    UINT32 (*pVoutOsdCtrl)(const UINT8 VoutIdx, const UINT8 Enable, const UINT8 SyncWithVin, const UINT64 *pAttachedRawSeq);
    UINT32 (*pVoutDisplayConfig)(const UINT8 VoutIdx, const AMBA_DSP_DISPLAY_CONFIG_s *pConfig);
    UINT32 (*pVoutDisplayCtrl)(const UINT8 VoutIdx);
    UINT32 (*pVoutMixerConfig)(const UINT8 VoutIdx, const AMBA_DSP_VOUT_MIXER_CONFIG_s *pConfig);
    UINT32 (*pVoutMixerConfigBackColor)(const UINT8 VoutIdx, const UINT32 BackColorYUV);
    UINT32 (*pVoutMixerConfigHighlightColor)(const UINT8 VoutIdx, const UINT8 LumaThreshold, const UINT32 HighlightColorYUV);
    UINT32 (*pVoutMixerConfigCsc)(const UINT8 VoutIdx, const UINT8 CscCtrl);
    UINT32 (*pVoutMixerCscMatrixConfig)(const UINT8 VoutIdx, const dsp_vout_csc_matrix_s *pCscMatrix);
    UINT32 (*pVoutMixerCtrl)(const UINT8 VoutIdx);
    UINT32 (*pVoutDisplayConfigGamma)(const UINT8 VoutIdx, ULONG TableAddr);
    UINT32 (*pVoutDisplayControlGamma)(const UINT8 VoutIdx, const UINT8 Enable);
    UINT32 (*pVoutConfigMixerBinding)(const UINT8 NumVout, const UINT8 *pVoutIdx, const AMBA_DSP_VOUT_DATA_PATH_CFG_s *pConfig);
    UINT32 (*pVoutDveConfig)(const UINT8 VoutIdx, const UINT8 DevMode);
    UINT32 (*pVoutDveCtrl)(const UINT8 VoutIdx);

    /* StillDecode */
    UINT32 (*pStillDecStart)(const UINT16 StreamIdx, const AMBA_DSP_STLDEC_START_CONFIG_s *pDecConfig);

    /* VideoDecode */
    UINT32 (*pVideoDecConfig)(const UINT16 MaxNumStream, const AMBA_DSP_VIDDEC_STREAM_CONFIG_s *pStreamConfig);
    UINT32 (*pVideoDecStart)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig);
    UINT32 (*pVideoDecPostCtrl)(const UINT16 StreamIdx, const UINT16 NumPostCtrl, const AMBA_DSP_VIDDEC_POST_CTRL_s *pPostCtrl);
    UINT32 (*pVideoDecBitsFifoUpdate)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_BITS_FIFO_s *pBitsFifo);
    UINT32 (*pVideoDecStop)(const UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pShowLastFrame);
    UINT32 (*pVideoDecTrickPlay)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_TRICKPLAY_s *pTrickPlay);

} Dsp_CheckApi_t;

void CheckApiReg(const UINT8 Disable, const UINT32 Mask, const UINT8 IsAutoInit);
Dsp_CheckApi_t* AmbaDSP_GetCheckApiFunc(void);

#endif  //AMBA_CHECK_DSP_API_H
