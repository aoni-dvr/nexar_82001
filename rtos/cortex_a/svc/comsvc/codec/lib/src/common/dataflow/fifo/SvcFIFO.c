/**
 * @file SvcFIFO.c
 *
 * Copyright (c) 2019 Ambarella International LP
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
 */

#include "FifoCommon.h"
#include "FIFO.h"

static SVC_FIFO_VAR_s g_SvcFIFO GNU_SECTION_NOZEROINIT; /**< The fifo manager */

static SVC_FIFO_HANDLER_s *g_ActFifo = NULL; /**< The active fifo list */

/**
 * Get the private fifo handler.
 *
 * @param [in] Hdlr Fifo handler to operate
 * @return private hdlr
 */
static inline SVC_FIFO_HANDLER_s *GetHandler(const SVC_FIFO_HDLR_s *Hdlr)
{
    SVC_FIFO_HANDLER_s *Handler = NULL;
    ULONG AddrUL, BaseUL, DiffUL;
    AmbaMisra_TypeCast(&AddrUL, &Hdlr);
    AmbaMisra_TypeCast(&BaseUL, &g_ActFifo);
    DiffUL = AddrUL - BaseUL;
    if ((AddrUL >= BaseUL) && ((DiffUL % (ULONG)sizeof(SVC_FIFO_HANDLER_s)) == 0UL)) {
        DiffUL /= (ULONG)sizeof(SVC_FIFO_HANDLER_s);
        if (DiffUL < (ULONG)g_SvcFIFO.NumMaxFifo) {
            Handler = &g_ActFifo[DiffUL];
        }
    }
    return Handler;
}

static inline void SvcFIFO_memset(void *Ptr, INT32 V, UINT32 N, UINT32 *Rval)
{
    if (AmbaWrap_memset(Ptr, V, N) != OK) {
        *Rval = FIFO_ERROR_FATAL;
        AmbaPrint_PrintUInt5("[fifo] memset fail!", 0U, 0U, 0U, 0U, 0U);
    }
}

