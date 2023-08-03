/**
 *  @file SvcImgFIFO.c
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
 *  @details Implementation of Svc Image FIFO utility
 *
 */

#include "AmbaTypes.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_Event_Def.h"
#include "AmbaDSP_Event.h"
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

#define SVC_LOG_IMG_FIFO         "IMG_FIFO"

#define SVC_IMG_FIFO_SIZE_PER_CHAN       (4096U)
#define SVC_IMG_FIFO_EVENT_HANDLER_TYPES (8U)

static inline UINT32 SVC_IMG_FIFO_ADD_BIT     (UINT32 Bits, UINT32 Idx) {return (Bits | ((UINT32)1U << Idx));}
static inline UINT32 SVC_IMG_FIFO_REMOVE_BIT  (UINT32 Bits, UINT32 Idx) {return (Bits & (~((UINT32)1U << Idx)));}

typedef struct {
    UINT32                        RegBits;
    UINT16                        EventID;
    AMBA_DSP_EVENT_HANDLER_f      EventHandler;
    UINT32                        ElementSize;
} SVC_IMG_FIFO_EVENT_HANDLER_CTRL_s;

typedef struct {
    UINT32                        Created  :1;
    UINT32                        Registered :1;
    UINT32                        Reserved :30;
    SVC_CV_INPUT_IMG_CONTENT_s    Content;
    SVC_IMG_FIFO_CALLBACK_f       pUserCallback[SVC_IMG_FIFO_MAX_USER_PER_CHAN];
    UINT32                        UserTag[SVC_IMG_FIFO_MAX_USER_PER_CHAN];

    AMBA_KAL_MUTEX_t              ChanMutex;
    UINT8                         *pDataQue;
    UINT32                        ElementSize;
    UINT32                        MaxElement;
    UINT32                        Wp;
    UINT32                        WrCount;      /* Number of data being written */
    UINT32                        CountLimit;   /* Maximum of WrCount */
} SVC_IMG_FIFO_CHAN_CTRL_s;

static AMBA_KAL_MUTEX_t         g_Mutex;
static SVC_IMG_FIFO_CHAN_CTRL_s ImgFIFOChan[SVC_IMG_FIFO_MAX_CHAN] GNU_SECTION_NOZEROINIT;
static UINT8  ImgFIFODebugEnable = 0U;

