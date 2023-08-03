#ifndef CTFW_SPUFEX_TASK_H
#define CTFW_SPUFEX_TASK_H
#include "Ctfw_TaskUtil.h"
#include "cvapi_flexidag_ambaspufex_def_cv2.h"

extern CtfwTaskObj_t gCtfwSpuFexTaskObj;

typedef struct {
    AMBA_CV_SPU_SCALE_CFG_s     SpuScaleCfg[MAX_HALF_OCTAVES];
    AMBA_CV_FEX_SCALE_CFG_s     FexScaleCfg[MAX_HALF_OCTAVES];
    UINT32 SpuDisplayMode;
} CTFW_SPU_FEX_TASK_CONFIG_t;

typedef struct {
    flexidag_memblk_t     SpuOut;
    flexidag_memblk_t     FexOut;
} CTFW_SPU_FEX_TASK_OUT_t;

#endif
