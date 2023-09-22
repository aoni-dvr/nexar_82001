/**
 *  @file SvcImgFeeder.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *  @details Implementation of Svc Image feeder utility
 *
 */

#include "AmbaTypes.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaNAND.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"
#include "AmbaPrint.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcTask.h"

#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowInputUtil.h"
#include "SvcImgFIFO.h"
#include "SvcImgFeeder.h"

#define SVC_LOG_IMG_FEEDER         "IMG_FD"

#define SVC_IMG_FEEDER_STACK_SIZE                (0x4000U)

/* Ctrl Flag */
#define SVC_IMG_FEEDER_FLAG_ENABLE               (1U)
#define SVC_IMG_FEEDER_FLAG_RUN                  (2U)
#define SVC_IMG_FEEDER_FLAG_PORT_READY           (4U)
#define SVC_IMG_FEEDER_FLAG_DATA_UPDATE          (8U)

//static inline UINT32 SVC_IMG_FEEDER_GET_BIT     (UINT32 Bits, UINT32 Idx) {return (Bits & ((UINT32) 1U << Idx));}
static inline UINT32 SVC_IMG_FEEDER_ADD_BIT     (UINT32 Bits, UINT32 Idx) {return (Bits | ((UINT32)1U << Idx));}
static inline UINT32 SVC_IMG_FEEDER_REMOVE_BIT  (UINT32 Bits, UINT32 Idx) {return (Bits & (~((UINT32)1U << Idx)));}

#define COUNTER_MAX     (0xFFFFFFFFU)

typedef struct {
    UINT8                         Used;
    UINT8                         FIFOChanID;       /* FIFO channel the port lintens to */
} SVC_IMG_FEEDER_SUBPORT_STATUS_s;

typedef struct {
    UINT32                        Opened   :1;
    UINT32                        Connect  :1;
    UINT32                        Reserved :30;
    SVC_IMG_FEEDER_SUBPORT_STATUS_s SubPort[SVC_CV_INPUT_INFO_MAX_DATA_NUM];
} SVC_IMG_FEEDER_PORT_STATUS_s;

typedef struct {
    UINT32                        Registered :1;
    UINT32                        Reserved   :31;
    SVC_IMG_FIFO_CHAN_s           Chan;
    SVC_CV_INPUT_IMG_CONTENT_s    Content;
    UINT32                        PortLintenBits;   /* Ports which linten to the FIFO */
    UINT32                        RdCount;
} SVC_IMG_FEEDER_FIFO_STATUS_s;

typedef UINT32 (*SVC_FEEDER_HANDLER_f)(void *pHandlePtr);

typedef struct {
    UINT32                        FeederID;
    UINT32                        Created  :1;
    UINT32                        RdStInit :1;
    UINT32                        ExtSync  :1;
    UINT32                        Reserved :29;
    UINT8                         Divisor;

    AMBA_KAL_MUTEX_t              Mutex;
    SVC_TASK_CTRL_s               TxTask;
    UINT64                        TaskIdentifier;
    AMBA_KAL_EVENT_FLAG_t         CtrlFlagId;
    SVC_FEEDER_HANDLER_f          pFeederHandler;

    UINT32                        PortEnaBits;      /* Ports which are enabled */
    SVC_IMG_FEEDER_PORT_CFG_s     PortCfg[SVC_IMG_FEEDER_PORT_MAX];
    SVC_IMG_FEEDER_PORT_STATUS_s  Ports[SVC_IMG_FEEDER_PORT_MAX];
    UINT32                        PortRdyFlag;

    UINT32                        FIFOEnaBits;      /* FIFOs which are enabled */
    SVC_IMG_FEEDER_FIFO_STATUS_s  FIFO[SVC_IMG_FIFO_MAX_CHAN];
    UINT32                        DataUpdateFlag;
    UINT32                        DataRdyFlag;
    AMBA_KAL_MUTEX_t              DataStMutex;
} SVC_IMG_FEEDER_HANDLE_s;

static SVC_IMG_FEEDER_HANDLE_s FeederHandle[SVC_IMG_FEEDER_MAX_ID] GNU_SECTION_NOZEROINIT;
static UINT8  FeederDebugEnable = 0U;

static void*  ImgFeeder_TxTaskEntry(void* EntryArg);
static UINT32 ImgFeeder_HandlerSyncPorts(void *pHandlePtr);
static UINT32 ImgFeeder_HandlerSyncCapSeq(void *pHandlePtr);
static UINT32 ImgFeeder_HandlerDivisor(void *pHandlePtr);

static UINT32 ImgFeeder_FIFOCallback(SVC_IMG_FIFO_CHAN_s *pChan, UINT32 Event, void *pInfo);

static UINT32 ImgFeeder_GetNewPortID(SVC_IMG_FEEDER_HANDLE_s *pHandle, UINT32 *pPortID);

static UINT32 ImgFeeder_SearchExistFIFO(SVC_IMG_FEEDER_HANDLE_s *pHandle, SVC_CV_INPUT_IMG_CONTENT_s *pContent,  UINT32 *pFIFOChanID);
static UINT32 ImgFeeder_RegisterFIFO(SVC_IMG_FEEDER_HANDLE_s *pHandle, UINT32 PortID, SVC_CV_INPUT_IMG_CONTENT_s *pContent, UINT32 *pFIFOChanID);
static UINT32 ImgFeeder_UnRegisterFIFO(SVC_IMG_FEEDER_HANDLE_s *pHandle, UINT32 PortID, UINT32 FIFOChanID);
static void   ImgFeeder_UpdateFIFOReadStatus(SVC_IMG_FEEDER_FIFO_STATUS_s *pFIFOStatus, UINT32 RdIdx);

static UINT32 ImgFeeder_CheckCapSeg(SVC_IMG_FEEDER_HANDLE_s *pHandle, SVC_IMG_FIFO_DATA_STATUS_s *pDataStatusArr, UINT32 *pRetRdIdxArr);
static UINT32 ImgFeeder_GetCapSeg(UINT32 DataSrc, void *pDataInfo, UINT64 *pCapSeg);
static UINT32 ImgFeeder_SearchCapSegInFIFO(SVC_IMG_FEEDER_HANDLE_s *pHandle, SVC_IMG_FIFO_DATA_STATUS_s *pDataStatusArr, UINT64 TargetCapSeg, UINT32 *pRetRdIdxArr);
static UINT32 ImgFeeder_CheckDivisor(SVC_IMG_FEEDER_HANDLE_s *pHandle, SVC_IMG_FIFO_DATA_STATUS_s *pDataStatusArr, UINT32 *pRetRdIdxArr);

static UINT32 SvcCvFlow_TaskIdentify(UINT64 *pTaskIdentifier);

static void ImgFeeder_Dbg(const char *pFormat, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    if (FeederDebugEnable > 0U) {
        AmbaPrint_PrintUInt5(pFormat, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}

/**
 * Svc image feeder debug enable/disable function
 * @param [in] DebugEnable enable/disable debug message
 * return None
 */
void SvcImgFeeder_DebugEnable(UINT32 DebugEnable)
{
    FeederDebugEnable = (UINT8)DebugEnable;
    SvcLog_OK(SVC_LOG_IMG_FEEDER, "FeederDebugEnable = %d", FeederDebugEnable, 0U);
}

static void ImgFeeder_MutexTake(AMBA_KAL_MUTEX_t *pMutex)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(pMutex, 5000)) {
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "ImgFeeder_MutexTake: timeout", 0U, 0U);
    }
}

static void ImgFeeder_MutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(pMutex)) {
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "ImgFeeder_MutexGive: error", 0U, 0U);
    }
}

static UINT32 ImgFeeder_GetNewPortID(SVC_IMG_FEEDER_HANDLE_s *pHandle, UINT32 *pPortID)
{
    UINT32 RetVal = SVC_NG;
    UINT32 i;

    AmbaMisra_TouchUnused(pHandle);

    for (i = 0; i < SVC_IMG_FEEDER_PORT_MAX; i++) {
        if (0U == pHandle->Ports[i].Opened) {
            *pPortID = i;
            RetVal = SVC_OK;
            break;
        }
    }

    return RetVal;
}

static UINT32 ImgFeeder_SearchExistFIFO(SVC_IMG_FEEDER_HANDLE_s *pHandle, SVC_CV_INPUT_IMG_CONTENT_s *pContent, UINT32 *pFIFOChanID)
{
    UINT32 RetVal = 0U;
    UINT32 ChanID;

    AmbaMisra_TouchUnused(pHandle);
    AmbaMisra_TouchUnused(pContent);

    for (ChanID = 0; ChanID < SVC_IMG_FIFO_MAX_CHAN; ChanID++) {
        if ((pHandle->FIFO[ChanID].Registered == 1U) &&
            (pHandle->FIFO[ChanID].Content.DataSrc == pContent->DataSrc) &&
            (pHandle->FIFO[ChanID].Content.StrmId == pContent->StrmId)) {
            *pFIFOChanID = ChanID;
            RetVal = 1U;
            break;
        }
    }

    return RetVal;
}