static inline UINT32 SvcFIFO_MtxTake(AMBA_KAL_MUTEX_t *Mtx, UINT32 LineNum)
{
    UINT32 Rval = OK;
    if (AmbaKAL_MutexTake(Mtx, 3000) != OK) {
        Rval = FIFO_ERROR_FATAL;
        AmbaPrint_PrintUInt5("[fifo] (%u) Take mutex fail!", LineNum, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

static inline UINT32 SvcFIFO_MtxGive(AMBA_KAL_MUTEX_t *Mtx, UINT32 LineNum, UINT32 RetVal)
{
    UINT32 Rval = RetVal;
    if (AmbaKAL_MutexGive(Mtx) != OK) {
        Rval = FIFO_ERROR_FATAL;
        AmbaPrint_PrintUInt5("[fifo] (%u) Give mutex fail!", LineNum, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

/**
 * Set Event Flag
 *
 * @param [in] EventFlag EventFlag
 * @param [in] SetFlags Pattern of flags to set
 * @param [in] LineNum LineNumber
 * @param [in,out] Rval Return value
 */
static inline void SvcFIFO_FlagSet(AMBA_KAL_EVENT_FLAG_t *EventFlag, UINT32 SetFlags, UINT32 LineNum, UINT32 *Rval)
{
    UINT32 Flags = (*Rval == OK)? SetFlags : (SetFlags | (SetFlags << 1)); /* Set error flag */
    if (AmbaKAL_EventFlagSet(EventFlag, Flags) == OK) {
        *Rval = OK;
    } else {
        *Rval = FIFO_ERROR_FATAL;
        AmbaPrint_PrintUInt5("[fifo] (%u) Event flag set failed", LineNum, 0U, 0U, 0U, 0U);
    }
}

/**
 * Get Event Flag
 *
 * @param [in] EventFlag EventFlag
 * @param [in] ReqFlags Pattern of flags to get
 * @param [in] AutoClear Clear event flags that satisfy the request
 * @param [in] CheckError Check error
 * @param [in] LineNum LineNumber
 * @param [in,out] Rval Return value
 */
static inline void SvcFIFO_FlagGet(AMBA_KAL_EVENT_FLAG_t *EventFlag, UINT32 ReqFlags, UINT32 AutoClear, UINT8 CheckError, UINT32 LineNum, UINT32 *Rval)
{
    UINT32 ActualFlags;
    if (AmbaKAL_EventFlagGet(EventFlag, ReqFlags, 0x0U, AutoClear, &ActualFlags, SVC_FIFO_WAIT_TIMEOUT) == OK) {
        if (CheckError == 1U) {
            UINT32 ErrorFlag = (ReqFlags << 1);
            if ((ActualFlags & ErrorFlag) != OK) {
                *Rval = FIFO_ERROR_FATAL;
                AmbaPrint_PrintUInt5("[fifo] (%u) Process Event(%x) faled", LineNum, ReqFlags, 0U, 0U, 0U);
                if (AmbaKAL_EventFlagClear(EventFlag, ErrorFlag) != OK) {
                    *Rval = FIFO_ERROR_FATAL;
                    AmbaPrint_PrintUInt5("[fifo] (%u) Event flag clear failed", LineNum, 0U, 0U, 0U, 0U);
                }
            }
        }
    } else {
        *Rval = FIFO_ERROR_FATAL;
        AmbaPrint_PrintUInt5("[fifo] (%u) Get event flag(%u, %u) failed", LineNum, ReqFlags, AutoClear, 0U, 0U);
    }
}

/**
 * Clear Event Flag
 *
 * @param [in] EventFlag EventFlag
 * @param [in] ClearFlags Pattern of flags to clear
 * @param [in] LineNum LineNumber
 * @param [in,out] Rval Return value
 */
static inline void SvcFIFO_FlagClear(AMBA_KAL_EVENT_FLAG_t *EventFlag, UINT32 ClearFlags, UINT32 LineNum, UINT32 *Rval)
{
    if (AmbaKAL_EventFlagClear(EventFlag, ClearFlags) != OK) {
        *Rval = FIFO_ERROR_FATAL;
        AmbaPrint_PrintUInt5("[fifo] (%u) Event flag clear failed", LineNum, 0U, 0U, 0U, 0U);
    }
}

/**
 * Send a callback event.
 *
 * @param [in] CbEvent Callback function
 * @param [in] Hdlr Fifo handler
 * @param [in] Event Event Id
 * @param [in] Info Information for the event
 * @return 0 - OK, others - fifo error code
 */
static inline UINT32 SvcFIFO_SendCallbackEvent(SVC_FIFO_CALLBACK_f CbEvent, const SVC_FIFO_HDLR_s *Hdlr, UINT8 Event, SVC_FIFO_CALLBACK_INFO_s *Info)
{
    UINT32 Rval = OK;
    if (CbEvent != NULL) {
        Rval = CbEvent(Hdlr, Event, Info);
        if (Rval != OK) {
            if ((Rval < FIFO_ERROR_ARGUMENT_INVALID) || (Rval > FIFO_ERROR_FIFO_FULL)) {
                Rval = FIFO_ERROR_FATAL; /* Undefined callback error */
            }
            AmbaPrint_PrintUInt5("[fifo] (%u) Hdlr Callback Event(%u) failed(%u)!", __LINE__, Event, Rval, 0U, 0U);
        }
    }
    return Rval;
}

/**
 * Free the cmd list of the handler.
 *
 * @param [in] Hdlr Private fifo hdlr
 * @return 0 - OK, others - fifo error code
 */
static UINT32 SvcFIFO_FreeCmdList(SVC_FIFO_HANDLER_s *Hdlr)
{
    UINT32 Rval = SvcFIFO_MtxTake(&g_SvcFIFO.CmdPool.Mutex, __LINE__);
    if (Rval == OK) {
        if (Hdlr->CmdList.HeadIdx != g_SvcFIFO.CmdPool.MaxCmdNum) {
            SVC_FIFO_CMD_s *Cmd = g_SvcFIFO.CmdPool.Cmds;
            Cmd[Hdlr->CmdList.TailIdx].NextIdx = g_SvcFIFO.CmdPool.FreeIdx;
            g_SvcFIFO.CmdPool.FreeIdx = Hdlr->CmdList.HeadIdx;
            Hdlr->CmdList.HeadIdx = g_SvcFIFO.CmdPool.MaxCmdNum;
            AmbaPrint_PrintUInt5("[fifo] (%u) delete fifo(%p) cmds waiting to be processed.", __LINE__, Hdlr->Public.FifoId, 0U, 0U, 0U);
        }
        Rval = SvcFIFO_MtxGive(&g_SvcFIFO.CmdPool.Mutex, __LINE__, Rval);
    }
    return Rval;
}

/**
 * Get a free cmd from pool.
 * g_SvcFIFO.CmdPool.Mutex must be taken in advanced.
 *
 * @param [in] Idx The cmd index
 * @return 0 - OK, others - fifo error code
 */
static UINT32 SvcFIFO_GetFreeCmd(UINT32 *Idx)
{
    UINT32 Rval = OK;
    UINT32 FreeIdx = g_SvcFIFO.CmdPool.FreeIdx;
    if (FreeIdx < g_SvcFIFO.CmdPool.MaxCmdNum) {
        *Idx = FreeIdx;
        g_SvcFIFO.CmdPool.FreeIdx = g_SvcFIFO.CmdPool.Cmds[FreeIdx].NextIdx;
    } else {
        Rval = FIFO_ERROR_FATAL;
        AmbaPrint_PrintUInt5("[fifo] (%u) Fifo cmd pool full", __LINE__, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

/**
 * Add a cmd to the waiting queue.
 *
 * @param [in] Hdlr Fifo handler to operate
 * @param [in] EventId Event ID
 * @param [in] Info Information (depend on eventId)
 * @param [in] Desc Descriptor (depend on eventId)
 * @return 0 - OK, others - fifo error code
 */
static UINT32 SvcFIFO_AddCmd(SVC_FIFO_HANDLER_s *Hdlr, UINT8 EventId, UINT32 Info, const SVC_FIFO_BITS_DESC_s *Desc)
{
    UINT32 Rval = SvcFIFO_MtxTake(&g_SvcFIFO.CmdPool.Mutex, __LINE__);
    if (Rval == OK) {
        UINT32 Idx;
        Rval = SvcFIFO_GetFreeCmd(&Idx);
        if (Rval == OK) {
            SVC_FIFO_CMD_s *Cmd = g_SvcFIFO.CmdPool.Cmds;
            Cmd[Idx].EventId = EventId;
            Cmd[Idx].Info = Info;
            if ((EventId == SVC_FIFO_EVENT_WRITE_LINKED_FIFO) && (Desc != NULL)) {
                /* Check Desc for Misra. Desc will only be NULL when SVC_FIFO_CREATE_WITH_DATA. */
                Cmd[Idx].Desc = *Desc;
            }
            Cmd[Idx].NextIdx = g_SvcFIFO.CmdPool.MaxCmdNum;
            if (Hdlr->CmdList.HeadIdx == g_SvcFIFO.CmdPool.MaxCmdNum) {
                Hdlr->CmdList.HeadIdx = Idx;
                Hdlr->CmdList.TailIdx = Idx;
            } else {
                Cmd[Hdlr->CmdList.TailIdx].NextIdx = Idx;
                Hdlr->CmdList.TailIdx = Idx;
            }
        }
        Rval = SvcFIFO_MtxGive(&g_SvcFIFO.CmdPool.Mutex, __LINE__, Rval);
    }
    if (Rval == OK) {
        Rval = SvcFIFO_SendCallbackEvent(g_SvcFIFO.CbEvent, &(Hdlr->Public), EventId, NULL);
    }
    return Rval;
}

/**
 * Take a cmd from the waiting queue.
 *
 * @param [in] Hdlr Fifo handler to operate
 * @param [in] EventId Event ID
 * @param [out] TargetIdx The taken cmd
 * @return 0 - OK, others - fifo error code
 */
static UINT32 SvcFIFO_TakeCmd(SVC_FIFO_HANDLER_s *Hdlr, UINT8 EventId, UINT32 *TargetIdx)
{
    UINT32 Rval = SvcFIFO_MtxTake(&g_SvcFIFO.CmdPool.Mutex, __LINE__);
    if (Rval == OK) {
        const SVC_FIFO_CMD_s *Cmd = g_SvcFIFO.CmdPool.Cmds;
        UINT32 Idx = Hdlr->CmdList.HeadIdx;
        UINT32 MaxIdx = g_SvcFIFO.CmdPool.MaxCmdNum;
        if ((Idx < MaxIdx) && (Cmd[Idx].EventId == EventId)) {
            Hdlr->CmdList.HeadIdx = Cmd[Idx].NextIdx;
            *TargetIdx = Idx;
        } else {
            Rval = FIFO_ERROR_ARGUMENT_INVALID;
            AmbaPrint_PrintUInt5("[fifo] (%u) Wrong event info.", __LINE__, 0U, 0U, 0U, 0U);
        }
        Rval = SvcFIFO_MtxGive(&g_SvcFIFO.CmdPool.Mutex, __LINE__, Rval);
    }
    return Rval;
}

/**
 * Free a cmd back to cmd pool.
 *
 * @param [in] TargetIdx The cmd index
 * @return 0 - OK, others - fifo error code
 */
static UINT32 SvcFIFO_FreeCmd(UINT32 TargetIdx)
{
    UINT32 Rval = SvcFIFO_MtxTake(&g_SvcFIFO.CmdPool.Mutex, __LINE__);
    if (Rval == OK) {
        /* TargetIdx is from TakeCmd, TargetIdx must be less than MaxCmdNum since TakeCmd return ok. */
        SVC_FIFO_CMD_s *Cmd = g_SvcFIFO.CmdPool.Cmds;
        Cmd[TargetIdx].NextIdx = g_SvcFIFO.CmdPool.FreeIdx;
        g_SvcFIFO.CmdPool.FreeIdx = TargetIdx;
        Rval = SvcFIFO_MtxGive(&g_SvcFIFO.CmdPool.Mutex, __LINE__, Rval);
    }
    return Rval;
}

/**
 * Check the completeness of the IDR frame to be consumed.
 * Prevent a virtual FIFO with an initial incompleted IDR.
 *
 * @param [in] Hdlr Fifo handler to operate
 * @param [in] NumConsumed Number of entry to remove (unchecked)
 * @return Number of entries to remove (checked)
 */
static UINT32 SvcFIFO_CheckCompleteness(const SVC_FIFO_HANDLER_s *Hdlr, UINT32 NumConsumed)
{
    const SVC_FIFO_BITS_DESC_s *DescQueue = Hdlr->DescQueue.Queue;
    UINT32 i;
    UINT32 Rp = (UINT32)((Hdlr->DescQueue.ReadPointer + (UINT64)NumConsumed - 1U) % Hdlr->DescQueue.NumEntries);
    for (i = 0; i < NumConsumed; i++) {
        if ((DescQueue[Rp].FrameType == SVC_FIFO_TYPE_IDR_FRAME) && (DescQueue[Rp].Completed == 0U)) {
            Rp = (Rp == 0U)? (Hdlr->DescQueue.NumEntries - 1U) : (Rp - 1U);
        } else {
            break;
        }
    }
    return (NumConsumed - i);
}

/**
 * Remove entries of a base FIFO.
 * Entries that are still used by virtuals cannot be removed.
 *
 * @param [in] Hdlr Fifo handler to operate
 * @param [in] NumEntries Number of entry to remove
 * @return 0 - OK, others - fifo error code
 */
static UINT32 SvcFIFO_ConsumeBase(SVC_FIFO_HANDLER_s *Hdlr, UINT32 NumEntries)
{
    UINT32 Rval = OK;
    if (NumEntries > 0U) {
        // check if the target entries is still in any of its virtual fifos
        UINT64 MinReadPoint = 0xFFFFFFFFU;
        UINT32 i;
        for (i = 0; i < g_SvcFIFO.NumMaxFifo; i++) { /* Check other virtual fifos */
            SVC_FIFO_HANDLER_s *VirtHdlr = g_SvcFIFO.VirtualFifoHandler[i];
            if (VirtHdlr != NULL) {
                Rval = SvcFIFO_MtxTake(&VirtHdlr->Mutex, __LINE__);
                if (Rval == OK) {
                    if ((VirtHdlr->Used != 0U) && (VirtHdlr->ParentBase == Hdlr)) {
                        if (VirtHdlr->DescQueue.ReadPointer < MinReadPoint) {
                            MinReadPoint = VirtHdlr->DescQueue.ReadPointer;
                        }
                    }
                    Rval = SvcFIFO_MtxGive(&VirtHdlr->Mutex, __LINE__, Rval);
                }
                if (Rval != OK) {
                    break;
                }
            }
        }
        if (Rval == OK) {
            Rval = SvcFIFO_MtxTake(&Hdlr->Mutex, __LINE__);
            if (Rval == OK) {
                const SVC_FIFO_HDLR_s *Public = &(Hdlr->Public);
                SVC_FIFO_CALLBACK_f CbEvent = Hdlr->CbEvent;
                UINT32 Consumed = 0U;
                if (MinReadPoint < Hdlr->DescQueue.ReadPointer) {
                    AmbaPrint_PrintUInt5("[fifo] (%u) Invalid min RP!", __LINE__, 0U, 0U, 0U, 0U);
                    Rval = FIFO_ERROR_INSUFFICIENT_ENTRIES;
                } else {
                    UINT32 MaxRemove = (UINT32)(MinReadPoint - Hdlr->DescQueue.ReadPointer);
                    UINT32 Dist = (UINT32)(Hdlr->DescQueue.WritePointer - Hdlr->DescQueue.ReadPointer);
                    Consumed = (NumEntries > Dist)? Dist : NumEntries;
                    if (Consumed > MaxRemove) {
                        Consumed = MaxRemove;
                    }
                    Consumed = SvcFIFO_CheckCompleteness(Hdlr, Consumed);
                    if (Consumed > 0U) {
                        Hdlr->DescQueue.ReadPointer += Consumed;
                    }
                }
                Rval = SvcFIFO_MtxGive(&Hdlr->Mutex, __LINE__, Rval);
                if ((Rval == OK) && (Consumed > 0U)) {
                    SVC_FIFO_CALLBACK_INFO_s Info = {0};
                    Info.FrameNum = Consumed;
                    Rval = SvcFIFO_SendCallbackEvent(CbEvent, Public, SVC_FIFO_EVENT_DATA_CONSUMED, &Info);
                }
            }
        }
    }
    return Rval;
}

/**
 * Sync removing N entries of a fifo handler
 * Encode: fifo Task sync base fifo with virtual fifo, and notify encoder
 * Decode: fifo Task sync virtual fifo with base fifo, and notify demuxer
 *
 * @param [in] Hdlr Fifo handler to operate
 * @param [in] NumEntries Number of entry to remove
 * @return 0 - OK, others - fifo error code
 */
static UINT32 SvcFIFO_SyncConsumed(SVC_FIFO_HANDLER_s *Hdlr, UINT32 NumEntries)
{
    UINT32 Rval = SvcFIFO_MtxTake(&Hdlr->Mutex, __LINE__);
    if (Rval == OK) {
        if (Hdlr->Used != 0U) {
            const SVC_FIFO_HDLR_s *InPublic = &Hdlr->Public;
            SVC_FIFO_CALLBACK_f InCbEvent = Hdlr->CbEvent;
            if (Hdlr->ParentBase != NULL) { /* encoder : virtual fifo */
                SVC_FIFO_HANDLER_s *BaseHdlr = Hdlr->ParentBase;
                Rval = SvcFIFO_MtxGive(&Hdlr->Mutex, __LINE__, Rval);
                if (Rval == OK) {
                    Rval = SvcFIFO_MtxTake(&BaseHdlr->Mutex, __LINE__);
                    if (Rval == OK) {
                        UINT32 ConsumeBase = 0U;
                        if ((BaseHdlr->Used != 0U) && (BaseHdlr->AutoRemove != 0U)) {
                            ConsumeBase = (UINT32)(BaseHdlr->DescQueue.WritePointer - BaseHdlr->DescQueue.ReadPointer);
                        }
                        /* sync base FIFO if it's AutoRemove */
                        Rval = SvcFIFO_MtxGive(&BaseHdlr->Mutex, __LINE__, Rval);
                        if (Rval == OK) {
                            Rval = SvcFIFO_ConsumeBase(BaseHdlr, ConsumeBase);
                        }
                    }
                }
            } else {  /* decoder : base fifo */
                Rval = SvcFIFO_MtxGive(&Hdlr->Mutex, __LINE__, Rval);
                if (Rval == OK) {
                    UINT32 i;
                    for (i = 0; i < g_SvcFIFO.NumMaxFifo; i++) { /* Check all virtual fifo */
                        SVC_FIFO_HANDLER_s *VirtHdlr = g_SvcFIFO.VirtualFifoHandler[i];
                        if (VirtHdlr != NULL) {
                            Rval = SvcFIFO_MtxTake(&VirtHdlr->Mutex, __LINE__);
                            if (Rval == OK) {
                                if ((VirtHdlr->Used != 0U) && (VirtHdlr->ParentBase == Hdlr)) {
                                    SVC_FIFO_CALLBACK_f CbEvent = VirtHdlr->CbEvent;
                                    const SVC_FIFO_HDLR_s *Public = &VirtHdlr->Public;
                                    UINT64 Dist;
                                    Dist = VirtHdlr->DescQueue.WritePointer - VirtHdlr->DescQueue.ReadPointer;
                                    if (NumEntries > Dist) {
                                        AmbaPrint_PrintUInt5("[fifo] (%u) Rm too many", __LINE__, 0U, 0U, 0U, 0U);
                                        Rval = FIFO_ERROR_INSUFFICIENT_ENTRIES;
                                    } else {
                                        VirtHdlr->DescQueue.ReadPointer += NumEntries;
                                    }
                                    Rval = SvcFIFO_MtxGive(&VirtHdlr->Mutex, __LINE__, Rval);
                                    if (Rval == OK) {
                                        /* Notify demuxer */
                                        SVC_FIFO_CALLBACK_INFO_s Info = {0};
                                        Info.FrameNum = NumEntries;
                                        Rval = SvcFIFO_SendCallbackEvent(CbEvent, Public, SVC_FIFO_EVENT_DATA_CONSUMED, &Info);
                                    }
                                } else {
                                    Rval = SvcFIFO_MtxGive(&VirtHdlr->Mutex, __LINE__, Rval);
                                }
                            }
                            if (Rval != OK) {
                                break;
                            }
                        }
                    }
                }
            }
            if ((Rval != OK) && (Rval != FIFO_ERROR_FATAL)) {
                SVC_FIFO_CALLBACK_INFO_s Info = {0};
                Info.FrameNum = NumEntries;
                Info.ErrorCode = Rval;
                Rval = SvcFIFO_SendCallbackEvent(InCbEvent, InPublic, SVC_FIFO_EVENT_ERROR, &Info);
            }
        } else {
            AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (unused hdlr)", __LINE__, 0U, 0U, 0U, 0U);
            Rval = SvcFIFO_MtxGive(&Hdlr->Mutex, __LINE__, FIFO_ERROR_ARGUMENT_INVALID);
        }
    }
    return Rval;
}

/**
 * Sync writing operation.
 * Decode: dispatch to base fifo, and notify codec
 *
 * @param [in] BaseHdlr Base fifo handler to operate
 * @param [in] InDesc Incoming descriptor content
 * @return 0 - OK, others - fifo error code
 */
static UINT32 SvcFIFO_SyncWrittenBase(SVC_FIFO_HANDLER_s *BaseHdlr, const SVC_FIFO_BITS_DESC_s *InDesc)
{
    UINT32 Rval = SvcFIFO_MtxTake(&BaseHdlr->Mutex, __LINE__);
    if (Rval == OK) {
        if (BaseHdlr->Used != 0U) {
            SVC_FIFO_CALLBACK_f CbEvent = BaseHdlr->CbEvent;
            const SVC_FIFO_HDLR_s *Public = &BaseHdlr->Public;
            SVC_FIFO_CALLBACK_INFO_s Info = {0};
            if ((BaseHdlr->DescQueue.WritePointer - BaseHdlr->DescQueue.ReadPointer) >= BaseHdlr->DescQueue.NumEntries) { /* FIFO full */
                Rval = FIFO_ERROR_FIFO_FULL;
                AmbaPrint_PrintUInt5("[fifo] (%u) SyncWr overlap", __LINE__, 0U, 0U, 0U, 0U);
            } else {
                SVC_FIFO_BITS_DESC_s *DescQueue = BaseHdlr->DescQueue.Queue;
                UINT64 Wp = BaseHdlr->DescQueue.WritePointer % BaseHdlr->DescQueue.NumEntries;
                DescQueue[Wp] = *InDesc;
                BaseHdlr->DescQueue.WritePointer++;
                BaseHdlr->DescQueue.TotalWrite++;
                Info.FrameNum = 1;
                Info.Desc = &DescQueue[Wp];
            }
            Rval = SvcFIFO_MtxGive(&BaseHdlr->Mutex, __LINE__, Rval);
            if (Rval == OK) {
                /* Notify codec */
                Rval = SvcFIFO_SendCallbackEvent(CbEvent, Public, SVC_FIFO_EVENT_DATA_READY, &Info);
            }
        } else {
            AmbaPrint_PrintUInt5("[fifo] (%u) Base Hdlr not used", __LINE__, 0U, 0U, 0U, 0U);
            Rval = SvcFIFO_MtxGive(&BaseHdlr->Mutex, __LINE__, Rval);
        }
    }
    return Rval;
}

/**
 * Sync writing operation.
 * Encode: dispatch to virtual fifos, and notify muxer
 *
 * @param [in] BaseHdlr Base fifo handler to operate
 * @param [in] InDesc Incoming descriptor content
 * @return 0 - OK, others - fifo error code
 */
static UINT32 SvcFIFO_SyncWrittenVirtual(SVC_FIFO_HANDLER_s *BaseHdlr, const SVC_FIFO_BITS_DESC_s *InDesc)
{
    UINT32 Rval = OK;
    UINT32 i, VirtNum = 0;
    for (i = 0; i < g_SvcFIFO.NumMaxFifo; i++) { /* Check all virtual fifos */
        SVC_FIFO_HANDLER_s *VirtHdlr = g_SvcFIFO.VirtualFifoHandler[i];
        if (VirtHdlr != NULL) {
            Rval = SvcFIFO_MtxTake(&VirtHdlr->Mutex, __LINE__);
            if (Rval == OK) {
                if ((VirtHdlr->Used != 0U) && (VirtHdlr->ParentBase == BaseHdlr) && (VirtHdlr->State != SVC_FIFO_STATE_IDLE)) {
                    SVC_FIFO_CALLBACK_f CbEvent = VirtHdlr->CbEvent;
                    const SVC_FIFO_HDLR_s *Public = &VirtHdlr->Public;
                    SVC_FIFO_CALLBACK_INFO_s Info = {0};
                    UINT8 DataRdy = 1U;
                    VirtNum++;
                    if ((VirtHdlr->DescQueue.WritePointer - VirtHdlr->DescQueue.ReadPointer) >= VirtHdlr->DescQueue.NumEntries) { /* FIFO full */
                        Rval = FIFO_ERROR_FIFO_FULL;
                        AmbaPrint_PrintUInt5("[fifo] (%u) SyncWr overlap", __LINE__, 0U, 0U, 0U, 0U);
                    } else {
                        UINT64 Wp = VirtHdlr->DescQueue.WritePointer % VirtHdlr->DescQueue.NumEntries;
                        SVC_FIFO_BITS_DESC_s *DescQueue = VirtHdlr->DescQueue.Queue;
                        DescQueue[Wp] = *InDesc;
                        VirtHdlr->DescQueue.WritePointer++;
                        VirtHdlr->DescQueue.TotalWrite++;
                        Info.FrameNum = 1;
                        Info.Desc = &DescQueue[Wp];
                        if (VirtHdlr->State == SVC_FIFO_STATE_WAIT_FIRST_FRAME) {
                            /* still waiting for certain frame type */
                            if ((DescQueue[Wp].FrameType != SVC_FIFO_TYPE_I_FRAME) && (DescQueue[Wp].FrameType != SVC_FIFO_TYPE_P_FRAME) && (DescQueue[Wp].FrameType != SVC_FIFO_TYPE_B_FRAME)) {
                                VirtHdlr->State = SVC_FIFO_STATE_READY;
                                //AmbaPrintColor(BLUE, "Found Frame WP(%llu) RP(%llu)", VirtHdlr->DescQueue.WritePointer, VirtHdlr->DescQueue.ReadPointer);
                            } else {
                                /* wait for next */
                                VirtHdlr->DescQueue.ReadPointer = VirtHdlr->DescQueue.WritePointer;
                                DataRdy = 0U;
                            }
                        }
                    }
                    Rval = SvcFIFO_MtxGive(&VirtHdlr->Mutex, __LINE__, Rval);
                    if ((Rval == OK) && (DataRdy == 1U)) {
                        /* Notify muxer */
                        Rval = SvcFIFO_SendCallbackEvent(CbEvent, Public, SVC_FIFO_EVENT_DATA_READY, &Info);
                    }
                } else {
                    Rval = SvcFIFO_MtxGive(&VirtHdlr->Mutex, __LINE__, Rval);
                }
            }
            if (Rval != OK) {
                break;
            }
        }
    }
    /* If there's no virtual FIFO for the AutoRm base Fifo, remove the written entry from base and BitsHdlr */
    if ((Rval == OK) && (VirtNum == 0U)) {
        Rval = SvcFIFO_MtxTake(&BaseHdlr->Mutex, __LINE__);
        if (Rval == OK) {
            if (BaseHdlr->AutoRemove != 0U) {
                const SVC_FIFO_HDLR_s *Public = &(BaseHdlr->Public);
                SVC_FIFO_CALLBACK_f CbEvent = BaseHdlr->CbEvent;
                BaseHdlr->DescQueue.ReadPointer++;
                Rval = SvcFIFO_MtxGive(&BaseHdlr->Mutex, __LINE__, Rval);
                if (Rval == OK) {
                    SVC_FIFO_CALLBACK_INFO_s Info = {0};
                    Info.FrameNum = 1U;
                    Rval = SvcFIFO_SendCallbackEvent(CbEvent, Public, SVC_FIFO_EVENT_DATA_CONSUMED, &Info);
                }
            } else {
                Rval = SvcFIFO_MtxGive(&BaseHdlr->Mutex, __LINE__, Rval);
            }
        }
    }
    return Rval;
}

/**
 * Sync writing operation.
 * The handler's mutex must has been taken.
 * Encode: dispatch to virtual fifos, and notify muxer
 * Decode: dispatch to base fifo, and notify codec
 *
 * @param [in] Hdlr Fifo handler to operate
 * @param [in] InDesc Incoming descriptor content
 * @return 0 - OK, others - fifo error code
 */
static UINT32 SvcFIFO_SyncWritten(SVC_FIFO_HANDLER_s *Hdlr, const SVC_FIFO_BITS_DESC_s *InDesc)
{
    UINT32 Rval = OK;
    SVC_FIFO_HANDLER_s *BaseHdlr = Hdlr->ParentBase;
    Rval = SvcFIFO_MtxGive(&Hdlr->Mutex, __LINE__, Rval);
    if (Rval == OK) {
        if (BaseHdlr != NULL) { /* demuxer : virtual fifo */
            Rval = SvcFIFO_SyncWrittenBase(BaseHdlr, InDesc);
        } else { /* encoder: base fifo => dispatch to all vitural fifo */
            Rval = SvcFIFO_SyncWrittenVirtual(Hdlr, InDesc);
        }
        if (SvcFIFO_MtxTake(&Hdlr->Mutex, __LINE__) != OK) {
            Rval = FIFO_ERROR_FATAL;
        }
    }
    return Rval;
}


/**
 * Write an entry to fifo
 * Encoder: Codec write to base fifo, (notify codec if Async), then sync virtual fifo
 * Decoder: Demuxer write to virtual fifo, then sync base fifo
 *
 * @param [in] Hdlr Fifo handler
 * @param [in] Desc The descriptor of the data to write
 * @param [in] Async Asynchronous write or not
 * @return 0 - OK, others - fifo error code
 */
static UINT32 SvcFIFO_OnWriteEntry(SVC_FIFO_HANDLER_s *Hdlr, const SVC_FIFO_BITS_DESC_s *Desc, UINT8 Async)
{
    UINT32 Rval = SvcFIFO_MtxTake(&Hdlr->Mutex, __LINE__);
    if (Rval == OK) {
        SVC_FIFO_CALLBACK_f CbEvent = NULL;
        const SVC_FIFO_HDLR_s *Public = NULL;
        SVC_FIFO_CALLBACK_INFO_s Info = {0};
        UINT8 MtxLock = 1U;
        if (Hdlr->Used != 0U) {
            UINT64 AvailEntries = Hdlr->DescQueue.WritePointer - Hdlr->DescQueue.ReadPointer; /* Number of entries being used */
            UINT32 NumEntries = Hdlr->DescQueue.NumEntries;
            CbEvent = Hdlr->CbEvent;
            Public = &Hdlr->Public;
            if (AvailEntries >= NumEntries) { /* FIFO full */
                Rval = FIFO_ERROR_FIFO_FULL;
                AmbaPrint_PrintUInt5("[fifo] (%u) Wr overlap", __LINE__, 0U, 0U, 0U, 0U);
            } else {
                SVC_FIFO_BITS_DESC_s *DescQueue = Hdlr->DescQueue.Queue;
                UINT64 Idx = Hdlr->DescQueue.WritePointer % NumEntries;
                DescQueue[Idx] = *Desc;
                Hdlr->DescQueue.WritePointer++;
                Hdlr->DescQueue.TotalWrite++;
                if (Async == 1U) {
                    /* Inform BitsHdlr a descriptor ready */
                    Info.FrameNum = 1;
                    Info.Desc = &DescQueue[Idx];
                    MtxLock = 0U;
                    Rval = SvcFIFO_MtxGive(&Hdlr->Mutex, __LINE__, Rval);
                    if (Rval == OK) {
                        /* Notify codec */
                        Rval = SvcFIFO_SendCallbackEvent(CbEvent, Public, SVC_FIFO_EVENT_DATA_READY, &Info);
                        if (Rval == OK) {
                            Rval = SvcFIFO_MtxTake(&Hdlr->Mutex, __LINE__);
                            if (Rval == OK) {
                                MtxLock = 1U;
                            }
                        }
                    }
                }
                if (Rval == OK) {
                    Rval = SvcFIFO_SyncWritten(Hdlr, Desc);
                }
            }
        } else {
            Rval = FIFO_ERROR_ARGUMENT_INVALID;
            AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (unused hdlr)", __LINE__, 0U, 0U, 0U, 0U);
        }
        if (MtxLock == 1U) {
            Rval = SvcFIFO_MtxGive(&Hdlr->Mutex, __LINE__, Rval);
        }
        if (Rval != FIFO_ERROR_FATAL) {
            if (Async == 0U) {
                SvcFIFO_FlagSet(&Hdlr->Flag, SVC_FIFO_FLAG_PROCESS_WRITE, __LINE__, &Rval);
            } else {
                if (Rval != OK) {
                    /* If async write, return error via callback function. */
                    Info.ErrorCode = Rval;
                    Rval = SvcFIFO_SendCallbackEvent(CbEvent, Public, SVC_FIFO_EVENT_ERROR, &Info);
                }
            }
        }
    }
    return Rval;
}

/**
 * Reset all linked fifo.
 *
 * @param [in] Hdlr Fifo handler
 * @return 0 - OK, others - fifo error code
 */
static UINT32 SvcFIFO_OnReset(SVC_FIFO_HANDLER_s *Hdlr)
{
    UINT32 Rval = OK;
    Rval = SvcFIFO_MtxTake(&Hdlr->Mutex, __LINE__);
    if (Rval == OK) {
        if (Hdlr->Used != 0U) {
            SVC_FIFO_HANDLER_s *BaseHdlr = (Hdlr->ParentBase == NULL)? Hdlr : Hdlr->ParentBase;
            Rval = SvcFIFO_MtxGive(&Hdlr->Mutex, __LINE__, Rval);
            if (Rval == OK) {
                /* Reset all linked virtual fifos */
                UINT32 i;
                for (i = 0; i < g_SvcFIFO.NumMaxFifo; i++) { /* Check all virtual fifos */
                    SVC_FIFO_HANDLER_s *VirtHdlr = g_SvcFIFO.VirtualFifoHandler[i];
                    if (VirtHdlr != NULL) {
                        Rval = SvcFIFO_MtxTake(&VirtHdlr->Mutex, __LINE__);
                        if (Rval == OK) {
                            if ((VirtHdlr->Used != 0U) && (VirtHdlr->ParentBase == BaseHdlr)) {
                                VirtHdlr->DescQueue.ReadPointer = 0;
                                VirtHdlr->DescQueue.WritePointer = 0;
                                VirtHdlr->DescQueue.TotalWrite = 0;
                                SvcFIFO_memset(VirtHdlr->DescQueue.Queue, 0, VirtHdlr->DescQueue.QueueSize, &Rval);
                            }
                            Rval = SvcFIFO_MtxGive(&VirtHdlr->Mutex, __LINE__, Rval);
                        }
                    }
                    if (Rval != OK) {
                        break;
                    }
                }

                /* Reset base fifo */
                if (Rval == OK) {
                    Rval = SvcFIFO_MtxTake(&BaseHdlr->Mutex, __LINE__);
                    if (Rval == OK) {
                        if (BaseHdlr->Used != 0U) {
                            SVC_FIFO_CALLBACK_f CbEvent = BaseHdlr->CbEvent;
                            const SVC_FIFO_HDLR_s *Public = &BaseHdlr->Public;
                            BaseHdlr->DescQueue.ReadPointer = 0;
                            BaseHdlr->DescQueue.WritePointer = 0;
                            BaseHdlr->DescQueue.TotalWrite = 0;
                            SvcFIFO_memset(BaseHdlr->DescQueue.Queue, 0, BaseHdlr->DescQueue.QueueSize, &Rval);
                            Rval = SvcFIFO_MtxGive(&BaseHdlr->Mutex, __LINE__, Rval);
                            /* Notify codec if BaseHdlr is reset by virtual */
                            if ((Rval == OK) && (Hdlr != BaseHdlr)) {
                                Rval = SvcFIFO_SendCallbackEvent(CbEvent, Public, SVC_FIFO_EVENT_RESET_FIFO, NULL);
                            }
                        } else {
                            AmbaPrint_PrintUInt5("[fifo] (%u) Base Hdlr not used", __LINE__, 0U, 0U, 0U, 0U);
                            Rval = SvcFIFO_MtxGive(&BaseHdlr->Mutex, __LINE__, Rval);
                        }
                    }
                }
            }
        } else {
            Rval = FIFO_ERROR_ARGUMENT_INVALID;
            AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (unused hdlr)", __LINE__, 0U, 0U, 0U, 0U);
            Rval = SvcFIFO_MtxGive(&Hdlr->Mutex, __LINE__, Rval);
        }
    }
    if (Rval != FIFO_ERROR_FATAL) {
        SvcFIFO_FlagSet(&Hdlr->Flag, SVC_FIFO_FLAG_PROCESS_RESET, __LINE__, &Rval);
    }
    return Rval;
}

/**
 * Fifo handler creation with initial data
 *
 * @param [in] Hdlr The target fifo handler
 * @param [in] StartTime Expected start time in Pts
 * @param [in] TimeScale Time scale of the codec
 * @param [in] IsCompleted If the previous IDR is a completed frame or not
 * @return 0 - OK, others - fifo error code
 */
static void SvcFIFO_SearchFirstFrame(SVC_FIFO_HANDLER_s *Hdlr, UINT64 StartTime, UINT32 TimeScale, UINT8 IsCompleted)
{
    UINT64 Wp = Hdlr->DescQueue.WritePointer;
    UINT64 CurrentRp = Hdlr->DescQueue.ReadPointer;
    UINT32 NumDesc = Hdlr->DescQueue.NumEntries;
    const SVC_FIFO_BITS_DESC_s *DescQueue = Hdlr->DescQueue.Queue;
    UINT64 TargetPts = (StartTime * (UINT64)TimeScale) / 1000U;
    UINT8 CompletedIDR = IsCompleted;
    while(CurrentRp != Wp) {
        UINT64 Idx = CurrentRp % NumDesc;
        if ((Hdlr->State == SVC_FIFO_STATE_READY) && (DescQueue[Idx].Pts > TargetPts)) {
            break;
        } else if ((DescQueue[Idx].FrameType != SVC_FIFO_TYPE_I_FRAME) && (DescQueue[Idx].FrameType != SVC_FIFO_TYPE_P_FRAME) && (DescQueue[Idx].FrameType != SVC_FIFO_TYPE_B_FRAME)) {
            /* if the first frame is IDR, check its completeness */
            if (CompletedIDR == 1U) {
                Hdlr->DescQueue.ReadPointer = CurrentRp;
                Hdlr->State = SVC_FIFO_STATE_READY;
            }
            if (DescQueue[Idx].FrameType == SVC_FIFO_TYPE_IDR_FRAME) {
                CompletedIDR = DescQueue[Idx].Completed;
            }
        } else {
            // not found yet
        }
        CurrentRp++;
    }
    if (Hdlr->State == SVC_FIFO_STATE_WAIT_FIRST_FRAME) {
        Hdlr->DescQueue.ReadPointer = Wp;
    }
}

static UINT32 SvcFIFO_CreateWithData(SVC_FIFO_HANDLER_s *Hdlr)
{
    UINT32 Rval = SvcFIFO_MtxTake(&Hdlr->Mutex, __LINE__);
    if (Rval == OK) {
        SVC_FIFO_HANDLER_s *BaseHdlr = Hdlr->ParentBase;
        Rval = SvcFIFO_MtxTake(&BaseHdlr->Mutex, __LINE__);
        if (Rval == OK) {
            Hdlr->DescQueue.WritePointer = BaseHdlr->DescQueue.WritePointer;
            if ((BaseHdlr->AutoRemove != 0U) || (BaseHdlr->TimeScale == 0U)) {
                Hdlr->DescQueue.ReadPointer = BaseHdlr->DescQueue.WritePointer;
                Hdlr->DescQueue.TotalWrite = 0;
                Hdlr->State = SVC_FIFO_STATE_READY;
            } else {
                UINT8 IsCompleted = 1U; /* if the previous IDR is a completed frame or not */
                UINT64 ValidData = BaseHdlr->DescQueue.WritePointer - BaseHdlr->DescQueue.ReadPointer;
                /* Create with initial data */
                Hdlr->State = SVC_FIFO_STATE_WAIT_FIRST_FRAME;
                /* check length */
                if (ValidData > (UINT64)Hdlr->DescQueue.NumEntries) {
                    const SVC_FIFO_BITS_DESC_s *DescQueue = BaseHdlr->DescQueue.Queue;
                    UINT64 IdxRp = (BaseHdlr->DescQueue.WritePointer - Hdlr->DescQueue.NumEntries -1U) % BaseHdlr->DescQueue.NumEntries;
                    if ((DescQueue[IdxRp].FrameType == SVC_FIFO_TYPE_IDR_FRAME) && (DescQueue[IdxRp].Completed == 0U)) {
                        IsCompleted = 0U;
                    }
                    ValidData = (UINT64)Hdlr->DescQueue.NumEntries;
                    Hdlr->DescQueue.ReadPointer =  Hdlr->DescQueue.WritePointer - ValidData;
                } else {
                    Hdlr->DescQueue.ReadPointer = BaseHdlr->DescQueue.ReadPointer;
                }
                /* start after EOS , if EOS exists */
                if (ValidData > 0U) {
                    UINT64 T;
                    UINT64 IdxWp = BaseHdlr->DescQueue.WritePointer % BaseHdlr->DescQueue.NumEntries;
                    const SVC_FIFO_BITS_DESC_s *DescQueue = BaseHdlr->DescQueue.Queue;
                    for (T = 0; T < ValidData; T++) {
                        IdxWp = (IdxWp == 0U)? ((UINT64)BaseHdlr->DescQueue.NumEntries - 1U) : (IdxWp - 1U);
                        if (DescQueue[IdxWp].FrameType == SVC_FIFO_TYPE_EOS) {
                            break;
                        }
                    }
                    if (T != ValidData) {
                        IsCompleted = 1U;
                        Hdlr->DescQueue.ReadPointer = Hdlr->DescQueue.WritePointer - T;
                        ValidData = T;
                    }
                }
                //(void)AmbaPrint("Create w/ data RP(%llu) WP(%llu)", Hdlr->DescQueue.WritePointer, Hdlr->DescQueue.ReadPointer);
                /* copy data */
                if (ValidData > 0U) {
                    UINT64 T;
                    UINT64 IdxDest = Hdlr->DescQueue.ReadPointer % Hdlr->DescQueue.NumEntries;
                    UINT64 IdxSrc = Hdlr->DescQueue.ReadPointer % BaseHdlr->DescQueue.NumEntries;
                    SVC_FIFO_BITS_DESC_s *QueueDest = Hdlr->DescQueue.Queue;
                    const SVC_FIFO_BITS_DESC_s *QueueSrc = BaseHdlr->DescQueue.Queue;
                    for (T = 0; T < ValidData; T++) {
                        QueueDest[IdxDest] = QueueSrc[IdxSrc];
                        //(void)AmbaPrint("[desc %llu] SrcPts(%llu) DestPts(%llu) idx(%llu)", T, QueueSrc[IdxSrc].Pts, QueueDest[IdxDest].Pts, IdxSrc);
                        IdxDest++;
                        if (IdxDest == Hdlr->DescQueue.NumEntries) {
                            IdxDest = 0;
                        }
                        IdxSrc++;
                        if (IdxSrc == BaseHdlr->DescQueue.NumEntries) {
                            IdxSrc = 0;
                        }
                    }
                    /* scan to meet init condition */
                    SvcFIFO_SearchFirstFrame(Hdlr, Hdlr->StartTime, BaseHdlr->TimeScale, IsCompleted);
                }
                Hdlr->DescQueue.TotalWrite = Hdlr->DescQueue.WritePointer - Hdlr->DescQueue.ReadPointer;
            }
            Rval = SvcFIFO_MtxGive(&BaseHdlr->Mutex, __LINE__, Rval);
        }
        Rval = SvcFIFO_MtxGive(&Hdlr->Mutex, __LINE__, Rval);
    }
    if (Rval != FIFO_ERROR_FATAL) {
        SvcFIFO_FlagSet(&Hdlr->Flag, SVC_FIFO_FLAG_PROCESS_CREATE, __LINE__, &Rval);
    }
    return Rval;
}

/**
 * ParamCheck for SvcFIFO_ProcessEvent
 *
 * @param [in] Fifo Fifo handler
 * @param [in] EventId Event Id
 * @return 0 - OK, others - fifo error code
 */
static inline UINT32 SvcFIFO_ProcessEvent_ParamCheck(const SVC_FIFO_HDLR_s *Fifo, UINT8 EventId)
{
    UINT32 Rval;
    if (Fifo == NULL) {
        Rval = FIFO_ERROR_ARGUMENT_INVALID;
        AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else if ((EventId < SVC_FIFO_EVENT_WRITE_LINKED_FIFO) || (EventId > SVC_FIFO_EVENT_RESET_FIFO)) {
        Rval = FIFO_ERROR_ARGUMENT_INVALID;
        AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (invalid eventId)", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
 * Process a Fifo Event.
 *
 * @param [in] Fifo Fifo handler
 * @param [in] EventId Event Id
 * @return 0 - OK, others - fifo error code
 */
UINT32 SvcFIFO_ProcessEvent(const SVC_FIFO_HDLR_s *Fifo, UINT8 EventId)
{
    UINT32 Rval = SvcFIFO_ProcessEvent_ParamCheck(Fifo, EventId);
    if (Rval == OK) {
        SVC_FIFO_HANDLER_s *Hdlr = GetHandler(Fifo);
        if (Hdlr != NULL) {
            UINT32 TargetIdx = g_SvcFIFO.CmdPool.MaxCmdNum;
            Rval = SvcFIFO_TakeCmd(Hdlr, EventId, &TargetIdx);
            if (Rval == OK) {
                const SVC_FIFO_CMD_s *Cmd = g_SvcFIFO.CmdPool.Cmds;
                if (EventId == SVC_FIFO_EVENT_WRITE_LINKED_FIFO) {
                    if (Cmd[TargetIdx].Info == SVC_FIFO_CREATE_WITH_DATA) {
                        Rval = SvcFIFO_CreateWithData(Hdlr);
                    } else {
                        UINT8 Async = (UINT8) Cmd[TargetIdx].Info;
                        Rval = SvcFIFO_OnWriteEntry(Hdlr, &(Cmd[TargetIdx].Desc), Async);
                    }
                } else if (EventId == SVC_FIFO_EVENT_REMOVE_LINKED_FIFO) {
                    Rval = SvcFIFO_SyncConsumed(Hdlr, Cmd[TargetIdx].Info);
                } else {
                    /* EventId == SVC_FIFO_EVENT_RESET_FIFO */
                    Rval = SvcFIFO_OnReset(Hdlr);
                }
                if (Rval != FIFO_ERROR_FATAL) {
                    UINT32 Error = SvcFIFO_FreeCmd(TargetIdx);
                    if (Error != OK) {
                        Rval = Error;
                    }
                }
            }
        } else {
            Rval = FIFO_ERROR_ARGUMENT_INVALID;
            AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (get private hdlr failed)", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

/**
 * ParamCheck for SvcFIFO_GetInitBufferSize
 *
 * @param [in] NumMaxFifo Maximum number of handlers that will be used
 * @param [in] NumMaxPendingEvent Maximum number of pending events
 * @param [in] BufferSize Required buffer size
 * @return 0 - OK, others - fifo error code
 */
static inline UINT32 SvcFIFO_GetInitBufferSize_ParamCheck(UINT32 NumMaxFifo, UINT32 NumMaxPendingEvent, const UINT32 *BufferSize)
{
    UINT32 Rval;
    if ((NumMaxFifo == 0U) || (NumMaxPendingEvent == 0U) || (BufferSize == NULL)) {
        Rval = FIFO_ERROR_ARGUMENT_INVALID;
        AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
 * Get the required buffer size
 *
 * @param [in] NumMaxFifo Maximum number of handlers that will be used
 * @param [in] NumMaxPendingEvent Maximum number of pending events
 * @param [out] BufferSize Required buffer size
 * @return 0 - OK, others - fifo error code
 */
UINT32 SvcFIFO_GetInitBufferSize(UINT32 NumMaxFifo, UINT32 NumMaxPendingEvent, UINT32 *BufferSize)
{
    UINT32 Rval = SvcFIFO_GetInitBufferSize_ParamCheck(NumMaxFifo, NumMaxPendingEvent, BufferSize);
    if (Rval == OK) {
        UINT32 HdlrSize = NumMaxFifo * (UINT32)sizeof(SVC_FIFO_HANDLER_s);
        UINT32 MsgSize = NumMaxPendingEvent * (UINT32)sizeof(SVC_FIFO_CMD_s);
        *BufferSize = GetAlignedValU32(HdlrSize + MsgSize, (UINT32)AMBA_CACHE_LINE_SIZE);
    }
    return Rval;
}

/**
 * ParamCheck for SvcFIFO_GetInitDefaultCfg
 *
 * @param [in] DefaultCfg Default configuration
 * @return 0 - OK, others - fifo error code
 */
static inline UINT32 SvcFIFO_GetInitDefaultCfg_ParamCheck(const SVC_FIFO_INIT_CFG_s *DefaultCfg)
{
    UINT32 Rval;
    if (DefaultCfg == NULL) {
        Rval = FIFO_ERROR_ARGUMENT_INVALID;
        AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
 * Get default config of FIFO module initialization
 *
 * @param [in,out] DefaultCfg Default configuration
 * @return 0 - OK, others - fifo error code
 */
UINT32 SvcFIFO_GetInitDefaultCfg(SVC_FIFO_INIT_CFG_s *DefaultCfg)
{
    UINT32 Rval = SvcFIFO_GetInitDefaultCfg_ParamCheck(DefaultCfg);
    if (Rval == OK) {
        DefaultCfg->NumMaxFifo = 16;   // 4 base fifo, 12 virtual fifo
        DefaultCfg->NumMaxPendingEvent = 16 * 128;
        DefaultCfg->MemoryPoolAddr = NULL;
        Rval = SvcFIFO_GetInitBufferSize(DefaultCfg->NumMaxFifo, DefaultCfg->NumMaxPendingEvent, &DefaultCfg->MemoryPoolSize);
    }
    return Rval;
}

/**
 * Implementation function of FIFO module initialization
 *
 * @param [in] Cfg Config
 * @return 0 - OK, others - fifo error code
 */
static UINT32 SvcFIFO_InitImpl(const SVC_FIFO_INIT_CFG_s *Cfg)
{
    UINT32 Rval = OK;
    UINT32 i;
    static char MtxName[] = "AmbaFifoHdlrMutex";
    static char FlagName[] = "AmbaFifoHdlrFlag";

    g_SvcFIFO.CbEvent = Cfg->CbEvent;
    g_SvcFIFO.NumMaxFifo = Cfg->NumMaxFifo;
    SvcFIFO_memset(&g_SvcFIFO.BaseFifoHandler[0], 0, SVC_FIFO_MAX_ACTIVE_FIFO*((UINT32)sizeof(SVC_FIFO_HANDLER_s *)), &Rval);
    SvcFIFO_memset(&g_SvcFIFO.VirtualFifoHandler[0], 0, SVC_FIFO_MAX_ACTIVE_FIFO*((UINT32)sizeof(SVC_FIFO_HANDLER_s *)), &Rval);
    if (Rval == OK) {
        g_SvcFIFO.MemPool = Cfg->MemoryPoolAddr;
        g_SvcFIFO.MemPoolSize = Cfg->MemoryPoolSize;

        /* Assign fifo handler memory */
        AmbaMisra_TypeCast(&g_ActFifo, &Cfg->MemoryPoolAddr);
        for (i = 0; i < Cfg->NumMaxFifo; i++) {
            g_ActFifo[i].Public.FifoId = 0xFFFFFFFFU;
            if (AmbaKAL_MutexCreate(&g_ActFifo[i].Mutex, MtxName) == OK) {
                if (AmbaKAL_EventFlagCreate(&g_ActFifo[i].Flag, FlagName) != OK) {
                    Rval = FIFO_ERROR_FATAL;
                    AmbaPrint_PrintUInt5("[fifo] (%u) Create flag failed", __LINE__, 0U, 0U, 0U, 0U);
                }
            } else {
                Rval = FIFO_ERROR_FATAL;
                AmbaPrint_PrintUInt5("[fifo] (%u) Create hdlr's mutex fail", __LINE__, 0U, 0U, 0U, 0U);
            }
            if (Rval != OK) {
                break;
            }
        }
    }
    if (Rval == OK) {
        /* Create message queue */
        UINT32 NumMsg = Cfg->NumMaxPendingEvent;
        UINT32 Offset = Cfg->NumMaxFifo * (UINT32)sizeof(SVC_FIFO_HANDLER_s);
        const UINT8 *Addr = &Cfg->MemoryPoolAddr[Offset];
        AmbaMisra_TypeCast(&g_SvcFIFO.CmdPool.Cmds, &Addr);
        g_SvcFIFO.CmdPool.MaxCmdNum = NumMsg;
        g_SvcFIFO.CmdPool.FreeIdx = 0;
        for (i = 0; i< NumMsg; i++) {
            g_SvcFIFO.CmdPool.Cmds[i].NextIdx = i + 1U;
        }
        for (i = 0; i < Cfg->NumMaxFifo; i++) {
            g_ActFifo[i].CmdList.HeadIdx = NumMsg;
            g_ActFifo[i].CmdList.TailIdx = NumMsg;
        }
    }
    return Rval;
}

/**
 * ParamCheck for SvcFIFO_Init
 *
 * @param [in] Cfg Config
 * @return 0 - OK, others - fifo error code
 */
static UINT32 SvcFIFO_Init_ParamCheck(const SVC_FIFO_INIT_CFG_s *Cfg)
{
    UINT32 Rval = FIFO_ERROR_ARGUMENT_INVALID;
    if (Cfg == NULL) {
        AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else if (Cfg->MemoryPoolAddr == NULL) {
        AmbaPrint_PrintUInt5("[fifo] (%u) MemoryPoolAddr is NULL", __LINE__, 0U, 0U, 0U, 0U);
    } else if ((Cfg->NumMaxFifo > SVC_FIFO_MAX_ACTIVE_FIFO) || (Cfg->NumMaxPendingEvent == 0U) || (Cfg->NumMaxFifo == 0U) || (Cfg->CbEvent == NULL)) {
        AmbaPrint_PrintUInt5("[fifo] (%u) NumMaxFifo > %d or NumMaxFifo = 0 or CbEvent is NULL.", __LINE__, SVC_FIFO_MAX_ACTIVE_FIFO, 0U, 0U, 0U);
    } else {
        UINT32 BufferSize = 0;
        Rval = SvcFIFO_GetInitBufferSize(Cfg->NumMaxFifo, Cfg->NumMaxPendingEvent, &BufferSize); /* must be ok */
        if (BufferSize > Cfg->MemoryPoolSize) {
            Rval = FIFO_ERROR_ARGUMENT_INVALID;
            AmbaPrint_PrintUInt5("[fifo] (%u) MemoryPoolSize is too small.", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

/**
 * AMBA FIFO module initialization
 *
 * @param [in] Cfg Config
 * @return 0 - OK, others - fifo error code
 */
UINT32 SvcFIFO_Init(const SVC_FIFO_INIT_CFG_s *Cfg)
{
    UINT32 Rval;
    static UINT8 InitFlag = 0U; /**< Initial flag */
    static char GMtxName[] = "AmbaFifoGMutex";
    static char CmdMtxName[] = "AmbaFifoCmdMutex";
    if (InitFlag == 0U) {
        Rval = SvcFIFO_Init_ParamCheck(Cfg);
        if (Rval == OK) {
            SvcFIFO_memset(Cfg->MemoryPoolAddr, 0, Cfg->MemoryPoolSize, &Rval);
            if (Rval == OK) {
                if (AmbaKAL_MutexCreate(&g_SvcFIFO.Mutex, GMtxName) == OK) {
                    if (AmbaKAL_MutexCreate(&g_SvcFIFO.CmdPool.Mutex, CmdMtxName) == OK) {
                        Rval = SvcFIFO_InitImpl(Cfg);
                        if (Rval == OK) {
                            InitFlag = 1U;
                        }
                    } else {
                        Rval = FIFO_ERROR_FATAL;
                        AmbaPrint_PrintUInt5("[fifo] (%u) Create mutex failed!", __LINE__, 0U, 0U, 0U, 0U);
                    }
                } else {
                    Rval = FIFO_ERROR_FATAL;
                    AmbaPrint_PrintUInt5("[fifo] (%u) Create mutex failed!", __LINE__, 0U, 0U, 0U, 0U);
                }
            }
        }
    } else {
        Rval = FIFO_ERROR_FATAL;
        AmbaPrint_PrintUInt5("[fifo] (%u) SvcFIFO_Init has been Inited", __LINE__, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

/**
 * ParamCheck for SvcFIFO_GetDefaultCfg
 *
 * @param [in] DefaultCfg Default configuration
 * @return 0 - OK, others - fifo error code
 */
static inline UINT32 SvcFIFO_GetDefaultCfg_ParamCheck(const SVC_FIFO_CFG_s *DefaultCfg)
{
    UINT32 Rval;
    if (DefaultCfg == NULL) {
        Rval = FIFO_ERROR_ARGUMENT_INVALID;
        AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
 * Get FIFO handler default configuration
 *
 * @param [in,out] DefaultCfg Default configuration
 * @return 0 - OK, others - fifo error code
 */
UINT32 SvcFIFO_GetDefaultCfg(SVC_FIFO_CFG_s *DefaultCfg)
{
    UINT32 Rval = SvcFIFO_GetDefaultCfg_ParamCheck(DefaultCfg);
    if (Rval == OK) {
        SvcFIFO_memset(DefaultCfg, 0, (UINT32)sizeof(SVC_FIFO_CFG_s), &Rval);
        DefaultCfg->NumEntries = 128U;
        DefaultCfg->BufferSize = 128U * (UINT32)sizeof(SVC_FIFO_BITS_DESC_s);
        DefaultCfg->BaseCfg.AutoRemove = 1U;
        DefaultCfg->CbEvent = NULL;
    }
    return Rval;
}

/**
 * ParamCheck for SvcFIFO_GetHdlrBufferSize
 *
 * @param [in] NumEntries Number of entries
 * @param [in] BufferSize Buffer size
 * @return 0 - OK, others - fifo error code
 */
static inline UINT32 SvcFIFO_GetHdlrBufferSize_ParamCheck(UINT32 NumEntries, const UINT32 *BufferSize)
{
    UINT32 Rval;
    if ((NumEntries == 0U) || (BufferSize == NULL)) {
        Rval = FIFO_ERROR_ARGUMENT_INVALID;
        AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
 * Get the hdlr buffer size for a fifo handler
 *
 * @param [in] NumEntries Number of entries
 * @param [out] BufferSize Buffer size
 * @return 0 - OK, others - Error
 */
UINT32 SvcFIFO_GetHdlrBufferSize(UINT32 NumEntries, UINT32 *BufferSize) {
    UINT32 Rval = SvcFIFO_GetHdlrBufferSize_ParamCheck(NumEntries, BufferSize);
    if (Rval == OK) {
        *BufferSize = GetAlignedValU32(NumEntries * (UINT32)sizeof(SVC_FIFO_BITS_DESC_s), (UINT32)AMBA_CACHE_LINE_SIZE);
    }
    return Rval;
}

/**
 * Mark used fifo handlers
 *
 * @param [in] BaseHdlr BaseHdlr of the target fifo handler
 * @param [out] Hdlr The target fifo handler
 * @return 0 - OK, others - Error
 */
static UINT32 SvcFIFO_MarkActiveFifo(const SVC_FIFO_HDLR_s *BaseHdlr, SVC_FIFO_HANDLER_s **Hdlr)
{
    UINT32 Rval = FIFO_ERROR_FATAL;
    UINT32 i;
    for (i = 0; i < g_SvcFIFO.NumMaxFifo; i++) {
        if (g_ActFifo[i].Used == 0U) {
            g_ActFifo[i].Public.FifoId = i;
            *Hdlr = &g_ActFifo[i];
            Rval = OK;
            break;
        } else {
            // continue
        }
    }
    if (Rval == OK) {
        SVC_FIFO_HANDLER_s **Handlers = (BaseHdlr == NULL)? g_SvcFIFO.BaseFifoHandler : g_SvcFIFO.VirtualFifoHandler;
        Rval = FIFO_ERROR_FATAL;
        for (i = 0; i < (g_SvcFIFO.NumMaxFifo - 1U); i++) { /* at least 1 for virtual, 1 for base */
            if (Handlers[i] == NULL) {
                Handlers[i] = *Hdlr;
                Rval = OK;
                break;
            } else {
                // continue
            }
        }
    }
    return Rval;
}

/**
 * Fifo handler creation
 *
 * @param [in] Cfg The config to initialize fifo
 * @param [in,out] Hdlr The target fifo handler
 * @return 0 - OK, others - fifo error code
 */
static UINT32 SvcFIFO_InitFifoHdlr(const SVC_FIFO_CFG_s *Cfg, SVC_FIFO_HANDLER_s *Hdlr)
{
    UINT32 Rval = OK;
    Hdlr->CbEvent = Cfg->CbEvent;
    Hdlr->AutoRemove = Cfg->BaseCfg.AutoRemove;
    Hdlr->TimeScale = Cfg->BaseCfg.TimeScale;
    Hdlr->StartTime = Cfg->VirtualCfg.StartTime;
    if (Cfg->BaseHdlr == NULL) {
        Hdlr->ParentBase = NULL;
    } else {
        Hdlr->ParentBase = GetHandler(Cfg->BaseHdlr);
        if (Hdlr->ParentBase == NULL) {
            Rval = FIFO_ERROR_ARGUMENT_INVALID;
            AmbaPrint_PrintUInt5("[fifo] (%u) Can't find base fifo", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    if (Rval == OK) {
        /* Alloc descriptor */
        Hdlr->DescQueue.NumEntries = Cfg->NumEntries;
        Hdlr->DescQueue.QueueSize = Hdlr->DescQueue.NumEntries * (UINT32)sizeof(SVC_FIFO_BITS_DESC_s);
        AmbaMisra_TypeCast(&Hdlr->DescQueue.Queue, &Cfg->BufferAddr);
        SvcFIFO_memset(Hdlr->DescQueue.Queue, 0, Hdlr->DescQueue.QueueSize, &Rval);
        SvcFIFO_FlagClear(&Hdlr->Flag, 0xFFFFFFFFU, __LINE__, &Rval);
        if (Rval == OK) {
            SvcFIFO_FlagSet(&Hdlr->Flag, (SVC_FIFO_FLAG_PROCESS_WRITE | SVC_FIFO_FLAG_PROCESS_RESET | SVC_FIFO_FLAG_PROCESS_CREATE), __LINE__, &Rval);
        }
    }
    return Rval;
}

/**
 * ParamCheck for SvcFIFO_Create
 *
 * @param [in] Cfg The config to initialize fifo
 * @param [in] Fifo Fifo handler
 * @return 0 - OK, others - fifo error code
 */
static inline UINT32 SvcFIFO_Create_ParamCheck(const SVC_FIFO_CFG_s *Cfg, SVC_FIFO_HDLR_s *const *Fifo)
{
    UINT32 Rval = FIFO_ERROR_ARGUMENT_INVALID;
    if ((Fifo == NULL) || (Cfg == NULL)) {
        AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else if (Cfg->NumEntries == 0U) {
        AmbaPrint_PrintUInt5("[fifo] (%u) NumEntries cannot be 0.", __LINE__, 0U, 0U, 0U, 0U);
    } else if (Cfg->CbEvent == NULL) {
        AmbaPrint_PrintUInt5("[fifo] (%u) CbEvent cannot be NULL.", __LINE__, 0U, 0U, 0U, 0U);
    } else if ((Cfg->BaseHdlr == NULL) && (Cfg->BaseCfg.AutoRemove > 1U)) {
        AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument. (AutoRemove))", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        UINT32 BufferSize = Cfg->NumEntries * (UINT32)sizeof(SVC_FIFO_BITS_DESC_s);
        if ((Cfg->BufferAddr == NULL) || (Cfg->BufferSize < BufferSize)) {
            AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (invalid buffer)", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            Rval = OK;
        }
    }
    return Rval;
}

/**
 * Fifo handler creation
 *
 * @param [in] Cfg The config to initialize fifo
 * @param [in,out] Fifo Fifo handler
 * @return 0 - OK, others - fifo error code
 */
UINT32 SvcFIFO_Create(const SVC_FIFO_CFG_s *Cfg, SVC_FIFO_HDLR_s **Fifo)
{
    UINT32 Rval = SvcFIFO_Create_ParamCheck(Cfg, Fifo);
    if (Rval == OK) {
        SVC_FIFO_HANDLER_s *Hdlr = NULL;
        Rval = SvcFIFO_MtxTake(&g_SvcFIFO.Mutex, __LINE__);
        if (Rval == OK) {
            /* Assign a virtual handler */
            Rval = SvcFIFO_MarkActiveFifo(Cfg->BaseHdlr, &Hdlr);
            if (Rval == OK) {
                Rval = SvcFIFO_MtxTake(&Hdlr->Mutex, __LINE__);
                if (Rval == OK) {
                    Rval = SvcFIFO_InitFifoHdlr(Cfg, Hdlr);
                    if (Rval == OK) {
                        if (Cfg->BaseHdlr == NULL) {
                            Hdlr->DescQueue.ReadPointer = 0;
                            Hdlr->DescQueue.WritePointer = 0;
                            Hdlr->DescQueue.TotalWrite = 0;
                            Hdlr->Used = 1U;
                            *Fifo = &Hdlr->Public;
                            AmbaPrint_PrintUInt5("SvcFIFO_Create Base(%u) : AutoRm(%u) TimeScale(%u) NumEntries(%u)",
                                Hdlr->Public.FifoId, Hdlr->AutoRemove, Hdlr->TimeScale, Hdlr->DescQueue.NumEntries, 0U);
                        } else {
                            SVC_FIFO_HANDLER_s *BaseHdlr = Hdlr->ParentBase;
                            Rval = SvcFIFO_MtxTake(&BaseHdlr->Mutex, __LINE__);
                            if (Rval == OK) {
                                if (BaseHdlr->Used == 1U) {
                                    Hdlr->State = SVC_FIFO_STATE_IDLE;
                                    Hdlr->Used = 1U;
                                    *Fifo = &Hdlr->Public;
                                    AmbaPrint_PrintUInt5("SvcFIFO_Create Virtual(%u) : Base(%u) StartTime(%u) NumEntries(%u)",
                                        Hdlr->Public.FifoId, BaseHdlr->Public.FifoId, (UINT32)Hdlr->StartTime, Hdlr->DescQueue.NumEntries, 0U);
                                } else {
                                    UINT32 i;
                                    SVC_FIFO_HANDLER_s **Handlers = g_SvcFIFO.VirtualFifoHandler;
                                    Rval = FIFO_ERROR_FATAL;
                                    AmbaPrint_PrintUInt5("[fifo] (%u) Base hdlr is not created yet!", __LINE__, 0U, 0U, 0U, 0U);
                                    Hdlr->ParentBase = NULL;
                                    for (i = 0; i < g_SvcFIFO.NumMaxFifo; i++) {
                                        if (Handlers[i] == Hdlr) {
                                            Handlers[i] = NULL;
                                        }
                                    }
                                    Hdlr->DescQueue.Queue = NULL;
                                }
                                Rval = SvcFIFO_MtxGive(&BaseHdlr->Mutex, __LINE__, Rval);
                            }
                        }
                    }
                    Rval = SvcFIFO_MtxGive(&Hdlr->Mutex, __LINE__, Rval);
                }
            } else {
                AmbaPrint_PrintUInt5("[fifo] (%u) too many active fifo!", __LINE__, 0U, 0U, 0U, 0U);
            }
            Rval = SvcFIFO_MtxGive(&g_SvcFIFO.Mutex, __LINE__, Rval);
        }
        /* setting Rp and Wp for virtual fifo by fifo task */
        if ((Rval == OK) && (Cfg->BaseHdlr != NULL) && (Hdlr != NULL)) { /* for misra */
            SvcFIFO_FlagGet(&Hdlr->Flag, SVC_FIFO_FLAG_PROCESS_CREATE, /*CLEAR*/0x1U, 0U, __LINE__, &Rval);
            if (Rval == OK) {
                Rval = SvcFIFO_AddCmd(Hdlr, SVC_FIFO_EVENT_WRITE_LINKED_FIFO, SVC_FIFO_CREATE_WITH_DATA, NULL);
                if (Rval == OK) {
                    SvcFIFO_FlagGet(&Hdlr->Flag, SVC_FIFO_FLAG_PROCESS_CREATE, 0x0U, 1U, __LINE__, &Rval);
                }
            } else {
                Rval = FIFO_ERROR_FATAL;
                AmbaPrint_PrintUInt5("[fifo] (%u) Take event flag(AMBA_FIFO_FLAG_PROCESS_EVENT) failed", __LINE__, 0U, 0U, 0U, 0U);
            }
        }
    }
    return Rval;
}

/**
 * ParamCheck for SvcFIFO_Delete
 *
 * @param [in] Fifo Fifo handler
 * @return 0 - OK, others - fifo error code
 */
static inline UINT32 SvcFIFO_Delete_ParamCheck(const SVC_FIFO_HDLR_s *Fifo)
{
    UINT32 Rval;
    if (Fifo == NULL) {
        Rval = FIFO_ERROR_ARGUMENT_INVALID;
        AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
 * Delete a fifo.
 *
 * @param [in] Fifo Target fifo to close
 * @return 0 - OK, others - fifo error code
 */
UINT32 SvcFIFO_Delete(SVC_FIFO_HDLR_s *Fifo)
{
    UINT32 Rval = SvcFIFO_Delete_ParamCheck(Fifo);
    if (Rval == OK) {
        SVC_FIFO_HANDLER_s *Hdlr = GetHandler(Fifo);
        if (Hdlr != NULL) {
            SVC_FIFO_HANDLER_s *BaseHdlr = NULL;
            Rval = SvcFIFO_MtxTake(&g_SvcFIFO.Mutex, __LINE__);
            if (Rval == OK) {
                Rval = SvcFIFO_MtxTake(&Hdlr->Mutex, __LINE__);
                if (Rval == OK) {
                    if (Hdlr->Used != 0U) {
                        UINT32 i;
                        SVC_FIFO_HANDLER_s **Handlers = (Hdlr->ParentBase == NULL)? g_SvcFIFO.BaseFifoHandler : g_SvcFIFO.VirtualFifoHandler;
                        BaseHdlr = Hdlr->ParentBase;
                        for (i = 0; i < g_SvcFIFO.NumMaxFifo; i++) {
                            if (Handlers[i] == Hdlr) {
                                Handlers[i] = NULL;
                            }
                        }
                        Rval = SvcFIFO_FreeCmdList(Hdlr);
                        if (Rval == OK) {
                            Fifo->FifoId = 0xFFFFFFFFU;
                            Hdlr->Used = 0U;
                        }
                    } else {
                        Rval = FIFO_ERROR_ARGUMENT_INVALID;
                        AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (unused hdlr)", __LINE__, 0U, 0U, 0U, 0U);
                    }
                    Rval = SvcFIFO_MtxGive(&Hdlr->Mutex, __LINE__, Rval);
                }
                Rval = SvcFIFO_MtxGive(&g_SvcFIFO.Mutex, __LINE__, Rval);
            }
            if ((Rval == OK) && (BaseHdlr != NULL)) {
                Rval = SvcFIFO_MtxTake(&BaseHdlr->Mutex, __LINE__);
                if (Rval == OK) {
                    UINT32 ConsumeBase = 0U;
                    if ((BaseHdlr->Used != 0U) && (BaseHdlr->AutoRemove != 0U)) {
                        ConsumeBase = (UINT32)(BaseHdlr->DescQueue.WritePointer - BaseHdlr->DescQueue.ReadPointer);
                    }
                    Rval = SvcFIFO_MtxGive(&BaseHdlr->Mutex, __LINE__, Rval);
                    if  (Rval == OK){
                        Rval = SvcFIFO_ConsumeBase(BaseHdlr, ConsumeBase);
                    }
                }
            }
        } else {
            Rval = FIFO_ERROR_ARGUMENT_INVALID;
            AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (get private hdlr failed)", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

/**
 * ParamCheck for SvcFIFO_PeekEntry
 *
 * @param [in] Fifo Fifo handler
 * @param [in] Desc The result of target descriptor
 * @return 0 - OK, others - fifo error code
 */
static inline UINT32 SvcFIFO_PeekEntry_ParamCheck(const SVC_FIFO_HDLR_s *Fifo, SVC_FIFO_BITS_DESC_s *const *Desc)
{
    UINT32 Rval;
    if ((Fifo == NULL) || (Desc == NULL)) {
        Rval = FIFO_ERROR_ARGUMENT_INVALID;
        AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
 * Peek the entry in fifo by the given distance.
 * Encoder: Muxer peek in a virtual fifo
 * Decoder: Codec peek in a base fifo
 *
 * @param [in] Fifo Fifo handler
 * @param [out] Desc The result of target descriptor
 * @param [in] Index Index of the peeked entry
 * @return 0 - OK, others - fifo error code
 */
UINT32 SvcFIFO_PeekEntry(const SVC_FIFO_HDLR_s *Fifo, SVC_FIFO_BITS_DESC_s **Desc, UINT32 Index)
{
    UINT32 Rval = SvcFIFO_PeekEntry_ParamCheck(Fifo, Desc);
    if (Rval == OK) {
        SVC_FIFO_HANDLER_s *Hdlr = GetHandler(Fifo);
        if (Hdlr != NULL) {
            Rval = SvcFIFO_MtxTake(&Hdlr->Mutex, __LINE__);
            if (Rval == OK) {
                if (Hdlr->Used != 0U) {
                    UINT64 TargetPointer = Hdlr->DescQueue.ReadPointer + (UINT64)Index;
                    if ((Hdlr->DescQueue.WritePointer - Hdlr->DescQueue.ReadPointer) > (UINT64)Hdlr->DescQueue.NumEntries) {
                        (*Desc) = NULL;
                        AmbaPrint_PrintUInt5("[fifo] (%u) [FIFO] hdlr(%u) full", __LINE__, Hdlr->Public.FifoId, 0U, 0U, 0U);
                        Rval = FIFO_ERROR_FIFO_FULL;
                    } else if ((Index >= Hdlr->DescQueue.NumEntries) || (TargetPointer >= Hdlr->DescQueue.WritePointer)) {
                        (*Desc) = NULL;
                        Rval = FIFO_ERROR_INSUFFICIENT_ENTRIES;
                    } else {
                        TargetPointer = TargetPointer % Hdlr->DescQueue.NumEntries;
                        (*Desc) = &Hdlr->DescQueue.Queue[TargetPointer];
                    }
                } else {
                    Rval = FIFO_ERROR_ARGUMENT_INVALID;
                    AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (unused hdlr)", __LINE__, 0U, 0U, 0U, 0U);
                }
                Rval = SvcFIFO_MtxGive(&Hdlr->Mutex, __LINE__, Rval);
            }
        } else {
            Rval = FIFO_ERROR_ARGUMENT_INVALID;
            AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (get private hdlr failed)", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

/**
 * ParamCheck for SvcFIFO_PrepareEntry
 *
 * @param [in] Fifo Fifo handler
 * @param [out] Desc The descriptor of the space
 * @return 0 - OK, others - fifo error code
 */
static inline UINT32 SvcFIFO_PrepareEntry_ParamCheck(const SVC_FIFO_HDLR_s *Fifo, const SVC_FIFO_BITS_DESC_s *Desc)
{
    UINT32 Rval;
    if ((Fifo == NULL) || (Desc == NULL)) {
        Rval = FIFO_ERROR_ARGUMENT_INVALID;
        AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
 * Prepare space to write entries to fifo.
 * The function can only be called by a virtual fifo.
 * Encoder: No use
 * Decoder: Demuxer request write pointer "directly", not through fifo Task
 *
 * @param [in] Fifo Fifo handler
 * @param [out] Desc The descriptor of the space
 * @return 0 - OK, others - fifo error code
 */
UINT32 SvcFIFO_PrepareEntry(const SVC_FIFO_HDLR_s *Fifo, SVC_FIFO_BITS_DESC_s *Desc)
{
    UINT32 Rval = SvcFIFO_PrepareEntry_ParamCheck(Fifo, Desc);
    if (Rval == OK) {
        SVC_FIFO_HANDLER_s *Hdlr = GetHandler(Fifo);
        if (Hdlr != NULL) {
            /* Do not prepare while writing */
            SvcFIFO_FlagGet(&Hdlr->Flag, SVC_FIFO_FLAG_PROCESS_WRITE, 0x0U, 0U, __LINE__, &Rval);
            if (Rval == OK) {
                Rval = SvcFIFO_MtxTake(&Hdlr->Mutex, __LINE__);
                if (Rval == OK) {
                    const SVC_FIFO_HDLR_s *Public = NULL;
                    SVC_FIFO_CALLBACK_f CbEvent = NULL;
                    if ((Hdlr->Used != 0U) && (Hdlr->ParentBase != NULL)) {
                        UINT64 Dist = Hdlr->DescQueue.WritePointer - Hdlr->DescQueue.ReadPointer;
                        Public = &(Hdlr->ParentBase->Public);
                        CbEvent = Hdlr->ParentBase->CbEvent;
                        if (Dist >= (UINT64)Hdlr->DescQueue.NumEntries) {
                            Rval = FIFO_ERROR_FIFO_FULL;
                        }
                    } else {
                        Rval = FIFO_ERROR_ARGUMENT_INVALID;
                        AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (unused hdlr)", __LINE__, 0U, 0U, 0U, 0U);
                    }
                    Rval = SvcFIFO_MtxGive(&Hdlr->Mutex, __LINE__, Rval);
                    if (Rval == OK) {
                        SVC_FIFO_CALLBACK_INFO_s Info = {0};
                        Info.Desc = Desc;
                        /* callback to get bitsbuffer info from codec */
                        Rval = SvcFIFO_SendCallbackEvent(CbEvent, Public, SVC_FIFO_EVENT_GET_WRITE_POINT, &Info);
                        if ((Rval == OK) && (Desc->Size == 0U)) {
                            Rval = FIFO_ERROR_FIFO_FULL;
                        }
                    }
                }
            } else {
                Rval = FIFO_ERROR_FATAL;
                AmbaPrint_PrintUInt5("[fifo] (%u) Take event flag(AMBA_FIFO_FLAG_PROCESS_EVENT) failed", __LINE__, 0U, 0U, 0U, 0U);
            }
        } else {
            Rval = FIFO_ERROR_ARGUMENT_INVALID;
            AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (get private hdlr failed)", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

/**
 * ParamCheck for SvcFIFO_RemoveEntry
 *
 * @param [in] Fifo Fifo handler
 * @return 0 - OK, others - fifo error code
 */
static inline UINT32 SvcFIFO_RemoveEntry_ParamCheck(const SVC_FIFO_HDLR_s *Fifo)
{
    UINT32 Rval;
    if (Fifo == NULL) {
        Rval = FIFO_ERROR_ARGUMENT_INVALID;
        AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
 * Remove N entries of a fifo handler
 * Encoder: Muxer remove virtual fifo entries, ask fifo Task to sync base fifo
 *          (For Non-AutoRemove) codec remove base fifo entries.
 * Decoder: Codec remove base fifo entries, ask fifo Task to sync virtual fifo
 *
 * @param [in] Fifo Fifo handler
 * @param [in] NumEntries The number of entry to remove
 * @return 0 - OK, others - fifo error code
 */
UINT32 SvcFIFO_RemoveEntry(const SVC_FIFO_HDLR_s *Fifo, UINT32 NumEntries)
{
    UINT32 Rval  = SvcFIFO_RemoveEntry_ParamCheck(Fifo);
    if (Rval == OK) {
        SVC_FIFO_HANDLER_s *Hdlr = GetHandler(Fifo);
        if (Hdlr != NULL) {
            Rval = SvcFIFO_MtxTake(&Hdlr->Mutex, __LINE__);
            if (Rval == OK) {
                if (Hdlr->Used != 0U) {
                    UINT32 Dist = (UINT32)(Hdlr->DescQueue.WritePointer - Hdlr->DescQueue.ReadPointer);
                    UINT8 IsBase = (Hdlr->ParentBase == NULL)? 1U : 0U;
                    UINT8 AutoRemove = Hdlr->AutoRemove;
                    /* Deal with caller's own fifo first */
                    if ((IsBase == 1U) && (AutoRemove == 0U)) {
                        UINT32 ConsumeBase = (NumEntries > Dist)? Dist : NumEntries;
                        Rval = SvcFIFO_MtxGive(&Hdlr->Mutex, __LINE__, Rval);
                        if (Rval == OK) {
                            Rval = SvcFIFO_ConsumeBase(Hdlr, ConsumeBase);
                        }
                    } else {
                        if (NumEntries > Dist) {
                            Rval = FIFO_ERROR_INSUFFICIENT_ENTRIES;
                            AmbaPrint_PrintUInt5("[fifo] (%u) RM too many", __LINE__, 0U, 0U, 0U, 0U);
                        } else {
                            Hdlr->DescQueue.ReadPointer += NumEntries;
                        }
                        Rval = SvcFIFO_MtxGive(&Hdlr->Mutex, __LINE__, Rval);
                    }
                    if ((Rval == OK) && ((IsBase != 1U) || (AutoRemove != 0U))) {
                        Rval = SvcFIFO_AddCmd(Hdlr, SVC_FIFO_EVENT_REMOVE_LINKED_FIFO, NumEntries, NULL);
                    }
                } else {
                    Rval = SvcFIFO_MtxGive(&Hdlr->Mutex, __LINE__, FIFO_ERROR_ARGUMENT_INVALID);
                    AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (unused hdlr)", __LINE__, 0U, 0U, 0U, 0U);
                }
            }
        } else {
            Rval = FIFO_ERROR_ARGUMENT_INVALID;
            AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (get private hdlr failed)", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

/**
 * ParamCheck for SvcFIFO_WriteEntry
 *
 * @param [in] Fifo Fifo handler
 * @param [in] Desc The descriptor of the data to write
 * @param [in] Async Asynchronous write or not
 * @return 0 - OK, others - fifo error code
 */
static inline UINT32 SvcFIFO_WriteEntry_ParamCheck(const SVC_FIFO_HDLR_s *Fifo, const SVC_FIFO_BITS_DESC_s *Desc, UINT8 Async)
{
    UINT32 Rval;
    if ((Fifo == NULL) || (Desc == NULL) || (Async > 1U)) {
        Rval = FIFO_ERROR_ARGUMENT_INVALID;
        AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
 * Write an entry to fifo, send to FIFO task directly.
 * Return immediatly if Async is set (only for encoder). Otherwise, wait the event done.
 * Encoder: Codec write to base fifo, then sync virtual fifo
 * Decoder: Demuxer write to virtual fifo,then sync base fifo
 *
 * @param [in] Fifo Fifo handler
 * @param [in] Desc The descriptor of the data to write
 * @param [in] Async Asynchronous write or not
 * @return 0 - OK, others - fifo error code
 */
UINT32 SvcFIFO_WriteEntry(const SVC_FIFO_HDLR_s *Fifo, const SVC_FIFO_BITS_DESC_s *Desc, UINT8 Async)
{
    UINT32 Rval = SvcFIFO_WriteEntry_ParamCheck(Fifo, Desc, Async);
    if (Rval == OK) {
        SVC_FIFO_HANDLER_s *Hdlr = GetHandler(Fifo);
        if (Hdlr != NULL) {
            if (Async == 0U) {
                /* prevent prepare when writing is not done yet*/
                SvcFIFO_FlagGet(&Hdlr->Flag, SVC_FIFO_FLAG_PROCESS_WRITE, /*CLEAR*/0x1U, 0U, __LINE__, &Rval);
                if (Rval == OK) {
                    Rval = SvcFIFO_AddCmd(Hdlr, SVC_FIFO_EVENT_WRITE_LINKED_FIFO, SVC_FIFO_SYNC_WRITE, Desc);
                    if (Rval == OK) {
                        SvcFIFO_FlagGet(&Hdlr->Flag, SVC_FIFO_FLAG_PROCESS_WRITE, 0x0U, 1U, __LINE__, &Rval);
                    }
                } else {
                    Rval = FIFO_ERROR_FATAL;
                    AmbaPrint_PrintUInt5("[fifo] (%u) Take event flag(AMBA_FIFO_FLAG_PROCESS_EVENT) failed", __LINE__, 0U, 0U, 0U, 0U);
                }
            } else {
                Rval = SvcFIFO_AddCmd(Hdlr, SVC_FIFO_EVENT_WRITE_LINKED_FIFO, SVC_FIFO_ASYNC_WRITE, Desc);
            }
        } else {
            Rval = FIFO_ERROR_ARGUMENT_INVALID;
            AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (get private hdlr failed)", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

/**
 * ParamCheck for SvcFIFO_Reset
 *
 * @param [in] Fifo Fifo handler
 * @return 0 - OK, others - fifo error code
 */
static inline UINT32 SvcFIFO_Reset_ParamCheck(const SVC_FIFO_HDLR_s *Fifo)
{
    UINT32 Rval;
    if (Fifo == NULL) {
        Rval = FIFO_ERROR_ARGUMENT_INVALID;
        AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
 * Reset fifo.
 *
 * @param [in] Fifo Fifo handler
 * @return 0 - OK, others - fifo error code
 */
UINT32 SvcFIFO_Reset(const SVC_FIFO_HDLR_s *Fifo)
{
    UINT32 Rval = SvcFIFO_Reset_ParamCheck(Fifo);
    if (Rval == OK) {
        SVC_FIFO_HANDLER_s *Hdlr = GetHandler(Fifo);
        if (Hdlr != NULL) {
            SvcFIFO_FlagGet(&Hdlr->Flag, SVC_FIFO_FLAG_PROCESS_RESET, /*CLEAR*/0x1U, 0U, __LINE__, &Rval);
            if (Rval== OK) {
                Rval = SvcFIFO_AddCmd(Hdlr, SVC_FIFO_EVENT_RESET_FIFO, 0U, NULL);
                if (Rval == OK) {
                    SvcFIFO_FlagGet(&Hdlr->Flag, SVC_FIFO_FLAG_PROCESS_RESET, 0x0U, 1U, __LINE__, &Rval);
                }
            }
        } else {
            Rval = FIFO_ERROR_ARGUMENT_INVALID;
            AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (get private hdlr failed)", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

/**
 * ParamCheck for SvcFIFO_GetInfo
 *
 * @param [in] Fifo Fifo handler
 * @param [in] Info Information of the fifo handler
 * @return 0 - OK, others - fifo error code
 */
static inline UINT32 SvcFIFO_GetInfo_ParamCheck(const SVC_FIFO_HDLR_s *Fifo, const SVC_FIFO_INFO_s *Info)
{
    UINT32 Rval;
    if ((Fifo == NULL) || (Info == NULL)) {
        Rval = FIFO_ERROR_ARGUMENT_INVALID;
        AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
 * Get fifo info
 *
 * @param [in] Fifo Fifo handler
 * @param [out] Info Information of the fifo handler
 * @return 0 - OK, others - fifo error code
 */
UINT32 SvcFIFO_GetInfo(const SVC_FIFO_HDLR_s *Fifo, SVC_FIFO_INFO_s *Info)
{
    UINT32 Rval = SvcFIFO_GetInfo_ParamCheck(Fifo, Info);
    if (Rval == OK) {
        SVC_FIFO_HANDLER_s *Hdlr = GetHandler(Fifo);
        if (Hdlr != NULL) {
            Rval = SvcFIFO_MtxTake(&Hdlr->Mutex, __LINE__);
            if (Rval == OK) {
                if (Hdlr->Used != 0U) {
                    Info->AvailEntries = (UINT32)(Hdlr->DescQueue.WritePointer - Hdlr->DescQueue.ReadPointer);
                    Info->AccumulateEntries = Hdlr->DescQueue.TotalWrite;
                } else {
                    Rval = FIFO_ERROR_ARGUMENT_INVALID;
                    AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (unused hdlr)", __LINE__, 0U, 0U, 0U, 0U);
                }
                Rval = SvcFIFO_MtxGive(&Hdlr->Mutex, __LINE__, Rval);
            }
        } else {
            Rval = FIFO_ERROR_ARGUMENT_INVALID;
            AmbaPrint_PrintUInt5("[fifo] (%u) Invalid argument (get private hdlr failed)", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

