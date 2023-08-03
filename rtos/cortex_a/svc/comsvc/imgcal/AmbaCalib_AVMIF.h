/**
 *  @file AmbaCalib_AVMIF.h
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
#ifndef AMBA_CALIB_AVM_API_H
#define AMBA_CALIB_AVM_API_H
#include "AmbaTypes.h"
#include "AmbaCalib_Def.h"
#include "AmbaCalib_AVMDef.h"
#include "AmbaCalib_WarpDef.h"
#include "AmbaCalib_LDCDef.h"

#define AMBA_CAL_AVM_DATA_VERSION (1U)

#define AMBA_CAL_AVM_2D_PRECAL_DATA_VER (3U)
#define AMBA_CAL_AVM_3D_PRECAL_DATA_VER (6U)

#define MAX_PRE_CHECK_VIEW_NUM (121 * 4)
#define AMBA_CAL_AVM_PRE_CHECK_OK (0)
#define AMBA_CAL_AVM_PRE_CHECK_NG (1)


typedef enum {
    AMBA_CAL_AVM_OPTIMIZE_LV_0 = 0, // without optimize
    AMBA_CAL_AVM_OPTIMIZE_LV_1, // optimize OC
    AMBA_CAL_AVM_OPTIMIZE_LV_2, // optimize H matrix
    AMBA_CAL_AVM_OPTIMIZE_LV_3, // optimize OC + H matrix
    AMBA_CAL_AVM_OPTIMIZE_LV_MAX
} AMBA_CAL_AVM_OPTIMIZE_LEVEL_e;

typedef enum {
    AMBA_CAL_AVM_BIRDVIEW = 0,
    AMBA_CAL_AVM_3DVIEW,
    AMBA_CAL_AVM_MV_PERSPECTIVE,
    AMBA_CAL_AVM_MV_EQUAL_DISTANCE,
    AMBA_CAL_AVM_MV_LDC,
    AMBA_CAL_AVM_MV_FLOOR,
} AMBA_CAL_AVM_VIEW_TYPE_e;

typedef struct {
    AMBA_CAL_WORLD_RAW_POINT_s CalibPoints[AVM_CALIB_POINT_NUM];
    UINT32 AssistancePointNumber;
    AMBA_CAL_WORLD_RAW_POINT_s AssistancePoints[MAX_AVM_ASSISTANCE_POINT_NUM];
} AMBA_CAL_AVM_POINT_MAP_s;

typedef struct {
    UINT32 DebugID;
    UINT32 RefineDmaEnable;
    UINT32 RefineWaitlineEnable;
    UINT32 RefineOutOfRoiEnable;
    UINT32 RefineVFlipEnable;
    UINT32 ExtrapolatePlaneBoundary;
    UINT32 RepeatUnusedArea;
    UINT32 WaitlineTh;
    UINT32 DmaTh;
} AMBA_CAL_AVM_INTERNAL_CFG_s;

typedef enum {
    AVM_3D_PLUGIN_MODE_MSG_RECEIVER = 0x01,
    AVM_3D_PLUGIN_MODE_SAVE_INT_DATA = 0x02,
    AVM_3D_PLUGIN_MODE_FEED_INT_DATA = 0x04,
    AVM_3D_PLUGIN_MODE_CUSTOM_CALIB = 0x08,
    AVM_3D_PLUGIN_MODE_SAVE_PRECHECK = 0x10,
    AVM_3D_PLUGIN_MODE_CUSTOM_W_TO_I = 0x20,
    AVM_3D_PLUGIN_MODE_SAVE_CAR_VOUT = 0x40,
} AMBA_CAL_AVM_3D_PLUGIN_MODE_e;

typedef enum {
    AVM_2D_PLUGIN_MODE_MSG_RECEIVER = 0x01,
    AVM_2D_PLUGIN_MODE_SAVE_INT_DATA = 0x02,
    AVM_2D_PLUGIN_MODE_FEED_INT_DATA = 0x04,
    AVM_2D_PLUGIN_MODE_CUSTOM_CALIB = 0x08,
    AVM_2D_PLUGIN_MODE_SAVE_PRECHECK = 0x10,
    AVM_2D_PLUGIN_MODE_CUSTOM_W_TO_I = 0x20
} AMBA_CAL_AVM_2D_PLUGIN_MODE_e;

typedef enum {
    AVM_MV_PLUGIN_MODE_MSG_RECEIVER = 0x01,
    AVM_MV_PLUGIN_MODE_CUSTOM_CALIB = 0x02,
    AVM_MV_PLUGIN_MODE_SAVE_PRECHECK = 0x04,
    AVM_MV_PLUGIN_MODE_CUSTOM_W_TO_I = 0x08
} AMBA_CAL_AVM_MV_PLUGIN_MODE_e;

typedef enum {
    AVM_CALIB_PLUGIN_CUSTOM_CALIB = 0x01,
    AVM_CALIB_PLUGIN_ERROR_REPORT = 0x02,
} AMBA_CAL_AVM_CALIB_PLUGIN_MODE_e;

typedef struct {
    UINT32 Version;
    UINT32 NumOfPtr;
    void *pDataPtr[16];
    UINT32 DataSize[16];
} AMBA_CAL_AVM_PRE_CAL_DATA_s;

typedef struct {
    UINT32 ChID;
    AMBA_CAL_ROI_s ManualROI;
    UINT16 WallWorldsLength;
    AMBA_CAL_POINT_DB_2D_s WallWorldsPos[MAX_PRE_CHECK_DATA_LEN];
    UINT16 GroundWorldsLength;
    AMBA_CAL_POINT_DB_2D_s GroundWorldsPos[MAX_PRE_CHECK_DATA_LEN];
} AMBA_CAL_AVM_3D_PRE_CHECK_DATA_s;

typedef struct {
    /*
    union {
        AMBA_CAL_AVM_PRE_CHECK_2D_TBL_s PosMatMode;
        AMBA_CAL_AVM_PRE_CHECK_3D_TBL_s AngleMode;
    } PreCheckData;
    */
    AMBA_CAL_AVM_3D_PRE_CHECK_V2_UNION_s PreCheckData;
} AMBA_CAL_AVM_3D_PRE_CHECK_V2_s;

typedef struct {
    UINT32 ChID;
    AMBA_CAL_ROI_s ManualROI;
    UINT16 GroundWorldsLength;
    AMBA_CAL_POINT_DB_2D_s GroundWorldsPos[MAX_PRE_CHECK_DATA_LEN];
} AMBA_CAL_AVM_2D_PRE_CHECK_DATA_s;

typedef struct {
    UINT32 ChID;
    AMBA_CAL_ROI_s ManualROI;
    DOUBLE APCoef[4];
    UINT16 WallWorldsLength;
    AMBA_CAL_POINT_DB_2D_s WallWorldsPos[MAX_PRE_CHECK_DATA_LEN];
} AMBA_CAL_AVM_MV_PRE_CHECK_DATA_s;