static void   ImgFIFO_Dbg(const char *pFormat, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
static UINT32 ImgFIFO_CreateChan(SVC_IMG_FIFO_CFG_s *pCfg, UINT32 *pChanID);
static UINT32 ImgFIFO_DeleteChan(UINT32 ChanID);
static void   ImgFIFO_GetNextWriteIdx(UINT32 *pWp, UINT32 MaxElement);
static void   ImgFIFO_UpdateWriteIdx(SVC_IMG_FIFO_CHAN_CTRL_s *pChanCtrl);

static UINT32 ImgFIFO_PyramidDataRdyHandler(const void *pEventInfo);
static UINT32 ImgFIFO_YuvDataRdyHandler(const void *pEventInfo);
static UINT32 ImgFIFO_RawDataRdyHandler(const void *pEventInfo);
#if defined(AMBA_DSP_EVENT_LV_MAIN_Y12_RDY)
static UINT32 ImgFIFO_MainY12DataRdyHandler(const void *pEventInfo);
#endif
static UINT32 ImgFIFO_PyramidExDataRdyHandler(const void *pEventInfo);

static SVC_IMG_FIFO_EVENT_HANDLER_CTRL_s DataSrcEventHandler[SVC_IMG_FIFO_EVENT_HANDLER_TYPES] = {
    [SVC_IMG_FIFO_SRC_PYRAMID]  = { 0U, AMBA_DSP_EVENT_LV_PYRAMID_RDY,  ImgFIFO_PyramidDataRdyHandler, (UINT32)sizeof(AMBA_DSP_PYMD_DATA_RDY_s) },
    [SVC_IMG_FIFO_SRC_MAIN_YUV] = { 0U, AMBA_DSP_EVENT_LV_YUV_DATA_RDY, ImgFIFO_YuvDataRdyHandler,     (UINT32)sizeof(AMBA_DSP_YUV_DATA_RDY_s)  },
    [SVC_IMG_FIFO_SRC_YUV_STRM] = { 0U, AMBA_DSP_EVENT_LV_YUV_DATA_RDY, ImgFIFO_YuvDataRdyHandler,     (UINT32)sizeof(AMBA_DSP_YUV_DATA_RDY_s)  },
    [SVC_IMG_FIFO_SRC_RAW_DATA] = { 0U, AMBA_DSP_EVENT_LV_RAW_RDY,      ImgFIFO_RawDataRdyHandler,     (UINT32)sizeof(AMBA_DSP_RAW_DATA_RDY_s)  },
    [SVC_IMG_FIFO_SRC_PROC_RAW] = { 0U, AMBA_DSP_EVENT_LV_RAW_RDY,      ImgFIFO_RawDataRdyHandler,     (UINT32)sizeof(AMBA_DSP_RAW_DATA_RDY_s)  },
#if defined(AMBA_DSP_EVENT_LV_MAIN_Y12_RDY)
    [SVC_IMG_FIFO_SRC_MAIN_Y12] = { 0U, AMBA_DSP_EVENT_LV_MAIN_Y12_RDY, ImgFIFO_MainY12DataRdyHandler, (UINT32)sizeof(AMBA_DSP_YUV_DATA_RDY_s)  },
#else
    [SVC_IMG_FIFO_SRC_MAIN_Y12] = { 0U, 0U,                             NULL,                          (UINT32)0U                               },
#endif
    [SVC_IMG_FIFO_SRC_PYRAMID_EX_SCALE] = { 0U, AMBA_DSP_EVENT_LV_LNDT_RDY,  ImgFIFO_PyramidExDataRdyHandler, (UINT32)sizeof(AMBA_DSP_LNDT_DATA_RDY_s) },
};

static void ImgFIFO_Dbg(const char *pFormat, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    if (ImgFIFODebugEnable > 0U) {
        AmbaPrint_PrintUInt5(pFormat, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}

/**
 * Svc image FIFO debug enable/disable function
 * @param [in] DebugEnable enable/disable debug message
 * return None
 */
void SvcImgFIFO_DebugEnable(UINT32 DebugEnable)
{
    ImgFIFODebugEnable = (UINT8)DebugEnable;
    SvcLog_OK(SVC_LOG_IMG_FIFO, "ImgFIFODebugEnable = %d", ImgFIFODebugEnable, 0U);
}

static void ImgFIFO_MutexTake(AMBA_KAL_MUTEX_t *pMutex)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(pMutex, 5000)) {
        SvcLog_NG(SVC_LOG_IMG_FIFO, "ImgFIFO_MutexTake: timeout", 0U, 0U);
    }
}

static void ImgFIFO_MutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(pMutex)) {
        SvcLog_NG(SVC_LOG_IMG_FIFO, "ImgFIFO_MutexGive: error", 0U, 0U);
    }
}