static UINT32 ImgFeeder_RegisterFIFO(SVC_IMG_FEEDER_HANDLE_s *pHandle, UINT32 PortID, SVC_CV_INPUT_IMG_CONTENT_s *pContent, UINT32 *pFIFOChanID)
{
    UINT32 RetVal = SVC_NG;
    UINT32 ChanID;
    SVC_IMG_FIFO_CHAN_s Chan = {0};
    SVC_IMG_FIFO_CFG_s Cfg = {0};

    AmbaMisra_TouchUnused(pContent);

    if (1U == ImgFeeder_SearchExistFIFO(pHandle, pContent, &ChanID)) {
        /* Return existing FIFO channel */
        RetVal = SVC_OK;
    } else {
        /* Register new FIFO channel */
        RetVal = AmbaWrap_memcpy(&Cfg.Content, pContent, sizeof(SVC_CV_INPUT_IMG_CONTENT_s));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_IMG_FEEDER, "memcpy Content failed", 0U, 0U);
        }
        Cfg.pCallback = ImgFeeder_FIFOCallback;
        Cfg.UserTag   = pHandle->FeederID;
        RetVal = SvcImgFIFO_Register(&Cfg, &Chan);
        if (SVC_OK == RetVal) {
            ChanID = Chan.ChanID;
            if (0U == pHandle->FIFO[ChanID].Registered) {
                pHandle->FIFO[ChanID].Registered = 1U;
                RetVal = AmbaWrap_memcpy(&pHandle->FIFO[ChanID].Chan, &Chan, sizeof(SVC_IMG_FIFO_CHAN_s));
                RetVal |= AmbaWrap_memcpy(&pHandle->FIFO[ChanID].Content, pContent, sizeof(SVC_CV_INPUT_IMG_CONTENT_s));
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_IMG_FEEDER, "memcpy FIFO info failed", 0U, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "FIFO(%d) status mismatch", ChanID, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_IMG_FEEDER, "Register FIFO failed", 0U, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        /* Update FIFO status */
        pHandle->FIFO[ChanID].PortLintenBits = SVC_IMG_FEEDER_ADD_BIT(pHandle->FIFO[ChanID].PortLintenBits, PortID);
        pHandle->FIFOEnaBits = SVC_IMG_FEEDER_ADD_BIT(pHandle->FIFOEnaBits, ChanID);
        *pFIFOChanID = ChanID;
    }

    return RetVal;
}

static UINT32 ImgFeeder_UnRegisterFIFO(SVC_IMG_FEEDER_HANDLE_s *pHandle, UINT32 PortID, UINT32 FIFOChanID)
{
    UINT32 RetVal = SVC_NG;
    SVC_IMG_FEEDER_FIFO_STATUS_s *pFIFOSt;

    pFIFOSt = &pHandle->FIFO[FIFOChanID];

    if ((1U == pFIFOSt->Registered) &&
        (0U < (pFIFOSt->PortLintenBits & ((UINT32) 1U << PortID)))) {
        pFIFOSt->PortLintenBits = SVC_IMG_FEEDER_REMOVE_BIT(pFIFOSt->PortLintenBits, PortID);
        if (pFIFOSt->PortLintenBits == 0U) {
            /* Unregister if no one lintens to the FIFO */
            RetVal = SvcImgFIFO_Unregister(&pFIFOSt->Chan);
            if (SVC_OK == RetVal) {
                pFIFOSt->Registered = 0U;
                pHandle->FIFOEnaBits = SVC_IMG_FEEDER_REMOVE_BIT(pHandle->FIFOEnaBits, FIFOChanID);
            } else {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "Unregister FIFO failed", 0U, 0U);
            }
        } else {
            RetVal = SVC_OK;
        }
    } else {
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "FIFO(%d) status mismatch. PortLintenBits = 0x%x", FIFOChanID, pFIFOSt->PortLintenBits);
    }

    return RetVal;
}

/* Get FIFO status from ImgFIFO for all */
static UINT32 ImgFeeder_GetFIFOStatusAll(SVC_IMG_FEEDER_HANDLE_s *pHandle, SVC_IMG_FIFO_DATA_STATUS_s *pDataStatusArr)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;

    for (i = 0; i < SVC_IMG_FIFO_MAX_CHAN; i++) {
        pDataStatusArr[i].pQueueBase = NULL;
        if (0U < pHandle->FIFO[i].PortLintenBits) {
            if (SVC_OK != SvcImgFIFO_GetFIFOStatus(&pHandle->FIFO[i].Chan, &pDataStatusArr[i])) {
                SvcLog_DBG(SVC_LOG_IMG_FEEDER, "Feeder(%u) get FIFO(%u) status failed", pHandle->FeederID, i);
                RetVal |= SVC_NG;
            }
        }
    }

    return RetVal;
}

/* Set RdCount as the previous point before WrCount */
static void ImgFeeder_InitFIFOReadStatusAll(SVC_IMG_FEEDER_HANDLE_s *pHandle, SVC_IMG_FIFO_DATA_STATUS_s *pDataStatusArr)
{
    #define DIVISOR_BASE    3U
    #define INIT_LATENCY    4U
    UINT32 ChanID;
    UINT32 InitLatency;

    AmbaMisra_TouchUnused(pDataStatusArr);

    InitLatency = INIT_LATENCY;
    if (pHandle->Divisor > DIVISOR_BASE) {
        InitLatency += ((UINT32)pHandle->Divisor - DIVISOR_BASE);
    }

    for (ChanID = 0; ChanID < SVC_IMG_FIFO_MAX_CHAN; ChanID++) {
        if (0U < pHandle->FIFO[ChanID].PortLintenBits) {
            if (pDataStatusArr[ChanID].WrCount > InitLatency) {
                pHandle->FIFO[ChanID].RdCount = pDataStatusArr[ChanID].WrCount - InitLatency;
            } else {
                pHandle->FIFO[ChanID].RdCount = 0U;
            }

            AmbaPrint_PrintUInt5("Feeder(%u) Init FIFO(%u) ReadStatus: WrCount(%u) RdCount(%u)",
                pHandle->FeederID, ChanID, pDataStatusArr[ChanID].WrCount, pHandle->FIFO[ChanID].RdCount, 0U);
        }
    }
}

/* Update local FIFO status */
static void ImgFeeder_UpdateFIFOReadStatus(SVC_IMG_FEEDER_FIFO_STATUS_s *pFIFOStatus, UINT32 RdIdx)
{
    /* Check read and write gap */
#if 0
    if ((RdIdx - pFIFOStatus->RdCount) > 1U) {
        ImgFeeder_Dbg("FDTx: skip", 0U, 0U, 0U, 0U, 0U);
    }
#endif
    AmbaMisra_TouchUnused(pFIFOStatus);
    pFIFOStatus->RdCount = RdIdx;
}

static UINT32 ImgFeeder_CheckCapSeg(SVC_IMG_FEEDER_HANDLE_s *pHandle, SVC_IMG_FIFO_DATA_STATUS_s *pDataStatusArr, UINT32 *pRetRdIdxArr)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i, ChanID;
    UINT32 Offset;
    UINT32 NumValidFifo = 0U, Valid = 0U;
    UINT64 CapSegArr[SVC_IMG_FIFO_MAX_CHAN];

    if (SVC_OK != AmbaWrap_memset(CapSegArr, 0, sizeof(CapSegArr))) {
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) clear CapSegArr failed", pHandle->FeederID, 0U);
    }

    /* Get the latest capture sequence for each FIFO */
    for (ChanID = 0; ChanID < SVC_IMG_FIFO_MAX_CHAN; ChanID++) {
        if (0U < pHandle->FIFO[ChanID].PortLintenBits) {
            Offset = pDataStatusArr[ChanID].Wp * pDataStatusArr[ChanID].ElementSize;
            RetVal = ImgFeeder_GetCapSeg(pDataStatusArr[ChanID].Content.DataSrc,
                                         &pDataStatusArr[ChanID].pQueueBase[Offset],
                                         &CapSegArr[NumValidFifo]);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "ImgFeeder_GetCapSeg failed", 0U, 0U);
                break;
            }
            NumValidFifo++;
        }
    }

    ImgFeeder_Dbg("CheckCapSeg: NumFifo = %u (%u, %u, %u, %u)",
        NumValidFifo,
        (UINT32)CapSegArr[0U],
        (UINT32)CapSegArr[1U],
        (UINT32)CapSegArr[2U],
        (UINT32)CapSegArr[3U]);

    /* Compare CapSeg for each channel */
    if (RetVal == SVC_OK) {
        for (i = 0; i < NumValidFifo; i++) {
            /* Skip repeat CapSeq */
            if ((i > 0U) && (CapSegArr[i] == CapSegArr[i - 1U])) {
                continue;
            }

            if (NumValidFifo == ImgFeeder_SearchCapSegInFIFO(pHandle, pDataStatusArr, CapSegArr[i], pRetRdIdxArr)) {
                Valid = 1U;
                break;
            }
        }
    }

    return Valid;
}

static UINT32 ImgFeeder_GetCapSeg(UINT32 DataSrc, void *pDataInfo, UINT64 *pCapSeg)
{
    UINT32 RetVal = SVC_OK;

    AmbaMisra_TouchUnused(pDataInfo);

    switch (DataSrc) {
    case SVC_CV_DATA_SRC_PYRAMID:
        {
            const AMBA_DSP_PYMD_DATA_RDY_s *pInfo;
            AmbaMisra_TypeCast(&pInfo, &pDataInfo);
            *pCapSeg = pInfo->CapSequence;
        }
        break;
    case SVC_CV_DATA_SRC_MAIN_YUV:
    case SVC_CV_DATA_SRC_YUV_STRM:
        {
            const AMBA_DSP_YUV_DATA_RDY_s *pInfo;
            AmbaMisra_TypeCast(&pInfo, &pDataInfo);
            *pCapSeg = pInfo->CapSequence;
        }
        break;
    default:
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "ImgFeeder_GetCapSeg failed", 0U, 0U);
        RetVal = SVC_NG;
        break;
    }

    return RetVal;
}

