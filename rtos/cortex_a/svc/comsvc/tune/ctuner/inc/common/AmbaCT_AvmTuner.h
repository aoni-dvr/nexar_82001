 /**
  * @file AmbaCT_AvmTuner.h
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
#ifndef CT_AVM_TUNER_H_
#define CT_AVM_TUNER_H_
#include "AmbaCalib_Camera.h"
#include "AmbaCalib_AVMIF.h"
#include "AmbaCalib_LDCIF.h"
#include "AmbaCT_AvmTunerIF.h"





typedef enum {
    AMBA_CT_AVM_SYSTEM = 0,
    AMBA_CT_AVM_CAR,
    AMBA_CT_AVM_2D_VIEW,
    AMBA_CT_AVM_2D_CUSTOM_CALIB,
    AMBA_CT_AVM_3D_VIEW_MODEL_TYPE,
    AMBA_CT_AVM_3D_VIEW,
    AMBA_CT_AVM_3D_ADV_VIEW,
    AMBA_CT_AVM_3D_VITRUAL_CAM,
    AMBA_CT_AVM_3D_WARP,
    AMBA_CT_AVM_MAIN_VIEW,
    AMBA_CT_AVM_MAIN_VIEW_PERSPECTIVE,
    AMBA_CT_AVM_MAIN_VIEW_EQUAL_DIST,
    AMBA_CT_AVM_MAIN_VIEW_LDC,
    AMBA_CT_AVM_MAIN_VIEW_DIRECTION,
    AMBA_CT_AVM_VIEW_BLEND,
    AMBA_CT_AVM_OSD_MASK,
    AMBA_CT_AVM_LENS,
    AMBA_CT_AVM_SENSOR,
    AMBA_CT_AVM_OPTICAL_CENTER,
    AMBA_CT_AVM_CAMERA,
    AMBA_CT_AVM_VIN,
    AMBA_CT_AVM_CALIB_POINTS,
    AMBA_CT_AVM_ASSISTANCE_POINTS,
    AMBA_CT_AVM_ROI,
    AMBA_CT_AVM_MAIN,
    AMBA_CT_AVM_MIN_VOUT,
    AMBA_CT_AVM_TILE_SIZE,
    AMBA_CT_AVM_AUTO,
    AMBA_CT_AVM_OPTIMIZE,
    AMBA_CT_AVM_INTERNAL,
    AMBA_CT_AVM_PRECHECKDATA,
    AMBA_CT_AVM_MIN_ROI_SIZE,
    AMBA_CT_AVM_CALIB,
    AMBA_CT_AVM_TUNER_MAX,
} AMBA_CT_AVM_TUNER_GROUP_s;

typedef struct {
    void *pWorkingBuf;
    size_t WorkingBufSize;
    void *pCalibWorkingBuf;
    size_t CalibWorkingBufSize;
    UINT32 (*CbMsgReciver)(AMBA_CAL_AVM_MSG_TYPE_e Type, AMBA_CAL_AVM_CAM_ID_e CamId,const AMBA_CAL_AVM_MSG_s *pMsg);
    UINT32 (*CbFeedPreCalData)(AMBA_CAL_AVM_CAM_ID_e CamId, AMBA_CAL_AVM_PRE_CAL_DATA_s *pData);
    UINT32 (*CbSavePreCalData)(AMBA_CAL_AVM_CAM_ID_e CamId, const AMBA_CAL_AVM_PRE_CAL_DATA_s *pData);
    UINT32 (*Cb2DCustomCalibCalib)(AMBA_CAL_AVM_CAM_ID_e CamId, DOUBLE *pMat);
    UINT32 (*CbSavePreCheckData)(AMBA_CAL_AVM_CAM_ID_e CamId, const void *pData, UINT32 SaveLength);
    UINT32 (*CbGetPreCheckData)(const char *pFileName, void *pData, UINT32 DataLength);
    UINT32 (*CbFeedOSDMaskFunc)(UINT32 Width, UINT32 Height, const char *TablePath, UINT8 *pTable);
    UINT32 (*CbSaveCarVout4Points)(const AMBA_CAL_POINT_DB_2D_s CarVout4Points[4U]);
    UINT32 (*CbCalibErrorReport)(AMBA_CAL_AVM_CAM_ID_e CamId, UINT32 CalibPointNum, const DOUBLE ReProjectError[]);
} AMBA_CT_AVM_INIT_CFG_s;

void AmbaCT_AvmGetSystem(AMBA_CT_AVM_SYSTEM_s *pData);
void AmbaCT_AvmSetSystem(const AMBA_CT_AVM_SYSTEM_s *pData);
void AmbaCT_AvmGetCar(AMBA_CAL_SIZE_s *pData);
void AmbaCT_AvmSetCar(const AMBA_CAL_SIZE_s *pData);
void AmbaCT_AvmGet2dViewRange(AMBA_CT_AVM_2D_VIEW_s *pData);
void AmbaCT_AvmSet2dViewRange(const AMBA_CT_AVM_2D_VIEW_s *pData);
void AmbaCT_AvmGet2dCustomCalib(UINT32 ArrayIndex, AMBA_CT_AVM_2D_CUSTOM_CALIB_s *pData);
void AmbaCT_AvmSet2dCustomCalib(const UINT32 ArrayIndex,const AMBA_CT_AVM_2D_CUSTOM_CALIB_s *pData);
void AmbaCT_AvmGet3dViewModelType(AMBA_CT_AVM_3D_VIEW_MODEL_TYPE_s *pData);
void AmbaCT_AvmSet3dViewModelType(const AMBA_CT_AVM_3D_VIEW_MODEL_TYPE_s *pData);
void AmbaCT_AvmGet3dView(AMBA_CT_AVM_3D_VIEW_s *pData);
void AmbaCT_AvmSet3dView(const AMBA_CT_AVM_3D_VIEW_s *pData);
void AmbaCT_AvmGet3dAdvView(AMBA_CT_AVM_3D_ADV_VIEW_s *pData);
void AmbaCT_AvmSet3dAdvView(const AMBA_CT_AVM_3D_ADV_VIEW_s *pData);
void AmbaCT_AvmGet3dVirtualCam(AMBA_CAL_VIRTUAL_CAM_s *pData);
void AmbaCT_AvmSet3dVirtualCam(const AMBA_CAL_VIRTUAL_CAM_s *pData);
void AmbaCT_AvmGet3dWarp(UINT32 ArrayIndex, AMBA_CAL_AVM_3D_WARP_CAL_TYPE_e *pData);
void AmbaCT_AvmSet3dWarp(const UINT32 ArrayIndex,const AMBA_CAL_AVM_3D_WARP_CAL_TYPE_e *pData);
void AmbaCT_AvmGetViewBlendV2(AMBA_CAL_AVM_BLEND_CFG_V2_s *pData);
void AmbaCT_AvmSetViewBlendV2(const AMBA_CAL_AVM_BLEND_CFG_V2_s *pData);
void AmbaCT_AvmGetMainView(UINT8 *pData);
void AmbaCT_AvmSetMainView(const UINT8 *pData);
void AmbaCT_AvmGetMVPerspectiveCfg(AMBA_CAL_AVM_MV_PERSPET_CFG_s *pData);
void AmbaCT_AvmSetMVPerspectiveCfg(const AMBA_CAL_AVM_MV_PERSPET_CFG_s *pData);
void AmbaCT_AvmGetMVPerspectiveV1Cfg(AMBA_CAL_AVM_MV_PERSPET_V1_CFG_s *pData);
void AmbaCT_AvmSetMVPerspectiveV1Cfg(const AMBA_CAL_AVM_MV_PERSPET_V1_CFG_s *pData);
void AmbaCT_AvmGetMVEqualDistCfg(AMBA_CAL_AVM_MV_EQ_DISTANCE_s *pData);
void AmbaCT_AvmSetMVEqualDistCfg(const AMBA_CAL_AVM_MV_EQ_DISTANCE_s *pData);
void AmbaCT_AvmGetMVLdcCfg(AMBA_CAL_AVM_MV_LDC_CFG_s *pData);
void AmbaCT_AvmSetMVLdcCfg(const AMBA_CAL_AVM_MV_LDC_CFG_s *pData);
void AmbaCT_AvmGetMVDirection(AMBA_CAL_POINT_DB_3D_s *pData);
void AmbaCT_AvmSetMVDirection(const AMBA_CAL_POINT_DB_3D_s *pData);

void AmbaCT_AvmGetOsdMask(AMBA_CT_AVM_OSD_MASK_s **pData);
void AmbaCT_AvmSetOsdMask(const AMBA_CT_AVM_OSD_MASK_s *pData);
void AmbaCT_AvmGetLens(UINT32 ArrayIndex, AMBA_CT_AVM_LENS_s *pData);
void AmbaCT_AvmSetLens(const UINT32 ArrayIndex,const AMBA_CT_AVM_LENS_s *pData);
void AmbaCT_AvmGetSensor(UINT32 ArrayIndex, AMBA_CAL_SENSOR_s *pData);
void AmbaCT_AvmSetSensor(const UINT32 ArrayIndex,const AMBA_CAL_SENSOR_s *pData);
void AmbaCT_AvmGetOpticalCenter(UINT32 ArrayIndex, AMBA_CAL_POINT_DB_2D_s *pData);
void AmbaCT_AvmSetOpticalCenter(const UINT32 ArrayIndex,const AMBA_CAL_POINT_DB_2D_s *pData);
void AmbaCT_AvmGetCamera(UINT32 ArrayIndex, AMBA_CT_AVM_CAMERA_s *pData);
void AmbaCT_AvmSetCamera(const UINT32 ArrayIndex,const AMBA_CT_AVM_CAMERA_s *pData);
void AmbaCT_AvmSetVin(const UINT32 ArrayIndex,const AMBA_IK_VIN_SENSOR_GEOMETRY_s *pData);
void AmbaCT_AvmGetVin(UINT32 ArrayIndex, AMBA_IK_VIN_SENSOR_GEOMETRY_s *pData);
void AmbaCT_AvmGetCalibPoints(UINT32 ArrayIndex, AMBA_CT_AVM_CALIB_POINTS_s *pData);
void AmbaCT_AvmSetCalibPoints(const UINT32 ArrayIndex,const AMBA_CT_AVM_CALIB_POINTS_s *pData);
void AmbaCT_AvmGetAssistancePoints(UINT32 ArrayIndex, AMBA_CT_AVM_ASSISTANCE_POINTS_s *pData);
void AmbaCT_AvmSetAssistancePoints(const UINT32 ArrayIndex,const AMBA_CT_AVM_ASSISTANCE_POINTS_s *pData);
void AmbaCT_AvmGetRoi(UINT32 ArrayIndex, AMBA_CAL_ROI_s *pData);
void AmbaCT_AvmSetRoi(const UINT32 ArrayIndex,const AMBA_CAL_ROI_s *pData);
void AmbaCT_AvmGetMainSize(UINT32 ArrayIndex, AMBA_CAL_SIZE_s *pData);
void AmbaCT_AvmSetMainSize(const UINT32 ArrayIndex,const AMBA_CAL_SIZE_s *pData);
void AmbaCT_AvmGetMinVoutSize(UINT32 ArrayIndex, AMBA_CAL_SIZE_s *pData);
void AmbaCT_AvmSetMinVoutSize(const UINT32 ArrayIndex,const AMBA_CAL_SIZE_s *pData);
void AmbaCT_AvmGetMinRoiSize(UINT32 ArrayIndex, AMBA_CAL_SIZE_s *pData);
void AmbaCT_AvmSetMinRoiSize(const UINT32 ArrayIndex,const AMBA_CAL_SIZE_s *pData);
void AmbaCT_AvmGetTileSize(UINT32 ArrayIndex, AMBA_CAL_SIZE_s *pData);
void AmbaCT_AvmSetTileSize(const UINT32 ArrayIndex,const AMBA_CAL_SIZE_s *pData);
void AmbaCT_AvmGetAuto(UINT32 ArrayIndex, AMBA_CT_AVM_AUTO_s *pData);
void AmbaCT_AvmSetAuto(const UINT32 ArrayIndex,const AMBA_CT_AVM_AUTO_s *pData);
void AmbaCT_AvmGetOptimize(UINT32 ArrayIndex, UINT8 *pData);
void AmbaCT_AvmSetOptimize(const UINT32 ArrayIndex,const UINT8 *pData);
void AmbaCT_AvmGetInternal(UINT32 ArrayIndex, AMBA_CAL_AVM_INTERNAL_CFG_s *pData);
void AmbaCT_AvmSetInternal(const UINT32 ArrayIndex,const AMBA_CAL_AVM_INTERNAL_CFG_s *pData);
void AmbaCT_AvmGetPreCheckData(AMBA_CT_AVM_PRE_CHECK_s *pData);
void AmbaCT_AvmSetPreCheckData(const AMBA_CT_AVM_PRE_CHECK_s *pData);
void AmbaCT_AvmGetCalib(AMBA_CT_AVM_CALIB_s *pData);
void AmbaCT_AvmSetCalib(const AMBA_CT_AVM_CALIB_s *pData);

void AmbaCT_AvmGetWorkingBufSize(SIZE_t *pSize);
UINT32 AmbaCT_AvmTunerInit(const AMBA_CT_AVM_INIT_CFG_s *pInitCfg);
UINT8 AmbaCT_AvmGetGroupStatus(UINT8 GroupId);

UINT32 AmbaCT_AvmExecute(void);
#endif
