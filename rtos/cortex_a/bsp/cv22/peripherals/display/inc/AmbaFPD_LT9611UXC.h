/**
 *  @file AmbaFPD_LT9611UXC.h
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
 *  @details Definitions & Constants for DZXtech FPD LT9611UXC APIs.
 *
 */

#ifndef AMBA_FPD_LT9611UXC_H
#define AMBA_FPD_LT9611UXC_H

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for all the view mode.
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_FPD_LT9611_YUV422_720P_60HZ                0U     /* 59.94Hz */
#define AMBA_FPD_LT9611_RGB888_720P_60HZ                1U     /* 59.94Hz */
#define AMBA_FPD_LT9611_YUV422_1080P_30HZ               2U     /* 29.97Hz */
#define AMBA_FPD_LT9611_RGB888_1080P_30HZ               3U     /* 29.97Hz */
#define AMBA_FPD_LT9611_YUV422_1080P_60HZ               4U     /* 59.94Hz */
#define AMBA_FPD_LT9611_RGB888_1080P_60HZ               5U     /* 59.94Hz */
#define AMBA_FPD_LT9611_YUV422_2160P_30HZ               6U     /* 29.97Hz */
#define AMBA_FPD_LT9611_RGB888_2160P_30HZ               7U     /* 29.97Hz */
#define AMBA_FPD_LT9611_YUV422_720P_A60HZ               8U     /* 60.00Hz */
#define AMBA_FPD_LT9611_RGB888_720P_A60HZ               9U     /* 60.00Hz */
#define AMBA_FPD_LT9611_YUV422_1080P_A30HZ             10U     /* 30.00Hz */
#define AMBA_FPD_LT9611_RGB888_1080P_A30HZ             11U     /* 30.00Hz */
#define AMBA_FPD_LT9611_YUV422_1080P_A60HZ             12U     /* 60.00Hz */
#define AMBA_FPD_LT9611_RGB888_1080P_A60HZ             13U     /* 60.00Hz */
#define AMBA_FPD_LT9611_YUV422_2160P_A30HZ             14U     /* 30.00Hz */
#define AMBA_FPD_LT9611_RGB888_2160P_A30HZ             15U     /* 30.00Hz */
#define AMBA_FPD_LT9611_SERDES_NUM_MODE                16U

/*-----------------------------------------------------------------------------------------------*\
 * LT9611UXC DISPLAY CONFIG
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32                                  OutputMode;
    AMBA_VOUT_FRAME_TIMING_CONFIG_s         FrameTiming;
} AMBA_FPD_LT9611UXC_CONFIG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaFPD_LT9611UXC.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_FPD_OBJECT_s AmbaFPD_LT9611UXCObj;

#endif /* AMBA_FPD_LT9611UXC_H */
