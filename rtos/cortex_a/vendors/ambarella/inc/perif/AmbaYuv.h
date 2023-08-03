/**
 *  @file AmbaYuv.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions for Ambarella Yuv driver APIs
 *
 */

#ifndef AMBA_YUV_H
#define AMBA_YUV_H

#ifndef AMBA_VIN_H
#include "AmbaVIN.h"
#endif

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

/* ERROR CODE */
#define YUV_ERR_NONE         (0U)
#define YUV_ERR_INVALID_API  (YUV_ERR_BASE + 1U)
#define YUV_ERR_ARG          (YUV_ERR_BASE + 2U)
#define YUV_ERR_COMMUNICATE  (YUV_ERR_BASE + 3U)
#define YUV_ERR_UNEXPECTED   (YUV_ERR_BASE + 4U)

/* print module */
#define YUV_MODULE_ID        ((UINT16)(YUV_ERR_BASE >> 16U))

typedef struct {
    INT16   StartX;
    INT16   StartY;
    UINT16  Width;
    UINT16  Height;
} AMBA_YUV_AREA_INFO_s;

typedef struct {
    UINT32 VinID;
    UINT32 Reserved;
} AMBA_YUV_CHANNEL_s;

typedef struct {
    UINT32 ModeID;
    UINT32 Reserved[4];
} AMBA_YUV_CONFIG_s;

#define AMBA_YUV_YUV_ORDER_Y0_CB_Y1_CR AMBA_VIN_YUV_ORDER_Y0_CB_Y1_CR
#define AMBA_YUV_YUV_ORDER_CR_Y0_CB_Y1 AMBA_VIN_YUV_ORDER_CR_Y0_CB_Y1
#define AMBA_YUV_YUV_ORDER_CB_Y0_CR_Y1 AMBA_VIN_YUV_ORDER_CB_Y0_CR_Y1
#define AMBA_YUV_YUV_ORDER_Y0_CR_Y1_CB AMBA_VIN_YUV_ORDER_Y0_CR_Y1_CB

typedef struct {
    UINT64                  DataRate;           /* output bit/pixel clock frequency from device */
    UINT16                  OutputWidth;        /* valid pixels per line */
    UINT16                  OutputHeight;       /* valid lines per frame */
    AMBA_YUV_AREA_INFO_s    RecordingPixels;    /* maximum recording frame size */
    UINT8                   YuvOrder;           /* YUV pixels order */
    AMBA_VIN_FRAME_RATE_s   FrameRate;          /* framerate value of this sensor mode */
} AMBA_YUV_OUTPUT_INFO_s;

typedef struct {
    AMBA_YUV_CONFIG_s       Config;
    AMBA_YUV_OUTPUT_INFO_s  OutputInfo;            /* info about output data format */
} AMBA_YUV_MODE_INFO_s;

typedef struct {
    AMBA_YUV_MODE_INFO_s        ModeInfo;
    UINT32                      Reserved[4];
} AMBA_YUV_STATUS_INFO_s;

typedef struct {
    UINT32 (*Init)(const AMBA_YUV_CHANNEL_s *pChan);
    UINT32 (*Enable)(const AMBA_YUV_CHANNEL_s *pChan);
    UINT32 (*Disable)(const AMBA_YUV_CHANNEL_s *pChan);
    UINT32 (*Config)(const AMBA_YUV_CHANNEL_s *pChan, const AMBA_YUV_CONFIG_s *pMode);
    UINT32 (*GetStatus)(const AMBA_YUV_CHANNEL_s *pChan, AMBA_YUV_STATUS_INFO_s *pStatus);
    UINT32 (*GetModeInfo)(const AMBA_YUV_CHANNEL_s *pChan, const AMBA_YUV_CONFIG_s *pMode, AMBA_YUV_MODE_INFO_s *pModeInfo);
} AMBA_YUV_OBJ_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaYuv.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_YUV_OBJ_s *pAmbaYuvObj[AMBA_NUM_VIN_CHANNEL];
extern void AmbaYuv_Hook(const AMBA_YUV_CHANNEL_s *pChan, AMBA_YUV_OBJ_s *pYuvObj);

static inline UINT32 AmbaYuv_Init(const AMBA_YUV_CHANNEL_s *pChan)
{
    return ((pAmbaYuvObj[(pChan)->VinID]->Init == NULL) ? YUV_ERR_INVALID_API: pAmbaYuvObj[(pChan)->VinID]->Init(pChan));
}
static inline UINT32 AmbaYuv_Enable(const AMBA_YUV_CHANNEL_s *pChan)
{
    return ((pAmbaYuvObj[(pChan)->VinID]->Enable == NULL) ? YUV_ERR_INVALID_API : pAmbaYuvObj[(pChan)->VinID]->Enable((pChan)));
}
static inline UINT32 AmbaYuv_Disable(const AMBA_YUV_CHANNEL_s *pChan)
{
    return ((pAmbaYuvObj[(pChan)->VinID]->Disable == NULL) ? YUV_ERR_INVALID_API : pAmbaYuvObj[(pChan)->VinID]->Disable((pChan)));
}
static inline UINT32 AmbaYuv_Config(const AMBA_YUV_CHANNEL_s *pChan, const AMBA_YUV_CONFIG_s *pMode)
{
    return ((pAmbaYuvObj[(pChan)->VinID]->Config == NULL) ? YUV_ERR_INVALID_API : pAmbaYuvObj[(pChan)->VinID]->Config((pChan), (pMode)));
}
static inline UINT32 AmbaYuv_GetStatus(const AMBA_YUV_CHANNEL_s *pChan, AMBA_YUV_STATUS_INFO_s *pStatus)
{
    return ((pAmbaYuvObj[(pChan)->VinID]->GetStatus == NULL) ? YUV_ERR_INVALID_API : pAmbaYuvObj[(pChan)->VinID]->GetStatus((pChan), (pStatus)));
}
static inline UINT32 AmbaYuv_GetModeInfo(const AMBA_YUV_CHANNEL_s *pChan, const AMBA_YUV_CONFIG_s *pMode, AMBA_YUV_MODE_INFO_s *pModeInfo)
{
    return ((pAmbaYuvObj[(pChan)->VinID]->GetModeInfo == NULL) ? YUV_ERR_INVALID_API : pAmbaYuvObj[(pChan)->VinID]->GetModeInfo((pChan), (pMode), (pModeInfo)));
}

#endif /* AMBA_YUV_H */
