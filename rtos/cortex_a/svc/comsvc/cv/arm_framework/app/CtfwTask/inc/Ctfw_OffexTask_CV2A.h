#ifndef CTFW_OFFEX_TASK_CV2A_H
#define CTFW_OFFEX_TASK_CV2A_H

#include "Ctfw_TaskUtil.h"
#include "cvapi_flexidag_ambaspufex_def_cv2fs.h"

extern CtfwTaskObj_t gCtfwOffexCv2akObj;

typedef struct {
    UINT32 scaleId;
} CTFW_OFFEXCV2_TASK_CONFIG_t;

typedef struct {
    AMBA_CV_SPU_BUF_s spuBuf;
    AMBA_CV_FEX_BUF_s fexBuf;
} CTFW_OFFEXCV2_TASK_OUT_t;

#endif