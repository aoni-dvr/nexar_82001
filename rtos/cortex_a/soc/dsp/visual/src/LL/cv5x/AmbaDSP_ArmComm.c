/**
 *  @file AmbaDSP_ArmComm.c
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
 *  @details ARM-DSP communication module
 *
 */

#include <AmbaVer.h>
#include "AmbaDSP_Int.h"
#include "AmbaDSP_Log.h"
#include "AmbaDSP_ArmComm.h"
#include "AmbaDSP_EventCtrl.h"
#include "AmbaDSP_MsgDispatcher.h"
#include "ucode_error.h"

static UINT32 LLSuspendStatus = 0U;
static UINT32 ArmCommInitFlag = 0U;
/* For AMBA_DSP_MESSAGE_ARRIVED Event*/
static osal_even_t LL_MsgEventFlag;

/* Cmdbuf Mutex */
static osal_mutex_t DspCmdBufMtx GNU_SECTION_NOZEROINIT;
static osal_mutex_t DspAsyncCmdBufMtx GNU_SECTION_NOZEROINIT;

#define WAIT_ISR_TIMEOUT                    (500U)    //500ms

/**
 * Update the read/write pointers of the command queue
 * This is called during VDSP/VCAP ISR
 * @param [in] pCmdBufCtrl Pointer to the command buffer control block
 * @param [in] pGlbCmdBufCtrl Pointer to the global command buffer control block
 */
static void UpdateCmdBufReadWritePtr(AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pCmdBufCtrl, const AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pGlbCmdBufCtrl);

#if defined (CONFIG_THREADX)
/**
 * ARM-DSP communication Task entry
 * This task handles the interrupt 0-3 from DSP
 * @param [in] EntryArg Task entry argument
 */
static void* ArmCommIsrTaskEntry(void *EntryArg);
#endif

void GET_DSP_CMD_CAT(UINT32 Cmd, UINT32 *pCat)
{
    *pCat = (((Cmd)>>24U) & 0x7FU);
}

#ifdef ENABLE_CMD_MSG_CRC
//#define DEBUG_CRC
static inline void UpdateCmdBufCheckSum(AMBA_DSP_HEADER_CMD_s  *pHeader, UINT32 CmdIdx, UINT32 CmdNum)
{
    const AMBA_DSP_HEADER_CMD_s *pNewCmd = &pHeader[CmdIdx];
    const UINT32 *pU32 = NULL;
    UINT32 CRCVal;
#ifdef DEBUG_CRC
    UINT32 Time = 0U;
    ULONG StartAddr, EndAddr;
#endif

    if (CmdIdx == 1U) {
        CRCVal = 0U;
    } else {
        CRCVal = pHeader->Contents.cmd_block_crc;
    }
#ifdef DEBUG_CRC
    AmbaLL_Log(AMBALLLOG_TYPE_CMD, "  Prev CheckSum 0x%x Cmd#%d CmdNum %d", CRCVal, CmdIdx, CmdNum);
#endif

    dsp_osal_typecast(&pU32, &pNewCmd);
//    Time = dsp_osal_get_orc_timer();
    CRCVal = CalcCheckSum32Add(pU32, sizeof(dsp_cmd_t)*CmdNum, CRCVal);
#ifdef DEBUG_CRC
    AmbaLL_Log(AMBALLLOG_TYPE_ISR, "UpdateCmdBufCheckSum %u, crc:%u", dsp_osal_get_orc_timer() - Time, CRCVal, 0);
    dsp_osal_typecast(&StartAddr, &pNewCmd);
    pNewCmd = &pHeader[CmdIdx+CmdNum];
    dsp_osal_typecast(&EndAddr, &pNewCmd);
    AmbaLL_Log(AMBALLLOG_TYPE_CMD, "   Cur CheckSum 0x%x AddrS 0x%X AddrE 0x%X", CRCVal, StartAddr, EndAddr);
#endif
    pHeader->Contents.cmd_block_crc = CRCVal;
}
#endif

static void SendSyncCmd_LogCmdBufID(UINT8 CmdBufID, UINT32 WritePtr, UINT32 CmdReqReadPtr, UINT32 PrevReadPtr)
{
    if (CmdBufID == AMBA_DSP_SYNC_CMD_BUF_VDSP0) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "SendReqSyncCmd Vdsp, W/CRR/R[%d %d %d]", WritePtr, CmdReqReadPtr, PrevReadPtr);
    } else if (CmdBufID == AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "SendReqSyncCmd Vcap, W/CRR/R[%d %d %d]", WritePtr, CmdReqReadPtr, PrevReadPtr);
    } else {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "SendReqSyncCmd Unknown, W/CRR/R[%d %d %d]", WritePtr, CmdReqReadPtr, PrevReadPtr);
    }
}

static UINT32 CheckCmdBufandArmComm(UINT8 CmdBufID)
{
    UINT32 Rval;

    if (CmdBufID >= AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "    Wrong CmdBufID (%u)", (UINT32) CmdBufID, 0, 0);
        Rval = DSP_ERR_0001;
    } else if (ArmCommInitFlag != 1U) {
        Rval = ArmCommInit();
        if (Rval != OK) {
            AmbaLL_Log(AMBALLLOG_TYPE_ISR, "    ArmCommInit failed", 0, 0, 0);
        }
    } else {
        Rval = DSP_ERR_NONE;
    }

    return Rval;
}

static void CheckCmdReqTimeout(const dsp_cmd_req_msg_t *pMsgReq, const UINT8 CmdBufID)
{
    UINT32 Rval;

    if (pMsgReq->is_time_out > 0U) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR,"[Err] PreCmdReqTimeout PreCmdSeq[%d] PreCmdNum[%d]", pMsgReq->prev_cmd_seq, pMsgReq->prev_num_cmds, 0U);

        // Set VDSP TIMEOUT INT: LL_CheckDspAssert to handle Vdsp timeout flag
        if (CmdBufID == AMBA_DSP_SYNC_CMD_BUF_VDSP0) {
            Rval = DSP_SetEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, (UINT32)EVENT_FLAG_INT_VDSP0_TIMEOUT);
        } else {
            Rval = DSP_SetEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, (UINT32)EVENT_FLAG_INT_VDSP1_TIMEOUT);
        }
        if (Rval != OK) {
            AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] DSP_SetEventFlag return 0x%x", __LINE__, Rval, 0U);
        }
    }
}

static void DSP_SendSyncCmd(UINT8 CmdBufID)
{
    AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pVdspSyncCmdBufCtrl;
    const AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pVdspSyncGlbCmdBufCtrl = NULL;
    UINT32 ReadPtr = 0, WritePtr = 0, DspMaxNumMsg = 0, CmdReqCnt = 0;
    const AMBA_DSP_MSG_STATUS_s *pStatusMsg = NULL, *pMsgBufBase = NULL;
    ULONG ULAddr;
    const dsp_cmd_req_msg_t *pMsgReq;
    UINT16 i;
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 Rval;

    Rval = CheckCmdBufandArmComm(CmdBufID);
    if (Rval == OK) {
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, CmdBufID, &MemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }
    if (Rval == OK) {
        dsp_osal_typecast(&pVdspSyncCmdBufCtrl, &MemBlk.Base);

        // Commend Request Read Ptr
        ReadPtr = *(pVdspSyncCmdBufCtrl->MsgBufCtrl.pCmdReqRptr);
        WritePtr = pVdspSyncCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr;

        DspMaxNumMsg = pVdspSyncCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->max_num_msg;
        Rval = dsp_osal_cli2virt(pVdspSyncCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->base_daddr, &ULAddr);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        if (Rval == OK) {
            dsp_osal_typecast(&pMsgBufBase, &ULAddr);

            while (ReadPtr != WritePtr) {
                /* 'CmdReqMsg' must always be the 1st message from DSP/uCode */
                /* To avoid DSP assert(req_dsp_cmd_from_arm() time out) that caused by MsgDispatch task long run */
                /* SSP shall search all messages which may carry CmdReqMsg */
                pStatusMsg = &pMsgBufBase[ReadPtr];
                dsp_osal_typecast(&pMsgReq, &pStatusMsg);

                SendSyncCmd_LogCmdBufID(CmdBufID, WritePtr, ReadPtr, pVdspSyncCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->read_ptr);

                if (pMsgReq->msg_code == MSG_CMD_REQ) {
                    CmdReqCnt = 1U;

                    /* Check cmd_req timeout information */
                    CheckCmdReqTimeout(pMsgReq, CmdBufID);

                    if ((CmdBufID == AMBA_DSP_SYNC_CMD_BUF_VDSP0) &&
                        ((pMsgReq->vin_id == 0x0U) || (pMsgReq->vin_id == 0xFEU) || (pMsgReq->vin_id == 0xFFU))) {
                        AmbaLL_Log(AMBALLLOG_TYPE_ISR,"CmdBufID[%d] VinId[%u] OrcTimer[%d]", CmdBufID, pMsgReq->vin_id, dsp_osal_get_orc_timer());

                        Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP0, &MemBlk);
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                        if (Rval == OK) {
                            dsp_osal_typecast(&pVdspSyncCmdBufCtrl, &MemBlk.Base);

                            /* update Read and Write pointers, then Send Sync Counter to DSP */
                            pVdspSyncCmdBufCtrl->PrevCmdSeqNo = pMsgReq->prev_cmd_seq;
                            /* count the VDSP0 interrupt */
                            pVdspSyncCmdBufCtrl->IntFlagCounter++;

                            UpdateCmdBufReadWritePtr(pVdspSyncCmdBufCtrl, pVdspSyncGlbCmdBufCtrl);
                        }
                    } else if ((CmdBufID == AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE) &&
                               (pMsgReq->vin_id < AMBA_DSP_MAX_VIN_NUM)) {
                        /* MultiVin share same Vdsp ISR */
                        Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, (AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE + pMsgReq->vin_id), &MemBlk);
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                        if (Rval == OK) {
                            dsp_osal_typecast(&pVdspSyncCmdBufCtrl, &MemBlk.Base);

                            /* update Read and Write pointers, then Send Sync Counter to DSP */
                            pVdspSyncCmdBufCtrl->PrevCmdSeqNo = pMsgReq->prev_cmd_seq;
                            /* count the VDSP1 interrupt by vin_id, for DSP_WaitVcapEvent */
                            pVdspSyncCmdBufCtrl->IntFlagCounter++;
                            AmbaLL_Log(AMBALLLOG_TYPE_ISR,"CmdBufID[%d] VinId[%u] OrcTimer[%d]", CmdBufID, pMsgReq->vin_id, dsp_osal_get_orc_timer());

                            UpdateCmdBufReadWritePtr(pVdspSyncCmdBufCtrl, pVdspSyncGlbCmdBufCtrl);
                        }
                    } else if ((CmdBufID == AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE) &&
                               (pMsgReq->vin_id == 0xFFU)) {
                        /*
                         * When Vin enter timer mode(Vin idle), DSP will use VinId = 0xFF.
                         * Here we sweep all VinCmdQ.
                         */
                        for (i = 0U; i<AMBA_DSP_MAX_VIN_NUM; i++) {
                            Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, ((UINT32)AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE + i), &MemBlk);
                            LL_PrintErrLine(Rval, __func__, __LINE__);
                            if (Rval == OK) {
                                dsp_osal_typecast(&pVdspSyncCmdBufCtrl, &MemBlk.Base);

                                /* update Read and Write pointers, then Send Sync Counter to DSP */
                                pVdspSyncCmdBufCtrl->PrevCmdSeqNo = pMsgReq->prev_cmd_seq;
                                AmbaLL_Log(AMBALLLOG_TYPE_ISR," TMR CmdBufID[%d] VinId[%u] OrcTimer[%d]", CmdBufID, i, dsp_osal_get_orc_timer());

                                UpdateCmdBufReadWritePtr(pVdspSyncCmdBufCtrl, pVdspSyncGlbCmdBufCtrl);
                            }
                        }

                        /* count the VDSP1 interrupt by vin index, for DSP_WaitVcapEvent */
//FIXME, ISRCNT under timer mode?
                        //AmbaDSP_SyncCmdBufCtrl[AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE + i].IntFlagCounter++;
                    } else {
                        /* update Read and Write pointers, then Send Sync Counter to DSP */
                        pVdspSyncCmdBufCtrl->PrevCmdSeqNo = pMsgReq->prev_cmd_seq;
                        AmbaLL_Log(AMBALLLOG_TYPE_ISR,"CmdBufID[%d] VinId[%u] OrcTimer[%d]", CmdBufID, pMsgReq->vin_id, dsp_osal_get_orc_timer());

                        UpdateCmdBufReadWritePtr(pVdspSyncCmdBufCtrl, pVdspSyncGlbCmdBufCtrl);
                    }
                    AmbaLL_Log(AMBALLLOG_TYPE_ISR,"SetSyncCnt[%u] OrcTimer[%d]", pVdspSyncCmdBufCtrl->SyncCounter, dsp_osal_get_orc_timer(), 0U);
                    dsp_osal_dbgport_set_sync_cnt(pVdspSyncCmdBufCtrl->SyncCounter, 1U);
                    break;
                }
                /* Update Rp */
                if (++ReadPtr >= DspMaxNumMsg) {
                    ReadPtr = 0U;
                }
            }

            /* Update CmdReqRptr to WritePtr */
            *(pVdspSyncCmdBufCtrl->MsgBufCtrl.pCmdReqRptr) = WritePtr;
            if (CmdReqCnt == 0U) {
                AmbaLL_Log(AMBALLLOG_TYPE_ISR,"[Err] No CmdReqMsg, CmdBufID[%d]", CmdBufID, 0U, 0U);
            }
        }
    }
}

/**
 * Check the command sequence number continuity
 * @param [in] CurCmdSeqNo Current command sequence number reported from DSP
 * @param [in] PrevCmdSeqNo Previous command sequence number reported from DSP
 * @return 0 - OK, others - ErrorCode
 */
static inline UINT32 DSP_SeqNumChk(UINT32 CurCmdSeqNo, UINT32 PrevCmdSeqNo)
{
    UINT32 Rval = DSP_ERR_0004;

    if (CurCmdSeqNo == PrevCmdSeqNo) {  // Make sure uCode already received the previous command block
        Rval = OK;
    } else {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR | AMBALLLOG_TYPE_IN_LOCKED, "[Err] CmdSeqNum [%d %d]", CurCmdSeqNo, PrevCmdSeqNo, 0U);
    }
    return Rval;
}

