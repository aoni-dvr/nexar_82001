#ifndef AMBACTFW_OFFEX_FLOW_CV2A_H
#define AMBACTFW_OFFEX_FLOW_CV2A_H
#include "Ctfw_TaskUtil.h"
#include "cvapi_flexidag_ambaspufex_def_cv2fs.h"

void AmbaCtfw_OffexCv2aProfilerReport(void);

UINT32 AmbaCtfw_OffexCv2aFeedOneFrame(const CtfwFrame_t *pFrame);

UINT32 AmbaCtfw_OffexCv2aProduceFrame(const AMBA_CV_IMG_BUF_s *pImage, UINT32 FrameIndex, CtfwFrame_t *pFrame);

UINT32 AmbaCtfw_OffexCv2aInit(const AMBA_CV_SPUFEX_CFG_s *pSpufexCfg,
                              CtfwGetProcResult_t GetResultCb, flexidag_memblk_t FlexiBinBlk, UINT32 PoolFrameNumTobeAlloc, CtfwFrame_t FramePool[]);

UINT32 AmbaCtfw_OffexCv2aStop(void);
UINT32 AmbaCtfw_OffexCv2aDeInit(void);
UINT32 AmbaCtfw_OffexCv2aStart(void);
UINT32 AmbaCtfw_OffexCv2aRestart(void);

#endif