static UINT32 ImgFIFO_CreateChan(SVC_IMG_FIFO_CFG_s *pCfg, UINT32 *pChanID)
{
    UINT32 RetVal = SVC_OK;
    SVC_IMG_FIFO_CHAN_CTRL_s *pChanCtrl;
    UINT32 i, Exist = 0U, ChanID = 0U;
    UINT16 DataSrc;
    static char MutexName[] = "ImgFIFOChanMutex";
    static UINT8 ImgFIFOQue[SVC_IMG_FIFO_MAX_CHAN][SVC_IMG_FIFO_SIZE_PER_CHAN] GNU_SECTION_NOZEROINIT;
    #define MAX_VALUE_U32       0xFFFFFFFFU

    AmbaMisra_TouchUnused(pCfg);

    /* Search if any existing channel matched */
    for (i = 0; i < SVC_IMG_FIFO_MAX_CHAN; i++) {
        pChanCtrl = &ImgFIFOChan[i];
        if ((pChanCtrl->Created == 1U) &&
            (pChanCtrl->Content.DataSrc == pCfg->Content.DataSrc) &&
            (pChanCtrl->Content.StrmId == pCfg->Content.StrmId)) {
            Exist = 1U;
            ChanID = i;
            break;
        }
    }

    if (Exist == 0U) {
        /* Get new FIFO channel */
        for (i = 0; i < SVC_IMG_FIFO_MAX_CHAN; i++) {
            if (0U == ImgFIFOChan[i].Created) {
                break;
            }
        }

        if (i < SVC_IMG_FIFO_MAX_CHAN) {
            ChanID = i;
            pChanCtrl = &ImgFIFOChan[ChanID];
            AmbaSvcWrap_MisraMemset(pChanCtrl, 0, sizeof(SVC_IMG_FIFO_CHAN_CTRL_s));

            pChanCtrl->Created = 1U;
            RetVal = AmbaWrap_memcpy(&pChanCtrl->Content, &pCfg->Content, sizeof(SVC_CV_INPUT_IMG_CONTENT_s));
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_IMG_FIFO, "memcpy Content failed", 0U, 0U);
            }

            /* Hook data queue */
            pChanCtrl->pDataQue = &ImgFIFOQue[ChanID][0];

            /* Create chan mutex */
            if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&pChanCtrl->ChanMutex, MutexName)) {
                SvcLog_NG(SVC_LOG_IMG_FIFO, "CreateChan: MutexCreate error", 0U, 0U);
                RetVal |= SVC_NG;
            }
        } else {
            SvcLog_NG(SVC_LOG_IMG_FIFO, "CreateChan: no empty chan", 0U, 0U);
            RetVal = SVC_NG;
        }
    }

    if (RetVal == SVC_OK) {
        *pChanID = ChanID;
        pChanCtrl = &ImgFIFOChan[ChanID];

        /* Register event callback if it has not been registered. */
        DataSrc = pCfg->Content.DataSrc;
        if ((DataSrc < SVC_IMG_FIFO_EVENT_HANDLER_TYPES) && (NULL != DataSrcEventHandler[DataSrc].EventHandler)) {
            if (DataSrcEventHandler[DataSrc].RegBits == 0U) {
                RetVal = AmbaDSP_EventHandlerRegister(DataSrcEventHandler[DataSrc].EventID,
                                                      DataSrcEventHandler[DataSrc].EventHandler);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_IMG_FIFO, "Register Data Rdy event error(0x%X)", RetVal, 0U);
                }
            }
            DataSrcEventHandler[DataSrc].RegBits = SVC_IMG_FIFO_ADD_BIT(DataSrcEventHandler[DataSrc].RegBits, ChanID);

            pChanCtrl->ElementSize = DataSrcEventHandler[DataSrc].ElementSize;
            pChanCtrl->MaxElement = (SVC_IMG_FIFO_SIZE_PER_CHAN / pChanCtrl->ElementSize) - 1U;
            pChanCtrl->Wp = pChanCtrl->MaxElement;      /* End of FIFO */
            /* Example: if MaxElement = 10, CountLimit(UINT8) = 250, WrCount(UINT8) = 1 ~ 250 */
            pChanCtrl->WrCount = 0U;
            pChanCtrl->CountLimit = (MAX_VALUE_U32 / pChanCtrl->MaxElement) * pChanCtrl->MaxElement;
        } else {
            SvcLog_NG(SVC_LOG_IMG_FIFO, "Unsupported DataSrc(0x%X)", DataSrc, 0U);
        }

        ImgFIFO_Dbg("FF[%u]: Create DataSrc(%u) StrmId(%u) ElementSize(%u) MaxElement(%u)",
            i, pCfg->Content.DataSrc, pCfg->Content.StrmId, pChanCtrl->ElementSize, pChanCtrl->MaxElement);
    } else {
        SvcLog_NG(SVC_LOG_IMG_FIFO, "Create FIFO Chan failed", 0U, 0U);
    }

    return RetVal;
}

