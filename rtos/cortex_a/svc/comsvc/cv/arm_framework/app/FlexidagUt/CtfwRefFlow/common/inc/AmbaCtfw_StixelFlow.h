#ifndef AMBACTFW_STIXEL_FLOW_H
#define AMBACTFW_STIXEL_FLOW_H
#include "Ctfw_TaskUtil.h"
#include "Ctfw_StixelTask.h"
#include "cvapi_svccvalgo_stixel.h"

void AmbaCtfw_StixelProfilerReport(void);
UINT32 AmbaCtfw_StixelProduceFrame(const flexidag_memblk_t *pSpuOut,
                                   UINT32 FrameIndex, CtfwFrame_t *pFrame);
UINT32 AmbaCtfw_StixelInit(const CTFW_STIXEL_TASK_CONFIG_t *pStixelCfg,
                           CtfwGetProcResult_t GetResultCb, flexidag_memblk_t StixelFlexiBinBlk[], UINT32 PoolFrameNumTobeAlloc, CtfwFrame_t FramePool[]);
UINT32 AmbaCtfw_StixelFeedOneFrame(const CtfwFrame_t *pFrame);
UINT32 AmbaCtfw_StixelStop(void);
UINT32 AmbaCtfw_StixelDeInit(void);
UINT32 AmbaCtfw_StixelStart(void);
UINT32 AmbaCtfw_StixelRestart(void);

#endif
