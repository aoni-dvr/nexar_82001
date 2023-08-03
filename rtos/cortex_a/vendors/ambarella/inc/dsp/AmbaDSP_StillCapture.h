/**
 *  @file AmbaDSP_StillCapture.h
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
 *  @details Definitions & Constants for Ambarella DSP Driver Still capture APIs
 *
 */
#ifndef AMBA_DSP_STLCAPTURE_H
#define AMBA_DSP_STLCAPTURE_H

#include "AmbaDSP_StillCapture_Def.h"


UINT32 AmbaDSP_DataCapCfg(UINT16 CapInstance, const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg);

UINT32 AmbaDSP_UpdateCapBuffer(UINT16 CapInstance, const AMBA_DSP_DATACAP_BUF_CFG_s *pCapBuf, UINT64 *pAttachedRawSeq);

UINT32 AmbaDSP_DataCapCtrl(UINT16 NumCapInstance, const UINT16 *pCapInstance, const AMBA_DSP_DATACAP_CTRL_s *pDataCapCtrl, UINT64 *pAttachedRawSeq);

UINT32 AmbaDSP_StillYuv2Yuv(const AMBA_DSP_YUV_IMG_BUF_s *pYuvIn, const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, UINT32 Opt, UINT64 *pAttachedRawSeq);

UINT32 AmbaDSP_StillEncodeCtrl(UINT16 StreamIdx, const AMBA_DSP_STLENC_CTRL_s *pStlEncCtrl, UINT64 *pAttachedRawSeq);

UINT32 AmbaDSP_CalcStillYuvExtBufSize(UINT16 StreamIdx, UINT16 BufType, UINT16 *pBufPitch, UINT32 *pBufUnitSize);

UINT32 AmbaDSP_StillRaw2Yuv(const AMBA_DSP_RAW_BUF_s *pRawIn,
                            const AMBA_DSP_BUF_s *pAuxBufIn,
                            const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                            const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                            UINT32 Opt,
                            UINT64 *pAttachedRawSeq);

UINT32 AmbaDSP_StillRaw2Raw(const AMBA_DSP_RAW_BUF_s *pRawIn,
                            const AMBA_DSP_BUF_s *pAuxBufIn,
                            const AMBA_DSP_RAW_BUF_s *pRawOut,
                            const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                            UINT32 Opt,
                            UINT64 *pAttachedRawSeq);

#endif  /* AMBA_DSP_STLCAPTURE_H */
