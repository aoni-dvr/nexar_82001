/**
*  @file AmbaDSP_ApiTiming.h
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

#ifndef AMBA_DSP_API_TIMING_H
#define AMBA_DSP_API_TIMING_H

#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_StillCapture.h"
#include "AmbaDSP_Liveview.h"

typedef struct {
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

    /* StillCapture */
    UINT32 (*pStillYuv2Yuv)(const AMBA_DSP_YUV_IMG_BUF_s *pYuvIn, const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, const UINT32 Opt, const UINT64 *pAttachedRawSeq);
    UINT32 (*pStillRaw2Yuv)(const AMBA_DSP_RAW_BUF_s *pRawIn, const AMBA_DSP_BUF_s *pAuxBufIn, const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, const UINT32 Opt, const UINT64 *pAttachedRawSeq);
    UINT32 (*pStillRaw2Raw)(const AMBA_DSP_RAW_BUF_s *pRawIn, const AMBA_DSP_BUF_s *pAuxBufIn, const AMBA_DSP_RAW_BUF_s *pRawOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, const UINT32 Opt, const UINT64 *pAttachedRawSeq);
    UINT32 (*pStillEncodeCtrl)(const UINT16 StreamIdx, const AMBA_DSP_STLENC_CTRL_s *pStlEncCtrl, const UINT64 *pAttachedRawSeq);

    /* Liveview */
    UINT32 (*pLiveviewFeedRawData)(const UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_LIVEVIEW_EXT_RAW_s *pExtBuf);
    UINT32 (*pLiveviewFeedYuvData)(const UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf);
} Dsp_TimingApi_t;

void TimingApiReg(const UINT8 Disable, const UINT32 Mask, const UINT8 IsAutoInit);
Dsp_TimingApi_t* AmbaDSP_GetTimingApiFunc(void);

#endif  //AMBA_DSP_API_TIMING_H