static UINT32 ImgFeeder_SearchCapSegInFIFO(SVC_IMG_FEEDER_HANDLE_s *pHandle, SVC_IMG_FIFO_DATA_STATUS_s *pDataStatusArr, UINT64 TargetCapSeg, UINT32 *pRetRdIdxArr)
{
    #define SEARCH_RANGE    (2U)
    UINT32 i, ChanID, Found, End;
    UINT32 Offset, SearchRange, NumMatch = 0U;
    UINT64 CapSeg;
    const SVC_IMG_FIFO_DATA_STATUS_s *pDataStatus;
    UINT32 RdIdx, Rp = 0;
    UINT32 WrCount, RdCount;

    AmbaMisra_TouchUnused(pHandle);
    AmbaMisra_TouchUnused(pDataStatusArr);

    //ImgFeeder_Dbg("#TargetCapSeg = %u", TargetCapSeg, 0U, 0U, 0U, 0U);

    for (ChanID = 0; ChanID < SVC_IMG_FIFO_MAX_CHAN; ChanID++) {
        if (0U < pHandle->FIFO[ChanID].PortLintenBits) {
            pDataStatus = &pDataStatusArr[ChanID];
            WrCount = pDataStatus->WrCount;
            RdCount = pHandle->FIFO[ChanID].RdCount;
            if (WrCount >= RdCount) {
                SearchRange = pDataStatus->WrCount - pHandle->FIFO[ChanID].RdCount;
            } else {
                /* wrap around case */
                SearchRange = (COUNTER_MAX - (pHandle->FIFO[ChanID].RdCount - pDataStatus->WrCount)) + 1U;
            }
            if (SearchRange > SEARCH_RANGE) {
                SearchRange = SEARCH_RANGE;
            }

            //ImgFeeder_Dbg("ChanID(%u) Range(%u)", ChanID, SearchRange, 0U, 0U, 0U);

            Found = 0U;
            End = 0U;
            RdIdx = pDataStatus->WrCount;
            for (i = 0; i < SearchRange; i++) {
                SvcImgFIFO_CountToPointer(RdIdx,
                                          pDataStatus->CountLimit,
                                          pDataStatus->MaxElement,
                                          &Rp);
                Offset = Rp * pDataStatus->ElementSize;
                if (SVC_OK == ImgFeeder_GetCapSeg(pDataStatus->Content.DataSrc, &pDataStatus->pQueueBase[Offset], &CapSeg)) {
                    //ImgFeeder_Dbg("ChanID(%u) CapSeg(%u)", ChanID, CapSeg, 0U, 0U, 0U);

                    if (CapSeg == TargetCapSeg) {
                        pRetRdIdxArr[ChanID] = RdIdx;
                        NumMatch++;
                        Found = 1U;
                        //ImgFeeder_Dbg("ChanID(%u) Found", ChanID, 0U, 0U, 0U, 0U);
                    } else {
                        /* Search backward */
                        if (RdIdx == 0U) {
                            RdIdx = COUNTER_MAX;
                            ImgFeeder_Dbg("ImgFeeder_SearchCapSegInFIFO: RdIdx = 0U", pHandle->FeederID, 0U, 0U, 0U, 0U);
                        } else {
                            RdIdx--;
                        }
                    }
                } else {
                    SvcLog_NG(SVC_LOG_IMG_FEEDER, "FIFO(%u) GetCapSeg failed", ChanID, 0U);
                    End = 1U;
                }

                if ((Found == 1U) || (End == 1U)) {
                    break;
                }
            }

            if (End == 1U) {
                break;
            }
        }
    }

    return NumMatch;
}

static UINT32 ImgFeeder_CheckDivisor(SVC_IMG_FEEDER_HANDLE_s *pHandle, SVC_IMG_FIFO_DATA_STATUS_s *pDataStatusArr, UINT32 *pRetRdIdxArr)
{
#if 1
#define DIVISOR_BASE    3U
#define LATENCY_TH      5U
    UINT32 ChanID;
    UINT32 Valid = 0U;
    UINT32 RdCount, WrCount, CountLimit, LatencyTh;
    UINT32 Stride, RWGap = 0U;

    AmbaMisra_TouchUnused(pHandle);
    AmbaMisra_TouchUnused(pDataStatusArr);

    LatencyTh = LATENCY_TH;
    if (pHandle->Divisor > DIVISOR_BASE) {
        LatencyTh += ((UINT32)pHandle->Divisor - DIVISOR_BASE);
    }

    for (ChanID = 0; ChanID < SVC_IMG_FIFO_MAX_CHAN; ChanID++) {
        if (0U < pHandle->FIFO[ChanID].PortLintenBits) {
            RdCount = pHandle->FIFO[ChanID].RdCount;
            WrCount = pDataStatusArr[ChanID].WrCount;
            CountLimit = pDataStatusArr[ChanID].CountLimit;

            /* Check gap between read and write */
            if (WrCount >= RdCount) {
                RWGap = WrCount - RdCount;
            } else {
                SvcLog_DBG(SVC_LOG_IMG_FEEDER, "FIFO(%u) CheckDivisor Wrap Around", ChanID, 0U);
                RWGap = (CountLimit - RdCount) + WrCount;
            }

            ImgFeeder_Dbg("CheckDivisor(%u) Wr %u Rd %u RWGap %u",
                ChanID, WrCount, RdCount, RWGap, 0U);

            Stride = pHandle->Divisor;
            if (RWGap >= Stride) {
                /* If the gap is over the threshold, increment more. */
                if (RWGap > LatencyTh) {
                    //Stride++;
                    Stride = Stride + (RWGap - LatencyTh);
                    SvcLog_DBG(SVC_LOG_IMG_FEEDER, "FIFO(%u) RdCount increment(%u)", ChanID, Stride);
                }

                SvcImgFIFO_CountIncrement(&RdCount, CountLimit, Stride);
                pRetRdIdxArr[ChanID] = RdCount;

                ImgFeeder_Dbg("CheckDivisor(%u) done Rd = %u",
                    ChanID, pRetRdIdxArr[ChanID], 0U, 0U, 0U);

                Valid = 1U;
            } else if (RWGap >= 1U) {
                /* If the gap is less than the threshold, increment RWGap only. */
                SvcLog_DBG(SVC_LOG_IMG_FEEDER, "FIFO(%u) RdCount increment(%u)", ChanID, RWGap);

                SvcImgFIFO_CountIncrement(&RdCount, CountLimit, RWGap);
                pRetRdIdxArr[ChanID] = RdCount;

                ImgFeeder_Dbg("CheckDivisor(%u) done Rd = %u",
                    ChanID, pRetRdIdxArr[ChanID], 0U, 0U, 0U);

                Valid = 1U;
            } else {
                Valid = 0U;
                break;
            }
        }
    }

    return Valid;
#else
    UINT32 ChanID;
    UINT32 Valid = 0U;
    UINT32 Stride = pHandle->Divisor;

    for (ChanID = 0; ChanID < SVC_IMG_FIFO_MAX_CHAN; ChanID++) {
        if (0U < pHandle->FIFO[ChanID].PortLintenBits) {
            if ((pDataStatusArr[ChanID].WrCount % Stride) == 0U) {
                pRetRdIdxArr[ChanID] = pDataStatusArr[ChanID].WrCount;
                Valid = 1U;
            } else {
                Valid = 0U;
                break;
            }
        }
    }

    return Valid;
#endif
}