static void UpdateCmdBufReadWritePtr(AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pCmdBufCtrl, const AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pGlbCmdBufCtrl)
{
    UINT32 Rval;
    AMBA_DSP_HEADER_CMD_s *pHeader;
    const dsp_cmd_t *pDspCmd;
    UINT32 CurReadPtr, CurWritePtr;
    UINT32 *CurCmdSeqNo;
    const UINT32 *PrevCmdSeqNo;
    ULONG ULAddr;
    UINT32 PhysAddr;
    osal_spinlock_t SpinLock;
    const void *pVoid = NULL;

    /* prevent entering either IRQ or FIQ handler */
    Rval = dsp_osal_cmdlock(&DspCmdBufMtx, &SpinLock);
    LL_PrintErrLine(Rval, __func__, __LINE__);

    if (Rval == OK) {
        CurReadPtr  = pCmdBufCtrl->ReadPtr;
        CurWritePtr = pCmdBufCtrl->WritePtr;
        CurCmdSeqNo = (pGlbCmdBufCtrl == NULL)? (pCmdBufCtrl->CurCmdSeqNo): (pGlbCmdBufCtrl->CurCmdSeqNo);
        PrevCmdSeqNo = (pGlbCmdBufCtrl == NULL)? &(pCmdBufCtrl->PrevCmdSeqNo): &(pGlbCmdBufCtrl->PrevCmdSeqNo);
        pDspCmd = &(pCmdBufCtrl->pCmdBufBaseAddr[CurReadPtr * pCmdBufCtrl->MaxNumCmds]);
        dsp_osal_typecast(&pHeader, &pDspCmd);
        Rval = DSP_SeqNumChk(*CurCmdSeqNo/*pCmdBufCtrl->CurCmdSeqNo*/, *PrevCmdSeqNo/*pCmdBufCtrl->PrevCmdSeqNo*/);
        if ((pHeader->Contents.num_cmds > 0U) &&
            (OK == Rval)) {
            /* Increase the command sequence number of write buffer */
            (*CurCmdSeqNo)++;
            pHeader->Contents.cmd_seq_num = (*CurCmdSeqNo);

            /* Update the cmd addr specified in dsp_init_data_t */
            dsp_osal_typecast(&ULAddr, &pHeader);
            Rval = dsp_osal_virt2cli(ULAddr, &PhysAddr);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (OK == Rval) {
                *pCmdBufCtrl->pCmdQueueBaseAddr = PhysAddr;
                dsp_osal_typecast(&pVoid, &pCmdBufCtrl->pCmdQueueBaseAddr);
                Rval = dsp_osal_cache_clean(pVoid, (UINT32)sizeof(UINT32));
                AmbaMisra_TouchUnused(&Rval);
            }

            {   // For the log
                const dsp_cmd_t *pCmd;
                UINT32 CurCmdIdx, CurCmdCnt;

                CurCmdCnt = pHeader->Contents.num_cmds;
                pCmd = &pDspCmd[1];
                dsp_osal_typecast(&ULAddr, &pHeader);

                if (pCmdBufCtrl->SyncCounter == DSP_SYNC_COUNTER_CMD_REQ_VDSP0) {
                    AmbaLL_Log(AMBALLLOG_TYPE_ISR | AMBALLLOG_TYPE_IN_LOCKED, "Vdsp, CmdBuf 0x%X%X", DSP_GetU64Msb((UINT64)ULAddr), DSP_GetU64Lsb((UINT64)ULAddr), 0U);
                    AmbaLL_Log(AMBALLLOG_TYPE_ISR | AMBALLLOG_TYPE_IN_LOCKED, "      CmdSeq %d, NumCmds %d",
                        pHeader->Contents.cmd_seq_num, pHeader->Contents.num_cmds, 0U);
                } else if (pCmdBufCtrl->SyncCounter == DSP_SYNC_COUNTER_CMD_REQ_VDSP1) {
                    AmbaLL_Log(AMBALLLOG_TYPE_ISR | AMBALLLOG_TYPE_IN_LOCKED, "Vcap, CmdBuf 0x%X%X", DSP_GetU64Msb((UINT64)ULAddr), DSP_GetU64Lsb((UINT64)ULAddr), 0U);
                    AmbaLL_Log(AMBALLLOG_TYPE_ISR | AMBALLLOG_TYPE_IN_LOCKED, "      CmdSeq %d, NumCmds %d, CmdBufId %d",
                        pHeader->Contents.cmd_seq_num, pHeader->Contents.num_cmds, pCmdBufCtrl->Id);
                } else if (pCmdBufCtrl->SyncCounter == DSP_SYNC_COUNTER_CMD_REQ_ASYNC) {
                    AmbaLL_Log(AMBALLLOG_TYPE_ISR | AMBALLLOG_TYPE_IN_LOCKED, "Async, CmdBuf 0x%X%X", DSP_GetU64Msb((UINT64)ULAddr), DSP_GetU64Lsb((UINT64)ULAddr), 0U);
                    AmbaLL_Log(AMBALLLOG_TYPE_ISR | AMBALLLOG_TYPE_IN_LOCKED, "       CmdSeq %d, NumCmds %d, CmdBufId %d",
                        pHeader->Contents.cmd_seq_num, pHeader->Contents.num_cmds, pCmdBufCtrl->Id);
                } else {
                    AmbaLL_Log(AMBALLLOG_TYPE_ISR | AMBALLLOG_TYPE_IN_LOCKED, "Unknown, CmdBuf 0x%X%X", DSP_GetU64Msb((UINT64)ULAddr), DSP_GetU64Lsb((UINT64)ULAddr), 0U);
                    AmbaLL_Log(AMBALLLOG_TYPE_ISR | AMBALLLOG_TYPE_IN_LOCKED, "         CmdSeq %d, NumCmds %d",
                        pHeader->Contents.cmd_seq_num, pHeader->Contents.num_cmds, 0U);
                }

                for (CurCmdIdx = 0U; CurCmdIdx < CurCmdCnt; CurCmdIdx ++) {
                    AmbaLL_Log(AMBALLLOG_TYPE_ISR | AMBALLLOG_TYPE_IN_LOCKED, "  CmdCode 0x%X", pCmd->cmd_code, 0U, 0U);
                    pCmd++;
                }
            }

            /* move the ReadPtr forward */
            pCmdBufCtrl->ReadPtr++;
            if (pCmdBufCtrl->ReadPtr >= pCmdBufCtrl->MaxNumCmdBuf) {
                pCmdBufCtrl->ReadPtr = 0U;
            }

            /*-----------------------------------------------------------------------*\
             *  move the WritePtr forward, but if the WritePtr was already moved forward
             *  by WriteCmdBuf() due to cmd buf fullness, there is no need.
            \*-----------------------------------------------------------------------*/
            if (CurReadPtr == CurWritePtr) {
                /* move the Write Pointer forward */
                pCmdBufCtrl->WritePtr++;
                if (pCmdBufCtrl->WritePtr >= pCmdBufCtrl->MaxNumCmdBuf) {
                    pCmdBufCtrl->WritePtr = 0U;
                }

                CurWritePtr = pCmdBufCtrl->WritePtr;
                pDspCmd = &(pCmdBufCtrl->pCmdBufBaseAddr[CurWritePtr * pCmdBufCtrl->MaxNumCmds]);
                dsp_osal_typecast(&pHeader, &pDspCmd);
                pHeader->Contents.num_cmds = 0;  /* reset the number of commands */
            }
            /* Clear cmd lock */
            pCmdBufCtrl->CmdLock = 0U;
        }

        Rval = dsp_osal_cmdunlock(&DspCmdBufMtx, &SpinLock);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }
}

static void VdspEventHandler_LogMsg(UINT32 SyncCounter, UINT32 MsgCode, UINT32 ReadEndPtr, UINT32 ReadPtr)
{
    if (SyncCounter == DSP_SYNC_COUNTER_CMD_REQ_VDSP0) {
        AmbaLL_Log(AMBALLLOG_TYPE_MSG_HEAD, "VDSP0 GEN MSG: 0x%X, W/R[%d %d]", MsgCode, ReadEndPtr, ReadPtr);
    } else if (SyncCounter == DSP_SYNC_COUNTER_CMD_REQ_VDSP1) {
        AmbaLL_Log(AMBALLLOG_TYPE_MSG_HEAD, "VDSP1 VIN MSG: 0x%X, W/R[%d %d]", MsgCode, ReadEndPtr, ReadPtr);
    } else if (SyncCounter == DSP_SYNC_COUNTER_CMD_REQ_ASYNC) {
        AmbaLL_Log(AMBALLLOG_TYPE_MSG_HEAD, "VDSP2 ASYNC MSG: 0x%X, W/R[%d %d]", MsgCode, ReadEndPtr, ReadPtr);
    } else {
        AmbaLL_Log(AMBALLLOG_TYPE_MSG_HEAD, "VPROC MSG: 0x%X, W/R[%d %d]", MsgCode, ReadEndPtr, ReadPtr);
    }
}

static void DSP_ResetUnreadMsgCount(AMBA_DSP_MSG_BUF_CTRL_s *pMsgBufCtrl)
{
    UINT32 CurWp, PreWp;

    CurWp = pMsgBufCtrl->pMsgFifoCtrl->write_ptr;
    PreWp = pMsgBufCtrl->DspMsgLastPtr;

    if (PreWp <= CurWp) {
        pMsgBufCtrl->UnReadMsgNum = (CurWp - PreWp);
    } else {
        pMsgBufCtrl->UnReadMsgNum = ((CurWp + pMsgBufCtrl->pMsgFifoCtrl->max_num_msg) - PreWp);
    }
}

/**
 * VDSP_INT event handler
 * @param [in] pMsgBufCtrl Pointer to the message buffer control block
 * @param [in] SyncCounter Sync counter of the command bufffer control block
 * @param [in] pPrevCmdSeqNo Previous command sequence number reported from DSP
 * @param [in] IsVpMsg 1 - is VP message, 0 - is not VP message
 * @return 0 - OK, other - ErrorCode
 */
static UINT32 ArmCommVdspEventHandler(AMBA_DSP_MSG_BUF_CTRL_s *pMsgBufCtrl, UINT32 SyncCounter, UINT8 IsVpMsg)
{
    UINT32 Rval = OK;
    UINT32 GenVdspEvent = 0U;
    osal_spinlock_t SpinLock;
    UINT32 ReadPtr, ReadEndPtr;
    UINT32 DspMaxNumMsg = pMsgBufCtrl->pMsgFifoCtrl->max_num_msg;
    AMBA_DSP_MSG_STATUS_s *pStatusMsg;
    AMBA_DSP_MSG_STATUS_s *pMsgBufBase;
    const dsp_cmd_req_msg_t *pMsgReq;
    UINT32 MsgNum = 0;
    ULONG ULAddr;

    Rval = dsp_osal_cli2virt(pMsgBufCtrl->pMsgFifoCtrl->base_daddr, &ULAddr);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        dsp_osal_typecast(&pStatusMsg, &ULAddr);
        pMsgBufBase = pStatusMsg;

        Rval = dsp_osal_cmdlock(&DspCmdBufMtx, &SpinLock);      /* prevent entering either IRQ or FIQ handler */
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            ReadPtr = pMsgBufCtrl->DspMsgRp;
            ReadEndPtr = pMsgBufCtrl->DspMsgLastPtr;

            // Reset unread msg count
            DSP_ResetUnreadMsgCount(pMsgBufCtrl);

            Rval = dsp_osal_cmdunlock(&DspCmdBufMtx, &SpinLock);      /* prevent entering either IRQ or FIQ handler */
            LL_PrintErrLine(Rval, __func__, __LINE__);

            /* process all DSP message data */
            while (ReadPtr != ReadEndPtr) {
                MsgNum++;
                pStatusMsg = &pMsgBufBase[ReadPtr];
                dsp_osal_typecast(&pMsgReq, &pStatusMsg);

                VdspEventHandler_LogMsg(SyncCounter, pMsgReq->msg_code, ReadEndPtr, ReadPtr);

                if ((pMsgReq->req_cmd != 0U) && (pMsgReq->msg_code == MSG_CMD_REQ) && (IsVpMsg == 0U)) {
                    //AmbaLL_Log(AMBALLLOG_TYPE_MSG, "Get ReqMsg NoVp, bypass it", 0,0,0);
                } else if ((pMsgReq->req_cmd != 0U) && (pMsgReq->msg_code == MSG_CMD_REQ)) {
                    //AmbaLL_Log(AMBALLLOG_TYPE_MSG, "Get ReqMsg, bypass it", 0,0,0);
                } else {
                    if (IsVpMsg == 1U) {
                        msg_vp_hier_out_lane_out_t *VpMsg;
                        dsp_osal_typecast(&VpMsg, &pStatusMsg);

                        //for debug, overwirte resv field as "BEEF" to parsing it
                        VpMsg->header.reserved = VP_MSG_PATTERN;
                    }

                    Rval |= LL_MsgParser(pStatusMsg);

                    GenVdspEvent |= ((IsVpMsg == 0U)? 1U: 0U);
                }

                ReadPtr++;
                if (ReadPtr >= DspMaxNumMsg) {
                    ReadPtr = 0U;
                }
            }

            //update read_ptr in the end
            if (MsgNum > 0U) {
                pMsgBufCtrl->DspMsgRp = ReadPtr;
                pMsgBufCtrl->pMsgFifoCtrl->read_ptr = ReadPtr;
            }

            if (GenVdspEvent != 0U) {
                // DO NOTHING
            }
        }
    }

    return Rval;
}

static UINT32 DSP_MsgOverflowDetection(const AMBA_DSP_MSG_BUF_CTRL_s *pMsgBufCtrl, UINT32 MsgBufId, UINT32 AuxId)
{
    UINT32 Rval = OK;
    UINT32 TotalUnRead = 0U, MaxBufCnt;

    TotalUnRead = pMsgBufCtrl->UnReadMsgNum;
    MaxBufCnt = pMsgBufCtrl->pMsgFifoCtrl->max_num_msg;

    if (TotalUnRead >= MaxBufCnt) {
        switch (MsgBufId) {
            case DSP_PROT_BUF_SYNC_CMD_CTRL:
                {
                    if (AuxId == AMBA_DSP_SYNC_CMD_BUF_VDSP0) {
                        AmbaLL_Log(AMBALLLOG_TYPE_ERR, "[Err @ ArmComm] VDSP0MsgBuffer overflow: unread_msg[%d] max_msg[%d]", TotalUnRead, MaxBufCnt, 0U);
                    } else if (AuxId == AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE) {
                        AmbaLL_Log(AMBALLLOG_TYPE_ERR, "[Err @ ArmComm] VDSP1MsgBuffer overflow: unread_msg[%d] max_msg[%d]", TotalUnRead, MaxBufCnt, 0U);
                    } else {
                        AmbaLL_Log(AMBALLLOG_TYPE_ERR, "[Err @ ArmComm] Unknown MsgBuffer overflow: unread_msg[%d] max_msg[%d]", TotalUnRead, MaxBufCnt, 0U);
                    }
                }
                break;
            case DSP_PROT_BUF_ASYNC_CMD_CTRL:
                AmbaLL_Log(AMBALLLOG_TYPE_ERR, "[Err @ ArmComm] AsyncMsgBuffer overflow: unread_msg[%d] max_msg[%d]", TotalUnRead, MaxBufCnt, 0U);
                break;
            case DSP_PROT_BUF_VPROC_MSG_BUF_CTRL:
                AmbaLL_Log(AMBALLLOG_TYPE_ERR, "[Err @ ArmComm] VprocMsgBuffer overflow: unread_msg[%d] max_msg[%d]", TotalUnRead, MaxBufCnt, 0U);
                break;
            case DSP_PROT_BUF_ASYNC_ENC_MSG_BUF_CTRL:
                AmbaLL_Log(AMBALLLOG_TYPE_ERR, "[Err @ ArmComm] AsyncEncMsgBuffer overflow: unread_msg[%d] max_msg[%d]", TotalUnRead, MaxBufCnt, 0U);
                break;
            default:
                AmbaLL_Log(AMBALLLOG_TYPE_ERR, "[Err @ ArmComm] Unknown MsgBuffer overflow: unread_msg[%d] max_msg[%d]", TotalUnRead, MaxBufCnt, 0U);
                break;
        }
        Rval = DSP_ERR_0005;
    }
    return Rval;
}

static void DSP_CalUnreadMsgCount(AMBA_DSP_MSG_BUF_CTRL_s *pMsgBufCtrl)
{
    UINT32 Rval, CurWp, PreWp;
    osal_spinlock_t SpinLock;

    /* prevent entering either IRQ or FIQ handler */
    Rval = dsp_osal_cmdlock(&DspCmdBufMtx, &SpinLock);
    LL_PrintErrLine(Rval, __func__, __LINE__);

    if (Rval == OK) {
        CurWp = pMsgBufCtrl->pMsgFifoCtrl->write_ptr;
        PreWp = *(pMsgBufCtrl->pCmdReqRptr);

        if (PreWp <= CurWp) {
            pMsgBufCtrl->UnReadMsgNum += (CurWp - PreWp);
        } else {
           pMsgBufCtrl->UnReadMsgNum += ((CurWp + pMsgBufCtrl->pMsgFifoCtrl->max_num_msg) - PreWp);
        }

        Rval = dsp_osal_cmdunlock(&DspCmdBufMtx, &SpinLock);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }
}

// disable NEON registers usage in ISR
#pragma GCC push_options
#pragma GCC target("general-regs-only")

/**
 * AmbaDSP_ArmCommTask event handler for VDSP0 ISR.\n
 * 1. DSP_SendSyncCmd
 *    a. Update rp.
 *    b. Set DSP sync counter
 */
static void DSP_HandleVdsp0ISR(void)
{
    DSP_MEM_BLK_t MemBlk = {0};
    AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pSyncCmdBufCtrl;
    UINT32 Rval;

    AmbaLL_Log(AMBALLLOG_TYPE_ISR, "Handle VDSP0 interrupt: OrcTimer[%d]", dsp_osal_get_orc_timer(), 0U, 0U);
    Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP0, &MemBlk);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] DSP_GetProtBuf return 0x%x", __LINE__, Rval, 0U);
    } else {
        dsp_osal_typecast(&pSyncCmdBufCtrl, &MemBlk.Base);
        /* Calculate unread msg count to detect buffer overflow */
        DSP_CalUnreadMsgCount(&(pSyncCmdBufCtrl->MsgBufCtrl));
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "                        UnreadMsg[%d] W/R[%d %d]"
                , pSyncCmdBufCtrl->MsgBufCtrl.UnReadMsgNum
                , pSyncCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr
                , pSyncCmdBufCtrl->MsgBufCtrl.DspMsgRp);
    }
    /* Check command request and update CmdReqRptr to WritePtr */
    DSP_SendSyncCmd(AMBA_DSP_SYNC_CMD_BUF_VDSP0);

    /* Set message arrived flag */
    Rval = dsp_osal_eventflag_set(&LL_MsgEventFlag, AMBA_DSP_MESSAGE_ARRIVED);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] dsp_osal_eventflag_set return 0x%x", __LINE__, Rval, 0U);
    }

    /* Set DSP INT Event */
    Rval = DSP_SetEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, (UINT32)EVENT_FLAG_INT_VDSP0_EVENT);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] DSP_SetEventFlag return 0x%x", __LINE__, Rval, 0U);
    }
}

static void DSP_HandleVdsp1ISR(void)
{
    DSP_MEM_BLK_t MemBlk = {0};
    AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pSyncCmdBufCtrl;
    UINT32 Rval;

    AmbaLL_Log(AMBALLLOG_TYPE_ISR, "Handle VDSP1 interrupt: OrcTimer[%d]", dsp_osal_get_orc_timer(), 0U, 0U);
    Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE, &MemBlk);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] DSP_GetProtBuf return 0x%x", __LINE__, Rval, 0U);
    } else {
        dsp_osal_typecast(&pSyncCmdBufCtrl, &MemBlk.Base);
        /* Calculate unread msg count to detect buffer overflow */
        DSP_CalUnreadMsgCount(&(pSyncCmdBufCtrl->MsgBufCtrl));
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "                        UnreadMsg[%d] W/R[%d %d]"
        , pSyncCmdBufCtrl->MsgBufCtrl.UnReadMsgNum
        , pSyncCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr
        , pSyncCmdBufCtrl->MsgBufCtrl.DspMsgRp);
    }
    /* Check command request and update CmdReqRptr to WritePtr */
    DSP_SendSyncCmd(AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE);

    /* Set message arrived flag */
    Rval = dsp_osal_eventflag_set(&LL_MsgEventFlag, AMBA_DSP_MESSAGE_ARRIVED);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] dsp_osal_eventflag_set return 0x%x", __LINE__, Rval, 0U);
    }

    // Set DSP INT Event
    Rval = DSP_SetEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, (UINT32)EVENT_FLAG_INT_VDSP1_EVENT);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] DSP_SetEventFlag return 0x%x", __LINE__, Rval, 0U);
    }
}

