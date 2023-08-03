#ifndef AMBACTFW_MVAC_FLOW_H
#define AMBACTFW_MVAC_FLOW_H
#include "Ctfw_TaskUtil.h"
#include "Ctfw_SourceTask.h"
#include "Ctfw_SinkTask.h"
#include "Ctfw_FexTask.h"
#include "Ctfw_SpuFexTask.h"
#include "Ctfw_FmaTask.h"
#include "Ctfw_MvacVoTask.h"
#include "Ctfw_MvacAcTask.h"

void AmbaCtfw_MvacProfilerReport(void);
UINT32 AmbaCtfw_MvacProduceFrame(const flexidag_memblk_t *pPicInfo, const CtfwCalibInfo_t *pCalibInfo,
                                 UINT32 FrameIndex, CtfwFrame_t *pFrame);
UINT32 AmbaCtfw_MvacInit(const CTFW_FMA_TASK_CONFIG_t *pFmaTaskConfig,
                         const CTFW_MVAC_VO_TASK_CONFIG_t *pVoTaskConfig,
                         const CTFW_MVAC_AC_TASK_CONFIG_t *pAcTaskConfig,
                         CtfwGetProcResult_t GetMvacResultCb,
                         UINT32 PoolFrameNumTobeAlloc,
                         CtfwFrame_t FramePool[]);
UINT32 AmbaCtfw_MvacFeedOneFrame(const CtfwFrame_t *pFrame);
UINT32 AmbaCtfw_MvacStop(void);
UINT32 AmbaCtfw_MvacStart(const CTFW_MVAC_AC_TASK_CONFIG_t *pAcTaskConfig);
UINT32 AmbaCtfw_MvacRestart(const CTFW_MVAC_AC_TASK_CONFIG_t *pAcTaskConfig);

#endif
