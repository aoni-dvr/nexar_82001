#ifndef CTFW_MVAC_AC_TASK_H
#define CTFW_MVAC_AC_TASK_H
#include "Ctfw_TaskUtil.h"
#include "cvapi_flexidag_ambamvac_cv2.h"

extern CtfwTaskObj_t gCtfwMvacAcTaskObj;

typedef struct {
    flexidag_memblk_t *pInitMetaLeftBuf;
    AMBA_CV_MVAC_VIEW_INFO_s MvacViewInfo;
    UINT32 FexSrcTaskType;
} CTFW_MVAC_AC_TASK_CONFIG_t;

#endif