static void DSP_HandleVdsp2ISR(void)
{
    DSP_MEM_BLK_t MemBlk = {0};
    AMBA_DSP_ASYNC_CMD_BUF_CTRL_s *pAsyncCmdBufCtrl;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_ASYNC_CMD_CTRL, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] DSP_GetProtBuf Rval[0x%X]", __LINE__, Rval, 0U);
    } else {
        dsp_osal_typecast(&pAsyncCmdBufCtrl, &MemBlk.Base);
        /* Calculate unread msg count to detect buffer overflow */
        DSP_CalUnreadMsgCount(&(pAsyncCmdBufCtrl->MsgBufCtrl));
        /* Update CmdReqRptr to WritePtr */
        *(pAsyncCmdBufCtrl->MsgBufCtrl.pCmdReqRptr) = pAsyncCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr;

        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "Handle VDSP2 interrupt: UnreadMsg[%d] W/R[%d %d]"
        , pAsyncCmdBufCtrl->MsgBufCtrl.UnReadMsgNum
        , pAsyncCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr
        , pAsyncCmdBufCtrl->MsgBufCtrl.DspMsgRp);

    }

    /* Set message arrived flag */
    Rval = dsp_osal_eventflag_set(&LL_MsgEventFlag, AMBA_DSP_MESSAGE_ARRIVED);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] dsp_osal_eventflag_set return 0x%x", __LINE__, Rval, 0U);
    }

    /* Set DSP INT Event */
    Rval = DSP_SetEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, (UINT32)EVENT_FLAG_INT_VDSP2_EVENT);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] DSP_SetEventFlag return 0x%x", __LINE__, Rval, 0U);
    }
}

static void DSP_HandleVdsp3ISR(void)
{
    DSP_MEM_BLK_t MemBlk = {0};
    AMBA_DSP_MSG_BUF_CTRL_s *pMsgBufCtrl;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_VPROC_MSG_BUF_CTRL, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] DSP_GetProtBuf return 0x%x", __LINE__, Rval, 0U);
    } else {
        dsp_osal_typecast(&pMsgBufCtrl, &MemBlk.Base);
        /* Calculate unread msg count to detect buffer overflow */
        DSP_CalUnreadMsgCount(pMsgBufCtrl);
        /* Update CmdReqRptr to WritePtr */
        *(pMsgBufCtrl->pCmdReqRptr) = pMsgBufCtrl->pMsgFifoCtrl->write_ptr;

        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "Handle VDSP3 interrupt: UnreadMsg[%d] W/R[%d %d]"
        , pMsgBufCtrl->UnReadMsgNum
        , pMsgBufCtrl->pMsgFifoCtrl->write_ptr
        , pMsgBufCtrl->DspMsgRp);
    }

    /* Set message arrived flag */
    Rval = dsp_osal_eventflag_set(&LL_MsgEventFlag, AMBA_DSP_MESSAGE_ARRIVED);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] dsp_osal_eventflag_set return 0x%x", __LINE__, Rval, 0U);
    }

    /* Set DSP INT Event */
    Rval = DSP_SetEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, (UINT32)EVENT_FLAG_INT_VDSP3_EVENT);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] DSP_SetEventFlag return 0x%x", __LINE__, Rval, 0U);
    }
}

static void DSP_HandleVdsp4ISR(void)
{
    DSP_MEM_BLK_t MemBlk = {0};
    AMBA_DSP_MSG_BUF_CTRL_s *pMsgBufCtrl;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_ASYNC_ENC_MSG_BUF_CTRL, 0U, &MemBlk);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] DSP_GetProtBuf return 0x%x", __LINE__, Rval, 0U);
    } else {
        dsp_osal_typecast(&pMsgBufCtrl, &MemBlk.Base);
        /* Calculate unread msg count to detect buffer overflow */
        DSP_CalUnreadMsgCount(pMsgBufCtrl);
        /* Update CmdReqRptr to WritePtr */
        *(pMsgBufCtrl->pCmdReqRptr) = pMsgBufCtrl->pMsgFifoCtrl->write_ptr;

        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "Handle VDSP4 interrupt: UnreadMsg[%d] W/R[%d %d]"
        , pMsgBufCtrl->UnReadMsgNum
        , pMsgBufCtrl->pMsgFifoCtrl->write_ptr
        , pMsgBufCtrl->DspMsgRp);
    }

    /* Set message arrived flag */
    Rval = dsp_osal_eventflag_set(&LL_MsgEventFlag, AMBA_DSP_MESSAGE_ARRIVED);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] dsp_osal_eventflag_set return 0x%x", __LINE__, Rval, 0U);
    }

    /* Set DSP INT Event */
    Rval = DSP_SetEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, (UINT32)EVENT_FLAG_INT_VDSP4_EVENT);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] DSP_SetEventFlag return 0x%x", __LINE__, Rval, 0U);
    }
}

#if defined(CONFIG_THREADX)
static void DSP_Vdsp0ISRHandler(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t DSP_Vdsp0ISRHandler(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void DSP_Vdsp0ISRHandler(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *DSP_Vdsp0ISRHandler(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
    AmbaMisra_TouchUnused(&IntID);
    AmbaMisra_TouchUnused(&UserArg);

    AmbaLL_Log(AMBALLLOG_TYPE_INT, "VDSP0 interrupt OrcTimer[%d]", dsp_osal_get_orc_timer(), 0U, 0U);

#if defined(CONFIG_THREADX)
    // Set VDSP0 INT: ArmCommIsrTask to handle Vdsp0 ISR
    Rval = DSP_SetEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, (UINT32)EVENT_FLAG_INT_VDSP0_INT);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] DSP_SetEventFlag return 0x%x", __LINE__, Rval, 0U);
    }
#else
    DSP_HandleVdsp0ISR();
#endif

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

/**
 * AmbaDSP_ArmCommTask event handler for VDSP1 ISR.\n
 */
#if defined(CONFIG_THREADX)
static void DSP_Vdsp1ISRHandler(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t DSP_Vdsp1ISRHandler(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void DSP_Vdsp1ISRHandler(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *DSP_Vdsp1ISRHandler(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
    AmbaMisra_TouchUnused(&IntID);
    AmbaMisra_TouchUnused(&UserArg);

    AmbaLL_Log(AMBALLLOG_TYPE_INT, "VDSP1 interrupt OrcTimer[%d]", dsp_osal_get_orc_timer(), 0U, 0U);

#if defined(CONFIG_THREADX)
    // Set VDSP1 INT: ArmCommIsrTask to handle Vdsp1 ISR
    Rval = DSP_SetEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, (UINT32)EVENT_FLAG_INT_VDSP1_INT);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] DSP_SetEventFlag return 0x%x", __LINE__, Rval, 0U);
    }
#else
    DSP_HandleVdsp1ISR();
#endif

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

/**
 * AmbaDSP_ArmCommTask event handler for VDSP2 ISR.\n
 */
#if defined(CONFIG_THREADX)
static void DSP_Vdsp2ISRHandler(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t DSP_Vdsp2ISRHandler(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void DSP_Vdsp2ISRHandler(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *DSP_Vdsp2ISRHandler(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
    AmbaMisra_TouchUnused(&IntID);
    AmbaMisra_TouchUnused(&UserArg);

    AmbaLL_Log(AMBALLLOG_TYPE_INT, "VDSP2 interrupt", 0U, 0U, 0U);

#if defined(CONFIG_THREADX)
    // Set VDSP2 INT: ArmCommIsrTask to handle Vdsp2 ISR
    Rval = DSP_SetEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, (UINT32)EVENT_FLAG_INT_VDSP2_INT);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] DSP_SetEventFlag return 0x%x", __LINE__, Rval, 0U);
    }
    // Set ASYNC CMD ASK INT: WriteAsyncCmdBuf to handle this ISR
    Rval = DSP_SetEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, (UINT32)EVENT_FLAG_INT_CMD_ACK_ASYNC);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] DSP_SetEventFlag return 0x%x", __LINE__, Rval, 0U);
    }
#else
    Rval = DSP_SetEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, (UINT32)EVENT_FLAG_INT_CMD_ACK_ASYNC);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] DSP_SetEventFlag return 0x%x", __LINE__, Rval, 0U);
    }
    DSP_HandleVdsp2ISR();
#endif

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

/**
 * AmbaDSP_ArmCommTask event handler for VDSP3 ISR.\n
 */
#if defined(CONFIG_THREADX)
static void DSP_Vdsp3ISRHandler(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t DSP_Vdsp3ISRHandler(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void DSP_Vdsp3ISRHandler(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *DSP_Vdsp3ISRHandler(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
    AmbaMisra_TouchUnused(&IntID);
    AmbaMisra_TouchUnused(&UserArg);

    AmbaLL_Log(AMBALLLOG_TYPE_INT, "VDSP3 interrupt", 0U, 0U, 0U);

#if defined(CONFIG_THREADX)
    // Set VDSP3 INT: ArmCommIsrTask to handle Vdsp3 ISR
    Rval = DSP_SetEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, (UINT32)EVENT_FLAG_INT_VDSP3_INT);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] DSP_SetEventFlag return 0x%x", __LINE__, Rval, 0U);
    }
#else
    DSP_HandleVdsp3ISR();
#endif

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

/**
 * AmbaDSP_ArmCommTask event handler for VDSP3 ISR.\n
 */
#if defined(CONFIG_THREADX)
static void DSP_Vdsp4ISRHandler(uint32_t IntID, uint32_t UserArg)
#elif defined(CONFIG_LINUX)
static irqreturn_t DSP_Vdsp4ISRHandler(int32_t IntID, void *UserArg)
#else
#ifdef USE_ISR_EVENT_ATTACH
static void DSP_Vdsp4ISRHandler(UINT32 IntID, void *UserArg)
#else
static const struct sigevent *DSP_Vdsp4ISRHandler(void *UserArg, uint32_t IntID)
#endif
#endif
{
    UINT32 Rval;
    AmbaMisra_TouchUnused(&IntID);
    AmbaMisra_TouchUnused(&UserArg);

    AmbaLL_Log(AMBALLLOG_TYPE_INT, "VDSP4 interrupt", 0U, 0U, 0U);

#if defined(CONFIG_THREADX)
    // Set VDSP4 INT: ArmCommIsrTask to handle Vdsp4 ISR
    Rval = DSP_SetEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, (UINT32)EVENT_FLAG_INT_VDSP4_INT);
    if (Rval != OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[%d] DSP_SetEventFlag return 0x%x", __LINE__, Rval, 0U);
    }
#else
    DSP_HandleVdsp4ISR();
#endif

#if defined(CONFIG_LINUX)
    return IRQ_HANDLED;
#endif
}

#pragma GCC pop_options

UINT32 DSP_ClearGroupCmdBuffer(UINT8 CmdBufID)
{
    UINT32 Rval = OK;
    AMBA_DSP_HEADER_CMD_s *pHeader = NULL;
    const AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pCmdBufCtrl;
    DSP_MEM_BLK_t MemBlk = {0};

    AmbaLL_Log(AMBALLLOG_TYPE_ISR, "[ClearGroupCmdBuffer]", 0, 0, 0);
    Rval = CheckCmdBufandArmComm(CmdBufID);

    if (Rval == OK) {
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, CmdBufID, &MemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == DSP_ERR_NONE) {
            dsp_osal_typecast(&pCmdBufCtrl, &MemBlk.Base);

            /* Initialize GrpCmdBuf with CMD_DSP_HEADER */
            dsp_osal_typecast(&pHeader, &pCmdBufCtrl->pGroupCmdBufBaseAddr);
            pHeader->Contents.num_cmds = 0U;
        }
    }

    return Rval;
}

void DSP_ClearAllUnreadMsg(void)
{
    UINT32 Rval = OK;
    AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pCmdBufCtrl;
    AMBA_DSP_ASYNC_CMD_BUF_CTRL_s *pAsyncCmdBufCtrl;
    AMBA_DSP_MSG_BUF_CTRL_s *pMsgBufCtrl;
    DSP_MEM_BLK_t MemBlk = {0};

    if (ArmCommInitFlag != 1U) {
        Rval = ArmCommInit();
    }
    LL_PrintErrLine(Rval, __func__, __LINE__);

    if (Rval == OK) {
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP0, &MemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            dsp_osal_typecast(&pCmdBufCtrl, &MemBlk.Base);
            pCmdBufCtrl->MsgBufCtrl.UnReadMsgNum = 0U;
            pCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr = 0U;
            *(pCmdBufCtrl->MsgBufCtrl.pCmdReqRptr) = pCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr;
            pCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->read_ptr = pCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr;
            pCmdBufCtrl->MsgBufCtrl.DspMsgRp = pCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr;
            pCmdBufCtrl->MsgBufCtrl.DspMsgLastPtr = pCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr;
        }
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE, &MemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            dsp_osal_typecast(&pCmdBufCtrl, &MemBlk.Base);
            pCmdBufCtrl->MsgBufCtrl.UnReadMsgNum = 0U;
            *(pCmdBufCtrl->MsgBufCtrl.pCmdReqRptr) = pCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr;
            pCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->read_ptr = pCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr;
            pCmdBufCtrl->MsgBufCtrl.DspMsgRp = pCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr;
            pCmdBufCtrl->MsgBufCtrl.DspMsgLastPtr = pCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr;
        }
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_ASYNC_CMD_CTRL, 0U, &MemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            dsp_osal_typecast(&pAsyncCmdBufCtrl, &MemBlk.Base);
            pAsyncCmdBufCtrl->MsgBufCtrl.UnReadMsgNum = 0U;
            *(pAsyncCmdBufCtrl->MsgBufCtrl.pCmdReqRptr) = pAsyncCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr;
            pAsyncCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->read_ptr= pAsyncCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr;
            pAsyncCmdBufCtrl->MsgBufCtrl.DspMsgRp = pAsyncCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr;
            pAsyncCmdBufCtrl->MsgBufCtrl.DspMsgLastPtr = pAsyncCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr;
        }
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_ASYNC_ENC_MSG_BUF_CTRL, 0U, &MemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            dsp_osal_typecast(&pMsgBufCtrl, &MemBlk.Base);
            pMsgBufCtrl->UnReadMsgNum = 0U;
            *(pMsgBufCtrl->pCmdReqRptr) = pMsgBufCtrl->pMsgFifoCtrl->write_ptr;
            pMsgBufCtrl->pMsgFifoCtrl->read_ptr = pMsgBufCtrl->pMsgFifoCtrl->write_ptr;
            pMsgBufCtrl->DspMsgRp = pMsgBufCtrl->pMsgFifoCtrl->write_ptr;
            pMsgBufCtrl->DspMsgLastPtr = pMsgBufCtrl->pMsgFifoCtrl->write_ptr;
        }
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_VPROC_MSG_BUF_CTRL, 0U, &MemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            dsp_osal_typecast(&pMsgBufCtrl, &MemBlk.Base);
            pMsgBufCtrl->UnReadMsgNum = 0U;
            *(pMsgBufCtrl->pCmdReqRptr) = pMsgBufCtrl->pMsgFifoCtrl->write_ptr;
            pMsgBufCtrl->pMsgFifoCtrl->read_ptr = pMsgBufCtrl->pMsgFifoCtrl->write_ptr;
            pMsgBufCtrl->DspMsgRp = pMsgBufCtrl->pMsgFifoCtrl->write_ptr;
            pMsgBufCtrl->DspMsgLastPtr = pMsgBufCtrl->pMsgFifoCtrl->write_ptr;
        }
    }
}

void DSP_ResetCmdMsgBuf(void)
{
    AMBA_DSP_HEADER_CMD_s *pHeader = NULL;
    const AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pCmdBufCtrl;
    const AMBA_DSP_ASYNC_CMD_BUF_CTRL_s *pAsyncCmdBufCtrl;
    UINT16 StartIdx = (1U - (UINT16)LLSuspendStatus);
    UINT16 Index = 0U, i, j;
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 Rval;

    // Make sure the buffer is initialized before using AmbaDSP_SyncCmdBufCtrl
    if (ArmCommInit() == OK) {
        /*
         * Initialize Sync Command Buffers
         * Per ChenHan[2015/10/16], vdsp0 cmdQ will always be there no matter for which profile,
         * but vdsp1 is only for those profile with vin. So, we only need to reset vdsp1 CmdQ
         */
        for (i = StartIdx; i < AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL; i++) {
            Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, i, &MemBlk);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == DSP_ERR_NONE) {
                dsp_osal_typecast(&pCmdBufCtrl, &MemBlk.Base);

                dsp_osal_typecast(&pHeader, &pCmdBufCtrl->pCmdBufBaseAddr);
                *(pCmdBufCtrl->CurCmdSeqNo) = 0U;
                Index = 0U;
                for (j = 0U; j < pCmdBufCtrl->MaxNumCmdBuf; j++) {
                    pHeader[Index].Contents.num_cmds = 0U;
                    pHeader[Index].Contents.cmd_seq_num = 0U;
                    Index += pCmdBufCtrl->MaxNumCmds;
                }

                /* Initialize Group Command Buffers with CMD_DSP_HEADER */
                dsp_osal_typecast(&pHeader, &pCmdBufCtrl->pGroupCmdBufBaseAddr);
                pHeader->Contents.cmd_seq_num = 0U;
                pHeader->Contents.num_cmds = 0U;
            }
        }

        /* Async Cmd buf */
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_ASYNC_CMD_CTRL, 0U, &MemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == DSP_ERR_NONE) {
            dsp_osal_typecast(&pAsyncCmdBufCtrl, &MemBlk.Base);
            dsp_osal_typecast(&pHeader, &pAsyncCmdBufCtrl->pCmdBufBaseAddr);
            pHeader->Contents.cmd_seq_num = 0U;
            pHeader->Contents.num_cmds = 0U;
        }
    }
}

