/**
*  @file AmbaDSP_ApiDump.h
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
*  @details DSP command message dump defines.
*
*/

#ifndef AMBA_DUMP_DSP_API_H
#define AMBA_DUMP_DSP_API_H

#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_VideoDec.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_StillDec.h"
#include "AmbaDSP_StillCapture.h"
#include "ambadsp_ioctl.h"

typedef struct {
    /* Main */
    void (*pMainInit)(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig);
    void (*pMainGetDefaultSysCfg)(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig);
    void (*pMainSetWorkArea)(const ULONG WorkAreaAddr, const UINT32 WorkSize);
    void (*pMainGetDspVerInfo)(const AMBA_DSP_VERSION_INFO_s *pInfo);
    void (*pMainMsgParseEntry)(const UINT32 EntryArg);
    void (*pMainWaitVinInterrupt)(const UINT32 Flag, const UINT32 *pActualFlag, const UINT32 TimeOut);
    void (*pMainWaitVoutInterrupt)(const UINT32 Flag, const UINT32 *pActualFlag, const UINT32 TimeOut);
    void (*pMainWaitFlag)(const UINT32 Flag, const UINT32 *pActualFlag, const UINT32 TimeOut);
    void (*pMainResourceLimit)(const AMBA_DSP_RESOURCE_s *pResource);
    void (*pGetCmprRawBufInfo)(const UINT16 Width, const UINT16 CmprRate, const UINT16 *pRawWidth, const UINT16 *pRawPitch);
    void (*pParLoadConfig)(const UINT32 Enable, const UINT32 Data);
    void (*pParLoadRegionUnlock)(const UINT16 RegionIdx,const UINT16 SubRegionIdx);
    void (*pCalcEncMvBufInfo)(const UINT16 Width, const UINT16 Height, const UINT32 Option, const UINT32 *pBufSize);
    void (*pMainGetBufInfo)(const UINT32 Type, const AMBA_DSP_BUF_INFO_s *pBufInfo);

    /* Liveview */
    void (*pLiveviewConfig)(const UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s* pViewZoneCfg, const UINT16 NumYUVStream, const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg);
    void (*pLiveviewCtrl)(const UINT16 NumViewZone, const UINT16 *pViewZoneId, const UINT8* pEnable);
    void (*pLiveviewUpdateConfig)(const UINT16 NumYuvStream, const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYuvStrmCfg, const UINT64 *pAttachedRawSeq);
    void (*pLiveviewIsoConfigUpdate)(const UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s *pIsoCfgCtrl, const UINT64 *pAttachedRawSeq);
    void (*pLiveviewUpdateVinCfg)(const UINT16 VinId, const UINT16 SubChNum, const AMBA_DSP_VIN_SUB_CHAN_s *pSubCh, const AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s *pLvVinCfgCtrl, const UINT64 *pAttachedRawSeq);
    void (*pLiveviewUpdatePymdCfg)(const UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_LIVEVIEW_PYRAMID_s *pPyramid, const AMBA_DSP_LIVEVIEW_YUV_BUF_s *pPyramidBuf, const UINT64 *pAttachedRawSeq);
    void (*pLiveviewConfigVinCapture)(const UINT16 VinId, const UINT16 SubChNum, const AMBA_DSP_VIN_SUB_CHAN_CFG_s *pSubChCfg);
    void (*pLiveviewConfigVinPost)(const UINT8 Type, const UINT16 VinId);
    void (*pLiveviewFeedRawData)(const UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_LIVEVIEW_EXT_RAW_s *pExtBuf);
    void (*pLiveviewFeedYuvData)(const UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf);
    void (*pLiveviewGetIDspCfg)(const UINT16 ViewZoneId, const ULONG *CfgAddr);
    void (*pLiveviewSidebandUpdate)(const UINT16 ViewZoneId, const UINT8 NumBand, const ULONG *pSidebandBufAddr);
    void (*pLiveviewYuvStreamSync)(const UINT16 YuvStrmIdx, const AMBA_DSP_LIVEVIEW_SYNC_CTRL_s *pYuvStrmSyncCtrl, const UINT32 *pSyncJobId, const UINT64 *pAttachedRawSeq);
    void (*pLiveviewUpdateGeoCfg)(const UINT16 ViewZoneId, const AMBA_DSP_LIVEVIEW_GEOCFG_CTRL_s *pGeoCfgCtrl, const UINT64 *pAttachedRawSeq);
    void (*pLiveviewUpdateVZSrcCfg)(const UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_VZ_SRC_CFG_s *pVzSrcCfg, const UINT64 *pAttachedRawSeq);
    void (*pLiveviewSliceCfg)(const UINT16 ViewZoneId, const AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg);
    void (*pLiveviewSliceCfgCalc)(const UINT16 ViewZoneId, AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg);
    void (*pLiveviewVZPostponeCfg)(const UINT16 ViewZoneId, const AMBA_DSP_LV_VZ_POSTPONE_CFG_s *pVzPostPoneCfg);
    void (*pLiveviewUpdateVinState)(const UINT16 NumVin, const AMBA_DSP_LV_VIN_STATE_s *pVinState);
    void (*pLiveviewSlowShutterCtrl)(const UINT16 NumViewZone, const UINT16* pViewZoneId, const UINT32* pRatio);

    /* StillCapture */
    void (*pDataCapCfg)(const UINT16 CapInstance, const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg);
    void (*pUpdateCapBuffer)(const UINT16 CapInstance, const AMBA_DSP_DATACAP_BUF_CFG_s *pCapBuf, const UINT64 *pAttachedRawSeq);
    void (*pDataCapCtrl)(const UINT16 NumCapInstance, const UINT16 *pCapInstance, const AMBA_DSP_DATACAP_CTRL_s *pDataCapCtrl, const UINT64 *pAttachedRawSeq);
    void (*pStillYuv2Yuv)(const AMBA_DSP_YUV_IMG_BUF_s *pYuvIn, const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, const UINT32 Opt, const UINT64 *pAttachedRawSeq);
    void (*pStillEncodeCtrl)(const UINT16 StreamIdx, const AMBA_DSP_STLENC_CTRL_s *pStlEncCtrl, const UINT64 *pAttachedRawSeq);
    void (*pCalcStillYuvExtBufSize)(const UINT16 StreamIdx, const UINT16 BufType, const UINT16 *pBufPitch, const UINT32 *pBufUnitSize);
    void (*pStillRaw2Yuv)(const AMBA_DSP_RAW_BUF_s *pRawIn, const AMBA_DSP_BUF_s *pAuxBufIn, const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, const UINT32 Opt, const UINT64 *pAttachedRawSeq);
    void (*pStillRaw2Raw)(const AMBA_DSP_RAW_BUF_s *pRawIn, const AMBA_DSP_BUF_s *pAuxBufIn, const AMBA_DSP_RAW_BUF_s *pRawOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, const UINT32 Opt, const UINT64 *pAttachedRawSeq);

    /* VideoEncode */
    void (*pVideoEncConfig)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pStreamConfig);
    void (*pVideoEncStart)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStartConfig, const UINT64 *pAttachedRawSeq);
    void (*pVideoEncStop)(const UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pStopOption, const UINT64 *pAttachedRawSeq);
    void (*pVideoEncControlFrameRate)(const UINT16 NumStream, const UINT16 *pStreamIdx, const UINT32 *pDivisor, const UINT64 *pAttachedRawSeq);
    void (*pVideoEncControlRepeatDrop)(const UINT16 NumStream, const UINT16* pStreamIdx, const AMBA_DSP_VIDEO_REPEAT_DROP_CONFIG_s *pRepeatDropCfg, const UINT64* pAttachedRawSeq);
    void (*pVideoEncControlQuality)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_QUALITY_s *pQCtrl);
    void (*pVideoEncControlBlend)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_BLEND_CONFIG_s *pBlendCfg);
    void (*pVideoEncExecIntervalCap)(const UINT16 NumStream, const UINT16 *pStreamIdx);
    void (*pVideoEncFeedYuvData)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf);
    void (*pVideoEncGrpConfig)(const AMBA_DSP_VIDEO_ENC_GROUP_s *pGrpCfg);
    void (*pVideoEncMvConfig)(const UINT16 StreamIdx, const AMBA_DSP_VIDEO_ENC_MV_CFG_s *pMvCfg);
    void (*pVideoEncDescFmtConfig)(const UINT16 StreamIdx, const UINT16 CatIdx, const UINT32 OptVal);

    /* Vout */
    void (*pVoutReset)(const UINT8 VoutIdx);
    void (*pVoutVideoConfig)(const UINT8 VoutIdx, const AMBA_DSP_VOUT_VIDEO_CFG_s *pConfig);
    void (*pVoutVideoCtrl)(const UINT8 VoutIdx, const UINT8 Enable, const UINT8 SyncWithVin, const UINT64 *pAttachedRawSeq);
    void (*pVoutOsdConfigBuf)(const UINT8 VoutIdx, const AMBA_DSP_VOUT_OSD_BUF_CONFIG_s *pBufConfig);
    void (*pVoutOsdCtrl)(const UINT8 VoutIdx, const UINT8 Enable, const UINT8 SyncWithVin, const UINT64 *pAttachedRawSeq);
    void (*pVoutDisplayConfig)(const UINT8 VoutIdx, const AMBA_DSP_DISPLAY_CONFIG_s *pConfig);
    void (*pVoutDisplayCtrl)(const UINT8 VoutIdx);
    void (*pVoutMixerConfig)(const UINT8 VoutIdx, const AMBA_DSP_VOUT_MIXER_CONFIG_s *pConfig);
    void (*pVoutMixerConfigBackColor)(const UINT8 VoutIdx, const UINT32 BackColorYUV);
    void (*pVoutMixerConfigHighlightColor)(const UINT8 VoutIdx, const UINT8 LumaThreshold, const UINT32 HighlightColorYUV);
    void (*pVoutMixerConfigCsc)(const UINT8 VoutIdx, const UINT8 CscCtrl);
    void (*pVoutMixerCscMatrixConfig)(const UINT8 VoutIdx, const dsp_vout_csc_matrix_s *pCscMatrix);
    void (*pVoutMixerCtrl)(const UINT8 VoutIdx);
    void (*pVoutDisplayConfigGamma)(const UINT8 VoutIdx, const ULONG TableAddr);
    void (*pVoutDisplayControlGamma)(const UINT8 VoutIdx, const UINT8 Enable);
    void (*pVoutConfigMixerBinding)(const UINT8 NumVout, const UINT8 *pVoutIdx, const AMBA_DSP_VOUT_DATA_PATH_CFG_s *pConfig);
    void (*pVoutDveConfig)(const UINT8 VoutIdx, const UINT8 DevMode);
    void (*pVoutDveCtrl)(const UINT8 VoutIdx);

    /* StillDecode */
    void (*pStillDecStart)(const UINT16 StreamIdx, const AMBA_DSP_STLDEC_START_CONFIG_s *pDecConfig);
    void (*pStillDecYuv2Yuv)(const AMBA_DSP_YUV_IMG_BUF_s *pSrcYuvBufAddr,
                             const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                             const AMBA_DSP_STLDEC_YUV2YUV_s *pOperation);
    void (*pStillDecYuvBlend)(const AMBA_DSP_YUV_IMG_BUF_s *pSrc1YuvBufAddr,
                              const AMBA_DSP_YUV_IMG_BUF_s *pSrc2YuvBufAddr,
                              const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                              const AMBA_DSP_STLDEC_BLEND_s *pOperation);
    void (*pStillDecDispYuvImg)(const UINT8 VoutIdx,
                                const AMBA_DSP_YUV_IMG_BUF_s *pYuvBufAddr,
                                const AMBA_DSP_VOUT_VIDEO_CFG_s *pVoutConfig);

    /* VideoDecode */
    void (*pVideoDecConfig)(const UINT16 MaxNumStream, const AMBA_DSP_VIDDEC_STREAM_CONFIG_s *pStreamConfig);
    void (*pVideoDecStart)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig);
    void (*pVideoDecPostCtrl)(const UINT16 StreamIdx, const UINT16 NumPostCtrl, const AMBA_DSP_VIDDEC_POST_CTRL_s *pPostCtrl);
    void (*pVideoDecBitsFifoUpdate)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_BITS_FIFO_s *pBitsFifo);
    void (*pVideoDecStop)(const UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pShowLastFrame);
    void (*pVideoDecTrickPlay)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_TRICKPLAY_s *pTrickPlay);

} Dsp_DumpApi_t;

void DumpApiReg(const UINT8 Enable, const UINT32 Mask);
Dsp_DumpApi_t* AmbaDSP_GetDumpApiFunc(void);

#endif  //AMBA_DUMP_DSP_API_H
