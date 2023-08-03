/**
 *  @file AmbaCalib_EmirrorDef.h
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
#ifndef AMBA_CALIB_EMIRROR_DEF_H
#define AMBA_CALIB_EMIRROR_DEF_H
#include "AmbaCalib_LDCDef.h"

#define EMIR_CALIB_POINT_NUM (4U)
#define EMIR_CALIB_MULTI_POINT_NUM (4000U)
#define MAX_EMIR_ASSISTANCE_POINT_NUM (32U)
#define EMIR_BLEND_MAX_SIZE (960U * 480U)// Note # (540U * 480U)
#define EMIR_STATUS_MAX_SIZE (MAX_WARP_TBL_LEN)
#define AMBA_CAL_EM_MASK_VISIBLE (255U)
#define AMBA_CAL_EM_MASK_INVIIABLE (0U)

typedef enum {
    AMBA_CAL_EM_MODEL_PINHOLE = 0,
    AMBA_CAL_EM_MODEL_SPHERICAL,
    AMBA_CAL_EM_MODEL_MAX,
} AMBA_CAL_EM_PROJECTION_MODEL_e;

typedef enum {
    AMBA_CAL_EM_CAM_BACK = 0,
    AMBA_CAL_EM_CAM_LEFT,
    AMBA_CAL_EM_CAM_RIGHT,
    AMBA_CAL_EM_CAM_MAX,
} AMBA_CAL_EM_CAM_ID_e;

typedef enum {
    AMBA_EM_CALIB_4_POINT = 0,
    AMBA_EM_CALIB_MULTI_POINT,
} AMBA_CAL_EM_CALIB_POINT_TYPE_e;

#define AMBA_CAL_ST_CAM_LEFT        (0U)
#define AMBA_CAL_ST_CAM_RIGHT       (1U)
#define AMBA_CAL_ST_CAM_NUM         (2U)

typedef struct {
    AMBA_CAL_WORLD_RAW_POINT_s CalibPoints[EMIR_CALIB_POINT_NUM];
    UINT32 AssistancePointNumber;
    AMBA_CAL_WORLD_RAW_POINT_s AssistancePoints[MAX_EMIR_ASSISTANCE_POINT_NUM];
} AMBA_CAL_EM_POINT_MAP_s;

typedef struct {
    UINT32 MultiCalibPointsNumber;
    AMBA_CAL_WORLD_RAW_POINT_s *pMultiCalibPoints;
} AMBA_CAL_EM_MULTI_POINT_MAP_s;

typedef struct {
    AMBA_CAL_EM_CALIB_POINT_TYPE_e Type;
    AMBA_CAL_EM_POINT_MAP_s *p4Point;
    AMBA_CAL_EM_MULTI_POINT_MAP_s *pMultiPoint;
} AMBA_CAL_EM_CALIB_POINT_INFO_s;

typedef struct {
    UINT32 HorGridNum;
    UINT32 VerGridNum;
    UINT32 GridPosX;
    UINT32 GridPosY;
    UINT32 Status;
} AMBA_CAL_EM_WARP_TBL_STATUS_s;

typedef struct {
    UINT32 AssistancePointNumber;
    AMBA_CAL_POINT_DB_2D_s AssistancePointsErr[MAX_EMIR_ASSISTANCE_POINT_NUM];
} AMBA_CAL_EM_PTN_ERR_INFO_s;

typedef struct {
    UINT32 OutOfRoiGridNum;
    UINT32 VflipGridNum;
} AMBA_CAL_EM_REPORT_s;

typedef AMBA_CAL_EM_WARP_TBL_STATUS_s AMBA_CAL_EM_REPEAT_INFO_s;
typedef AMBA_CAL_EM_WARP_TBL_STATUS_s AMBA_CAL_EM_RECORD_INFO_s;
typedef AMBA_CAL_EM_WARP_TBL_STATUS_s AMBA_CAL_EM_OUT_OF_ROI_INFO_s;

typedef enum {
    AMBA_EM_MSG_GRID_STATUS = 0,
    AMBA_EM_MSG_PTN_ERR,
    AMBA_EM_MSG_REPORT,
    AMBA_EM_MSG_MAX,
} AMBA_CAL_EM_MSG_TYPE_e;

typedef struct {
    AMBA_CAL_EM_WARP_TBL_STATUS_s *pGridStatus;
    AMBA_CAL_EM_PTN_ERR_INFO_s *pPatternError;
    AMBA_CAL_EM_REPORT_s *pReport;
} AMBA_CAL_EM_MSG_s;

typedef struct {
    UINT32 GridStatus;
    UINT32 PatternError;
    UINT32 Report;
} AMBA_CAL_EM_MSG_RECIVER_CFG_s;

typedef UINT32 (*AMBA_CAL_EM_CB_MSG_RECIVER_s)(AMBA_CAL_EM_MSG_TYPE_e Type, AMBA_CAL_EM_CAM_ID_e CamId, const AMBA_CAL_EM_MSG_s *pMsg);
typedef UINT32 (*AMBA_CAL_EM_SV_CB_MSG_RECIVER_s)(AMBA_CAL_EM_MSG_TYPE_e Type, const AMBA_CAL_EM_MSG_s *pMsg);

typedef struct {
    UINT32 PlugInMode;
    AMBA_CAL_EM_MSG_RECIVER_CFG_s MsgReciverCfg;
    AMBA_CAL_EM_SV_CB_MSG_RECIVER_s MsgReciver;
} AMBA_CAL_EM_SV_PLUGIN_s;

typedef struct {
    UINT32 PlugInMode;
    UINT32 (*pPointMapAdjuster)(AMBA_CAL_EM_POINT_MAP_s *pOrigPointMap, AMBA_CAL_EM_POINT_MAP_s *pAdjPointMap);
    AMBA_CAL_EM_MSG_RECIVER_CFG_s MsgReciverCfg;
    AMBA_CAL_EM_CB_MSG_RECIVER_s MsgReciver;
} AMBA_CAL_EM_3IN1_PLUGIN_s;

typedef struct {
    UINT32 StartX;
    INT32  Angle;
    UINT32 Width;
} AMBA_CAL_EM_BLEND_CFG_s;

typedef enum {
    AMBA_CAL_EM_LDC = 0,
    AMBA_CAL_EM_ASPHERIC,
    AMBA_CAL_EM_CURVED_SURFACE,
    AMBA_CAL_EM_CURVED_ASPHERIC,
    AMBA_CAL_EM_KEEP_LDC,
    AMBA_CAL_EM_KEEP_ASPHERIC,
    AMBA_CAL_EM_3IN1,
    AMBA_CAL_EM_MAX
} AMBA_CAL_EM_VIEW_TYPE_e;

typedef enum {
    AMBA_CAL_EM_OPTIMIZE_LV_0 = 0, // Note #  without optimize
    AMBA_CAL_EM_OPTIMIZE_LV_1, // Note #  optimize OC
    AMBA_CAL_EM_OPTIMIZE_LV_2, // Note #  optimize matrix
    AMBA_CAL_EM_OPTIMIZE_LV_3, // Note #  optimize OC + matrix
    AMBA_CAL_EM_OPTIMIZE_LV_MAX,
} AMBA_CAL_EM_OPTIMIZE_LEVEL_e;

typedef struct {
    AMBA_CAL_POINT_DB_2D_s Src;
    AMBA_CAL_POINT_DB_2D_s Dst;
} AMBA_CAL_EM_ASP_POINT_s;

typedef struct {
    AMBA_CAL_EM_ASP_POINT_s PointMap[6];
} AMBA_CAL_EM_ASP_VIEW_CFG_s;

typedef struct {
AMBA_CAL_LDC_VIEW_CFG_s LdcCfg;
AMBA_CAL_EM_ASP_POINT_s RenewRawPos[EMIR_CALIB_POINT_NUM];
} AMBA_CAL_EM_KEEP_LDC_VIEW_CFG_s;

typedef struct {
AMBA_CAL_EM_ASP_VIEW_CFG_s AsphericCfg;
AMBA_CAL_EM_ASP_POINT_s RenewRawPos[EMIR_CALIB_POINT_NUM];
} AMBA_CAL_EM_KEEP_ASP_VIEW_CFG_s;

typedef struct {
    UINT32 AngleX;
    UINT32 AngleY;
    UINT32 AngleZ;
} AMBA_CAL_EM_ROTATE_ANGLE_s;

typedef struct {
    // Note #  Plane Equation: CoefAx + CoefBy + CoefCz + CoefD = 0
    DOUBLE CoefA;
    DOUBLE CoefB;
    DOUBLE CoefC;
    DOUBLE CoefD;
} AMBA_CAL_EM_PLANE_EQUATION_s;

typedef struct {
    AMBA_CAL_VIRTUAL_CAM_s VirtualCam;
    AMBA_CAL_POINT_DB_2D_s WorldPosCurveLeft;
    AMBA_CAL_POINT_DB_2D_s WorldPosCurveRight;
    AMBA_CAL_POINT_DB_2D_s WorldPosLineLeft;
    AMBA_CAL_POINT_DB_2D_s WorldPosLineRight;
    DOUBLE Rotation;
} AMBA_CAL_EM_CURVED_SURFACE_CFG_s;

typedef struct {
    UINT32 Width;
    UINT32 Height;
    UINT8 *pMaskTbl;
} AMBA_CAL_EM_VISIBLE_AREA_MASK_s;

typedef struct {
    AMBA_CAL_POINT_DB_3D_s ViewStartPos;
    DOUBLE ViewWidth;
    UINT32 DistanceOfStitchPlane;
    DOUBLE BackViewScale;
    DOUBLE BackViewVerShift;
    DOUBLE LeftViewHorShift;
    DOUBLE RightViewHorShift;
    UINT32 ValidVoutLowerBound;
    AMBA_CAL_EM_BLEND_CFG_s BlendLeft;
    AMBA_CAL_EM_BLEND_CFG_s BlendRight;
    AMBA_CAL_ROI_s VoutAreaLeft;
    AMBA_CAL_ROI_s VoutAreaRight;
    AMBA_CAL_ROI_s VoutAreaBack;
    AMBA_CAL_EM_VISIBLE_AREA_MASK_s MaskLeft;
    AMBA_CAL_EM_VISIBLE_AREA_MASK_s MaskRight;
    AMBA_CAL_EM_VISIBLE_AREA_MASK_s MaskBack;
} AMBA_CAL_EM_3IN1_VIEW_CFG_s;

#define EMIR_VIN_FLIP  0x01U
#define EMIR_VOUT_FLIP 0x02U
typedef struct {
    AMBA_CAL_POINT_DB_3D_s ViewStartPos;
    DOUBLE ViewWidth;
    UINT32 DistanceOfStitchPlane;
    DOUBLE BackViewScale;
    DOUBLE BackViewVerShift;
    DOUBLE LeftViewHorShift;
    DOUBLE RightViewHorShift;
    UINT32 ValidVoutLowerBound;
    AMBA_CAL_EM_BLEND_CFG_s BlendLeft;
    AMBA_CAL_EM_BLEND_CFG_s BlendRight;
    AMBA_CAL_ROI_s VoutAreaLeft;
    AMBA_CAL_ROI_s VoutAreaRight;
    AMBA_CAL_ROI_s VoutAreaBack;
    AMBA_CAL_EM_VISIBLE_AREA_MASK_s MaskLeft;
    AMBA_CAL_EM_VISIBLE_AREA_MASK_s MaskRight;
    AMBA_CAL_EM_VISIBLE_AREA_MASK_s MaskBack;
    UINT32 MirrorFlipCfg;
} AMBA_CAL_EM_3IN1_VIEW_CFG_V1_s;

typedef struct {
    UINT32 DebugID;
    UINT32 RefineOutOfRoiEnable;
    UINT32 RefineDmaEnable;
    UINT32 RefineWaitlineEnable;
    UINT32 RefineVFlipEnable;
    UINT32 ExtrapolatePlaneBoundary;
    UINT32 RepeatUnusedArea;
    UINT32 WaitlineTh;
    UINT32 DmaTh;
    UINT32 PointMapHFlipEnable;
    UINT32 AutoRoiEnable;
} AMBA_CAL_EM_INTERNAL_CFG_s;

typedef struct {
    AMBA_CAL_CAM_s Cam;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_ROI_s ROI;
    AMBA_CAL_SIZE_s Tile;
    AMBA_CAL_EM_POINT_MAP_s PointMap;
    AMBA_CAL_EM_OPTIMIZE_LEVEL_e OptimizeLevel;
    AMBA_CAL_EM_INTERNAL_CFG_s InternalCfg;
    AMBA_CAL_EM_3IN1_PLUGIN_s PlugIn;
} AMAB_CAL_EM_CAM_CAL_INFO_s;

typedef struct {
    AMBA_CAL_SIZE_s Car;
    AMBA_CAL_EM_3IN1_VIEW_CFG_s View;
    AMAB_CAL_EM_CAM_CAL_INFO_s Cam[AMBA_CAL_EM_CAM_MAX];
} AMBA_CAL_EM_3IN1_CFG_s;

typedef struct {
    UINT32 Width;
    UINT32 Height;
    UINT8 Table[EMIR_BLEND_MAX_SIZE];
} AMBA_CAL_EM_BLEND_TBL_s;

typedef struct {
    UINT32 Width;
    UINT32 Height;
    UINT32 StatusTbl[EMIR_STATUS_MAX_SIZE];
} AMBA_CAL_EM_STATUS_TBL_s;

typedef struct {
    AMBA_CAL_POINT_DB_2D_s LeftSide[2U][4U];
    AMBA_CAL_POINT_DB_2D_s RightSide[2U][4U];
} AMBA_CAL_EM_OVERLAP_AREA_s;

typedef struct {
    UINT32 Version;
    AMBA_CAL_WARP_CALIB_DATA_s WarpTbl;
    DOUBLE CalibMatrix[9];
} AMBA_CAL_EM_SV_DATA_s;

typedef struct {
    AMBA_CAL_EM_VIEW_TYPE_e Type;
    struct {
        AMBA_CAL_LDC_VIEW_CFG_s *pLDC;
        AMBA_CAL_EM_CURVED_SURFACE_CFG_s *pCurvedSurface;
        AMBA_CAL_EM_ASP_VIEW_CFG_s *pAspheric;
        AMBA_CAL_EM_KEEP_LDC_VIEW_CFG_s *pKeepLDC;
        AMBA_CAL_EM_KEEP_ASP_VIEW_CFG_s *pKeepAspheric;
    } Cfg;
    AMBA_CAL_ROI_s VoutArea;
    AMBA_CAL_EM_SV_PLUGIN_s PlugIn;
} AMBA_CAL_EM_SV_VIEW_CFG_V1_s;

typedef struct {
    AMBA_CAL_EM_VIEW_TYPE_e Type;
    struct {
        AMBA_CAL_LDC_VIEW_CFG_s *pLDC;
        AMBA_CAL_EM_CURVED_SURFACE_CFG_s *pCurvedSurface;
        AMBA_CAL_EM_ASP_VIEW_CFG_s *pAspheric;
    } Cfg;
    AMBA_CAL_ROI_s VoutArea;
    AMBA_CAL_EM_SV_PLUGIN_s PlugIn;
} AMBA_CAL_EM_SV_VIEW_CFG_s; // Note # single view view config

typedef struct {
    AMBA_CAL_CAM_s Cam;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_EM_SV_VIEW_CFG_V1_s View;
    // Note # AMBA_CAL_EM_POINT_MAP_s PointMap;
    AMBA_CAL_EM_CALIB_POINT_INFO_s Calibinfo;
    AMBA_CAL_SIZE_s Tile;
    AMBA_CAL_ROI_s ROI;
    AMBA_CAL_EM_OPTIMIZE_LEVEL_e OptimizeLevel;
    AMBA_CAL_EM_INTERNAL_CFG_s InternalCfg;
} AMBA_CAL_EM_SV_CFG_V1_s;

typedef struct {
    AMBA_CAL_CAM_s Cam;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_EM_SV_VIEW_CFG_s View;
    // Note # AMBA_CAL_EM_POINT_MAP_s PointMap;
    AMBA_CAL_EM_CALIB_POINT_INFO_s Calibinfo;
    AMBA_CAL_SIZE_s Tile;
    AMBA_CAL_ROI_s ROI;
    AMBA_CAL_EM_OPTIMIZE_LEVEL_e OptimizeLevel;
    AMBA_CAL_EM_INTERNAL_CFG_s InternalCfg;
} AMBA_CAL_EM_SV_CFG_s;

typedef struct {
    UINT32 TblWidth;
    UINT32 TblHeight;
    AMBA_CAL_EM_BLEND_CFG_s BlendLeft;
    AMBA_CAL_EM_BLEND_CFG_s BlendRight;
} AMBA_CAL_EM_BLEND_TBL_CFG_s;

typedef struct {
    DOUBLE CalibMatrix[9];
    DOUBLE AssistPlaneDist;
} AMBA_CAL_EM_CAM_CALIB_DATA_s;

typedef struct {
    AMBA_CAL_EM_VIEW_TYPE_e Type;
    AMBA_CAL_POINT_DB_3D_s CamPos;
    AMBA_CAL_ROI_s Roi;
    AMBA_CAL_ROI_s VoutArea;
    struct {
        AMBA_CAL_EM_CURVED_SURFACE_CFG_s *pCurvedSurface; /* For Virtula Cam Based Case */
        AMBA_CAL_EM_CAM_CALIB_DATA_s *pCalibDataRaw2World;     /* For Other Case */


    } TransCfg;
} AMBA_CAL_EM_CALC_COORD_CFG_s;