void DSP_ClrDefCfgBuffer(void)
{
    DSP_MEM_BLK_t MemBlk = {0};
    AMBA_DSP_DEFAULT_CMDS_s *pDefCmd;
    UINT32 Rval;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_DEF_CMD, 0U, &MemBlk);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == DSP_ERR_NONE) {
        dsp_osal_typecast(&pDefCmd, &MemBlk.Base);
        pDefCmd->Header.cmd_code = CMD_DSP_HEADER;
        pDefCmd->Header.pay_load[0] = 0U;
        pDefCmd->Header.pay_load[1] = 0U;
    }
}

UINT32 DSP_CheckDefaultCmds(void)
{
    return OK;
}

UINT32 ArmCommInit(void)
{
static char MtxName[] = "MtxMsg";
static char FlgName[] = "DSPLLEventFlg";
#if defined (CONFIG_THREADX)
static char CmdBufMtxName[] = "DspCmdBufMtx";
#endif
    UINT32 i, j;
    UINT32 Rval = OK;
    AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pSyncCmdBufCtrl = NULL;
    AMBA_DSP_ASYNC_CMD_BUF_CTRL_s *pAsyncCmdBufCtrl = NULL;
    AMBA_DSP_HEADER_CMD_s *pHeader = NULL;
    UINT32 Index = 0U;
    DSP_MEM_BLK_t MemBlk = {0};

    if (ArmCommInitFlag == 0U) {
        // Make sure the buffer is initialized before using AmbaDSP_SyncCmdBufCtrl
        AmbaDSP_InitBuffers();

        /*---------------------------------------------------------------------------*\
         * Initializations of sync command buffer control blocks
        \*---------------------------------------------------------------------------*/
        for (i = 0U; i < AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL; i++) {
            Index = 0U;
            /* Create Mutex */
            Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, i, &MemBlk);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == DSP_ERR_NONE) {
                dsp_osal_typecast(&pSyncCmdBufCtrl, &MemBlk.Base);

                /* Initialize Command Buffers with CMD_DSP_HEADER */
                dsp_osal_typecast(&pHeader, &pSyncCmdBufCtrl->pCmdBufBaseAddr);
                for (j = 0U; j < pSyncCmdBufCtrl->MaxNumCmdBuf; j++) {
                    pHeader[Index].Contents.cmd_code = CMD_DSP_HEADER;
                    pHeader[Index].Contents.cmd_seq_num = 0U;
                    Index += pSyncCmdBufCtrl->MaxNumCmds;
                }

                /* Initialize Group Command Buffers with CMD_DSP_HEADER */
                dsp_osal_typecast(&pHeader, &pSyncCmdBufCtrl->pGroupCmdBufBaseAddr);
                pHeader->Contents.cmd_code = CMD_DSP_HEADER;
                pHeader->Contents.cmd_seq_num = 0U;
                pHeader->Contents.num_cmds = 0U;
            }
        }

        /*---------------------------------------------------------------------------*\
         * Initializations of async command buffer control blocks
        \*---------------------------------------------------------------------------*/
        /* Create Mutex */
        if (Rval == OK) {
            Rval = DSP_GetProtBuf(DSP_PROT_BUF_ASYNC_CMD_CTRL, 0U, &MemBlk);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == DSP_ERR_NONE) {
                dsp_osal_typecast(&pAsyncCmdBufCtrl, &MemBlk.Base);

                if (dsp_osal_mutex_init(&DspAsyncCmdBufMtx, MtxName) != OK) {
                    AmbaLL_LogUInt5("[Err] ArmComm[%d] Create mutex failed", __LINE__, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0003;
                }

                /* Initialize Command Buffers with CMD_DSP_HEADER */
                dsp_osal_typecast(&pHeader, &pAsyncCmdBufCtrl->pCmdBufBaseAddr);
                pHeader->Contents.cmd_code = CMD_DSP_HEADER;
                pHeader->Contents.cmd_seq_num = 0U;
            }
        }

        /*------------------------------------------------------------------------------------------*\
         * Create LL_MsgEventFlag for LL_WaitDspMsgFlag function
        \*------------------------------------------------------------------------------------------*/
        if (Rval == OK) {
            //create dsp event flag
            if (dsp_osal_eventflag_init(&LL_MsgEventFlag, FlgName) != OK) {
                AmbaLL_LogUInt5("[Err] ArmComm[%d] Create LL_MsgEventFlag error", __LINE__, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }
        }

#if defined (CONFIG_THREADX)
        if (Rval == OK) {
            AMBA_VerInfo_s VerInfo;

            Rval = dsp_osal_memset(&VerInfo, 0, sizeof(AMBA_VerInfo_s));
            LL_PrintErrLine(Rval, __func__, __LINE__);
            AmbaVer_SetDspDspKernelLL(&VerInfo);
            if (VerInfo.DateNum == 0xDEADBEEFU) {
                AmbaLL_LogUInt5("[Err] ArmComm[%d] AmbaVer_SetDspDspKernelLL error", __LINE__, 0U, 0U, 0U, 0U);
            }

            Rval = dsp_osal_mutex_init(&DspCmdBufMtx, CmdBufMtxName);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[Err] ArmComm[%d] Create mutex failed", __LINE__, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }
        }
#endif

        if (Rval == OK) {
            ArmCommInitFlag = 1U;
        }
    }
    return Rval;
}

#if defined (CONFIG_THREADX)
static UINT8 ArmCommIsrTaskCreateFlag = 0U;
static osal_thread_t DspArmIsrTask GNU_SECTION_NOZEROINIT;

static UINT32 CreateArmCommIsrTask(void)
{
    UINT32 Rval = OK;
    static UINT8 DspArmIsrTaskStack[DSP_ARM_ISR_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char IsrTaskName[] = "DspArmIsrTask";
    void *pVoid;
    UINT8 *pU8 = &DspArmIsrTaskStack[0U];

    if (ArmCommIsrTaskCreateFlag == 0U) {
        Rval = dsp_osal_memset(pU8, 0, sizeof(UINT8)*DSP_ARM_ISR_TASK_STACK_SIZE);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        dsp_osal_typecast(&pVoid, &pU8);

        Rval = dsp_osal_thread_create(&DspArmIsrTask, IsrTaskName,
                                      DSP_ARM_ISR_TASK_PRIORITY, ArmCommIsrTaskEntry,
                                      NULL, pVoid,
                                      DSP_ARM_ISR_TASK_STACK_SIZE, 1U/*AutoStart*/);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            Rval = dsp_osal_thread_set_affinity(&DspArmIsrTask, DSP_ARM_ISR_TASK_CORE_SELECTION);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            ArmCommIsrTaskCreateFlag = 1U;
        }
    }

    return Rval;
}

UINT32 DeleteArmCommIsrTask(void)
{
    UINT32 Rval = OK;

    if (ArmCommIsrTaskCreateFlag == 1U) {
        Rval = dsp_osal_thread_should_stop(&DspArmIsrTask);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        Rval = dsp_osal_thread_delete(&DspArmIsrTask);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        if (Rval == OK) {
            ArmCommIsrTaskCreateFlag = 0U;
        }
    }

    return Rval;
}
#endif

/**
 * Create ARM-DSP ISR handler functions
 * Need to call ArmCommInit function before call this
 * Because handler function will using LL_MsgEventFlag
 * @return 0 - OK, others - ErrorCode
 */
static UINT32 ArmCommIsrFunCreate(UINT32 MonitorMask)
{
    UINT32 Rval = OK;
    osal_irq_t IntConfig;
    UINT32 IrqId = 0U;

AmbaMisra_TouchUnused(&MonitorMask);

    /*------------------------------------------------------------------------------------------*\
     * Setup VDSPx interrupt ISR handler function
    \*------------------------------------------------------------------------------------------*/
    /* VDSP0 : VoutCmdQ/Msg */
    IntConfig.trigger_type = IRQ_TRIG_RISE_EDGE;
    IntConfig.irq_type = IRQ_TYPE_IRQ;
    IntConfig.cpu_targets = INT_TARGET_CORE;
    Rval = dsp_osal_irq_id_query(DSP_IRQ_TYPE_CODE, 0U, &IrqId);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        Rval = dsp_osal_irq_config(IrqId, &IntConfig, DSP_Vdsp0ISRHandler, 0);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }
    if (Rval == OK) {
        Rval = dsp_osal_irq_enable(IrqId);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }

    /* VDSP1 : VinCmdQ/Msg */
    if (Rval == OK) {
        IntConfig.trigger_type = IRQ_TRIG_RISE_EDGE;
        IntConfig.irq_type = IRQ_TYPE_IRQ;
        IntConfig.cpu_targets = INT_TARGET_CORE;
        Rval = dsp_osal_irq_id_query(DSP_IRQ_TYPE_CODE, 1U, &IrqId);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }
    if (Rval == OK) {
        Rval = dsp_osal_irq_config(IrqId, &IntConfig, DSP_Vdsp1ISRHandler, 0);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }
    if (Rval == OK) {
        Rval = dsp_osal_irq_enable(IrqId);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }

    /* VDSP2 : AsyncCmdQ */
    if (Rval == OK) {
        IntConfig.trigger_type = IRQ_TRIG_RISE_EDGE;
        IntConfig.irq_type = IRQ_TYPE_IRQ;
        IntConfig.cpu_targets = INT_TARGET_CORE;
        Rval = dsp_osal_irq_id_query(DSP_IRQ_TYPE_CODE, 2U, &IrqId);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }
    if (Rval == OK) {
        Rval = dsp_osal_irq_config(IrqId, &IntConfig, DSP_Vdsp2ISRHandler, 0);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }
    if (Rval == OK) {
        Rval = dsp_osal_irq_enable(IrqId);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }

    /* VDSP3 : VprocMsgQ */
    if (Rval == OK) {
        IntConfig.trigger_type = IRQ_TRIG_RISE_EDGE;
        IntConfig.irq_type = IRQ_TYPE_IRQ;
        IntConfig.cpu_targets = INT_TARGET_CORE;
        Rval = dsp_osal_irq_id_query(DSP_IRQ_TYPE_CODE, 3U, &IrqId);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }
    if (Rval == OK) {
        Rval = dsp_osal_irq_config(IrqId, &IntConfig, DSP_Vdsp3ISRHandler, 0);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }
    if (Rval == OK) {
        Rval = dsp_osal_irq_enable(IrqId);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }

    /* VDSP4 : Enc/Dec MsgQ */
    if (Rval == OK) {
        IntConfig.trigger_type = IRQ_TRIG_RISE_EDGE;
        IntConfig.irq_type = IRQ_TYPE_IRQ;
        IntConfig.cpu_targets = INT_TARGET_CORE;
        Rval = dsp_osal_irq_id_query(DSP_IRQ_TYPE_CODE, 4U, &IrqId);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }
    if (Rval == OK) {
        Rval = dsp_osal_irq_config(IrqId, &IntConfig, DSP_Vdsp4ISRHandler, 0);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }
    if (Rval == OK) {
        Rval = dsp_osal_irq_enable(IrqId);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }


#if defined (CONFIG_THREADX)
    if (Rval == OK) {
        Rval = CreateArmCommIsrTask();
    }
#endif
    return Rval;
}

#if defined (CONFIG_THREADX)
static void *ArmCommIsrTaskEntry(void *EntryArg)
{
    UINT8 DoWhile = 1U;
    UINT32 ActualFlags, Flag = 0U, Rval;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaLL_LogUInt5("ArmCommIsrTaskEntry created", 0U, 0U, 0U, 0U, 0U);

    Flag = (UINT32)EVENT_FLAG_INT_VDSP0_INT|
           (UINT32)EVENT_FLAG_INT_VDSP1_INT|
           (UINT32)EVENT_FLAG_INT_VDSP2_INT|
           (UINT32)EVENT_FLAG_INT_VDSP3_INT|
           (UINT32)EVENT_FLAG_INT_VDSP4_INT;

    while (DoWhile == 1U) {
        /* wait for interrupts from DSP/uCode */
        Rval = DSP_WaitEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, Flag, 0U, 1U, &ActualFlags, WAIT_ISR_TIMEOUT);
        if ((Rval == DSP_ERR_TIMEOUT) && (LLSuspendStatus == 1U)) {
            // do nothing;
        } else if (Rval == DSP_ERR_TIMEOUT) {
            AmbaLL_LogUInt5("[Err @ ArmComm][%d] ArmCommIsrTask WaitEventFlag failed", __LINE__, 0U, 0U, 0U, 0U);
        } else if (Rval != OK) {
            AmbaLL_LogUInt5("[Err @ ArmComm][%d] ArmCommIsrTask IsrEventFlag initial failed, stop the task", __LINE__, 0U, 0U, 0U, 0U);
            DoWhile = 0U;
        } else {
            if ((ActualFlags & (UINT32)EVENT_FLAG_INT_VDSP0_INT) > 0U) {
                DSP_HandleVdsp0ISR();
            }
            if ((ActualFlags & (UINT32)EVENT_FLAG_INT_VDSP1_INT) > 0U) {
                DSP_HandleVdsp1ISR();
            }
            if ((ActualFlags & (UINT32)EVENT_FLAG_INT_VDSP2_INT) > 0U) {
                DSP_HandleVdsp2ISR();
            }
            if ((ActualFlags & (UINT32)EVENT_FLAG_INT_VDSP3_INT) > 0U) {
                DSP_HandleVdsp3ISR();
            }
            if ((ActualFlags & (UINT32)EVENT_FLAG_INT_VDSP4_INT) > 0U) {
                DSP_HandleVdsp4ISR();
            }
        }
    }
    return NULL;
}
#endif

static UINT32 ReplaceVoutDefaultCmdBuf(const AMBA_DSP_HEADER_CMD_s *pHeader, const void *pCmdData)
{
    const dsp_cmd_t * Cmd;
    UINT32 Rval = DSP_ERR_0000;
    UINT32 CmdCat;
    UINT32 NumCmds;
    UINT32 i;
    const AMBA_DSP_HEADER_CMD_s *pTmp;
    const cmd_vout_hdr_t* SrcCmd = NULL;
    cmd_vout_hdr_t* DstCmd = NULL;

    dsp_osal_typecast(&Cmd, &pCmdData);
    GET_DSP_CMD_CAT(Cmd->cmd_code, &CmdCat);

    if ((pHeader == NULL) || (pCmdData == NULL) || (CmdCat != CAT_VOUT)) {
        Rval = DSP_ERR_0000;
    } else {
        NumCmds = pHeader->Contents.num_cmds;
        Rval = DSP_ERR_0000;
        switch (Cmd->cmd_code) {
        case CMD_VOUT_MIXER_SETUP:
        case CMD_VOUT_VIDEO_SETUP:
        case CMD_VOUT_DEFAULT_IMG_SETUP:
        case CMD_VOUT_DISPLAY_SETUP:
        case CMD_VOUT_DVE_SETUP:
        case CMD_VOUT_RESET:
        case CMD_VOUT_DISPLAY_CSC_SETUP:
        case CMD_VOUT_DIGITAL_OUTPUT_MODE_SETUP:
        case CMD_VOUT_GAMMA_SETUP:
        {
            NumCmds = pHeader->Contents.num_cmds;
            dsp_osal_typecast(&SrcCmd, &pCmdData);
            for (i = 0U; i < NumCmds; i++) {
                pTmp = &pHeader[i + 1U];
                dsp_osal_typecast(&DstCmd, &pTmp);
                if ((DstCmd->cmd_code == SrcCmd->cmd_code) && (DstCmd->vout_id == SrcCmd->vout_id)) {
                    Rval = dsp_osal_memcpy(DstCmd, SrcCmd, sizeof(dsp_cmd_t));
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    Rval = OK;
                    break;
                }
            }
        }
            break;
        default:
            //
            break;
        }
    }
    return Rval;
}