UINT32 SvcImgFeeder_Init(SVC_IMG_FEEDER_INIT_CFG_s *pCfg)
{
    UINT32 RetVal;

    AmbaMisra_TouchUnused(pCfg);

    if (NULL != pCfg) {
        RetVal = AmbaWrap_memset(FeederHandle, 0, sizeof(FeederHandle));
        if (RetVal != SVC_OK) {
            RetVal = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "SvcImgFeeder_Init: invalid param", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
UINT32 SvcImgFeeder_ReEnable(UINT32 FeederID)
{
    SVC_IMG_FEEDER_HANDLE_s *pHandle;

    pHandle = &FeederHandle[FeederID];
    ImgFeeder_MutexTake(&pHandle->Mutex);
    //pHandle->DataUpdateFlag = 1;
    pHandle->PortRdyFlag = SVC_IMG_FEEDER_ADD_BIT(pHandle->PortRdyFlag, FeederID);
    AmbaKAL_EventFlagSet(&pHandle->CtrlFlagId, SVC_IMG_FEEDER_FLAG_PORT_READY);
    ImgFeeder_MutexGive(&pHandle->Mutex);

    return SVC_OK;
}
#endif

/**
* Create image feeder
* @param [in] FeederID feeder ID
* @param [in] pCfg configuration of create
* @return 0-OK, 1-NG
*/
UINT32 SvcImgFeeder_Create(UINT32 FeederID, SVC_IMG_FEEDER_CREATE_CFG_s *pCfg)
{
    UINT32 RetVal = SVC_OK;
    SVC_IMG_FEEDER_HANDLE_s *pHandle;
    static UINT8 TxTaskStack[SVC_IMG_FEEDER_MAX_ID][SVC_IMG_FEEDER_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char MutexName[] = "FdMtx";
    static char CtrlFlagName[] = "FdCtrlFlag";

    AmbaMisra_TouchUnused(pCfg);

    if ((FeederID < SVC_IMG_FEEDER_MAX_ID) && (NULL != pCfg)) {
        pHandle = &FeederHandle[FeederID];
        if (0U == pHandle->Created) {
            if(KAL_ERR_NONE != AmbaKAL_MutexCreate(&pHandle->Mutex, MutexName)) {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "Create Mutex error(0x%x)", RetVal, 0U);
                RetVal |= SVC_NG;
            }

            if(KAL_ERR_NONE != AmbaKAL_MutexCreate(&pHandle->DataStMutex, MutexName)) {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "Create DataStMutex error(0x%x)", RetVal, 0U);
                RetVal |= SVC_NG;
            }

            if (KAL_ERR_NONE != AmbaKAL_EventFlagCreate(&pHandle->CtrlFlagId, CtrlFlagName)) {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "EventFlagCreate error(0x%x)", RetVal, 0U);
                RetVal |= SVC_NG;
            }

            if (SVC_OK == RetVal) {
                pHandle->FeederID          = FeederID;
                if ((pCfg->FeederFlag & SVC_IMG_FEEDER_FLAG_SYNC_CAP_SEQ) > 0U) {
                    pHandle->pFeederHandler = ImgFeeder_HandlerSyncCapSeq;
                } else if ((pCfg->FeederFlag & SVC_IMG_FEEDER_FLAG_DIVISOR) > 0U) {
                    pHandle->pFeederHandler = ImgFeeder_HandlerDivisor;
                    pHandle->Divisor = (UINT8)((pCfg->FeederFlag >> SVC_IMG_FEEDER_FLAG_DIVISOR_SFT) & SVC_IMG_FEEDER_FLAG_DIVISOR_BITS);
                    if (pHandle->Divisor == 0U) {
                        pHandle->Divisor = 1U;
                    }
                } else {
                    pHandle->pFeederHandler = ImgFeeder_HandlerSyncPorts;
                }

                if ((pCfg->FeederFlag & SVC_IMG_FEEDER_FLAG_EXT_SYNC) > 0U) {
                    pHandle->ExtSync = 1U;
                }

                AmbaPrint_PrintUInt5("SvcImgFeeder_Create[%u] Flag(0x%x) Divisor(%u) ExtSync(%u)", FeederID,
                    pCfg->FeederFlag, pHandle->Divisor, pHandle->ExtSync, 0U);

                RetVal = AmbaKAL_EventFlagSet(&pHandle->CtrlFlagId, SVC_IMG_FEEDER_FLAG_ENABLE);
                if (KAL_ERR_NONE != RetVal) {
                    SvcLog_NG(SVC_LOG_IMG_FEEDER, "Create: Feeder(%u) EventFlagSet error", FeederID, 0U);
                }

                pHandle->TxTask.Priority   = pCfg->TaskPriority;
                pHandle->TxTask.EntryFunc  = ImgFeeder_TxTaskEntry;
                AmbaMisra_TypeCast(&pHandle->TxTask.EntryArg, &pHandle);
                pHandle->TxTask.pStackBase = &TxTaskStack[FeederID][0];
                pHandle->TxTask.StackSize  = SVC_IMG_FEEDER_STACK_SIZE;
                pHandle->TxTask.CpuBits    = pCfg->CpuBits;
                RetVal = SvcTask_Create("FeederTxTask", &pHandle->TxTask);
                if (SVC_OK == RetVal) {
                    pHandle->Created = 1U;
                } else {
                    SvcLog_NG(SVC_LOG_IMG_FEEDER, "TaskCrete error(0x%x)", RetVal, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "MutexCreate error", 0U, 0U);
            }
        } else {
            SvcLog_DBG(SVC_LOG_IMG_FEEDER, "Feeder(%d) has created", FeederID, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "SvcImgFeeder_Create: Feeder(%d) invalid param", FeederID, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Control for image feeder utility
* @param [in] FeederID configuration of image feeder
* @param [in] Msg message code
* @param [in] pInfo message information
* @return 0-OK, 1-NG
*/
UINT32 SvcImgFeeder_Ctrl(UINT32 FeederID, UINT32 Msg, void *pInfo)
{
    UINT32 RetVal = SVC_OK;
    SVC_IMG_FEEDER_HANDLE_s *pHandle;
    const UINT32 *pInfoU32;

    AmbaMisra_TouchUnused(pInfo);

    if (FeederID < SVC_IMG_FEEDER_MAX_ID) {
        pHandle = &FeederHandle[FeederID];
        ImgFeeder_MutexTake(&pHandle->Mutex);

        if (1U == pHandle->Created) {
            switch (Msg) {
                case SVC_IMG_FEEDER_CTRL_ENABLE:
                    AmbaMisra_TypeCast(&pInfoU32, &pInfo);
                    if (*pInfoU32 == 1U) {
                        RetVal = AmbaKAL_EventFlagSet(&pHandle->CtrlFlagId, SVC_IMG_FEEDER_FLAG_ENABLE);
                        if (KAL_ERR_NONE != RetVal) {
                            SvcLog_NG(SVC_LOG_IMG_FEEDER, "Enable: Feeder(%u) EventFlagSet error", pHandle->FeederID, 0U);
                        }
                    } else {
                        RetVal = AmbaKAL_EventFlagClear(&pHandle->CtrlFlagId, SVC_IMG_FEEDER_FLAG_ENABLE);
                        if (KAL_ERR_NONE != RetVal) {
                            SvcLog_NG(SVC_LOG_IMG_FEEDER, "Disable: Feeder(%u) EventFlagClear error", pHandle->FeederID, 0U);
                        }
                    }
                    break;
                case SVC_IMG_FEEDER_EXT_SYNC:
                    RetVal = AmbaKAL_EventFlagSet(&pHandle->CtrlFlagId, SVC_IMG_FEEDER_FLAG_RUN);
                    if (KAL_ERR_NONE != RetVal) {
                        SvcLog_NG(SVC_LOG_IMG_FEEDER, "ExtSync: Feeder(%u) EventFlagSet error", pHandle->FeederID, 0U);
                    } else {
                        ImgFeeder_Dbg("FD[%u]: ExtSync", FeederID, 0U, 0U, 0U, 0U);
                    }
                    break;
                default:
                    SvcLog_NG(SVC_LOG_IMG_FEEDER, "SvcImgFeeder_Ctrl: invalid msg(0x%x)", Msg, 0U);
                    RetVal = SVC_NG;
                    break;
            }
        } else {
            SvcLog_NG(SVC_LOG_IMG_FEEDER, "OpenPort: Feeder(%u) is not created", FeederID, 0U);
            RetVal = SVC_NG;
        }

        ImgFeeder_MutexGive(&pHandle->Mutex);
    } else {
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "SvcImgFeeder_Ctrl: invalid param", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Open a port and specify the configuration of the port
* @param [in] FeederID feeder ID
* @param [in] pCfg configuration of port
* @param [out] pPort return the port identifier
* @return 0-OK, 1-NG
*/
UINT32 SvcImgFeeder_OpenPort(UINT32 FeederID, SVC_IMG_FEEDER_PORT_CFG_s *pCfg, SVC_IMG_FEEDER_PORT_s *pPort)
{
    UINT32 RetVal = SVC_OK;
    SVC_IMG_FEEDER_HANDLE_s *pHandle;
    UINT32 PortID = 0;
    SVC_IMG_FEEDER_PORT_STATUS_s *pPortSt;
    UINT32 i, FIFOChanID;

    AmbaMisra_TouchUnused(pCfg);

    if ((FeederID < SVC_IMG_FEEDER_MAX_ID) &&
        (NULL != pCfg) &&
        (NULL != pPort)) {
        pHandle = &FeederHandle[FeederID];
        ImgFeeder_MutexTake(&pHandle->Mutex);

        if (1U == pHandle->Created) {
            /* Get new port */
            RetVal = ImgFeeder_GetNewPortID(pHandle, &PortID);
            if (SVC_OK == RetVal) {
                pPortSt = &pHandle->Ports[PortID];
                for (i = 0; i < pCfg->NumPath; i++) {
                    /* Register FIFO channel */
                    if (SVC_OK == ImgFeeder_RegisterFIFO(pHandle, PortID, &pCfg->Content[i], &FIFOChanID)) {
                        pPortSt->SubPort[i].Used = 1U;
                        pPortSt->SubPort[i].FIFOChanID = (UINT8)FIFOChanID;
                        ImgFeeder_Dbg("FD[%u]: SubPort(%u) links to FIFO(%u)", FeederID, i, FIFOChanID, 0U, 0U);
                    } else {
                        SvcLog_NG(SVC_LOG_IMG_FEEDER, "Open: Feeder(%u) Port(%d) register FIFO error", pPort->FeederID, pPort->PortID);
                        RetVal |= SVC_NG;
                    }
                }

                if (SVC_OK == RetVal) {
                    RetVal = AmbaWrap_memcpy(&pHandle->PortCfg[PortID], pCfg, sizeof(SVC_IMG_FEEDER_PORT_CFG_s));
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_IMG_FEEDER, "memcpy PortCfg failed", 0U, 0U);
                    }
                    pHandle->Ports[PortID].Opened = 1U;
                    pPort->FeederID = FeederID;
                    pPort->PortID   = PortID;
                }

                ImgFeeder_Dbg("FD[%u]: OpenPort(%u)", FeederID, PortID, 0U, 0U, 0U);
            } else {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "OpenPort: Feeder(%u) has no available port", FeederID, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_IMG_FEEDER, "OpenPort: Feeder(%u) is not created", FeederID, 0U);
            RetVal = SVC_NG;
        }

        ImgFeeder_MutexGive(&pHandle->Mutex);
    } else {
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "SvcImgFeeder_OpenPort: invalid param", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Close a port
* @param [in] pPort port identifier
* @return 0-OK, 1-NG
*/
UINT32 SvcImgFeeder_ClosePort(SVC_IMG_FEEDER_PORT_s *pPort)
{
    UINT32 RetVal = SVC_OK;
    SVC_IMG_FEEDER_HANDLE_s *pHandle;
    SVC_IMG_FEEDER_PORT_STATUS_s *pPortSt;
    UINT32 i;

    AmbaMisra_TouchUnused(pPort);

    if ((NULL != pPort) &&
        (pPort->FeederID < SVC_IMG_FEEDER_MAX_ID) &&
        (pPort->PortID < SVC_IMG_FEEDER_PORT_MAX)) {
        pHandle = &FeederHandle[pPort->FeederID];
        ImgFeeder_MutexTake(&pHandle->Mutex);

        if (1U == pHandle->Created) {
            if (1U == pHandle->Ports[pPort->PortID].Opened) {
                pHandle->Ports[pPort->PortID].Opened = 0U;
                pHandle->PortCfg[pPort->PortID].NumPath = 0U;

                pPortSt = &pHandle->Ports[pPort->PortID];
                for (i = 0; i < SVC_CV_INPUT_INFO_MAX_DATA_NUM; i++) {
                    if (1U == pPortSt->SubPort[i].Used) {
                        pPortSt->SubPort[i].Used = 0U;
                        if (SVC_OK != ImgFeeder_UnRegisterFIFO(pHandle, pPort->PortID, pPortSt->SubPort[i].FIFOChanID)) {
                            SvcLog_NG(SVC_LOG_IMG_FEEDER, "ClosePort: Feeder(%u) Port(%d) unregister FIFO error", pPort->FeederID, pPort->PortID);
                            RetVal |= SVC_NG;
                        }
                    }
                }

                ImgFeeder_Dbg("FD[%u]: ClosePort(%u)", pPort->FeederID, pPort->PortID, 0U, 0U, 0U);
            } else {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "ClosePort: Feeder(%u) port(%u) is not opened", pPort->FeederID, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_IMG_FEEDER, "ClosePort: Feeder(%u) is not created", pPort->FeederID, 0U);
            RetVal = SVC_NG;
        }

        ImgFeeder_MutexGive(&pHandle->Mutex);
    } else {
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "SvcImgFeeder_ClosePort: invalid param", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Connect the port to feeder
* @param [in] pPort port identifier
* @return 0-OK, 1-NG
*/
UINT32 SvcImgFeeder_Connect(SVC_IMG_FEEDER_PORT_s *pPort)
{
    UINT32 RetVal = SVC_OK;
    SVC_IMG_FEEDER_HANDLE_s *pHandle;
    UINT32 SetFlags;

    AmbaMisra_TouchUnused(pPort);

    if ((NULL != pPort) &&
        (pPort->FeederID < SVC_IMG_FEEDER_MAX_ID) &&
        (pPort->PortID < SVC_IMG_FEEDER_PORT_MAX)) {
        pHandle = &FeederHandle[pPort->FeederID];
        ImgFeeder_MutexTake(&pHandle->Mutex);

        ImgFeeder_Dbg("FD[%u]: Connect Port(%u)", pPort->FeederID, pPort->PortID, 0U, 0U, 0U);

        /* Update Port status */
        pHandle->Ports[pPort->PortID].Connect = 1U;
        pHandle->PortEnaBits = SVC_IMG_FEEDER_ADD_BIT(pHandle->PortEnaBits, pPort->PortID);
        pHandle->PortRdyFlag = SVC_IMG_FEEDER_ADD_BIT(pHandle->PortRdyFlag, pPort->PortID);

        /* Inform TX task */
        if ((pHandle->PortEnaBits & pHandle->PortRdyFlag) == pHandle->PortEnaBits) {
            SetFlags = SVC_IMG_FEEDER_FLAG_PORT_READY;
            if (pHandle->ExtSync == 0U) {
                SetFlags |= SVC_IMG_FEEDER_FLAG_RUN;
            }

            RetVal = AmbaKAL_EventFlagSet(&pHandle->CtrlFlagId, SetFlags);
            if (KAL_ERR_NONE != RetVal) {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "Connect: Feeder(%u) Port(%d) EventFlagSet error", pPort->FeederID, pPort->PortID);
            }
        } else {
            RetVal = AmbaKAL_EventFlagClear(&pHandle->CtrlFlagId, SVC_IMG_FEEDER_FLAG_PORT_READY);
            if (KAL_ERR_NONE != RetVal) {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "Connect: Feeder(%u) Port(%d) EventFlagClear error", pPort->FeederID, pPort->PortID);
            }
        }

        ImgFeeder_MutexGive(&pHandle->Mutex);
    } else {
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "SvcImgFeeder_Connect: invalid param", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Disconnect the port from feeder
* @param [in] pPort port identifier
* @return 0-OK, 1-NG
*/
UINT32 SvcImgFeeder_Disconnect(SVC_IMG_FEEDER_PORT_s *pPort)
{
    UINT32 RetVal = SVC_OK;
    SVC_IMG_FEEDER_HANDLE_s *pHandle;

    AmbaMisra_TouchUnused(pPort);

    if ((NULL != pPort) &&
        (pPort->FeederID < SVC_IMG_FEEDER_MAX_ID) &&
        (pPort->PortID < SVC_IMG_FEEDER_PORT_MAX)) {
        pHandle = &FeederHandle[pPort->FeederID];
        ImgFeeder_MutexTake(&pHandle->Mutex);

        ImgFeeder_Dbg("FD[%u]: Disconn Port%u", pPort->FeederID, pPort->PortID, 0U, 0U, 0U);

        /* Update Port status */
        pHandle->Ports[pPort->PortID].Connect = 0U;
        pHandle->PortEnaBits = SVC_IMG_FEEDER_REMOVE_BIT(pHandle->PortEnaBits, pPort->PortID);
        pHandle->PortRdyFlag = SVC_IMG_FEEDER_REMOVE_BIT(pHandle->PortRdyFlag, pPort->PortID);

        /* Inform TX task */
        if ((pHandle->PortEnaBits & pHandle->PortRdyFlag) == pHandle->PortEnaBits) {
            RetVal = AmbaKAL_EventFlagSet(&pHandle->CtrlFlagId, SVC_IMG_FEEDER_FLAG_PORT_READY);
            if (KAL_ERR_NONE != RetVal) {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "Disconnect: Feeder(%u) Port(%d) EventFlagSet error", pPort->FeederID, pPort->PortID);
            }
        } else {
            RetVal = AmbaKAL_EventFlagClear(&pHandle->CtrlFlagId, SVC_IMG_FEEDER_FLAG_PORT_READY);
            if (KAL_ERR_NONE != RetVal) {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "Disconnect: Feeder(%u) Port(%d) EventFlagClear error", pPort->FeederID, pPort->PortID);
            }
        }

        ImgFeeder_MutexGive(&pHandle->Mutex);
    } else {
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "SvcImgFeeder_Connect: invalid param", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Send message to feeder
* @param [in] pPort port identifier
* @param [in] Msg message code
* @param [in] pInfo message information
* @return 0-OK, 1-NG
*/
UINT32 SvcImgFeeder_MsgToFeeder(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Msg, void *pInfo)
{
    UINT32 RetVal = SVC_OK;
    SVC_IMG_FEEDER_HANDLE_s *pHandle;
    UINT64 TaskIdentifier = 0;
    UINT32 MutexTake = 0U;

    AmbaMisra_TouchUnused(pPort);
    AmbaMisra_TouchUnused(pInfo);

    if ((NULL != pPort) &&
        (pPort->FeederID < SVC_IMG_FEEDER_MAX_ID) &&
        (pPort->PortID < SVC_IMG_FEEDER_PORT_MAX)) {
        pHandle = &FeederHandle[pPort->FeederID];

        if (0U == SvcCvFlow_TaskIdentify(&TaskIdentifier)) {
            if (TaskIdentifier != pHandle->TaskIdentifier) {
                MutexTake = 1U;
            }
        } else {
            SvcLog_NG(SVC_LOG_IMG_FEEDER, "MsgToFeeder: TaskIdentify failed", 0U, 0U);
        }

        if (MutexTake > 0U) {
            ImgFeeder_MutexTake(&pHandle->Mutex);
        }

        if (Msg == SVC_IMG_FEEDER_RECEIVER_READY) {
            pHandle->PortRdyFlag = SVC_IMG_FEEDER_ADD_BIT(pHandle->PortRdyFlag, pPort->PortID);
            ImgFeeder_Dbg("FD[%u]: Port%d Avail(0x%x) E(0x%x)", pPort->FeederID, pPort->PortID, pHandle->PortRdyFlag, pHandle->PortEnaBits, 0U);

            /* Inform TX task */
            if ((pHandle->PortEnaBits & pHandle->PortRdyFlag) == pHandle->PortEnaBits) {
                RetVal = AmbaKAL_EventFlagSet(&pHandle->CtrlFlagId, SVC_IMG_FEEDER_FLAG_PORT_READY);
                if (KAL_ERR_NONE != RetVal) {
                    SvcLog_NG(SVC_LOG_IMG_FEEDER, "MsgToFeeder: Feeder(%u) Port(%d) EventFlagSet error", pPort->FeederID, pPort->PortID);
                }
            } else {
                RetVal = AmbaKAL_EventFlagClear(&pHandle->CtrlFlagId, SVC_IMG_FEEDER_FLAG_PORT_READY);
                if (KAL_ERR_NONE != RetVal) {
                    SvcLog_NG(SVC_LOG_IMG_FEEDER, "MsgToFeeder: Feeder(%u) Port(%d) EventFlagClear error", pPort->FeederID, pPort->PortID);
                }
            }
        } else {
            SvcLog_NG(SVC_LOG_IMG_FEEDER, "MsgToFeeder: unknown msg", 0U, 0U);
        }

        if (MutexTake > 0U) {
            ImgFeeder_MutexGive(&pHandle->Mutex);
        }
    } else {
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "MsgToFeeder: invalid param", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

static void* ImgFeeder_TxTaskEntry(void* EntryArg)
{
    const ULONG  *pArg;
    ULONG        Arg;
    UINT32 RetVal = SVC_OK;
    SVC_IMG_FEEDER_HANDLE_s *pHandle;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Arg = (*pArg);
    AmbaMisra_TypeCast(&pHandle, &Arg);

    if (0U != SvcCvFlow_TaskIdentify(&pHandle->TaskIdentifier)) {
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "ImgFeeder_TxTaskEntry[%d] TaskIdentify failed", pHandle->FeederID, 0U);
    }

    SvcLog_DBG(SVC_LOG_IMG_FEEDER, "ImgFeeder_TxTaskEntry[%d] start", pHandle->FeederID, 0U);

    while (SVC_OK == RetVal) {
        RetVal = pHandle->pFeederHandler(pHandle);
    }

    SvcLog_DBG(SVC_LOG_IMG_FEEDER, "ImgFeeder_TxTaskEntry[%u]: exit", pHandle->FeederID, 0U);

    return NULL;
}

static UINT32 ImgFeeder_HandlerSyncPorts(void *pHandlePtr)
{
    UINT32 RetVal = SVC_OK;
    SVC_IMG_FEEDER_HANDLE_s *pHandle;
    UINT32 PortsReady = 0U, DataReady = 0U, ActualFlags = 0U;
    UINT32 i;
    UINT32 FIFOChanID, PortID, NumInfo, Offset;
    SVC_IMG_FEEDER_PORT_s Port;
    SVC_IMG_FIFO_DATA_STATUS_s DataStatusArr[SVC_IMG_FIFO_MAX_CHAN] = {0U};
    SVC_CV_INPUT_IMG_INFO_s FrameSend;

    AmbaMisra_TouchUnused(pHandlePtr);
    AmbaMisra_TypeCast(&pHandle, &pHandlePtr);

    ImgFeeder_Dbg("FDTx[%u]: wait", pHandle->FeederID, 0U, 0U, 0U, 0U);

    RetVal = AmbaKAL_EventFlagGet(&pHandle->CtrlFlagId,
                                  (SVC_IMG_FEEDER_FLAG_ENABLE |
                                   SVC_IMG_FEEDER_FLAG_RUN |
                                   SVC_IMG_FEEDER_FLAG_PORT_READY |
                                   SVC_IMG_FEEDER_FLAG_DATA_UPDATE),
                                  AMBA_KAL_FLAGS_ALL,
                                  AMBA_KAL_FLAGS_CLEAR_NONE,
                                  &ActualFlags,
                                  AMBA_KAL_WAIT_FOREVER);

    ImgFeeder_Dbg("FDTx[%u]: run", pHandle->FeederID, 0U, 0U, 0U, 0U);

    if (KAL_ERR_NONE == RetVal) {
        ImgFeeder_MutexTake(&pHandle->Mutex);

        ImgFeeder_Dbg("FDTx[%u]: Po(0x%x 0x%x) Dt(0x%x 0x%x)",
            pHandle->FeederID,
            pHandle->PortEnaBits,
            pHandle->PortRdyFlag,
            pHandle->FIFOEnaBits,
            pHandle->DataRdyFlag);

        /* Check port ready */
        if ((pHandle->PortEnaBits & pHandle->PortRdyFlag) == pHandle->PortEnaBits) {
            PortsReady = 1U;

            RetVal = AmbaKAL_MutexTake(&pHandle->DataStMutex, AMBA_KAL_WAIT_FOREVER);
            if (RetVal == KAL_ERR_NONE) {
                /* Get Data ready flag */
                pHandle->DataRdyFlag |= pHandle->DataUpdateFlag;

                /* Clear Data update flag */
                pHandle->DataUpdateFlag = 0U;
                if (SVC_OK != AmbaKAL_EventFlagClear(&pHandle->CtrlFlagId, SVC_IMG_FEEDER_FLAG_DATA_UPDATE)) {
                    SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) clear data update failed(0x%x)", pHandle->FeederID, RetVal);
                }

                /* Check data ready */
                if ((pHandle->FIFOEnaBits & pHandle->DataRdyFlag) == pHandle->FIFOEnaBits) {
                    if (SVC_OK != AmbaWrap_memset(DataStatusArr, 0, sizeof(DataStatusArr))) {
                        SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) clear DataStatusArr failed(0x%x)", pHandle->FeederID, 0U);
                    }

                    /* Get all FIFO status at the same time */
                    RetVal = ImgFeeder_GetFIFOStatusAll(pHandle, DataStatusArr);
                    if (SVC_OK == RetVal) {
                        DataReady = 1U;
                    } else {
                        SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) get FIFO status failed", pHandle->FeederID, 0U);
                    }
                }

                RetVal = AmbaKAL_MutexGive(&pHandle->DataStMutex);
                if (RetVal != KAL_ERR_NONE) {
                    SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) give DataStMutex failed(0x%x)", pHandle->FeederID, RetVal);
                    RetVal = SVC_NG;
                }
            } else {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) take DataStMutex failed(0x%x)", pHandle->FeederID, RetVal);
                RetVal = SVC_NG;
            }
        }

        if ((PortsReady == 1U) && (DataReady == 1U)) {
            /* Clear ready bit before sending. */
            pHandle->DataRdyFlag = 0U;
            pHandle->PortRdyFlag = 0U;
            if (SVC_OK != AmbaKAL_EventFlagClear(&pHandle->CtrlFlagId, SVC_IMG_FEEDER_FLAG_PORT_READY)) {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) clear port ready failed(0x%x)", pHandle->FeederID, RetVal);
            }
            ImgFeeder_Dbg("FDTx[%u]: clr", pHandle->FeederID, 0U, 0U, 0U, 0U);

            for (PortID = 0; PortID < SVC_IMG_FEEDER_PORT_MAX; PortID++) {
                if (1U == pHandle->Ports[PortID].Connect) {
                    /* Prepare frame info */
                    NumInfo = 0U;
                    for (i = 0; i < SVC_CV_INPUT_INFO_MAX_DATA_NUM; i++) {
                        if (1U == pHandle->Ports[PortID].SubPort[i].Used) {
                            FIFOChanID = pHandle->Ports[PortID].SubPort[i].FIFOChanID;

                            if (NULL != DataStatusArr[FIFOChanID].pQueueBase) {
                                /* Peek the latest frame */
                                Offset = DataStatusArr[FIFOChanID].Wp * DataStatusArr[FIFOChanID].ElementSize;
                                FrameSend.Info[NumInfo].pBase = &DataStatusArr[FIFOChanID].pQueueBase[Offset];
                                FrameSend.Info[NumInfo].ElementSize = DataStatusArr[FIFOChanID].ElementSize;
                                if (SVC_OK == AmbaWrap_memcpy(&FrameSend.Info[NumInfo].Content,
                                                              &DataStatusArr[FIFOChanID].Content,
                                                              sizeof(SVC_CV_INPUT_IMG_CONTENT_s))) {
                                    NumInfo++;
                                } else {
                                    SvcLog_NG(SVC_LOG_IMG_FEEDER, "Port(%u, %u) memcpy Content failed", PortID, i);
                                }

                                /* Update FIFO status */
                                ImgFeeder_UpdateFIFOReadStatus(&pHandle->FIFO[FIFOChanID], DataStatusArr[FIFOChanID].WrCount);
                            } else {
                                SvcLog_NG(SVC_LOG_IMG_FEEDER, "DataStatusArr[%d] = NULL", FIFOChanID, 0U);
                            }
                        }
                    }

                    if (NumInfo > 0U) {
                        ImgFeeder_Dbg("FDTx[%u]: send(P%u)", pHandle->FeederID, PortID, 0U, 0U, 0U);

                        /* Dispatch */
                        Port.FeederID     = pHandle->FeederID;
                        Port.PortID       = PortID;
                        FrameSend.NumInfo = NumInfo;
                        RetVal = pHandle->PortCfg[PortID].SendFunc(&Port, SVC_IMG_FEEDER_IMAGE_DATA_READY, &FrameSend);
                        if (SVC_OK != RetVal) {
                            SvcLog_DBG(SVC_LOG_IMG_FEEDER, "Feeder(%u) to Port(%u) error", 0U, 0U);
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) NumInfo = 0", 0U, 0U);
                    }
                }
            }
        } else {
            /* Do nothing */
        }

        ImgFeeder_MutexGive(&pHandle->Mutex);
    } else {
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "IMG_FD[%u]: FlagGet error(0x%x)", pHandle->FeederID, RetVal);
    }

    return SVC_OK;
}

static UINT32 ImgFeeder_HandlerSyncCapSeq(void *pHandlePtr)
{
    UINT32 RetVal = SVC_OK;
    SVC_IMG_FEEDER_HANDLE_s *pHandle;
    UINT32 PortsReady = 0U, DataReady = 0U, ActualFlags = 0U;
    UINT32 i;
    UINT32 FIFOChanID, PortID, NumInfo, Offset, Rp;
    SVC_IMG_FEEDER_PORT_s Port;
    SVC_IMG_FIFO_DATA_STATUS_s DataStatusArr[SVC_IMG_FIFO_MAX_CHAN] = {0U};
    UINT32 RetRdIdxArr[SVC_IMG_FIFO_MAX_CHAN] = {0U};
    SVC_CV_INPUT_IMG_INFO_s FrameSend;

    AmbaMisra_TouchUnused(pHandlePtr);
    AmbaMisra_TypeCast(&pHandle, &pHandlePtr);

    ImgFeeder_Dbg("FDTx[%u]: wait", pHandle->FeederID, 0U, 0U, 0U, 0U);

    RetVal = AmbaKAL_EventFlagGet(&pHandle->CtrlFlagId,
                                  (SVC_IMG_FEEDER_FLAG_ENABLE |
                                   SVC_IMG_FEEDER_FLAG_RUN |
                                   SVC_IMG_FEEDER_FLAG_PORT_READY |
                                   SVC_IMG_FEEDER_FLAG_DATA_UPDATE),
                                  AMBA_KAL_FLAGS_ALL,
                                  AMBA_KAL_FLAGS_CLEAR_NONE,
                                  &ActualFlags,
                                  AMBA_KAL_WAIT_FOREVER);

    ImgFeeder_Dbg("FDTx[%u]: run", pHandle->FeederID, 0U, 0U, 0U, 0U);

    if (KAL_ERR_NONE == RetVal) {
        ImgFeeder_MutexTake(&pHandle->Mutex);

        ImgFeeder_Dbg("FDTx[%u]: Po(0x%x 0x%x) Dt(0x%x 0x%x)",
            pHandle->FeederID,
            pHandle->PortEnaBits,
            pHandle->PortRdyFlag,
            pHandle->FIFOEnaBits,
            pHandle->DataRdyFlag);

        /* Check port ready */
        if ((pHandle->PortEnaBits & pHandle->PortRdyFlag) == pHandle->PortEnaBits) {
            PortsReady = 1U;

            RetVal = AmbaKAL_MutexTake(&pHandle->DataStMutex, AMBA_KAL_WAIT_FOREVER);
            if (RetVal == KAL_ERR_NONE) {
                /* Get Data ready flag */
                pHandle->DataRdyFlag |= pHandle->DataUpdateFlag;

                /* Clear Data update flag */
                pHandle->DataUpdateFlag = 0U;
                if (SVC_OK != AmbaKAL_EventFlagClear(&pHandle->CtrlFlagId, SVC_IMG_FEEDER_FLAG_DATA_UPDATE)) {
                    SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) clear data update failed(0x%x)", pHandle->FeederID, RetVal);
                }

                /* Check data ready */
                if ((pHandle->FIFOEnaBits & pHandle->DataRdyFlag) == pHandle->FIFOEnaBits) {
                    if (SVC_OK != AmbaWrap_memset(DataStatusArr, 0, sizeof(DataStatusArr))) {
                        SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) clear DataStatusArr failed(0x%x)", pHandle->FeederID, 0U);
                    }

                    /* Get all FIFO status at the same time */
                    RetVal = ImgFeeder_GetFIFOStatusAll(pHandle, DataStatusArr);
                    if (SVC_OK == RetVal) {
                        DataReady = 1U;
                    } else {
                        SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) get FIFO status failed", pHandle->FeederID, 0U);
                    }
                }

                RetVal = AmbaKAL_MutexGive(&pHandle->DataStMutex);
                if (RetVal != KAL_ERR_NONE) {
                    SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) give DataStMutex failed(0x%x)", pHandle->FeederID, RetVal);
                    RetVal = SVC_NG;
                }

                /* Check the rules */
                if ((RetVal == SVC_OK) && (DataReady == 1U)) {
                    if (SVC_OK != AmbaWrap_memset(RetRdIdxArr, 0, sizeof(RetRdIdxArr))) {
                        SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) clear RetRdIdxArr failed", pHandle->FeederID, 0U);
                    }

                    if (1U == ImgFeeder_CheckCapSeg(pHandle, DataStatusArr, RetRdIdxArr)) {
                        /* Valid */
                        ImgFeeder_Dbg("FDTx[%u]: Match", pHandle->FeederID, 0U, 0U, 0U, 0U);
                    } else {
                        /* Clear data ready, and wait for next data update. */
                        ImgFeeder_Dbg("FDTx[%u]: Mismatch", pHandle->FeederID, 0U, 0U, 0U, 0U);
                        DataReady = 0U;
                    }
                }
            } else {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) take DataStMutex failed(0x%x)", pHandle->FeederID, RetVal);
                RetVal = SVC_NG;
            }
        }

        if ((PortsReady == 1U) && (DataReady == 1U)) {
            /* Clear ready bit before sending. */
            pHandle->DataRdyFlag = 0U;
            pHandle->PortRdyFlag = 0U;
            if (SVC_OK != AmbaKAL_EventFlagClear(&pHandle->CtrlFlagId, SVC_IMG_FEEDER_FLAG_PORT_READY)) {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) clear port ready failed(0x%x)", pHandle->FeederID, RetVal);
            }
            ImgFeeder_Dbg("FDTx[%u]: clr", pHandle->FeederID, 0U, 0U, 0U, 0U);

            for (PortID = 0; PortID < SVC_IMG_FEEDER_PORT_MAX; PortID++) {
                if (1U == pHandle->Ports[PortID].Connect) {
                    /* Prepare frame info */
                    NumInfo = 0U;
                    for (i = 0; i < SVC_CV_INPUT_INFO_MAX_DATA_NUM; i++) {
                        if (1U == pHandle->Ports[PortID].SubPort[i].Used) {
                            FIFOChanID = pHandle->Ports[PortID].SubPort[i].FIFOChanID;

                            if (NULL != DataStatusArr[FIFOChanID].pQueueBase) {
                                /* Peek the specific frame */
                                SvcImgFIFO_CountToPointer(RetRdIdxArr[FIFOChanID],
                                                          DataStatusArr[FIFOChanID].CountLimit,
                                                          DataStatusArr[FIFOChanID].MaxElement,
                                                          &Rp);
                                Offset = Rp * DataStatusArr[FIFOChanID].ElementSize;
                                FrameSend.Info[NumInfo].pBase = &DataStatusArr[FIFOChanID].pQueueBase[Offset];
                                FrameSend.Info[NumInfo].ElementSize = DataStatusArr[FIFOChanID].ElementSize;
                                if (SVC_OK == AmbaWrap_memcpy(&FrameSend.Info[NumInfo].Content,
                                                              &DataStatusArr[FIFOChanID].Content,
                                                              sizeof(SVC_CV_INPUT_IMG_CONTENT_s))) {
                                    NumInfo++;
                                } else {
                                    SvcLog_NG(SVC_LOG_IMG_FEEDER, "Port(%u, %u) memcpy Content failed", PortID, i);
                                }

                                /* Update FIFO status */
                                ImgFeeder_UpdateFIFOReadStatus(&pHandle->FIFO[FIFOChanID], RetRdIdxArr[FIFOChanID]);
                            } else {
                                SvcLog_NG(SVC_LOG_IMG_FEEDER, "DataStatusArr[%d] = NULL", FIFOChanID, 0U);
                            }
                        }
                    }

                    if (NumInfo > 0U) {
                        ImgFeeder_Dbg("FDTx[%u]: send(P%u)", pHandle->FeederID, PortID, 0U, 0U, 0U);

                        /* Dispatch */
                        Port.FeederID     = pHandle->FeederID;
                        Port.PortID       = PortID;
                        FrameSend.NumInfo = NumInfo;
                        RetVal = pHandle->PortCfg[PortID].SendFunc(&Port, SVC_IMG_FEEDER_IMAGE_DATA_READY, &FrameSend);
                        if (SVC_OK != RetVal) {
                            SvcLog_DBG(SVC_LOG_IMG_FEEDER, "Feeder(%u) to Port(%u) error", 0U, 0U);
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) NumInfo = 0", 0U, 0U);
                    }
                }
            }
        } else {
            /* Do nothing */
        }

        ImgFeeder_MutexGive(&pHandle->Mutex);
    } else {
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "IMG_FD[%u]: FlagGet error(0x%x)", pHandle->FeederID, RetVal);
    }

    return SVC_OK;
}

