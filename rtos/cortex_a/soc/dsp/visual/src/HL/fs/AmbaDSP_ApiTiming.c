/**
*  @file AmbaDSP_ApiTiming.c
*
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
*  @details DSP API Timing Check.
*
*/

#include "AmbaDSP_Log.h"
#include "AmbaDSP_ApiTiming.h"
#include "AmbaDSP_ContextUtility.h"

static Dsp_TimingApi_t *pDspApiTimingFunc = NULL;

static UINT32 IsInSafetyMode(void)
{
    UINT32 Rval = 0U;
    if (DSP_GetProfState() == DSP_PROF_STATUS_SAFETY) {
        Rval = 1U;
    }
    return Rval;
}
static UINT32 Timing_CalcEncMvBufInfo(const UINT16 Width, const UINT16 Height, const UINT32 Option, const UINT32 *pBufSize)
{
    UINT32 Rval = OK;
//    UINT32 ErrLine = 0U;
(void)Width;
(void)Height;
(void)Option;
(void)pBufSize;

//    if (Rval != OK) {
//        AmbaLL_LogUInt5("CalcEncMvBufInfo Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
//    }
    return Rval;
}

static UINT32 Timing_CalcStillYuvExtBufSize(const UINT16 StreamIdx, const UINT16 BufType, const UINT16 *pBufPitch, const UINT32 *pBufUnitSize)
{
    UINT32 Rval = OK;
//    UINT32 ErrLine = 0U;
(void)StreamIdx;
(void)BufType;
(void)pBufPitch;
(void)pBufUnitSize;

//    if (Rval != OK) {
//        AmbaLL_LogUInt5("CalcStillYuvExtBufSize Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
//    }
    return Rval;
}

static UINT32 Timing_DataCapCfg(const UINT16 CapInstance, const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)CapInstance;
(void)pDataCapCfg;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("DataCapCfg Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_DataCapCtrl(const UINT16 NumCapInstance,
                                const UINT16 *pCapInstance,
                                const AMBA_DSP_DATACAP_CTRL_s *pDataCapCtrl,
                                const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)NumCapInstance;
(void)pCapInstance;
(void)pDataCapCtrl;
(void)pAttachedRawSeq;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("DataCapCtrl Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_LiveviewConfigVinCapture(const UINT16 VinId,
                                              const UINT16 SubChNum,
                                              const AMBA_DSP_VIN_SUB_CHAN_CFG_s *pSubChCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)VinId;