UINT32 WriteCmdBuf(AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pCmdBufCtrl, const void *pCmdData, UINT32 CmdDataSize)
{
    UINT32 Rval = OK;
    UINT32 NumCmds = 0, MaxNum = 0;
    osal_spinlock_t SpinLock;
    AMBA_DSP_HEADER_CMD_s *pHeader;
    dsp_cmd_t *pDspCmd;
    const cmd_vin_send_input_data_t *pSendInputData;
    UINT32 LockCmdVinId = (AMBA_DSP_MAX_VIN_NUM + AMBA_DSP_MAX_VIRT_VIN_NUM);

    if (CmdDataSize > DSP_CMD_SIZE) {
        AmbaLL_LogUInt5("[Err @ ArmComm][%d] CmdDataSize[%d] exceeds pre-defined %d bytes", __LINE__, CmdDataSize, DSP_CMD_SIZE, 0U, 0U);
        Rval = DSP_ERR_0001;
    } else {
        /* For lock cmd buffer */
        dsp_osal_typecast(&pSendInputData, &pCmdData);
        if (pSendInputData->cmd_code == CMD_VIN_SEND_INPUT_DATA) {
            LockCmdVinId = pSendInputData->vin_id;
        }

        /* prevent entering either IRQ or FIQ handler */
        Rval = dsp_osal_cmdlock(&DspCmdBufMtx, &SpinLock);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        if (Rval == OK) {
            pDspCmd = &(pCmdBufCtrl->pCmdBufBaseAddr[pCmdBufCtrl->WritePtr * pCmdBufCtrl->MaxNumCmds]);
            dsp_osal_typecast(&pHeader, &pDspCmd);

            MaxNum = pCmdBufCtrl->MaxNumCmds;

            NumCmds = pHeader->Contents.num_cmds + 1U;
            if (NumCmds >= MaxNum) {
                UINT8 TempWritePtr = pCmdBufCtrl->WritePtr;

                AmbaLL_LogUInt5("This command block is FULL", 0U, 0U, 0U, 0U, 0U);
                /* CmdBuf full, let WritePtr forward */
                TempWritePtr++;
                if (TempWritePtr >= pCmdBufCtrl->MaxNumCmdBuf) {
                    TempWritePtr = 0U;
                }

                if (TempWritePtr == pCmdBufCtrl->ReadPtr) {
                    AmbaLL_LogUInt5("No more free command block", 0U, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0007;
                }
                if (Rval == OK) {
                    pCmdBufCtrl->WritePtr = TempWritePtr;
                    pDspCmd = &(pCmdBufCtrl->pCmdBufBaseAddr[pCmdBufCtrl->WritePtr * pCmdBufCtrl->MaxNumCmds]);
                    dsp_osal_typecast(&pHeader, &pDspCmd);
                    NumCmds = 1U;
                }
            }
            if (Rval == OK) {
                /* first of all, clear the command area */
                Rval = dsp_osal_memset((&pDspCmd[NumCmds]), 0, sizeof(dsp_cmd_t));
                LL_PrintErrLine(Rval, __func__, __LINE__);
                /* store command data into the buffer */
                pHeader->Contents.num_cmds = NumCmds;
                Rval = dsp_osal_memcpy(&pHeader[NumCmds], pCmdData, CmdDataSize);
                LL_PrintErrLine(Rval, __func__, __LINE__);
#ifdef ENABLE_CMD_MSG_CRC
                UpdateCmdBufCheckSum(pHeader, NumCmds, 1U);
#endif
                if (LockCmdVinId != (AMBA_DSP_MAX_VIN_NUM + AMBA_DSP_MAX_VIRT_VIN_NUM)) {
                    DSP_SetBit(&(pCmdBufCtrl->CmdLock), LockCmdVinId);
                }
            }

            Rval |= dsp_osal_cmdunlock(&DspCmdBufMtx, &SpinLock);
            LL_PrintErrLine(Rval, __func__, __LINE__);
        }
    }
    return Rval;
}

static inline UINT32 WriteDefCmd_HandleFullBuf(UINT8 SysState, const void *pCmdData, UINT32 CmdDataSize,
                                               const AMBA_DSP_HEADER_CMD_s *pHeader,
                                               DSP_MEM_BLK_t* pMemBlk)
{
    const dsp_cmd_t *Cmd;
    UINT32 CmdCat;
    const UINT32 *CmdCode;
    UINT32 Rval = OK;
    AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pSyncCmdBufCtrl;

    if (CmdDataSize > DSP_CMD_SIZE) {
        AmbaLL_LogUInt5("[Err @ ArmComm][%d] CmdDataSize[%d] exceeds pre-defined %d bytes", __LINE__, CmdDataSize, DSP_CMD_SIZE, 0U, 0U);
        Rval = DSP_ERR_0001;
    }

    if (Rval == OK) {
        if (SysState == AMBA_DSP_SYS_STATE_LIVEVIEW) {
            dsp_osal_typecast(&Cmd, &pCmdData);

            GET_DSP_CMD_CAT(Cmd->cmd_code, &CmdCat);
            if (CmdCat == CAT_VIN) {
                AmbaLL_LogUInt5("[%d]Write to Vin0 gen", __LINE__, 0U, 0U, 0U, 0U);
                Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE, pMemBlk);
                LL_PrintErrLine(Rval, __func__, __LINE__);
            } else {
                AmbaLL_LogUInt5("[%d]Write to Vout gen", __LINE__, 0U, 0U, 0U, 0U);
                Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP0, pMemBlk);
                LL_PrintErrLine(Rval, __func__, __LINE__);
            }
            if (Rval == OK) {
                dsp_osal_typecast(&pSyncCmdBufCtrl, &pMemBlk->Base);
                Rval = WriteCmdBuf(pSyncCmdBufCtrl, pCmdData, CmdDataSize);
            }
        } else if (SysState == AMBA_DSP_SYS_STATE_NUM) {
            dsp_osal_typecast(&Cmd, &pCmdData);
            /* We may have chance to replace existed vout cmd */
            if (OK != ReplaceVoutDefaultCmdBuf(pHeader, pCmdData)) {
                dsp_osal_typecast(&CmdCode, &pCmdData);
                AmbaLL_LogUInt5("[%d] Too many default command, Write[0x%X] to Vdsp gen", __LINE__, *CmdCode, 0U, 0U, 0U);
                Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP0, pMemBlk);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                if (Rval == OK) {
                    dsp_osal_typecast(&pSyncCmdBufCtrl, &pMemBlk->Base);
                    Rval = WriteCmdBuf(pSyncCmdBufCtrl, pCmdData, CmdDataSize);
                }
            }
        } else {
            Rval = DSP_ERR_0003;
        }
    }

    return Rval;
}

UINT32 WriteDefaultCmdBuf(UINT8 SysState, const void *pCmdData, UINT32 CmdDataSize)
{
    UINT32 Rval = OK;
    DSP_MEM_BLK_t MemBlk = {0};
    AMBA_DSP_HEADER_CMD_s *pHeader = NULL;
    UINT32 NumCmds;

    /* Make sure the buffer is initialized before using AmbaDSP_SyncCmdBufCtrl */
    if (ArmCommInitFlag != 1U) {
        Rval = ArmCommInit();
    }

    if (Rval == OK) {
        switch(SysState) {
        case AMBA_DSP_SYS_STATE_LIVEVIEW:
        case AMBA_DSP_SYS_STATE_SENSORLESS:
        case AMBA_DSP_SYS_STATE_NUM:
            Rval = DSP_GetProtBuf(DSP_PROT_BUF_DEF_CMD, 0U, &MemBlk);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == OK) {
                dsp_osal_typecast(&pHeader, &MemBlk.Base);
            }
            break;
        default:
            AmbaLL_LogUInt5("[Err @ ArmComm][%d] SysState[%d] is incorrect", __LINE__, SysState, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
            break;
        }
    }

    if ((Rval == OK) && (pHeader != NULL)) {
        NumCmds = pHeader->Contents.num_cmds + 1U;
        if (NumCmds == DSP_MAX_NUM_DEFAULT_CMD) {
            Rval = WriteDefCmd_HandleFullBuf(SysState, pCmdData, CmdDataSize, pHeader, &MemBlk);
            AmbaLL_Log(AMBALLLOG_TYPE_CMD_DEF, "DefCmdBuf full", 0U, 0U, 0U);
        } else {
            const UINT32 *pPrint;

            /* first of all, to clear the command area */
            Rval = dsp_osal_memset(&pHeader[NumCmds], 0, sizeof(dsp_cmd_t));
            LL_PrintErrLine(Rval, __func__, __LINE__);
            /* save command data into the buffer */
            pHeader->Contents.num_cmds = NumCmds;
            Rval = dsp_osal_memcpy(&pHeader[NumCmds], pCmdData, CmdDataSize);
            LL_PrintErrLine(Rval, __func__, __LINE__);

            dsp_osal_typecast(&pPrint, &pCmdData);
            AmbaLL_Log(AMBALLLOG_TYPE_CMD_DEF, "Write Def 0x%X", *pPrint, 0U, 0U);
#ifdef ENABLE_CMD_MSG_CRC
            UpdateCmdBufCheckSum(pHeader, NumCmds, 1U);
            AmbaLL_Log(AMBALLLOG_TYPE_CMD_DEF, "    UpdateCRC 0x%x", pHeader->Contents.cmd_block_crc, 0U, 0U);
#endif
        }
    }

    return Rval;
}

UINT32 WriteAsyncCmdBuf(const void *pCmdData, UINT32 CmdDataSize)
{
#define ASYNC_CMD_FLAG_TIMEOUT  (500U)
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 NumCmds = 0, MaxNum = 0;
    AMBA_DSP_HEADER_CMD_s *pHeader;
    dsp_cmd_t *pDspCmd;
    DSP_MEM_BLK_t MemBlk = {0};
    AMBA_DSP_ASYNC_CMD_BUF_CTRL_s *pAsyncCmdBufCtrl;
    UINT32 i;
    UINT32 Flag = (UINT32)EVENT_FLAG_INT_CMD_ACK_ASYNC;
    UINT32 ActualFlags;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_ASYNC_CMD_CTRL, 0U, &MemBlk);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        dsp_osal_typecast(&pAsyncCmdBufCtrl, &MemBlk.Base);
    }

    if (CmdDataSize > DSP_CMD_SIZE) {
        AmbaLL_LogUInt5("[Err] ArmComm[%d] CmdDataSize[%d] exceeds pre-defined %d bytes", __LINE__, CmdDataSize, DSP_CMD_SIZE, 0U, 0U);
        Rval = DSP_ERR_0001;
    } else {
        Rval = dsp_osal_mutex_lock(&DspAsyncCmdBufMtx, 500U);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        if (Rval == DSP_ERR_NONE) {
            pDspCmd = pAsyncCmdBufCtrl->pCmdBufBaseAddr;
            dsp_osal_typecast(&pHeader, &pDspCmd);

            MaxNum = pAsyncCmdBufCtrl->MaxNumCmds;

            NumCmds = pHeader->Contents.num_cmds + 1U;
            if (NumCmds >= MaxNum) {
                AmbaLL_LogUInt5("[Err] ArmComm[%d] AsyncCmdFull", __LINE__, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            } else {
                /* first of all, clear the command area */
                Rval = dsp_osal_memset((&pDspCmd[NumCmds]), 0, sizeof(dsp_cmd_t));
                LL_PrintErrLine(Rval, __func__, __LINE__);

                /* store command data into the buffer */
                pHeader->Contents.num_cmds = NumCmds;
                Rval = dsp_osal_memcpy(&pHeader[NumCmds], pCmdData, CmdDataSize);
                LL_PrintErrLine(Rval, __func__, __LINE__);
#ifdef ENABLE_CMD_MSG_CRC
                UpdateCmdBufCheckSum(pHeader, NumCmds, 1U);
#endif
            }
        }

        /* Send Cmd */
        if (Rval == DSP_ERR_NONE) {
            if (pHeader->Contents.num_cmds > 0U) {
                pHeader->Contents.cmd_seq_num++;

                AmbaLL_Log(AMBALLLOG_TYPE_CMD_DEF, "Async, cmd_seq_num: %d, num_cmds: %d", pHeader->Contents.cmd_seq_num, pHeader->Contents.num_cmds, 0U);
                for (i = 0; i < pHeader->Contents.num_cmds; i++) {
                    AmbaLL_Log(AMBALLLOG_TYPE_CMD_DEF, "  cmd 0x%X", pHeader[i+1U].Contents.cmd_code, 0U, 0U);
                }

                /* Clear event flag */
                Rval = DSP_ClearEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, Flag);
                if (Rval == DSP_ERR_NONE) {
                    /* Send SyncCnt */
                    dsp_osal_dbgport_set_sync_cnt(pAsyncCmdBufCtrl->SyncCounter, 1U);

                    /* Wait for the cmd ack */
                    Rval = DSP_WaitEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, Flag, 0U/*All*/, 1U/*Clear*/, &ActualFlags, ASYNC_CMD_FLAG_TIMEOUT);
                    if (Rval != DSP_ERR_NONE) {
                        AmbaLL_LogUInt5("[Err] ArmComm[%d] WaitFlg[0x%X] timeout", __LINE__, 0U, 0U, 0U, 0U);
                    } else {
                        pHeader->Contents.num_cmds = 0U;
                    }
                }
            }
        }

        Rval = dsp_osal_mutex_unlock(&DspAsyncCmdBufMtx);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }

    return Rval;
}

