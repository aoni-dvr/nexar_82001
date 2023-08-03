#ifndef CTFW_STIXEL_TASK_H
#define CTFW_STIXEL_TASK_H
#include "Ctfw_TaskUtil.h"
#include "CameraPitchDetection.h"
#include "cvapi_svccvalgo_stixel.h"

extern CtfwTaskObj_t gCtfwStixelTaskObj;

#define STIXEL_FX_IDX_VDISP              (0U)
#define STIXEL_FX_IDX_GEN_ESTI_ROAD      (1U)
#define STIXEL_FX_IDX_PREPROCESS         (2U)
#define STIXEL_FX_IDX_FREE_SPACE         (3U)
#define STIXEL_FX_IDX_HEIGHT_SEG         (4U)
#define STIXEL_FX_IDX_NUM                (5U)

typedef struct {
    UINT32 InDispScaleId;
    AMBA_CV_STIXEL_CFG_V2_s StixelCfg;
} CTFW_STIXEL_TASK_CONFIG_t;

typedef struct {
    AMBA_CV_STIXEL_DET_OUT_s StixelInfo;
} CTFW_STIXEL_TASK_OUT_t;

#endif
