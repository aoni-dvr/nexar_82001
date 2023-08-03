/**
 *  @file AmbaDSP_VOUT_Def.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
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
 *  @details Definitions & Constants for DSP Driver VOUT Definition
 *
 */
#ifndef AMBA_DSP_VOUT_DEF_H
#define AMBA_DSP_VOUT_DEF_H

#include "AmbaDSP.h"

/* AMBA_DSP_MAX_VOUT_NUM */
#define VOUT_IDX_A      (0U)
#define VOUT_IDX_B      (1U)
#define NUM_VOUT_IDX    (2U)

#define MIXER_IN_YUV_422        (0x0U)
#define MIXER_IN_YUV_444_RGB    (0x1U)
typedef struct {
    UINT16 ActiveWidth;
    UINT16 ActiveHeight;
    UINT8  VideoHorReverseEnable;
    AMBA_DSP_FRAME_RATE_s FrameRate;
    UINT8  MixerColorFormat;
} AMBA_DSP_VOUT_MIXER_CONFIG_s;

/* CscCtrl */
#define MIXER_CSC_DISABLE       (0U)
#define MIXER_CSC_FOR_VIDEO     (1U)
#define MIXER_CSC_FOR_OSD       (2U)

typedef struct {
    FLOAT Coef[3][3];
    FLOAT Offset[3];
    UINT16 MinVal[3];
    UINT16 MaxVal[3];
} AMBA_DSP_VOUT_CSC_MATRIX_s;

/* OSD Data Format */
#define OSD_8BIT_CLUT_MODE      (0U)
#define OSD_16BIT_VYU_RGB_565   (1U)
#define OSD_16BIT_UYV_BGR_565   (2U)
#define OSD_16BIT_AYUV_4444     (3U)
#define OSD_16BIT_RGBA_4444     (4U)
#define OSD_16BIT_BGRA_4444     (5U)
#define OSD_16BIT_ABGR_4444     (6U)
#define OSD_16BIT_ARGB_4444     (7U)
#define OSD_16BIT_AYUV_1555     (8U)
#define OSD_16BIT_YUV_1555      (9U)
#define OSD_16BIT_RGBA_5551     (10U)
#define OSD_16BIT_BGRA_5551     (11U)
#define OSD_16BIT_ABGR_1555     (12U)
#define OSD_16BIT_ARGB_1555     (13U)
#define OSD_32BIT_AYUV_8888     (14U)
#define OSD_32BIT_RGBA_8888     (15U)
#define OSD_32BIT_BGRA_8888     (16U)
#define OSD_32BIT_ABGR_8888     (17U)
#define OSD_32BIT_ARGB_8888     (18U)
#define OSD_FORMAT_NUM          (19U)

/* Vout Video Source */
#define VOUT_SOURCE_DEFAULT_IMAGE     (0U)    // user specific default image
#define VOUT_SOURCE_BACKGROUND_COLOR  (1U)    // background color
#define VOUT_SOURCE_INTERNAL          (2U)    // operation mode video plane

typedef struct {
    UINT8  FieldRepeat;
    UINT16 Pitch;
    ULONG  BaseAddr;
    UINT16 InputWidth;
    UINT16 InputHeight;
    AMBA_DSP_WINDOW_s Window;
    UINT8  DataFormat;
    ULONG  CLUTAddr;
    UINT8  SwapByteEnable;
    UINT8  PremultipliedEnable;
    UINT8  GlobalBlendEnable;
    UINT8  TransparentColorEnable;
    UINT16 TransparentColor;
} AMBA_DSP_VOUT_OSD_BUF_CONFIG_s;

typedef struct {
    UINT16 VinVoutSyncDelay;
    UINT16 VoutSyncDelay;
} AMBA_DSP_DISPLAY_CONFIG_s;

typedef struct {
    UINT8  FieldRepeat;
    UINT16 Pitch;
    ULONG  BaseAddrY;
    ULONG  BaseAddrUV;
    UINT8  DataFormat; //only Yuv420 and Yuv422
} AMBA_DSP_VOUT_DEFAULT_IMG_CONFIG_s;

typedef struct {
    AMBA_DSP_WINDOW_s Window;
    UINT8  RotateFlip;
    UINT8  VideoSource;
    AMBA_DSP_VOUT_DEFAULT_IMG_CONFIG_s DefaultImgConfig;
} AMBA_DSP_VOUT_VIDEO_CFG_s;

typedef struct {
    UINT8  UseMixer;
    UINT8  MixerIdx;
} AMBA_DSP_VOUT_DATA_PATH_CFG_s;

#define VOUT_DVE_NTSC   (0x0U)
#define VOUT_DVE_PAL    (0x1U)

#endif  /* AMBA_DSP_VOUT_DEF_H */
