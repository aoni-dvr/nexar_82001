/**
 *  @file AmbaDSP_StillDec.h
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
 *  @details Definitions & Constants for Ambarella DSP Driver Still Picture Decoder APIs
 *
 */
#ifndef AMBA_DSP_STILL_DEC_H
#define AMBA_DSP_STILL_DEC_H

#include "AmbaDSP.h"
#include "AmbaDSP_StillDec_Def.h"
#include "AmbaDSP_VOUT.h"

UINT32 AmbaDSP_StillDecStart(UINT16 StreamIdx, const AMBA_DSP_STLDEC_START_CONFIG_s *pDecConfig);

UINT32 AmbaDSP_StillDecStop(void);

UINT32 AmbaDSP_StillDecYuv2Yuv(const AMBA_DSP_YUV_IMG_BUF_s *pSrcYuvBufAddr,
                               const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                               const AMBA_DSP_STLDEC_YUV2YUV_s *pOperation);

UINT32 AmbaDSP_StillDecYuvBlend(const AMBA_DSP_YUV_IMG_BUF_s *pSrc1YuvBufAddr,
                                const AMBA_DSP_YUV_IMG_BUF_s *pSrc2YuvBufAddr,
                                const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                                const AMBA_DSP_STLDEC_BLEND_s *pOperation);

UINT32 AmbaDSP_StillDecDispYuvImg(UINT8 VoutIdx,
                                  const AMBA_DSP_YUV_IMG_BUF_s *pYuvBufAddr,
                                  const AMBA_DSP_VOUT_VIDEO_CFG_s *pVoutConfig);

#endif  /* AMBA_DSP_STILL_DEC_H */