UINT32 WriteGroupCmdBuf(const AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pCmdBufCtrl, const void *pCmdData, UINT32 CmdDataSize)
{
    UINT32 Rval = OK;
    const dsp_cmd_t *pDspCmd = pCmdBufCtrl->pGroupCmdBufBaseAddr;
    AMBA_DSP_HEADER_CMD_s *pHeader;
    UINT32 NumCmds;

    if (CmdDataSize > DSP_CMD_SIZE) {
        AmbaLL_LogUInt5("[Err @ ArmComm][%d] CmdDataSize[%d] exceeds pre-defined %d bytes", __LINE__, CmdDataSize, DSP_CMD_SIZE, 0U, 0U);
        Rval = DSP_ERR_0001;
    } else {

        dsp_osal_typecast(&pHeader, &pDspCmd);
        NumCmds = pHeader->Contents.num_cmds + 1U;
        if (NumCmds >= pCmdBufCtrl->MaxNumCmds) {
            AmbaLL_LogUInt5("[Err @ ArmComm][%d] Group command buffer is full", __LINE__, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0002;
        } else {
            /* first of all, clear the command area */
            Rval = dsp_osal_memset(&pHeader[NumCmds], 0, sizeof(dsp_cmd_t));
            LL_PrintErrLine(Rval, __func__, __LINE__);

            if (Rval == OK) {
                /* store command data into the buffer */
                pHeader->Contents.num_cmds = NumCmds;
                Rval = dsp_osal_memcpy(&pHeader[NumCmds], pCmdData, CmdDataSize);
                LL_PrintErrLine(Rval, __func__, __LINE__);
            }
        }
    }
    return Rval;
}

static UINT32 SendGroupCmdBuf(AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pCmdBufCtrl)
{
    UINT32 Rval = OK;
    const dsp_cmd_t *pDspCmd = pCmdBufCtrl->pGroupCmdBufBaseAddr;
    AMBA_DSP_HEADER_CMD_s *pHeader;
    const dsp_cmd_t *pCmdBufBaseAddr = pCmdBufCtrl->pGroupCmdBufBaseAddr;
    UINT32 GroupBufNumCmds;
    osal_spinlock_t SpinLock;

    dsp_osal_typecast(&pHeader, &pDspCmd);
    Rval = dsp_osal_cmdlock(&DspCmdBufMtx, &SpinLock);
    LL_PrintErrLine(Rval, __func__, __LINE__);

    if (Rval == OK) {
        /*-----------------------------------------------------------------------*\
         *  Move commands of local buffer into the Tx command buffer
        \*-----------------------------------------------------------------------*/
        GroupBufNumCmds = pHeader->Contents.num_cmds;
        if (GroupBufNumCmds > 0U) {
            UINT32 TxBufNumCmds;
            pHeader->Contents.num_cmds = 0U;  /* reset the number of commands */

            pDspCmd = &(pCmdBufCtrl->pCmdBufBaseAddr[pCmdBufCtrl->WritePtr * pCmdBufCtrl->MaxNumCmds]);
            dsp_osal_typecast(&pHeader, &pDspCmd);

            TxBufNumCmds = pHeader->Contents.num_cmds + 1U;  // +1 to include the command header

            if ((pCmdBufCtrl->MaxNumCmds - TxBufNumCmds) >= GroupBufNumCmds) {
                 Rval = dsp_osal_memcpy(&pHeader[TxBufNumCmds], &pCmdBufBaseAddr[1], GroupBufNumCmds * sizeof(dsp_cmd_t));
                 LL_PrintErrLine(Rval, __func__, __LINE__);
                 pHeader->Contents.num_cmds += GroupBufNumCmds;
#ifdef ENABLE_CMD_MSG_CRC
                 UpdateCmdBufCheckSum(pHeader, TxBufNumCmds, GroupBufNumCmds);
#endif
            } else {
                UINT8 TempWritePtr = pCmdBufCtrl->WritePtr;

                AmbaLL_LogUInt5("This command block is not enough [NumCmd:%u NumGrpCmd:%u]", TxBufNumCmds, GroupBufNumCmds, 0U, 0U, 0U);
                /* CmdBuf full, let WritePtr forward */
                TempWritePtr++;
                if (TempWritePtr >= pCmdBufCtrl->MaxNumCmdBuf) {
                    TempWritePtr = 0U;
                }
                if (TempWritePtr == pCmdBufCtrl->ReadPtr) {
                    AmbaLL_LogUInt5("No more free command block", 0U, 0U, 0U, 0U, 0U);
                    Rval = DSP_ERR_0007;
                }
                if (Rval == OK) {
                    pCmdBufCtrl->WritePtr = TempWritePtr;
                    pDspCmd = &(pCmdBufCtrl->pCmdBufBaseAddr[pCmdBufCtrl->WritePtr * pCmdBufCtrl->MaxNumCmds]);
                    dsp_osal_typecast(&pHeader, &pDspCmd);
                    Rval = dsp_osal_memcpy(&pHeader[1], &pCmdBufBaseAddr[1], GroupBufNumCmds * sizeof(dsp_cmd_t));
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pHeader->Contents.num_cmds = GroupBufNumCmds;
#ifdef ENABLE_CMD_MSG_CRC
                    UpdateCmdBufCheckSum(pHeader, 1U, GroupBufNumCmds);
#endif
                }
            }
        }

        Rval = dsp_osal_cmdunlock(&DspCmdBufMtx, &SpinLock);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }


    return Rval;
}

UINT32 DSP_Bootup(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig,
                  ULONG *pChipInfoAddr,
                  UINT32 MonitorMask,
                  UINT8 ParLoadEnable,
                  UINT8 IdspBinShare)
{
    static UINT32 DspBooted = 0U;
    UINT32 Rval = OK;

    DSP_ResetCmdMsgBuf();

    if (DspBooted == 0U) {
        if(ArmCommInit() != OK) {
            AmbaLL_LogUInt5("[Err] Initial ArmComm failed", 0U, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        }

        /* Create ARM-DSP ISR handler functions */
        if (ArmCommIsrFunCreate(MonitorMask) != OK) {
            AmbaLL_LogUInt5("[Err] Create ARM-DSP ISR handler functions failed", 0U, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        }
    }

#if defined (CONFIG_THREADX)
    if ((Rval == OK) && (LLSuspendStatus == 1U)) {
        Rval = CreateArmCommIsrTask();
    }
#endif

    if (Rval == OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_INIT, "Setup DSP_INIT_DATA", 0U, 0U, 0U);
        if (OK != SetupDSPInitData(pDspSysConfig, pChipInfoAddr, ParLoadEnable)) {
            AmbaLL_LogUInt5("[Err] DSP_INIT_DATA is NG to boot DSP. Aborted", 0U, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        }
    }
    if (Rval == OK) {
        osal_ucode_addr_t BaseAddr = {0};
        osal_ucode_addr_t BasePhysAddr = {0};
        UINT32 PhysAddr;
        UINT32 OrcMask = 0U;
        DSP_MEM_BLK_t InitDataMemBlk = {0};
        AMBA_DSP_INIT_DATA_s *pInitData = NULL;

        Rval = dsp_osal_get_ucode_base_addr(&BaseAddr);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            Rval = dsp_osal_virt2cli(BaseAddr.CodeAddr, &PhysAddr);
            BasePhysAddr.CodeAddr = PhysAddr;
            LL_PrintErrLine(Rval, __func__, __LINE__);
            Rval = dsp_osal_virt2cli(BaseAddr.Code1Addr, &PhysAddr);
            BasePhysAddr.Code1Addr = PhysAddr;
            LL_PrintErrLine(Rval, __func__, __LINE__);
            Rval = dsp_osal_virt2cli(BaseAddr.Code2Addr, &PhysAddr);
            BasePhysAddr.Code2Addr = PhysAddr;
            LL_PrintErrLine(Rval, __func__, __LINE__);
            Rval = dsp_osal_virt2cli(BaseAddr.Code3Addr, &PhysAddr);
            BasePhysAddr.Code3Addr = PhysAddr;
            LL_PrintErrLine(Rval, __func__, __LINE__);

            Rval = dsp_osal_virt2cli(BaseAddr.MeAddr, &PhysAddr);
            BasePhysAddr.MeAddr = PhysAddr;
            LL_PrintErrLine(Rval, __func__, __LINE__);
            Rval = dsp_osal_virt2cli(BaseAddr.Me1Addr, &PhysAddr);
            BasePhysAddr.Me1Addr = PhysAddr;
            LL_PrintErrLine(Rval, __func__, __LINE__);

            Rval = dsp_osal_virt2cli(BaseAddr.MdxfAddr, &PhysAddr);
            BasePhysAddr.MdxfAddr = PhysAddr;
            LL_PrintErrLine(Rval, __func__, __LINE__);
            Rval = dsp_osal_virt2cli(BaseAddr.Mdxf1Addr, &PhysAddr);
            BasePhysAddr.Mdxf1Addr = PhysAddr;
            LL_PrintErrLine(Rval, __func__, __LINE__);

            AmbaLL_Log(AMBALLLOG_TYPE_INIT, "Set DSP PC Registers to 0x%X, 0x%X, 0x%X",
                                            (UINT32)BasePhysAddr.CodeAddr,
                                            (UINT32)BasePhysAddr.Code1Addr,
                                            (UINT32)BasePhysAddr.Code2Addr);
            AmbaLL_Log(AMBALLLOG_TYPE_INIT, "                        0x%X, 0x%X, 0x%X",
                                            (UINT32)BasePhysAddr.Code3Addr,
                                            (UINT32)BasePhysAddr.MeAddr,
                                            (UINT32)BasePhysAddr.Me1Addr);
            AmbaLL_Log(AMBALLLOG_TYPE_INIT, "                        0x%X, 0x%X, 0x%X",
                                            (UINT32)BasePhysAddr.MdxfAddr,
                                            (UINT32)BasePhysAddr.Mdxf1Addr, 0U);

            AmbaLL_Log(AMBALLLOG_TYPE_INIT, "Enable all DSPs", 0U, 0U, 0U);
            dsp_osal_orc_set_pc(&BasePhysAddr, IdspBinShare);
            (void)DSP_GetProtBuf(DSP_PROT_BUF_INIT_DATA, 0U, &InitDataMemBlk);
            dsp_osal_typecast(&pInitData, &InitDataMemBlk.Base);
            if ((ParLoadEnable == 1U) &&
                (pInitData->Data.is_orccode_fully_load == 0U)) {
                DSP_SetBit(&OrcMask, DSP_ORC_CODE_IDX);
                DSP_SetBit(&OrcMask, DSP_ORC_ME0_IDX);
                DSP_SetBit(&OrcMask, DSP_ORC_ME1_IDX);
                DSP_SetBit(&OrcMask, DSP_ORC_MDXF0_IDX);
                DSP_SetBit(&OrcMask, DSP_ORC_MDXF1_IDX);
                dsp_osal_orc_enable(OrcMask);
            } else {
                dsp_osal_orc_enable(OrcMask);
            }
            DSP_Suspend(0U);

            DspBooted = 1U;
        }
    }
    return Rval;
}

UINT32 DSP_UpdateWorkBuffer(ULONG WorkAreaAddr, UINT32 WorkAreaSize)
{
    UINT32 Rval = OK;
    DSP_MEM_BLK_t MemBlk = {0};
    AMBA_DSP_INIT_DATA_s *pInitData = NULL;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_INIT_DATA, 0U, &MemBlk);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        dsp_osal_typecast(&pInitData, &MemBlk.Base);

        if ((WorkAreaAddr > 0U) && (WorkAreaSize > 0U)) {
            Rval = dsp_osal_virt2cli(WorkAreaAddr, &pInitData->Data.dsp_dram_daddr);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            pInitData->Data.dsp_dram_size = WorkAreaSize;
        } else {
            AmbaLL_LogUInt5("DSP_UpdateWorkBuffer %d 0x%X%X",
                WorkAreaSize, DSP_GetU64Msb((UINT64)WorkAreaAddr), DSP_GetU64Lsb((UINT64)WorkAreaAddr), 0U, 0U);
            Rval = DSP_ERR_0001;
        }
    }

    return Rval;
}

void DSP_DumpDspInitDataInfo(void)
{
    UINT32 Rval;
    const dsp_init_data_t *pDSPInitData;
    osal_ucode_addr_t BaseAddr = {0};
    UINT16 Idx;

    Rval = dsp_osal_get_ucode_base_addr(&BaseAddr);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        BaseAddr.Code1Addr += DSP_INIT_DATA_OFFSET;
        dsp_osal_typecast(&pDSPInitData, &BaseAddr.Code1Addr);

        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG,"=== DSP_INIT_DATA ===", 0U, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," default_binary_data_daddr     = 0x%X", pDSPInitData->default_binary_data_daddr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," default_binary_data_dsize     = 0x%X", pDSPInitData->default_binary_data_dsize, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," default_config_daddr          = 0x%X", pDSPInitData->default_config_daddr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," default_config_dsize          = 0x%X", pDSPInitData->default_config_dsize, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," dsp_dram_daddr                = 0x%X", pDSPInitData->dsp_dram_daddr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," dsp_dram_size                 = 0x%X", pDSPInitData->dsp_dram_size, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," idsp_ext_dram_daddr           = 0x%X", pDSPInitData->idsp_ext_dram_daddr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," idsp_ext_dram_dsize           = 0x%X", pDSPInitData->idsp_ext_dram_dsize, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," dsp_debug_daddr               = 0x%X", pDSPInitData->dsp_debug_daddr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," dsp_debug_dsize               = 0x%X", pDSPInitData->dsp_debug_dsize, 0U, 0U);

        for (Idx = 0U; Idx < MAX_NUM_DSP_CMD_Q; Idx++) {
            AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," cmd_data_daddr[%d]             = 0x%X", Idx, pDSPInitData->cmd_data_daddr[Idx], 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," cmd_data_dsize[%d]             = 0x%X", Idx, pDSPInitData->cmd_data_dsize[Idx], 0U);
        }

        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," cmd_req_timer                 = %d  ", pDSPInitData->cmd_req_timer, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," msg_q_full_assert             = %d  ", pDSPInitData->msg_q_full_assert, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," req_cmd_timeout_assert        = %d  ", pDSPInitData->req_cmd_timeout_assert, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," run_with_vp                   = %d  ", pDSPInitData->run_with_vp, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," enable_vout_ln_sync_monitor   = %d  ", pDSPInitData->enable_vout_ln_sync_monitor, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," is_orccode_fully_load         = %d  ", pDSPInitData->is_orccode_fully_load, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," data_prot_safety_enabled      = %d  ", pDSPInitData->data_prot_safety_enabled, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," orc_printf_ver                = %d  ", pDSPInitData->orc_printf_ver, 0U, 0U);

        for (Idx = 0U; Idx < MAX_NUM_DSP_MSG_Q; Idx++) {
            AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," msg_q_info_data_daddr[%d]      = 0x%X", Idx, pDSPInitData->msg_q_info_data_daddr[Idx], 0U);
        }
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," chip_id_daddr                 = 0x%X", pDSPInitData->chip_id_daddr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," orc_assert_daddr              = 0x%X", pDSPInitData->orc_assert_daddr, 0U, 0U);

        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," is_prt_opt                    = %d  ", pDSPInitData->is_prt_opt, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," delta_audio_ticks             = %d  ", pDSPInitData->delta_audio_ticks, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," att_prot_dram_daddr           = 0x%X", pDSPInitData->att_prot_dram_daddr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," att_prot_dram_dsize           = %d  ", pDSPInitData->att_prot_dram_dsize, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," dram_exclusion_region_num     = %d  ", pDSPInitData->dram_exclusion_region_num, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," dram_exclusion_region_0_daddr = 0x%X", pDSPInitData->dram_exclusion_region_0_daddr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," dram_exclusion_region_0_dsize = %d  ", pDSPInitData->dram_exclusion_region_0_dsize, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," dram_exclusion_region_1_daddr = 0x%X", pDSPInitData->dram_exclusion_region_1_daddr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," dram_exclusion_region_1_dsize = %d  ", pDSPInitData->dram_exclusion_region_1_dsize, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," dram_exclusion_region_2_daddr = 0x%X", pDSPInitData->dram_exclusion_region_2_daddr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," dram_exclusion_region_2_dsize = %d  ", pDSPInitData->dram_exclusion_region_2_dsize, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," dram_exclusion_region_3_daddr = 0x%X", pDSPInitData->dram_exclusion_region_3_daddr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," dram_exclusion_region_3_dsize = %d  ", pDSPInitData->dram_exclusion_region_3_dsize, 0U, 0U);
        for (Idx = 0U; Idx < MAX_NUM_DSP_MSG_Q; Idx++) {
            AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," msg_q_wr_daddr[%d]        = 0x%X", Idx, pDSPInitData->msg_q_wr_daddr[Idx], 0U);
        }
        AmbaLL_Log(AMBALLLOG_TYPE_INIT | AMBALLLOG_TYPE_MSG," crc_init_data                 = 0x%X", pDSPInitData->crc_init_data, 0U, 0U);
    }
}

#if 0
void DSP_DumpCmdDefaultBuffer(void)
{
    UINT32 Idx, IdxMax;
    const dsp_cmd_t *pCmd = NULL;
    const dsp_cmd_t *pHead = NULL;
    const AMBA_DSP_HEADER_CMD_s *pHeaderCmd;
    const AMBA_DSP_DEFAULT_CMDS_s *pDefCmd;
    DSP_MEM_BLK_t MemBlk = {0};

    /* Make sure the buffer is initialized before using AmbaDSP_DefaultCmds */
    if (ArmCommInit() == OK) {
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_DEF_CMD, 0U, &MemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        dsp_osal_typecast(&pDefCmd, &MemBlk.Base);
        pCmd = pDefCmd->CmdData;
        pHead = &pDefCmd->Header;

        dsp_osal_typecast(&pHeaderCmd, &pHead);

        IdxMax = pHeaderCmd->Contents.num_cmds;
        AmbaLL_LogUInt5("DefBuffer:0x%X n:%d s:%d", pHeaderCmd->Contents.cmd_code,
                                                    IdxMax,
                                                    pHeaderCmd->Contents.cmd_seq_num, 0U, 0U);
        for (Idx = 0U; Idx <= IdxMax; Idx++) {
            AmbaLL_LogUInt5("DefBuffer[%2d]:0x%x", Idx, pCmd[Idx].cmd_code, 0U, 0U, 0U);
        }
    }

}

void DSP_DumpCmdBuffer(UINT8 CmdType, UINT8 Detail)
{
    UINT32 Idx, IdxMax;
    const dsp_cmd_t *pCmd;
    const dsp_cmd_t *pTmp;
    const AMBA_DSP_HEADER_CMD_s *pHeader;
    const AMBA_DSP_SYNC_CMD_BUF_CTRL_s  *pCmdCtrl = NULL;
    UINT32 HeadAddr;
    DSP_MEM_BLK_t MemBlk = {0};

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, CmdType, &MemBlk);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    dsp_osal_typecast(&pCmdCtrl, &MemBlk.Base);

    if (CmdType <= AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE) {
        /* Make sure the buffer is initialized before using AmbaDSP_SyncCmdBufCtrl */
        if (ArmCommInit() == OK) {
            AmbaLL_LogUInt5("VDSP%d CmdBuffer wp:%d rp:%d", CmdType, pCmdCtrl->WritePtr, pCmdCtrl->ReadPtr, 0U, 0U);

            dsp_osal_typecast(&pCmd, &pCmdCtrl->pCmdQueueBaseAddr);
            IdxMax = (UINT32) ((UINT32)pCmdCtrl->MaxNumCmds * ((UINT32)pCmdCtrl->MaxNumCmdBuf + 1UL));
            for (Idx = 0U; Idx < IdxMax; Idx++) {
                if ((Idx % pCmdCtrl->MaxNumCmds) == 0UL) {
                    pTmp = &pCmd[Idx];
                    dsp_osal_typecast(&pHeader, &pTmp);
                    dsp_osal_typecast(&HeadAddr, &pHeader);
                    AmbaLL_LogUInt5("        Buffer[%d] 0x%x     n:%d s:%d", Idx / pCmdCtrl->MaxNumCmds,
                                                             HeadAddr,
                                                             pHeader->Contents.num_cmds,
                                                             pHeader->Contents.cmd_seq_num, 0U);
                } else if ((Detail & 0x1U) != 0U) {
                    AmbaLL_LogUInt5("        Buffer[%d][%2d]:0x%x", Idx / pCmdCtrl->MaxNumCmds,
                                                         Idx % pCmdCtrl->MaxNumCmds,
                                                         pCmd[Idx].cmd_code, 0U, 0U);
                } else {
                    // DO NOTHING
                }
            }

            dsp_osal_typecast(&pCmd, &pCmdCtrl->pGroupCmdBufBaseAddr);
            IdxMax = pCmdCtrl->MaxNumCmds;
            for (Idx = 0U; Idx < IdxMax; Idx++) {
                if ((Idx % IdxMax) == 0UL) {
                    pTmp = &pCmd[Idx];
                    dsp_osal_typecast(&pHeader, &pTmp);
                    dsp_osal_typecast(&HeadAddr, &pHeader);
                    AmbaLL_LogUInt5("        Grp   [%d] 0x%x     n:%d s:%d", Idx / IdxMax,
                                                             HeadAddr,
                                                             pHeader->Contents.num_cmds,
                                                             pHeader->Contents.cmd_seq_num, 0U);
                } else if ((Detail & 0x2U) != 0U) {
                    AmbaLL_LogUInt5("        Grp   [%d][%2d]:0x%x", Idx / IdxMax,
                                                         Idx % IdxMax,
                                                         pCmd[Idx].cmd_code, 0U, 0U);
                } else {
                    // DO NOTHING
                }
            }
        }
    }
}
#endif

