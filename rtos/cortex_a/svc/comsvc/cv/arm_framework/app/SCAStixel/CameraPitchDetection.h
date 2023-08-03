#ifndef CAM_PH_DET_H
#define CAM_PH_DET_H

#include "AmbaTypes.h"
#include "AmbaCalib_UtilityIF.h"

typedef struct {
    UINT16 *pBuff;
    UINT32 StartY; // start y in image coordinate
    UINT32 Width;
    UINT32 Height;
    UINT32 Precision;  //1: 1 pixel, 2: 0.5 pixel, 4: 0.25 pixel ..
} CAM_PITCH_DET_V_DISP_s;

typedef struct {
    DOUBLE CameraHeightMeter;
    DOUBLE CameraPitchRad;
} CAM_PITCH_DET_CAM_EXT_s;

typedef struct {
    DOUBLE BaseLineMeter;
    DOUBLE FocalLengthPixel;
    DOUBLE V0;
} CAM_PITCH_DET_CAM_INT_s;

typedef struct {
    DOUBLE R;
    DOUBLE Deg;
} CAM_PITCH_DET_LINE_TOLER_PARAM_s;

typedef struct {
    // v = mw + c
    DOUBLE m;
    DOUBLE c;
    DOUBLE inv_m;
    // polorR = cos(polorT)w + sin(polorT)v
    DOUBLE polorR;
    DOUBLE polorT;
} CAM_PITCH_DET_LINE_s;

typedef struct {
    CAM_PITCH_DET_CAM_INT_s IntCam;
} CAM_PITCH_DET_AUTO_PARAM_s;

#define CAM_PITCH_DET_MANUAL_PITCH_NUM     (128U)
#define CAM_PITCH_DET_MANUAL_HEIGHT_NUM    (64U)
#define CAM_PITCH_DET_MANUAL_CAND_NUM      (CAM_PITCH_DET_MANUAL_PITCH_NUM*CAM_PITCH_DET_MANUAL_HEIGHT_NUM)

typedef struct {
    CAM_PITCH_DET_LINE_s GCLine;
    DOUBLE PitchRad;
    DOUBLE HeightMeter;
} CAM_PITCH_DET_MANUAL_CAND_s;

typedef struct {
    UINT32 CandidateNum;
    CAM_PITCH_DET_MANUAL_CAND_s Candidates[CAM_PITCH_DET_MANUAL_CAND_NUM];
} CAM_PITCH_DET_MANUAL_HDLR_s;

typedef struct {
    UINT32 XPixelCountThr;
    DOUBLE YRatioThr;
} CAM_PITCH_DET_MANUAL_THR_s;

typedef struct {
    UINT32 ValidPriorInfo;
    CAM_PITCH_DET_MANUAL_CAND_s Road;
} CAM_PITCH_DET_MANUAL_PRIOR_ESTI_s;

UINT32 CamPhDet_CalExpectedGCLine(DOUBLE CamHeight, DOUBLE CamPitch,
                                  DOUBLE V0, DOUBLE BaseLine, DOUBLE FocalLength,
                                  DOUBLE *pM, DOUBLE *pInvM, DOUBLE *pC);

DOUBLE CamPhDet_GetGroundDispByV(UINT32 V, DOUBLE GCLineM, DOUBLE GCLineC);

UINT32 CamPhDet_GetAutoDetWorkSize(UINT32 *pWorkSize);
UINT32 CamPhDet_AutoDetection(const UINT8 *pWorkBuff,
                              const CAM_PITCH_DET_V_DISP_s *pVdisp,
                              const CAM_PITCH_DET_AUTO_PARAM_s *pCfg,
                              DOUBLE *pEstiCamHeight, DOUBLE *pEstiCamPitch, CAM_PITCH_DET_LINE_s *pEstiGcLine);


UINT32 CamPhDet_InitManualDetection(DOUBLE BaseCamHeight, DOUBLE BaseCamPitch,
                                    DOUBLE V0, DOUBLE BaseLine, DOUBLE FocalLength, DOUBLE VDispPrecision,
                                    DOUBLE PitchDegStride, DOUBLE HeightMeterStride,
                                    UINT32 PitchSetNum, UINT32 HeightSetNum,
                                    CAM_PITCH_DET_MANUAL_PRIOR_ESTI_s *pPriorEsti,
                                    CAM_PITCH_DET_MANUAL_HDLR_s *pHdlr);

UINT32 CamPhDet_ManualDetection(const CAM_PITCH_DET_V_DISP_s *pVdisp,
                                const CAM_PITCH_DET_MANUAL_HDLR_s *pHdlr,
                                const CAM_PITCH_DET_MANUAL_THR_s *pThr,
                                CAM_PITCH_DET_MANUAL_PRIOR_ESTI_s *pPriorEsti,
                                DOUBLE *pEstiCamHeight, DOUBLE *pEstiCamPitch,
                                CAM_PITCH_DET_LINE_s *pEstiGcLine,
                                UINT32 *pQualifiedRoadEsti);

