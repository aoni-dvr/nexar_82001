/**
 *  @file AmbaDSP_EventCtrl.c
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
 *  @details Ambarella DSP internal event handler and event flag APIs
 *
 */

#include "AmbaDSP.h"
#include "AmbaDSP_EventCtrl.h"
#include "AmbaDSP_Log.h"
#include "AmbaDSP_Priv.h"
#include "AmbaDSP_Buffers.h"
#include "AmbaDSP_CommonAPI.h"

#define WAIT_DSP_EVENT_MUTEX_TIMEOUT_10S (10000U)

static UINT32 DSP_EventInitFlag = 0U;
static DSP_EVENT_HANDLER_CTRL_s DSP_EventHandler[LL_EVENT_NUM_ALL] GNU_SECTION_NOZEROINIT;

static osal_even_t DSP_EventFlag[DSP_NUM_EVENT_TYPE] GNU_SECTION_NOZEROINIT;
static osal_even_t DSP_VinEventFlag[DSP_VIN_MAX_NUM] GNU_SECTION_NOZEROINIT;
static osal_even_t DSP_VprocEventFlag[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
static osal_even_t DSP_VoutEventFlag[AMBA_DSP_MAX_VOUT_NUM] GNU_SECTION_NOZEROINIT;

static AMBA_DSP_EVENT_INFO_CTRL_s EventInfoCtrl[EVENT_INFO_POOL_NUM] GNU_SECTION_NOZEROINIT;

/**
 * Lock mutex of a specified event ID. If ok, assign the handler ctrl with specified ID
 * @param [in] EventID Event ID
 * @param [out] ppEventHdlr Pointer to event handler object
 * @return 0 - OK, others - ErrorCode
 */
static inline UINT32 EventTakeMtx(UINT16 EventID, DSP_EVENT_HANDLER_CTRL_s **ppEventHdlr)
{
    UINT32 Rval = OK;

    if (DSP_EventInitFlag != 1U) {
        Rval = DSP_EventInit();
    }

    if (EventID >= LL_EVENT_NUM_ALL) {
        Rval = DSP_ERR_0000;
    } else {
        if (Rval == OK) {
            *ppEventHdlr = &DSP_EventHandler[EventID];

            if (dsp_osal_mutex_lock(&(*ppEventHdlr)->Mtx, WAIT_DSP_EVENT_MUTEX_TIMEOUT_10S) != OK) {
                AmbaLL_LogUInt5("[Err][%d] Get mutex failed for DSP event (%u)", __LINE__, EventID, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }
        }
    }
    return Rval;
}

/**
 * Release mutex of a specified event ID
 * @param [in] EventID Event ID
 * @param [out] ppEventHdlr Pointer to event handler object
 * @return 0 - OK, others - ErrorCode
 */
static inline UINT32 EventGiveMtx(DSP_EVENT_HANDLER_CTRL_s *pEventHdlr)
{
    UINT32 Rval = OK;

    if (dsp_osal_mutex_unlock(&pEventHdlr->Mtx) != OK) {
        AmbaLL_LogUInt5("[Err][%d] Release mutex failed", __LINE__, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0003;
    }

    return Rval;
}

static UINT32 EventFlagInit(void)
{
    UINT32 Rval = OK;
    UINT16 i;
    static char EventFlagName[] = "EventFlag";
    static char VinEventFlagName[] = "VinEventFlag";
    static char VprocEventFlagName[] = "VprocEventFlag";
    static char VoutEventFlagName[] = "VoutEventFlag";

    for (i = 0U; i < DSP_NUM_EVENT_TYPE; i++) {
        if (dsp_osal_eventflag_init(&DSP_EventFlag[i], EventFlagName) != OK) {
            AmbaLL_LogUInt5("[Err] Create event flag failed", 0U, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
            break;
        }
    }
    for (i = 0U; i < DSP_VIN_MAX_NUM; i++) {
        if (dsp_osal_eventflag_init(&DSP_VinEventFlag[i], VinEventFlagName) != OK) {
            AmbaLL_LogUInt5("[Err] Create vin event flag failed", 0U, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
            break;
        }
    }
    for (i = 0U; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        if (dsp_osal_eventflag_init(&DSP_VprocEventFlag[i], VprocEventFlagName) != OK) {
            AmbaLL_LogUInt5("[Err] Create vproc event flag failed", 0U, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
            break;
        }
    }
    for (i = 0U; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
        if (dsp_osal_eventflag_init(&DSP_VoutEventFlag[i], VoutEventFlagName) != OK) {
            AmbaLL_LogUInt5("[Err] Create vout event flag failed", 0U, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
            break;
        }
    }

    return Rval;
}

UINT32 DSP_EventInit(void)
{
    static const UINT32 LL_EvtInfoPoolSizeMap[EVENT_INFO_POOL_NUM] = {
        [EVENT_INFO_POOL_PROT_BUF]      = DSP_EVENT_INFO_POOL_DEPTH,
        [EVENT_INFO_POOL_STAT_CFA]      = DSP_3A_BUFFER,
        [EVENT_INFO_POOL_STAT_PG]       = DSP_3A_BUFFER,
        [EVENT_INFO_POOL_STAT_HIST0]    = DSP_3A_BUFFER,
        [EVENT_INFO_POOL_STAT_HIST1]    = DSP_3A_BUFFER,
        [EVENT_INFO_POOL_STAT_HIST2]    = DSP_3A_BUFFER,
    };
    UINT32 Rval = OK;
    UINT16 i, j;
    char MtxName[] = "EventFlag";
    char InfoMtxName[] = "EventInfoPool";

    if (DSP_EventInitFlag == 0U) {
        Rval = dsp_osal_memset(DSP_EventHandler, 0, sizeof(DSP_EventHandler));
        LL_PrintErrLine(Rval, __func__, __LINE__);

        for (i = 0U; i < LL_EVENT_NUM_ALL; i++) {
            Rval = dsp_osal_mutex_init(&(DSP_EventHandler[i].Mtx), MtxName);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[Err] Create Event mutex failed[%d], DspDrv can't run!", Rval, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
                break;
            }
        }

        if (Rval == OK) {
            Rval = dsp_osal_memset(DSP_EventFlag, 0, sizeof(DSP_EventFlag));
            LL_PrintErrLine(Rval, __func__, __LINE__);
            Rval = dsp_osal_memset(DSP_VinEventFlag, 0, sizeof(DSP_VinEventFlag));
            LL_PrintErrLine(Rval, __func__, __LINE__);
            Rval = dsp_osal_memset(DSP_VprocEventFlag, 0, sizeof(DSP_VprocEventFlag));
            LL_PrintErrLine(Rval, __func__, __LINE__);
            Rval = dsp_osal_memset(DSP_VoutEventFlag, 0, sizeof(DSP_VoutEventFlag));
            LL_PrintErrLine(Rval, __func__, __LINE__);

            //EventInfoPool
            Rval = dsp_osal_memset(EventInfoCtrl, 0, sizeof(EventInfoCtrl));
            LL_PrintErrLine(Rval, __func__, __LINE__);
            for (i = 0U; i < EVENT_INFO_POOL_NUM; i++) {
                EventInfoCtrl[i].PoolDesc.Size = LL_EvtInfoPoolSizeMap[i];
                EventInfoCtrl[i].PoolDesc.Wp = LL_EvtInfoPoolSizeMap[i] - 1U;
                EventInfoCtrl[i].PoolDesc.Rp = LL_EvtInfoPoolSizeMap[i] - 1U;
                for (j = 0U; j < DSP_EVENT_INFO_POOL_DEPTH; j++) {
                    EventInfoCtrl[i].Used[j] = 0U;
                }
                Rval = dsp_osal_mutex_init(&(EventInfoCtrl[i].Mtx), InfoMtxName);
                LL_PrintErrLine(Rval, __func__, __LINE__);
            }

            Rval |= EventFlagInit();
            if (Rval == OK) {
                DSP_EventInitFlag = 1U;
            }
        }
    }

    return Rval;
}

UINT32 DSP_EventHandlerConfig(UINT16 EventID, UINT8 Type, UINT32 MaxNumHandlers, AMBA_DSP_EVENT_HANDLER_f *pEventHandlers)
{
    UINT32 Rval;
    DSP_EVENT_HANDLER_CTRL_s *pEventHdlrCtrl;

    if (pEventHandlers == NULL) {
        Rval = DSP_ERR_0000;
    } else if (EventTakeMtx(EventID, &pEventHdlrCtrl) != OK) {
        Rval = DSP_ERR_0003;
    } else {
        switch (Type) {
        case LL_EVENT_HANDLER:
            pEventHdlrCtrl->Main.MaxNumHandlers = MaxNumHandlers;
            pEventHdlrCtrl->Main.pEventHandlers = pEventHandlers;
            break;
        case LL_EVENT_HANDLER_PRE_PROC:
            pEventHdlrCtrl->PreProc.MaxNumHandlers = MaxNumHandlers;
            pEventHdlrCtrl->PreProc.pEventHandlers = pEventHandlers;
            break;
        case LL_EVENT_HANDLER_POST_PROC:
            pEventHdlrCtrl->PostProc.MaxNumHandlers = MaxNumHandlers;
            pEventHdlrCtrl->PostProc.pEventHandlers = pEventHandlers;
            break;
        default:
            AmbaLL_LogUInt5("[Err][%d] Unknown DSP event type[%d] for the ID[%d]", __LINE__, Type, EventID, 0U, 0U);
            break;
        }

        Rval = EventGiveMtx(pEventHdlrCtrl);
    }

    return Rval;
}

UINT32 DSP_RegisterEventHandler(UINT16 EventID, UINT8 Type, AMBA_DSP_EVENT_HANDLER_f EventHandler)
{
    UINT32 Rval = OK;
    DSP_EVENT_HANDLER_CTRL_s  *pEventHdlrCtrl;
    const DSP_EVENT_HANDLER_s *pEventHdlr = NULL;
    AMBA_DSP_EVENT_HANDLER_f  *pEventHdlrFunc;
    UINT32 Idx;
    UINT32 IdxMax = 0;

    if (EventTakeMtx(EventID, &pEventHdlrCtrl) != OK) {
        Rval = DSP_ERR_0003;
    } else {
        switch (Type) {
        case LL_EVENT_HANDLER:
            pEventHdlr = &(pEventHdlrCtrl->Main);
            break;
        case LL_EVENT_HANDLER_PRE_PROC:
            pEventHdlr = &(pEventHdlrCtrl->PreProc);
            break;
        case LL_EVENT_HANDLER_POST_PROC:
            pEventHdlr = &(pEventHdlrCtrl->PostProc);
            break;
        default:
            AmbaLL_LogUInt5("[Err][%d] Unknown DSP event type[%d] for the ID[%d]", __LINE__, Type, EventID, 0U, 0U);
            break;
        }

        if (pEventHdlr != NULL) {
            IdxMax          = pEventHdlr->MaxNumHandlers;
            pEventHdlrFunc  = pEventHdlr->pEventHandlers;

            for (Idx = 0U; Idx < IdxMax; Idx++) {
                if (pEventHdlrFunc[Idx] == NULL) {
                    pEventHdlrFunc[Idx] = EventHandler;
                    break;
                }
            }
            if (Idx == IdxMax) {
                AmbaLL_LogUInt5("EventHandlerRegister[%d] No more space[%d], ignored!", EventID, IdxMax, 0U, 0U, 0U);
                Rval = DSP_ERR_0005;
            }
            Rval |= EventGiveMtx(pEventHdlrCtrl);
        }
    }

    return Rval;
}

UINT32 DSP_UnRegisterEventHandler(UINT16 EventID, UINT8 Type, AMBA_DSP_EVENT_HANDLER_f EventHandler)
{
    UINT32 Rval = OK;
    DSP_EVENT_HANDLER_CTRL_s  *pEventHdlrCtrl;
    const DSP_EVENT_HANDLER_s *pEventHdlr = NULL;
    AMBA_DSP_EVENT_HANDLER_f  *pEventHdlrFunc;
    UINT32 Idx;
    UINT32 IdxMax = 0;

    if (EventTakeMtx(EventID, &pEventHdlrCtrl) != OK) {
        Rval = DSP_ERR_0003;
    } else {
        switch (Type) {
        case LL_EVENT_HANDLER:
            pEventHdlr = &(pEventHdlrCtrl->Main);
            break;
        case LL_EVENT_HANDLER_PRE_PROC:
            pEventHdlr = &(pEventHdlrCtrl->PreProc);
            break;
        case LL_EVENT_HANDLER_POST_PROC:
            pEventHdlr = &(pEventHdlrCtrl->PostProc);
            break;
        default:
            AmbaLL_LogUInt5("[Err][%d] Unknown DSP event type[%d] for the ID[%d]", __LINE__, Type, EventID, 0U, 0U);
            break;
        }

        if (pEventHdlr != NULL) {
            IdxMax          = pEventHdlr->MaxNumHandlers;
            pEventHdlrFunc  = pEventHdlr->pEventHandlers;

            for (Idx = 0U; Idx < IdxMax; Idx++) {
                if (pEventHdlrFunc[Idx] == EventHandler) {
                    pEventHdlrFunc[Idx] = NULL;
                    break;
                }
            }
            if (Idx == IdxMax) {
                AmbaLL_LogUInt5("UnRegisterEventHandler: can't find %d, ignored!",EventID, 0U, 0U, 0U, 0U);
            }
            Rval = EventGiveMtx(pEventHdlrCtrl);
        }
    }

    return Rval;
}

UINT32 DSP_EventHandlerReset(UINT16 EventID, UINT8 Type)
{
    UINT32 Rval = OK;
    DSP_EVENT_HANDLER_CTRL_s    *pEventHdlrCtrl;
    DSP_EVENT_HANDLER_s         *pEventHdlr = NULL;

    if (EventTakeMtx(EventID, &pEventHdlrCtrl) != OK) {
        Rval = DSP_ERR_0003;
    } else {
        switch (Type) {
        case LL_EVENT_HANDLER:
            pEventHdlr = &(pEventHdlrCtrl->Main);
            break;
        case LL_EVENT_HANDLER_PRE_PROC:
            pEventHdlr = &(pEventHdlrCtrl->PreProc);
            break;
        case LL_EVENT_HANDLER_POST_PROC:
            pEventHdlr = &(pEventHdlrCtrl->PostProc);
            break;
        default:
            AmbaLL_LogUInt5("[Error @ Event] %d: Unknown DSP event type (%u) for the ID (%u)", __LINE__, Type, EventID, 0U, 0U);
            break;
        }

        if (pEventHdlr != NULL) {
            Rval = dsp_osal_memset(pEventHdlr, 0, sizeof(DSP_EVENT_HANDLER_s));
        }
        Rval |= EventGiveMtx(pEventHdlrCtrl);
    }


    return Rval;
}

UINT32 DSP_GiveEvent(UINT16 EventID, const void *pEventInfo, const UINT16 EventBufIdx)
{
    DSP_EVENT_HANDLER_CTRL_s *pEventHdlrCtrl;
    const AMBA_DSP_EVENT_HANDLER_f *pEventHdlrFunc;
    UINT32 FunctionNum;
    UINT32 i;
    UINT32 Rval = OK;

    if (EventTakeMtx(EventID, &pEventHdlrCtrl) != OK) {
        Rval = DSP_ERR_0003;
    } else {
        /* PreProc */
        pEventHdlrFunc  = pEventHdlrCtrl->PreProc.pEventHandlers;
        FunctionNum     = pEventHdlrCtrl->PreProc.MaxNumHandlers;
        for (i = 0U; i < FunctionNum; i++) {
            if (pEventHdlrFunc[i] != NULL) {
                Rval = (pEventHdlrFunc[i])(pEventInfo);
                if (Rval != DSP_ERR_NONE) {
                    break;
                }
            }
        }

        if (Rval == OK) {
            /* Main */
            pEventHdlrFunc  = pEventHdlrCtrl->Main.pEventHandlers;
            FunctionNum     = pEventHdlrCtrl->Main.MaxNumHandlers;
            Rval = dsp_osal_exec_evnt_user_cb(FunctionNum, pEventHdlrFunc, EventID, pEventInfo, EventBufIdx);
            AmbaMisra_TouchUnused(&Rval);

            /* PostProc */
            pEventHdlrFunc  = pEventHdlrCtrl->PostProc.pEventHandlers;
            FunctionNum     = pEventHdlrCtrl->PostProc.MaxNumHandlers;
            for (i = 0U; i < FunctionNum; i++) {
                if (pEventHdlrFunc[i] != NULL) {
                    Rval = (pEventHdlrFunc[i])(pEventInfo);
                    AmbaMisra_TouchUnused(&Rval);
                }
            }
        }

        if ((Rval != OK) && (EventID < AMBA_DSP_EVENT_NUM) && (pEventInfo != NULL)) {
            // Release event info buufer
            Rval = DSP_ReleaseEventInfoBuf(EventBufIdx);
            AmbaMisra_TouchUnused(&Rval);
        }
        Rval = EventGiveMtx(pEventHdlrCtrl);
    }
    return Rval;
}

UINT32 DSP_RequestEventInfoBuf(const UINT8 Category, ULONG *pULAddr, UINT16 *pBufIdx, const UINT16 EventID)
{
    static const UINT16 LL_EvtInfoBufIdMap[EVENT_INFO_POOL_NUM] = {
        [EVENT_INFO_POOL_PROT_BUF]      = DSP_PROT_BUF_EVENT,
        [EVENT_INFO_POOL_STAT_CFA]      = DSP_STAT_BUF_CFA,
        [EVENT_INFO_POOL_STAT_PG]       = DSP_STAT_BUF_PG,
        [EVENT_INFO_POOL_STAT_HIST0]    = DSP_STAT_BUF_HIST0,
        [EVENT_INFO_POOL_STAT_HIST1]    = DSP_STAT_BUF_HIST1,
        [EVENT_INFO_POOL_STAT_HIST2]    = DSP_STAT_BUF_HIST2,
    };
    UINT32 Rval;
    UINT32 NewWp = 0U;
    UINT16 BufId;

    if (Category < EVENT_INFO_POOL_NUM) {
        BufId = LL_EvtInfoBufIdMap[Category];
        if (dsp_osal_mutex_lock(&EventInfoCtrl[Category].Mtx, 1000U) != OK) {
            Rval = DSP_ERR_0003;
        } else {
            /* Request EventInfo buffer index */
            Rval = DSP_ReqBuf(&EventInfoCtrl[Category].PoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);

            if (Rval == DSP_ERR_NONE) {
                EventInfoCtrl[Category].PoolDesc.Wp = NewWp;
                EventInfoCtrl[Category].Used[NewWp] = (UINT8)(EventID + 1U);
                *pBufIdx = ((UINT16)Category << 12U) | (UINT16)NewWp;

                if (Category == EVENT_INFO_POOL_PROT_BUF) {
                    DSP_GetDspEventInfoAddr((UINT16)NewWp, pULAddr);
                } else {
                    DSP_GetDspStatBufAddr(BufId, (UINT16)NewWp, pULAddr);
                }
            }
            Rval |= dsp_osal_mutex_unlock(&EventInfoCtrl[Category].Mtx);
        }
    } else {
        Rval = DSP_ERR_0001;
    }

    return Rval;
}

UINT32 DSP_ReleaseEventInfoBuf(const UINT16 BufIdx)
{
    UINT32 Rval = DSP_ERR_0001;
    UINT16 Category, PoolIdx, Mask = 0xF000U;
    UINT32 SearchPtr = 0U;

    Category = (BufIdx & Mask) >> 12U; // Bit[15:12]
    PoolIdx = BufIdx & (~Mask);        // Bit[11:0]

    if (Category < EVENT_INFO_POOL_NUM) {
        if (PoolIdx < EventInfoCtrl[Category].PoolDesc.Size) {
            if (dsp_osal_mutex_lock(&EventInfoCtrl[Category].Mtx, 1000U) != OK) {
                Rval = DSP_ERR_0003;
            } else {
                // 1. Mark the EventInfo buf to be unused: Used[id] = 0U
                EventInfoCtrl[Category].Used[PoolIdx] = 0U;

                // 2. Update RP
                SearchPtr = EventInfoCtrl[Category].PoolDesc.Rp;
                while (SearchPtr != EventInfoCtrl[Category].PoolDesc.Wp) {
                    SearchPtr = (SearchPtr + 1U) % EventInfoCtrl[Category].PoolDesc.Size;

                    if (EventInfoCtrl[Category].Used[SearchPtr] > 0U) {
                        break;
                    }

                    EventInfoCtrl[Category].PoolDesc.Rp = SearchPtr;
                }
                Rval = dsp_osal_mutex_unlock(&EventInfoCtrl[Category].Mtx);
            }
        }
    }

    return Rval;
}

UINT32 DSP_ResetEventInfoBuf(const UINT16 EventID)
{
    UINT32 Rval = OK;
    UINT16 CategoryStart = 0U, CategoryEnd = 0U, CategoryIdx;
    UINT32 PoolIdx;

    switch (EventID) {
        case LL_EVENT_LV_CFA_AAA_RDY:
        case LL_EVENT_STL_CFA_AAA_RDY:
            CategoryStart = EVENT_INFO_POOL_STAT_CFA;
            CategoryEnd = EVENT_INFO_POOL_STAT_CFA;
            break;
        case LL_EVENT_LV_PG_AAA_RDY:
        case LL_EVENT_STL_PG_AAA_RDY:
            CategoryStart = EVENT_INFO_POOL_STAT_PG;
            CategoryEnd = EVENT_INFO_POOL_STAT_PG;
            break;
        case LL_EVENT_LV_HIST_AAA_RDY:
            CategoryStart = EVENT_INFO_POOL_STAT_HIST0;
            CategoryEnd = EVENT_INFO_POOL_STAT_HIST2;
            break;
        default:
            CategoryStart = EVENT_INFO_POOL_PROT_BUF;
            CategoryEnd = EVENT_INFO_POOL_PROT_BUF;
            break;
    }

    for (CategoryIdx = CategoryStart; CategoryIdx <= CategoryEnd; CategoryIdx++) {
        for (PoolIdx = 0U; PoolIdx < EventInfoCtrl[CategoryIdx].PoolDesc.Size; PoolIdx++) {
            if (EventInfoCtrl[CategoryIdx].Used[PoolIdx] == (EventID + 1U)) {
                Rval = dsp_osal_mutex_lock(&EventInfoCtrl[CategoryIdx].Mtx, 1000U);
                if (Rval == OK) {
                    EventInfoCtrl[CategoryIdx].Used[PoolIdx] = 0U;
                    Rval = dsp_osal_mutex_unlock(&EventInfoCtrl[CategoryIdx].Mtx);
                }
            }
        }
    }

    return Rval;
}

UINT32 DSP_SetEventFlag(UINT16 Category, UINT16 Type, UINT32 Flag)
{
    UINT32 Rval = OK;
    osal_even_t *pEventFlag = NULL;

    if (DSP_EventInit() == OK) {
        if (Category == DSP_EVENT_CAT_VIN) {
            pEventFlag = &DSP_VinEventFlag[Type];
        } else if (Category == DSP_EVENT_CAT_VPROC) {
            pEventFlag = &DSP_VprocEventFlag[Type];
        } else if (Category == DSP_EVENT_CAT_VOUT) {
            pEventFlag = &DSP_VoutEventFlag[Type];
        } else {
            pEventFlag = &DSP_EventFlag[Type];
        }

        Rval = dsp_osal_eventflag_set(pEventFlag, Flag);
    }
    return Rval;
}

UINT32 DSP_SetEventFlagExclusive(UINT16 Category, UINT16 Type, UINT32 Flag)
{
    UINT32 Rval = OK;
    osal_even_t *pEventFlag = NULL;

    if (DSP_EventInit() == OK) {
        if (Category == DSP_EVENT_CAT_VIN) {
            pEventFlag = &DSP_VinEventFlag[Type];
        } else if (Category == DSP_EVENT_CAT_VPROC) {
            pEventFlag = &DSP_VprocEventFlag[Type];
        } else if (Category == DSP_EVENT_CAT_VOUT) {
            pEventFlag = &DSP_VoutEventFlag[Type];
        } else {
            pEventFlag = &DSP_EventFlag[Type];
        }

        if (dsp_osal_eventflag_clear(pEventFlag, ~Flag) != OK) {
            AmbaLL_LogUInt5("[Err][%d] clear 0x%X 0x%X", __LINE__, Type, ~Flag, 0U, 0U);
            Rval = DSP_ERR_0003;
        } else {
            if (dsp_osal_eventflag_set(pEventFlag, Flag) != OK) {
                AmbaLL_LogUInt5("[Err][%d] give 0x%X 0x%X", __LINE__, Type, Flag, 0U, 0U);
                Rval = DSP_ERR_0003;
            }
        }
    }
    return Rval;
}

UINT32 DSP_ClearEventFlag(UINT16 Category, UINT16 Type, UINT32 Flag)
{
    UINT32 Rval = OK;
    osal_even_t *pEventFlag = NULL;

    if (DSP_EventInit() == OK) {
        if (Category == DSP_EVENT_CAT_VIN) {
            pEventFlag = &DSP_VinEventFlag[Type];
        } else if (Category == DSP_EVENT_CAT_VPROC) {
            pEventFlag = &DSP_VprocEventFlag[Type];
        } else if (Category == DSP_EVENT_CAT_VOUT) {
            pEventFlag = &DSP_VoutEventFlag[Type];
        } else {
            pEventFlag = &DSP_EventFlag[Type];
        }

        Rval = dsp_osal_eventflag_clear(pEventFlag, Flag);
    }
    return Rval;
}

#if 0
UINT32 DSP_CheckEventFlag(UINT16 Category, UINT16 Type, UINT32 Flag)
{
    UINT32 ActualFlags = 0;
    osal_even_t *pEventFlag = NULL;

    if (DSP_EventInit() == OK) {
        if (Category == DSP_EVENT_CAT_VIN) {
            pEventFlag = &DSP_VinEventFlag[Type];
        } else if (Category == DSP_EVENT_CAT_VPROC) {
            pEventFlag = &DSP_VprocEventFlag[Type];
        } else if (Category == DSP_EVENT_CAT_VOUT) {
            pEventFlag = &DSP_VoutEventFlag[Type];
        } else {
            pEventFlag = &DSP_EventFlag[Type];
        }

        if (dsp_osal_eventflag_get(pEventFlag, Flag, 0U, 0U, &ActualFlags, 0/*AMBA_KAL_NO_WAIT*/) == OK) {
            ActualFlags &= Flag;
        }
    }

    return ActualFlags;
}
#endif

UINT32 DSP_WaitEventFlag(UINT16 Category, UINT16 Type, UINT32 Flag, UINT32 All, UINT32 Clear, UINT32 *pActualFlags, UINT32 Timeout)
{
    UINT32 Rval;
    osal_even_t *pEventFlag = NULL;

    Rval = DSP_EventInit();
    *pActualFlags = 0U;

    if (Rval == OK) {
        if (Category == DSP_EVENT_CAT_VIN) {
            if (Type >= DSP_VIN_MAX_NUM) {
                AmbaLL_LogUInt5("[Err][%d] Invalid flag type[%u] ", __LINE__, Type, 0U, 0U, 0U);
                Rval = DSP_ERR_TIMEOUT;
            } else {
                pEventFlag = &DSP_VinEventFlag[Type];
            }
        } else if (Category == DSP_EVENT_CAT_VPROC) {
            if (Type >= AMBA_DSP_MAX_VIEWZONE_NUM) {
                AmbaLL_LogUInt5("[Err][%d] Invalid flag type[%u] ", __LINE__, Type, 0U, 0U, 0U);
                Rval = DSP_ERR_TIMEOUT;
            } else {
                pEventFlag = &DSP_VprocEventFlag[Type];
            }
        } else if (Category == DSP_EVENT_CAT_VOUT) {
            if (Type >= AMBA_DSP_MAX_VOUT_NUM) {
                AmbaLL_LogUInt5("[Err][%d] Invalid flag type[%u] ", __LINE__, Type, 0U, 0U, 0U);
                Rval = DSP_ERR_TIMEOUT;
            } else {
                pEventFlag = &DSP_VoutEventFlag[Type];
            }
        } else {
            if (Type >= DSP_NUM_EVENT_TYPE) {
                AmbaLL_LogUInt5("[Err][%d] Invalid flag type[%u] ", __LINE__, Type, 0U, 0U, 0U);
                Rval = DSP_ERR_TIMEOUT;
            } else {
                pEventFlag = &DSP_EventFlag[Type];
            }
        }

        /* Make sure the event flag is created */
        if (Rval == OK) {
            if (dsp_osal_eventflag_get(pEventFlag, Flag, All, Clear, pActualFlags, Timeout) == OK) {
                *pActualFlags &= Flag;
            } else {
                Rval = DSP_ERR_TIMEOUT;
            }
        }
    }

    return Rval;
}

#if 0
UINT32 DSP_ResetAllEventFlags(UINT16 Category, UINT16 Type)
{
    UINT32 Rval = OK;
    osal_even_t *pEventFlag = NULL;

    if (DSP_EventInit() == OK) {
        if (Category == DSP_EVENT_CAT_VIN) {
            pEventFlag = &DSP_VinEventFlag[Type];
        } else if (Category == DSP_EVENT_CAT_VPROC) {
            pEventFlag = &DSP_VprocEventFlag[Type];
        } else if (Category == DSP_EVENT_CAT_VOUT) {
            pEventFlag = &DSP_VoutEventFlag[Type];
        } else {
            pEventFlag = &DSP_EventFlag[Type];
        }
        Rval = dsp_osal_eventflag_clear(pEventFlag, 0xFFFFFFFFU);
    }
    return Rval;
}
#endif