typedef struct {
    AMBA_CAL_EM_VIEW_TYPE_e Type;
    AMBA_CAL_POINT_DB_3D_s CamPos;
    AMBA_CAL_ROI_s Roi;
    AMBA_CAL_ROI_s VoutArea;
    AMBA_CAL_SIZE_s CarSize;
    struct {
        AMBA_CAL_EM_CURVED_SURFACE_CFG_s *pCurvedSurface;   /* For Virtula Cam Based Case */
        AMBA_CAL_EM_CAM_CALIB_DATA_s *pCalibDataRaw2World;  /* For Other Case */
        AMBA_CAL_EM_3IN1_VIEW_CFG_V1_s *p3in1ViewCfg;
        AMBA_CAL_EM_KEEP_LDC_VIEW_CFG_s *pKeepLDC;
        AMBA_CAL_EM_KEEP_ASP_VIEW_CFG_s *pKeepAspheric;
    } TransCfg;
} AMBA_CAL_EM_CALC_COORD_CFG_V1_s;


typedef struct {
    AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataWorld2Raw;
    AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataRaw2World;
    AMBA_CAL_POINT_DB_2D_s OpticalCenter;
} AMBA_CAL_EM_CALIB_INFO_DATA_s;

typedef struct {
    AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataWorld2Raw;
    AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataRaw2World;
    AMBA_CAL_POINT_DB_2D_s OpticalCenter;          /* Unit: pixel */
    DOUBLE CameraRotationMatrix[9];
    DOUBLE CameraTranslationMatrix[3];
    AMBA_CAL_ROTATION_ANGLE_3D_s CameraEulerAngle; /* Unit: degree */
    AMBA_CAL_POINT_DB_3D_s CalculatedCameraPos;    /* Unit: mm     */
} AMBA_CAL_EM_CALIB_INFO_DATA_V1_s;