static UINT32 ImgFIFO_DeleteChan(UINT32 ChanID)
{
    UINT32 RetVal = SVC_OK;
    SVC_IMG_FIFO_CHAN_CTRL_s *pChanCtrl;
    UINT16 DataSrc;

    if (ChanID < SVC_IMG_FIFO_MAX_CHAN) {
        pChanCtrl = &ImgFIFOChan[ChanID];
        if ((1U == pChanCtrl->Created) &&
            (0U == pChanCtrl->Registered)) {
            pChanCtrl->Created = 0U;

            /* UnRegister event callback */
            DataSrc = pChanCtrl->Content.DataSrc;
            if (DataSrc < SVC_IMG_FIFO_EVENT_HANDLER_TYPES) {
                DataSrcEventHandler[DataSrc].RegBits = SVC_IMG_FIFO_REMOVE_BIT(DataSrcEventHandler[DataSrc].RegBits, ChanID);
                if (DataSrcEventHandler[DataSrc].RegBits == 0U) {
                    RetVal = AmbaDSP_EventHandlerUnRegister(DataSrcEventHandler[DataSrc].EventID,
                                                            DataSrcEventHandler[DataSrc].EventHandler);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_IMG_FIFO, "UnRegister Data Rdy event error(0x%X)", RetVal, 0U);
                    }
                }
            } else {
                SvcLog_NG(SVC_LOG_IMG_FIFO, "Unsupported DataSrc(0x%X)", DataSrc, 0U);
            }

            /* Delete chan mutex */
            if (KAL_ERR_NONE != AmbaKAL_MutexDelete(&pChanCtrl->ChanMutex)) {
                SvcLog_NG(SVC_LOG_IMG_FIFO, "DeleteChan: MutexDelete error", 0U, 0U);
                RetVal |= SVC_NG;
            }
        } else {
            SvcLog_NG(SVC_LOG_IMG_FIFO, "DeleteChan: chan(%u) is still used", ChanID, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_IMG_FIFO, "DeleteChan: invalid chan(%u)", ChanID, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

static void ImgFIFO_GetNextWriteIdx(UINT32 *pWp, UINT32 MaxElement)
{
    UINT32 Wp = *pWp;

    Wp++;
    if (Wp >= MaxElement) {
        Wp = 0U;
    }
    *pWp = Wp;
}

static void ImgFIFO_UpdateWriteIdx(SVC_IMG_FIFO_CHAN_CTRL_s *pChanCtrl)
{
    UINT32 Wp = 0U;

    ImgFIFO_GetNextWriteIdx(&pChanCtrl->Wp, pChanCtrl->MaxElement);
    SvcImgFIFO_CountIncrement(&pChanCtrl->WrCount, pChanCtrl->CountLimit, 1U);

    /* Sanity check */
    SvcImgFIFO_CountToPointer(pChanCtrl->WrCount, pChanCtrl->CountLimit, pChanCtrl->MaxElement, &Wp);
    if (Wp != pChanCtrl->Wp) {
        SvcLog_NG(SVC_LOG_IMG_FIFO, "ImgFIFO_UpdateWriteIdx: WriteIdx(%u, %u) out of sync", Wp, pChanCtrl->Wp);
    }
}

static void ImgFIFO_FIFODataPut(SVC_IMG_FIFO_CHAN_CTRL_s *pChanCtrl, const void *pEventInfo)
{
    UINT32 RetVal;
    UINT32 Offset, Wp = 0U;

    /* Copy to FIFO */
    Wp = pChanCtrl->Wp;
    ImgFIFO_GetNextWriteIdx(&Wp, pChanCtrl->MaxElement);
    Offset = Wp * pChanCtrl->ElementSize;
    RetVal = AmbaWrap_memcpy(&pChanCtrl->pDataQue[Offset], pEventInfo, pChanCtrl->ElementSize);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_IMG_FIFO, "memcpy pEventInfo failed", 0U, 0U);
    }

    ImgFIFO_MutexTake(&pChanCtrl->ChanMutex);
    ImgFIFO_UpdateWriteIdx(pChanCtrl);
    ImgFIFO_MutexGive(&pChanCtrl->ChanMutex);
}

