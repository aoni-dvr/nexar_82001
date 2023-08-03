/**
 *  @file AmbaDSP_VOUT.h
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
 *  @details Definitions & Constants for DSP Driver VOUT APIs
 *
 */
#ifndef AMBA_DSP_VOUT_H
#define AMBA_DSP_VOUT_H

#include "AmbaDSP.h"
#include "AmbaDSP_VOUT_Def.h"

UINT32 AmbaDSP_VoutReset(const UINT8 VoutIdx);

UINT32 AmbaDSP_VoutMixerConfig(const UINT8 VoutIdx, const AMBA_DSP_VOUT_MIXER_CONFIG_s *pConfig);

UINT32 AmbaDSP_VoutMixerConfigBackColor(const UINT8 VoutIdx, UINT32 BackColorYUV);

UINT32 AmbaDSP_VoutMixerConfigHighlightColor(const UINT8 VoutIdx, UINT8 LumaThreshold, UINT32 HighlightColorYUV);

UINT32 AmbaDSP_VoutMixerConfigCsc(const UINT8 VoutIdx, const UINT8 CscCtrl);

UINT32 AmbaDSP_VoutMixerCtrl(const UINT8 VoutIdx);

UINT32 AmbaDSP_VoutOsdConfigBuf(const UINT8 VoutIdx, const AMBA_DSP_VOUT_OSD_BUF_CONFIG_s *pBufConfig);

UINT32 AmbaDSP_VoutOsdCtrl(const UINT8 VoutIdx, UINT8 Enable, UINT8 SyncWithVin, UINT64 *pAttachedRawSeq);

UINT32 AmbaDSP_VoutDisplayConfig(const UINT8 VoutIdx, const AMBA_DSP_DISPLAY_CONFIG_s* pConfig);

UINT32 AmbaDSP_VoutDisplayCtrl(const UINT8 VoutIdx);

UINT32 AmbaDSP_VoutDisplayConfigGamma(const UINT8 VoutIdx, ULONG TableAddr);

UINT32 AmbaDSP_VoutDisplayControlGamma(const UINT8 VoutIdx, UINT8 Enable);

UINT32 AmbaDSP_VoutVideoConfig(const UINT8 VoutIdx, const AMBA_DSP_VOUT_VIDEO_CFG_s *pConfig);

UINT32 AmbaDSP_VoutVideoCtrl(const UINT8 VoutIdx, UINT8 Enable, UINT8 SyncWithVin, UINT64 *pAttachedRawSeq);

UINT32 AmbaDSP_VoutConfigMixerBinding(UINT8 NumVout, const UINT8 *pVoutIdx, const AMBA_DSP_VOUT_DATA_PATH_CFG_s *pConfig);

UINT32 AmbaDSP_VoutMixerCscMatrixConfig(const UINT8 VoutIdx, const AMBA_DSP_VOUT_CSC_MATRIX_s *pCscMatrix);

UINT32 AmbaDSP_VoutDveConfig(const UINT8 VoutIdx, UINT8 DveMode);

UINT32 AmbaDSP_VoutDveCtrl(const UINT8 VoutIdx);

#endif  /* AMBA_DSP_VOUT_H */