typedef struct {
    AMBA_CAL_CAM_s Cam;
    AMBA_CAL_EM_VIEW_TYPE_e Type;
    AMBA_CAL_ROI_s VoutArea;
    // Note # AMBA_CAL_EM_POINT_MAP_s PointMap;
    AMBA_CAL_EM_CALIB_POINT_INFO_s Calibinfo;
    AMBA_CAL_EM_OPTIMIZE_LEVEL_e OptimizeLevel;
    UINT32 PointMapHFlipEnable;
    AMBA_CAL_EM_SV_PLUGIN_s PlugIn;
} AMBA_CAL_EM_CALIB_INFO_CFG_s;

typedef struct {
    AMBA_CAL_CAM_V3_s Cam;
    AMBA_CAL_EM_VIEW_TYPE_e Type;
    AMBA_CAL_ROI_s VoutArea;
    // Note # AMBA_CAL_EM_POINT_MAP_s PointMap;
    AMBA_CAL_EM_CALIB_POINT_INFO_s Calibinfo;
    AMBA_CAL_EM_OPTIMIZE_LEVEL_e OptimizeLevel;
    UINT32 PointMapHFlipEnable;
    AMBA_CAL_EM_SV_PLUGIN_s PlugIn;
} AMBA_CAL_EM_CALIB_INFO_CFG_V1_s;

