/**
 *  @file AmbaDSP_VideoDec.h
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
 *  @details Definitions & Constants for Ambarella DSP Driver Video Decoder APIs
 *
 */
#ifndef AMBA_DSP_VIDEO_DEC_H
#define AMBA_DSP_VIDEO_DEC_H

#include "AmbaDSP.h"
#include "AmbaDSP_VideoDec_Def.h"

UINT32 AmbaDSP_VideoDecConfig(UINT16 MaxNumStream, const AMBA_DSP_VIDDEC_STREAM_CONFIG_s *pStreamConfig);

UINT32 AmbaDSP_VideoDecStart(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig);

UINT32 AmbaDSP_VideoDecStop(UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pShowLastFrame);

UINT32 AmbaDSP_VideoDecTrickPlay(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_TRICKPLAY_s *pTrickPlay);

UINT32 AmbaDSP_VideoDecBitsFifoUpdate(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_BITS_FIFO_s *pBitsFifo);

UINT32 AmbaDSP_VideoDecPostCtrl(UINT16 StreamIdx, UINT16 NumPostCtrl, const AMBA_DSP_VIDDEC_POST_CTRL_s *pPostCtrl);

#endif  /* AMBA_DSP_VIDEO_DEC_H */
