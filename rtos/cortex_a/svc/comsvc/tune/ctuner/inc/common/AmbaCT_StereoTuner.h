/**
 *  @file AmbaCT_StereoTuner.h
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
#ifndef AMBA_CT_STEREO_TUNER_H
#define AMBA_CT_STEREO_TUNER_H
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV2FS)
#include "AmbaTypes.h"
#include "AmbaCalib_EmirrorIF.h"
#include "AmbaCT_StereoTunerIF.h"

typedef enum {
    AMBA_CT_STEREO_SYSTEM = 0,
    AMBA_CT_STEREO_WARP_OPTIMIZER,
    AMBA_CT_STEREO_OUTPUT_CFG,
    AMBA_CT_STEREO_VIRTUAL_CAM,
    AMBA_CT_STEREO_VOUT,
    AMBA_CT_STEREO_LENS,
    AMBA_CT_STEREO_SENSOR,
    AMBA_CT_STEREO_OPTICAL_CENTER,
    AMBA_CT_STEREO_CAMERA,
    AMBA_CT_STEREO_VIN,
    AMBA_CT_STEREO_ROI,
    AMBA_CT_STEREO_TILE_SIZE,
    AMBA_CT_STEREO_CALIB_POINTS,
    AMBA_CT_STEREO_ASSIST_POINTS,
    AMBA_CT_STEREO_OPTIMIZE,
    AMBA_CT_STEREO_INTERNAL,
    AMBA_CT_STEREO_TUNER_MAX,
} AMBA_CT_STEREO_TUNER_GROUP_s;

typedef struct {
    void *pWorkingBuf;
    SIZE_t WorkingBufSize;
    void *pCalibWorkingBuf;
    SIZE_t CalibWorkingBufSize;
    UINT32 (*SvCbMsgReciver)(AMBA_CAL_EM_MSG_TYPE_e Type, const AMBA_CAL_EM_MSG_s *pMsg);
    UINT32 (*SvCbMsgFeedCornerMap)(const char *FilePath, AMBA_CAL_SIZE_s *pCorner2dMapSize,
        AMBA_CAL_POINT_INT_2D_s *pBaseCorner2dIdx, AMBA_CAL_EM_MULTI_POINT_MAP_s *pMultiPointSet);
} AMBA_CT_STEREO_INIT_CFG_s;

typedef struct {
    AMBA_CAL_EM_SV_DATA_s *pDataSingleView;
} AMBA_CT_STEREO_CALIB_DATA_s;

void AmbaCT_StereoGetOutputCfg(UINT32 ArrayIndex, AMBA_CT_STEREO_OUTPUT_CFG_s *pData);
void AmbaCT_StereoSetOutputCfg(UINT32 ArrayIndex, const AMBA_CT_STEREO_OUTPUT_CFG_s *pData);
void AmbaCT_StereoGetSystem(AMBA_CT_STEREO_SYSTEM_s *pData);
void AmbaCT_StereoSetSystem(const AMBA_CT_STEREO_SYSTEM_s *pData);
void AmbaCT_StereoGetWarpOptimizer(AMBA_CT_STEREO_WARP_OPTIMIZER_s *pData);
void AmbaCT_StereoSetWarpOptimizer(const AMBA_CT_STEREO_WARP_OPTIMIZER_s *pData);
void AmbaCT_StereoGetVirtulaCam(UINT32 ArrayIndex, AMBA_CAL_EM_CURVED_SURFACE_CFG_s *pData);
void AmbaCT_StereoSetVirtulaCam(UINT32 ArrayIndex, const AMBA_CAL_EM_CURVED_SURFACE_CFG_s *pData);
void AmbaCT_StereoGetVout(UINT32 ArrayIndex, AMBA_CAL_ROI_s *pData);
void AmbaCT_StereoSetVout(UINT32 ArrayIndex, const AMBA_CAL_ROI_s *pData);
void AmbaCT_StereoGetLens(UINT32 ArrayIndex, AMBA_CT_STEREO_LENS_s *pData);
void AmbaCT_StereoSetLens(UINT32 ArrayIndex, const AMBA_CT_STEREO_LENS_s *pData);
void AmbaCT_StereoGetSensor(UINT32 ArrayIndex, AMBA_CAL_SENSOR_s *pData);
void AmbaCT_StereoSetSensor(UINT32 ArrayIndex, const AMBA_CAL_SENSOR_s *pData);
void AmbaCT_StereoGetOpticalCenter(UINT32 ArrayIndex, AMBA_CAL_POINT_DB_2D_s *pData);
void AmbaCT_StereoSetOpticalCenter(UINT32 ArrayIndex, const AMBA_CAL_POINT_DB_2D_s *pData);
void AmbaCT_StereoGetCamera(UINT32 ArrayIndex, AMBA_CT_STEREO_CAMERA_s *pData);
void AmbaCT_StereoSetCamera(UINT32 ArrayIndex, const AMBA_CT_STEREO_CAMERA_s *pData);
void AmbaCT_StereoGetVin(UINT32 ArrayIndex, AMBA_CAL_VIN_SENSOR_GEOMETRY_s *pData);
void AmbaCT_StereoSetVin(UINT32 ArrayIndex, const AMBA_CAL_VIN_SENSOR_GEOMETRY_s *pData);
void AmbaCT_StereoGetRoi(UINT32 ArrayIndex, AMBA_CAL_ROI_s *pData);
void AmbaCT_StereoSetRoi(UINT32 ArrayIndex, const AMBA_CAL_ROI_s *pData);
void AmbaCT_StereoGetTileSize(UINT32 ArrayIndex, AMBA_CAL_SIZE_s *pData);
void AmbaCT_StereoSetTileSize(UINT32 ArrayIndex, const AMBA_CAL_SIZE_s *pData);
void AmbaCT_StereoGetCalibPoints(UINT32 ArrayIndex, AMBA_CT_STEREO_CALIB_POINTS_s *pData);
void AmbaCT_StereoSetCalibPoints(UINT32 ArrayIndex, const AMBA_CT_STEREO_CALIB_POINTS_s *pData);
void AmbaCT_StereoGetAssistPoints(UINT32 ArrayIndex, AMBA_CT_STEREO_ASSIST_POINTS_s *pData);
void AmbaCT_StereoSetAssistPoints(UINT32 ArrayIndex, const AMBA_CT_STEREO_ASSIST_POINTS_s *pData);
void AmbaCT_StereoGetOptimize(UINT32 ArrayIndex, UINT8 *pData);
void AmbaCT_StereoSetOptimize(UINT32 ArrayIndex, const UINT8 *pData);
void AmbaCT_StereoGetInternal(UINT32 ArrayIndex, AMBA_CAL_EM_INTERNAL_CFG_s *pData);
void AmbaCT_StereoSetInternal(UINT32 ArrayIndex, const AMBA_CAL_EM_INTERNAL_CFG_s *pData);

void AmbaCT_StereoGetWorkingBufSize(SIZE_t *pSize);
UINT32 AmbaCT_StereoTunerInit(const AMBA_CT_STEREO_INIT_CFG_s *pInitCfg);
UINT8 AmbaCT_StereoGetGroupStatus(UINT8 GroupId);
UINT32 AmbaCT_StereoExecute(void);
UINT32 AmbaCT_StereoGetCalibData(UINT32 CameraId, AMBA_CT_STEREO_CALIB_DATA_s *pData);
#endif
#endif