typedef struct {
    UINT32 ChID;
    UINT32 Status;
} AMBA_CAL_AVM_PRE_CHECK_STATUS_s;


typedef struct {
    UINT32 PreCheckNumber;
    AMBA_CAL_AVM_PRE_CHECK_STATUS_s *pPreCheckResults;
} AMBA_CAL_AVM_PRE_CHECK_RESULT_s;


typedef struct {
    UINT32 PlugInMode;

    UINT32 (*pSavePreCalData)(AMBA_CAL_AVM_CAM_ID_e CamId, const AMBA_CAL_AVM_PRE_CAL_DATA_s *pData);
    UINT32 (*pFeedPreCalData)(AMBA_CAL_AVM_CAM_ID_e CamId, AMBA_CAL_AVM_PRE_CAL_DATA_s *pData);

    UINT32 (*pInsertCustomCalibMat)(AMBA_CAL_AVM_CAM_ID_e CamId, DOUBLE CalibMat[9]);
    UINT32 (*pSavePreCheckData)(AMBA_CAL_AVM_CAM_ID_e CamId, const void *pData, UINT32 SaveLength);
    UINT32 (*pGetWorldPtOnImage)(AMBA_CAL_AVM_CAM_ID_e CamId, const AMBA_CAL_POINT_DB_3D_s *pWorldPt, AMBA_CAL_POINT_DB_2D_s *pImagePt);

    // TODO:
    // FindPos
    struct {
        UINT32 GridStatus;
        UINT32 PatternError;
        UINT32 Report;
    } MsgReciverCfg;
    AMBA_CAL_AVM_CB_MSG_RECIVER_s MsgReciver;
} AMBA_CAL_AVM_2D_PLUG_IN_s;

typedef struct {
    UINT32 PlugInMode;

    UINT32 (*pSavePreCalData)(AMBA_CAL_AVM_CAM_ID_e CamId, const AMBA_CAL_AVM_PRE_CAL_DATA_s *pData);
    UINT32 (*pFeedPreCalData)(AMBA_CAL_AVM_CAM_ID_e CamId, AMBA_CAL_AVM_PRE_CAL_DATA_s *pData);
    UINT32 (*pInsertCustomCalibMat)(AMBA_CAL_AVM_CAM_ID_e CamId, DOUBLE CalibMat[9]);
    UINT32 (*pSavePreCheckData)(AMBA_CAL_AVM_CAM_ID_e CamId, const void *pData, UINT32 SaveLength);
    UINT32 (*pGetWorldPtOnImage)(AMBA_CAL_AVM_CAM_ID_e CamId, const AMBA_CAL_POINT_DB_3D_s *pWorldPt, AMBA_CAL_POINT_DB_2D_s *pImagePt);

    // TODO:
    struct {
        UINT32 GridStatus;
        UINT32 PatternError;
        UINT32 Report;
    } MsgReciverCfg;
    AMBA_CAL_AVM_CB_MSG_RECIVER_s MsgReciver;

} AMBA_CAL_AVM_3D_PLUG_IN_s;

typedef struct {
    UINT32 PlugInMode;

    UINT32 (*pSavePreCalData)(AMBA_CAL_AVM_CAM_ID_e CamId, const AMBA_CAL_AVM_PRE_CAL_DATA_s *pData);
    UINT32 (*pFeedPreCalData)(AMBA_CAL_AVM_CAM_ID_e CamId, AMBA_CAL_AVM_PRE_CAL_DATA_s *pData);
    UINT32 (*pInsertCustomCalibMat)(AMBA_CAL_AVM_CAM_ID_e CamId, DOUBLE CalibMat[9]);
    UINT32 (*pSavePreCheckData)(AMBA_CAL_AVM_CAM_ID_e CamId, const void *pData, UINT32 SaveLength);
    UINT32 (*pGetWorldPtOnImage)(AMBA_CAL_AVM_CAM_ID_e CamId, const AMBA_CAL_POINT_DB_3D_s *pWorldPt, AMBA_CAL_POINT_DB_2D_s *pImagePt);
    UINT32 (*pSaveCarVout4Points)(const AMBA_CAL_POINT_DB_2D_s CarVout4Points[4U]);

    // TODO:
    struct {
        UINT32 GridStatus;
        UINT32 PatternError;
        UINT32 Report;
    } MsgReciverCfg;
    AMBA_CAL_AVM_CB_MSG_RECIVER_s MsgReciver;

} AMBA_CAL_AVM_3D_PLUG_IN_V2_s;

typedef struct {
    UINT32 PlugInMode;

    UINT32 (*pInsertCustomCalibMat)(AMBA_CAL_AVM_CAM_ID_e CamId, DOUBLE CalibMat[9]);
    UINT32 (*pSavePreCheckData)(AMBA_CAL_AVM_CAM_ID_e CamId, const void *pData, UINT32 SaveLength);
    UINT32 (*pGetWorldPtOnImage)(AMBA_CAL_AVM_CAM_ID_e CamId, const AMBA_CAL_POINT_DB_3D_s *pWorldPt, AMBA_CAL_POINT_DB_2D_s *pImagePt);

    // TODO:
    AMBA_CAL_AVM_CB_MSG_RECIVER_s MsgReciver;
} AMBA_CAL_AVM_MV_PLUG_IN_s;

typedef struct {
    UINT32 DisplayHeight;
    UINT32 AutoVoutOrderEnable;
    //UINT32 ViewRange[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_AVM_VIEW_LAYOUT_s ViewRange;
    AMBA_CAL_AVM_BLEND_CFG_s Blend;
} AMBA_CAL_AVM_2D_VIEW_CFG_s;

typedef struct {
    UINT32 DisplayHeight;
    UINT32 AutoVoutOrderEnable;
    //UINT32 ViewRange[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_AVM_VIEW_LAYOUT_s ViewRange;
    AMBA_CAL_AVM_BLEND_CFG_V2_s Blend;
} AMBA_CAL_AVM_2D_VIEW_CFG_V2_s;

typedef struct {
    AMBA_CAL_CAM_s Cam;
    AMBA_IK_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_AVM_POINT_MAP_s PointMap;
    AMBA_CAL_ROI_s ROI;
    AMBA_CAL_SIZE_s Tile;
    AMBA_CAL_SIZE_s Main;
    AMBA_CAL_SIZE_s MinVout;
    UINT32 AutoROI;
    UINT32 AutoFrontEndRotation;
    AMBA_CAL_AVM_OPTIMIZE_LEVEL_e OptimizeLevel;
    AMBA_CAL_AVM_INTERNAL_CFG_s InternalCfg;
    AMBA_CAL_AVM_2D_PLUG_IN_s PlugIn;
} AMBA_CAL_AVM_2D_CAM_CAL_INFO_s;

