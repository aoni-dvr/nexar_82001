/**
*  @file AmbaDSP_ApiResource.h
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

#ifndef AMBA_DSP_API_RESOURCE_H
#define AMBA_DSP_API_RESOURCE_H

#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_VideoDec.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_StillDec.h"
#include "AmbaDSP_StillCapture.h"

typedef struct {
    /* Main */
    UINT32 (*pMainInit)(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig);

    /* Liveview */
    UINT32 (*pLiveviewCtrl)(const UINT16 NumViewZone, const UINT16 *pViewZoneId, const UINT8* pEnable);
    UINT32 (*pLiveviewUpdateIsoCfg)(const UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s *pIsoCfgCtrl, const UINT64 *pAttachedRawSeq);
    UINT32 (*pLiveviewUpdateVinCfg)(const UINT16 VinId, const UINT16 SubChNum, const AMBA_DSP_VIN_SUB_CHAN_s *pSubCh, const AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s *pLvVinCfgCtrl, const UINT64 *pAttachedRawSeq);
    UINT32 (*pLiveviewUpdatePymdCfg)(const UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_LIVEVIEW_PYRAMID_s *pPyramid, const AMBA_DSP_LIVEVIEW_YUV_BUF_s *pPyramidBuf, const UINT64 *pAttachedRawSeq);
    UINT32 (*pLiveviewFeedRawData)(const UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_LIVEVIEW_EXT_RAW_s *pExtBuf);
    UINT32 (*pLiveviewFeedYuvData)(const UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf);
    UINT32 (*pLiveviewYuvStreamSync)(const UINT16 YuvStrmIdx, const AMBA_DSP_LIVEVIEW_SYNC_CTRL_s *pYuvStrmSyncCtrl, const UINT32 *pSyncJobId, const UINT64 *pAttachedRawSeq);
    UINT32 (*pLiveviewSideBandUpdate)(const UINT16 ViewZoneId, const UINT8 NumBand, const ULONG *pSidebandBufAddr);
    UINT32 (*pLiveviewUpdateVinState)(const UINT16 NumVin, const AMBA_DSP_LV_VIN_STATE_s *pVinState);

    /* StillCapture */
    UINT32 (*pUpdateCapBuffer)(const UINT16 CapInstance, const AMBA_DSP_DATACAP_BUF_CFG_s *pCapBuf, const UINT64 *pAttachedRawSeq);
    UINT32 (*pDataCapCtrl)(const UINT16 NumCapInstance, const UINT16 *pCapInstance, const AMBA_DSP_DATACAP_CTRL_s *pDataCapCtrl, const UINT64 *pAttachedRawSeq);
    UINT32 (*pStillYuv2Yuv)(const AMBA_DSP_YUV_IMG_BUF_s *pYuvIn, const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, const UINT32 Opt, const UINT64 *pAttachedRawSeq);
    UINT32 (*pStillEncodeCtrl)(const UINT16 StreamIdx, const AMBA_DSP_STLENC_CTRL_s *pStlEncCtrl, const UINT64 *pAttachedRawSeq);
    UINT32 (*pStillRaw2Yuv)(const AMBA_DSP_RAW_BUF_s *pRawIn, const AMBA_DSP_BUF_s *pAuxBufIn, const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, const UINT32 Opt, const UINT64 *pAttachedRawSeq);
    UINT32 (*pStillRaw2Raw)(const AMBA_DSP_RAW_BUF_s *pRawIn, const AMBA_DSP_BUF_s *pAuxBufIn, const AMBA_DSP_RAW_BUF_s *pRawOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, const UINT32 Opt, const UINT64 *pAttachedRawSeq);

    /* VideoEncode */
    UINT32 (*pVideoEncStart)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStartConfig, const UINT64 *pAttachedRawSeq);
    UINT32 (*pVideoEncStop)(const UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pStopOption, const UINT64 *pAttachedRawSeq);
    UINT32 (*pVideoEncControlFrameRate)(const UINT16 NumStream, const UINT16 *pStreamIdx, const UINT32 *pDivisor, const UINT64 *pAttachedRawSeq);
    UINT32 (*pVideoEncControlQuality)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_QUALITY_s *pQCtrl);
    UINT32 (*pVideoEncControlBlend)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_BLEND_CONFIG_s *pBlendCfg);
    UINT32 (*pVideoEncFeedYuvData)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf);

    /* Vout */
    UINT32 (*pVoutReset)(const UINT8 VoutIdx);
    UINT32 (*pVoutVideoCtrl)(const UINT8 VoutIdx, const UINT8 Enable, const UINT8 SyncWithVin, const UINT64 *pAttachedRawSeq);
    UINT32 (*pVoutOsdCtrl)(const UINT8 VoutIdx, const UINT8 Enable, const UINT8 SyncWithVin, const UINT64 *pAttachedRawSeq);
    UINT32 (*pVoutDisplayCtrl)(const UINT8 VoutIdx);
    UINT32 (*pVoutMixerCtrl)(const UINT8 VoutIdx);
    UINT32 (*pVoutDisplayControlGamma)(const UINT8 VoutIdx, const UINT8 Enable);
    UINT32 (*pVoutDveCtrl)(const UINT8 VoutIdx);

    /* StillDecode */
    UINT32 (*pStillDecStart)(const UINT16 StreamIdx, const AMBA_DSP_STLDEC_START_CONFIG_s *pDecConfig);
    UINT32 (*pStillDecYuv2Yuv)(const AMBA_DSP_YUV_IMG_BUF_s *pSrcYuvBufAddr,
                             const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                             const AMBA_DSP_STLDEC_YUV2YUV_s *pOperation);

    /* VideoDecode */
    UINT32 (*pVideoDecStart)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig);
    UINT32 (*pVideoDecPostCtrl)(const UINT16 StreamIdx, const UINT16 NumPostCtrl, const AMBA_DSP_VIDDEC_POST_CTRL_s *pPostCtrl);
    UINT32 (*pVideoDecBitsFifoUpdate)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_BITS_FIFO_s *pBitsFifo);
    UINT32 (*pVideoDecStop)(const UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pShowLastFrame);
    UINT32 (*pVideoDecTrickPlay)(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_TRICKPLAY_s *pTrickPlay);

} Dsp_ResourceApi_t;

void ResourceApiReg(const UINT8 Disable, const UINT32 Mask, const UINT8 IsAutoInit);
Dsp_ResourceApi_t* AmbaDSP_GetResourceApiFunc(void);

#endif  //AMBA_DSP_API_RESOURCE_H