static UINT32 ImgFeeder_HandlerDivisor(void *pHandlePtr)
{
    UINT32 RetVal = SVC_OK;
    SVC_IMG_FEEDER_HANDLE_s *pHandle;
    UINT32 PortsReady = 0U, DataReady = 0U, ActualFlags = 0U, ClearFlags = 0U;
    UINT32 i;
    UINT32 FIFOChanID, PortID, NumInfo, Offset, Rp;
    SVC_IMG_FEEDER_PORT_s Port;
    SVC_IMG_FIFO_DATA_STATUS_s DataStatusArr[SVC_IMG_FIFO_MAX_CHAN] = {0U};
    UINT32 RetRdIdxArr[SVC_IMG_FIFO_MAX_CHAN] = {0U};
    SVC_CV_INPUT_IMG_INFO_s FrameSend;

    AmbaMisra_TouchUnused(pHandlePtr);
    AmbaMisra_TypeCast(&pHandle, &pHandlePtr);

    ImgFeeder_Dbg("FDTx[%u]: wait", pHandle->FeederID, 0U, 0U, 0U, 0U);

    RetVal = AmbaKAL_EventFlagGet(&pHandle->CtrlFlagId,
                                  (SVC_IMG_FEEDER_FLAG_ENABLE |
                                   SVC_IMG_FEEDER_FLAG_RUN |
                                   SVC_IMG_FEEDER_FLAG_PORT_READY |
                                   SVC_IMG_FEEDER_FLAG_DATA_UPDATE),
                                  AMBA_KAL_FLAGS_ALL,
                                  AMBA_KAL_FLAGS_CLEAR_NONE,
                                  &ActualFlags,
                                  AMBA_KAL_WAIT_FOREVER);

    ImgFeeder_Dbg("FDTx[%u]: run", pHandle->FeederID, 0U, 0U, 0U, 0U);

    if (KAL_ERR_NONE == RetVal) {
        ImgFeeder_MutexTake(&pHandle->Mutex);

        ImgFeeder_Dbg("FDTx[%u]: Po(0x%x 0x%x) Dt(0x%x 0x%x)",
            pHandle->FeederID,
            pHandle->PortEnaBits,
            pHandle->PortRdyFlag,
            pHandle->FIFOEnaBits,
            pHandle->DataRdyFlag);

        /* Check port ready */
        if ((pHandle->PortEnaBits & pHandle->PortRdyFlag) == pHandle->PortEnaBits) {
            PortsReady = 1U;

            RetVal = AmbaKAL_MutexTake(&pHandle->DataStMutex, AMBA_KAL_WAIT_FOREVER);
            if (RetVal == KAL_ERR_NONE) {
                /* Get Data ready flag */
                pHandle->DataRdyFlag |= pHandle->DataUpdateFlag;

                /* Clear Data update flag */
                pHandle->DataUpdateFlag = 0U;
                if (SVC_OK != AmbaKAL_EventFlagClear(&pHandle->CtrlFlagId, SVC_IMG_FEEDER_FLAG_DATA_UPDATE)) {
                    SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) clear data update failed(0x%x)", pHandle->FeederID, RetVal);
                }

                /* Check data ready */
                if ((pHandle->FIFOEnaBits & pHandle->DataRdyFlag) == pHandle->FIFOEnaBits) {
                    if (SVC_OK != AmbaWrap_memset(DataStatusArr, 0, sizeof(DataStatusArr))) {
                        SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) clear DataStatusArr failed(0x%x)", pHandle->FeederID, 0U);
                    }

                    /* Get all FIFO status at the same time */
                    RetVal = ImgFeeder_GetFIFOStatusAll(pHandle, DataStatusArr);
                    if (SVC_OK == RetVal) {
                        DataReady = 1U;
                    } else {
                        SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) get FIFO status failed", pHandle->FeederID, 0U);
                    }

                    if (pHandle->RdStInit == 0U) {
                        pHandle->RdStInit = 1U;
                        ImgFeeder_InitFIFOReadStatusAll(pHandle, DataStatusArr);
                        ImgFeeder_Dbg("FDTx[%u]: Init Rd status", pHandle->FeederID, 0U, 0U, 0U, 0U);
                    }
                }

                RetVal = AmbaKAL_MutexGive(&pHandle->DataStMutex);
                if (RetVal != KAL_ERR_NONE) {
                    SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) give DataStMutex failed(0x%x)", pHandle->FeederID, RetVal);
                    RetVal = SVC_NG;
                }

                /* Check the rules */
                if ((RetVal == SVC_OK) && (DataReady == 1U)) {
                    if (SVC_OK != AmbaWrap_memset(RetRdIdxArr, 0, sizeof(RetRdIdxArr))) {
                        SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) clear RetRdIdxArr failed", pHandle->FeederID, 0U);
                    }

                    if (1U == ImgFeeder_CheckDivisor(pHandle, DataStatusArr, RetRdIdxArr)) {
                        /* Valid */
                        ImgFeeder_Dbg("FDTx[%u]: Match", pHandle->FeederID, 0U, 0U, 0U, 0U);
                    } else {
                        /* Clear data ready, and wait for next data update. */
                        ImgFeeder_Dbg("FDTx[%u]: Mismatch", pHandle->FeederID, 0U, 0U, 0U, 0U);
                        DataReady = 0U;
                    }
                }
            } else {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) take DataStMutex failed(0x%x)", pHandle->FeederID, RetVal);
                RetVal = SVC_NG;
            }
        }

        if ((PortsReady == 1U) && (DataReady == 1U)) {
            /* Clear ready bit before sending. */
            pHandle->DataRdyFlag = 0U;
            pHandle->PortRdyFlag = 0U;

            ClearFlags = SVC_IMG_FEEDER_FLAG_PORT_READY;
            if (pHandle->ExtSync != 0U) {
                ClearFlags |= SVC_IMG_FEEDER_FLAG_RUN;
            }
            if (SVC_OK != AmbaKAL_EventFlagClear(&pHandle->CtrlFlagId, ClearFlags)) {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) clear port ready failed(0x%x)", pHandle->FeederID, RetVal);
            }
            ImgFeeder_Dbg("FDTx[%u]: clr", pHandle->FeederID, 0U, 0U, 0U, 0U);

            for (PortID = 0; PortID < SVC_IMG_FEEDER_PORT_MAX; PortID++) {
                if (1U == pHandle->Ports[PortID].Connect) {
                    /* Prepare frame info */
                    NumInfo = 0U;
                    for (i = 0; i < SVC_CV_INPUT_INFO_MAX_DATA_NUM; i++) {
                        if (1U == pHandle->Ports[PortID].SubPort[i].Used) {
                            FIFOChanID = pHandle->Ports[PortID].SubPort[i].FIFOChanID;

                            if (NULL != DataStatusArr[FIFOChanID].pQueueBase) {
                                /* Peek the specific frame */
                                SvcImgFIFO_CountToPointer(RetRdIdxArr[FIFOChanID],
                                                          DataStatusArr[FIFOChanID].CountLimit,
                                                          DataStatusArr[FIFOChanID].MaxElement,
                                                          &Rp);
                                Offset = Rp * DataStatusArr[FIFOChanID].ElementSize;
                                FrameSend.Info[NumInfo].pBase = &DataStatusArr[FIFOChanID].pQueueBase[Offset];
                                FrameSend.Info[NumInfo].ElementSize = DataStatusArr[FIFOChanID].ElementSize;
                                if (SVC_OK == AmbaWrap_memcpy(&FrameSend.Info[NumInfo].Content,
                                                              &DataStatusArr[FIFOChanID].Content,
                                                              sizeof(SVC_CV_INPUT_IMG_CONTENT_s))) {
                                    NumInfo++;
                                } else {
                                    SvcLog_NG(SVC_LOG_IMG_FEEDER, "Port(%u, %u) memcpy Content failed", PortID, i);
                                }

                                /* Update FIFO status */
                                ImgFeeder_UpdateFIFOReadStatus(&pHandle->FIFO[FIFOChanID], RetRdIdxArr[FIFOChanID]);
                            } else {
                                SvcLog_NG(SVC_LOG_IMG_FEEDER, "DataStatusArr[%d] = NULL", FIFOChanID, 0U);
                            }
                        }
                    }

                    if (NumInfo > 0U) {
                        ImgFeeder_Dbg("FDTx[%u]: send(P%u)", pHandle->FeederID, PortID, 0U, 0U, 0U);

                        /* Dispatch */
                        Port.FeederID     = pHandle->FeederID;
                        Port.PortID       = PortID;
                        FrameSend.NumInfo = NumInfo;
                        RetVal = pHandle->PortCfg[PortID].SendFunc(&Port, SVC_IMG_FEEDER_IMAGE_DATA_READY, &FrameSend);
                        if (SVC_OK != RetVal) {
                            SvcLog_DBG(SVC_LOG_IMG_FEEDER, "Feeder(%u) to Port(%u) error", 0U, 0U);
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_IMG_FEEDER, "Feeder(%u) NumInfo = 0", 0U, 0U);
                    }
                }
            }
        } else {
            /* Do nothing */
        }

        ImgFeeder_MutexGive(&pHandle->Mutex);
    } else {
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "IMG_FD[%u]: FlagGet error(0x%x)", pHandle->FeederID, RetVal);
    }

    return SVC_OK;
}