//================================================================
//================================================================

typedef struct {
    UINT16 *pBuff;
    UINT16 *pMaxPos;
    UINT32 StartY; // start y in image coordinate
    UINT32 Width;
    UINT32 Height;
    UINT32 Precision;  //1: 1 pixel, 2: 0.5 pixel, 4: 0.25 pixel ..
} CAM_PITCH_DET_V2_V_DISP_s;

typedef struct {
    DOUBLE CameraHeightMeter;
    DOUBLE CameraPitchRad;
} CAM_PITCH_DET_V2_CAM_EXT_s;

typedef struct {
    DOUBLE BaseLineMeter;
    DOUBLE FocalLengthPixel;
    DOUBLE U0;
    DOUBLE V0;
} CAM_PITCH_DET_V2_CAM_INT_s;

typedef struct {
    DOUBLE R;
    DOUBLE Deg;
} CAM_PITCH_DET_V2_LINE_TOLER_PARAM_s;

typedef struct {
    // v = mw + c
    DOUBLE m;
    DOUBLE c;
    DOUBLE inv_m;
    // polorR = cos(polorT)w + sin(polorT)v
    DOUBLE polorR;
    DOUBLE polorT;
} CAM_PITCH_DET_V2_LINE_s;

typedef struct {
    CAM_PITCH_DET_V2_CAM_INT_s IntCam;
} CAM_PITCH_DET_V2_AUTO_PARAM_s;

#define CAM_PITCH_DET_V2_MANUAL_MAX_VDISP_HEIGHT     (1920U)

#define CAM_PITCH_DET_V2_MANUAL_PITCH_NUM     (129U)
#define CAM_PITCH_DET_V2_MANUAL_HEIGHT_NUM    (41U)
#define CAM_PITCH_DET_V2_MANUAL_CAND_NUM      (CAM_PITCH_DET_V2_MANUAL_PITCH_NUM*CAM_PITCH_DET_V2_MANUAL_HEIGHT_NUM)

typedef struct {
    CAM_PITCH_DET_V2_LINE_s GCLine;
    DOUBLE PitchRad;
    DOUBLE HeightMeter;
} CAM_PITCH_DET_V2_ESTI_ROAD_s;

typedef struct {
    UINT32 HeightNum;
    UINT32 PitchNum;
    UINT32 UseMultiLineFit;
    CAM_PITCH_DET_CAM_INT_s IntPararm;
    CAM_PITCH_DET_V2_ESTI_ROAD_s Candidates[CAM_PITCH_DET_V2_MANUAL_CAND_NUM];
    UINT8 WorkBuff[CAM_PITCH_DET_V2_MANUAL_MAX_VDISP_HEIGHT*sizeof(INT32)];
} CAM_PITCH_DET_V2_MANUAL_HDLR_s;

typedef struct {
    UINT32 ValidPriorInfo;
    UINT32 RoadSplitPos;
    CAM_PITCH_DET_V2_ESTI_ROAD_s Road[2U];
} CAM_PITCH_DET_V2_MANUAL_PRIOR_ESTI_s;

DOUBLE CamPhDetV2_GetGroundDispByV(UINT32 V, DOUBLE GCLineM, DOUBLE GCLineC);

UINT32 CamPhDetV2_GetAutoDetWorkSize(UINT32 *pWorkSize);
UINT32 CamPhDetV2_AutoDetection(const UINT8 *pWorkBuff,
                                const CAM_PITCH_DET_V2_V_DISP_s *pVdisp,
                                const CAM_PITCH_DET_V2_AUTO_PARAM_s *pCfg,
                                CAM_PITCH_DET_V2_ESTI_ROAD_s *pEstiRoad);

UINT32 CamPhDetV2_InitManualDetection(DOUBLE BaseCamHeight, DOUBLE BaseCamPitch,
                                      DOUBLE V0, DOUBLE BaseLine, DOUBLE FocalLength, DOUBLE VDispPrecision,
                                      DOUBLE PitchDegStride, DOUBLE HeightMeterStride,
                                      UINT32 PitchSetNum, UINT32 HeightSetNum, DOUBLE CamHeightLowerBnd, UINT32 UseMultiLineFit,
                                      CAM_PITCH_DET_V2_MANUAL_PRIOR_ESTI_s *pPriorEsti,
                                      CAM_PITCH_DET_V2_MANUAL_HDLR_s *pHdlr);
UINT32 CamPhDetV2_RenewMDHdlr(DOUBLE CurV0, DOUBLE CurBaseLine, DOUBLE CurFocalLength,
                              CAM_PITCH_DET_V2_MANUAL_HDLR_s *pHdlr);