(void)SubChNum;
(void)pSubChCfg;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewConfigVinCapture Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_LiveviewConfigVinPost(const UINT8 Type, const UINT16 VinId)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)Type;
(void)VinId;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewConfigVinPost Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_LiveviewFeedRawData(const UINT16 NumViewZone,
                                        const UINT16 *pViewZoneId,
                                        const AMBA_DSP_LIVEVIEW_EXT_RAW_s *pExtBuf)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT16 Idx, VinId;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    } else {
        for (Idx = 0U; Idx < NumViewZone; Idx++) {
            HL_GetViewZoneInfoPtr(pViewZoneId[Idx], &ViewZoneInfo);

            if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM) {
                VinId = (UINT16)DSP_GetU16Bit(pExtBuf[Idx].VinId, 0U, 15U);
                if (1U == DSP_GetU16Bit(pExtBuf[Idx].VinId, VIN_VIRT_IDX, 1U)) {
                    VinId += AMBA_DSP_MAX_VIN_NUM;
                }
                if (DSP_CheckCmdBufLock(VinId) != OK) {
                    Rval = DSP_ERR_0004; ErrLine = __LINE__;
                    break;
                }
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewFeedRawData Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_LiveviewFeedYuvData(const UINT16 NumViewZone,
                                        const UINT16 *pViewZoneId,
                                        const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT16 Idx, VinId, DecIdx = 0U;
    UINT8 RescChanged = (HL_GetRescState() == HL_RESC_CONFIGED)? 1U: 0U;
    UINT8 VprocState;
    CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VPROC_INFO_s VprocInfo = {0};
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    UINT8 ExitLoop = (UINT8)0U;
(void)pExtYuvBuf;

    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    } else {
        for (Idx = 0U; Idx < NumViewZone; Idx++) {
            HL_GetViewZoneInfoPtr(pViewZoneId[Idx], &pViewZoneInfo);
            VprocState = DSP_GetVprocState((UINT8)pViewZoneId[Idx]);

            if ((pViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_420) ||
                (pViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DRAM_422)) {
                HL_GetVprocInfo(HL_MTX_OPT_ALL, pViewZoneId[Idx], &VprocInfo);

                if ((VprocInfo.Status != DSP_VPROC_STATUS_IDLE2RUN) &&
                    ((RescChanged == 1U) ||
                     (DSP_VPROC_STATUS_ACTIVE != VprocState))) {
                    // DO NOTHING
                } else {
                    DSP_Bit2U16Idx(pViewZoneInfo->SourceVin, &VinId);

                    if (DSP_CheckCmdBufLock(VinId) != OK) {
                        ExitLoop = (UINT8)1U; ErrLine = __LINE__;
                    }
                }
            } else if (pViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {

                if ((RescChanged == 1U) ||
                    (DSP_VPROC_STATUS_ACTIVE != VprocState)) {
                    // DO NOTHING
                } else {
                    DSP_Bit2U16Idx(pViewZoneInfo->SourceVin, &DecIdx);
                    HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecIdx, &VidDecInfo);
                    VinId = VidDecInfo.YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM;

                    if (DSP_CheckCmdBufLock(VinId) != OK) {
                        ExitLoop = (UINT8)1U; ErrLine = __LINE__;
                    }
                }
            } else {
                // DO NOTHING
            }

            if (ExitLoop == (UINT8)1U) {
                Rval = DSP_ERR_0004;
                break;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewFeedYuvData Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_LiveviewGetIDspCfg(const UINT16 ViewZoneId,
                                       const ULONG *CfgAddr)
{
    UINT32 Rval = OK;
//    UINT32 ErrLine = 0U;
(void)ViewZoneId;
(void)CfgAddr;

//    if (Rval != OK) {
//        AmbaLL_LogUInt5("LiveviewGetIDspCfg Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
//    }
    return Rval;
}

static UINT32 Timing_LiveviewSidebandUpdate(const UINT16 ViewZoneId,
                                            const UINT8 NumBand,
                                            const ULONG *pSidebandBufAddr)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)ViewZoneId;
(void)NumBand;
(void)pSidebandBufAddr;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewSidebandUpdate Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_LiveviewSliceCfg(const UINT16 ViewZoneId,
                                     const AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)ViewZoneId;
(void)pLvSliceCfg;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewSliceCfg Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_LiveviewUpdateConfig(const UINT16 NumYuvStream,
                                         const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYuvStrmCfg,
                                         const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)NumYuvStream;
(void)pYuvStrmCfg;
(void)pAttachedRawSeq;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewUpdateConfig Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_LiveviewUpdateGeoCfg(const UINT16 ViewZoneId,
                                         const AMBA_DSP_LIVEVIEW_GEOCFG_CTRL_s *pGeoCfgCtrl,
                                         const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)ViewZoneId;
(void)pGeoCfgCtrl;
(void)pAttachedRawSeq;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewUpdateGeoCfg Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_LiveviewUpdateIsoCfg(const UINT16 NumViewZone,
                                         const AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s *pIsoCfgCtrl,
                                         const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)NumViewZone;
(void)pIsoCfgCtrl;
(void)pAttachedRawSeq;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewUpdateIsoCfg Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_LiveviewUpdatePymdCfg(const UINT16 NumViewZone,
                                          const UINT16 *pViewZoneId,
                                          const AMBA_DSP_LIVEVIEW_PYRAMID_s *pPyramid,
                                          const AMBA_DSP_LIVEVIEW_YUV_BUF_s *pPyramidBuf,
                                          const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)NumViewZone;
(void)pViewZoneId;
(void)pPyramid;
(void)pPyramidBuf;
(void)pAttachedRawSeq;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewUpdatePymdCfg Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_LiveviewUpdateVZSrcCfg(const UINT16 NumViewZone,
                                           const AMBA_DSP_LIVEVIEW_VZ_SRC_CFG_s *pVzSrcCfg,
                                           const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)NumViewZone;
(void)pVzSrcCfg;
(void)pAttachedRawSeq;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewUpdateVZSrcCfg Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_LiveviewUpdateVinCfg(const UINT16 VinId,
                                          const UINT16 SubChNum,
                                          const AMBA_DSP_VIN_SUB_CHAN_s *pSubCh,
                                          const AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s *pLvVinCfgCtrl,
                                          const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)VinId;
(void)SubChNum;
(void)pSubCh;
(void)pLvVinCfgCtrl;
(void)pAttachedRawSeq;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewUpdateVinCfg Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_LiveviewUpdateVinState(const UINT16 NumVin, const AMBA_DSP_LV_VIN_STATE_s *pVinState)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)NumVin;
(void)pVinState;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewUpdateVinState Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_LiveviewVZPostponeCfg(const UINT16 ViewZoneId, const AMBA_DSP_LV_VZ_POSTPONE_CFG_s *pVzPostPoneCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)ViewZoneId;
(void)pVzPostPoneCfg;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewVZPostponeCfg Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_LiveviewYuvStreamSync(const UINT16 YuvStrmIdx,
                                          const AMBA_DSP_LIVEVIEW_SYNC_CTRL_s *pYuvStrmSyncCtrl,
                                          const UINT32 *pSyncJobId,
                                          const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)YuvStrmIdx;
(void)pYuvStrmSyncCtrl;
(void)pSyncJobId;
(void)pAttachedRawSeq;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewYuvStreamSync Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_MainInit(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)pDspSysConfig;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("MainInit Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_MainMsgParseEntry(const UINT32 EntryArg)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)EntryArg;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("MainMsgParseEntry Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_MainResourceLimit(const AMBA_DSP_RESOURCE_s *pResource)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)pResource;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("MainResourceLimit Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_MainSetWorkArea(const ULONG WorkAreaAddr, const UINT32 WorkSize)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)WorkAreaAddr;
(void)WorkSize;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("MainSetWorkArea Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_MainWaitFlag(const UINT32 Flag, const UINT32 *pActualFlag, const UINT32 TimeOut)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)Flag;
(void)pActualFlag;
(void)TimeOut;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        (void)ErrLine;
        //AmbaLL_LogUInt5("MainWaitFlag Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_MainWaitVinInterrupt(const UINT32 Flag, const UINT32 *pActualFlag, const UINT32 TimeOut)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)Flag;
(void)pActualFlag;
(void)TimeOut;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        (void)ErrLine;
        //AmbaLL_LogUInt5("MainWaitVinInterrupt Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_MainWaitVoutInterrupt(const UINT32 Flag, const UINT32 *pActualFlag, const UINT32 TimeOut)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)Flag;
(void)pActualFlag;
(void)TimeOut;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        (void)ErrLine;
        //AmbaLL_LogUInt5("MainWaitVoutInterrupt Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_ParLoadConfig(const UINT32 Enable, const UINT32 Data)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)Enable;
(void)Data;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("ParLoadConfig Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_ParLoadRegionUnlock(const UINT16 RegionIdx, const UINT16 SubRegionIdx)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)RegionIdx;
(void)SubRegionIdx;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("ParLoadRegionUnlock Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_TestFrameConfig(const UINT32 StageId, const AMBA_DSP_TEST_FRAME_CFG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)StageId;
(void)pCfg;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("TestFrameConfig Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_TestFrameCtrl(const UINT32 NumStage, const AMBA_DSP_TEST_FRAME_CTRL_s *pCtrl)
{
    UINT32 i;
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *pResource = HL_CtxResInfoPtr;

    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    HL_GetResourcePtr(&pResource);
    for (i = 0U; i < NumStage; i++) {
        if (pResource->TestFrmStatus[pCtrl[i].StageId] == DSP_TESTFRAME_STATE_DISABLE) {
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("TestFrameCtrl Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_MainSafetyChk(const UINT32 ID, const UINT32 SubID, const UINT32 *pStatus)
{
    UINT32 Rval = OK, ErrLine = 0U;
    (void)ID;
    (void)SubID;
    (void)pStatus;

    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("Timing_MainSafetyChk Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_MainSafetyCfg(const UINT32 ID, const UINT32 Val0, const UINT32 Val1)
{
    UINT32 Rval = OK, ErrLine = 0U;
    (void)ID;
    (void)Val0;
    (void)Val1;

    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("Timing_MainSafetyCfg Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_StillDecStart(const UINT16 StreamIdx, const AMBA_DSP_STLDEC_START_CONFIG_s *pDecConfig)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)StreamIdx;
(void)pDecConfig;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillDecStart Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_UpdateCapBuffer(const UINT16 CapInstance, const AMBA_DSP_DATACAP_BUF_CFG_s *pCapBuf, const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)CapInstance;
(void)pCapBuf;
(void)pAttachedRawSeq;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("UpdateCapBuffer Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoDecBitsFifoUpdate(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_BITS_FIFO_s *pBitsFifo)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)NumStream;
(void)pStreamIdx;
(void)pBitsFifo;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoDecBitsFifoUpdate Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoDecConfig(const UINT16 MaxNumStream, const AMBA_DSP_VIDDEC_STREAM_CONFIG_s *pStreamConfig)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)MaxNumStream;
(void)pStreamConfig;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoDecConfig Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoDecStart(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)NumStream;
(void)pStreamIdx;
(void)pStartConfig;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoDecStart Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoDecStop(const UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pShowLastFrame)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)NumStream;
(void)pStreamIdx;
(void)pShowLastFrame;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoDecStop Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoDecTrickPlay(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_TRICKPLAY_s *pTrickPlay)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)NumStream;
(void)pStreamIdx;
(void)pTrickPlay;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoDecTrickPlay Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VoutConfigMixerBinding(const UINT8 NumVout,
                                        const UINT8 *pVoutIdx,
                                        const AMBA_DSP_VOUT_DATA_PATH_CFG_s *pConfig)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)NumVout;
(void)pVoutIdx;
(void)pConfig;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutConfigMixerBinding Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VoutDisplayConfig(const UINT8 VoutIdx,
                                      const AMBA_DSP_DISPLAY_CONFIG_s *pConfig)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)VoutIdx;
(void)pConfig;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutDisplayConfig Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VoutDisplayConfigGamma(const UINT8 VoutIdx, ULONG TableAddr)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)VoutIdx;
(void)TableAddr;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutDisplayConfigGamma Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VoutDisplayControlGamma(const UINT8 VoutIdx, const UINT8 Enable)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)VoutIdx;
(void)Enable;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutDisplayControlGamma Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VoutDisplayCtrl(const UINT8 VoutIdx)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)VoutIdx;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutDisplayCtrl Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VoutMixerConfig(const UINT8 VoutIdx, const AMBA_DSP_VOUT_MIXER_CONFIG_s *pConfig)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)VoutIdx;
(void)pConfig;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutMixerConfig Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VoutMixerConfigBackColor(const UINT8 VoutIdx, const UINT32 BackColorYUV)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)VoutIdx;
(void)BackColorYUV;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutMixerConfigBackColor Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VoutMixerConfigCsc(const UINT8 VoutIdx, const UINT8 CscCtrl)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)VoutIdx;
(void)CscCtrl;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutMixerConfigCsc Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VoutMixerConfigHighlightColor(const UINT8 VoutIdx,
                                                   const UINT8 LumaThreshold,
                                                   const UINT32 HighlightColorYUV)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)VoutIdx;
(void)LumaThreshold;
(void)HighlightColorYUV;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutMixerConfigHighlightColor Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VoutMixerCscMatrixConfig(const UINT8 VoutIdx, const dsp_vout_csc_matrix_s *pCscMatrix)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)VoutIdx;
(void)pCscMatrix;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutMixerCscMatrixConfig Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VoutMixerCtrl(const UINT8 VoutIdx)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)VoutIdx;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutMixerCtrl Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VoutOsdConfigBuf(const UINT8 VoutIdx,
                                     const AMBA_DSP_VOUT_OSD_BUF_CONFIG_s *pBufConfig)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)VoutIdx;
(void)pBufConfig;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutOsdConfigBuf Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VoutOsdCtrl(const UINT8 VoutIdx,
                                 const UINT8 Enable,
                                 const UINT8 SyncWithVin,
                                 const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)VoutIdx;
(void)Enable;
(void)SyncWithVin;
(void)pAttachedRawSeq;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutOsdCtrl Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VoutReset(const UINT8 VoutIdx)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)VoutIdx;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutReset Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VoutVideoConfig(const UINT8 VoutIdx,
                                    const AMBA_DSP_VOUT_VIDEO_CFG_s *pConfig)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)VoutIdx;
