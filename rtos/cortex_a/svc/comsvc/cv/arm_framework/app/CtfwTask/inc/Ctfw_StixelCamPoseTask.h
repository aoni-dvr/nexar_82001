#ifndef CTFW_STIXEL_CAMERAPOSE_TASK_H
#define CTFW_STIXEL_CAMERAPOSE_TASK_H
#include "Ctfw_TaskUtil.h"
#include "CameraPitchDetection.h"
#include "cvapi_svccvalgo_stixel_v2.h"

extern CtfwTaskObj_t gCtfwStixelCamPoseTaskObj;

typedef struct {
    UINT32 InDispScaleId;
    AMBA_CV_STIXEL_V2_CFG_s StixelConfig;
} CTFW_STIXEL_CAM_POSE_TASK_CONFIG_t;

typedef struct {
    CAM_PITCH_DET_V2_ESTI_ROAD_s EstiRoad[STIXEL_V2_ROAD_SECTION_NUM];
    AMBA_CAL_UTIL_PLANE_COEF_s UvdPlaneFitCoef;
    UINT32 EstiRoadSplitPos;
    UINT32 EstiRoadLinePlaneFusePos;
} CTFW_STIXEL_CAM_POSE_TASK_OUT_t;

#endif