UINT32 CamPhDetV2_ManualDetection(const CAM_PITCH_DET_V2_V_DISP_s *pVdisp,
                                  const CAM_PITCH_DET_V2_MANUAL_HDLR_s *pHdlr,
                                  const CAM_PITCH_DET_MANUAL_THR_s *pThr,
                                  CAM_PITCH_DET_V2_MANUAL_PRIOR_ESTI_s *pPriorEsti,
                                  UINT32 *pEstiRoadSplitPos,
                                  CAM_PITCH_DET_V2_ESTI_ROAD_s EstiRoad[2U],
                                  UINT32 *pQualifiedRoadEsti);

typedef struct {
    UINT32 StartX;
    UINT32 StartY;
    UINT32 Width;
    UINT32 Height;
} CAM_PITCH_DET_V2_ROI_s;

typedef struct {
    UINT8 *pData;
    UINT32 UseMask;
    CAM_PITCH_DET_V2_ROI_s Roi;
} CAM_PITCH_DET_V2_FIT_3D_MASK_s;

UINT32 CamPhDetV2_UvdToXyz(const CAM_PITCH_DET_V2_CAM_INT_s *pCamIntParam,
                           AMBA_CAL_POINT_DB_3D_s Uvd, AMBA_CAL_POINT_DB_3D_s *pXyz);
UINT32 CamPhDetV2_XyzToUvd(const CAM_PITCH_DET_V2_CAM_INT_s *pCamIntParam,
                           AMBA_CAL_POINT_DB_3D_s Xyz, AMBA_CAL_POINT_DB_3D_s *pUvd);

UINT32 CamPhDetV2_GetFit3DGroundWorkSize(UINT32 *pWorkSize);

UINT32 CamPhDetV2_Fit3DGround(const void *pWorkBuff,
                              const CAM_PITCH_DET_V2_FIT_3D_MASK_s *pMaskCfg,
                              const UINT16 *pHWDisparityMap,
                              UINT32 HWDispWidth, UINT32 HWDispPitch, UINT32 HWDispHeight, UINT32 InvalidHWDispVal,
                              const CAM_PITCH_DET_V2_ROI_s *pProcArea,
                              const CAM_PITCH_DET_V2_CAM_INT_s *pCamIntParam,
                              const AMBA_CAL_UTIL_PLANE_COEF_s *pPriorUvdPlaneCoef,
                              AMBA_CAL_UTIL_PLANE_COEF_s *pUvdPlaneCoef, DOUBLE *pEstiHeight, DOUBLE *pEstiPitch);


UINT32 CamPhDetV2_UvdPlaneToXyzPlane(const AMBA_CAL_UTIL_PLANE_COEF_s *pUvdCoef,
                                     const CAM_PITCH_DET_V2_CAM_INT_s *pCamIntParam, AMBA_CAL_UTIL_PLANE_COEF_s *pXyzCoef);

UINT32 CamPhDetV2_XyzPlaneToUvdPlane(const AMBA_CAL_UTIL_PLANE_COEF_s *pXyzCoef,
                                     const CAM_PITCH_DET_V2_CAM_INT_s *pCamIntParam, AMBA_CAL_UTIL_PLANE_COEF_s *pUvdCoef);

UINT32 CamPhDetV2_VDispParamToXyzPlane(const CAM_PITCH_DET_V2_ESTI_ROAD_s *pVDispParam,
                                       AMBA_CAL_UTIL_PLANE_COEF_s *pCoef);
UINT32 CamPhDetV2_VDispParamToUvdPlane(const CAM_PITCH_DET_V2_ESTI_ROAD_s *pVDispParam,
                                       AMBA_CAL_UTIL_PLANE_COEF_s *pCoef);


DOUBLE CamPhDetV2_CalcDistToPlaneByXyz(AMBA_CAL_POINT_DB_3D_s Point,
                                       const AMBA_CAL_UTIL_PLANE_COEF_s *pXyzPlaneCoef);
DOUBLE CamPhDetV2_CalcDistToPlaneByUvd(DOUBLE U, DOUBLE V, DOUBLE D,
                                       const CAM_PITCH_DET_V2_CAM_INT_s *pCamIntParam,
                                       const AMBA_CAL_UTIL_PLANE_COEF_s *pXyzPlaneCoef);

DOUBLE CamPhDetV2_CalcAngleToPlaneByXyz(AMBA_CAL_POINT_DB_3D_s Vector,
                                        const AMBA_CAL_UTIL_PLANE_COEF_s *pPlaneCoef);

UINT32 CamPhDetV2_CalVDisparity(const UINT16 *pInDisparity, UINT32 InDisparityWidth, UINT32 InDisparityPitch,
                                UINT32 InRowStart, UINT32 InRowEnd, UINT32 InHorizontalStride, const CAM_PITCH_DET_V2_V_DISP_s *pOutVdisp);

#endif
