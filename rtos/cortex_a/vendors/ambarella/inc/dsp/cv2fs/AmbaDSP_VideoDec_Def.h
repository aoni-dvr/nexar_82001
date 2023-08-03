/**
 *  @file AmbaDSP_VideoDec_Def.h
 *
 *  @copyright Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions & Constants for Ambarella DSP Driver Video Decoder Definition
 *
 */
#ifndef AMBA_DSP_VIDEO_DEC_DEF_H
#define AMBA_DSP_VIDEO_DEC_DEF_H

#include "AmbaDSP.h"

/** AMBA_DSP_VIDDEC_STREAM_CONFIG_s.XcodeMode */
#define AMBA_DSP_XCODE_NONE             (0U)
#define AMBA_DSP_XCODE_THROTTLE         (1U)
#define AMBA_DSP_XCODE_NON_THROTTLE     (2U)

/** AmbaDSP_VideoDecTrickPlay */
#define AMBA_DSP_VIDDEC_PAUSE           (0U)
#define AMBA_DSP_VIDDEC_RESUME          (1U)
#define AMBA_DSP_VIDDEC_STEP            (2U)

typedef struct {
    UINT8 Operation;
    UINT8 ResumeDirection;
    UINT8 ResumeSpeedIndex;
} AMBA_DSP_VIDDEC_TRICKPLAY_s;

typedef struct {
    UINT16  StreamID;
    UINT16  BitsFormat;
    ULONG   BitsBufAddr;
    UINT32  BitsBufSize;
    UINT16  MaxFrameWidth;     /* 0 - use deafult value */
    UINT16  MaxFrameHeight;    /* 0 - use deafult value */

    UINT8   XcodeMode;
    UINT16  XcodeWidth;
    UINT16  XcodeHeight;

    UINT16  MaxVideoBufferWidth;      /* Max buffer width  */
    UINT16  MaxVideoBufferHeight;     /* Max buffer height */
    AMBA_DSP_FRAME_RATE_s FrameRate;
} AMBA_DSP_VIDDEC_STREAM_CONFIG_s;

/** AMBA_DSP_VIDDEC_START_CONFIG_s.SpeedIndex */
#define AMBA_DSP_VIDDEC_SPEED_01X    (0U)   /**< playback speed 1x */
#define AMBA_DSP_VIDDEC_SPEED_02X    (1U)   /**< playback speed 2x */
#define AMBA_DSP_VIDDEC_SPEED_04X    (2U)   /**< playback speed 4x */
#define AMBA_DSP_VIDDEC_SPEED_08X    (3U)   /**< playback speed 8x */
#define AMBA_DSP_VIDDEC_SPEED_16X    (4U)   /**< playback speed 16x */
#define AMBA_DSP_VIDDEC_SPEED_32X    (5U)   /**< playback speed 32x */
#define AMBA_DSP_VIDDEC_SPEED_64X    (6U)   /**< playback speed 64x */
#define AMBA_DSP_VIDDEC_SPEED_1_2X   (7U)   /**< playback speed 1/2x */
#define AMBA_DSP_VIDDEC_SPEED_1_4X   (8U)   /**< playback speed 1/4x */
#define AMBA_DSP_VIDDEC_SPEED_1_8X   (9U)   /**< playback speed 1/8x */
#define AMBA_DSP_VIDDEC_SPEED_1_16X  (10U)  /**< playback speed 1/16x */

typedef struct {
    UINT32  PreloadDataSize;
    UINT16  SpeedIndex;
    UINT16  Direction;           /**< 0: Forward play, 1: Backward play */
    UINT64  FirstDisplayPTS;
} AMBA_DSP_VIDDEC_START_CONFIG_s;

typedef struct {
    ULONG    StartAddr;
    ULONG    EndAddr;
} AMBA_DSP_VIDDEC_BITS_FIFO_s;

typedef struct {
    AMBA_DSP_WINDOW_s       InputWindow;         /* cropping of decode YUV */
    AMBA_DSP_WINDOW_s       TargetWindow;        /* Target display size */
    UINT8                   TargetRotateFlip;    /* Target window rotate and flip setting */

    UINT8                   VoutIdx;             /* Vout index */
    UINT8                   VoutRotateFlip;      /* Vout rotate and flip setting */
    AMBA_DSP_WINDOW_s       VoutWindow;          /* Vout video window */
} AMBA_DSP_VIDDEC_POST_CTRL_s;

#endif  /* AMBA_DSP_VIDEO_DEC_DEF_H */