UINT32 DSP_WaitVdspEvent(UINT32 DspEventFlag, UINT32 WaitCount, UINT32 Timeout)
{
    UINT32 Rval = OK;
    UINT32 i, ActualFlags;

    Rval = DSP_ClearEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, DspEventFlag);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        for (i = 0; i < WaitCount; i++) {
            Rval = DSP_WaitEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, DspEventFlag, 0U, 1U, &ActualFlags, Timeout);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[Err] %d: Can't wait for Vdsp event", __LINE__, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
                break;
            }
        }
    }

    return Rval;
}

void DSP_Suspend(UINT32 Suspend)
{
    LLSuspendStatus = Suspend;
    return;
}

UINT32 LL_WaitDspMsgFlag(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut)
{
    UINT32 Rval = OK;

    /* Make sure the event flag is created before getting LL_MsgEventFlag */
    if (ArmCommInitFlag != 1U) {
        Rval = ArmCommInit();
    }
    if (Rval == OK) {
        Rval = dsp_osal_eventflag_get(&LL_MsgEventFlag, Flag, 0U/*AnyOrAll*/, 1U/*AutoClear*/, ActualFlag, TimeOut);
    }
    return Rval;
}

UINT32 LL_MsgParseEntry(UINT32 EntryArg)
{
    UINT32 Rval = OK, OverflowRval = OK;
    DSP_MEM_BLK_t MemBlk = {0};
    AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pVoutCmdBufCtrl, *pVinCmdBufCtrl;
    AMBA_DSP_ASYNC_CMD_BUF_CTRL_s *pAsyncCmdBufCtrl;
    AMBA_DSP_MSG_BUF_CTRL_s *pVprocMsgBufCtrl, *pAsyncEncMsgBufCtrl;

    AmbaMisra_TouchUnused(&EntryArg);

    /* Make sure the buffer is initialized before using LL_MsgParseEntry */
    if (ArmCommInitFlag != 1U) {
        Rval = ArmCommInit();
    }
    /* Decides the last index that needs to be read (copy from *pCmdReqRptr) */
    if (Rval == OK) {
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP0, &MemBlk);
        if (Rval == OK) {
            dsp_osal_typecast(&pVoutCmdBufCtrl, &MemBlk.Base);
            pVoutCmdBufCtrl->MsgBufCtrl.DspMsgLastPtr = *(pVoutCmdBufCtrl->MsgBufCtrl.pCmdReqRptr);
        }
    }
    if (Rval == OK) {
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_ASYNC_CMD_CTRL, 0U, &MemBlk);
        if (Rval == OK) {
            dsp_osal_typecast(&pAsyncCmdBufCtrl, &MemBlk.Base);
            pAsyncCmdBufCtrl->MsgBufCtrl.DspMsgLastPtr = *(pAsyncCmdBufCtrl->MsgBufCtrl.pCmdReqRptr);
        }
    }
    if (Rval == OK) {
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_ASYNC_ENC_MSG_BUF_CTRL, 0U, &MemBlk);
        if (Rval == OK) {
            dsp_osal_typecast(&pAsyncEncMsgBufCtrl, &MemBlk.Base);
            pAsyncEncMsgBufCtrl->DspMsgLastPtr = *(pAsyncEncMsgBufCtrl->pCmdReqRptr);
        }
    }
    if (Rval == OK) {
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_VPROC_MSG_BUF_CTRL, 0U, &MemBlk);
        if (Rval == OK) {
            dsp_osal_typecast(&pVprocMsgBufCtrl, &MemBlk.Base);
            pVprocMsgBufCtrl->DspMsgLastPtr = *(pVprocMsgBufCtrl->pCmdReqRptr);
        }
    }
    if (Rval == OK) {
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE, &MemBlk);
        if (Rval == OK) {
            dsp_osal_typecast(&pVinCmdBufCtrl, &MemBlk.Base);
            pVinCmdBufCtrl->MsgBufCtrl.DspMsgLastPtr = *(pVinCmdBufCtrl->MsgBufCtrl.pCmdReqRptr);
        }
    }
    /* Start to parse all message buffers */
    if (Rval == OK) {
        OverflowRval = DSP_MsgOverflowDetection(&(pVoutCmdBufCtrl->MsgBufCtrl), DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP0);
        Rval = ArmCommVdspEventHandler(&(pVoutCmdBufCtrl->MsgBufCtrl),
                                       pVoutCmdBufCtrl->SyncCounter,
                                       /*&pVoutCmdBufCtrl->PrevCmdSeqNo,*/ 0U);

        OverflowRval |= DSP_MsgOverflowDetection(&(pVinCmdBufCtrl->MsgBufCtrl), DSP_PROT_BUF_SYNC_CMD_CTRL, AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE);
        Rval |= ArmCommVdspEventHandler(&(pVinCmdBufCtrl->MsgBufCtrl),
                                        pVinCmdBufCtrl->SyncCounter,
                                        /*&pVinCmdBufCtrl->PrevCmdSeqNo,*/ 0U);

        OverflowRval |= DSP_MsgOverflowDetection(&(pAsyncCmdBufCtrl->MsgBufCtrl), DSP_PROT_BUF_ASYNC_CMD_CTRL, 0U);
        Rval |= ArmCommVdspEventHandler(&(pAsyncCmdBufCtrl->MsgBufCtrl), pAsyncCmdBufCtrl->SyncCounter, /*NULL,*/ 0U);

        OverflowRval |= DSP_MsgOverflowDetection(pVprocMsgBufCtrl, DSP_PROT_BUF_VPROC_MSG_BUF_CTRL, 0U);
        Rval |= ArmCommVdspEventHandler(pVprocMsgBufCtrl, 0U, /*NULL,*/ 0U);

        OverflowRval |= DSP_MsgOverflowDetection(pAsyncEncMsgBufCtrl, DSP_PROT_BUF_ASYNC_ENC_MSG_BUF_CTRL, 0U);
        Rval |= ArmCommVdspEventHandler(pAsyncEncMsgBufCtrl, 0U, /*NULL,*/ 0U);
    }

    return (Rval|OverflowRval);
}

/* Mod0~Mode9 */
static inline const char* LL_GetAssertStringPart0(UINT32 ModuleId, const AMBA_DSP_ASSERT_INFO_s *pAssertInfo)
{
    const char *pFmt = NULL;

    if (ModuleId == 0U/*ERR_CODE_MOD_MM*/) {
        pFmt = mm_err_code[pAssertInfo->Info.prn_str];
    } else if (ModuleId == 1U/*ERR_CODE_MOD_API*/) {
        pFmt = api_err_code[pAssertInfo->Info.prn_str - 0x10000U/*API_ERR_UNKNOWN_CODE*/];
    } else if (ModuleId == 2U/*ERR_CODE_MOD_MSG*/) {
        pFmt = msg_err_code[pAssertInfo->Info.prn_str - 0x20000U/*MSG_ERR_UNKNOWN_CODE*/];
    } else if (ModuleId == 3U/*ERR_CODE_MOD_BOOT*/) {
        pFmt = boot_err_code[pAssertInfo->Info.prn_str - 0x30000U/*BOOT_ERR_UNKNOWN_CODE*/];
    } else if (ModuleId == 4U/*ERR_CODE_MOD_IENG*/) {
        pFmt = ieng_err_code[pAssertInfo->Info.prn_str - 0x40000U/*IENG_ERR_UNKNOWN_CODE*/];
    } else if (ModuleId == 5U/*ERR_CODE_MOD_VPROC*/) {
        pFmt = vproc_err_code[pAssertInfo->Info.prn_str - 0x50000U/*VPROC_ERR_UNKNOWN_CODE*/];
    } else if (ModuleId == 6U/*ERR_CODE_MOD_IDSPDRV*/) {
        pFmt = idspdrv_err_code[pAssertInfo->Info.prn_str - 0x60000U/*IDSPDRV_ERR_CODE_UNKNOWN*/];
    } else if (ModuleId == 7U/*ERR_CODE_MOD_VIN*/) {
        pFmt = vin_err_code[pAssertInfo->Info.prn_str - 0x70000U/*VIN_ERR_UNKNOWN_CODE*/];
    } else if (ModuleId == 8U/*ERR_CODE_MOD_PREV*/) {
        pFmt = prev_err_code[pAssertInfo->Info.prn_str - 0x80000U/*PREV_ERR_UNKNOWN_CODE*/];
    } else if (ModuleId == 9U/*ERR_CODE_MOD_PROC_HL*/) {
        pFmt = hl_proc_err_code[pAssertInfo->Info.prn_str - 0x90000U/*HLP_ERRC_UNKNOWN*/];
    } else {
        // DO NOTHING
    }

    return pFmt;
}

/* Mod10~Mode14 */
static inline const char* LL_GetAssertStringPart1(UINT32 ModuleId, const AMBA_DSP_ASSERT_INFO_s *pAssertInfo)
{
    const char *pFmt = NULL;

    if (ModuleId == 10U/*ERR_CODE_MOD_VOUT*/) {
        pFmt = vout_err_code[pAssertInfo->Info.prn_str - 0xA0000U/*ERR_CODE_WRONG_VOUT_BIT_MASK*/];
    } else if (ModuleId == 11U/*ERR_CODE_MOD_ENC*/) {
        pFmt = enc_err_code[pAssertInfo->Info.prn_str - 0xB0000U/*ENC_ERRC_ENC_CFG_DADDR_INVALID*/];
    } else if (ModuleId == 12U/*ERR_CODE_MOD_ENG0*/) {
        pFmt = eng0_err_code[pAssertInfo->Info.prn_str - 0xC0000U/*ENG0_ERRC_UNKNOWN*/];
    } else if (ModuleId == 13U/*ERR_CODE_MOD_PROXY*/) {
        pFmt = proxy_err_code[pAssertInfo->Info.prn_str - 0xD0000U/*PROXY_ERRC_BOOT_0*/];
    } else if (ModuleId == 14U/*ERR_CODE_MOD_DEC*/) {
        pFmt = dec_err_code[pAssertInfo->Info.prn_str - 0xE0000U/*DEC_ERR_UNKNOWN_CODE*/];
    } else {
        // DO NOTHING
    }

    return pFmt;
}

static inline const char* LL_GetAssertString(UINT32 ModuleId, const AMBA_DSP_ASSERT_INFO_s *pAssertInfo)
{
    const char *pFmt = NULL;

    if (ModuleId <= 9U/*ERR_CODE_MOD_PROC_HL*/) {
        pFmt = LL_GetAssertStringPart0(ModuleId, pAssertInfo);
//    } else if ((ModuleId >= 10U ERR_CODE_MOD_VOUT) &&
//               (ModuleId <= 14U ERR_CODE_MOD_DEC)) {
    } else if (ModuleId <= 14U/*ERR_CODE_MOD_DEC*/) {
        pFmt = LL_GetAssertStringPart1(ModuleId, pAssertInfo);
    } else {
        // DO NOTHING
    }

    return pFmt;
}

/* CV5 OrcAsseet format is changed,
 * it needs ucode_error.h/idspdrv_error.h
 * assert_info_t.prn_str will return ErrorCode
 * then we use module id and ErrorCode to map string
 */
void LL_CheckDspAssert(UINT8 IdspBinShare)
{
    UINT32 Rval;
    AMBA_DSP_ASSERT_INFO_s *pAssertInfo;
    UINT32 i, Flag, ActualFlags = 0U;
    DSP_MEM_BLK_t MemBlk = {0};
    AMBA_DSP_EVENT_ERROR_INFO_s *pErrInfo = NULL;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_ASSERT_INFO, 0U, &MemBlk);

    if ((Rval == OK) && (MemBlk.Base > 0U)) {
        dsp_osal_typecast(&pAssertInfo, &MemBlk.Base);
        for (i = 0U; i < DSP_MAX_NUM_ASSERT_INFO; i++) {
            /* Check Thread# assert or not */
            if (pAssertInfo[i].Info.file_name > 0U) {
                osal_ucode_addr_t BaseAddr = {0};
                ULONG CodeVirtAddr;
                ULONG CodeBaseAddr = 0U;
                UINT32 CodeTextAddr = 0U;
                const char *pFmt;
                const char *pFileName;
                char ThreadStr[5], FileLineStr[5];
                UINT32 ModuleId = pAssertInfo[i].Info.prn_str >> 16U;
                UINT32 Len;

                pFmt = LL_GetAssertString(ModuleId, &pAssertInfo[i]);
                Rval = dsp_osal_get_ucode_base_addr(&BaseAddr);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                if (Rval == OK) {
                    if ((i/4U) == 0U) {
                        CodeBaseAddr = BaseAddr.CodeAddr;
                        CodeTextAddr = DSP_LOG_TEXT_BASE_ADDR_CORE;
                    } else if ((i/4U) == 1U) {
                        CodeBaseAddr = BaseAddr.Code1Addr;
                        CodeTextAddr = DSP_LOG_TEXT_BASE_ADDR_VIN;
                    } else if ((i/4U) == 2U) {
                        CodeBaseAddr = BaseAddr.Code2Addr;
                        CodeTextAddr = DSP_LOG_TEXT_BASE_ADDR_IDSP0;
                    } else {
                        if (IdspBinShare == 1U) {
                            CodeBaseAddr = BaseAddr.Code2Addr;
                            CodeTextAddr = DSP_LOG_TEXT_BASE_ADDR_IDSP0;
                        } else {
                            CodeBaseAddr = BaseAddr.Code3Addr;
                            CodeTextAddr = DSP_LOG_TEXT_BASE_ADDR_IDSP1;
                        }
                    }
                    CodeVirtAddr = (pAssertInfo[i].Info.file_name + CodeBaseAddr) - CodeTextAddr;

                    // Only alarm once
                    pAssertInfo[i].Info.file_name = 0U;
                    dsp_osal_typecast(&pFileName, &CodeVirtAddr);
                    Len = dsp_osal_u32_to_str(ThreadStr, 5, i, 10);
                    AmbaMisra_TouchUnused(&Len);
                    Len = dsp_osal_u32_to_str(FileLineStr, 5, pAssertInfo[i].Info.file_line, 10);
                    AmbaMisra_TouchUnused(&Len);
                    AmbaLL_LogStr5("[DSP:th%s] Assertion failure at %s:%s", ThreadStr, pFileName, FileLineStr, NULL, NULL);
                    if (pFmt != NULL) {
                        AmbaLL_LogUInt5(pFmt,
                                        pAssertInfo[i].Info.prn_arg1,
                                        pAssertInfo[i].Info.prn_arg2,
                                        pAssertInfo[i].Info.prn_arg3,
                                        pAssertInfo[i].Info.prn_arg4,
                                        pAssertInfo[i].Info.prn_arg5);
                    } else {
                        AmbaLL_LogUInt5("Unknown AssertModuleId[%d] ERR_CODE[0x%x]", ModuleId, pAssertInfo[i].Info.prn_str, 0U, 0U, 0U);
                    }

                    Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_ERROR);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    if (Rval == OK) {
                        dsp_osal_typecast(&pErrInfo, &EventBufAddr);
                        pErrInfo->Type = AMBA_DSP_ERROR_ASSERT;
                        pErrInfo->Info = pAssertInfo[i].Info.prn_str;
                        pErrInfo->InfoAux = i;
                        Rval = DSP_GiveEvent(LL_EVENT_ERROR, pErrInfo, EventBufIdx);
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                    }
                }
            }
        }

        // Check cmd_req timeout flag
        Flag = (UINT32)EVENT_FLAG_INT_VDSP0_TIMEOUT | (UINT32)EVENT_FLAG_INT_VDSP1_TIMEOUT;
        Rval = DSP_WaitEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, Flag, 0U, 1U, &ActualFlags, 0U/*NO_WAIT*/);
        if ((Rval == OK) && ((ActualFlags & Flag) > 0U)) {
            Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_ERROR);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == OK) {
                dsp_osal_typecast(&pErrInfo, &EventBufAddr);
                if ((ActualFlags & EVENT_FLAG_INT_VDSP0_TIMEOUT) > 0U) {
                    pErrInfo->Type = AMBA_DSP_ERROR_VOUT_CMDREQ;
                } else {
                    pErrInfo->Type = AMBA_DSP_ERROR_VIN_CMDREQ;
                }
                pErrInfo->Info = 0U;
                Rval = DSP_GiveEvent(LL_EVENT_ERROR, pErrInfo, EventBufIdx);
                LL_PrintErrLine(Rval, __func__, __LINE__);
            }
        }
    } else {
        AmbaLL_LogUInt5("[Err][%d] DSP_GetProtBuf return %d", __LINE__, Rval, 0U, 0U, 0U);
    }
}

