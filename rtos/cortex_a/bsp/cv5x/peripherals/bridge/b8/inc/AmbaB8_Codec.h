/**
 *  @file AmbaB8_Codec.h
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details Definitions & Constants for B6 CFA Codec Control APIs
 *
 */

#ifndef AMBA_B8_CODEC_H
#define AMBA_B8_CODEC_H

/*-----------------------------------------------------------------------------------------------*\
 * Codec Configuration
\*-----------------------------------------------------------------------------------------------*/
#define B8_CODEC_ENCODER       0U
#define B8_CODEC_DECODER       1U

#define B8_CODEC_CFA_TYPE_RG   0U
#define B8_CODEC_CFA_TYPE_BG   1U
#define B8_CODEC_CFA_TYPE_GR   2U
#define B8_CODEC_CFA_TYPE_GB   3U

typedef struct {
    UINT16 PictureHeight;       /* Total line number to be proceed */
    UINT16 PictureWidth;        /* Picture width of one exposure picture */
    UINT32 SerdesDataRate;      /* Serdes data rate (in unit of bps) */
    UINT16 TargetBitRate;       /* Target bitrate (in uint of 1/1024 bits/pixel) */
    UINT8  CfaType;             /* Color Filter Array type, defined as B8_CODEC_CFA_TYPE_xxx */
    UINT8  TwoCables;           /* 0: Transmission over 1 cable (Packer 0), 1: Transmission over 2 calbes (Packer 0 & 1) */
    UINT8  SplitHorizontal;     /* 0 - Data coded as one picture 1 - Data coded as side-by-side pictures */
    UINT8  NumFarSensors;       /* Number of sensors on a B6F */
    UINT8  NumPicturesPerFrame; /* Number of pictures from a sensor with different exposures */
    UINT8  NumInputBits;        /* Number of data bits from sensor */
} B8_CODEC_CONFIG_s;

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB8_Codec.c
\*---------------------------------------------------------------------------*/
UINT16 AmbaB8_CodecGetVariableBitRate(UINT16 PictureWidth, UINT8 NumFarSensors, UINT32 SerdesDataRate, FLOAT RowTime);
UINT32 AmbaB8_CodecConfig(UINT32 ChipID, UINT8 CodecMode, const B8_CODEC_CONFIG_s *pCodecConfig);

#endif  /* AMBA_B8_CODEC_H */
