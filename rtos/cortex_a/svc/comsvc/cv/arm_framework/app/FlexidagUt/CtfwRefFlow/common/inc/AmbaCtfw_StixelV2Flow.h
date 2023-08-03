#ifndef AMBACTFW_STIXELV2_FLOW_H
#define AMBACTFW_STIXELV2_FLOW_H
#include "Ctfw_TaskUtil.h"
#include "Ctfw_StixelCamPoseTask.h"
#include "Ctfw_StixelGenStixelTask.h"
#include "cvapi_svccvalgo_stixel_v2.h"

void AmbaCtfw_StixelV2ProfilerReport(void);
UINT32 AmbaCtfw_StixelV2ProduceImgFrame(const flexidag_memblk_t *pPicInfo,
                                        UINT32 FrameIndex, CtfwFrame_t *pFrame);
UINT32 AmbaCtfw_StixelV2ProduceSpuFrame(const flexidag_memblk_t *pSpuOut,
                                        UINT32 FrameIndex, CtfwFrame_t *pFrame);
UINT32 AmbaCtfw_StixelV2Init(const CTFW_STIXEL_CAM_POSE_TASK_CONFIG_t *pStixelv2CamPoseTaskConfig,
                             const CTFW_STIXEL_GEN_STIXEL_TASK_CONFIG_t *pStixelv2GenStixelTaskConfig,
                             CtfwGetProcResult_t GetStixelResultCb,
                             flexidag_memblk_t StixelFlexiBinBlk[], UINT32 PoolFrameNumTobeAlloc,
                             CtfwFrame_t SpuFramePool[], CtfwFrame_t ImgFramePool[]);
UINT32 AmbaCtfw_StixelV2FeedOneSpuFrame(const CtfwFrame_t *pFrame);
UINT32 AmbaCtfw_StixelV2FeedOneImgFrame(const CtfwFrame_t *pFrame);
UINT32 AmbaCtfw_StixelV2Stop(void);
UINT32 AmbaCtfw_StixelV2DeInit(void);
UINT32 AmbaCtfw_StixelV2Start(void);
UINT32 AmbaCtfw_StixelV2Restart(void);

#endif