typedef struct {
    AMBA_CAL_CAM_V2_s Cam;
    AMBA_IK_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_AVM_POINT_MAP_s PointMap;
    AMBA_CAL_ROI_s ROI;
    AMBA_CAL_SIZE_s Tile;
    AMBA_CAL_SIZE_s Main;
    AMBA_CAL_SIZE_s MinVout;
    AMBA_CAL_SIZE_s MinRoiSize;
    UINT32 AutoROI;
    UINT32 AutoFrontEndRotation;
    AMBA_CAL_AVM_OPTIMIZE_LEVEL_e OptimizeLevel;
    AMBA_CAL_AVM_INTERNAL_CFG_s InternalCfg;
    AMBA_CAL_AVM_2D_PLUG_IN_s PlugIn;
} AMBA_CAL_AVM_2D_CAM_CAL_INFO_V2_s;

typedef struct {
    AMBA_CAL_CAM_s Cam;
    AMBA_IK_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_AVM_POINT_MAP_s PointMap;
    AMBA_CAL_ROI_s ROI;
    AMBA_CAL_SIZE_s Tile;
    AMBA_CAL_SIZE_s Main;
    AMBA_CAL_SIZE_s MinVout;
    UINT32 AutoROI;
    UINT32 AutoFrontEndRotation;
    AMBA_CAL_AVM_OPTIMIZE_LEVEL_e OptimizeLevel;
    AMBA_CAL_AVM_INTERNAL_CFG_s InternalCfg;
    AMBA_CAL_AVM_3D_PLUG_IN_s PlugIn;
    AMBA_CAL_AVM_3D_WARP_CAL_TYPE_e WarpCalType;
} AMBA_CAL_AVM_3D_CAM_CAL_INFO_s;

typedef struct {
    AMBA_CAL_CAM_V2_s Cam;
    AMBA_IK_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_AVM_POINT_MAP_s PointMap;
    AMBA_CAL_ROI_s ROI;
    AMBA_CAL_SIZE_s Tile;
    AMBA_CAL_SIZE_s Main;
    AMBA_CAL_SIZE_s MinVout;
    AMBA_CAL_SIZE_s MinRoiSize;
    UINT32 AutoROI;
    UINT32 AutoFrontEndRotation;
    AMBA_CAL_AVM_OPTIMIZE_LEVEL_e OptimizeLevel;
    AMBA_CAL_AVM_INTERNAL_CFG_s InternalCfg;
    AMBA_CAL_AVM_3D_PLUG_IN_V2_s PlugIn;
    AMBA_CAL_AVM_3D_WARP_CAL_TYPE_e WarpCalType;
} AMBA_CAL_AVM_3D_CAM_CAL_INFO_V2_s;

typedef struct {
    AMBA_CAL_CAM_s Cam;
    AMBA_CAL_POINT_DB_3D_s Direction;
    AMBA_IK_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_AVM_POINT_MAP_s PointMap;
    AMBA_CAL_ROI_s ROI;
    AMBA_CAL_SIZE_s Tile;
    AMBA_CAL_AVM_OPTIMIZE_LEVEL_e OptimizeLevel;
    AMBA_CAL_AVM_INTERNAL_CFG_s InternalCfg;
    AMBA_CAL_AVM_MV_PLUG_IN_s PlugIn;
} AMBA_CAL_AVM_MV_CAM_CAL_INFO_s;

typedef struct {
    AMBA_CAL_CAM_V2_s Cam;
    AMBA_CAL_POINT_DB_3D_s Direction;
    AMBA_IK_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_AVM_POINT_MAP_s PointMap;
    AMBA_CAL_ROI_s ROI;
    AMBA_CAL_SIZE_s Tile;
    AMBA_CAL_AVM_OPTIMIZE_LEVEL_e OptimizeLevel;
    AMBA_CAL_AVM_INTERNAL_CFG_s InternalCfg;
    AMBA_CAL_AVM_MV_PLUG_IN_s PlugIn;
} AMBA_CAL_AVM_MV_CAM_CAL_INFO_V2_s;

typedef struct {
    AMBA_CAL_SIZE_s Car;
    AMBA_CAL_AVM_CAR_PATTERN_s CarVoutOSD;
    AMBA_CAL_AVM_2D_VIEW_CFG_s View;
    AMBA_CAL_AVM_2D_CAM_CAL_INFO_s Cam[AMBA_CAL_AVM_CAM_MAX];
} AMBA_CAL_AVM_2D_CFG_s;

typedef struct {
    AMBA_CAL_SIZE_s Car;
    AMBA_CAL_AVM_CAR_PATTERN_s CarVoutOSD;
    AMBA_CAL_AVM_2D_VIEW_CFG_V2_s View;
    AMBA_CAL_AVM_2D_CAM_CAL_INFO_V2_s Cam[AMBA_CAL_AVM_CAM_MAX];
} AMBA_CAL_AVM_2D_CFG_V2_s;

typedef struct {
    DOUBLE StartDegreesX;
    DOUBLE StartDegreesY;
    DOUBLE TotalViewAngleX;
    DOUBLE TotalViewAngleY;
    UINT32 DisplayWidth;
    UINT32 DisplayHeight;
    DOUBLE ProjectionRotationDegreeH;
    DOUBLE ProjectionRotationDegreeV;
} AMBA_CAL_AVM_MV_PERSPET_CFG_s;

typedef struct {
    DOUBLE StartDegreesX;
    DOUBLE StartDegreesY;
    DOUBLE TotalViewAngleX;
    DOUBLE TotalViewAngleY;
    UINT32 DisplayWidth;
    UINT32 DisplayHeight;
    DOUBLE ProjectionRotationDegreeH;
    DOUBLE ProjectionRotationDegreeV;
    DOUBLE ProjectionRotationDegreeF;
    AMBA_CAL_LENS_DST_REAL_EXPECT_s LdeRealExpectUpper;
    AMBA_CAL_LENS_DST_REAL_EXPECT_s LdeRealExpectLower;
    AMBA_CAL_LENS_DST_REAL_EXPECT_s LdeRealExpectLeft;
    AMBA_CAL_LENS_DST_REAL_EXPECT_s LdeRealExpectRight;
    AMBA_CAL_POINT_DB_2D_s LdeOpticalCenter;
} AMBA_CAL_AVM_MV_PERSPET_V1_CFG_s;

typedef struct {
    DOUBLE StartDegreesX;
    DOUBLE StartDegreesY;
    DOUBLE TotalViewAngleX;
    DOUBLE TotalViewAngleY;
    UINT32 DisplayWidth;
    UINT32 DisplayHeight;
} AMBA_CAL_AVM_MV_EQ_DISTANCE_s;

typedef struct {
    UINT32 CompensateRatio;
} AMBA_CAL_AVM_MV_LDC_CFG_s;

typedef struct {
    AMBA_CAL_ROI_s ViewRange;
} AMBA_CAL_AVM_MV_FLOOR_CFG_s;

