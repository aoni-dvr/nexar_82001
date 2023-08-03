/**
 *  @file AmbaYuv_MAX9295_96712_AmbaVout.h
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
 *  @details Control APIs of OmniVision AMBAVOUT CMOS sensor with MIPI interface
 *
 */

#ifndef AMBA_MX01_YUV_AMBAVOUT_H
#define AMBA_MX01_YUV_AMBAVOUT_H

/* Mode ID */
#define AMBA_MX01_VOUT_1080P60_4L    (0U)                /* 1080P60 4 lanes */
#define AMBA_MX01_VOUT_2560_1440_30P (1U)
#define AMBA_MX01_VOUT_3840_2160_30P (2U)
#define AMBA_MX01_VOUT_MODE          (3U)

typedef struct {
    UINT32  DataRate;
    UINT8   NumDataLanes;
    UINT8   NumDataBits;
    AMBA_YUV_OUTPUT_INFO_s OutputInfo;
} AMBA_MX01_VOUT_MODE_INFO_s;

extern const AMBA_MX01_VOUT_MODE_INFO_s AmbaVout_MX01_ModeInfo[AMBA_MX01_VOUT_MODE] ;
extern AMBA_YUV_OBJ_s AmbaYuv_MX01_AmbaVoutObj;

#endif /* AMBA_MX01_YUV_AMBAVOUT_H */

