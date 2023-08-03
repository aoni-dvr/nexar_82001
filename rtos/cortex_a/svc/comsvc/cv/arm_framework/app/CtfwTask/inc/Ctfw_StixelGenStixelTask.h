#ifndef CTFW_STIXEL_GEN_STIXEL_TASK_H
#define CTFW_STIXEL_GEN_STIXEL_TASK_H
#include "Ctfw_TaskUtil.h"
#include "cvapi_svccvalgo_stixel_v2.h"
//#include "Ctfw_StixelCamPoseTask.h"

extern CtfwTaskObj_t gCtfwStixelGenStixelTaskObj;

#define GEN_STIXEL_FX_IDX_GEN_ESTI_ROAD      (0U)
#define GEN_STIXEL_FX_IDX_PREPROCESS         (1U)
#define GEN_STIXEL_FX_IDX_FREE_SPACE         (2U)
#define GEN_STIXEL_FX_IDX_FREE_SPACE_DP      (3U)
#define GEN_STIXEL_FX_IDX_HEIGHT_SEG         (4U)
#define GEN_STIXEL_FX_IDX_NUM                (5U)

typedef struct {
    UINT32 InImgScaleId;
    UINT32 InDispScaleId;
    AMBA_CV_STIXEL_V2_CFG_s StixelConfig;
} CTFW_STIXEL_GEN_STIXEL_TASK_CONFIG_t;

typedef struct {
    AMBA_CV_STIXEL_V2_DET_OUT_s StixelInfo;
} CTFW_STIXEL_GEN_STIXEL_TASK_OUT_t;

#endif