static void ImgFIFO_DispatchToUser(UINT32 ChanID, SVC_IMG_FIFO_CHAN_CTRL_s *pChanCtrl)
{
    UINT32 RetVal;
    UINT32 UserIdx;
    SVC_IMG_FIFO_CHAN_s Channel = {0};
    SVC_IMG_FIFO_DATA_STATUS_s DataRdy = {0};

    AmbaMisra_TouchUnused(pChanCtrl);

    Channel.ChanID          = (UINT8)ChanID;
    DataRdy.Content.DataSrc = pChanCtrl->Content.DataSrc;
    DataRdy.Content.StrmId  = pChanCtrl->Content.StrmId;
    DataRdy.pQueueBase      = pChanCtrl->pDataQue;
    DataRdy.ElementSize     = pChanCtrl->ElementSize;
    DataRdy.MaxElement      = pChanCtrl->MaxElement;
    DataRdy.Wp              = pChanCtrl->Wp;
    DataRdy.WrCount         = pChanCtrl->WrCount;
    DataRdy.CountLimit      = pChanCtrl->CountLimit;

    ImgFIFO_Dbg("FF[%u]: Send D%u S%u Wp%u", ChanID, DataRdy.Content.DataSrc, DataRdy.Content.StrmId, DataRdy.Wp, 0U);

    /* Broadcast to users */
    for (UserIdx = 0; UserIdx < SVC_IMG_FIFO_MAX_USER_PER_CHAN; UserIdx++) {
        if (NULL != pChanCtrl->pUserCallback[UserIdx]) {
            Channel.UserID  = (UINT8)UserIdx;
            Channel.UserTag = pChanCtrl->UserTag[UserIdx];
            RetVal = pChanCtrl->pUserCallback[UserIdx](&Channel, SVC_IMG_FIFO_DATA_READY, &DataRdy);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_IMG_FIFO, "UserCallback(%u, %u) returns error", ChanID, UserIdx);
            }
        }
    }
}

/**
* Initialize image FIFO utility
* @param [in] pCfg initialization configuration of image FIFO
* @return 0-OK, 1-NG
*/
UINT32 SvcImgFIFO_Init(SVC_IMG_FIFO_INIT_CFG_s *pCfg)
{
    UINT32 RetVal;
    static char MutexName[] = "ImgFIFOMutex";

    AmbaMisra_TouchUnused(pCfg);

    if (NULL != pCfg) {
        RetVal = AmbaWrap_memset(ImgFIFOChan, 0, sizeof(SVC_IMG_FIFO_CHAN_CTRL_s) * SVC_IMG_FIFO_MAX_CHAN);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_IMG_FIFO, "Init: memset ImgFIFOChan error", 0U, 0U);
        }

        /* Create global mutex */
        if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&g_Mutex, MutexName)) {
            SvcLog_NG(SVC_LOG_IMG_FIFO, "Init: MutexCreate error", 0U, 0U);
            RetVal |= SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_IMG_FIFO, "Init: invalid param", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Register to listen specific content
* @param [in] pCfg configuration of image FIFO
* @param [out] pChan FIFO channel
* @return 0-OK, 1-NG
*/
UINT32 SvcImgFIFO_Register(SVC_IMG_FIFO_CFG_s *pCfg, SVC_IMG_FIFO_CHAN_s *pChan)
{
    UINT32 RetVal = SVC_OK;
    UINT32 ChanID = 0U;
    SVC_IMG_FIFO_CHAN_CTRL_s *pChanCtrl;
    UINT32 i;

    AmbaMisra_TouchUnused(pCfg);

    ImgFIFO_MutexTake(&g_Mutex);

    /* Create if there is no existing chan matched */
    RetVal = ImgFIFO_CreateChan(pCfg, &ChanID);
    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_IMG_FIFO, "Register: create chan failed", 0U, 0U);
    }

    if (SVC_OK == RetVal) {
        pChanCtrl = &ImgFIFOChan[ChanID];
        if (1U == pChanCtrl->Created) {
            /* Get new user ID */
            for (i = 0; i < SVC_IMG_FIFO_MAX_USER_PER_CHAN; i++) {
                if (NULL == pChanCtrl->pUserCallback[i]) {
                    break;
                }
            }

            if (i < SVC_IMG_FIFO_MAX_USER_PER_CHAN) {
                pChanCtrl->UserTag[i]       = pCfg->UserTag;
                pChanCtrl->pUserCallback[i] = pCfg->pCallback;
                pChanCtrl->Registered       = 1U;
                pChan->ChanID  = (UINT8)ChanID;
                pChan->UserID  = (UINT8)i;
                pChan->UserTag = pCfg->UserTag;

                ImgFIFO_Dbg("FF[%u]: Register User(%u) done", ChanID, i, 0U, 0U, 0U);
            } else {
                SvcLog_NG(SVC_LOG_IMG_FIFO, "Register: chan(%d) is full", ChanID, 0U);
                RetVal = SVC_NG;
            }
        } else {
            SvcLog_NG(SVC_LOG_IMG_FIFO, "Register: chan(%d) has not created", ChanID, 0U);
            RetVal = SVC_NG;
        }
    }

    ImgFIFO_MutexGive(&g_Mutex);

    return RetVal;
}