typedef struct {
    AMBA_CAL_AVM_VIEW_TYPE_e Type;
    struct {
        AMBA_CAL_AVM_MV_PERSPET_CFG_s *pPerspective;
        AMBA_CAL_AVM_MV_EQ_DISTANCE_s *pEqualDistance;
        AMBA_CAL_AVM_MV_LDC_CFG_s *pLDC;
        AMBA_CAL_AVM_MV_FLOOR_CFG_s *pFloor;
    } Cfg;
} AMBA_CAL_AVM_MV_VIEW_CFG_s;

typedef struct {
    AMBA_CAL_AVM_VIEW_TYPE_e Type;
    struct {
        AMBA_CAL_AVM_MV_PERSPET_V1_CFG_s *pPerspective;
        AMBA_CAL_AVM_MV_EQ_DISTANCE_s *pEqualDistance;
        AMBA_CAL_AVM_MV_LDC_CFG_s *pLDC;
        AMBA_CAL_AVM_MV_FLOOR_CFG_s *pFloor;
    } Cfg;
} AMBA_CAL_AVM_MV_VIEW_CFG_V1_s;

typedef struct {
    AMBA_CAL_AVM_MV_VIEW_CFG_s View;
    AMBA_CAL_AVM_MV_CAM_CAL_INFO_s Cam;
} AMBA_CAL_AVM_MV_CFG_s;

typedef struct {
    AMBA_CAL_AVM_MV_VIEW_CFG_V1_s View;
    AMBA_CAL_AVM_MV_CAM_CAL_INFO_s Cam;
} AMBA_CAL_AVM_MV_CFG_V1_s;

typedef struct {
    AMBA_CAL_AVM_MV_VIEW_CFG_V1_s View;
    AMBA_CAL_AVM_MV_CAM_CAL_INFO_V2_s Cam;
} AMBA_CAL_AVM_MV_CFG_V2_s;


typedef struct {
    DOUBLE Calib2DMatrix[9];
    INT32 Calib2DMatrixZSign;
} AMBA_CAL_AVM_CALIB_DATA_s;

typedef struct {
    DOUBLE Calib2DMatrix[9];
    INT32 Calib2DMatrixZSign;
    AMBA_CAL_POINT_DB_3D_s RVec;
    AMBA_CAL_POINT_DB_3D_s TVec;
    AMBA_CAL_POINT_DB_3D_s CamFocusPos;
    AMBA_CAL_POINT_DB_3D_s CamDir;
    DOUBLE CameraMatrix[12];
} AMBA_CAL_AVM_CALIB_DATA_V2_s;

