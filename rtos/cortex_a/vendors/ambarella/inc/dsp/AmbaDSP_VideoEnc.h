/**
 *  @file AmbaDSP_VideoEnc.h
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for Ambarella DSP Driver Video Encode APIs
 */
#ifndef AMBA_DSP_VIDEO_ENC_H
#define AMBA_DSP_VIDEO_ENC_H

#include "AmbaDSP.h"
#include "AmbaDSP_VideoEnc_Def.h"
#include "AmbaDSP_EventInfo.h"

UINT32 AmbaDSP_VideoEncConfig(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pStreamConfig);

UINT32 AmbaDSP_VideoEncStart(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStartConfig, UINT64 *pAttachedRawSeq);

UINT32 AmbaDSP_VideoEncStop(UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pStopOption, UINT64 *pAttachedRawSeq);

UINT32 AmbaDSP_VideoEncControlFrameRate(UINT16 NumStream, const UINT16 *pStreamIdx, const UINT32 *pDivisor, UINT64 *pAttachedRawSeq);

UINT32 AmbaDSP_VideoEncControlRepeatDrop(UINT16 NumStream, const UINT16* pStreamIdx, const AMBA_DSP_VIDEO_REPEAT_DROP_CONFIG_s *pRepeatDropCfg, const UINT64* pAttachedRawSeq);

UINT32 AmbaDSP_VideoEncControlSlowShutter(UINT16 NumVin, const UINT16 *pVinIdx, const UINT32 *pUpSamplingRate, const UINT64 *pAttachedRawSeq);

UINT32 AmbaDSP_VideoEncControlBlend(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_BLEND_CONFIG_s *pBlendCfg);

UINT32 AmbaDSP_VideoEncControlQuality(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_QUALITY_s *pQCtrl);

UINT32 AmbaDSP_VideoEncExecIntervalCap(UINT16 NumStream, const UINT16 *pStreamIdx);

UINT32 AmbaDSP_VideoEncFeedYuvData(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf);

UINT32 AmbaDSP_VideoEncGrpConfig(const AMBA_DSP_VIDEO_ENC_GROUP_s *pGrpCfg);

UINT32 AmbaDSP_VideoEncMvConfig(UINT16 StreamIdx, const AMBA_DSP_VIDEO_ENC_MV_CFG_s *pMvCfg);

UINT32 AmbaDSP_VideoEncDescFmtConfig(UINT16 StreamIdx, UINT16 CatIdx, UINT32 OptVal);

#endif  /* AMBA_DSP_VIDEO_ENC_H */