/**
* Unregister image FIFO channel
* @param [in] pChan FIFO channel
* @return 0-OK, 1-NG
*/
UINT32 SvcImgFIFO_Unregister(SVC_IMG_FIFO_CHAN_s *pChan)
{
    UINT32 RetVal = SVC_OK;
    SVC_IMG_FIFO_CHAN_CTRL_s *pChanCtrl;
    UINT32 i = 0;

    AmbaMisra_TouchUnused(pChan);

    ImgFIFO_MutexTake(&g_Mutex);

    if ((NULL != pChan) &&
        (pChan->ChanID < SVC_IMG_FIFO_MAX_CHAN) &&
        (pChan->UserID < SVC_IMG_FIFO_MAX_USER_PER_CHAN)) {
        pChanCtrl = &ImgFIFOChan[pChan->ChanID];
        if (1U == pChanCtrl->Created) {
            if (1U == pChanCtrl->Registered) {
                pChanCtrl->UserTag[pChan->UserID]       = 0U;
                pChanCtrl->pUserCallback[pChan->UserID] = NULL;

                for (i = 0; i < SVC_IMG_FIFO_MAX_USER_PER_CHAN; i++) {
                    if (NULL != pChanCtrl->pUserCallback[i]) {
                        break;
                    }
                }

                /* If no one listens to the channel */
                if (i >= SVC_IMG_FIFO_MAX_USER_PER_CHAN) {
                    pChanCtrl->Registered = 0U;

                    RetVal = ImgFIFO_DeleteChan(pChan->ChanID);
                    if (RetVal != SVC_OK) {
                        SvcLog_NG(SVC_LOG_IMG_FIFO, "UnRegister: Delete chan failed", 0U, 0U);
                    }
                }

                ImgFIFO_Dbg("FF[%u]: UnRegister User(%u)", pChan->ChanID, pChan->UserID, 0U, 0U, 0U);
            } else {
                SvcLog_NG(SVC_LOG_IMG_FIFO, "UnRegister: chan(%d) User(%d) has not Registered", pChan->ChanID, pChan->UserID);
                RetVal = SVC_NG;
            }
        } else {
            SvcLog_NG(SVC_LOG_IMG_FIFO, "UnRegister: chan(%d) has not created", pChan->ChanID, 0U);
            RetVal = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_IMG_FIFO, "Unregister: invalid param", 0U, 0U);
        RetVal = SVC_NG;
    }

    ImgFIFO_MutexGive(&g_Mutex);

    return RetVal;
}

/**
* Get FIFO status
* @param [in] pChan FIFO channel
* @param [out] pStatus FIFO data status
* @return 0-OK, 1-NG
*/
UINT32 SvcImgFIFO_GetFIFOStatus(SVC_IMG_FIFO_CHAN_s *pChan, SVC_IMG_FIFO_DATA_STATUS_s *pStatus)
{
    UINT32 RetVal = SVC_OK;
    const SVC_IMG_FIFO_CHAN_CTRL_s *pChanCtrl;

    AmbaMisra_TouchUnused(pChan);

    ImgFIFO_MutexTake(&g_Mutex);

    if ((NULL != pChan) &&
        (pChan->ChanID < SVC_IMG_FIFO_MAX_CHAN) &&
        (pChan->UserID < SVC_IMG_FIFO_MAX_USER_PER_CHAN)) {
        pChanCtrl = &ImgFIFOChan[pChan->ChanID];
        pStatus->Content.DataSrc = pChanCtrl->Content.DataSrc;
        pStatus->Content.StrmId  = pChanCtrl->Content.StrmId;
        pStatus->pQueueBase      = pChanCtrl->pDataQue;
        pStatus->ElementSize     = pChanCtrl->ElementSize;
        pStatus->MaxElement      = pChanCtrl->MaxElement;
        pStatus->Wp              = pChanCtrl->Wp;
        pStatus->WrCount         = pChanCtrl->WrCount;
        pStatus->CountLimit      = pChanCtrl->CountLimit;
    } else {
        SvcLog_NG(SVC_LOG_IMG_FIFO, "GetFIFOStatus: invalid param", 0U, 0U);
        RetVal = SVC_NG;
    }

    ImgFIFO_MutexGive(&g_Mutex);

    return RetVal;
}

