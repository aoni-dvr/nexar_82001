#ifndef AMBACTFW_MVAC_STIXEL_FLOW_H
#define AMBACTFW_MVAC_STIXEL_FLOW_H
#include "Ctfw_TaskUtil.h"
#include "Ctfw_SourceTask.h"
#include "Ctfw_SinkTask.h"
#include "Ctfw_SpuFexTask.h"
#include "Ctfw_SpuFusionTask.h"
#include "Ctfw_FmaTask.h"
#include "Ctfw_MvacVoTask.h"
#include "Ctfw_MvacAcTask.h"
#include "Ctfw_StixelTask.h"

void AmbaCtfw_MvacStixelProfilerReport(void);
UINT32 AmbaCtfw_MvacStixelProduceFrame(const flexidag_memblk_t *pPicInfo, const CtfwCalibInfo_t *pCalibInfo,
                                       UINT32 FrameIndex, CtfwFrame_t *pFrame);
UINT32 AmbaCtfw_MvacStixelInit(const CTFW_SPU_FEX_TASK_CONFIG_t *pSpuFexTaskConfig,
                               const CTFW_FMA_TASK_CONFIG_t *pFmaTaskConfig,
                               const CTFW_MVAC_VO_TASK_CONFIG_t *pVoTaskConfig,
                               const CTFW_MVAC_AC_TASK_CONFIG_t *pAcTaskConfig,
                               const CTFW_SPU_FUSION_TASK_CONFIG_t *pSpuFusionTaskConfig,
                               const CTFW_STIXEL_TASK_CONFIG_t *pStixelTaskConfig,
                               CtfwGetProcResult_t GetMvacResultCb, CtfwGetProcResult_t GetStixelResultCb,
                               flexidag_memblk_t StixelFlexiBinBlk[],
                               UINT32 PoolFrameNumTobeAlloc, CtfwFrame_t FramePool[]);
UINT32 AmbaCtfw_MvacStixelFeedOneFrame(const CtfwFrame_t *pFrame);
UINT32 AmbaCtfw_MvacStixelStop(void);
UINT32 AmbaCtfw_MvacStixelStart(const CTFW_SPU_FEX_TASK_CONFIG_t *pSpuFexTaskConfig,
                                const CTFW_FMA_TASK_CONFIG_t *pFmaTaskConfig,
                                const CTFW_MVAC_VO_TASK_CONFIG_t *pVoTaskConfig,
                                const CTFW_MVAC_AC_TASK_CONFIG_t *pAcTaskConfig,
                                const CTFW_SPU_FUSION_TASK_CONFIG_t *pSpuFusionTaskConfig,
                                const CTFW_STIXEL_TASK_CONFIG_t *pStixelTaskConfig,
                                CtfwGetProcResult_t GetMvacResultCb, CtfwGetProcResult_t GetStixelResultCb,
                                flexidag_memblk_t StixelFlexiBinBlk[]);
UINT32 AmbaCtfw_MvacStixelRestart(const CTFW_SPU_FEX_TASK_CONFIG_t *pSpuFexTaskConfig,
                                  const CTFW_FMA_TASK_CONFIG_t *pFmaTaskConfig,
                                  const CTFW_MVAC_VO_TASK_CONFIG_t *pVoTaskConfig,
                                  const CTFW_MVAC_AC_TASK_CONFIG_t *pAcTaskConfig,
                                  const CTFW_SPU_FUSION_TASK_CONFIG_t *pSpuFusionTaskConfig,
                                  const CTFW_STIXEL_TASK_CONFIG_t *pStixelTaskConfig,
                                  CtfwGetProcResult_t GetMvacResultCb, CtfwGetProcResult_t GetStixelResultCb,
                                  flexidag_memblk_t StixelFlexiBinBlk[]);

#endif