void DSP_DspParLoadUnlock(UINT8 FullUnlock, UINT32 Mask)
{
    UINT32 Rval;
    dsp_init_data_t *pDSPInitData;
    osal_ucode_addr_t BaseAddr = {0};
    DSP_MEM_BLK_t InitDataMemBlk = {0};
    AMBA_DSP_INIT_DATA_s *pInitData = NULL;
    UINT32 OrcMask = 0xFFFFFFFFU;
    const void *pVoid;

    AmbaMisra_TouchUnused(&Mask);
    Rval = dsp_osal_get_ucode_base_addr(&BaseAddr);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        BaseAddr.Code1Addr += DSP_INIT_DATA_OFFSET;
        dsp_osal_typecast(&pDSPInitData, &BaseAddr.Code1Addr);

        /*
         * CV5x didn't use this flag to indicate fully load.
         * To achieve partial load
         * we can postpone orccode/orcme/orcmdxf load and postpone orccode/orcme/orcmdxf orc-enable
         */
        pDSPInitData->is_orccode_fully_load = FullUnlock;

        Rval = DSP_GetProtBuf(DSP_PROT_BUF_INIT_DATA, 0U, &InitDataMemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            dsp_osal_typecast(&pInitData, &InitDataMemBlk.Base);
            if (pInitData->Data.is_orccode_fully_load == 0U) {
                if (DSP_GetProfState() != DSP_PROF_STATUS_INVALID) {
                    dsp_init_data_t *pTmpDSPInitData;

                    /* Fill OrcCode init-data for safe */
                    BaseAddr.CodeAddr += DSP_INIT_DATA_OFFSET;
                    dsp_osal_typecast(&pTmpDSPInitData, &BaseAddr.CodeAddr);
                    Rval = dsp_osal_memcpy(pTmpDSPInitData, pDSPInitData, sizeof(dsp_init_data_t));
                    LL_PrintErrLine(Rval, __func__, __LINE__);

                    dsp_osal_typecast(&pVoid, &pTmpDSPInitData);
                    Rval = dsp_osal_cache_clean(pVoid, (UINT32)sizeof(dsp_init_data_t));
                    LL_PrintErrLine(Rval, __func__, __LINE__);

                    DSP_ClearBit(&OrcMask, DSP_ORC_CODE_IDX);
                    DSP_ClearBit(&OrcMask, DSP_ORC_ME0_IDX);
                    DSP_ClearBit(&OrcMask, DSP_ORC_ME1_IDX);
                    DSP_ClearBit(&OrcMask, DSP_ORC_MDXF0_IDX);
                    DSP_ClearBit(&OrcMask, DSP_ORC_MDXF1_IDX);
                    dsp_osal_orc_enable(OrcMask);
                }
                pInitData->Data.is_orccode_fully_load = FullUnlock;
            }
        }
    }
}

UINT32 DSP_WaitVcapEvent(const UINT16 NumVin, const UINT16 *pVinIdx, UINT8 WaitCount, UINT32 Timeout)
{
    UINT32 Rval = OK;
    UINT64 BaseCounter[AMBA_DSP_MAX_VIN_NUM] = {0U};
    UINT64 TargetCounter[AMBA_DSP_MAX_VIN_NUM] = {0U}, Counter = 0U;
    UINT32 ActualFlags, i, TargetDoneNum = 0U;
    DSP_MEM_BLK_t MemBlk = {0};
    const AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pSyncCmdBufCtrl;

    Rval =  DSP_ClearEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, (UINT32)EVENT_FLAG_INT_VDSP1_EVENT);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        for (i = 0U; i < NumVin; i++) {
            if ((AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE + pVinIdx[i]) < AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL) {
                Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, (UINT32)AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE + pVinIdx[i], &MemBlk);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                if (Rval == OK) {
                    dsp_osal_typecast(&pSyncCmdBufCtrl, &MemBlk.Base);

                    BaseCounter[i] = pSyncCmdBufCtrl->IntFlagCounter;
                    TargetCounter[i] = BaseCounter[i] + (UINT64)WaitCount;
                }
            } else {
                Rval = DSP_ERR_0001;
                break;
            }
        }
    }

    if (Rval == OK) {
        while (TargetDoneNum < NumVin) {
            Rval = DSP_WaitEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_INT, (UINT32)EVENT_FLAG_INT_VDSP1_EVENT, 0U, 1U, &ActualFlags, Timeout);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[Error] %d: Can't wait for Vdsp1 event", __LINE__, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
                break;
            }
            TargetDoneNum = 0U;
            for (i = 0U; i < NumVin; i++) {
                Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, (UINT32)AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE + pVinIdx[i], &MemBlk);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                if (Rval == OK) {
                    dsp_osal_typecast(&pSyncCmdBufCtrl, &MemBlk.Base);
                    Counter = pSyncCmdBufCtrl->IntFlagCounter;
                    if (Counter >= TargetCounter[i]) {
                        TargetDoneNum++;
                    }
                }
            }
        }
    } else {
        Rval = DSP_ERR_0001;
    }
    return Rval;
}

UINT32 AmbaLL_CmdQuery(UINT8 WriteMode, UINT32 CmdCode, ULONG *pCmdAddr, UINT32 Val0, UINT32 Val1)
{
    UINT32 Rval;
    osal_spinlock_t SpinLock;
    UINT32 i;
    UINT8 ExitLoop = 0U;
    const AMBA_DSP_HEADER_CMD_s *pHeader;
    const AMBA_DSP_HEADER_CMD_s *pTmpHeader;
    const dsp_cmd_t *pDspCmd;
    const AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pCmdBufCtrl = NULL;
    ULONG ULAddr;
    DSP_MEM_BLK_t MemBlk = {0};

    // Make sure the buffer is initialized before using AmbaDSP_SyncCmdBufCtrl
    Rval = ArmCommInit();

    if (Rval == OK) {
        if ((IsNormalWriteMode(WriteMode) == 1U) && (WriteMode < AMBA_DSP_NUM_CMD_WRITE)) {
            Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, SyncCmdBufCtrlIdMap[WriteMode], &MemBlk);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == OK) {
                dsp_osal_typecast(&pCmdBufCtrl, &MemBlk.Base);
            }
        } else {
            AmbaLL_LogUInt5("[Err] Invalid CmdType[%u]", WriteMode, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0007;
        }
    }

    if (Rval == OK) {
        /* Only process VinAttachCmd now */
        if (CmdCode == CMD_VIN_ATTACH_PROC_CFG_TO_CAP_FRM) {
            const cmd_vin_attach_proc_cfg_to_cap_frm_t *pVinAttachProcCfg;
            const vin_fov_batch_cmd_set_t *pBatchCmdSet;

            /* Prevent entering IRQ or FIQ handler */
            Rval = dsp_osal_cmdlock(&DspCmdBufMtx, &SpinLock);
            LL_PrintErrLine(Rval, __func__, __LINE__);

            if (Rval == OK) {
                pDspCmd = &(pCmdBufCtrl->pCmdBufBaseAddr[pCmdBufCtrl->WritePtr * pCmdBufCtrl->MaxNumCmds]);
                dsp_osal_typecast(&pHeader, &pDspCmd);

                for (i = 1U; i <= pHeader->Contents.num_cmds; i++) {
                    pTmpHeader = &pHeader[i];
                    dsp_osal_typecast(&pVinAttachProcCfg, &pTmpHeader);
                    if (pVinAttachProcCfg->cmd_code == CMD_VIN_ATTACH_PROC_CFG_TO_CAP_FRM) {
                        Rval = dsp_osal_cli2virt(pVinAttachProcCfg->fov_batch_cmd_set_addr, &ULAddr);
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                        dsp_osal_typecast(&pBatchCmdSet, &ULAddr);
                        if ((pVinAttachProcCfg->vin_id == (UINT8)Val0) &&
                            (pBatchCmdSet->vin_id == (UINT8)Val0) &&
                            (pBatchCmdSet->chan_id == (UINT8)Val1)) {
                            //Report CmdAddr
                            dsp_osal_typecast(&ULAddr, &pVinAttachProcCfg);
                            *pCmdAddr = ULAddr;
                            ExitLoop = 1U;
                            break;
                        }
                    }
                }

                Rval = dsp_osal_cmdunlock(&DspCmdBufMtx, &SpinLock);
                LL_PrintErrLine(Rval, __func__, __LINE__);
            }

            if (ExitLoop == 0U) {
                /* No desired CmdFound or information no match */
                *pCmdAddr = 0x0U;
            }
        } else {
            *pCmdAddr = 0x0U;
        }
    }

    return Rval;
}

UINT32 AmbaLL_CmdSend(UINT8 WriteMode, const void *pCmdData, UINT32 CmdDataSize)
{
    UINT32 Rval;
    DSP_MEM_BLK_t MemBlk = {0};
    AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pSyncCmdBufCtrl;

    // Make sure the buffer is initialized before using AmbaDSP_SyncCmdBufCtrl
    Rval = ArmCommInit();

    if (Rval == OK) {
        if (WriteMode < AMBA_DSP_NUM_CMD_WRITE) {
            if (WriteMode == AMBA_DSP_CMD_NOP) {
                Rval = OK;
            } else if (WriteMode == AMBA_DSP_CMD_DEFAULT_WRITE) {
                Rval = WriteDefaultCmdBuf(AMBA_DSP_SYS_STATE_NUM, pCmdData, CmdDataSize);
            } else if (WriteMode == AMBA_DSP_CMD_ASYNC_WRITE) {
                Rval = WriteAsyncCmdBuf(pCmdData, CmdDataSize);
            } else if (IsNormalWriteMode(WriteMode) == 1U) {
                Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, SyncCmdBufCtrlIdMap[WriteMode], &MemBlk);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                if (Rval == OK) {
                    dsp_osal_typecast(&pSyncCmdBufCtrl, &MemBlk.Base);
                    Rval = WriteCmdBuf(pSyncCmdBufCtrl, pCmdData, CmdDataSize);
                }
            } else if (IsGroupWriteMode(WriteMode) == 1U) {
                Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, SyncCmdBufCtrlIdMap[WriteMode], &MemBlk);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                if (Rval == OK) {
                    dsp_osal_typecast(&pSyncCmdBufCtrl, &MemBlk.Base);
                    Rval = WriteGroupCmdBuf(pSyncCmdBufCtrl, pCmdData, CmdDataSize);
                }
            } else {
                AmbaLL_LogUInt5("[Err@CmdWrite] Invalid CmdType[%u]", WriteMode, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0007;
            }
        } else {
            AmbaLL_LogUInt5("[Err@CmdWrite] Invalid CmdType[%u]", WriteMode, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0007;
        }
    }
    return Rval;
}

UINT32 DSP_ProcessBufCmds(UINT8 GroupWrite)
{
    UINT32 Rval;
    DSP_MEM_BLK_t MemBlk = {0};
    AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pSyncCmdBufCtrl;

    // Make sure the event flag is created before getting AmbaDSP_SyncCmdBufCtrl
    Rval = ArmCommInit();

    if (Rval == OK) {
        if (GroupWrite < AMBA_DSP_NUM_CMD_WRITE) {
            if (GroupWrite == AMBA_DSP_CMD_DEFAULT_WRITE) {
                Rval = OK;
            } else if (IsGroupWriteMode(GroupWrite) == 1U) {
                Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, SyncCmdBufCtrlIdMap[GroupWrite], &MemBlk);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                if (Rval == OK) {
                    dsp_osal_typecast(&pSyncCmdBufCtrl, &MemBlk.Base);
                    Rval = SendGroupCmdBuf(pSyncCmdBufCtrl);
                }
            } else {
                AmbaLL_LogUInt5("[Err][%d] Invalid GrpWrType[%d]", __LINE__, GroupWrite, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            }
        } else {
            AmbaLL_LogUInt5("[Err][%d] Invalid GrpWrType[%d]", __LINE__, GroupWrite, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        }
    }
    return Rval;
}

#if defined (CONFIG_ENABLE_DSP_DIAG)
//#define DEBUG_DSP_DIAG_LOG
UINT32 DSP_GetFirstCmdAddr(UINT32 CmdBufIdx, ULONG *pCmdAddr)
{
    const AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pCmdBufCtrl;
    const dsp_cmd_t *pDspCmd;
    DSP_MEM_BLK_t MemBlk = {0};
    ULONG CmdBaseAddr;
    UINT32 Rval;

    // Make sure the buffer is initialized before using AmbaDSP_SyncCmdBufCtrl
    Rval = ArmCommInit();
    if (Rval == OK) {
        if (CmdBufIdx < AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL) {
            Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, CmdBufIdx, &MemBlk);
            LL_PrintErrLine(Rval, __func__, __LINE__);

            if (Rval == OK) {
                dsp_osal_typecast(&pCmdBufCtrl, &MemBlk.Base);
                pDspCmd = &(pCmdBufCtrl->pCmdBufBaseAddr[pCmdBufCtrl->WritePtr * pCmdBufCtrl->MaxNumCmds]);

                dsp_osal_typecast(&CmdBaseAddr, &pDspCmd);
                *pCmdAddr = CmdBaseAddr + sizeof(dsp_cmd_t);
#ifdef DEBUG_DSP_DIAG_LOG
                {
                    AMBA_DSP_HEADER_CMD_s *pHeader = NULL;
                    dsp_osal_typecast(&pHeader, &pDspCmd);

                    AmbaLL_LogUInt5(" [DSP Diag] CmdIdx[%d] BaseAddr[0x%x%x] CmdSize[%d] CmdNum[%d]"
                        , CmdBufIdx, DSP_GetU64Msb((UINT64)CmdBaseAddr), DSP_GetU64Lsb((UINT64)CmdBaseAddr), sizeof(dsp_cmd_t), pHeader->Contents.num_cmds);
                    AmbaLL_LogUInt5("            FirstCmdAddr[0x%x%x] CmdCode[0x%x]"
                        , DSP_GetU64Msb((UINT64)*pCmdAddr), DSP_GetU64Lsb((UINT64)*pCmdAddr), pHeader[1U].Contents.cmd_code, 0U, 0U);
                }
#endif
            }
        } else {
            AmbaLL_LogUInt5("[Err][%d] Invalid CmdBufIdx[%d]", __LINE__, CmdBufIdx, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        }
    }
    return Rval;
}

UINT32 DSP_GetNextMsgAddr(UINT32 MsgBufIdx, ULONG *pMsgAddr)
{
    const AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pCmdBufCtrl;
    DSP_MEM_BLK_t MemBlk = {0};
    ULONG ULAddr;
    UINT32 ResetIdx;
    UINT32 Rval;

    // Make sure the buffer is initialized before using AmbaDSP_SyncCmdBufCtrl
    Rval = ArmCommInit();
    if (Rval == OK) {
        if (MsgBufIdx < AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL) {
            Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, MsgBufIdx, &MemBlk);
            LL_PrintErrLine(Rval, __func__, __LINE__);

            if (Rval == OK) {
                dsp_osal_typecast(&pCmdBufCtrl, &MemBlk.Base);

                if (pCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl != NULL) {
                    Rval = dsp_osal_cli2virt(pCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->base_daddr, &ULAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);

                    if (Rval == OK) {
                        ResetIdx = (pCmdBufCtrl->MsgBufCtrl.DspMsgRp + 1U) % pCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->max_num_msg;
#ifdef DEBUG_DSP_DIAG_LOG
                        AmbaLL_LogUInt5(" [DSP Diag] W/R[%d %d] ResetIdx[%d]"
                            , pCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr, pCmdBufCtrl->MsgBufCtrl.DspMsgRp, ResetIdx, 0U, 0U);
#endif
                        *pMsgAddr = ULAddr + (sizeof(dsp_msg_t)*ResetIdx);
                    }
                } else {
                    Rval = DSP_ERR_0000;
                }
            }
        } else {
            AmbaLL_LogUInt5("[Err][%d] Invalid CmdBufIdx[%d]", __LINE__, MsgBufIdx, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        }
    }
    return Rval;
}

UINT32 DSP_GetMsgPtrData(UINT32 MsgBufIdx, UINT32 *pMaxNumMsg, UINT32 **pReadPtr, UINT32 **pWritePtr)
{
    const AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pCmdBufCtrl;
    DSP_MEM_BLK_t MemBlk = {0};
    ULONG ULAddr;
    UINT32 Rval;

    // Make sure the buffer is initialized before using AmbaDSP_SyncCmdBufCtrl
    Rval = ArmCommInit();
    if (Rval == OK) {
        if (MsgBufIdx < AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL) {
            Rval = DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, MsgBufIdx, &MemBlk);
            LL_PrintErrLine(Rval, __func__, __LINE__);

            if (Rval == OK) {
                dsp_osal_typecast(&pCmdBufCtrl, &MemBlk.Base);

                if (pCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl != NULL) {
                    Rval = dsp_osal_cli2virt(pCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->base_daddr, &ULAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);

                    if (Rval == OK) {
                        *pMaxNumMsg = pCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->max_num_msg;
                        *pReadPtr = &(pCmdBufCtrl->MsgBufCtrl.DspMsgRp);
                        *pWritePtr = &(pCmdBufCtrl->MsgBufCtrl.pMsgFifoCtrl->write_ptr);
                    }
                } else {
                    Rval = DSP_ERR_0000;
                }
            }
        } else {
            AmbaLL_LogUInt5("[Err][%d] Invalid CmdBufIdx[%d]", __LINE__, MsgBufIdx, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        }
    }
    return Rval;
}
#endif

UINT32 DSP_CheckCmdBufLock(const UINT16 VinId)
{
    UINT32 Rval = OK, i;
    DSP_MEM_BLK_t MemBlk = {0};
    AMBA_DSP_SYNC_CMD_BUF_CTRL_s *pVdspSyncCmdBufCtrl;

    for (i = 0U; i < AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL; i++) {
        if (OK == DSP_GetProtBuf(DSP_PROT_BUF_SYNC_CMD_CTRL, i, &MemBlk)) {
            dsp_osal_typecast(&pVdspSyncCmdBufCtrl, &MemBlk.Base);
            if (1U == DSP_GetBit(pVdspSyncCmdBufCtrl->CmdLock, VinId, 1U)) {
                Rval = DSP_ERR_0004;
                break;
            }
        }
    }
    return Rval;
}