void SvcImgFIFO_CountToPointer(UINT32 Count, UINT32 CountLimit, UINT32 MaxElement, UINT32 *pPointer)
{
    if ((Count != 0U) && (Count <= CountLimit)) {
        *pPointer = (Count - 1U) % MaxElement;
    } else {
        SvcLog_NG(SVC_LOG_IMG_FIFO, "CountToPointer: Count(%u) CountLimit(%u)", Count, CountLimit);
    }
}

void SvcImgFIFO_CountIncrement(UINT32 *pCount, UINT32 CountLimit, UINT32 Stride)
{
    UINT32 Count = *pCount;
    UINT32 Tail;

    if (Count <= CountLimit) {
        Tail = CountLimit - Count;

        if (Tail >= Stride) {
            Count += Stride;
        } else {
            Count = Stride - Tail;
        }
        *pCount = Count;

        if ((Count == 0U) || (Count > CountLimit)) {
            SvcLog_NG(SVC_LOG_IMG_FIFO, "CountIncrement failed. New Count(%u) CountLimit(%u)", Count, CountLimit);
        }
    } else {
        SvcLog_NG(SVC_LOG_IMG_FIFO, "CountIncrement failed. Ori Count(%u) CountLimit(%u)", Count, CountLimit);
    }

}

/**
* DSP pyramid data ready handler
*/
static UINT32 ImgFIFO_PyramidDataRdyHandler(const void *pEventInfo)
{
    const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo;
    SVC_IMG_FIFO_CHAN_CTRL_s *pChanCtrl;
    UINT32 i;

    AmbaMisra_TypeCast(&pYuvInfo, &pEventInfo);

    for (i = 0; i < SVC_IMG_FIFO_MAX_CHAN; i++) {
        pChanCtrl = &ImgFIFOChan[i];
        if ((pChanCtrl->Registered == 1U) &&
            (pChanCtrl->Content.DataSrc == SVC_IMG_FIFO_SRC_PYRAMID) &&
            (pChanCtrl->Content.StrmId == pYuvInfo->ViewZoneId)) {
            //ImgFIFO_Dbg("FF: Seq(%llu)", pYuvInfo->CapSequence, 0U, 0U, 0U, 0U);

            /* Copy to FIFO */
            ImgFIFO_FIFODataPut(pChanCtrl, pEventInfo);
            ImgFIFO_DispatchToUser(i, pChanCtrl);
            break;
        }
    }

    return SVC_OK;
}

/**
* DSP pyramid extension scaler data ready handler
*/
static UINT32 ImgFIFO_PyramidExDataRdyHandler(const void *pEventInfo)
{
    const AMBA_DSP_LNDT_DATA_RDY_s *pYuvInfo;
    SVC_IMG_FIFO_CHAN_CTRL_s *pChanCtrl;
    UINT32 i;

    AmbaMisra_TypeCast(&pYuvInfo, &pEventInfo);

    for (i = 0; i < SVC_IMG_FIFO_MAX_CHAN; i++) {
        pChanCtrl = &ImgFIFOChan[i];
        if ((pChanCtrl->Registered == 1U) &&
            (pChanCtrl->Content.DataSrc == SVC_IMG_FIFO_SRC_PYRAMID_EX_SCALE) &&
            (pChanCtrl->Content.StrmId == pYuvInfo->ViewZoneId)) {
            //ImgFIFO_Dbg("FF: Seq(%llu)", pYuvInfo->CapSequence, 0U, 0U, 0U, 0U);

            /* Copy to FIFO */
            ImgFIFO_FIFODataPut(pChanCtrl, pEventInfo);
            ImgFIFO_DispatchToUser(i, pChanCtrl);
            break;
        }
    }

    return SVC_OK;
}


