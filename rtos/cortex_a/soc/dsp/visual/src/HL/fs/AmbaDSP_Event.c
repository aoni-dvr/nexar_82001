/**
 *  @file AmbaDSP_Event.c
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
 *  @details Implementation of SSP Event API
 *
 */

#include "AmbaDSP_Log.h"
#include "AmbaDSP_Cmds.h"
#include "AmbaDSP_Priv.h"
#include "AmbaDSP_ArmComm.h"
#include "AmbaDSP_EventCtrl.h"
#include "AmbaDSP_EncodeAPI.h"
#include "AmbaDSP_EncodeUtility.h"
#include "AmbaDSPSimilar_EncodeUtility.h"
#include "AmbaDSP_Image3aStatistics.h"
#include "dsp_priv_api.h"
#include "AmbaDSP_Monitor.h"
#include "AmbaDSP_TimeManager.h"

#define MAX_PREPROC_HANDLER (3U)

static void PtsOverflowHdlr(UINT32 LastPts, UINT32 Pts32, UINT64 *PtsBase, UINT64 *Pts64)
{
    *Pts64 = (UINT64)Pts32;
    if (LastPts > Pts32) {
        *PtsBase += 0x100000000ULL;
    }
    *Pts64 += *PtsBase;
}

static inline UINT32 HL_PreProcCheckStlStatus(UINT16 ChannelId,
                                             UINT8 DataFmt,
                                             const CTX_STILL_INFO_s *pStlInfo,
                                             UINT8 *pRawInVprocIdCheck,
                                             UINT8 *pYuvInVprocIdCheck,
                                             UINT8 *pRawInVprocStatusCheck,
                                             UINT8 *pYuvInVprocStatusCheck)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (pStlInfo->RawInVprocId == ChannelId) {
        *pRawInVprocIdCheck = 1U;
        if (pStlInfo->RawInVprocStatus == STL_VPROC_STATUS_RUN) {
            *pRawInVprocStatusCheck = 1U;
        }
    }

#ifndef SUPPORT_VPROC_RT_RESTART
    if ((DataFmt == AMBA_DSP_YUV420) &&
        (pStlInfo->YuvInVprocId == ChannelId)) {
        *pYuvInVprocIdCheck = 1U;
        if (pStlInfo->YuvInVprocStatus == STL_VPROC_STATUS_RUN) {
            *pYuvInVprocStatusCheck = 1U;
        }
    } else if ((DataFmt == AMBA_DSP_YUV422) &&
               (pStlInfo->Yuv422InVprocId == ChannelId)) {
        *pYuvInVprocIdCheck = 1U;
        if (pStlInfo->Yuv422InVprocStatus == STL_VPROC_STATUS_RUN) {
            *pYuvInVprocStatusCheck = 1U;
        }
    }
#else
    (void)DataFmt;
    if (pStlInfo->YuvInVprocId == ChannelId) {
        *pYuvInVprocIdCheck = 1U;
        if (pStlInfo->YuvInVprocStatus == STL_VPROC_STATUS_RUN) {
            *pYuvInVprocStatusCheck = 1U;
        }
    }
#endif

    return Rval;
}

static UINT32 LiveviewRawReadyVinInfo(const DSP_RAW_CAPTURED_BUF_s *pRawCapBuf, AMBA_DSP_RAW_DATA_RDY_EXT_s *pRawData, UINT16 *pPhyVinId, UINT8 *pIsVirtVin)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    UINT16 TmpVinId = 0U;

    if (pRawCapBuf->IsCompRaw == (UINT8)1U) {
        DSP_SetU8Bit(&pRawData->IsVirtChan, VIEWZONE_PROC_RAW_BIT_IDX);
        Rval = HL_GetViewZoneVinId(pRawData->VinId/* ViewZoneId */, &TmpVinId);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);

        if (Rval == DSP_ERR_NONE) {
            Rval = HL_IsVirtualVinIdx(TmpVinId, pIsVirtVin);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
        if (*pIsVirtVin == 0U) {
            *pPhyVinId = TmpVinId;
        } else {
            if (Rval == DSP_ERR_NONE) {
                Rval = HL_GetTimeDivisionVirtVinInfo(TmpVinId, pPhyVinId);
                if (*pPhyVinId == DSP_VIRT_VIN_IDX_INVALID) {
                    *pPhyVinId = 0U;
                }
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
            }
        }
    } else {
        *pPhyVinId = pRawData->VinId;
        Rval = HL_IsVirtualVinIdx(*pPhyVinId, pIsVirtVin);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    return Rval;

}

static inline void HL_IsLvRawDataRdyLastSlice(const DSP_RAW_CAPTURED_BUF_s *pRawCapBuf,
                                              UINT8 *pIsLastSliceIdx)
{
    if (pRawCapBuf->CapSliceNum == 0U) {
        *pIsLastSliceIdx = 0U;
    } else {
        if (pRawCapBuf->CapSliceIdx == (pRawCapBuf->CapSliceNum - 1U)) {
            *pIsLastSliceIdx = 1U;
        }
    }
}

static inline void HL_LvRawDataRdyVirtVinUpt(const CTX_VIN_INFO_s *pVinInfo,
                                             UINT16 PhyVinId,
                                             const DSP_RAW_CAPTURED_BUF_s *pRawCapBuf,
                                             AMBA_DSP_RAW_DATA_RDY_EXT_s *pRawData)
{
    UINT16 TdVinIdx = 0U, i;

    if (pVinInfo->TimeDivisionNum[0U] > 0U) {
        DSP_SetU8Bit(&pRawData->IsVirtChan, VIEWZONE_VIN_TD_BIT_IDX);

        if (PhyVinId == pRawCapBuf->TargetVinId) { //TimeDivision of MasterVin, MUST be first Index
            TdVinIdx = 0U;
            pRawData->VinVirtChId |= (TdVinIdx << 8U); // B[8:15] for TDIdx
        } else { //TimeDivision of SlaveVin
            for (i=1U; i<pVinInfo->TimeDivisionNum[0U]; i++) {
                if (pRawCapBuf->TargetVinId == (pVinInfo->TimeDivisionVinId[0U][i] + AMBA_DSP_MAX_VIN_NUM)) {
                    TdVinIdx = i;
                    break;
                }
            }
            pRawData->VinVirtChId |= (TdVinIdx << 8U); // B[8:15] for TDIdx
        }
    } else {
        DSP_ClearU8Bit(&pRawData->IsVirtChan, VIEWZONE_VIN_TD_BIT_IDX);
    }
}

static UINT32 LiveviewRawReadyPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    UINT64 RawCapPts64 = 0ULL, RawCapSeq64 = 0ULL;
    CTX_VIN_INFO_s VinInfo = {0};
    DSP_RAW_CAPTURED_BUF_s *pRawCapBuf;
    AMBA_DSP_RAW_DATA_RDY_EXT3_s *pRawDataRdyExt3;
    AMBA_DSP_RAW_DATA_RDY_EXT_s *pRawData;
    DSP_STL_PROC_JOB_ID_s StlProcJobId = {0};
    CTX_STILL_INFO_s StlInfo = {0};
    CTX_BATCHQ_INFO_s BatchQInfo = {0};
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;
    UINT8 RawInVprocIdCheck = 0U, YuvInVprocIdCheck = 0U;
    UINT8 RawInVprocStatusCheck = 0U, YuvInVprocStatusCheck = 0U;
    UINT8 IsVirtVin = 0U, IsLastSliceIdx = 0U;
    UINT16 PhyVinId = 0U;
    UINT8 RawSeqId;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
#ifdef USE_TIME_MANAGER
    UINT64 InTimeU64, OutTimeU64 = 0ULL;
#endif

    dsp_osal_typecast(&pRawCapBuf, &pEventData);
    pRawDataRdyExt3 = &pRawCapBuf->RawDataRdy;
    dsp_osal_typecast(&pRawData, &pRawDataRdyExt3);

    // Update the status of the vproc for still R2R
    // Get StlProcId from BatchCmdId and then release BatchCmdId
    HL_GetDspBatchQInfo(pRawCapBuf->BatchCmdId, &BatchQInfo);
    Rval = dsp_osal_memcpy(&StlProcJobId, &BatchQInfo.StlProcId, sizeof(UINT32));
    DSP_FillErrline(Rval, &ErrLine, __LINE__);
    HL_UpdateCtxViewZoneBatchQRPtr(pRawCapBuf->BatchCmdId);
    if (pIsSimilarEnabled != NULL) {
        if (1U == IsSimilarEnabled()) {
            SIM_UpdateCtxViewZoneBatchQRPtr(pRawCapBuf->BatchCmdId);
        }
    }
    pRawCapBuf->BatchCmdId = 0U;

    HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
    if ((StlProcJobId.DataFmt == 2U/*Raw2Raw*/) &&
        (pRawData->VinId == StlProcJobId.VprocId)) {

        Rval = HL_PreProcCheckStlStatus((UINT16)pRawData->VinId,
                                        (UINT8)0U/*DataFmt*/,
                                        &StlInfo,
                                        &RawInVprocIdCheck,
                                        &YuvInVprocIdCheck,
                                        &RawInVprocStatusCheck,
                                        &YuvInVprocStatusCheck);

        /* Redirect to stl cfa aaa if needed */
        if ((RawInVprocIdCheck == 1U) && (RawInVprocStatusCheck == 1U)) {
            AMBA_DSP_RAW_DATA_RDY_s *pStlRawData = NULL;

            Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_STL_RAW_POSTPROC_RDY);
            dsp_osal_typecast(&pStlRawData, &EventBufAddr);
            Rval = dsp_osal_memcpy(pStlRawData, pRawData, sizeof(AMBA_DSP_RAW_DATA_RDY_s));
            if (pStlRawData->RawBuffer.Compressed == 0U) {
                //Raw rescale output always be uncompressed
                pStlRawData->RawBuffer.Window.Width /= 2U;
            }
            HL_GetStlInfo(HL_MTX_OPT_GET, &StlInfo);
            StlInfo.RawInVprocStatus = STL_VPROC_STATUS_COMPLETE;
            HL_SetStlInfo(HL_MTX_OPT_SET, &StlInfo);
            /* Still Yuv Event */
            Rval = DSP_GiveEvent(LL_EVENT_STL_RAW_POSTPROC_RDY, pStlRawData, EventBufIdx);
            if (Rval == DSP_ERR_NONE) {
                //DO NOTHING
            }
        }
        //discard this event, doesn't need to report error code
        Rval = DSP_ERR_0004;
    }

    if (Rval == DSP_ERR_NONE) {
        Rval = LiveviewRawReadyVinInfo(pRawCapBuf, pRawData, &PhyVinId, &IsVirtVin);

        HL_IsLvRawDataRdyLastSlice(pRawCapBuf, &IsLastSliceIdx);
        if ((pRawCapBuf->IsCompRaw == (UINT8)1U) ||
            ((IsVirtVin == 0U) && (IsLastSliceIdx == 1U))) {
            HL_GetVinInfo(HL_MTX_OPT_GET, PhyVinId, &VinInfo);

            if (VinInfo.VinCtrl.VinState == DSP_VIN_STATUS_TIMER) {
                /* The case "VinState is not ACTIVE but raw data from physical Vin is received" means that
                 * Vin is not activated in Liveview control, i.e. it's from data-capture-control */
                Rval = DSP_ERR_0001;
            }

            HL_LvRawDataRdyVirtVinUpt(&VinInfo, PhyVinId, pRawCapBuf, pRawData);

            pRawData->RawBuffer.Compressed = VinInfo.CfaCompressed;

            /* ProcessRaw is final raw */
            if (pRawCapBuf->IsCompRaw == (UINT8)0U) {
                pRawData->RawBuffer.Window.Width = VinInfo.CapWindow[0U].Width;
                pRawData->RawBuffer.Window.Height = VinInfo.CapWindow[0U].Height;

                if (pRawData->RawBuffer.Pitch == pRawData->AuxBuffer.Pitch) {
                    pRawData->AuxBuffer.Window.Width = VinInfo.CapWindow[0U].Width;
                } else {
                    pRawData->AuxBuffer.Window.Width = VinInfo.CapWindow[0U].Width/4U; // 4x DownScale
                }
                pRawData->AuxBuffer.Window.Height = VinInfo.CapWindow[0U].Height;

                if (VinInfo.VinCtrl.RawSeqWp != 0xFFU) {
                    PtsOverflowHdlr(VinInfo.VinCtrl.RawCapSeq[VinInfo.VinCtrl.RawSeqWp],
                                    pRawCapBuf->CapSeqU32,
                                    &VinInfo.VinCtrl.RawCapSeqBase,
                                    &RawCapSeq64);
#ifndef USE_TIME_MANAGER
                    PtsOverflowHdlr(VinInfo.VinCtrl.RawCapPts[VinInfo.VinCtrl.RawSeqWp],
                                    pRawCapBuf->CapPtsU32,
                                    &VinInfo.VinCtrl.RawCapPtsBase,
                                    &RawCapPts64);
#else
                    /* RawCapPts */
                    InTimeU64 = pRawCapBuf->CapPtsU32;
                    (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
                    RawCapPts64 = OutTimeU64;
                    VinInfo.VinCtrl.RawCapPtsBase = (OutTimeU64 & 0xFFFFFFFF00000000ULL);
#endif
                } else {
                    /* first raw, direct use it */
                    RawCapSeq64 = pRawCapBuf->CapSeqU32;
#ifndef USE_TIME_MANAGER
                    RawCapPts64 = pRawCapBuf->CapPtsU32;
#else
                    /* RawCapPts */
                    InTimeU64 = pRawCapBuf->CapPtsU32;
                    (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
                    RawCapPts64 = OutTimeU64;
                    VinInfo.VinCtrl.RawCapPtsBase = (OutTimeU64 & 0xFFFFFFFF00000000ULL);
#endif
                }

                /* Using CapSeqNo to determine wp index */
                VinInfo.VinCtrl.RawSeqWp = (UINT8)(pRawCapBuf->CapSeqU32 % MAX_RAW_SEQ_Q);
                VinInfo.VinCtrl.RawCapSeq[VinInfo.VinCtrl.RawSeqWp] = pRawCapBuf->CapSeqU32;
                VinInfo.VinCtrl.RawCapPts[VinInfo.VinCtrl.RawSeqWp] = pRawCapBuf->CapPtsU32;
                pRawData->CapPts = RawCapPts64;
                VinInfo.VinCtrl.RawCapPts64[VinInfo.VinCtrl.RawSeqWp] = RawCapPts64;
                pRawData->CapSequence = RawCapSeq64;
                VinInfo.VinCtrl.RawCapSeq64[VinInfo.VinCtrl.RawSeqWp] = RawCapSeq64;
            } else {
                /* Pts/Seq shall already update in VinStatusMessage, just use it */
                RawSeqId = (UINT8)(pRawCapBuf->CapSeqU32 % MAX_RAW_SEQ_Q);
                pRawData->CapPts = VinInfo.VinCtrl.RawCapPts64[RawSeqId];
                pRawData->CapSequence = VinInfo.VinCtrl.RawCapSeq64[RawSeqId];
            }
            HL_SetVinInfo(HL_MTX_OPT_SET, PhyVinId, &VinInfo);
        } else {
            //VirtualVin use internally, doesn't need to report error code
            Rval = DSP_ERR_0001;
        }
    }

    return Rval;
}

static UINT32 VideoDataPathPreProcFunc(const void *pEventData)
{
    UINT8 IsVirtVin = 0U;
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    CTX_VIN_INFO_s VinInfo = {0};
    CTX_VIN_INFO_s TmpVinInfo = {0};
    const AMBA_DSP_VIDEO_PATH_INFO_s *pVidPathInfo;
    UINT16 i;

    dsp_osal_typecast(&pVidPathInfo, &pEventData);

    if (pVidPathInfo->Status == VID_PATH_STATUS_VIN_TIMEOUT) {
        UINT16 VinId = (UINT16)pVidPathInfo->Data;

        Rval = HL_IsVirtualVinIdx(VinId, &IsVirtVin);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);

        if (Rval == DSP_ERR_NONE) {
            if (IsVirtVin == 0U) {
                HL_GetVinInfo(HL_MTX_OPT_GET, VinId, &VinInfo);
                /* Dispatch to virtual-vins */
                if (VinInfo.TimeDivisionNum[0] > 1U) {
                    for (i = 1U; i < AMBA_DSP_MAX_VIN_TD_NUM; i++) {
                        if (VinInfo.TimeDivisionVinId[0U][i] != VIN_IDX_INVALID) {
                            VinId = VinInfo.TimeDivisionVinId[0U][i] + AMBA_DSP_MAX_VIN_NUM;
                            HL_GetVinInfo(HL_MTX_OPT_GET, VinId, &TmpVinInfo);
                            if (TmpVinInfo.VinCtrl.VinState != DSP_VIN_STATUS_TIMEOUT) {
                                TmpVinInfo.VinCtrl.VinState = DSP_VIN_STATUS_TIMEOUT;
                            }
                            HL_SetVinInfo(HL_MTX_OPT_SET, VinId, &TmpVinInfo);
                        }
                    }
                }
                VinInfo.VinCtrl.VinState = DSP_VIN_STATUS_TIMEOUT;
                HL_SetVinInfo(HL_MTX_OPT_SET, VinId, &VinInfo);
            }
        }
    }

    if ((Rval != DSP_ERR_NONE) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("Video Data Path Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 LiveviewCfaReadyPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    AMBA_IK_CFA_3A_DATA_s *pCfaData;
    DSP_STL_PROC_JOB_ID_s StlProcJobId = {0};
    CTX_BATCHQ_INFO_s BatchQInfo = {0};
    CTX_STILL_INFO_s StlInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT8 RawInVprocIdCheck = 0U, YuvInVprocIdCheck = 0U;
    UINT8 RawInVprocStatusCheck = 0U, YuvInVprocStatusCheck = 0U;
    UINT8 IsAaaMuxSelEnable;
    UINT8 AaaMuxSelSrc;
    UINT32 VprocSeqNo = 0U;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;

    dsp_osal_typecast(&pCfaData, &pEventData);
    HL_GetDspBatchQInfo(pCfaData->Header.reserved1, &BatchQInfo);

    HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);

    Rval = dsp_osal_memcpy(&StlProcJobId, &BatchQInfo.StlProcId, sizeof(UINT32));
    DSP_FillErrline(Rval, &ErrLine, __LINE__);
    /* Release stl raw2raw batchId */
    HL_UpdateCtxViewZoneBatchQRPtr(pCfaData->Header.reserved1);
    pCfaData->Header.reserved1 = 0U;

    if (Rval == DSP_ERR_NONE) {
        if ((StlProcJobId.DataFmt == 2U/*GenAAA*/) &&
            (pCfaData->Header.ChanIndex == StlProcJobId.VprocId)) {

            Rval = HL_PreProcCheckStlStatus((UINT16)pCfaData->Header.ChanIndex,
                                           (UINT8)0U/*DataFmt*/,
                                           &StlInfo,
                                           &RawInVprocIdCheck,
                                           &YuvInVprocIdCheck,
                                           &RawInVprocStatusCheck,
                                           &YuvInVprocStatusCheck);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);

            /* Redirect to stl cfa aaa if needed */
            if (Rval == DSP_ERR_NONE) {
                if ((RawInVprocIdCheck == 1U) && (RawInVprocStatusCheck == 1U)) {
                    AMBA_IK_CFA_3A_DATA_s *pStlCfaData = NULL;

                    Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_STAT_CFA, &EventBufAddr, &EventBufIdx, LL_EVENT_STL_CFA_AAA_RDY);
                    DSP_FillErrline(Rval, &ErrLine, __LINE__);
                    if (Rval == DSP_ERR_NONE) {
                        dsp_osal_typecast(&pStlCfaData, &EventBufAddr);
                        Rval = dsp_osal_memcpy(pStlCfaData, pCfaData, sizeof(AMBA_IK_CFA_3A_DATA_s));
                        DSP_FillErrline(Rval, &ErrLine, __LINE__);

                        if (Rval == DSP_ERR_NONE) {
                            HL_GetStlInfo(HL_MTX_OPT_GET, &StlInfo);
                            StlInfo.RawInVprocStatus = STL_VPROC_STATUS_COMPLETE;
                            HL_SetStlInfo(HL_MTX_OPT_SET, &StlInfo);

                            /* Still Yuv Event */
                            Rval = DSP_GiveEvent(LL_EVENT_STL_CFA_AAA_RDY, pStlCfaData, EventBufIdx);
                            DSP_FillErrline(Rval, &ErrLine, __LINE__);
                        } else {
                            /* If you request EventInfoBuf but don't call GiveEvent
                             * Needs to release EventInfoBuf */
                            Rval = DSP_ReleaseEventInfoBuf(EventBufIdx);
                            DSP_FillErrline(Rval, &ErrLine, __LINE__);
                        }
                    }
                    //discard this event, doesn't need to report error code
                    Rval = DSP_ERR_0004;
                } else {
                    AmbaLL_LogUInt5("CfaAaa [%d %d %d]", pCfaData->Header.ChanIndex, StlProcJobId.VprocId, StlProcJobId.DataFmt, 0U, 0U);
                }
            }
        } else {
            HL_GetViewZoneInfoPtr((UINT16)pCfaData->Header.ChanIndex, &ViewZoneInfo);
            IsAaaMuxSelEnable = (UINT8)DSP_GetU16Bit(ViewZoneInfo->AaaMuxSel, VZ_AAA_MUXSEL_ENB_BIT_IDX, VZ_AAA_MUXSEL_ENB_LEN);
            AaaMuxSelSrc = (UINT8)DSP_GetU16Bit(ViewZoneInfo->AaaMuxSel, VZ_AAA_MUXSEL_SRC_BIT_IDX, VZ_AAA_MUXSEL_SRC_LEN);
            if (IsAaaMuxSelEnable == (UINT8)1U) {
                VprocSeqNo = pCfaData->Header.reserved3[0U];
                VprocSeqNo |= (UINT32)pCfaData->Header.reserved3[1U] << 16U;

                if ((VprocSeqNo % 2U) == 0U) {
                    pCfaData->Header.AaaCfaMuxSel = AaaMuxSelSrc;
                } else {
                    pCfaData->Header.AaaCfaMuxSel = (UINT8)1U; // TBD
                }
            } else {
                pCfaData->Header.AaaCfaMuxSel = (UINT8)1U;
            }
        }
    }

    pCfaData->Header.reserved3[0U] = 0U;
    pCfaData->Header.reserved3[1U] = 0U;

    if ((Rval != DSP_ERR_NONE) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("Liveview Cfa Ready Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 LiveviewPGReadyPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE;

//FIXME, Misra
    (void)pEventData;

    return Rval;
}

static UINT32 LiveviewHistReadyPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE;

//FIXME, Misra
    (void)pEventData;

    return Rval;
}