typedef struct {
    AMBA_CAL_CAM_s Cam;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_EM_SV_VIEW_CFG_V1_s View;
    AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataWorld2Raw;
    AMBA_CAL_SIZE_s Tile;
    AMBA_CAL_ROI_s ROI;
    AMBA_CAL_EM_INTERNAL_CFG_s InternalCfg;
} AMBA_CAL_EM_SV_GEN_TBL_CFG_V1_s;

typedef struct {
    AMBA_CAL_CAM_s Cam;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_EM_SV_VIEW_CFG_s View;
    AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataWorld2Raw;
    AMBA_CAL_SIZE_s Tile;
    AMBA_CAL_ROI_s ROI;
    AMBA_CAL_EM_INTERNAL_CFG_s InternalCfg;
} AMBA_CAL_EM_SV_GEN_TBL_CFG_s;

typedef struct {
    AMBA_CAL_CAM_s Cam;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_ROI_s ROI;
    AMBA_CAL_SIZE_s Tile;
    AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataWorld2Raw;
    AMBA_CAL_EM_INTERNAL_CFG_s InternalCfg;
    AMBA_CAL_EM_3IN1_PLUGIN_s PlugIn;
} AMAB_CAL_EM_3IN1_CAM_INFO_s;

typedef struct {
    AMBA_CAL_SIZE_s Car;
    AMBA_CAL_EM_3IN1_VIEW_CFG_s View;
    AMAB_CAL_EM_3IN1_CAM_INFO_s Cam[AMBA_CAL_EM_CAM_MAX];
} AMBA_CAL_EM_3IN1_GEN_TBL_CFG_s;

typedef struct {
    AMBA_CAL_SIZE_s Car;
    AMBA_CAL_EM_3IN1_VIEW_CFG_V1_s View;
    AMAB_CAL_EM_3IN1_CAM_INFO_s Cam[AMBA_CAL_EM_CAM_MAX];
} AMBA_CAL_EM3IN1_GEN_TBL_CFG_V1_s;
#endif
