#ifndef AMBACTFW_FUSION_STIXEL_FLOW_H
#define AMBACTFW_FUSION_STIXEL_FLOW_H
#include "Ctfw_TaskUtil.h"
#include "Ctfw_SourceTask.h"
#include "Ctfw_SinkTask.h"
#include "Ctfw_SpuFexTask.h"
#include "Ctfw_SpuFusionTask.h"
#include "Ctfw_StixelTask.h"

void AmbaCtfw_FusionStixelProfilerReport(void);
UINT32 AmbaCtfw_FusionStixelProduceFrame(const flexidag_memblk_t *pPicInfo, const CtfwCalibInfo_t *pCalibInfo,
        UINT32 FrameIndex, CtfwFrame_t *pFrame);
UINT32 AmbaCtfw_FusionStixelInit(const CTFW_SPU_FEX_TASK_CONFIG_t *pSpuFexTaskConfig,
                                 const CTFW_SPU_FUSION_TASK_CONFIG_t *pSpuFusionTaskConfig,
                                 const CTFW_STIXEL_TASK_CONFIG_t *pStixelTaskConfig,
                                 CtfwGetProcResult_t GetFusionResultCb, CtfwGetProcResult_t GetStixelResultCb,
                                 flexidag_memblk_t StixelFlexiBinBlk[],
                                 UINT32 PoolFrameNumTobeAlloc, CtfwFrame_t FramePool[]);
UINT32 AmbaCtfw_FusionStixelFeedOneFrame(const CtfwFrame_t *pFrame);
UINT32 AmbaCtfw_FusionStixelStop(void);
UINT32 AmbaCtfw_FusionStixelStart(void);
UINT32 AmbaCtfw_FusionStixelRestart(void);

#endif