static inline void HL_StlYuvValidateCapInst(const CTX_DATACAP_INFO_s *pDataCapInfo,
                                            const AMBA_DSP_YUV_INFO_s *pYuvInfo,
                                            UINT8 *pIsValidDataCapIdx,
                                            UINT8 *pIsPurePassThrough)
{
    UINT16 YuvStrmId = 0U;
    CTX_STREAM_INFO_s StrmInfo = {0};
    CTX_VPROC_INFO_s VprocInfo = {0};
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;

    if (pDataCapInfo->Cfg.CapDataType == DSP_DATACAP_TYPE_YUV) {
        YuvStrmId = pDataCapInfo->Cfg.Index;
        *pIsValidDataCapIdx = 1U;
    } else if (pDataCapInfo->Cfg.CapDataType == DSP_DATACAP_TYPE_SYNCED_YUV) {
        HL_GetStrmInfo(HL_MTX_OPT_ALL, pDataCapInfo->Cfg.Index, &StrmInfo);
        YuvStrmId = StrmInfo.SourceYuvStrmId;
        *pIsValidDataCapIdx = 1U;
    } else if (pDataCapInfo->Cfg.CapDataType == DSP_DATACAP_TYPE_VIDEO_YUV) {
        HL_GetStrmInfo(HL_MTX_OPT_ALL, pDataCapInfo->Cfg.Index, &StrmInfo);
        YuvStrmId = StrmInfo.SourceYuvStrmId;
        *pIsValidDataCapIdx = 1U;
    } else {
        *pIsValidDataCapIdx = 0U;
    }

    *pIsPurePassThrough = 0U;
    if (*pIsValidDataCapIdx == 1U) {
        HL_GetVprocInfo(HL_MTX_OPT_ALL, (UINT16)pYuvInfo->ChannelId, &VprocInfo);
        if (1U == DSP_GetBit(VprocInfo.PinUsageOnStrm[pYuvInfo->YuvType][YuvStrmId], pYuvInfo->ChannelId, 1U)) {
            UINT16 LastViewZoneId;

            HL_GetYuvStrmInfoLock(YuvStrmId, &YuvStrmInfo);

            /* PurePassThough: event given at LvYuvRdy
             * NonPurePassThrough: event given at EfctYuvRdy*/
            if (YuvStrmInfo->Layout.NumChan == 1U) {
                *pIsPurePassThrough = 1U;
            } else {
                if ((YuvStrmInfo->Layout.EffectBufDesc.CpyBufId == DSP_EFCT_INVALID_IDX) &&
                    (YuvStrmInfo->Layout.EffectBufDesc.Y2YBufId == DSP_EFCT_INVALID_IDX)) {
                    *pIsPurePassThrough = 1U;
                } else {
                    *pIsPurePassThrough = 0U;
                }
            }

            if ((YuvStrmInfo->Layout.NumChan - 1U) < AMBA_DSP_MAX_YUVSTRM_VIEW_NUM) {
                LastViewZoneId = YuvStrmInfo->Layout.ChanCfg[YuvStrmInfo->Layout.NumChan - 1U].ViewZoneId;
            } else {
                LastViewZoneId = YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId;
            }

            if (pYuvInfo->ChannelId == (UINT32)LastViewZoneId) {
                *pIsValidDataCapIdx = 1U;
            } else {
                *pIsValidDataCapIdx = 0U;
            }

            HL_GetYuvStrmInfoUnLock(YuvStrmId);
        } else {
            *pIsValidDataCapIdx = 0U;
        }
    }
}

