/**
 *  @file AmbaDSP_StillDec_Def.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by UINT32ellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for Ambarella DSP Driver Still Picture Decoder Definition
 *
 */
#ifndef AMBA_DSP_STILL_DEC_DEF_H
#define AMBA_DSP_STILL_DEC_DEF_H

#include "AmbaDSP.h"
#include "AmbaDSP_VOUT.h"

typedef struct {
    UINT8  BitsFormat;                         /**< see AMBA_DSP_DEC_BITS_FORMAT_NUM */
    UINT8  Rsvd[3U];
    ULONG  BitsAddr;                           /**< Start address of the bitstream */
    UINT32 BitsSize;                           /**< Size of the bitstream */

    ULONG  YuvBufAddr;                         /**< Start address of YUV buffer */
    UINT32 YuvBufSize;                         /**< Maximum size of YUV buffer */
} AMBA_DSP_STLDEC_START_CONFIG_s;

typedef struct {
    UINT8  RotateFlip;                 /**< rotate and flip setting  */
    UINT8  LumaGain;                   /**< luma gain: 1 ~ 255 */
    UINT8  Rsvd[2U];
} AMBA_DSP_STLDEC_YUV2YUV_s;

typedef struct {
    ULONG  AlphaMapAddr;               /* alpha map */
    UINT8  GlobalAlpha;                /* Use Global alpha value if pAlphaMap = NULL */
    UINT8  Rsvd[3U];
} AMBA_DSP_STLDEC_BLEND_s;

#endif  /* AMBA_DSP_STILL_DEC_DEF_H */
