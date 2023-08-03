#ifndef CTFW_MVAC_VO_TASK_H
#define CTFW_MVAC_VO_TASK_H
#include "Ctfw_TaskUtil.h"
#include "cvapi_flexidag_ambavo_cv2.h"

extern CtfwTaskObj_t gCtfwMvacVoTaskObj;

typedef struct {
    flexidag_memblk_t *pInitMetaRightBuf;
    AMBA_CV_VO_CFG_s VoCfg;
    UINT32 FexSrcTaskType;
} CTFW_MVAC_VO_TASK_CONFIG_t;


#endif
