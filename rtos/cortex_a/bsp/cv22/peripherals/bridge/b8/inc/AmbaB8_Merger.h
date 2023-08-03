/**
 *  @file AmbaB8_Merger.h
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
 *  @details Definitions & Constants for B6 Merger Control APIs
 *
 */

#ifndef AMBA_B8_MERGER_H
#define AMBA_B8_MERGER_H

#define B8_MERGER_MAX_INPUT_CHAN   2U
/*-----------------------------------------------------------------------------------------------*\
 * Merger Configuration
\*-----------------------------------------------------------------------------------------------*/
/* MERGER TYPE */
#define B8_MERGER_BYPASS   0U
#define B8_MERGER_MERGE    1U
#define B8_MERGER_SPLIT    2U

/* video input source */
#define B8_MERGER_SOURCE_NONE          0U
#define B8_MERGER_SOURCE_FROM_SERDES   1U
#define B8_MERGER_SOURCE_FROM_SENSOR   2U

typedef struct {
    UINT8  SourceSelect[B8_MERGER_MAX_INPUT_CHAN]; /* SourceSelect[0]: input from DES0/VIN
                                                           SourceSelect[1]: input from DES1/PIP,
                                                           defined as B8_MERGER_SOURCE_xxx */
    UINT16 InputWidth;                                  /* Merger input width */
    UINT16 InputHeight;                                 /* Merger input height */
    UINT16 MaxHblank;                                   /* Maximum horizontal blanking (in unit of core clock) */
    UINT16 PixelWidth;                                  /* NumDataBits */
    UINT8  ViewSwap;                                    /* only available for dual video inputs case.
                                                            0U(default): left= VIN0(Ch0), right= VIN1(ch1)
                                                            1U: left= VIN1(Ch1), right= VIN0(ch0)*/
} B8_MERGER_CONFIG_s;

#define B8_MERGER_NEAR_END_2_INPUT_STREAMS 0U
#define B8_MERGER_NEAR_END_4_INPUT_STREAMS 1U


typedef struct {
    UINT8  MergeType;                                   /* defined as B8_MERGER_xxx */
    UINT8  SourceSelect;                                /* defined as B8_MERGER_SOURCE_xxx */
    UINT16 InputWidth;                                  /* Merger input width */
    UINT16 InputHeight;                                 /* Merger input height */
    UINT32 NearEndInputStreams;                         /* Near-End Merger input streams from Post-processor (B6N only),
                                                           defined as B8_MERGER_NEAR_END_xxx */
    UINT16 PictureWidth;                                /* Picture width of one exposure picture */
    UINT8  NumExposures;                                /* Number of pictures from a sensor with different exposures */
    UINT16 PixelWidth;                                  /* VOUTF Pixel width */
    UINT16 MaxHblank;                                   /* Maximum horizontal blanking (in unit of core clock), valid  when CFA Codec is used */
} B8_MERGER_CONFIG_WITH_CODEC_s;

typedef struct {
    UINT32 SlvsDataRate;                                /* SLVS Data rate per lane (B6N SLVS only) */
    UINT8  LaneWidth;                                   /* Number of SLVS output lanes (1, 2, 4, 8, and 10 are valid) (B6N SLVS only) */
    UINT8  PixelWidth;                                  /* VOUTF Pixel width (8, 10, 12, and 14 are valid) (B6N SLVS only) */
    UINT8  MinVblank;                                   /* Minimum line count of vertical blank */
    UINT16 MinHblank;                                   /* Minimum pixels from EAV to SAV */
    UINT16 MaxHblank;                                   /* Maximum horizontal blanking (in unit of core clock) */
} B8_MERGER_SLVS_CONFIG_s;

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB8_Merger.c
\*---------------------------------------------------------------------------*/
UINT32 AmbaB8_MergerConfig(UINT32 ChipID, const B8_MERGER_CONFIG_s *pMergerConfig);
UINT32 AmbaB8_MergerConfigWithCodec(UINT32 ChipID, const B8_MERGER_CONFIG_WITH_CODEC_s *pMergerConfigWithCodec);
UINT32 AmbaB8_MergerSlvsConfig(UINT32 ChipID, const B8_MERGER_SLVS_CONFIG_s *pMergerSlvsConfig);

#endif  /* AMBA_B8_MERGER_H */