static UINT32 ImgFeeder_FIFOCallback(SVC_IMG_FIFO_CHAN_s *pChan, UINT32 Event, void *pInfo)
{
    UINT32 RetVal;
    UINT32 FeederID;
    SVC_IMG_FEEDER_HANDLE_s *pHandle;
    const SVC_IMG_FEEDER_FIFO_STATUS_s *pFIFOSt;
    const SVC_IMG_FIFO_DATA_STATUS_s *pDataSt;

    AmbaMisra_TouchUnused(pChan);
    AmbaMisra_TouchUnused(pInfo);

    FeederID = pChan->UserTag;
    ImgFeeder_Dbg("FD[%u]: FF%u Rdy", FeederID, pChan->ChanID, 0U, 0U, 0U);

    if ((FeederID < SVC_IMG_FEEDER_MAX_ID) &&
        (pChan->ChanID < SVC_IMG_FIFO_MAX_CHAN) &&
        (NULL != pInfo)) {
        pHandle = &FeederHandle[FeederID];
        pFIFOSt = &pHandle->FIFO[pChan->ChanID];

        if (Event == SVC_IMG_FIFO_DATA_READY) {
            AmbaMisra_TypeCast(&pDataSt, &pInfo);
            if ((pDataSt->Content.DataSrc == pFIFOSt->Content.DataSrc) &&
                (pDataSt->Content.StrmId == pFIFOSt->Content.StrmId)) {

                RetVal = AmbaKAL_MutexTake(&pHandle->DataStMutex, AMBA_KAL_WAIT_FOREVER);
                if (RetVal == KAL_ERR_NONE) {
                    pHandle->DataUpdateFlag = SVC_IMG_FEEDER_ADD_BIT(pHandle->DataUpdateFlag, pChan->ChanID);
                    ImgFeeder_Dbg("FD[%u]: FF%u Dt(0x%x)", FeederID, pChan->ChanID, pHandle->DataUpdateFlag, 0U, 0U);

                    if (KAL_ERR_NONE != AmbaKAL_EventFlagSet(&pHandle->CtrlFlagId, SVC_IMG_FEEDER_FLAG_DATA_UPDATE)) {
                        SvcLog_NG(SVC_LOG_IMG_FEEDER, "FIFOCallback: EventFlagSet error", 0U, 0U);
                    }

                    RetVal = AmbaKAL_MutexGive(&pHandle->DataStMutex);
                    if (RetVal != KAL_ERR_NONE) {
                        SvcLog_NG(SVC_LOG_IMG_FEEDER, "FIFOCallback: Give DataStMutex failed(0x%x)", RetVal, 0U);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_IMG_FEEDER, "FIFOCallback: Take DataStMutex failed(0x%x)", RetVal, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_IMG_FEEDER, "ImgFeeder_FIFOCallback: content not matched", 0U, 0U);
            }
        } else {
            /* Do nothing */
        }
    } else {
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "ImgFeeder_FIFOCallback: invalid info", 0U, 0U);
    }

    return SVC_OK;
}

static UINT32 SvcCvFlow_TaskIdentify(UINT64 *pTaskIdentifier)
{
    UINT32 RetVal;
    AMBA_KAL_TASK_t *pTaskPtr;
#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)
    UINT64 ThreadId = 0U;
    UINT64 *pThreadId = &ThreadId;

    AmbaMisra_TypeCast(&pTaskPtr, &pThreadId);
    RetVal = AmbaKAL_TaskIdentify(&pTaskPtr); /* Return ThreadId */
    if (RetVal == 0U) {
        *pTaskIdentifier = ThreadId;
    } else {
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "SvcCvFlow_TaskIdentify failed", 0U, 0U);
    }
#else
    ULONG TaskAddr = 0U;

    RetVal = AmbaKAL_TaskIdentify(&pTaskPtr); /* Return pointer */
    if (RetVal == 0U) {
        AmbaMisra_TypeCast(&TaskAddr, &pTaskPtr);
        *pTaskIdentifier = (UINT64)TaskAddr;
    } else {
        SvcLog_NG(SVC_LOG_IMG_FEEDER, "SvcCvFlow_TaskIdentify failed", 0U, 0U);
    }
#endif

    return RetVal;
}