(void)pConfig;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutVideoConfig Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VoutVideoCtrl(const UINT8 VoutIdx,
                                   const UINT8 Enable,
                                   const UINT8 SyncWithVin,
                                   const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)VoutIdx;
(void)Enable;
(void)SyncWithVin;
(void)pAttachedRawSeq;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutVideoCtrl Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}



/* VideoEncode */
static UINT32 Timing_VideoEncConfig(const UINT16 NumStream,
                                    const UINT16 *pStreamIdx,
                                    const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pStreamConfig)
{
    UINT32 Rval = OK, ErrLine = 0U;

(void)NumStream;
(void)pStreamIdx;
(void)pStreamConfig;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncConfig Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoEncStart(const UINT16 NumStream,
                                   const UINT16 *pStreamIdx,
                                   const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStartConfig,
                                   const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 Mask = 0U;
    UINT16 i;
    UINT8 EncState;
(void)pStartConfig;
(void)pAttachedRawSeq;

    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }
    if (Rval == OK) {
        HL_GetResourcePtr(&Resource);
        if (Resource->ParLoadEn == 1U) {
            Mask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
            if (Mask == 0U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        }
    }
    if (Rval == OK) {
        /* Prevent user to stop start cmd during encode busy status */
        for (i = 0U; i < NumStream; i++) {
            EncState = DSP_GetEncState(pStreamIdx[i]);
            if (EncState == DSP_ENC_STATUS_BUSY) {
                Rval = DSP_ERR_0004; ErrLine = __LINE__; break;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncStart Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoEncStop(const UINT16 NumStream,
                                  const UINT16 *pStreamIdx,
                                  const UINT8 *pStopOption,
                                  const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 Mask = 0U;
    UINT16 i;
    UINT8 EncState;
(void)pStopOption;
(void)pAttachedRawSeq;

    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval == OK) {
        HL_GetResourcePtr(&Resource);
        if (Resource->ParLoadEn == 1U) {
            Mask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
            if (Mask == 0U) {
                Rval = DSP_ERR_0004; ErrLine = __LINE__;
            }
        }
    }

    if (Rval == OK) {
        /* Prevent user to stop encode cmd during encode idle status */
        for (i = 0U; i < NumStream; i++) {
            EncState = DSP_GetEncState(pStreamIdx[i]);
            if (EncState == DSP_ENC_STATUS_IDLE) {
                Rval = DSP_ERR_0004; ErrLine = __LINE__; break;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncStop Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoEncControlFrameRate(const UINT16 NumStream,
                                              const UINT16 *pStreamIdx,
                                              const UINT32 *pDivisor,
                                              const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 Mask = 0U;
(void)NumStream;
(void)pStreamIdx;
(void)pDivisor;
(void)pAttachedRawSeq;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }
    if (Rval == OK) {
        HL_GetResourcePtr(&Resource);
        if (Resource->ParLoadEn == 1U) {
            Mask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
            if (Mask == 0U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncCtrlFrate Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoEncControlRepeatDrop(const UINT16 NumStream,
                                               const UINT16* pStreamIdx,
                                               const AMBA_DSP_VIDEO_REPEAT_DROP_CONFIG_s *pRepeatDropCfg,
                                               const UINT64* pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 Mask = 0U;
(void)NumStream;
(void)pStreamIdx;
(void)pRepeatDropCfg;
(void)pAttachedRawSeq;

    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }
    if (Rval == OK) {
        HL_GetResourcePtr(&Resource);
        if (Resource->ParLoadEn == 1U) {
            Mask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
            if (Mask == 0U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncCtrlRepeatDrop Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoEncControlQuality(const UINT16 NumStream,
                                            const UINT16 *pStreamIdx,
                                            const AMBA_DSP_VIDEO_ENC_QUALITY_s *pQCtrl)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT32 Mask;
(void)NumStream;
(void)pStreamIdx;
(void)pQCtrl;

    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }
    if (Rval == OK) {
        HL_GetResourcePtr(&Resource);
        if (Resource->ParLoadEn == 1U) {
            Mask = DSP_GetBit(Resource->ParLoadMask, (DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN), PARLOAD_IDX_LEN);
            if (Mask == 0U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncCtrlQt Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoEncControlBlend(const UINT16 NumStream,
                                          const UINT16 *pStreamIdx,
                                          const AMBA_DSP_VIDEO_BLEND_CONFIG_s *pBlendCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 Mask = 0U;
(void)NumStream;
(void)pStreamIdx;
(void)pBlendCfg;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }
    if (Rval == OK) {
        HL_GetResourcePtr(&Resource);
        if (Resource->ParLoadEn == 1U) {
            Mask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
            if (Mask == 0U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncControlBlend Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 Timing_VideoEncExecIntervalCap(const UINT16 NumStream,
                                             const UINT16 *pStreamIdx)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 Mask = 0U;
(void)NumStream;
(void)pStreamIdx;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }
    if (Rval == OK) {
        HL_GetResourcePtr(&Resource);
        if (Resource->ParLoadEn == 1U) {
            Mask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
            if (Mask == 0U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncExecIntCap Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoEncFeedYuvData(const UINT16 NumStream,
                                         const UINT16 *pStreamIdx,
                                         const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 Mask = 0U;
(void)NumStream;
(void)pStreamIdx;
(void)pExtYuvBuf;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }
    if (Rval == OK) {
        HL_GetResourcePtr(&Resource);
        if (Resource->ParLoadEn == 1U) {
            Mask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
            if (Mask == 0U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncFeedYuvData Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_VideoEncGrpConfig(const AMBA_DSP_VIDEO_ENC_GROUP_s *pGrpCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 Mask = 0U;
(void)pGrpCfg;

    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }
    if (Rval == OK) {
        HL_GetResourcePtr(&Resource);
        if (Resource->ParLoadEn == 1U) {
            Mask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
            if (Mask == 0U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncRcGrpConfig Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

#ifdef SUPPORT_DSP_MV_DUMP
static UINT32 Timing_VideoEncMvConfig(const UINT16 StreamIdx, const AMBA_DSP_VIDEO_ENC_MV_CFG_s *pMvCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 Mask = 0U;
(void)StreamIdx;
(void)pMvCfg;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }
    if (Rval == OK) {
        HL_GetResourcePtr(&Resource);
        if (Resource->ParLoadEn == 1U) {
            Mask = (UINT8)DSP_GetBit(Resource->ParLoadMask, (UINT32)DSP_REGION_ENC_IDX*PARLOAD_IDX_LEN, PARLOAD_IDX_LEN);
            if (Mask == 0U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncMvConfig Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}
#endif

static UINT32 Timing_VideoEncDescFmtConfig(const UINT16 StreamIdx, const UINT16 CatIdx, const UINT32 OptVal)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)StreamIdx;
(void)CatIdx;
(void)OptVal;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncDescFmtConfig Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

/* StillCapture */
static UINT32 Timing_StillYuv2Yuv(const AMBA_DSP_YUV_IMG_BUF_s *pYuvIn, const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, const UINT32 Opt, const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT16 VprocIdx;
    CTX_STILL_INFO_s StlInfo = {0};
(void)pYuvIn;
(void)pYuvOut;
(void)pIsoCfg;
(void)Opt;
(void)pAttachedRawSeq;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    } else {
        //TBD
    }

    if (Rval == OK) {
#ifdef SUPPORT_VPROC_RT_RESTART
        HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
        VprocIdx = StlInfo.YuvInVprocId;
        if ((StlInfo.RawInVprocId == VprocIdx) && (StlInfo.RawInVprocStatus == STL_VPROC_STATUS_RUN)) {
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        } else if (StlInfo.YuvInVprocStatus == STL_VPROC_STATUS_RUN) {
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        } else {
            //TBD
        }
#else
        HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
        VprocIdx = (pYuvIn->DataFmt == AMBA_DSP_YUV420) ? StlInfo.YuvInVprocId: StlInfo.Yuv422InVprocId);
        if ((StlInfo.RawInVprocId == VprocIdx) && (StlInfo.RawInVprocStatus == STL_VPROC_STATUS_RUN)) {
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        } else if ((StlInfo.YuvInVprocId == VprocIdx) && (StlInfo.YuvInVprocStatus == STL_VPROC_STATUS_RUN)) {
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        } else if ((StlInfo.Yuv422InVprocId == VprocIdx) && (StlInfo.Yuv422InVprocStatus == STL_VPROC_STATUS_RUN)) {
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        } else {
            //TBD
        }
#endif
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillYuv2Yuv Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_StillRaw2Yuv(const AMBA_DSP_RAW_BUF_s *pRawIn, const AMBA_DSP_BUF_s *pAuxBufIn, const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, const UINT32 Opt, const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT16 VprocIdx ;
    CTX_STILL_INFO_s StlInfo = {0};
(void)pRawIn;
(void)pAuxBufIn;
(void)pYuvOut;
(void)pIsoCfg;
(void)Opt;
(void)pAttachedRawSeq;
    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval == OK) {
        HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
        VprocIdx = StlInfo.RawInVprocId;

        if (StlInfo.RawInVprocStatus == STL_VPROC_STATUS_RUN) {
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        } else if ((StlInfo.YuvInVprocId == VprocIdx) && (StlInfo.YuvInVprocStatus == STL_VPROC_STATUS_RUN)) {
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
#ifndef SUPPORT_VPROC_RT_RESTART
        } else if ((StlInfo.Yuv422InVprocId == VprocIdx) && (StlInfo.Yuv422InVprocStatus == STL_VPROC_STATUS_RUN)) {
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
#endif
        } else {
            //TBD
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillRaw2Yuv Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 Timing_StillRaw2Raw(const AMBA_DSP_RAW_BUF_s *pRawIn, const AMBA_DSP_BUF_s *pAuxBufIn, const AMBA_DSP_RAW_BUF_s *pYuvOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, const UINT32 Opt, const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT16 VprocIdx;
    CTX_STILL_INFO_s StlInfo = {0};
(void)pRawIn;
(void)pAuxBufIn;
(void)pYuvOut;
(void)pIsoCfg;
(void)Opt;
(void)pAttachedRawSeq;

    if (IsInSafetyMode() == 1U) {
        Rval = DSP_ERR_0004; ErrLine = __LINE__;
    }

    if (Rval == OK) {
        HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
        VprocIdx = StlInfo.RawInVprocId;

        if (StlInfo.RawInVprocStatus == STL_VPROC_STATUS_RUN) {
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        } else if ((StlInfo.YuvInVprocId == VprocIdx) && (StlInfo.YuvInVprocStatus == STL_VPROC_STATUS_RUN)) {
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
#ifndef SUPPORT_VPROC_RT_RESTART
        } else if ((StlInfo.Yuv422InVprocId == VprocIdx) && (StlInfo.Yuv422InVprocStatus == STL_VPROC_STATUS_RUN)) {
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
#endif
        } else {
            //TBD
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillRaw2Raw Timing Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

void TimingApiReg(const UINT8 Disable, const UINT32 Mask, const UINT8 IsAutoInit)
{
    static UINT8 IsUserSetup = 0U;
    static Dsp_TimingApi_t DspApiTimingFunc = {
        /* Main */
        .pMainInit = Timing_MainInit,
        .pMainSetWorkArea = Timing_MainSetWorkArea,
        .pMainMsgParseEntry = Timing_MainMsgParseEntry,
        .pMainWaitVinInterrupt = Timing_MainWaitVinInterrupt,
        .pMainWaitVoutInterrupt = Timing_MainWaitVoutInterrupt,
        .pMainWaitFlag = Timing_MainWaitFlag,
        .pMainResourceLimit = Timing_MainResourceLimit,
        .pParLoadConfig = Timing_ParLoadConfig,
        .pParLoadRegionUnlock = Timing_ParLoadRegionUnlock,
        .pCalcEncMvBufInfo = Timing_CalcEncMvBufInfo,
        .pTestFrameConfig = Timing_TestFrameConfig,
        .pTestFrameCtrl = Timing_TestFrameCtrl,
        .pMainSafetyChk = Timing_MainSafetyChk,
        .pMainSafetyCfg = Timing_MainSafetyCfg,

        /* Liveview */
        .pLiveviewUpdateConfig = Timing_LiveviewUpdateConfig,
        .pLiveviewUpdateIsoConfig = Timing_LiveviewUpdateIsoCfg,
        .pLiveviewUpdateVinCfg = Timing_LiveviewUpdateVinCfg,
        .pLiveviewUpdatePymdCfg = Timing_LiveviewUpdatePymdCfg,
        .pLiveviewConfigVinCapture = Timing_LiveviewConfigVinCapture,
        .pLiveviewConfigVinPost = Timing_LiveviewConfigVinPost,
        .pLiveviewFeedRawData = Timing_LiveviewFeedRawData,
        .pLiveviewFeedYuvData = Timing_LiveviewFeedYuvData,
        .pLiveviewGetIDspCfg = Timing_LiveviewGetIDspCfg,
        .pLiveviewSidebandUpdate = Timing_LiveviewSidebandUpdate,
        .pLiveviewYuvStreamSync = Timing_LiveviewYuvStreamSync,
        .pLiveviewUpdateGeoCfg = Timing_LiveviewUpdateGeoCfg,
        .pLiveviewUpdateVZSrcCfg = Timing_LiveviewUpdateVZSrcCfg,
        .pLiveviewSliceCfg = Timing_LiveviewSliceCfg,
        .pLiveviewVZPostponeCfg = Timing_LiveviewVZPostponeCfg,
        .pLiveviewUpdateVinState = Timing_LiveviewUpdateVinState,

        /* StillCapture */
        .pDataCapCfg = Timing_DataCapCfg,
        .pUpdateCapBuffer = Timing_UpdateCapBuffer,
        .pDataCapCtrl = Timing_DataCapCtrl,
        .pStillYuv2Yuv = Timing_StillYuv2Yuv,
        .pCalcStillYuvExtBufSize = Timing_CalcStillYuvExtBufSize,
        .pStillRaw2Yuv = Timing_StillRaw2Yuv,
        .pStillRaw2Raw = Timing_StillRaw2Raw,

        /* VideoEncode */
        .pVideoEncConfig = Timing_VideoEncConfig,
        .pVideoEncStart = Timing_VideoEncStart,
        .pVideoEncStop = Timing_VideoEncStop,
        .pVideoEncControlFrameRate = Timing_VideoEncControlFrameRate,
        .pVideoEncControlRepeatDrop = Timing_VideoEncControlRepeatDrop,
        .pVideoEncControlQuality = Timing_VideoEncControlQuality,
        .pVideoEncControlBlend = Timing_VideoEncControlBlend,
        .pVideoEncExecIntervalCap = Timing_VideoEncExecIntervalCap,
        .pVideoEncFeedYuvData = Timing_VideoEncFeedYuvData,
        .pVideoEncGrpConfig = Timing_VideoEncGrpConfig,
#ifdef SUPPORT_DSP_MV_DUMP
        .pVideoEncMvConfig = Timing_VideoEncMvConfig,
#endif
        .pVideoEncDescFmtConfig = Timing_VideoEncDescFmtConfig,

        /* Vout */
        .pVoutReset = Timing_VoutReset,
        .pVoutVideoConfig = Timing_VoutVideoConfig,
        .pVoutVideoCtrl = Timing_VoutVideoCtrl,
        .pVoutOsdConfigBuf = Timing_VoutOsdConfigBuf,
        .pVoutOsdCtrl = Timing_VoutOsdCtrl,
        .pVoutDisplayConfig = Timing_VoutDisplayConfig,
        .pVoutDisplayCtrl = Timing_VoutDisplayCtrl,
        .pVoutMixerConfig = Timing_VoutMixerConfig,
        .pVoutMixerConfigBackColor = Timing_VoutMixerConfigBackColor,
        .pVoutMixerConfigHighlightColor = Timing_VoutMixerConfigHighlightColor,
        .pVoutMixerConfigCsc = Timing_VoutMixerConfigCsc,
        .pVoutMixerCscMatrixConfig = Timing_VoutMixerCscMatrixConfig,
        .pVoutMixerCtrl = Timing_VoutMixerCtrl,
        .pVoutDisplayConfigGamma = Timing_VoutDisplayConfigGamma,
        .pVoutDisplayControlGamma = Timing_VoutDisplayControlGamma,
        .pVoutConfigMixerBinding = Timing_VoutConfigMixerBinding,

        /* StillDecode */
        .pStillDecStart = Timing_StillDecStart,

        /* VideoDecode */
        .pVideoDecConfig = Timing_VideoDecConfig,
        .pVideoDecStart = Timing_VideoDecStart,
        .pVideoDecBitsFifoUpdate = Timing_VideoDecBitsFifoUpdate,
        .pVideoDecStop = Timing_VideoDecStop,
        .pVideoDecTrickPlay = Timing_VideoDecTrickPlay,

    };
//FIXME, Misra
(void)Mask;

    if (IsAutoInit == 0U) {
        IsUserSetup = 1U;
        if (Disable == 0U) {
            pDspApiTimingFunc = &DspApiTimingFunc;
        } else {
            pDspApiTimingFunc = NULL;
        }
    } else if (IsUserSetup == 0U) {
        if (Disable == 0U) {
            pDspApiTimingFunc = &DspApiTimingFunc;
        } else {
            pDspApiTimingFunc = NULL;
        }
    } else {
        //
    }
}

Dsp_TimingApi_t* AmbaDSP_GetTimingApiFunc(void){
    return pDspApiTimingFunc;
}
