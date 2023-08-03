/**
 *  @file AmbaCT_EmirTuner.h
 *
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef AMBA_CT_EM_TUNER_H
#define AMBA_CT_EM_TUNER_H
#include "AmbaCalib_EmirrorIF.h"
#include "AmbaCalib_LDCIF.h"
#include "AmbaCT_EmirTunerIF.h"
typedef enum {
    AMBA_CT_EM_SYSTEM = 0,
    AMBA_CT_EM_CAR,
    AMBA_CT_EM_3IN1_VIEW,
    AMBA_CT_EM_SINGLE_VIEW,
    AMBA_CT_EM_SINGLE_ASP_CFG,
    AMBA_CT_EM_SINGLE_CURVED_CFG,
    AMBA_CT_EM_3IN1_BLEND,
    AMBA_CT_EM_VOUT,
    AMBA_CT_EM_MASK,
    AMBA_CT_EM_LENS,
    AMBA_CT_EM_SENSOR,
    AMBA_CT_EM_OPTICAL_CENTER,
    AMBA_CT_EM_CAMERA,
    AMBA_CT_EM_VIN,
    AMBA_CT_EM_ROI,
    AMBA_CT_EM_TILE_SIZE,
    AMBA_CT_EM_CALIB_POINTS,
    AMBA_CT_EM_ASSISTANCE_POINTS,
    AMBA_CT_EM_OPTIMIZE,
    AMBA_CT_EM_INTERNAL,
    AMBA_CT_EM_TUNER_MAX,
} AMBA_CT_EM_TUNER_GROUP_s;




typedef struct {
    void *pWorkingBuf;
    SIZE_t WorkingBufSize;
    void *pCalibWorkingBuf;
    SIZE_t CalibWorkingBufSize;
    UINT32 (*CbMsgReciver)(AMBA_CAL_EM_MSG_TYPE_e Type, AMBA_CAL_EM_CAM_ID_e CamId, const AMBA_CAL_EM_MSG_s *pMsg);
    UINT32 (*SvCbMsgReciver)(AMBA_CAL_EM_MSG_TYPE_e Type, const AMBA_CAL_EM_MSG_s *pMsg);
    UINT32 (*CbFeedMaskFunc)(UINT32 CamId, UINT32 *pWidth, UINT32 *pHeight, UINT8 *pTable);
} AMBA_CT_EM_INIT_CFG_s;



void AmbaCT_EmGetSystem(AMBA_CT_EM_SYSTEM_s *pData);
void AmbaCT_EmSetSystem(const AMBA_CT_EM_SYSTEM_s *pData);
void AmbaCT_EmGetCar(AMBA_CAL_SIZE_s *pData);
void AmbaCT_EmSetCar(const AMBA_CAL_SIZE_s *pData);
void AmbaCT_EmGet3in1View(AMBA_CT_EM_3IN1_VIEW_s *pData);
void AmbaCT_EmSet3in1View(const AMBA_CT_EM_3IN1_VIEW_s *pData);
void AmbaCT_EmGet3in1Blend(AMBA_CT_EM_3IN1_BLEND_s *pData);
void AmbaCT_EmSet3in1Blend(const AMBA_CT_EM_3IN1_BLEND_s *pData);
void AmbaCT_EmGetSingleView(AMBA_CAL_EM_VIEW_TYPE_e *pData);
void AmbaCT_EmSetSingleView(const AMBA_CAL_EM_VIEW_TYPE_e *pData);
void AmbaCT_EmGetSingleViewAspCfg(AMBA_CT_EM_ASP_POINT_MAP_s *pData);
void AmbaCT_EmSetSingleViewAspCfg(const AMBA_CT_EM_ASP_POINT_MAP_s *pData);
void AmbaCT_EmGetSingleViewCurvedCfg(AMBA_CAL_EM_CURVED_SURFACE_CFG_s *pData);
void AmbaCT_EmSetSingleViewCurvedCfg(const AMBA_CAL_EM_CURVED_SURFACE_CFG_s *pData);
void AmbaCT_EmGetVout(UINT32 ArrayIndex, AMBA_CAL_ROI_s *pData);
void AmbaCT_EmSetVout(UINT32 ArrayIndex, const AMBA_CAL_ROI_s *pData);
void AmbaCT_EmGetMask(UINT32 ArrayIndex, AMBA_CT_EM_MASK_s **pData);
void AmbaCT_EmSetMask(UINT32 ArrayIndex, const AMBA_CT_EM_MASK_s *pData);
void AmbaCT_EmGetLens(UINT32 ArrayIndex, AMBA_CT_EM_LENS_s *pData);
void AmbaCT_EmSetLens(UINT32 ArrayIndex, const AMBA_CT_EM_LENS_s *pData);
void AmbaCT_EmGetSensor(UINT32 ArrayIndex, AMBA_CAL_SENSOR_s *pData);
void AmbaCT_EmSetSensor(UINT32 ArrayIndex, const AMBA_CAL_SENSOR_s *pData);
void AmbaCT_EmGetOpticalCenter(UINT32 ArrayIndex, AMBA_CAL_POINT_DB_2D_s *pData);
void AmbaCT_EmSetOpticalCenter(UINT32 ArrayIndex, const AMBA_CAL_POINT_DB_2D_s *pData);
void AmbaCT_EmGetCamera(UINT32 ArrayIndex, AMBA_CT_EM_CAMERA_s *pData);
void AmbaCT_EmSetCamera(UINT32 ArrayIndex, const AMBA_CT_EM_CAMERA_s *pData);
void AmbaCT_EmGetVin(UINT32 ArrayIndex, AMBA_CAL_VIN_SENSOR_GEOMETRY_s *pData);
void AmbaCT_EmSetVin(UINT32 ArrayIndex, const AMBA_CAL_VIN_SENSOR_GEOMETRY_s *pData);
void AmbaCT_EmGetRoi(UINT32 ArrayIndex, AMBA_CAL_ROI_s *pData);
void AmbaCT_EmSetRoi(UINT32 ArrayIndex, const AMBA_CAL_ROI_s *pData);
void AmbaCT_EmGetTileSize(UINT32 ArrayIndex, AMBA_CAL_SIZE_s *pData);
void AmbaCT_EmSetTileSize(UINT32 ArrayIndex, const AMBA_CAL_SIZE_s *pData);
void AmbaCT_EmGetCalibPoints(UINT32 ArrayIndex, AMBA_CT_EM_CALIB_POINTS_s *pData);
void AmbaCT_EmSetCalibPoints(UINT32 ArrayIndex, const AMBA_CT_EM_CALIB_POINTS_s *pData);
void AmbaCT_EmGetAssistancePoints(UINT32 ArrayIndex, AMBA_CT_EM_ASSISTANCE_POINTS_s *pData);
void AmbaCT_EmSetAssistancePoints(UINT32 ArrayIndex, const AMBA_CT_EM_ASSISTANCE_POINTS_s *pData);
void AmbaCT_EmGetOptimize(UINT32 ArrayIndex, UINT8 *pData);
void AmbaCT_EmSetOptimize(UINT32 ArrayIndex, const UINT8 *pData);
void AmbaCT_EmGetInternal(UINT32 ArrayIndex, AMBA_CAL_EM_INTERNAL_CFG_s *pData);
void AmbaCT_EmSetInternal(UINT32 ArrayIndex, const AMBA_CAL_EM_INTERNAL_CFG_s *pData);

void AmbaCT_EmGetWorkingBufSize(SIZE_t *pSize);
UINT32 AmbaCT_EmTunerInit(const AMBA_CT_EM_INIT_CFG_s *pInitCfg);
UINT8 AmbaCT_EmGetGroupStatus(UINT8 GroupId);
UINT32 AmbaCT_EmExecute(void);

#endif
