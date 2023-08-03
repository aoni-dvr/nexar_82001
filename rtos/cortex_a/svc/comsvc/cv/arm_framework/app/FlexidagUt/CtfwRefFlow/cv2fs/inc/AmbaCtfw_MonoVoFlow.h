#ifndef AMBACTFW_MONOVO_FLOW_H
#define AMBACTFW_MONOVO_FLOW_H
#include "Ctfw_TaskUtil.h"
#include "cvapi_monoVo_interface.h"
#include "cvapi_flexidag_ambaspufex_def_cv2fs.h"

void AmbaCtfw_MonoVoProfilerReport(void);

UINT32 AmbaCtfw_MonoVoFeedOneFrame(const CtfwFrame_t *pFrame);

UINT32 AmbaCtfw_MonoVoProduceFrame(const AMBA_CV_IMG_BUF_s *pImage, UINT32 FrameIndex, CtfwFrame_t *pFrame);

UINT32 AmbaCtfw_MonoVoInit(const AMBA_CV_SPUFEX_CFG_s *pSpufexCfg, const AMBA_MONOVO_CFG_s *pMonoVoCfg,
                           CtfwGetProcResult_t GetResultCb, flexidag_memblk_t FlexiBinBlk, UINT32 PoolFrameNumTobeAlloc, CtfwFrame_t FramePool[]);

UINT32 AmbaCtfw_MonoVoStop(void);
UINT32 AmbaCtfw_MonoVoDeInit(void);
UINT32 AmbaCtfw_MonoVoStart(void);
UINT32 AmbaCtfw_MonoVoRestart(void);

#endif