typedef struct {
    AMBA_CAL_CAM_s Cam[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_AVM_CALIB_DATA_s CalibData[AMBA_CAL_AVM_CAM_MAX];
} AMBA_CAL_AVM_PRE_CHECK_CALIB_DATA_s;

typedef struct {
    AMBA_CAL_CAM_V2_s Cam[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_AVM_CALIB_DATA_s CalibData[AMBA_CAL_AVM_CAM_MAX];
} AMBA_CAL_AVM_PRE_CHK_CAL_DA_V2_s;

typedef struct {
    UINT32 Version;
    struct {
        AMBA_CAL_WARP_CALIB_DATA_s WarpTbl;
        AMBA_CAL_AVM_BLEND_TBL_s BlendTbl;
        AMBA_CAL_TBL_STATUS_e WarpTblStatus;
        AMBA_CAL_TBL_STATUS_e BlendTblStatus;
        UINT32 VoutOrder;
        AMBA_CAL_ROI_s VoutArea;
        AMBA_CAL_ROTATION_e FrontendRotation;
        AMBA_CAL_ROTATION_e VoutRotation;
        DOUBLE CalibMatrix[9];
        DOUBLE WorldScaleMatrix[9];
    } Cam[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_ROI_s OverlapArea[AMBA_CAL_AVM_BlEND_CORNER_MAX];
    UINT32 VoutCarOSDCornerNum;
    AMBA_CAL_POINT_INT_2D_s VoutCarOSDCorner[AMBA_CAL_CAR_OSD_CORNER_MAX];
} AMBA_CAL_AVM_2D_DATA_s;

typedef struct {
    UINT32 Version;
    struct {
        AMBA_CAL_WARP_CALIB_DATA_s WarpTbl;
        AMBA_CAL_AVM_BLEND_TBL_s BlendTbl;
        AMBA_CAL_TBL_STATUS_e WarpTblStatus;
        AMBA_CAL_TBL_STATUS_e BlendTblStatus;
        UINT32 VoutOrder;
        AMBA_CAL_ROI_s VoutArea;
        AMBA_CAL_ROTATION_e FrontendRotation;
        AMBA_CAL_ROTATION_e VoutRotation;
        DOUBLE CalibMatrix[9];
    } Cam[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_ROI_s OverlapArea[AMBA_CAL_AVM_BlEND_CORNER_MAX];
    UINT32 VoutCarOSDCornerNum;
    AMBA_CAL_POINT_INT_2D_s VoutCarOSDCorner[AMBA_CAL_CAR_OSD_CORNER_MAX];
} AMBA_CAL_AVM_3D_DATA_s;

typedef struct {
    UINT32 Version;
    AMBA_CAL_WARP_CALIB_DATA_s WarpTbl;
    AMBA_CAL_ROI_s VoutArea; // TODO
    AMBA_CAL_ROTATION_e FrontendRotation;
    AMBA_CAL_ROTATION_e VoutRotation;
    DOUBLE CalibMatrix[9];
} AMBA_CAL_AVM_MV_DATA_s;

typedef struct {
    AMBA_CAL_AVM_VIEW_TYPE_e Type;
    AMBA_CAL_ROI_s RawArea;
    struct {
        AMBA_CAL_AVM_MV_PERSPET_CFG_s *pPerspective;
        AMBA_CAL_AVM_MV_EQ_DISTANCE_s *pEqualDistance;
        AMBA_CAL_AVM_MV_LDC_CFG_s *pLDC;
        AMBA_CAL_AVM_MV_FLOOR_CFG_s *pFloor;
    } Cfg;
    AMBA_CAL_CAM_s Cam;
    AMBA_CAL_POINT_DB_3D_s CamDirection;
    DOUBLE HMatrix[9]; // Reserved
    DOUBLE PMatrix[9];
} AMBA_CAL_AVM_PARK_ASSIST_CFG_s;

typedef struct {
    AMBA_CAL_AVM_VIEW_TYPE_e Type;
    AMBA_CAL_ROI_s RawArea;
    struct {
        AMBA_CAL_AVM_MV_PERSPET_V1_CFG_s *pPerspective;
        AMBA_CAL_AVM_MV_EQ_DISTANCE_s *pEqualDistance;
        AMBA_CAL_AVM_MV_LDC_CFG_s *pLDC;
        AMBA_CAL_AVM_MV_FLOOR_CFG_s *pFloor;
    } Cfg;
    AMBA_CAL_CAM_s Cam;
    AMBA_CAL_POINT_DB_3D_s CamDirection;
    DOUBLE HMatrix[9]; // Reserved
    DOUBLE PMatrix[9];
} AMBA_CAL_AVM_PARK_AST_CFG_V1_s;

typedef struct {
    AMBA_CAL_AVM_VIEW_TYPE_e Type;
    AMBA_CAL_ROI_s RawArea;
    struct {
        AMBA_CAL_AVM_MV_PERSPET_V1_CFG_s *pPerspective;
        AMBA_CAL_AVM_MV_EQ_DISTANCE_s *pEqualDistance;
        AMBA_CAL_AVM_MV_LDC_CFG_s *pLDC;
        AMBA_CAL_AVM_MV_FLOOR_CFG_s *pFloor;
    } Cfg;
    AMBA_CAL_CAM_V2_s Cam;
    AMBA_CAL_POINT_DB_3D_s CamDirection;
    DOUBLE HMatrix[9]; // Reserved
    DOUBLE PMatrix[9];
} AMBA_CAL_AVM_PARK_AST_CFG_V2_s;


typedef struct {
    UINT32 TopFrontRange;
    UINT32 TopBackRange;
    UINT32 TopLeftRange;
    UINT32 TopRightRange;
    UINT32 TopFrontLeftRadiusX;
    UINT32 TopFrontLeftRadiusY;
    UINT32 TopFrontRightRadiusX;
    UINT32 TopFrontRightRadiusY;
    UINT32 TopBackLeftRadiusX;
    UINT32 TopBackLeftRadiusY;
    UINT32 TopBackRightRadiusX;
    UINT32 TopBackRightRadiusY;
    UINT32 TopHeight;
    UINT32 BottomFrontRange;
    UINT32 BottomBackRange;
    UINT32 BottomLeftRange;
    UINT32 BottomRightRange;
    UINT32 BottomFrontLeftRadiusX;
    UINT32 BottomFrontLeftRadiusY;
    UINT32 BottomFrontRightRadiusX;
    UINT32 BottomFrontRightRadiusY;
    UINT32 BottomBackLeftRadiusX;
    UINT32 BottomBackLeftRadiusY;
    UINT32 BottomBackRightRadiusX;
    UINT32 BottomBackRightRadiusY;
    UINT32 TopToBottomRadiusH;
    UINT32 TopToBottomRadiusV;
    UINT32 BottomCornerIntervalNum;
    UINT32 PillarIntervalNum;
    AMBA_CAL_POINT_DB_2D_s ViewCenter;
    DOUBLE RotationAngle;
} AMBA_CAL_3D_ADV_MODEL_CFG_s;

typedef struct {
    UINT32 FloorViewRangeX;
    UINT32 FloorViewRangeY;
    UINT32 ViewDistance;
    UINT32 ViewHeight;
    AMBA_CAL_POINT_DB_2D_s ViewCenter;
} AMBA_CAL_3D_SIM_MODEL_CFG_s;

typedef struct {
    AMBA_CAL_AVM_3D_VIEW_MODEL_e Type;
    AMBA_CAL_3D_SIM_MODEL_CFG_s SimpleModelCfg;
    AMBA_CAL_3D_ADV_MODEL_CFG_s AdvanceModelCfg;
} AMBA_CAL_3D_MODEL_CFG_s;

typedef struct {
    AMBA_CAL_3D_MODEL_CFG_s ModelCfg;
    UINT32 DisplayWidth;
    UINT32 DisplayHeight;
    AMBA_CAL_ROTATION_e DisplayRotation;
    UINT32 AutoVoutOrderEnable;
    AMBA_CAL_VIRTUAL_CAM_s VirtualCam;
    AMBA_CAL_AVM_BLEND_CFG_s Blend;
} AMBA_CAL_AVM_3D_VIEW_CFG_s;

typedef struct {
    AMBA_CAL_SIZE_s Car;
    AMBA_CAL_AVM_CAR_PATTERN_s CarVoutOSD;
    AMBA_CAL_AVM_3D_VIEW_CFG_s View;
    AMBA_CAL_AVM_3D_CAM_CAL_INFO_s Cam[AMBA_CAL_AVM_CAM_MAX];
} AMBA_CAL_AVM_3D_CFG_s;

typedef struct {
    AMBA_CAL_3D_MODEL_CFG_s ModelCfg;
    UINT32 DisplayWidth;
    UINT32 DisplayHeight;
    AMBA_CAL_ROTATION_e DisplayRotation;
    UINT32 AutoVoutOrderEnable;
    AMBA_CAL_VIRTUAL_CAM_s VirtualCam;
    AMBA_CAL_AVM_BLEND_CFG_V2_s Blend;
} AMBA_CAL_AVM_3D_VIEW_CFG_V2_s;

typedef struct {
    AMBA_CAL_SIZE_s Car;
    AMBA_CAL_AVM_CAR_PATTERN_s CarVoutOSD;
    AMBA_CAL_AVM_3D_VIEW_CFG_V2_s View;
    AMBA_CAL_AVM_3D_CAM_CAL_INFO_V2_s Cam[AMBA_CAL_AVM_CAM_MAX];
} AMBA_CAL_AVM_3D_CFG_V2_s;

typedef struct {
    UINT32 PlugInMode;
    UINT32 (*pInsertCustomCalibMat)(AMBA_CAL_AVM_CAM_ID_e CamId, DOUBLE CalibMat[9]);
    UINT32 (*pCalibErrorReport)(AMBA_CAL_AVM_CAM_ID_e CamId, UINT32 CalibPointNum, const DOUBLE ReProjectError[]);
} AMBA_CAL_AVM_CALIB_PLUG_IN_s;

typedef struct {
    AMBA_CAL_LENS_SPEC_V2_s LenSpec;
    AMBA_CAL_POINT_DB_3D_s LenPos;
    AMBA_CAL_POINT_DB_2D_s OpticalCenter;
    DOUBLE FocalLength;
    DOUBLE CellSize;
    AMBA_CAL_AVM_POINT_MAP_s *pPointMap;
    AMBA_CAL_AVM_CALIB_PLUG_IN_s PlugIn;
} AMBA_CAL_AVM_CALIB_CFG_V2_s;

typedef struct {
    UINT32 Enable;
    UINT32 IterationNum;
    DOUBLE AllowedReprojErr;
} AMBA_CAL_AVM_CALIB_RANSAC_CFG_s;

typedef struct {
    AMBA_CAL_LENS_SPEC_V2_s LenSpec;
    AMBA_CAL_POINT_DB_3D_s LenPos;
    AMBA_CAL_POINT_DB_2D_s OpticalCenter;
    DOUBLE FocalLength;
    DOUBLE CellSize;
    AMBA_CAL_AVM_POINT_MAP_s *pPointMap;
    AMBA_CAL_AVM_CALIB_PLUG_IN_s PlugIn;
    AMBA_CAL_AVM_CALIB_RANSAC_CFG_s RansacCfg;
} AMBA_CAL_AVM_CALIB_CFG_V3_s;

UINT32 AmbaCal_AvmGetMax3DPreCalSize(SIZE_t *pSize);
UINT32 AmbaCal_AvmGetMax2DPreCalSize(SIZE_t *pSize);

UINT32 AmbaCal_GenV2CalibrationInfo(UINT32 ChIdx, const void *pWorkingBuf,
    const AMBA_CAL_AVM_CALIB_CFG_V2_s *pCfg, AMBA_CAL_AVM_CALIB_DATA_V2_s *pOutput);
UINT32 AmbaCal_GenV3CalibrationInfo(UINT32 ChIdx, const void *pWorkingBuf,
    const AMBA_CAL_AVM_CALIB_CFG_V3_s *pCfg, AMBA_CAL_AVM_CALIB_DATA_V2_s *pOutput);

/**
* This API is used to generate calibration information.
* Calibration information is used by other APIs that generate warp table/blend tables.
* Calibration information is used by other APIs that calculate world position on VIN.
* @param [in] ChIdx Channel index.
* 0: Front
* 1: Back
* 2: Left
* 3: Right
* @param [in] LenSpec Lens information. Please refer to AMBA_CAL_LENS_SPEC_s for more details.
* @param [in] LenPos Position of Lens. (In world coordinate system, unit: mm).
* Please refer to AMBA_CAL_POINT_DB_3D_s for more details.
* @param [in] OpticalCenter Optical canter of Lens. (Unit: pixel).
* Please refer to AMBA_CAL_POINT_DB_2D_s for more details.
* @param [out] pPointMap Calibration points information. Please refer to AMBA_CAL_AVM_POINT_MAP_s for more details.��
* @param [in] UseCustomCalibMatrix 0: Calculate calibration matrix internally.
* 1: Use customized calibration matrix.
* @param [in] pInsertCustomCalibMat Callback function for getting customized calibration matrix. This API will be called back if UseCustomCalibMatrix = 1.
* Please refer to UINT32()(AMBA_CAL_AVM_CAM_ID_eCamId,DOUBLECalibMat[9])r more details.��
* @param [out] pOutput Returned output calibration information.
* Please refer to AMBA_CAL_AVM_CALIB_DATA_s for more details.��
* @return ErrorCode
*/
UINT32 AmbaCal_GenCalibrationInfo(INT32 ChIdx, AMBA_CAL_LENS_SPEC_s LenSpec, AMBA_CAL_POINT_DB_3D_s LenPos, AMBA_CAL_POINT_DB_2D_s OpticalCenter,
    const AMBA_CAL_AVM_POINT_MAP_s *pPointMap,
    INT32 UseCustomCalibMatrix, UINT32 (*pInsertCustomCalibMat)(AMBA_CAL_AVM_CAM_ID_e CamId, DOUBLE CalibMat[9]), AMBA_CAL_AVM_CALIB_DATA_s *pOutput);

/**
* This API is used to get required working buffer size for Pre_Check mode calculation.��
* @param [out] pSize Returned required working buffer size of Pre-Check mode for the following APIs:
* AmbaCal_Avm3DPreCheckCalibEnv()
* AmbaCal_Avm2DPreCheckCalibEnv()
* AmbaCal_AvmMVPreCheckCalibEnv()
* @return ErrorCode
*/
UINT32 AmbaCal_AvmGetPreCheckWorkSize(SIZE_t *pSize);
UINT32 AmbaCal_Avm3DPreCheckCalibEnvV2(const AMBA_CAL_AVM_PRE_CHK_CAL_DA_V2_s *pCalibData, UINT32 PreCheckVinNum, const AMBA_CAL_AVM_CALIB_DATA_V2_s *pCalibDataV2,
    const AMBA_CAL_AVM_3D_PRE_CHECK_V2_s *pPreCheckData, UINT32 PreCheckTh, const void *pWorkBuf, AMBA_CAL_AVM_PRE_CHECK_RESULT_s  *pPreCheckResult);

/**
* This API is used to validate if calibration data is feasible by testing against collected Pre-Check data.
* Export check result for each channel of each view.
* @param [in] pCalibData Calibration data. Please refer to AMBA_CAL_AVM_PRE_CHECK_CALIB_DATA_s for more details.
* @param [in] PreCheckVinNum Number of Pre-Check records to be tested.
* @param [in] pPreCheckData Pre-Check data. Please refer to AMBA_CAL_AVM_3D_PRE_CHECK_DATA_s for more details.
* @param [in] PreCheckTh Tolerance threshold of Pre-Check mode.
* @param [in] pWorkBuf Working buffer provided by user.
* @param [out] pPreCheckResult Pre-Check result. Please refer to AMBA_CAL_AVM_PRE_CHECK_RESULT_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_Avm3DPreCheckCalibEnv(const AMBA_CAL_AVM_PRE_CHECK_CALIB_DATA_s *pCalibData, UINT32 PreCheckVinNum,
    const AMBA_CAL_AVM_3D_PRE_CHECK_DATA_s *pPreCheckData, UINT32 PreCheckTh, const void *pWorkBuf, AMBA_CAL_AVM_PRE_CHECK_RESULT_s  *pPreCheckResult);

/**
* This API is used to validate if calibration data is feasible by testing against collected Pre-Check data.
* Export check result for each channel of each view.
* @param [in] pCalibData Calibration data. Please refer to AMBA_CAL_AVM_PRE_CHECK_CALIB_DATA_s for more details.
* @param [in] PreCheckVinNum Number of Pre-Check records to be tested.
* @param [in] pPreCheckData Pre-Check data. Please refer to AMBA_CAL_AVM_2D_PRE_CHECK_DATA_s for more details.
* @param [in] PreCheckTh Tolerance threshold of Pre-Check mode.
* @param [in] pWorkBuf Working buffer provided by user.
* @param [out] pPreCheckResult Pre-Check result. Please refer to AMBA_CAL_AVM_PRE_CHECK_RESULT_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_Avm2DPreCheckCalibEnv(const AMBA_CAL_AVM_PRE_CHECK_CALIB_DATA_s *pCalibData, UINT32 PreCheckVinNum,
    const AMBA_CAL_AVM_2D_PRE_CHECK_DATA_s *pPreCheckData, UINT32 PreCheckTh, const void *pWorkBuf, AMBA_CAL_AVM_PRE_CHECK_RESULT_s  *pPreCheckResult);

/**
* This API is used to validate if calibration data is feasible by testing against collected Pre-Check data.
* Export check result for each channel of each view.
* @param [in] pCalibData Calibration data. Please refer to AMBA_CAL_AVM_PRE_CHECK_CALIB_DATA_s for more details.
* @param [in] PreCheckVinNum Number of Pre-Check records to be tested.
* @param [in] pPreCheckData Pre-Check data. Please refer to AMBA_CAL_AVM_MV_PRE_CHECK_DATA_s for more details.
* @param [in] PreCheckTh Tolerance threshold of Pre-Check mode.
* @param [in] pWorkBuf Working buffer provided by user.
* @param [out] pPreCheckResult Pre-Check result. Please refer to AMBA_CAL_AVM_PRE_CHECK_RESULT_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_AvmMVPreCheckCalibEnv(const AMBA_CAL_AVM_PRE_CHECK_CALIB_DATA_s *pCalibData, UINT32 PreCheckVinNum,
    const  AMBA_CAL_AVM_MV_PRE_CHECK_DATA_s *pPreCheckData, UINT32 PreCheckTh,const void *pWorkBuf, AMBA_CAL_AVM_PRE_CHECK_RESULT_s  *pPreCheckResult);

/**
* This API is used to get the required working buffer size for generate calibration information.
* @param [out] pSize Returned required working buffer size of calibration information for AmbaCal_GenV2CalibrationInfo() API.
* @return ErrorCode
*/
UINT32 AmbaCal_AVMGetCalibInfoWorkSize(SIZE_t *pSize);

/**
* This API is used to get the required working buffer size for 2D AVM calculation.��
* @param [out] pSize Returned required working buffer size of 2D AVM mode for AmbaCal_AvmGenBirdViewTbl() API.
* @return ErrorCode
*/
UINT32 AmbaCal_AvmGetBirdViewWorkSize(SIZE_t *pSize);
UINT32 AmbaCal_AvmGenBirdViewTblV2(const AMBA_CAL_AVM_CALIB_DATA_s CalibData[AMBA_CAL_AVM_CAM_MAX],
    const AMBA_CAL_AVM_2D_CFG_V2_s *pCfg, const void *pWorkingBuf, AMBA_CAL_AVM_2D_DATA_s *pOutput);

/**
* This API is used to generate 2D AVM warp tables and blending tables based on user specified view settings and pattern positions.��
* This API return error once the input parameters are invalid
* @param [in] CalibData Input calibration data for generating tables. Please refer to AMBA_CAL_AVM_CALIB_DATA_s for more details.
* @param [in] pCfg 2D AVM related configuration. Please refer to AMBA_CAL_AVM_2D_CFG_s for more details.��
* @param [in] pWorkingBuf Working buffer provided by user. Required buffer size can be queried by AmbaCal_AvmGetBirdViewWorkSize
* @param [out] pOutput Returned output settings. Please refer to AMBA_CAL_AVM_2D_DATA_s for more details.��
* @return ErrorCode
*/
UINT32 AmbaCal_AvmGenBirdViewTbl(const AMBA_CAL_AVM_CALIB_DATA_s CalibData[AMBA_CAL_AVM_CAM_MAX],
    const AMBA_CAL_AVM_2D_CFG_s *pCfg, const void *pWorkingBuf, AMBA_CAL_AVM_2D_DATA_s *pOutput);

/**
* This API is used to get required working buffer size for 3D AVM calculation.��
* @param [out] pSize Returned required working buffer size of 3D AVM mode for AmbaCal_AvmGen3DViewTbl() API.
* @return ErrorCode
*/
UINT32 AmbaCal_AvmGet3DWorkSize(SIZE_t *pSize);

/**
* This API is used to generate 3D AVM warp tables and blend tables based on user specified view settings and pattern positions.��
* The message receiver callback function registered in MsgReciver of structure PlugIn will be callbacked when user specified message types happen.��
* @param [in] CalibData Input calibration data for generating tables. Please refer to AMBA_CAL_AVM_CALIB_DATA_s for more details.
* @param [in] pCfg 3D AVM related configuration. Please refer to AMBA_CAL_AVM_3D_CFG_s for more details.��
* @param [in] pWorkingBuf Working buffer provided by user. Required buffer size can be queried by AmbaCal_AvmGetBirdViewWorkSize
* @param [out] pOutput Returned output settings. Please refer to AMBA_CAL_AVM_3D_DATA_s for more details.��
* @return ErrorCode
*/
UINT32 AmbaCal_AvmGen3DViewTbl(const AMBA_CAL_AVM_CALIB_DATA_s CalibData[AMBA_CAL_AVM_CAM_MAX],
    const AMBA_CAL_AVM_3D_CFG_s *pCfg, const void *pWorkingBuf, AMBA_CAL_AVM_3D_DATA_s *pOutput);

/**
* This API is used to generate 3D AVM warp tables and blend tables based on user specified view settings and pattern positions.��
* The message receiver callback function registered in MsgReciver of structure PlugIn will be callbacked when user specified message types happen.��
* @param [in] CalibData Input calibration data for generating tables. Please refer to AMBA_CAL_AVM_CALIB_DATA_V2_s more details.
* @param [in] pCfg 3D AVM related configuration. Please refer to AMBA_CAL_AVM_3D_CFG_V2_s more details.��
* @param [in] pWorkingBuf Working buffer provided by user. Required buffer size can be queried by AmbaCal_AvmGetBirdViewWorkSize
* @param [out] pOutput Returned output settings. Please refer to AMBA_CAL_AVM_3D_DATA_s for more details.��
* @return ErrorCode
*/
UINT32 AmbaCal_AvmGen3DViewTblV2(const AMBA_CAL_AVM_CALIB_DATA_V2_s CalibData[AMBA_CAL_AVM_CAM_MAX],
    const AMBA_CAL_AVM_3D_CFG_V2_s *pCfg, const void *pWorkingBuf, AMBA_CAL_AVM_3D_DATA_s *pOutput);

/**
* This API is used to get required working buffer size for MainView calculation.��
* @param [out] pSize Returned required working buffer size of MainView mode for AmbaCal_AvmGenMainViewTbl() API.
* @return ErrorCode
*/
UINT32 AmbaCal_AvmGetMainViewWorkSize(SIZE_t *pSize);

/**
* This API is used to generate MainView warp tables based on user specified view settings and pattern positions.��
* @param [in] pCalibData Input calibration data for generating tables.
* Please refer to AMBA_CAL_AVM_CALIB_DATA_s for more details.��
* @param [in] pCfg MainView related configuration. Please refer to AMBA_CAL_AVM_MV_CFG_s for more details.��
* @param [in] pWorkingBuf Working buffer provided by user. Required buffer size can be queried by AmbaCal_AvmGetMainViewWorkSize
* @param [out] pOutput Returned output settings. Please refer to AMBA_CAL_AVM_MV_DATA_s for more details.��
* @return ErrorCode
*/
UINT32 AmbaCal_AvmGenMainViewTbl(const AMBA_CAL_AVM_CALIB_DATA_s *pCalibData, const AMBA_CAL_AVM_MV_CFG_s *pCfg,
    const void *pWorkingBuf, AMBA_CAL_AVM_MV_DATA_s *pOutput);

/**
* This API is used to generate MainView warp tables based on user specified view settings and pattern positions.��
* @param [in] pCalibData Input calibration data for generating tables.
* Please refer to AMBA_CAL_AVM_CALIB_DATA_s for more details.��
* @param [in] pCfg MainView related configuration. Please refer to AMBA_CAL_AVM_MV_CFG_V2_s more details.��
* @param [in] pWorkingBuf Working buffer provided by user. Required buffer size can be queried by AmbaCal_AvmGetMainViewWorkSize
* @param [out] pOutput Returned output settings. Please refer to AMBA_CAL_AVM_MV_DATA_s for more details.��
* @return ErrorCode
*/
UINT32 AmbaCal_AvmGenMainViewTblV2(const AMBA_CAL_AVM_CALIB_DATA_s *pCalibData, const AMBA_CAL_AVM_MV_CFG_V2_s *pCfg,
    const void *pWorkingBuf, AMBA_CAL_AVM_MV_DATA_s *pOutput);

/**
* This API is used to generate MainView warp tables based on user specified view settings and pattern positions.��
* @param [in] pCalibData Input calibration data for generating tables.
* Please refer to AMBA_CAL_AVM_CALIB_DATA_s for more details.��
* @param [in] pCfg MainView related configuration. Please refer to AMBA_CAL_AVM_MV_CFG_V1_s more details.��
* @param [in] pWorkingBuf Working buffer provided by user. Required buffer size can be queried by AmbaCal_AvmGetMainViewWorkSize
* @param [out] pOutput Returned output settings. Please refer to AMBA_CAL_AVM_MV_DATA_s for more details.��
* @return ErrorCode
*/
UINT32 AmbaCal_AvmGenMainViewTblv1(const AMBA_CAL_AVM_CALIB_DATA_s *pCalibData, const AMBA_CAL_AVM_MV_CFG_V1_s *pCfg,
    const void *pWorkingBuf, AMBA_CAL_AVM_MV_DATA_s *pOutput);

/**
* This API is used to get corresponding position on Vin(After warpping) given by world position on ground.
* This API is only feasible under MainView mode.
* @param [in] pCalibData Input calibration data for generating tables. Please refer to AMBA_CAL_AVM_CALIB_DATA_s for more details.
* @param [in] pCfg Configuration. Please refer to AMBA_CAL_AVM_PARK_ASSIST_CFG_s for more details.
* @param [in] pWorldPos Given world position on ground. (In world coordinate) (Unit in mm). Please refer to AMBA_CAL_POINT_DB_3D_s for more details.
* @param [out] pOutputDewarpPos Output position on VIN. (In image coordinate) (Unit in pixel). Please refer to AMBA_CAL_POINT_DB_2D_s for more details
* @return ErrorCode
*/
UINT32 AmbaCal_AvmGetParkAssistPos(const AMBA_CAL_AVM_CALIB_DATA_s *pCalibData,
    const AMBA_CAL_AVM_PARK_ASSIST_CFG_s *pCfg, const AMBA_CAL_POINT_DB_3D_s *pWorldPos, AMBA_CAL_POINT_DB_2D_s *pOutputDewarpPos);

/**
* This API is used to get corresponding position on Vin(After warpping) given by world position on ground.
* This API is only feasible under MainView mode.
* @param [in] pCalibData Input calibration data for generating tables. Please refer to AMBA_CAL_AVM_CALIB_DATA_s for more details.
* @param [in] pCfg Configuration. Please refer to AMBA_CAL_AVM_PARK_ASSIST_CFG_V2_s more details.
* @param [in] pWorldPos Given world position on ground. (In world coordinate) (Unit in mm). Please refer to AMBA_CAL_POINT_DB_3D_s for more details.
* @param [out] pOutputDewarpPos Output position on VIN. (In image coordinate) (Unit in pixel). Please refer to AMBA_CAL_POINT_DB_2D_s for more details
* @return ErrorCode
*/
UINT32 AmbaCal_AvmGetParkAssistPosV2(const AMBA_CAL_AVM_CALIB_DATA_s *pCalibData,
    const AMBA_CAL_AVM_PARK_AST_CFG_V2_s *pCfg, const AMBA_CAL_POINT_DB_3D_s *pWorldPos, AMBA_CAL_POINT_DB_2D_s *pOutputDewarpPos);

/**
* This API is used to get corresponding position on Vin(After warpping) given by world position on ground.
* This API is only feasible under MainView mode.
* @param [in] pCalibData Input calibration data for generating tables. Please refer to AMBA_CAL_AVM_CALIB_DATA_s for more details.
* @param [in] pCfg Configuration. Please refer to AMBA_CAL_AVM_PARK_ASSIST_CFG_V1_s more details.
* @param [in] pWorldPos Given world position on ground. (In world coordinate) (Unit in mm). Please refer to AMBA_CAL_POINT_DB_3D_s for more details.
* @param [out] pOutputDewarpPos Output position on VIN. (In image coordinate) (Unit in pixel). Please refer to AMBA_CAL_POINT_DB_2D_s for more details
* @return ErrorCode
*/
UINT32 AmbaCal_AvmGetParkAssistPosv1(const AMBA_CAL_AVM_CALIB_DATA_s *pCalibData,
    const AMBA_CAL_AVM_PARK_AST_CFG_V1_s *pCfg, const AMBA_CAL_POINT_DB_3D_s *pWorldPos, AMBA_CAL_POINT_DB_2D_s *pOutputDewarpPos);

/**
* Calculate floor position of the world.
* @param [in]  pCalibData calibration information.
* @param [in]  pCfg view config. Currently, ViewType must be AMBA_CAL_AVM_MV_LDC and CompensateRatio must be 0 or 100.
* @param [in]  ImgPos image position.
* @param [in]  ImgPosNum number of the image position.
* @param [out] WorldPos world position.
* @return ErrorCode
*/
UINT32 AmbaCal_AvmGetMVParkSpacePosV2(const AMBA_CAL_AVM_CALIB_DATA_s *pCalibData,
            const AMBA_CAL_AVM_PARK_AST_CFG_V2_s *pCfg,
            const AMBA_CAL_POINT_DB_2D_s ImgPos[],
            UINT32 ImgPosNum,
            AMBA_CAL_POINT_DB_3D_s WorldPos[]);
UINT32 AmbaCal_AvmGetMVParkSpacePos(const AMBA_CAL_AVM_CALIB_DATA_s *pCalibData,
        const AMBA_CAL_AVM_PARK_AST_CFG_V1_s *pCfg,
        const AMBA_CAL_POINT_DB_2D_s ImgPos[],
        UINT32 ImgPosNum,
        AMBA_CAL_POINT_DB_3D_s WorldPos[]);

UINT32 AmbaCal_TranslatePlanePos(const DOUBLE CalibMat[9U],
    const AMBA_CAL_POINT_DB_2D_s *pInPoint, AMBA_CAL_POINT_DB_2D_s *pOutPoint);
#endif
