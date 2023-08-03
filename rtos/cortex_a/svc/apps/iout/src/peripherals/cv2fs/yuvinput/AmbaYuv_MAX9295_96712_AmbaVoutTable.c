/**
*  @file AmbaYuv_MAX9295_96712_AmbaVoutTable.c
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
*  @details Camera Information Managements
*
*/

#include "AmbaYuv.h"
#include "AmbaYuv_MAX9295_96712_AmbaVout.h"

const AMBA_MX01_VOUT_MODE_INFO_s AmbaVout_MX01_ModeInfo[AMBA_MX01_VOUT_MODE] = {
    [AMBA_MX01_VOUT_1080P60_4L] = {
        .DataRate = 600000000,
        .NumDataLanes = 4,
        .NumDataBits = 8,
        .OutputInfo = {
            .OutputWidth = 1920,
            .OutputHeight = 1080,
            .RecordingPixels = {
                .StartX = 0,
                .StartY = 0,
                .Width = 1920,
                .Height = 1080,
            },
            .YuvOrder = AMBA_VIN_YUV_ORDER_Y0_CR_Y1_CB,
            .FrameRate = {
                .Interlace = 0,
                .TimeScale = 60000,
                .NumUnitsInTick = 1001,
            }
        }
    },
    [AMBA_MX01_VOUT_2560_1440_30P] = {
        .DataRate = 400000000,
        .NumDataLanes = 4,
        .NumDataBits = 8,
        .OutputInfo = {
            .OutputWidth = 1280,
            .OutputHeight = 720,
            .RecordingPixels = {
                .StartX = 0,
                .StartY = 0,
                .Width = 1280,
                .Height = 720,
            },
            .YuvOrder = AMBA_VIN_YUV_ORDER_Y0_CR_Y1_CB,
            .FrameRate = {
                .Interlace = 0,
                .TimeScale = 30000,
                .NumUnitsInTick = 1001,
            }
        }
    },
    [AMBA_MX01_VOUT_3840_2160_30P] = {
        .DataRate = 1151961600,
        .NumDataLanes = 4,
        .NumDataBits = 8,
        .OutputInfo = {
            .OutputWidth = 3840,
            .OutputHeight = 2160,
            .RecordingPixels = {
                .StartX = 0,
                .StartY = 0,
                .Width = 3840,
                .Height = 2160,
            },
            .YuvOrder = AMBA_VIN_YUV_ORDER_Y0_CR_Y1_CB,
            .FrameRate = {
                .Interlace = 0,
                .TimeScale = 30000,
                .NumUnitsInTick = 1000,
            }
        }
    },
};