static inline UINT32 HL_LvYuvRdyTimingCheck(const AMBA_DSP_YUV_INFO_s *pYuvInfo)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    UINT8 MctfEnable = 0U, MctsEnable = 0U, MctsOutEnable = 0U, MctfCmpr = 0U;

    /* Mctf relative : for DSP_VPROC_PIN_MAIN */
    /* 1. Bypass SrcBufID=Sec3 event when Mctf is enable */
    /* 2. Bypass SrcBufID=Mctf event when Mctf is disable */
    Rval = HL_GetViewZoneMctfEnable((UINT16)pYuvInfo->ChannelId, &MctfEnable, &MctsEnable, &MctsOutEnable, &MctfCmpr);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);
    if ((MctfEnable == 1U) &&
        (pYuvInfo->SrcBufID == DSP_SEC3_CFG_INDEX)) {
        //discard this event
        Rval = DSP_ERR_0004;
    } else if ((MctfEnable == 0U) &&
               (pYuvInfo->SrcBufID == DSP_SEC18_CFG_INDEX)) {
        //discard this event
        Rval = DSP_ERR_0004;
    } else {
        Rval = DSP_ERR_NONE;
    }

    if ((Rval != DSP_ERR_NONE) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("Liveview yuv ready timing check Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_LvYuvRdyStlEvntProc(AMBA_DSP_YUV_INFO_s *pYuvInfo,
                                            const CTX_STILL_INFO_s *pStlInfo,
                                            const UINT8 *pRawInVprocIdCheck,
                                            const UINT8 *pRawInVprocStatusCheck)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    UINT32 IsBaseAddrYInRange, IsBaseAddrUVInRange;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_BATCHQ_INFO_s BatchQInfo = {0};
    DSP_STL_PROC_JOB_ID_s StlProcJobId = {0};
    CTX_STILL_INFO_s TmpStlInfo;
    UINT8 YuvInValidRange;
    AMBA_DSP_YUV_IMG_BUF_s *pYuvImg = NULL;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;

    pYuvInfo->YuvData.VinVirtChId = pStlInfo->YuvInVirtVinId;
    HL_GetDspBatchQInfo(pYuvInfo->BatchCmdId, &BatchQInfo);
    Rval = dsp_osal_memcpy(&StlProcJobId, &BatchQInfo.StlProcId, sizeof(UINT32));
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    HL_GetResourcePtr(&Resource);
    IsBaseAddrYInRange = IsInsideRange(pYuvInfo->YuvData.Buffer.BaseAddrY,
                                       Resource->DspSysCfg.WorkAreaAddr,
                                       Resource->DspSysCfg.WorkAreaSize);
    IsBaseAddrUVInRange = IsInsideRange(pYuvInfo->YuvData.Buffer.BaseAddrUV,
                                        Resource->DspSysCfg.WorkAreaAddr,
                                        Resource->DspSysCfg.WorkAreaSize);

    /* Simple Buffer range check */
    if (StlProcJobId.ExtMem == (UINT8)1U) {
        if ((0U == IsBaseAddrYInRange) && (0U == IsBaseAddrUVInRange)) {
            YuvInValidRange = 1U;
        } else {
            YuvInValidRange = 0U;
        }
    } else {
        if ((1U == IsBaseAddrYInRange) && (1U == IsBaseAddrUVInRange)) {
            YuvInValidRange = 1U;
        } else {
            YuvInValidRange = 0U;
        }
    }

    if (Rval == DSP_ERR_NONE) {
        if ((YuvInValidRange == 1U) && (pYuvInfo->YuvData.ViewZoneId != VIEWZONE_ID_INDICATOR_MAIN)) {
            if (1U == DSP_GetU16Bit((UINT16)StlProcJobId.OutputPin, (UINT32)pYuvInfo->YuvType, 1U)) {
                UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;

                Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_STL_YUV_DATA_RDY);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);

                if (Rval == DSP_ERR_NONE) {
                    dsp_osal_typecast(&pYuvImg, &EventBufAddr);
                    Rval = dsp_osal_memcpy(pYuvImg, &pYuvInfo->YuvData.Buffer, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                    DSP_FillErrline(Rval, &ErrLine, __LINE__);

                    if (Rval == DSP_ERR_NONE) {
                        pYuvImg->DataFmt = (UINT8)StlProcJobId.DataFmt;
                        Rval = DSP_GiveEvent(LL_EVENT_STL_YUV_DATA_RDY, pYuvImg, EventBufIdx);
                        DSP_FillErrline(Rval, &ErrLine, __LINE__);
                    } else {
                        /* If you request EventInfoBuf but don't call GiveEvent
                         * Needs to release EventInfoBuf */
                        Rval = DSP_ReleaseEventInfoBuf(EventBufIdx);
                        DSP_FillErrline(Rval, &ErrLine, __LINE__);
                    }

                    HL_GetStlInfo(HL_MTX_OPT_GET, &TmpStlInfo);
                    if ((*pRawInVprocIdCheck == 1U) && (*pRawInVprocStatusCheck == 1U)) {
                        TmpStlInfo.RawInVprocStatus = STL_VPROC_STATUS_COMPLETE;
#ifndef SUPPORT_VPROC_RT_RESTART
                    } else if (pYuvInfo->YuvData.Buffer.DataFmt == AMBA_DSP_YUV420) {
                        TmpStlInfo.YuvInVprocStatus = STL_VPROC_STATUS_COMPLETE;
                    } else {
                        TmpStlInfo.Yuv422InVprocStatus = STL_VPROC_STATUS_COMPLETE;
                    }
#else
                    } else {
                        TmpStlInfo.YuvInVprocStatus = STL_VPROC_STATUS_COMPLETE;
                    }
#endif
                    HL_SetStlInfo(HL_MTX_OPT_SET, &TmpStlInfo);
                    // Update CtxViewZone BatchQRptr
                    HL_UpdateCtxViewZoneBatchQRPtr(pYuvInfo->BatchCmdId);
                    if (pIsSimilarEnabled != NULL) {
                        if (1U == IsSimilarEnabled()) {
                            SIM_UpdateCtxViewZoneBatchQRPtr(pYuvInfo->BatchCmdId);
                        }
                    }
                }
            } else {
                //discard this event
                Rval = DSP_ERR_0004;
            }
        } else {
            //discard this event
            Rval = DSP_ERR_0004;
        }
    }

    if ((Rval != DSP_ERR_NONE) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("Liveview Ready still event Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline void HL_LvYuvRdyLvEvntProcVz(AMBA_DSP_YUV_INFO_s *pYuvInfo,
                                           const CTX_VPROC_INFO_s *pVprocInfo,
                                           UINT16 *pFindViewZone)
{
    CTX_YUV_STRM_INFO_s *pYuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    UINT16 i;

    // Update ViewZone id
    if ((pYuvInfo->YuvType == DSP_VPROC_YUV_MAIN) && (pYuvInfo->YuvData.ViewZoneId == VIEWZONE_ID_INDICATOR_MAIN)) {
        *pFindViewZone = 1U;
        pYuvInfo->YuvData.ViewZoneId = pYuvInfo->ChannelId;
    } else {
        UINT8 IsEfctYuvStrm = 0U;

        for (i = 0; i < AMBA_DSP_MAX_YUVSTRM_NUM; i++) {
            if (1U == DSP_GetBit(pVprocInfo->PinUsageOnStrm[pYuvInfo->YuvType][i], pYuvInfo->ChannelId, 1U)) {
                HL_GetYuvStrmInfoLock(i, &pYuvStrmInfo);
                IsEfctYuvStrm = HL_GetYuvStrmIsEffectOut(pYuvStrmInfo->MaxChanNum,
                                                         pYuvStrmInfo->Layout.ChanCfg[0U].Window.OffsetX,
                                                         pYuvStrmInfo->Layout.ChanCfg[0U].Window.OffsetY,
                                                         pYuvStrmInfo->Layout.ChanCfg[0U].RotateFlip);
                /*
                 * PreProc reports non-effect YUV-streams from msg_vproc_status,
                 * and reports effect YUV-stream from msg_vproc_ext_status
                 */
                if ((pYuvStrmInfo->MaxChanNum == 1U) && (IsEfctYuvStrm == 0U)) {
                    pYuvInfo->YuvData.ViewZoneId = 0x80000000U|i;
                    *pFindViewZone = 1U;
                } else if ((IsEfctYuvStrm == 1U) && (pYuvInfo->YuvData.ViewZoneId == VIEWZONE_ID_INDICATOR_EFFECT)) {
                    pYuvInfo->YuvData.ViewZoneId = 0x80000000U|i;
                    *pFindViewZone = 1U;
                    /* Fill YuvStrmWin */
                    pYuvInfo->YuvData.Buffer.Window.Width = pYuvStrmInfo->Width;
                    pYuvInfo->YuvData.Buffer.Window.Height = pYuvStrmInfo->Height;
#ifdef PPSTRM_SWITCH_BETWEEN_PASSTHROUGH_AND_BLENDING
                } else if ((pYuvStrmInfo->MaxChanNum > 1U) &&
                           (pYuvInfo->ChannelId == pYuvStrmInfo->Layout.ChanCfg[pYuvStrmInfo->MaxChanNum-1U].ViewZoneId)) {
                    pYuvInfo->YuvData.ViewZoneId = 0x80000000U|i;
                    *pFindViewZone = 1U;
#endif
                } else {
                    // TBD
                }
                HL_GetYuvStrmInfoUnLock(i);
            } else {
                // TBD
            }
            if (*pFindViewZone == 1U) {
                break;
            }
        }
    }
}

static inline UINT32 HL_LvYuvRdyLvEvntProc(AMBA_DSP_YUV_INFO_s *pYuvInfo,
                                           UINT64 *pRawCapPts64,
                                           UINT64 *pRawCapSeq64,
                                           UINT16 *pFindViewZone)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    UINT32 RawCapPts32 = 0U;
    CTX_VPROC_INFO_s VprocInfo = {0};
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 VinId = 0U;
    CTX_VIN_INFO_s VinInfo = {0};
    UINT64 YuvDonePts64 = 0ULL;
#ifdef USE_TIME_MANAGER
    UINT64 InTimeU64, OutTimeU64 = 0ULL;
#endif

    // Ref to HL_FillVideoProcSetup setting
    HL_GetVprocInfo(HL_MTX_OPT_ALL, (UINT16)pYuvInfo->ChannelId, &VprocInfo);
    pYuvInfo->YuvData.Buffer.DataFmt = AMBA_DSP_YUV420;
    // Replace U64 Pts
    HL_GetViewZoneInfoLock((UINT16)pYuvInfo->ChannelId, &ViewZoneInfo);
    if (pYuvInfo->YuvData.ViewZoneId == VIEWZONE_ID_INDICATOR_MAIN) {
        /* Get CapPts from VinInfo */
        UINT8 CapSeqQIdx;

        Rval = HL_GetViewZoneVinId((UINT16)pYuvInfo->ChannelId, &VinId);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        if (VinId < DSP_VIN_MAX_NUM) {
            HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);

            CapSeqQIdx = (UINT8)(pYuvInfo->CapSeqU32 % MAX_RAW_SEQ_Q);
            if ((VinInfo.VinCtrl.RawCapSeq[CapSeqQIdx] == pYuvInfo->CapSeqU32) &&
                (VinInfo.VinCtrl.RawCapPts[CapSeqQIdx] != 0U)) {
                RawCapPts32 = VinInfo.VinCtrl.RawCapPts[CapSeqQIdx];
            } else {
                RawCapPts32 = pYuvInfo->CapPtsU32;
            }

            /* Update PTS and sequence number when this event is for main yuv */
            if (ViewZoneInfo->YuvInput.RawSeqWp != 0xFFU) {
                PtsOverflowHdlr(ViewZoneInfo->YuvInput.RawCapSeq[ViewZoneInfo->YuvInput.RawSeqWp], pYuvInfo->CapSeqU32, &ViewZoneInfo->YuvInput.RawCapSeqBase, pRawCapSeq64);
#ifndef USE_TIME_MANAGER
                PtsOverflowHdlr(ViewZoneInfo->YuvInput.RawCapPts[ViewZoneInfo->YuvInput.RawSeqWp], RawCapPts32, &ViewZoneInfo->YuvInput.RawCapPtsBase, pRawCapPts64);
                PtsOverflowHdlr(ViewZoneInfo->YuvInput.DonePts[ViewZoneInfo->YuvInput.RawSeqWp], pYuvInfo->YuvPtsU32, &ViewZoneInfo->YuvInput.DonePtsBase, &YuvDonePts64);
#else
                /* RawCapPts */
                InTimeU64 = RawCapPts32;
                (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
                *pRawCapPts64 = OutTimeU64;
                ViewZoneInfo->YuvInput.RawCapPtsBase = (OutTimeU64 & 0xFFFFFFFF00000000ULL);

                /* YuvDonePts */
                InTimeU64 = pYuvInfo->YuvPtsU32;
                (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
                YuvDonePts64 = OutTimeU64;
                ViewZoneInfo->YuvInput.DonePtsBase = (OutTimeU64 & 0xFFFFFFFF00000000ULL);
#endif
            } else {
                /* first raw, directly use it */
                *pRawCapSeq64 = pYuvInfo->CapSeqU32;
#ifndef USE_TIME_MANAGER
                *pRawCapPts64 = RawCapPts32;
                YuvDonePts64 = pYuvInfo->YuvPtsU32;
#else
                /* RawCapPts */
                InTimeU64 = RawCapPts32;
                (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
                *pRawCapPts64 = OutTimeU64;
                ViewZoneInfo->YuvInput.RawCapPtsBase = (OutTimeU64 & 0xFFFFFFFF00000000ULL);

                /* YuvDonePts */
                InTimeU64 = pYuvInfo->YuvPtsU32;
                (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
                YuvDonePts64 = OutTimeU64;
                ViewZoneInfo->YuvInput.DonePtsBase = (OutTimeU64 & 0xFFFFFFFF00000000ULL);
#endif
            }
            /* Using CapSeqNo to determine wp index */
            ViewZoneInfo->YuvInput.RawSeqWp = (UINT8)(pYuvInfo->CapSeqU32 % MAX_RAW_SEQ_Q);
            ViewZoneInfo->YuvInput.RawCapSeq[ViewZoneInfo->YuvInput.RawSeqWp] = pYuvInfo->CapSeqU32;
            ViewZoneInfo->YuvInput.RawCapPts[ViewZoneInfo->YuvInput.RawSeqWp] = RawCapPts32;
            ViewZoneInfo->YuvInput.DonePts[ViewZoneInfo->YuvInput.RawSeqWp] = pYuvInfo->YuvPtsU32;
            ViewZoneInfo->YuvInput.RawCapPts64[ViewZoneInfo->YuvInput.RawSeqWp] = *pRawCapPts64;
            ViewZoneInfo->YuvInput.RawCapSeq64[ViewZoneInfo->YuvInput.RawSeqWp] = *pRawCapSeq64;
            ViewZoneInfo->YuvInput.DonePts64[ViewZoneInfo->YuvInput.RawSeqWp] = YuvDonePts64;
        }
    } else {
        /* For (1) YUV-streams source from main and (2) YUV-info of effect channels,
         * PreProc fills PTS/Seq of Main yuv into their YUV-info
         * */
        *pRawCapSeq64 = ViewZoneInfo->YuvInput.RawCapSeq64[ViewZoneInfo->YuvInput.RawSeqWp];
        *pRawCapPts64 = ViewZoneInfo->YuvInput.RawCapPts64[ViewZoneInfo->YuvInput.RawSeqWp];
        YuvDonePts64 = ViewZoneInfo->YuvInput.DonePts64[ViewZoneInfo->YuvInput.RawSeqWp];
    }
    pYuvInfo->YuvData.CapPts= *pRawCapPts64;
    pYuvInfo->YuvData.CapSequence = *pRawCapSeq64;
    pYuvInfo->YuvData.YuvPts = YuvDonePts64;
    HL_GetViewZoneInfoUnLock((UINT16)pYuvInfo->ChannelId);

    // Update ViewZone id
    HL_LvYuvRdyLvEvntProcVz(pYuvInfo, &VprocInfo, pFindViewZone);

    if ((Rval != DSP_ERR_NONE) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("Liveview yuv ready event Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_LvYuvRdyStlRawEvntProc(UINT16 CapInst,
                                               const CTX_DATACAP_INFO_s *pDataCapInfo,
                                               const AMBA_DSP_YUV_INFO_s *pYuvInfo,
                                               const UINT64 *pRawCapPts64,
                                               const UINT64 *pRawCapSeq64)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    AMBA_DSP_STL_RAW_DATA_INFO_s *pStlRawData = NULL;
    DSP_STL_RAW_CAP_BUF_s *pStlRawCapBuf = NULL;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;
    UINT8 InvokeEvt = 0U;

    Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_STL_RAW_RDY);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);
    if (Rval == DSP_ERR_NONE) {
        dsp_osal_typecast(&pStlRawCapBuf, &EventBufAddr);
        Rval = dsp_osal_memset(pStlRawCapBuf, 0, sizeof(DSP_STL_RAW_CAP_BUF_s));
        DSP_FillErrline(Rval, &ErrLine, __LINE__);

        if (Rval == DSP_ERR_NONE) {
            pStlRawData = &pStlRawCapBuf->StlRawData;
            pStlRawData->CapInstance = CapInst;
            pStlRawData->BufFormat = RAW_FORMAT_MEM_YUV420;
            pStlRawData->Buf.BaseAddr = pYuvInfo->YuvData.Buffer.BaseAddrY;
            pStlRawData->Buf.Pitch = pYuvInfo->YuvData.Buffer.Pitch;

            Rval = dsp_osal_memcpy(&pStlRawData->Buf.Window, &pYuvInfo->YuvData.Buffer.Window, sizeof(AMBA_DSP_WINDOW_s));
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            pStlRawData->AuxBufFormat = RAW_FORMAT_MEM_YUV420;
            pStlRawData->AuxBuf.BaseAddr = pYuvInfo->YuvData.Buffer.BaseAddrUV;
            pStlRawData->AuxBuf.Pitch = pYuvInfo->YuvData.Buffer.Pitch;
            pStlRawCapBuf->CapSrcType = CAP_SRC_TYPE_VPROC;

            Rval = dsp_osal_memcpy(&pStlRawData->AuxBuf.Window, &pYuvInfo->YuvData.Buffer.Window, sizeof(AMBA_DSP_WINDOW_s));
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            if (pStlRawData->AuxBufFormat == RAW_FORMAT_MEM_YUV420) {
                pStlRawData->AuxBuf.Window.Height = pStlRawData->AuxBuf.Window.Height >> 1;
            }

            // ME data
            if (pDataCapInfo->Cfg.CapDataType == DSP_DATACAP_TYPE_VIDEO_YUV) {
                Rval = dsp_osal_memcpy(&pStlRawData->Aux2Buf.Window, &pYuvInfo->Me1Buf.Window, sizeof(AMBA_DSP_WINDOW_s));
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
                pStlRawData->Aux2Buf.BaseAddr = pYuvInfo->Me1Buf.BaseAddr;
                pStlRawData->Aux2Buf.Pitch = pYuvInfo->Me1Buf.Pitch;
            }

            // Replace U64 RawSeq/RawPts
            if (*pRawCapSeq64 > 0ULL) {
                pStlRawData->CapSequence = *pRawCapSeq64;
                pStlRawData->CapPts = *pRawCapPts64;
            } else {
                UINT8 CapSeqQIdx;

                HL_GetViewZoneInfoPtr((UINT16)pYuvInfo->ChannelId, &ViewZoneInfo);
                CapSeqQIdx = (UINT8)(pYuvInfo->CapSeqU32 % MAX_RAW_SEQ_Q);
                if (ViewZoneInfo->YuvInput.RawCapSeq[CapSeqQIdx] == pYuvInfo->CapSeqU32) {
                    pStlRawData->CapSequence = ViewZoneInfo->YuvInput.RawCapSeq64[CapSeqQIdx];
                    pStlRawData->CapPts = ViewZoneInfo->YuvInput.RawCapPts64[CapSeqQIdx];
                } else {
                    pStlRawData->CapSequence = (UINT64)pYuvInfo->CapSeqU32;
                    pStlRawData->CapPts = (UINT64)pYuvInfo->CapPtsU32;
                }
            }

            if (Rval == DSP_ERR_NONE) {
                Rval = DSP_GiveEvent(LL_EVENT_STL_RAW_RDY, pStlRawCapBuf, EventBufIdx);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);
                InvokeEvt = 1U;
            }
        }
        if (InvokeEvt == 0U) {
            /* If you request EventInfoBuf but don't call GiveEvent
             * Needs to release EventInfoBuf */
            Rval = DSP_ReleaseEventInfoBuf(EventBufIdx);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
    }

    if ((Rval != DSP_ERR_NONE) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("Liveview yuv ready still raw event Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 HL_LvYuvRdyPreProcEvent(UINT8 RawInVprocIdCheck, UINT8 YuvInVprocIdCheck, UINT8 RawInVprocStatusCheck, UINT8 YuvInVprocStatusCheck,
                                      AMBA_DSP_YUV_INFO_s *pYuvInfo, const CTX_STILL_INFO_s *pStlInfo, UINT64 *pRawCapPts64, UINT64 *pRawCapSeq64)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT16 FindViewZone = 0U;

    if (((RawInVprocIdCheck == 1U) && (RawInVprocStatusCheck == 1U)) ||
        ((YuvInVprocIdCheck == 1U) && (YuvInVprocStatusCheck == 1U))) {
        /* Still Yuv Event */
        Rval = HL_LvYuvRdyStlEvntProc(pYuvInfo, pStlInfo, &RawInVprocIdCheck, &RawInVprocStatusCheck);
    } else if ((RawInVprocIdCheck == 1U) || (YuvInVprocIdCheck == 1U)) {
        // StillStatus check failed, discard this event
        Rval = DSP_ERR_0004;

        // Update CtxViewZone BatchQRptr
        if (pYuvInfo->YuvType == DSP_VPROC_PIN_MAIN) {
            UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
            HL_UpdateCtxViewZoneBatchQRPtr(pYuvInfo->BatchCmdId);
            if (pIsSimilarEnabled != NULL) {
                if (1U == IsSimilarEnabled()) {
                    SIM_UpdateCtxViewZoneBatchQRPtr(pYuvInfo->BatchCmdId);
                }
            }
        }
    } else {
        /* Liveview Yuv Event */
        Rval = HL_LvYuvRdyLvEvntProc(pYuvInfo, pRawCapPts64, pRawCapSeq64, &FindViewZone);
        if (FindViewZone == 0U) {
            //no YUV refer to this ViewZone, not invoke main event handler to upper layer
            Rval = DSP_ERR_0004;
        }

        // Update CtxViewZone BatchQRptr
        if (pYuvInfo->YuvType == DSP_VPROC_PIN_MAIN) {
            UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
            HL_UpdateCtxViewZoneBatchQRPtr(pYuvInfo->BatchCmdId);
            if (pIsSimilarEnabled != NULL) {
                if (1U == IsSimilarEnabled()) {
                    SIM_UpdateCtxViewZoneBatchQRPtr(pYuvInfo->BatchCmdId);
                }
            }
        }
    }

    return Rval;
}

static inline UINT32 HL_LvYuvRdyPreProcStlRaw(const AMBA_DSP_YUV_INFO_s *pYuvInfo,
                                              const UINT64 *pRawCapPts64,
                                              const UINT64 *pRawCapSeq64)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT16 i;
    UINT8 IsValidDataCapIdx = 0U, IsPurePassThrough = 0U;
    CTX_DATACAP_INFO_s DataCapInfo;

    for (i=0U; i<AMBA_DSP_MAX_DATACAP_NUM; i++) {
        HL_GetDataCapInfo(HL_MTX_OPT_ALL, i, &DataCapInfo);
        if ((DataCapInfo.Status == DATA_CAP_STATUS_RUNNING) ||
            (DataCapInfo.Status == DATA_CAP_STATUS_2_STOP)) {

            HL_StlYuvValidateCapInst(&DataCapInfo, pYuvInfo, &IsValidDataCapIdx, &IsPurePassThrough);

            if ((IsValidDataCapIdx == 1U) && (IsPurePassThrough == 1U)) {
                Rval = HL_LvYuvRdyStlRawEvntProc(i, &DataCapInfo, pYuvInfo, pRawCapPts64, pRawCapSeq64);
                break;
            }
        } else {
            continue;
        }
    }

    return Rval;
}

static UINT32 HL_LvYuvRdyPreProcDataCap(const AMBA_DSP_YUV_INFO_s *pYuvInfo, const UINT64 *pRawCapPts64, const UINT64 *pRawCapSeq64)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT16 i, IsDataCap;
    UINT8 IsExtMemMain, IsExtMemMctf, IsExtMemMcts;

    /* Check DataCapInfo for updating */
    IsDataCap = 0U;
    for (i=DSP_VPROC_PIN_PREVC; i<DSP_VPROC_PIN_NUM; i++) {
        if (pYuvInfo->YuvType == i) {
            // Avoid duplicate LL_EVENT_STL_RAW_RDY events when getting MainYuv
            if (1U == DSP_GetBit(pYuvInfo->YuvData.ViewZoneId, 31U, 1U)) {
                if (i == DSP_VPROC_PIN_MAIN) {
                    IsExtMemMain = (1U == DSP_GetBit(pYuvInfo->ExtBufMask, (UINT32)DSP_BIT_POS_EXT_MAIN, 1U))? (UINT8)1U: (UINT8)0U;
                    IsExtMemMctf = (1U == DSP_GetBit(pYuvInfo->ExtBufMask, (UINT32)DSP_BIT_POS_EXT_MCTF, 1U))? (UINT8)1U: (UINT8)0U;
                    IsExtMemMcts = (1U == DSP_GetBit(pYuvInfo->ExtBufMask, (UINT32)DSP_BIT_POS_EXT_MCTS, 1U))? (UINT8)1U: (UINT8)0U;

                    if ((IsExtMemMain > (UINT8)0U) || (IsExtMemMctf > (UINT8)0U) || (IsExtMemMcts > (UINT8)0U)) {
                        IsDataCap = 1U;
                    }
                } else {
                    if (1U == DSP_GetBit(pYuvInfo->ExtBufMask, (UINT32)DSP_VprocPinVprocMemTypeMap[i], 1U)) {
                        IsDataCap = 1U;
                    }
                }
                if (IsDataCap == 1U) {
                    break;
                }
            }
        }
    }

    if (IsDataCap == 1U) {
        Rval = HL_LvYuvRdyPreProcStlRaw(pYuvInfo, pRawCapPts64, pRawCapSeq64);
    }

    return Rval;
}

static UINT32 LiveviewYuvReadyPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    UINT64 RawCapPts64 = 0ULL, RawCapSeq64 = 0ULL;
    CTX_STILL_INFO_s StlInfo = {0};
    AMBA_DSP_YUV_INFO_s *pYuvInfo;
    UINT8 IsVirtVin = 0U;

    dsp_osal_typecast(&pYuvInfo, &pEventData);
    Rval = HL_LvYuvRdyTimingCheck(pYuvInfo);

    if (Rval == DSP_ERR_NONE) {
        UINT8 RawInVprocIdCheck = 0U, YuvInVprocIdCheck = 0U;
        UINT8 RawInVprocStatusCheck = 0U, YuvInVprocStatusCheck = 0U;

        HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);

        Rval = HL_IsVirtualVinIdx(pYuvInfo->YuvData.VinId, &IsVirtVin);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
        if ((IsVirtVin == 1U) && (Rval == DSP_ERR_NONE)) {
            UINT16 PhyVinId = pYuvInfo->YuvData.VinId;

            Rval = HL_GetTimeDivisionVirtVinInfo(pYuvInfo->YuvData.VinId, &PhyVinId);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            if (PhyVinId == DSP_VIRT_VIN_IDX_INVALID) {
                pYuvInfo->YuvData.IsVirtChan = IsVirtVin;
            } else {
                pYuvInfo->YuvData.VinId = PhyVinId;
            }
        }
        pYuvInfo->YuvData.VinVirtChId = 0U;

        if (Rval == DSP_ERR_NONE) {
            Rval = HL_PreProcCheckStlStatus((UINT16)pYuvInfo->ChannelId,
                                           pYuvInfo->YuvData.Buffer.DataFmt,
                                           &StlInfo,
                                           &RawInVprocIdCheck,
                                           &YuvInVprocIdCheck,
                                           &RawInVprocStatusCheck,
                                           &YuvInVprocStatusCheck);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }

        if (Rval == DSP_ERR_NONE) {
            Rval = HL_LvYuvRdyPreProcEvent(RawInVprocIdCheck, YuvInVprocIdCheck, RawInVprocStatusCheck, YuvInVprocStatusCheck,
                                           pYuvInfo, &StlInfo, &RawCapPts64, &RawCapSeq64);
        }
        if (Rval == DSP_ERR_NONE) {
            Rval = HL_LvYuvRdyPreProcDataCap(pYuvInfo, &RawCapPts64, &RawCapSeq64);
        }
    }

    if ((Rval != DSP_ERR_NONE) && (Rval != DSP_ERR_0004) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("Liveview yuv ready Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 LiveviewPymdReadyPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    UINT32 RawCapPts32 = 0U;
    UINT64 RawCapSeq64 = 0ULL, RawCapPts64 = 0ULL, PymdDonePts64 = 0ULL;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    AMBA_DSP_PYMD_DATA_RDY_s *PymdInfo;
    UINT16 IsPolySqrt2;
    UINT16 VinId = 0U;
    CTX_VIN_INFO_s VinInfo = {0};
#ifdef USE_TIME_MANAGER
    UINT64 InTimeU64, OutTimeU64 = 0ULL;
#endif

    dsp_osal_typecast(&PymdInfo, &pEventData);

    // Replace U64 Pts
    HL_GetViewZoneInfoLock((UINT16)PymdInfo->ViewZoneId, &ViewZoneInfo);

    IsPolySqrt2 = (UINT16)DSP_GetU16Bit(ViewZoneInfo->Pyramid.IsPolySqrt2, DSP_PYMD_PLOY_IDX, DSP_PYMD_PLOY_LEN);
    PymdInfo->Config.IsPolySqrt2 = IsPolySqrt2;

    /* Get CapPts from VinInfo */
    Rval = HL_GetViewZoneVinId(PymdInfo->ViewZoneId, &VinId);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);
    RawCapPts32 = (UINT32)PymdInfo->CapPts;
    if (VinId < DSP_VIN_MAX_NUM) {
        UINT8 CapSeqQIdx;

        HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);
        CapSeqQIdx = (UINT8)(PymdInfo->CapSequence % MAX_RAW_SEQ_Q);
        if (VinInfo.VinCtrl.RawCapSeq[CapSeqQIdx] == PymdInfo->CapSequence) {
            RawCapPts32 = VinInfo.VinCtrl.RawCapPts[CapSeqQIdx];
        }
    }

    if (ViewZoneInfo->PymdInput.RawSeqWp != 0xFFU) {
        PtsOverflowHdlr(ViewZoneInfo->PymdInput.RawCapSeq[ViewZoneInfo->PymdInput.RawSeqWp], (UINT32)PymdInfo->CapSequence, &ViewZoneInfo->PymdInput.RawCapSeqBase, &RawCapSeq64);
#ifndef USE_TIME_MANAGER
        PtsOverflowHdlr(ViewZoneInfo->PymdInput.RawCapPts[ViewZoneInfo->PymdInput.RawSeqWp], (UINT32)PymdInfo->CapPts, &ViewZoneInfo->PymdInput.RawCapPtsBase, &RawCapPts64);
        PtsOverflowHdlr(ViewZoneInfo->PymdInput.DonePts[ViewZoneInfo->PymdInput.RawSeqWp], (UINT32)PymdInfo->YuvPts, &ViewZoneInfo->PymdInput.DonePtsBase, &PymdDonePts64);
#else
        /* RawCapPts */
        InTimeU64 = PymdInfo->CapPts;
        (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
        RawCapPts64 = OutTimeU64;
        ViewZoneInfo->PymdInput.RawCapPtsBase = (OutTimeU64 & 0xFFFFFFFF00000000ULL);

        /* YuvDonePts */
        InTimeU64 = PymdInfo->YuvPts;
        (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
        PymdDonePts64 = OutTimeU64;
        ViewZoneInfo->PymdInput.DonePtsBase = (OutTimeU64 & 0xFFFFFFFF00000000ULL);
#endif
    } else {
        /* first raw, direct use it */
        RawCapSeq64 = PymdInfo->CapSequence;
#ifndef USE_TIME_MANAGER
        RawCapPts64 = PymdInfo->CapPts;
        PymdDonePts64 = PymdInfo->YuvPts;
#else
        /* RawCapPts */
        InTimeU64 = PymdInfo->CapPts;
        (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
        RawCapPts64 = OutTimeU64;
        ViewZoneInfo->PymdInput.RawCapPtsBase = (OutTimeU64 & 0xFFFFFFFF00000000ULL);

        /* YuvDonePts */
        InTimeU64 = PymdInfo->YuvPts;
        (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
        PymdDonePts64 = OutTimeU64;
        ViewZoneInfo->PymdInput.DonePtsBase = (OutTimeU64 & 0xFFFFFFFF00000000ULL);
#endif
    }
    /* Using CapSeqNo to determine wp index */
    ViewZoneInfo->PymdInput.RawSeqWp = (UINT8)(PymdInfo->CapSequence % MAX_RAW_SEQ_Q);
    ViewZoneInfo->PymdInput.RawCapSeq[ViewZoneInfo->PymdInput.RawSeqWp] = (UINT32)PymdInfo->CapSequence;
    ViewZoneInfo->PymdInput.RawCapPts[ViewZoneInfo->PymdInput.RawSeqWp] = RawCapPts32;
    ViewZoneInfo->PymdInput.DonePts[ViewZoneInfo->PymdInput.RawSeqWp] = (UINT32)PymdInfo->YuvPts;
    PymdInfo->CapPts = RawCapPts64;
    ViewZoneInfo->PymdInput.RawCapPts64[ViewZoneInfo->PymdInput.RawSeqWp] = RawCapPts64;
    PymdInfo->CapSequence = RawCapSeq64;
    ViewZoneInfo->PymdInput.RawCapSeq64[ViewZoneInfo->PymdInput.RawSeqWp] = RawCapSeq64;
    PymdInfo->YuvPts = PymdDonePts64;
    ViewZoneInfo->PymdInput.DonePts64[ViewZoneInfo->PymdInput.RawSeqWp] = PymdDonePts64;
    HL_GetViewZoneInfoUnLock((UINT16)PymdInfo->ViewZoneId);

    return Rval;
}

static UINT32 LiveviewLndtReadyPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    AMBA_DSP_LNDT_DATA_RDY_s *pLndtInfo;

    dsp_osal_typecast(&pLndtInfo, &pEventData);

    HL_GetViewZoneInfoPtr((UINT16)pLndtInfo->ViewZoneId, &ViewZoneInfo);
    pLndtInfo->HierSource = ViewZoneInfo->LaneDetect.HierSource;
    Rval = dsp_osal_memcpy(&pLndtInfo->CropWindow, &ViewZoneInfo->LaneDetect.CropWindow, sizeof(AMBA_DSP_WINDOW_s));
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* Lndt comes from Pymd result, so we just levrage pymd's Pts/Seq inforamtion */
    pLndtInfo->CapPts = ViewZoneInfo->PymdInput.RawCapPts64[ViewZoneInfo->PymdInput.RawSeqWp];
    pLndtInfo->YuvPts = ViewZoneInfo->PymdInput.DonePts64[ViewZoneInfo->PymdInput.RawSeqWp];
    pLndtInfo->CapSequence = ViewZoneInfo->PymdInput.RawCapSeq64[ViewZoneInfo->PymdInput.RawSeqWp];

    if ((Rval != DSP_ERR_NONE) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("Liveview Lndt ready Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 LiveviewMainY12ReadyPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    AMBA_DSP_YUV_INFO_s *pYuvInfo;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 VinId = 0U;
    CTX_VIN_INFO_s VinInfo = {0};
    UINT8 CapSeqQIdx;
    UINT32 RawCapPts32 = 0U;
    UINT64 RawCapPts64 = 0ULL, RawCapSeq64 = 0ULL, YuvDonePts64 = 0ULL;
#ifdef USE_TIME_MANAGER
    UINT64 InTimeU64, OutTimeU64 = 0ULL;
#endif

    dsp_osal_typecast(&pYuvInfo, &pEventData);

    Rval = HL_GetViewZoneVinId((UINT16)pYuvInfo->YuvData.ViewZoneId, &VinId);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);
    if (VinId < DSP_VIN_MAX_NUM) {
        HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);

        CapSeqQIdx = (UINT8)(pYuvInfo->CapSeqU32 % MAX_RAW_SEQ_Q);
        if (VinInfo.VinCtrl.RawCapSeq[CapSeqQIdx] == pYuvInfo->CapSeqU32) {
            RawCapPts32 = VinInfo.VinCtrl.RawCapPts[CapSeqQIdx];
        } else {
            RawCapPts32 = pYuvInfo->CapPtsU32;
        }

        HL_GetViewZoneInfoLock((UINT16)pYuvInfo->YuvData.ViewZoneId, &ViewZoneInfo);

        /* Update PTS and sequence number when this event is for main yuv */
        if (ViewZoneInfo->MainY12Input.RawSeqWp != 0xFFU) {
            PtsOverflowHdlr(ViewZoneInfo->MainY12Input.RawCapSeq[ViewZoneInfo->MainY12Input.RawSeqWp], pYuvInfo->CapSeqU32, &ViewZoneInfo->MainY12Input.RawCapSeqBase, &RawCapSeq64);
#ifndef USE_TIME_MANAGER
            PtsOverflowHdlr(ViewZoneInfo->MainY12Input.RawCapPts[ViewZoneInfo->MainY12Input.RawSeqWp], RawCapPts32, &ViewZoneInfo->MainY12Input.RawCapPtsBase, &RawCapPts64);
            PtsOverflowHdlr(ViewZoneInfo->MainY12Input.DonePts[ViewZoneInfo->MainY12Input.RawSeqWp], pYuvInfo->YuvPtsU32, &ViewZoneInfo->MainY12Input.DonePtsBase, &YuvDonePts64);
#else
            /* RawCapPts */
            InTimeU64 = RawCapPts32;
            (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
            RawCapPts64 = OutTimeU64;
            ViewZoneInfo->MainY12Input.RawCapPtsBase = (OutTimeU64 & 0xFFFFFFFF00000000ULL);

            /* YuvDonePts */
            InTimeU64 = pYuvInfo->YuvPtsU32;
            (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
            YuvDonePts64 = OutTimeU64;
            ViewZoneInfo->MainY12Input.DonePtsBase = (OutTimeU64 & 0xFFFFFFFF00000000ULL);
#endif
        } else {
            /* first raw, directly use it */
            RawCapSeq64 = pYuvInfo->CapSeqU32;
#ifndef USE_TIME_MANAGER
            RawCapPts64 = RawCapPts32;
            YuvDonePts64 = pYuvInfo->YuvPtsU32;
#else
            /* RawCapPts */
            InTimeU64 = RawCapPts32;
            (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
            RawCapPts64 = OutTimeU64;
            ViewZoneInfo->MainY12Input.RawCapPtsBase = (OutTimeU64 & 0xFFFFFFFF00000000ULL);

            /* YuvDonePts */
            InTimeU64 = pYuvInfo->YuvPtsU32;
            (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
            YuvDonePts64 = OutTimeU64;
            ViewZoneInfo->MainY12Input.DonePtsBase = (OutTimeU64 & 0xFFFFFFFF00000000ULL);
#endif
        }

        pYuvInfo->YuvData.CapPts = RawCapPts64;
        pYuvInfo->YuvData.YuvPts = YuvDonePts64;
        pYuvInfo->YuvData.CapSequence = RawCapSeq64;

        /* Using CapSeqNo to determine wp index */
        ViewZoneInfo->MainY12Input.RawSeqWp = (UINT8)(pYuvInfo->CapSeqU32 % MAX_RAW_SEQ_Q);
        ViewZoneInfo->MainY12Input.RawCapSeq[ViewZoneInfo->MainY12Input.RawSeqWp] = pYuvInfo->CapSeqU32;
        ViewZoneInfo->MainY12Input.RawCapPts[ViewZoneInfo->MainY12Input.RawSeqWp] = RawCapPts32;
        ViewZoneInfo->MainY12Input.DonePts[ViewZoneInfo->MainY12Input.RawSeqWp] = pYuvInfo->YuvPtsU32;
        ViewZoneInfo->MainY12Input.RawCapPts64[ViewZoneInfo->MainY12Input.RawSeqWp] = RawCapPts64;
        ViewZoneInfo->MainY12Input.RawCapSeq64[ViewZoneInfo->MainY12Input.RawSeqWp] = RawCapSeq64;
        ViewZoneInfo->MainY12Input.DonePts64[ViewZoneInfo->MainY12Input.RawSeqWp] = YuvDonePts64;
        HL_GetViewZoneInfoUnLock((UINT16)pYuvInfo->YuvData.ViewZoneId);
    }

    if ((Rval != DSP_ERR_NONE) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("Liveview main y12 ready Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 LiveviewVoutReadyPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE;
    AMBA_DSP_VOUT_DATA_INFO_s *pVoutInfo;
    CTX_VOUT_INFO_s VoutInfo;
    UINT64 StartPts64 = 0U, DonePts64 = 0U;
#ifdef USE_TIME_MANAGER
    UINT64 InTimeU64, OutTimeU64 = 0ULL;
#endif

    dsp_osal_typecast(&pVoutInfo, &pEventData);

    HL_GetVoutInfo(HL_MTX_OPT_GET, (UINT16)pVoutInfo->VoutIdx, &VoutInfo);

    if (VoutInfo.PtsWp != 0xFFU) {
#ifndef USE_TIME_MANAGER
        PtsOverflowHdlr(VoutInfo.StartPts[VoutInfo.PtsWp], (UINT32)pVoutInfo->DispStartTime, &VoutInfo.StartPtsBase, &StartPts64);
        PtsOverflowHdlr(VoutInfo.DonePts[VoutInfo.PtsWp], (UINT32)pVoutInfo->DispDoneTime, &VoutInfo.DonePtsBase, &DonePts64);
#else
        /* VoutStartPts */
        InTimeU64 = pVoutInfo->DispStartTime;
        (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
        StartPts64 = OutTimeU64;
        VoutInfo.StartPtsBase = (OutTimeU64 & 0xFFFFFFFF00000000ULL);

        /* VoutDonePts */
        InTimeU64 = pVoutInfo->DispDoneTime;
        (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
        DonePts64 = OutTimeU64;
        VoutInfo.DonePtsBase = (OutTimeU64 & 0xFFFFFFFF00000000ULL);
#endif
    } else {
        /* first raw, direct use it */
#ifndef USE_TIME_MANAGER
        StartPts64 = pVoutInfo->DispStartTime;
        DonePts64 = pVoutInfo->DispDoneTime;
#else
        /* VoutStartPts */
        InTimeU64 = pVoutInfo->DispStartTime;
        (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
        StartPts64 = OutTimeU64;
        VoutInfo.StartPtsBase = (OutTimeU64 & 0xFFFFFFFF00000000ULL);

        /* VoutDonePts */
        InTimeU64 = pVoutInfo->DispDoneTime;
        (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
        DonePts64 = OutTimeU64;
        VoutInfo.DonePtsBase = (OutTimeU64 & 0xFFFFFFFF00000000ULL);
#endif
        VoutInfo.PtsWp = (UINT8)(MAX_RAW_SEQ_Q-1U);
    }

    /* Using CapSeqNo to determine wp index */
    VoutInfo.PtsWp = (UINT8)((VoutInfo.PtsWp+1U) % MAX_RAW_SEQ_Q);
    VoutInfo.StartPts64[VoutInfo.PtsWp] = StartPts64;
    VoutInfo.DonePts64[VoutInfo.PtsWp] = DonePts64;
    VoutInfo.StartPts[VoutInfo.PtsWp] = (UINT32)pVoutInfo->DispStartTime;
    VoutInfo.DonePts[VoutInfo.PtsWp] = (UINT32)pVoutInfo->DispDoneTime;
    pVoutInfo->DispStartTime = StartPts64;
    pVoutInfo->DispDoneTime = DonePts64;

    HL_SetVoutInfo(HL_MTX_OPT_SET, (UINT16)pVoutInfo->VoutIdx, &VoutInfo);

    return Rval;
}

static UINT32 DspErrorPreProcFunc(const void *pEventData)
{
#if defined (CONFIG_ENABLE_DSP_MONITOR)
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    (void)pEventData;
    HL_GetResourcePtr(&Resource);
    if ((Resource->MonitorMask & DSP_MON_BITMASK_UCODE) > 0U) {
        AMBA_DSP_MONITOR_ERR_NOTIFY_s MonErrNotify = {0};

        MonErrNotify.ModuleId = (SSP_ERR_BASE >> 16U);
        MonErrNotify.ErrorId = DSP_ERR_0200;
        Rval = dsp_mon_error_notify(&MonErrNotify);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    if ((Rval != DSP_ERR_NONE) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("Dsp error failed[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }
#else
    UINT32 Rval = DSP_ERR_NONE;

    // DO NOTHING
    (void)pEventData;
#endif

    return Rval;
}

static UINT32 LiveviewSyncJobReadyPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    UINT32 i, Cmp, NewRp;
    AMBA_DSP_LV_SYNC_JOB_INFO_s *pSyncJobInfo;
    UINT8 ExitLoop, IsGroupJobDone = 0U;
    CTX_YUVSTRM_EFCT_SYNC_CTRL_s *EfctSyncCtrl = HL_CtxYuvStrmEfctSyncCtrlPtr;
    CTX_EFCT_SYNC_JOB_SET_s *EfctSyncJobSet = HL_CtxYuvStrmEfctSyncJobSetPtr;
    UINT16 YuvStrmIdx = 0U, SeqIdx = 0U, JobType = 0U, SubJobIdx = 0U;

    dsp_osal_typecast(&pSyncJobInfo, &pEventData);

    /* Sweep Effect and find matched */
    if (pSyncJobInfo->JobId > 0U) {
        YuvStrmIdx = (UINT16)DSP_GetBit(pSyncJobInfo->JobId, EFCT_SYNC_JOB_YUVSTRM_SHIFT_BIT, EFCT_SYNC_JOB_YUVSTRM_LENGTH);
        SeqIdx     = (UINT16)DSP_GetBit(pSyncJobInfo->JobId, EFCT_SYNC_JOB_SEQ_SHIFT_BIT, EFCT_SYNC_JOB_SEQ_LENGTH);
        JobType    = (UINT16)DSP_GetBit(pSyncJobInfo->JobId, EFCT_SYNC_JOB_TYPE_SHIFT_BIT, EFCT_SYNC_JOB_TYPE_LENGTH);
        SubJobIdx  = (UINT16)DSP_GetBit(pSyncJobInfo->JobId, 0U, EFCT_SYNC_JOB_SUBID_LENGTH);

        HL_GetYuvStrmEfctSyncCtrlLock(YuvStrmIdx, &EfctSyncCtrl);
        HL_GetYuvStrmEfctSyncJobSetLock(YuvStrmIdx, SeqIdx-1U, &EfctSyncJobSet);
        if ((EfctSyncJobSet->JobNum > 0U) &&
            (EfctSyncJobSet->Status == EFCT_SYNC_JOBSET_STATUS_PROC)) {
            ExitLoop = 0U;

            for (i=0U; i<EfctSyncJobSet->JobNum; i++) {
                if ((JobType == 0U) && (SubJobIdx == 0U)) {
                    /* MasterBatch */
                    Cmp = 0U;
                    Rval = dsp_osal_memcmp(&EfctSyncJobSet->JobId, &pSyncJobInfo->JobId, sizeof(UINT32), &Cmp);
                    DSP_FillErrline(Rval, &ErrLine, __LINE__);
                    if (Cmp == 0U) {
                        DSP_ClearU16Bit(&EfctSyncJobSet->JobStatus, EFCT_SYNC_JOB_STATUS_BATCH_BIT);
                        ExitLoop = 1U;
                    }
                } else {
                    /* GrpBatch */
                    Cmp = 0U;
                    Rval = dsp_osal_memcmp(&EfctSyncJobSet->Job[i].JobId, &pSyncJobInfo->JobId, sizeof(UINT32), &Cmp);
                    DSP_FillErrline(Rval, &ErrLine, __LINE__);
                    if (Cmp == 0U) {
                        DSP_ClearU16Bit(&EfctSyncJobSet->JobStatus, i);
                        ExitLoop = 1U;
                    }
                }

                if (ExitLoop == 1U) {
                    break;
                }
            }

            if ((EfctSyncJobSet->JobStatus == 0U) &&
                (EfctSyncJobSet->JobNum != 0xFFU) &&
                (Rval == DSP_ERR_NONE)) {
                    Rval = DSP_RelBuf(&EfctSyncCtrl->PoolDesc, 1U, &NewRp, 1U/*FullnessCheck*/);
                    DSP_FillErrline(Rval, &ErrLine, __LINE__);
                    if (Rval == DSP_ERR_NONE) {
                        EfctSyncCtrl->PoolDesc.Rp = NewRp;
                        IsGroupJobDone = 1U;
                        EfctSyncJobSet->JobNum = 0xFFU;
                        EfctSyncJobSet->Status = EFCT_SYNC_JOBSET_STATUS_IDLE;
                    }
            }
            if (EfctSyncCtrl->PoolDesc.Rp == EfctSyncCtrl->PoolDesc.Wp) {
                //Update SyncState of certain YuvStream
                EfctSyncCtrl->State = EFCT_SYNC_OFF;
            }
        }
        HL_GetYuvStrmEfctSyncJobSetUnLock(YuvStrmIdx, SeqIdx-1U);
        HL_GetYuvStrmEfctSyncCtrlUnLock(YuvStrmIdx);
    }

    if (IsGroupJobDone == 0U) {
        Rval = DSP_ERR_0004; //not the right timing to report
    } else {
        /* Fill correct JobId */
        DSP_EFCT_SYNC_JOB_ID_s NewJobId = {0};
//        if (Rval == DSP_ERR_NONE) {
            Rval = HL_ComposeEfctSyncJobId(&NewJobId,
                                          YuvStrmIdx,
                                          SeqIdx,
                                          EFCT_SYNC_JOB_ID_DONTCARE,
                                          EFCT_SYNC_JOB_ID_DONTCARE);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
//        }
        if (Rval == DSP_ERR_NONE) {
            Rval = dsp_osal_memcpy(&pSyncJobInfo->JobId, &NewJobId, sizeof(UINT32));
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
    }

    if ((Rval != DSP_ERR_NONE) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("Liveview sync job ready Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 VprocIdspDataPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    const IDSP_INFO_s *pIdspInfo;
    ULONG ULAddr;

    dsp_osal_typecast(&pIdspInfo, &pEventData);
    dsp_osal_typecast(&ULAddr, &pEventData);

    HL_GetViewZoneInfoLock((UINT16)pIdspInfo->ChannelId, &ViewZoneInfo);
    ViewZoneInfo->LastIdspCfgAddr = ULAddr;
    HL_GetViewZoneInfoUnLock((UINT16)pIdspInfo->ChannelId);

    Rval = DSP_ERR_0004; // internal event
    return Rval;
}

static UINT32 VinSectionCfgDataPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    const IDSP_VIN_CFG_RDY_s *pVinSecCfgInfo;
    const IDSP_INFO_s *pIdspInfoBase = NULL;
    IDSP_INFO_s *pIdspInfo = NULL;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 i, ViewZoneVinId;
#ifdef USE_STAT_IDSP_BUF
    DSP_MEM_BLK_t MemBlk = {0};
#endif

    dsp_osal_typecast(&pVinSecCfgInfo, &pEventData);
    if (pVinSecCfgInfo->IdspInfoBaseAddr > 0U) {
        dsp_osal_typecast(&pIdspInfoBase, &pVinSecCfgInfo->IdspInfoBaseAddr);

        HL_GetResourcePtr(&Resource);
        for (i=0U; i<Resource->MaxViewZoneNum; i++) {
            if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) {
                continue;
            }
            HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
            DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &ViewZoneVinId);
            if (ViewZoneVinId == pVinSecCfgInfo->VinId) {
#ifdef USE_STAT_IDSP_BUF
                Rval = DSP_GetStatBuf(DSP_STAT_BUF_IDSP, i, &MemBlk);
                DSP_FillErrline(Rval, &ErrLine, __LINE__);

                dsp_osal_typecast(&pIdspInfo, &MemBlk.Base);
#else
                pIdspInfo = &pIdspInfoBase[i];
#endif
                pIdspInfo->VinId = pVinSecCfgInfo->VinId;
                pIdspInfo->VinCfg.CapSeqNo = pVinSecCfgInfo->CapSeqNo;
                pIdspInfo->VinCfg.VinSecCfg.CfgAddr = pVinSecCfgInfo->CfgAddr;
                pIdspInfo->VinCfg.VinSecCfg.CfgSize = HL_GetVinSecCfgSize((UINT16)pVinSecCfgInfo->VinId);
            }
        }
    }

    Rval = DSP_ERR_0004; // internal event

    if (ErrLine != 0U) {
        AmbaLL_LogUInt5("Vin section config data Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 VprocEffectYuvGiveStlRawEvt(const UINT16 DataCapId, const AMBA_DSP_YUV_INFO_s *pYuvInfo,
                                                 const CTX_DATACAP_INFO_s *pDataCapInfo, UINT32 *pErrLine)
{
    UINT32 Rval = DSP_ERR_NONE;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    AMBA_DSP_STL_RAW_DATA_INFO_s *pStlRawData;
    DSP_STL_RAW_CAP_BUF_s *pStlRawCapBuf = NULL;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;
    UINT8 CapSeqQIdx, InvokeEvt = 0U;

    Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_STL_RAW_RDY);
    DSP_FillErrline(Rval, pErrLine, __LINE__);

    if (Rval == DSP_ERR_NONE) {
        dsp_osal_typecast(&pStlRawCapBuf, &EventBufAddr);
        Rval = dsp_osal_memset(pStlRawCapBuf, 0, sizeof(DSP_STL_RAW_CAP_BUF_s));
        DSP_FillErrline(Rval, pErrLine, __LINE__);

        if (Rval == DSP_ERR_NONE) {
            pStlRawData = &pStlRawCapBuf->StlRawData;
            pStlRawData->CapInstance = DataCapId;
            pStlRawData->BufFormat = RAW_FORMAT_MEM_YUV420;
            pStlRawData->Buf.BaseAddr = pYuvInfo->YuvData.Buffer.BaseAddrY;
            pStlRawData->Buf.Pitch = pYuvInfo->YuvData.Buffer.Pitch;
            // Using DataCapInfoWindow instead of StlRawDataWindow, because StlRawDataWindow is buf's w/h (not image's w/h)
            Rval = dsp_osal_memcpy(&pStlRawData->Buf.Window, &pDataCapInfo->Cfg.DataBuf.Window, sizeof(AMBA_DSP_WINDOW_s));
            DSP_FillErrline(Rval, pErrLine, __LINE__);

            pStlRawData->AuxBufFormat = RAW_FORMAT_MEM_YUV420;
            pStlRawData->AuxBuf.BaseAddr = pYuvInfo->YuvData.Buffer.BaseAddrUV;
            pStlRawData->AuxBuf.Pitch = pYuvInfo->YuvData.Buffer.Pitch;
            // Using DataCapInfoWindow instead of StlRawDataWindow, because StlRawDataWindow is buf's w/h (not image's w/h)
            Rval = dsp_osal_memcpy(&pStlRawData->AuxBuf.Window, &pDataCapInfo->Cfg.DataBuf.Window, sizeof(AMBA_DSP_WINDOW_s));
            DSP_FillErrline(Rval, pErrLine, __LINE__);
            if (pStlRawData->AuxBufFormat == RAW_FORMAT_MEM_YUV420) {
                pStlRawData->AuxBuf.Window.Height = pStlRawData->AuxBuf.Window.Height >> 1;
            }

            // ME data
            if (pDataCapInfo->Cfg.CapDataType == DSP_DATACAP_TYPE_VIDEO_YUV) {
                // Using DataCapInfoWindow instead of StlRawDataWindow, because StlRawDataWindow is buf's w/h (not image's w/h)
                Rval = dsp_osal_memcpy(&pStlRawData->Aux2Buf.Window, &pDataCapInfo->Cfg.AuxDataBuf.Window, sizeof(AMBA_DSP_WINDOW_s));
                DSP_FillErrline(Rval, pErrLine, __LINE__);
                pStlRawData->Aux2Buf.BaseAddr = pYuvInfo->Me1Buf.BaseAddr;
                pStlRawData->Aux2Buf.Pitch = pYuvInfo->Me1Buf.Pitch;
                pStlRawData->Aux2Buf.Window.Width = pStlRawData->Aux2Buf.Window.Width >> DSP_VIDEO_ME1_DATA_SHIFT;
                pStlRawData->Aux2Buf.Window.Height= pStlRawData->Aux2Buf.Window.Height >> DSP_VIDEO_ME1_DATA_SHIFT;
            }

            // Replace U64 RawSeq/RawPts
            HL_GetViewZoneInfoPtr((UINT16)pYuvInfo->ChannelId, &ViewZoneInfo);
            // Using CapSeqNo to find index
            CapSeqQIdx = (UINT8)(pYuvInfo->CapSeqU32 % MAX_RAW_SEQ_Q);
            if (ViewZoneInfo->YuvInput.RawCapSeq[CapSeqQIdx] == pYuvInfo->CapSeqU32) {
                pStlRawData->CapSequence = ViewZoneInfo->YuvInput.RawCapSeq64[CapSeqQIdx];
                pStlRawData->CapPts = ViewZoneInfo->YuvInput.RawCapPts64[CapSeqQIdx];
            } else {
                pStlRawData->CapSequence = (UINT64)pYuvInfo->CapSeqU32;
                pStlRawData->CapPts = (UINT64)pYuvInfo->CapPtsU32;
            }

            pStlRawCapBuf->CapSrcType = CAP_SRC_TYPE_VPROC;
            if (Rval == DSP_ERR_NONE) {
                Rval = DSP_GiveEvent(LL_EVENT_STL_RAW_RDY, pStlRawCapBuf, EventBufIdx);
                DSP_FillErrline(Rval, pErrLine, __LINE__);
                InvokeEvt = 1U;
            }
        }
        if (InvokeEvt == 0U) {
            /* EventInfoAddr need to be released
             * If you request EventInfoAddr but don't call GiveEvent */
            Rval = DSP_ReleaseEventInfoBuf(EventBufIdx);
            DSP_FillErrline(Rval, pErrLine, __LINE__);
        }
    }

    return Rval;
}

static UINT32 VprocEffectYuvDataPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    CTX_DATACAP_INFO_s DataCapInfo;
    const AMBA_DSP_YUV_INFO_s *pYuvInfo;
    UINT8 IsDataCap = 0U, IsValidDataCapIdx, IsPurePassThrough;
    UINT16 i;

    dsp_osal_typecast(&pYuvInfo, &pEventData);

    /* Check DataCapInfo for updating */
    for (i=DSP_VPROC_YUV_PREVC; i<DSP_VPROC_YUV_NUM; i++) {
        if (pYuvInfo->YuvType == i) {
            if (1U == DSP_GetBit(pYuvInfo->ExtBufMask, (UINT32)DSP_VprocPinVprocMemTypeMap[i], 1U)) {
                IsDataCap = 1U;
                break;
            }
        }
    }

    /* Currently only report when DataCapture, LV TBD */
    if (IsDataCap == 1U) {
        for (i=0U; i<AMBA_DSP_MAX_DATACAP_NUM; i++) {
            HL_GetDataCapInfo(HL_MTX_OPT_ALL, i, &DataCapInfo);
            if ((DataCapInfo.Status == DATA_CAP_STATUS_RUNNING) ||
                (DataCapInfo.Status == DATA_CAP_STATUS_2_STOP)) {
                HL_StlYuvValidateCapInst(&DataCapInfo, pYuvInfo, &IsValidDataCapIdx, &IsPurePassThrough);

                if ((IsValidDataCapIdx == 1U) && (IsPurePassThrough == 0U)) {
                    Rval = VprocEffectYuvGiveStlRawEvt(i, pYuvInfo, &DataCapInfo, &ErrLine);
                    DSP_FillErrline(Rval, &ErrLine, __LINE__);
                    break;
                }
            } else {
                continue;
            }
        }
    }

    Rval = DSP_ERR_0004; // internal event

    if (ErrLine != 0U) {
        AmbaLL_LogUInt5("Vproc Effect yuv data Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 VprocCfaModeMapPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    AMBA_DSP_VPROC_CFA_MODE_MAPPING_s *pCfaModeMap;
    UINT16 VinId = 0U;
    CTX_VIN_INFO_s VinInfo = {0};

    dsp_osal_typecast(&pCfaModeMap, &pEventData);
    Rval = HL_GetViewZoneVinId(pCfaModeMap->ChannelId, &VinId);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);
    if ((Rval == DSP_ERR_NONE) &&
        (VinId < DSP_VIN_MAX_NUM)) {
        HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);

        /*
         * 2019/12/26
         * When CFAMode = VIN_INPUT_MODE_YUV
         *   - if Vin support Yuv422, we shall not report HIST/CFA stat
         *     ucode still report CFA/HIST address, but the content is un-touched
         *   - if Vin not support Yuv422, we shall not report HIST/CFA/PG stat
         */
        pCfaModeMap->CfaMode = VinInfo.SensorMode;
    }

    Rval = DSP_ERR_0004; // internal event

    if (ErrLine != 0U) {
        AmbaLL_LogUInt5("Vproc cfa mode map Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 BatchInfoMapPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE;
    CTX_BATCHQ_INFO_s BatchQInfo = {0};
    AMBA_LL_BATCH_IDX_INFO_MAPPING_s *pBatchQInfo;

    dsp_osal_typecast(&pBatchQInfo, &pEventData);
    HL_GetDspBatchQInfo(pBatchQInfo->BatchCmdId, &BatchQInfo);
    pBatchQInfo->IsoCfgId = BatchQInfo.IsoCfgId;
    pBatchQInfo->ImgPrmdId = BatchQInfo.ImgPrmdId;
    pBatchQInfo->YuvStrmGrpId = BatchQInfo.YuvStrmGrpId;
    pBatchQInfo->StlProcId = BatchQInfo.StlProcId;

    Rval = DSP_ERR_0004; // internal event
    return Rval;
}

static UINT32 EncGetActiveBitPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE;
    CTX_ENCODER_INFO_s EncodeInfo = {0};
    AMBA_LL_ENC_GET_ACTIVE_BIT_s* pGetActiveBitInfo;

    dsp_osal_typecast(&pGetActiveBitInfo, &pEventData);
    HL_GetEncoderInfo(HL_MTX_OPT_ALL, &EncodeInfo);
    pGetActiveBitInfo->ActiveBit = EncodeInfo.ActiveBit;

    Rval = DSP_ERR_0004; // internal event
    return Rval;
}

static UINT32 DecGetBitsFormatPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    AMBA_LL_DEC_GET_BITSTRM_FORMAT_s *pDecGetBitFormat;

    dsp_osal_typecast(&pDecGetBitFormat, &pEventData);
    HL_GetVidDecInfo(HL_MTX_OPT_ALL, pDecGetBitFormat->DecoderId, &VidDecInfo);
    pDecGetBitFormat->BitsFormat = VidDecInfo.BitsFormat;

    Rval = DSP_ERR_0004; // internal event
    return Rval;
}

static UINT32 DecUpdateStatusPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE;
    const AMBA_LL_DEC_UPDATE_STATUS_s *pDecUpdateStatus;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};

    dsp_osal_typecast(&pDecUpdateStatus, &pEventData);
    HL_GetVidDecInfo(HL_MTX_OPT_GET, pDecUpdateStatus->DecoderId, &VidDecInfo);
    VidDecInfo.DspState = pDecUpdateStatus->Status;
    HL_SetVidDecInfo(HL_MTX_OPT_SET, pDecUpdateStatus->DecoderId, &VidDecInfo);

    Rval = DSP_ERR_0004; // internal event
    return Rval;
}

static UINT32 SysGetClkInfoPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE;
    AMBA_LL_SYS_CLK_INFO_s *pSysClkInfo;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);

    dsp_osal_typecast(&pSysClkInfo, &pEventData);
    if (pSysClkInfo->Type == LL_SYS_CLK_IDSP) {
        pSysClkInfo->Value = Resource->IdspClk;
    } else if (pSysClkInfo->Type == LL_SYS_CLK_CORE) {
        pSysClkInfo->Value = Resource->CoreClk;
    } else if (pSysClkInfo->Type == LL_SYS_CLK_DRAM) {
        pSysClkInfo->Value = Resource->DramClk;
    } else {
        pSysClkInfo->Value = Resource->AudioClk;
    }

    Rval = DSP_ERR_0004; // internal event
    return Rval;
}

static UINT32 DspSimOpPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE;
    AMBA_LL_SIM_OP_s *pSimOp;
    UINT8 (*pIsSimilarEnabled)(void) = IsSimilarEnabled;
    const UINT32 *pU32 = NULL;

    dsp_osal_typecast(&pSimOp, &pEventData);

    if (pSimOp->Op == LL_SIM_OP_QUERY_ENABLE) {
        if (pIsSimilarEnabled != NULL) {
            if (1U == IsSimilarEnabled()) {
                pSimOp->Data[0U] = 1U;
            } else {
                pSimOp->Data[0U] = 0U;
            }
        } else {
            pSimOp->Data[0U] = 0U;
        }
    } else {
        //Calc CRC
        dsp_osal_typecast(&pU32, &pSimOp->Addr);
        pSimOp->Data[2U] = SIM_CalcCheckSum32Add(pU32, pSimOp->Data[0U], pSimOp->Data[1U]);
    }

    Rval = DSP_ERR_0004; // internal event
    return Rval;
}

//#define DEBUG_TESTFRAME_STATUS
static UINT32 TestFrmPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 StageId;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    AMBA_DSP_EVENT_TESTFRAME_INFO_s *pTfInfo;

    dsp_osal_typecast(&pTfInfo, &pEventData);

    StageId = pTfInfo->Stage;
    if (StageId < DSP_TEST_STAGE_NUM) {
        UINT32 State;

        HL_GetResourceLock(&Resource);
        State = Resource->TestFrmStatus[StageId];
        if (State == DSP_TESTFRAME_STATE_CONFIG) {
#ifdef DEBUG_TESTFRAME_STATUS
            AmbaLL_LogUInt5("TestFrmPreProcFunc %u TestFrmStatus [%u]: %u -> %u",
                    __LINE__, StageId, Resource->TestFrmStatus[StageId], DSP_TESTFRAME_STATE_ENABLE, 0U);
#endif
            Resource->TestFrmStatus[StageId] = DSP_TESTFRAME_STATE_ENABLE;
            Rval = DSP_ERR_0004; // keep internal event
        } else if (State == DSP_TESTFRAME_STATE_ENABLE) {
            if (Resource->TestFrmRepeat[StageId] == 1U) {
#ifdef DEBUG_TESTFRAME_STATUS
                AmbaLL_LogUInt5("TestFrmPreProcFunc %u TestFrmStatus [%u]: %u -> %u",
                        __LINE__, StageId, Resource->TestFrmStatus[StageId], DSP_TESTFRAME_STATE_START, 0U);
#endif
                Resource->TestFrmStatus[StageId] = DSP_TESTFRAME_STATE_START;
            } else {
#ifdef DEBUG_TESTFRAME_STATUS
                AmbaLL_LogUInt5("TestFrmPreProcFunc %u TestFrmStatus [%u]: %u -> %u",
                        __LINE__, StageId, Resource->TestFrmStatus[StageId], DSP_TESTFRAME_STATE_CONFIG, 0U);
#endif
                Resource->TestFrmStatus[StageId] = DSP_TESTFRAME_STATE_CONFIG;
            }
            pTfInfo->State = 1U;
            Rval = DSP_ERR_NONE; // send out event
        } else if (State == DSP_TESTFRAME_STATE_START) {
            Rval = DSP_ERR_0004; // keep internal event
        } else {
            //disable case
            Rval = DSP_ERR_0004; // keep internal event
        }

        HL_GetResourceUnLock();
    } else {
        Rval = DSP_ERR_0001;
    }

    return Rval;
}

static UINT32 TimeTickUptPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 *pTimeTick;
    UINT64 TimeU64 = 0ULL;
    const UINT32 TimeId = DSP_TIME_ID_AUDIO_TICK;

    dsp_osal_typecast(&pTimeTick, &pEventData);
    TimeU64 = *pTimeTick;
    Rval = HL_TimeMngrSetTime(TimeId, TimeU64);
    if (Rval != DSP_ERR_NONE) {
        dsp_osal_printU5("[ERR] TimeTickPreproc : SetTime [%d][0x%X]", TimeId, Rval, 0U, 0U, 0U);
    }

    Rval = DSP_ERR_0004; // internal event
    return Rval;
}

static UINT32 VprocHierIdxMapPreProcFunc(const void *pEventData)
{
    const UINT16 HL_VprocHierIdOctaveMap[DSP_HIER_NUM] = {
        [0U]  = 0U,
        [1U]  = 0xFFFFU,
        [2U]  = 1U,
        [3U]  = 0xFFFFU,
        [4U]  = 2U,
        [5U]  = 0xFFFFU,
        [6U]  = 3U,
        [7U]  = 0xFFFFU,
        [8U]  = 4U,
        [9U]  = 0xFFFFU,
        [10U] = 5U,
        [11U] = 0xFFFFU,
        [12U] = 6U,
    };

    const UINT16 HL_VprocHierIdHalfOctaveMap[DSP_HIER_NUM] = {
        [0U]  = 0U,
        [1U]  = 1U,
        [2U]  = 2U,
        [3U]  = 3U,
        [4U]  = 4U,
        [5U]  = 5U,
        [6U]  = 6U,
        [7U]  = 0xFFFFU,
        [8U]  = 0xFFFFU,
        [9U]  = 0xFFFFU,
        [10U] = 0xFFFFU,
        [11U] = 0xFFFFU,
        [12U] = 0xFFFFU,
    };

    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    AMBA_DSP_VPROC_HIER_IDX_MAPPING_s *pHeirIdxMap;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 IsPolySqrt2;

    dsp_osal_typecast(&pHeirIdxMap, &pEventData);
    HL_GetViewZoneInfoPtr(pHeirIdxMap->ChannelId, &ViewZoneInfo);
    IsPolySqrt2 = (UINT16)DSP_GetU16Bit(ViewZoneInfo->Pyramid.IsPolySqrt2, DSP_PYMD_PLOY_IDX, DSP_PYMD_PLOY_LEN);
    if (pHeirIdxMap->InputIdx >= DSP_HIER_NUM) {
        Rval = DSP_ERR_0001;
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    } else if (IsPolySqrt2 > 1U) {
        Rval = DSP_ERR_0001;
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    } else {
        if (IsPolySqrt2 == 0U) {
            //When OctaveMode = 1, Hier0/2/4/6/8/10 will generate actual Hier0/1/2/3/4/5 result sequentially
            pHeirIdxMap->OutputIdx = HL_VprocHierIdOctaveMap[pHeirIdxMap->InputIdx];
        } else {
            //When OctaveMode = 0, Hier0/1/2/3/4/5 will generate actual Hier0/1/2/3/4/5 result sequentially
            pHeirIdxMap->OutputIdx = HL_VprocHierIdHalfOctaveMap[pHeirIdxMap->InputIdx];
        }
    }

    if (Rval == DSP_ERR_NONE) {
        Rval = DSP_ERR_0004; // internal event
    }

    if (ErrLine != 0U) {
        AmbaLL_LogUInt5("Vproc Hier Index Map Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 VideoDataReadyPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    AMBA_DSP_ENC_PIC_RDY_s *pEncPicInfo;
    CTX_STREAM_INFO_s StrmInfo = {0};
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT8 CapSeqQIdx;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
#ifdef USE_TIME_MANAGER
    UINT64 InTimeU64, OutTimeU64 = 0ULL;
#endif

    dsp_osal_typecast(&pEncPicInfo, &pEventData);
    if (pEncPicInfo->StreamId < AMBA_DSP_MAX_STREAM_NUM) {
        /* Ignore VideoEncodeEvent on TestEncStrm */
        HL_GetResourcePtr(&Resource);
        if (pEncPicInfo->StreamId == Resource->TestEncStrmId) {
            AMBA_DSP_EVENT_TESTFRAME_INFO_s *pTfInfo;
            ULONG EventBufAddrTfEnc = 0U;
            UINT16 EventBufIdxTfEnc = 0U;

#ifdef DEBUG_TESTFRAME_STATUS
            AmbaLL_Log(AMBALLLOG_TYPE_HYBRID,"VdoDataRdy TestEnc strm:%u fn:%u type:%u",
                    pEncPicInfo->StreamId, pEncPicInfo->FrmNo, pEncPicInfo->FrameType);
#endif

            Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddrTfEnc, &EventBufIdxTfEnc, LL_EVENT_TESTFRAME_RDY);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == OK) {
                dsp_osal_typecast(&pTfInfo, &EventBufAddrTfEnc);
                Rval = dsp_osal_memset(pTfInfo, 0, sizeof(AMBA_DSP_EVENT_TESTFRAME_INFO_s));
                LL_PrintErrLine(Rval, __func__, __LINE__);

                pTfInfo->Stage = DSP_TEST_STAGE_VDSP_0;
                pTfInfo->Count = 1;
                Rval = DSP_GiveEvent(LL_EVENT_TESTFRAME_RDY, pTfInfo, EventBufIdxTfEnc);
                LL_PrintErrLine(Rval, __func__, __LINE__);
            }
            Rval = DSP_ERR_0001;
        } else {
            // Cancel this event if it is dummy frame
            if (pEncPicInfo->CaptureTimeStamp != DSP_ENC_DUMMY_MARK) {
                HL_GetStrmInfo(HL_MTX_OPT_ALL, pEncPicInfo->StreamId, &StrmInfo);

                if ((pEncPicInfo->FrameType == PIC_FRAME_JPG) &&
                    (StrmInfo.IsMJPG == 0U)) {
                    Rval = DSP_ERR_0001;
                } else {
                    pEncPicInfo->Pts = (UINT64)pEncPicInfo->FrmNo*(UINT64)StrmInfo.FrameRate.NumUnitsInTick;
                }
                // Update CaptureTimeStamp and EncodeTimeStamp
                if ((pEncPicInfo->FrameType != PIC_FRAME_JPG) &&
                    (StrmInfo.SourceYuvStrmId != ENC_STRM_EXTMEM_IDX)) {
                    UINT32 CapSeqU32, CapPtsU32, EncPtsU32;
                    UINT64 CapPtsU64;
                    HL_GetYuvStrmInfoPtr(StrmInfo.SourceYuvStrmId, &YuvStrmInfo);
                    HL_GetViewZoneInfoPtr(YuvStrmInfo->Layout.ChanCfg[0].ViewZoneId, &ViewZoneInfo);

                    // Bit[63:32]: CaptureSeq Bit[31:0]: CapturePts
                    CapSeqU32 = DSP_GetU64Msb(pEncPicInfo->CaptureTimeStamp);
                    CapPtsU32 = DSP_GetU64Lsb(pEncPicInfo->CaptureTimeStamp);
                    EncPtsU32 = DSP_GetU64Lsb(pEncPicInfo->EncodeTimeStamp);
                    CapPtsU64 = (UINT64)CapPtsU32;

                    // Replace U64 RawSeq/RawPts from ViewZone yuv input control
                    // Using CapSeqNo to find index
                    CapSeqQIdx = (UINT8)(CapSeqU32 % MAX_RAW_SEQ_Q);
                    if (ViewZoneInfo->YuvInput.RawCapSeq[CapSeqQIdx] == CapSeqU32) {
                        CapPtsU64 = ViewZoneInfo->YuvInput.RawCapPts64[CapSeqQIdx];
                        CapPtsU32 = ViewZoneInfo->YuvInput.RawCapPts[CapSeqQIdx];
                    }

                    if (StrmInfo.PicRdyInfoOpt == DSP_ENC_CAPTS_OPT_SEQ_TIME) {
                        // CaptureTimeStamp = Bit[63:32]: CaptureSeq Bit[31:0]: CapturePts
                        pEncPicInfo->CaptureTimeStamp = ((UINT64)CapSeqU32 << 32) + CapPtsU32;

#ifndef USE_TIME_MANAGER
                        if (CapPtsU32 > EncPtsU32) {
                            pEncPicInfo->EncodeTimeStamp += 0x100000000ULL;
                        }
#else
                        /* EncDonePts */
                        InTimeU64 = EncPtsU32;
                        (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
                        pEncPicInfo->EncodeTimeStamp = OutTimeU64;
#endif
                    } else {
                        // CaptureTimeStamp = CapturePtsU64
#ifndef USE_TIME_MANAGER
                        pEncPicInfo->CaptureTimeStamp = CapPtsU64;
                        pEncPicInfo->EncodeTimeStamp += (CapPtsU64 & 0xFFFFFFFF00000000ULL);
                        if (CapPtsU32 > EncPtsU32) {
                            pEncPicInfo->EncodeTimeStamp += 0x100000000ULL;
                        }
#else
(void)CapPtsU64;
                        /* CapDonePts */
                        InTimeU64 = CapPtsU32;
                        (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
                        pEncPicInfo->CaptureTimeStamp = OutTimeU64;

                        /* EncDonePts */
                        InTimeU64 = EncPtsU32;
                        (void)HL_TimeMngrGetTime(DSP_TIME_ID_AUDIO_TICK, InTimeU64, &OutTimeU64);
                        pEncPicInfo->EncodeTimeStamp = OutTimeU64;
#endif
                    }
                }
            } else {
                Rval = DSP_ERR_0004;
            }
        }
    } else {
        Rval = DSP_ERR_0001;
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if ((Rval != DSP_ERR_NONE) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("Video Data ready Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

#ifdef SUPPORT_DSP_MV_DUMP
static UINT32 VideoMvDataReadyPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    AMBA_DSP_ENC_MV_RDY_s *pMvInfo;
    CTX_STREAM_INFO_s StrmInfo = {0};
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_VIN_INFO_s VinInfo = {0};
    UINT16 VinId = 0U;
    UINT8 CapSeqQIdx;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    dsp_osal_typecast(&pMvInfo, &pEventData);
    if (pMvInfo->StreamId < AMBA_DSP_MAX_STREAM_NUM) {
        HL_GetResourcePtr(&Resource);
        if (pMvInfo->StreamId == Resource->TestEncStrmId) {
            /* Ignore VideoEncodeEvent on TestEncStrm */
            Rval = DSP_ERR_0001;
        } else {
            HL_GetStrmInfo(HL_MTX_OPT_ALL, pMvInfo->StreamId, &StrmInfo);
            if (StrmInfo.SourceYuvStrmId != ENC_STRM_EXTMEM_IDX) {
                HL_GetYuvStrmInfoPtr(StrmInfo.SourceYuvStrmId, &YuvStrmInfo);
                if (YuvStrmInfo->Layout.NumChan > 0U) {
                    Rval = HL_GetViewZoneVinId(YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId, &VinId);
                    DSP_FillErrline(Rval, &ErrLine, __LINE__);

                    /* AVC MV need buffer information */
                    if (StrmInfo.CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264) {
                        Rval = AmbaDSP_CalcEncAvcMvBuf(StrmInfo.Window.Width, StrmInfo.Window.Height, &pMvInfo->MvBufPitch, &pMvInfo->MvBufWidth, &pMvInfo->MvBufHeight);
                        DSP_FillErrline(Rval, &ErrLine, __LINE__);
                    }

                    if (VinId < DSP_VIN_MAX_NUM) {
                        // Replace U64 RawSeq/RawPts
                        HL_GetVinInfo(HL_MTX_OPT_ALL, VinId, &VinInfo);
                        // Using CapSeqNo to find index
                        CapSeqQIdx = (UINT8)(pMvInfo->CapSequence % MAX_RAW_SEQ_Q);
                        if (VinInfo.VinCtrl.RawCapSeq[CapSeqQIdx] == (UINT32)pMvInfo->CapSequence) {
                            pMvInfo->CapSequence = VinInfo.VinCtrl.RawCapSeq64[CapSeqQIdx];
                            pMvInfo->CapPts = VinInfo.VinCtrl.RawCapPts64[CapSeqQIdx];
                        }
                    }
                }
            }
        }
    } else {
        Rval = DSP_ERR_0001;
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if ((Rval != DSP_ERR_NONE) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("Video Mv data ready Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}
#endif

static UINT32 VideoEncodeStartPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    const AMBA_DSP_EVENT_VIDEO_ENC_STATUS_s *pEncStatusInfo;
    CTX_ENCODER_INFO_s EncodeInfo = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    dsp_osal_typecast(&pEncStatusInfo, &pEventData);

    // Set active bit
    if (pEncStatusInfo->StreamId < AMBA_DSP_MAX_STREAM_NUM) {
        HL_GetEncoderInfo(HL_MTX_OPT_GET, &EncodeInfo);
        DSP_SetU16Bit(&EncodeInfo.ActiveBit, pEncStatusInfo->StreamId);
        HL_SetEncoderInfo(HL_MTX_OPT_SET, &EncodeInfo);

        HL_GetResourcePtr(&Resource);
        if (pEncStatusInfo->StreamId == Resource->TestEncStrmId) {
            AMBA_DSP_EVENT_TESTFRAME_INFO_s *pTfInfo;
            ULONG EventBufAddrTfEnc = 0U;
            UINT16 EventBufIdxTfEnc = 0U;

            Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddrTfEnc, &EventBufIdxTfEnc, LL_EVENT_TESTFRAME_RDY);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == OK) {
                dsp_osal_typecast(&pTfInfo, &EventBufAddrTfEnc);
                Rval = dsp_osal_memset(pTfInfo, 0, sizeof(AMBA_DSP_EVENT_TESTFRAME_INFO_s));
                LL_PrintErrLine(Rval, __func__, __LINE__);

                pTfInfo->Stage = DSP_TEST_STAGE_VDSP_0;
                pTfInfo->Count = 1;
                Rval = DSP_GiveEvent(LL_EVENT_TESTFRAME_RDY, pTfInfo, EventBufIdxTfEnc);
                LL_PrintErrLine(Rval, __func__, __LINE__);
            }
            /* Ignore VideoEncodeEvent on TestEncStrm */
            Rval = DSP_ERR_0001;
#ifdef DEBUG_TESTFRAME_STATUS
            AmbaLL_LogUInt5("TestEncStrm start %u", pEncStatusInfo->StreamId, 0U, 0U, 0U, 0U);
#endif
        }
    } else {
        Rval = DSP_ERR_0001;
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if ((Rval != DSP_ERR_NONE) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("Video encode start Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 VideoEncodeStopPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    const AMBA_DSP_EVENT_VIDEO_ENC_STATUS_s *pEncStatusInfo;
    CTX_ENCODER_INFO_s EncodeInfo = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    dsp_osal_typecast(&pEncStatusInfo, &pEventData);

    // Clear active bit
    if (pEncStatusInfo->StreamId < AMBA_DSP_MAX_STREAM_NUM) {
        HL_GetEncoderInfo(HL_MTX_OPT_GET, &EncodeInfo);
        DSP_ClearU16Bit(&EncodeInfo.ActiveBit, pEncStatusInfo->StreamId);
        HL_SetEncoderInfo(HL_MTX_OPT_SET, &EncodeInfo);

        /* Ignore VideoEncodeEvent on TestEncStrm */
        HL_GetResourcePtr(&Resource);
        if (pEncStatusInfo->StreamId == Resource->TestEncStrmId) {
            Rval = DSP_ERR_0001;
        }
    } else {
        Rval = DSP_ERR_0001;
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if ((Rval != DSP_ERR_NONE) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("Video encode stop Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static void StillRawReadyDataCapCountDown(UINT32 *CountDown)
{
    if ((*CountDown > 0U) && (*CountDown != DSP_DATACAP_INFINITE_CAP_NUM)) {
        (*CountDown)--;
    }
}

static inline UINT32 StillRawReadyPreProcFuncSrcVin(const UINT16 StlCapVinId,
                                                    AMBA_DSP_STL_RAW_DATA_INFO_s *pStlRawData,
                                                    UINT32 *pErrLine)
{
    UINT32 Rval, ErrLine = *pErrLine;
    UINT16 i;

    Rval = DSP_ERR_0001;
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    for (i=0U; i<AMBA_DSP_MAX_DATACAP_NUM; i++) {
        CTX_DATACAP_INFO_s DataCapInfo;

        HL_GetDataCapInfo(HL_MTX_OPT_ALL, i, &DataCapInfo);
        if ((DataCapInfo.Status == DATA_CAP_STATUS_RUNNING) ||
            (DataCapInfo.Status == DATA_CAP_STATUS_2_STOP)) {
            if ((DataCapInfo.Cfg.CapDataType == DSP_DATACAP_TYPE_RAW) &&
                (DataCapInfo.Cfg.Index == StlCapVinId)) {
                UINT8 CapSeqQIdx;
                CTX_VIN_INFO_s VinInfo = {0};

                pStlRawData->CapInstance = i;

                HL_GetDataCapInfo(HL_MTX_OPT_GET, i, &DataCapInfo);
                StillRawReadyDataCapCountDown(&DataCapInfo.CountDown);
                if ((DataCapInfo.CountDown <= 1U) && (DataCapInfo.Status == DATA_CAP_STATUS_RUNNING)) {
                    DataCapInfo.Status = DATA_CAP_STATUS_2_STOP;
                    /* (1) If the user-provided task for msg-parsing has low priority, then this command
                     *     could be delayed and produce redundant raw in the internal buffer
                     * (2) Issue VinIdle one-frame earlier to avoid an redundant raw reuse the buffer */
                    if (DataCapInfo.VprocIdle == 1U) {
                        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                        cmd_vin_idle_t *VinIdle = HL_DefCtxCmdBufPtrVinIdle;

                        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                        dsp_osal_typecast(&VinIdle, &CmdBufferAddr);
                        VinIdle->vin_id = (UINT8)(StlCapVinId);
                        Rval = AmbaHL_CmdVinIdle(HL_GetVin2CmdNormalWrite(StlCapVinId), VinIdle);
                        DSP_FillErrline(Rval, &ErrLine, __LINE__);
                        HL_RelCmdBuffer(CmdBufferId);
                    }
                }
                if ((DataCapInfo.CountDown == 0U) && (DataCapInfo.Status == DATA_CAP_STATUS_2_STOP)) {
                    DataCapInfo.Status = DATA_CAP_STATUS_IDLE;
                }
                HL_SetDataCapInfo(HL_MTX_OPT_SET, i, &DataCapInfo);

                // Replace U64 RawSeq/RawPts
                HL_GetVinInfo(HL_MTX_OPT_ALL, (UINT16)StlCapVinId, &VinInfo);
                // Using CapSeqNo to find index
                CapSeqQIdx = (UINT8)(pStlRawData->CapSequence % MAX_RAW_SEQ_Q);
                if (VinInfo.VinCtrl.RawCapSeq[CapSeqQIdx] == pStlRawData->CapSequence) {
                    pStlRawData->CapSequence = VinInfo.VinCtrl.RawCapSeq64[CapSeqQIdx];
                    pStlRawData->CapPts = VinInfo.VinCtrl.RawCapPts64[CapSeqQIdx];
                }
                Rval = DSP_ERR_NONE; ErrLine = 0U;
                break;
            }
        }
    }
    if (Rval != DSP_ERR_NONE) {
        UINT8 IsVirtVin = 0U;

        (void)HL_IsVirtualVinIdx(StlCapVinId, &IsVirtVin);
        if (IsVirtVin == 1U) {
            //something from virtual vin, handle this event in future if needed.
            //discard this event, doesn't need to report error code
            Rval = DSP_ERR_0004;
        }
    }
    *pErrLine = ErrLine;
    return Rval;
}

static UINT32 StillRawReadyPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    DSP_STL_RAW_CAP_BUF_s *pStlCapInfo;
    AMBA_DSP_STL_RAW_DATA_INFO_s *pStlRawData;
    CTX_DATACAP_INFO_s DataCapInfo;

    dsp_osal_typecast(&pStlCapInfo, &pEventData);
    pStlRawData = &pStlCapInfo->StlRawData;

    if (pStlCapInfo->CapSrcType == CAP_SRC_TYPE_VIN) {
        Rval = StillRawReadyPreProcFuncSrcVin(pStlCapInfo->VinId, pStlRawData, &ErrLine);
    } else if (pStlCapInfo->CapSrcType == CAP_SRC_TYPE_VPROC) {
        //SrcTypeVproc already fill every thing when invoke this PreProcFunc
        HL_GetDataCapInfo(HL_MTX_OPT_GET, pStlRawData->CapInstance, &DataCapInfo);
        if ((DataCapInfo.Status == DATA_CAP_STATUS_RUNNING) ||
            (DataCapInfo.Status == DATA_CAP_STATUS_2_STOP)) {
            StillRawReadyDataCapCountDown(&DataCapInfo.CountDown);
            if (DataCapInfo.CountDown == 0U) {
                DataCapInfo.Status = DATA_CAP_STATUS_IDLE;
            }
        }
        HL_SetDataCapInfo(HL_MTX_OPT_SET, pStlRawData->CapInstance, &DataCapInfo);
    } else {
        //Not support yet
    }

    if ((Rval != DSP_ERR_NONE) && (Rval != DSP_ERR_0004) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("Still Raw ready Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 StillRawPostProcReadyPreProcFunc(const void *pEventData)
{
    const AMBA_DSP_RAW_DATA_RDY_s *pStlRawData;

    dsp_osal_typecast(&pStlRawData, &pEventData);

    return 0;
}

static UINT32 StillDataReadyPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    const AMBA_DSP_ENC_PIC_RDY_s *pEncPicInfo;
    CTX_STREAM_INFO_s StrmInfo;

    dsp_osal_typecast(&pEncPicInfo, &pEventData);
    if (pEncPicInfo->StreamId < AMBA_DSP_MAX_STREAM_NUM) {
        HL_GetStrmInfo(HL_MTX_OPT_ALL, pEncPicInfo->StreamId, &StrmInfo);

        if (StrmInfo.IsMJPG == 1U) {
            Rval = DSP_ERR_0001;
        }
    } else {
        Rval = DSP_ERR_0001;
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if ((Rval != DSP_ERR_NONE) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("Still data ready Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 VideoDecodeReadyPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;
    AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s *pDecStatusInfo;
    CTX_DECODER_INFO_s DecoderInfo = {0};
    static UINT32 EosSent = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);

    if (pEventData != NULL) {
        dsp_osal_typecast(&pDecStatusInfo, &pEventData);

        //dsp_osal_printU5("VideoDecodeReadyPreProcFunc ## eos:%d st:%d es:%d", pDecStatusInfo->EndOfStream, pDecStatusInfo->DecState, EosSent, 0U, 0U);
        if ((UINT16)pDecStatusInfo->DecoderId != Resource->TestDecStrmId) {
            HL_GetDecoderInfo(HL_MTX_OPT_GET, &DecoderInfo);
            if (0U == pDecStatusInfo->EndOfStream) {
                DecoderInfo.DecoderStatus = pDecStatusInfo->DecState;
                DecoderInfo.DecoderID = (UINT8)pDecStatusInfo->DecoderId;
                DecoderInfo.BitsBufAddrNext = pDecStatusInfo->BitsNextReadAddr;
                DecoderInfo.DecPicNum = pDecStatusInfo->NumOfDecodedPic;
                DecoderInfo.DisPicNum = pDecStatusInfo->NumOfDisplayPic;
                DecoderInfo.DisplayPTS = pDecStatusInfo->DisplayFramePTS;

                if (DSP_DEC_OPM_RUN == pDecStatusInfo->DecState) {
                    //Give Normal
                    EosSent = 0U;
                }
            } else {
                if (EosSent == 0U) {
                    pDecStatusInfo->EndOfStream = 1U;
                    pDecStatusInfo->DecState = DecoderInfo.DecoderStatus;
                    pDecStatusInfo->DecoderId = DecoderInfo.DecoderID;
                    pDecStatusInfo->BitsNextReadAddr = DecoderInfo.BitsBufAddrNext;
                    pDecStatusInfo->NumOfDecodedPic = DecoderInfo.DecPicNum;
                    pDecStatusInfo->NumOfDisplayPic = DecoderInfo.DisPicNum;
                    pDecStatusInfo->DisplayFramePTS = DecoderInfo.DisplayPTS;

                    //Give EOS
                    EosSent = 1U;
                } else {
                    //Already sent EOS do nothing
                }
            }
            HL_SetDecoderInfo(HL_MTX_OPT_SET, &DecoderInfo);
        } else {
            AMBA_DSP_EVENT_TESTFRAME_INFO_s *pTfInfo;
            ULONG EventBufAddrTfDec = 0U;
            UINT16 EventBufIdxTfDec = 0U;

            Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddrTfDec, &EventBufIdxTfDec, LL_EVENT_TESTFRAME_RDY);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == OK) {
                dsp_osal_typecast(&pTfInfo, &EventBufAddrTfDec);
                Rval = dsp_osal_memset(pTfInfo, 0, sizeof(AMBA_DSP_EVENT_TESTFRAME_INFO_s));
                LL_PrintErrLine(Rval, __func__, __LINE__);

                pTfInfo->Stage = DSP_TEST_STAGE_VDSP_1;
                pTfInfo->Count = 1;
                Rval = DSP_GiveEvent(LL_EVENT_TESTFRAME_RDY, pTfInfo, EventBufIdxTfDec);
                LL_PrintErrLine(Rval, __func__, __LINE__);
            }
        }
        if ((UINT16)pDecStatusInfo->DecoderId == Resource->TestDecStrmId) {
            //skip this event
            Rval = DSP_ERR_0001;
        }
    } else {
        Rval = DSP_ERR_0001;
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if ((Rval != DSP_ERR_NONE) && (ErrLine != 0U)) {
        AmbaLL_LogUInt5("Video decode ready Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 VideoDecodePicPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE;
    const AMBA_LL_EVENT_VIDEO_DEC_PIC_INFO_s *pDecPicpInfo;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    dsp_osal_typecast(&pDecPicpInfo, &pEventData);
    HL_GetVidDecInfo(HL_MTX_OPT_GET, (UINT16)pDecPicpInfo->PicInfo.DecoderId, &VidDecInfo);
    VidDecInfo.LastYuvBuf.DataFmt = pDecPicpInfo->PicInfo.DataFmt;
    VidDecInfo.LastYuvBuf.BaseAddrY = pDecPicpInfo->PicInfo.YAddr;
    VidDecInfo.LastYuvBuf.BaseAddrUV = pDecPicpInfo->PicInfo.UVAddr;
    VidDecInfo.LastYuvBuf.Window.Width = (UINT16)pDecPicpInfo->PicInfo.Width;
    VidDecInfo.LastYuvBuf.Window.Height = (UINT16)pDecPicpInfo->PicInfo.Height;
    VidDecInfo.LastYuvBuf.Pitch = (UINT16)pDecPicpInfo->PicInfo.Pitch;
    VidDecInfo.LastYuvBufWidth = pDecPicpInfo->BufferWidth;
    VidDecInfo.LastYuvBufHeight = pDecPicpInfo->BufferHeight;
    HL_SetVidDecInfo(HL_MTX_OPT_SET, (UINT16)pDecPicpInfo->PicInfo.DecoderId, &VidDecInfo);

    HL_GetResourcePtr(&Resource);
    if ((UINT16)pDecPicpInfo->PicInfo.DecoderId == Resource->TestDecStrmId) {
        //skip this event
        Rval = DSP_ERR_0001;
    }

    return Rval;
}

static UINT32 StillDecodeReadyPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE;
(void)pEventData;
//    const AMBA_DSP_STLDEC_STATUS_s *pDecStatus;
//    dsp_osal_typecast(&pDecStatus, &pEventData);

    return Rval;
}

static UINT32 StillDecodeDispPreProcFunc(const void *pEventData)
{
    UINT32 Rval = DSP_ERR_NONE;
    AMBA_DSP_VIDEO_DEC_YUV_DISP_REPORT_s *pDecDispInfo;
    CTX_VID_DEC_INFO_s DecInfo = {0};
    UINT32 IsAddrYValid, IsAddrUVValid;

    dsp_osal_typecast(&pDecDispInfo, &pEventData);
    IsAddrYValid = IsValidULAddr(pDecDispInfo->YAddr);
    IsAddrUVValid = IsValidULAddr(pDecDispInfo->UVAddr);

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, 0U, &DecInfo);
    if ((IsAddrYValid == 1U) && (IsAddrUVValid == 1U)) {
        if (DecInfo.DecoderMode == DECODE_MODE_STILL) {
            pDecDispInfo->Width = DecInfo.PostCtlCfg[pDecDispInfo->VoutIdx].VoutWindow.Width;
            pDecDispInfo->Height = DecInfo.PostCtlCfg[pDecDispInfo->VoutIdx].VoutWindow.Height;
            pDecDispInfo->Pitch = pDecDispInfo->Width;
        } else {
            Rval = DSP_ERR_0001;
        }
    } else {
        Rval = DSP_ERR_0001;
    }
    return Rval;
}

static UINT32 HL_EventInitExternalLV(void)
{
    static AMBA_DSP_EVENT_HANDLER_f LiveviewRawReadyPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f LiveviewCfaReadyPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f LiveviewPGReadyPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f LiveviewHistReadyPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f LiveviewYuvReadyPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f LiveviewPymdReadyPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f LiveviewSyncJobReadyPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f LiveviewLndtReadyPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f LiveviewMainY12ReadyPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f TestFrmPreProc[MAX_PREPROC_HANDLER];

    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    LiveviewRawReadyPreProc[0] = LiveviewRawReadyPreProcFunc;
    Rval = DSP_EventHandlerConfig(LL_EVENT_LV_RAW_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, LiveviewRawReadyPreProc);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    if (Rval == DSP_ERR_NONE) {
        LiveviewCfaReadyPreProc[0] = LiveviewCfaReadyPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_LV_CFA_AAA_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, LiveviewCfaReadyPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        LiveviewPGReadyPreProc[0] = LiveviewPGReadyPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_LV_PG_AAA_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, LiveviewPGReadyPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        LiveviewHistReadyPreProc[0] = LiveviewHistReadyPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_LV_HIST_AAA_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, LiveviewHistReadyPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        LiveviewYuvReadyPreProc[0] = LiveviewYuvReadyPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_LV_YUV_DATA_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, LiveviewYuvReadyPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        LiveviewPymdReadyPreProc[0] = LiveviewPymdReadyPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_LV_PYRAMID_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, LiveviewPymdReadyPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        LiveviewSyncJobReadyPreProc[0] = LiveviewSyncJobReadyPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_LV_SYNC_JOB_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, LiveviewSyncJobReadyPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        LiveviewLndtReadyPreProc[0] = LiveviewLndtReadyPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_LV_LNDT_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, LiveviewLndtReadyPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        LiveviewMainY12ReadyPreProc[0] = LiveviewMainY12ReadyPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_LV_MAIN_Y12_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, LiveviewMainY12ReadyPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        TestFrmPreProc[0] = TestFrmPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_TESTFRAME_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, TestFrmPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("Init External Liveview Event Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 HL_EventInitExternalStill(void)
{
    static AMBA_DSP_EVENT_HANDLER_f StillRawReadyPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f StillRawCfaPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f StillRawPGPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f StillRawPostPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f StillRawYuvPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f StillDataReadyPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f StillDecodePreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f StillDecodeDispPreProc[MAX_PREPROC_HANDLER];
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    StillRawReadyPreProc[0] = StillRawReadyPreProcFunc;
    Rval = DSP_EventHandlerConfig(LL_EVENT_STL_RAW_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, StillRawReadyPreProc);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    if (Rval == DSP_ERR_NONE) {
        StillRawCfaPreProc[0] = NULL;
        Rval = DSP_EventHandlerConfig(LL_EVENT_STL_CFA_AAA_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, StillRawCfaPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        StillRawPGPreProc[0] = NULL;
        Rval = DSP_EventHandlerConfig(LL_EVENT_STL_PG_AAA_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, StillRawPGPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        StillRawPostPreProc[0] = StillRawPostProcReadyPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_STL_RAW_POSTPROC_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, StillRawPostPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        StillRawYuvPreProc[0] = NULL;
        Rval = DSP_EventHandlerConfig(LL_EVENT_STL_YUV_DATA_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, StillRawYuvPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        StillDataReadyPreProc[0] = StillDataReadyPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_JPEG_DATA_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, StillDataReadyPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        StillDecodePreProc[0] = StillDecodeReadyPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_STILL_DEC_STATUS, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, StillDecodePreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        StillDecodeDispPreProc[0] = StillDecodeDispPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_STILL_DEC_YUV_DISP_REPORT, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, StillDecodeDispPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("Init External Still Event Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 HL_EventInitExternal(void)
{
    static AMBA_DSP_EVENT_HANDLER_f VoutDataReadyPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f VideoDataReadyPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f VideoEncodeStartPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f VideoEncodeStopPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f VideoDataPathPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f VideoDecodePreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f VideoDecodePicPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f VinPostCfgPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f DspErrorPreProc[MAX_PREPROC_HANDLER];
#ifdef SUPPORT_DSP_MV_DUMP
    static AMBA_DSP_EVENT_HANDLER_f VideoMvDataReadyPreProc[MAX_PREPROC_HANDLER];
#endif
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    Rval = HL_EventInitExternalLV();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    if (Rval == DSP_ERR_NONE) {
        Rval = HL_EventInitExternalStill();
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        VoutDataReadyPreProc[0] = LiveviewVoutReadyPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_VOUT_DATA_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, VoutDataReadyPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        VideoDataReadyPreProc[0] = VideoDataReadyPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_VIDEO_DATA_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, VideoDataReadyPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        VideoEncodeStartPreProc[0] = VideoEncodeStartPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_VIDEO_ENC_START, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, VideoEncodeStartPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        VideoEncodeStopPreProc[0] = VideoEncodeStopPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_VIDEO_ENC_STOP, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, VideoEncodeStopPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        VideoDataPathPreProc[0] = VideoDataPathPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_VIDEO_PATH_STATUS, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, VideoDataPathPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        VideoDecodePreProc[0] = VideoDecodeReadyPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_VIDEO_DEC_STATUS, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, VideoDecodePreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        VideoDecodePicPreProc[0] = VideoDecodePicPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_VIDEO_DEC_PIC_INFO, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, VideoDecodePicPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        VinPostCfgPreProc[0] = NULL;
        Rval = DSP_EventHandlerConfig(LL_EVENT_VIN_POST_CONFIG, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, VinPostCfgPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        DspErrorPreProc[0] = DspErrorPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_ERROR, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, DspErrorPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

#ifdef SUPPORT_DSP_MV_DUMP
    if (Rval == DSP_ERR_NONE) {
        VideoMvDataReadyPreProc[0] = VideoMvDataReadyPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_VIDEO_MV_DATA_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, VideoMvDataReadyPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
#endif

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("Init External Event Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 HL_EventInitInternal(void)
{
    /* Internal */
//FIXME, srcbuf mapping
//    static AMBA_DSP_EVENT_HANDLER_f YuvSrcBufInfoPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f VprocIdspDataPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f VinSectionCfgDataPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f VprocEffectYuvDataPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f VprocCfaModeMapPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f VprocHierIdxMapPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f BatchInfoMapPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f EncGetActiveBitPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f DecGetBitsFormatPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f DecUpdateStatusPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f SysGetClkInfoPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f DspSimOpPreProc[MAX_PREPROC_HANDLER];
    static AMBA_DSP_EVENT_HANDLER_f TimeTickUptPreProc[MAX_PREPROC_HANDLER];
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;
    /* Internal */
//        YuvSrcBufInfoPreProc[0] = YuvSrcBufInfoPreProcFunc;
//        (void)DSP_EventHandlerConfig(LL_EVENT_YUV_SRC_BUF_INFO, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, YuvSrcBufInfoPreProc);
    VprocIdspDataPreProc[0] = VprocIdspDataPreProcFunc;
    Rval = DSP_EventHandlerConfig(LL_EVENT_VPROC_IDSP_DATA_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, VprocIdspDataPreProc);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    if (Rval == DSP_ERR_NONE) {
        VinSectionCfgDataPreProc[0] = VinSectionCfgDataPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_VIN_SECTION_CFG_DATA_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, VinSectionCfgDataPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        VprocEffectYuvDataPreProc[0] = VprocEffectYuvDataPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_EFCT_YUV_DATA_RDY, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, VprocEffectYuvDataPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        VprocCfaModeMapPreProc[0] = VprocCfaModeMapPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_CFA_MODE_MAPPING, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, VprocCfaModeMapPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        VprocHierIdxMapPreProc[0] = VprocHierIdxMapPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_HIER_IDX_MAPPING, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, VprocHierIdxMapPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        BatchInfoMapPreProc[0] = BatchInfoMapPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_BATCH_INFO_MAPPING, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, BatchInfoMapPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        EncGetActiveBitPreProc[0] = EncGetActiveBitPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_ENC_GET_ACTIVE_BIT, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, EncGetActiveBitPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        DecGetBitsFormatPreProc[0] = DecGetBitsFormatPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_DEC_GET_BITSTRM_FORMAT, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, DecGetBitsFormatPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        DecUpdateStatusPreProc[0] = DecUpdateStatusPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_DEC_UPDATE_STATUS, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, DecUpdateStatusPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        SysGetClkInfoPreProc[0] = SysGetClkInfoPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_SYS_GET_CLK_INFO, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, SysGetClkInfoPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        DspSimOpPreProc[0] = DspSimOpPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_SIM_OP, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, DspSimOpPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval == DSP_ERR_NONE) {
        TimeTickUptPreProc[0] = TimeTickUptPreProcFunc;
        Rval = DSP_EventHandlerConfig(LL_EVENT_TIME_TICK_UPDATE, LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, TimeTickUptPreProc);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("Init Internal Event Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 HL_EventInit(void)
{
    static UINT8 HL_EventInitFlag = 0U;
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;


    if (HL_EventInitFlag == 0U) {
        Rval = DSP_EventInit();
        DSP_FillErrline(Rval, &ErrLine, __LINE__);

        if (Rval == DSP_ERR_NONE) {
            Rval = HL_EventInitExternal();
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }

        if (Rval == DSP_ERR_NONE) {
            Rval = HL_EventInitInternal();
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }

        if (Rval == DSP_ERR_NONE) {
            HL_EventInitFlag = 1U;
        }
    }

#if 0
    //Hook SSP default event handler

    VcapLockVideoBufHdlr[0] = VcapLockVideoBufFunction;
    DSP_EventHandlerConfig(LL_EVENT_VCAP_LOCK_VIDEO_BUF, AMBA_LL_EVENT_HANDLER, MAX_PREPROC_HANDLER, VcapLockVideoBufHdlr);
    VcapScaleYuvHdlr[0] = VcapScaleYuvFunction;
    DSP_EventHandlerConfig(LL_EVENT_VCAP_SCALE_YUV, AMBA_LL_EVENT_HANDLER, MAX_PREPROC_HANDLER, VcapScaleYuvHdlr);

    MemCreateFrmBufPool[0] = MemCreateFrmBufPoolFunction;
    DSP_EventHandlerConfig(LL_EVENT_MEM_CREATE_FRM_BUF_POOL, AMBA_LL_EVENT_HANDLER, MAX_PREPROC_HANDLER, MemmCreateFrmBufPool);

    MemRequestFrmBuf[0] = MemmRequestFrmBufFunction;
    DSP_EventHandlerConfig(LL_EVENT_MEM_REQ_FRM_BUF, AMBA_LL_EVENT_HANDLER, MAX_PREPROC_HANDLER, MemmReqFrmBuf);

    VideoDataTransferHdlr[0] = VideoDataReadyTransferFunction;
    DSP_EventHandlerConfig(LL_EVENT_VIDEO_DATA_TRANSFER_READY, AMBA_LL_EVENT_HANDLER, MAX_PREPROC_HANDLER, VideoDataTransferHdlr);

    DspIsrCheckHdlr[0] = DspIsrCheckFunction;
    DSP_EventHandlerConfig(LL_EVENT_DSP_ISR_CHECK, AMBA_LL_EVENT_HANDLER, MAX_PREPROC_HANDLER, DspIsrCheckHdlr);

    DspFirstVdoDataRdyHdlr[0] = DspFirstVideoDataReadyFunction;
    DSP_EventHandlerConfig(LL_EVENT_FIRST_VIDEO_DATA_READY, AMBA_LL_EVENT_HANDLER, MAX_PREPROC_HANDLER, DspFirstVdoDataRdyHdlr);

    DspVdoPathTransferHdlr[0] = DspVideoPathTransferFunction;
    DSP_EventHandlerConfig(LL_EVENT_VDO_PATH_STATUS_TRANSFER_READY, AMBA_LL_EVENT_HANDLER, MAX_PREPROC_HANDLER, DspVdoPathTransferHdlr);

    VideoDataPtsTracePreProcHdlr[0] = VideoDataPtsTracePreProcFunction;
    DSP_EventHandlerConfig(LL_EVENT_VIDEO_PTS_TRACE, AMBA_LL_EVENT_HANDLER_PRE_PROC, MAX_PREPROC_HANDLER, VideoDataPtsTracePreProcHdlr);

#endif
    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("Init Event Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

/**
* DSP event control reset function
* @param [in]  EventID event index
* @return ErrorCode
*/
UINT32 dsp_event_hdlr_ctrl_reset(UINT16 EventID)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    Rval = HL_EventInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* MainProc */
    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_evnt_hdlr_reset(EventID);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }
    /* Release Event Buffer */
    if (Rval == DSP_ERR_NONE) {
        Rval = DSP_ResetEventInfoBuf(EventID);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp event handler control reset Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* DSP event control configuration function
* @param [in]  EventID event index
* @param [in]  MaxNumHandlers max handler
* @param [in]  pEventHandlers event handler array
* @return ErrorCode
*/
UINT32 dsp_event_hdlr_ctrl_cfg(UINT16 EventID, UINT16 MaxNumHandlers, AMBA_DSP_EVENT_HANDLER_f *pEventHandlers)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    Rval = HL_EventInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* MainProc */
    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_evnt_hdlr_cfg(EventID, MaxNumHandlers, pEventHandlers);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp event handler control config Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* DSP event handler register function
* @param [in]  EventID event index
* @param [in]  EventHandler resource setting
* @return ErrorCode
*/
UINT32 dsp_event_hdlr_register(UINT16 EventID, AMBA_DSP_EVENT_HANDLER_f EventHandler)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    Rval = HL_EventInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    /* MainProc */
    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_osal_evnt_hdlr_reg(EventID, 1U/*Reg*/, EventHandler);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp event handler register Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* DSP event handler unregister function
* @param [in]  EventID event index
* @param [in]  EventHandler resource setting
* @return ErrorCode
*/
UINT32 dsp_event_hdlr_unregister(UINT16 EventID, AMBA_DSP_EVENT_HANDLER_f EventHandler)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0U;

    /* MainProc */
    Rval = dsp_osal_evnt_hdlr_reg(EventID, 0U/*Reg*/, EventHandler);
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("dsp event handler unregister Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

#if !defined(CONFIG_THREADX)
UINT32 dsp_event_release_info_pool(UINT16 EvtBufIdx)
{
    UINT32 Rval = DSP_ERR_NONE, ErrLine = 0;

    Rval = HL_EventInit();
    DSP_FillErrline(Rval, &ErrLine, __LINE__);

    if (Rval == DSP_ERR_NONE) {
        Rval = DSP_ReleaseEventInfoBuf(EvtBufIdx);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("Event info buffer release Error[0x%x][%u]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}
#endif