/**
* DSP yuv data ready handler
*/
static UINT32 ImgFIFO_YuvDataRdyHandler(const void *pEventInfo)
{
    const AMBA_DSP_YUV_DATA_RDY_s *pYuvInfo;
    SVC_IMG_FIFO_CHAN_CTRL_s *pChanCtrl;
    UINT32 i;

    AmbaMisra_TypeCast(&pYuvInfo, &pEventInfo);

    for (i = 0; i < SVC_IMG_FIFO_MAX_CHAN; i++) {
        pChanCtrl = &ImgFIFOChan[i];
        if ((pChanCtrl->Registered == 1U) &&
            ((pChanCtrl->Content.DataSrc == SVC_IMG_FIFO_SRC_MAIN_YUV) || (pChanCtrl->Content.DataSrc == SVC_IMG_FIFO_SRC_YUV_STRM)) &&
            (pChanCtrl->Content.StrmId == pYuvInfo->ViewZoneId)) {
            //ImgFIFO_Dbg("FF: Seq(%llu)", pYuvInfo->CapSequence, 0U, 0U, 0U, 0U);

            /* Copy to FIFO */
            ImgFIFO_FIFODataPut(pChanCtrl, pEventInfo);
            ImgFIFO_DispatchToUser(i, pChanCtrl);

            break;
        }
    }

    return SVC_OK;
}

/**
* DSP raw data ready handler
*/
static UINT32 ImgFIFO_RawDataRdyHandler(const void *pEventInfo)
{
    #define PROC_RAW_BIT   0x8U
    const AMBA_DSP_RAW_DATA_RDY_s *pRawInfo;
    SVC_IMG_FIFO_CHAN_CTRL_s *pChanCtrl;
    UINT32 i;

    AmbaMisra_TypeCast(&pRawInfo, &pEventInfo);

    for (i = 0; i < SVC_IMG_FIFO_MAX_CHAN; i++) {
        pChanCtrl = &ImgFIFOChan[i];
        if ((pChanCtrl->Registered == 1U) &&
            (pChanCtrl->Content.StrmId == pRawInfo->VinId)) {
            if ((pChanCtrl->Content.DataSrc == SVC_IMG_FIFO_SRC_RAW_DATA) ||
                ((pChanCtrl->Content.DataSrc == SVC_IMG_FIFO_SRC_PROC_RAW) && (((UINT32)pRawInfo->IsVirtChan & PROC_RAW_BIT) > 0U))) {
                    ImgFIFO_Dbg("FF: VinId(%llu) VChan(%u)", pRawInfo->VinId, pRawInfo->IsVirtChan, 0U, 0U, 0U);

                    /* Copy to FIFO */
                    ImgFIFO_FIFODataPut(pChanCtrl, pEventInfo);
                    ImgFIFO_DispatchToUser(i, pChanCtrl);

                    break;
            }
        }
    }

    return SVC_OK;
}

#if defined(AMBA_DSP_EVENT_LV_MAIN_Y12_RDY)
/**
* DSP main Y12 data ready handler
*/
static UINT32 ImgFIFO_MainY12DataRdyHandler(const void *pEventInfo)
{
    const AMBA_DSP_YUV_DATA_RDY_s *pYuvInfo;
    SVC_IMG_FIFO_CHAN_CTRL_s *pChanCtrl;
    UINT32 i;

    AmbaMisra_TypeCast(&pYuvInfo, &pEventInfo);

    for (i = 0; i < SVC_IMG_FIFO_MAX_CHAN; i++) {
        pChanCtrl = &ImgFIFOChan[i];
        if ((pChanCtrl->Registered == 1U) &&
            (pChanCtrl->Content.DataSrc == SVC_IMG_FIFO_SRC_MAIN_Y12) &&
            (pChanCtrl->Content.StrmId == pYuvInfo->ViewZoneId)) {
            //ImgFIFO_Dbg("FF: Seq(%llu)", pYuvInfo->CapSequence, 0U, 0U, 0U, 0U);

            /* Copy to FIFO */
            ImgFIFO_FIFODataPut(pChanCtrl, pEventInfo);
            ImgFIFO_DispatchToUser(i, pChanCtrl);

            break;
        }
    }

    return SVC_OK;
}
#endif
