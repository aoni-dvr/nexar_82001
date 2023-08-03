/**
 *  @file AmbaDSP_Liveview.h
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for Ambarella DSP Driver Liveview APIs
 *
 */
#ifndef AMBA_DSP_LIVEVIEW_H
#define AMBA_DSP_LIVEVIEW_H

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview_Def.h"
#include "AmbaDSP_EventInfo.h"

UINT32 AmbaDSP_LiveviewConfig(UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pViewZoneCfg, UINT16 NumYUVStream, const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg);

UINT32 AmbaDSP_LiveviewCtrl(UINT16 NumViewZone, const UINT16 *pViewZoneId, const UINT8 *pEnable);

UINT32 AmbaDSP_LiveviewUpdateConfig(UINT16 NumYuvStream, const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYuvStrmCfg, UINT64 *pAttachedRawSeq);

UINT32 AmbaDSP_LiveviewUpdateIsoCfg(UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s *pIsoCfgCtrl, UINT64 *pAttachedRawSeq);

UINT32 AmbaDSP_LiveviewUpdateVinCfg(UINT16 VinId,
                                    UINT16 SubChNum,
                                    const AMBA_DSP_VIN_SUB_CHAN_s *pSubCh,
                                    const AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s *pLvVinCfgCtrl,
                                    UINT64 *pAttachedRawSeq);

UINT32 AmbaDSP_LiveviewUpdatePymdCfg(UINT16 NumViewZone,
                                     const UINT16 *pViewZoneId,
                                     const AMBA_DSP_LIVEVIEW_PYRAMID_s *pPyramid,
                                     const AMBA_DSP_LIVEVIEW_YUV_BUF_s *pPyramidBuf,
                                     UINT64 *pAttachedRawSeq);

UINT32 AmbaDSP_LiveviewConfigVinCapture(UINT16 VinId, UINT16 SubChNum, const AMBA_DSP_VIN_SUB_CHAN_CFG_s *pSubChCfg);

UINT32 AmbaDSP_LiveviewConfigVinPost(const UINT8 Type, UINT16 VinId);

UINT32 AmbaDSP_LiveviewFeedRawData(UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_LIVEVIEW_EXT_RAW_s *pExtBuf);

UINT32 AmbaDSP_LiveviewFeedYuvData(UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf);

UINT32 AmbaDSP_LiveviewYuvStreamSync(UINT16 YuvStrmIdx, const AMBA_DSP_LIVEVIEW_SYNC_CTRL_s *pYuvStrmSyncCtrl, UINT32 *pSyncJobId, UINT64 *pAttachedRawSeq);

UINT32 AmbaDSP_LiveviewUpdateGeoCfg(UINT16 ViewZoneId, const AMBA_DSP_LIVEVIEW_GEOCFG_CTRL_s *pGeoCfgCtrl, UINT64 *pAttachedRawSeq);

UINT32 AmbaDSP_LiveviewUpdateVZSource(UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_VZ_SRC_CFG_s *pVzSrcCfg, UINT64 *pAttachedRawSeq);

UINT32 AmbaDSP_LiveviewSideBandUpdate(UINT16 ViewZoneId, UINT8 NumBand, const ULONG *pSidebandBufAddr);

UINT32 AmbaDSP_LiveviewSliceCfg(UINT16 ViewZoneId, const AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg);

UINT32 AmbaDSP_LiveviewVZPostponeCfg(UINT16 ViewZoneId, const AMBA_DSP_LV_VZ_POSTPONE_CFG_s *pVzPostPoneCfg);

UINT32 AmbaDSP_LiveviewUpdateVinState(UINT16 NumVin, const AMBA_DSP_LV_VIN_STATE_s *pVinState);

UINT32 AmbaDSP_LiveviewSliceCfgCalc(UINT16 ViewZoneId, AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg);

UINT32 AmbaDSP_LiveviewSlowShutterCtrl(UINT16 NumViewZone, const UINT16* pViewZoneId, const UINT32* pRatio);

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
UINT32 AmbaDSP_LiveviewDropRepeatCtrl(UINT16 NumViewZone, const UINT16* pViewZoneId, const AMBA_DSP_LIVEVIEW_DROP_REPEAT_s* pDropRptCfg);
#endif

#endif  /* AMBA_DSP_LIVEVIEW_H */
