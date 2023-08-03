/**
 *  @file AmbaCSL_CAN.c
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
 *  @details CAN CSL Device Driver
 *
 */

#include "AmbaTypes.h"
#include "AmbaCortexA53.h"
#include "AmbaCSL_CAN.h"
#include "AmbaMisraFix.h"

static UINT8 AmbaCan_DlcTable[AMBA_NUM_CAN_DATA_LENGTH] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64
};

void AmbaCSL_CanSetTq(AMBA_CAN_REG_s *pCanReg, const AMBA_CAN_TQ_s *pTq)
{
    UINT32 CanTqRegVal;

    if (pTq != NULL) {
        UINT32 Value;

        Value = pTq->PhaseSeg2 /*<< AMBA_CAN_TQ_REG_TS2_SHIFT*/;
        CanTqRegVal   = Value & AMBA_CAN_TQ_REG_TS2_MASK;
        Value = pTq->PropSeg + pTq->PhaseSeg1;
        Value = Value << AMBA_CAN_TQ_REG_TS1_SHIFT;
        Value &= AMBA_CAN_TQ_REG_TS1_MASK;
        CanTqRegVal   |= Value;
        Value = pTq->SJW << AMBA_CAN_TQ_REG_SJW_SHIFT;
        Value &=  AMBA_CAN_TQ_REG_SJW_MASK;
        CanTqRegVal   |= Value;
        Value = pTq->BRP << AMBA_CAN_TQ_REG_BRP_SHIFT;
        Value &= AMBA_CAN_TQ_REG_BRP_MASK;
        CanTqRegVal   |= Value;

        AmbaCSL_CanSetTqReg(pCanReg, CanTqRegVal);
    }
}

void AmbaCSL_CanGetTq(const AMBA_CAN_REG_s *pCanReg, AMBA_CAN_TQ_s *pTq)
{
    UINT32 CanTqRegVal;

    if ((pCanReg != NULL) && (pTq != NULL)) {
        UINT32 Value;
        CanTqRegVal = AmbaCSL_CanGetTqReg(pCanReg);

        Value = CanTqRegVal & AMBA_CAN_TQ_REG_BRP_MASK;
        Value = Value >> AMBA_CAN_TQ_REG_BRP_SHIFT;
        pTq->BRP = Value;
        pTq->PropSeg = 0;                           // PropSeg is merged into Tseeq1.
        Value = CanTqRegVal & AMBA_CAN_TQ_REG_TS1_MASK;
        Value = Value >> AMBA_CAN_TQ_REG_TS1_SHIFT;
        pTq->PhaseSeg1 = Value;
        Value = CanTqRegVal & AMBA_CAN_TQ_REG_TS2_MASK;
        /* AMBA_CAN_TQ_REG_TS2_SHIFT is 0.*/
        pTq->PhaseSeg2 = Value;
        Value = CanTqRegVal & AMBA_CAN_TQ_REG_SJW_MASK;
        Value = Value >> AMBA_CAN_TQ_REG_SJW_SHIFT;
        pTq->SJW = Value;
    }
}


void AmbaCSL_CanSetFdTq(AMBA_CAN_REG_s *pCanReg, const AMBA_CAN_TQ_s *pTq)
{
    UINT32 CanFdTqRegVal;

    if (pTq != NULL) {
        UINT32 Value;
        Value = pTq->PhaseSeg2 /*<< AMBA_CAN_FDTQ_REG_TS2_SHIFT*/;
        Value &= AMBA_CAN_FDTQ_REG_TS2_MASK;
        CanFdTqRegVal   = Value;
        Value = pTq->PropSeg + pTq->PhaseSeg1;
        Value = Value << AMBA_CAN_FDTQ_REG_TS1_SHIFT;
        Value &= AMBA_CAN_FDTQ_REG_TS1_MASK;
        CanFdTqRegVal   |= Value;
        Value = pTq->SJW << AMBA_CAN_FDTQ_REG_SJW_SHIFT;
        Value &= AMBA_CAN_FDTQ_REG_SJW_MASK;
        CanFdTqRegVal   |= Value;
        Value = pTq->BRP << AMBA_CAN_FDTQ_REG_BRP_SHIFT;
        Value &= AMBA_CAN_FDTQ_REG_BRP_MASK;
        CanFdTqRegVal   |= Value;

        AmbaCSL_CanFdSetTqReg(pCanReg, CanFdTqRegVal);
    }
}

void AmbaCSL_CanGetFdTq(const AMBA_CAN_REG_s *pCanReg, AMBA_CAN_TQ_s *pTq)
{
    UINT32 CanFdTqRegVal;

    if ((pCanReg != NULL) && (pTq != NULL)) {
        UINT32 Value;

        CanFdTqRegVal= AmbaCSL_CanGetTqdReg(pCanReg);

        Value = CanFdTqRegVal & AMBA_CAN_FDTQ_REG_BRP_MASK;
        Value = Value >> AMBA_CAN_FDTQ_REG_BRP_SHIFT;
        pTq->BRP = Value;
        pTq->PropSeg = 0;                           // PropSeg is merged into Tseeq1.
        Value = CanFdTqRegVal & AMBA_CAN_FDTQ_REG_TS1_MASK;
        Value = Value >> AMBA_CAN_FDTQ_REG_TS1_SHIFT;
        pTq->PhaseSeg1 = Value;
        Value = CanFdTqRegVal & AMBA_CAN_FDTQ_REG_TS2_MASK;
        /*AMBA_CAN_FDTQ_REG_TS2_SHIFT is 0.*/
        pTq->PhaseSeg2 = Value;
        Value = CanFdTqRegVal & AMBA_CAN_FDTQ_REG_SJW_MASK;
        Value = Value >> AMBA_CAN_FDTQ_REG_SJW_SHIFT;
        pTq->SJW = Value;
    }
}

INT32 AmbaCSL_CanRequestMsgBuf(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo)
{
    UINT32 RequestMask = (1UL << MsgBufNo);
    UINT32 Request;
    UINT32 Granted;
    INT32 RetStatus;

    /* Send the message buffer request */
    Request = AmbaCSL_CanGetMsgBufRequest(pCanReg) | RequestMask;
    AmbaCSL_CanSetMsgBufRequest(pCanReg, Request);

    /* Poll the message buffer request */
    Request = AmbaCSL_CanGetMsgBufRequest(pCanReg) & RequestMask;

    /* Check if the request is grant or not */
    if (Request == 0U) {
        Granted = AmbaCSL_CanGetMsgBufGrant(pCanReg);
        Granted &= RequestMask;
        if (Granted != 0U) {
            RetStatus = 1;
        } else {
            RetStatus = 0;
        }
    } else {
        RetStatus = -1;
    }

    return RetStatus;
}

void AmbaCSL_CanGetRxMsgBufData(const AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo,  UINT8 *pData, UINT8 SizeCode)
{
    UINT32 CoupleData;
    UINT8 i = 0, j, k = 0, DataLength;

    if (pData != NULL) {
        DataLength = AmbaCan_DlcTable[SizeCode];
        while (i < DataLength) {
            CoupleData = pCanReg->MsgData[MsgBufNo][k];
            k++;
            for (j = 0; j < 4U; j++) {
                if (i < DataLength) {
                    pData[i] = (UINT8)(CoupleData & 0xffU);
                    CoupleData = (CoupleData >> 8U);
                    i++;
                } else {
                    break;
                }
            }
        }
    }
}

void AmbaCSL_CanSetIdFilter(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, UINT8 Enable, UINT32 Id, UINT32 IdMask)
{
    UINT32 IdFilterData;
    UINT32 Value;

    if (Enable != 0U) {
        IdFilterData = 1UL << CAN_IDF_REG_ENABLE_FILTER_SHIFT;
    } else {
        IdFilterData = 0;
    }
    Value = Id /*<< CAN_IDF_REG_ID_FILTER_SHIFT*/;
    Value &= CAN_IDF_REG_ID_FILTER_MASK;
    IdFilterData |= Value;

    AmbaCSL_CanSetFilterId(pCanReg, MsgBufNo, IdFilterData);
    AmbaCSL_CanSetFilterMask(pCanReg, MsgBufNo, IdMask);
}

void AmbaCSL_CanConfigDone(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo)
{
    volatile UINT32 *pMsgBufConfigDone = (volatile UINT32 *) &(pCanReg->MsgBufCfgDone);

    *pMsgBufConfigDone |= (1UL << MsgBufNo);
}

INT32 AmbaCSL_CanSetMsgBufCtrl(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, const AMBA_CAN_MSG_s *pMessage)
{
    INT32 Ret;

    if ((pCanReg != NULL) && (pMessage != NULL)) {
        UINT32 MsgBufRegCtrl;
        UINT32 Value;
        /* Fill message control register */
        MsgBufRegCtrl = (UINT32)pMessage->DataLengthCode /*<< CAN_MBC_REG_DLC_SHIFT*/;
        MsgBufRegCtrl = MsgBufRegCtrl & CAN_MBC_REG_DLC_MASK;
        Value = (pMessage->RemoteTxReq != 0U)?1UL:0UL;
        Value = Value << CAN_MBC_REG_RTR_SHIFT;
        Value = Value & CAN_MBC_REG_RTR_MASK;
        MsgBufRegCtrl |= Value;

        /* Set message control register */
        AmbaCSL_CanSetMsgCtrl(pCanReg, MsgBufNo, MsgBufRegCtrl);
        Ret = 0;
    } else {
        Ret = -1;
    }

    return Ret;
}

INT32 AmbaCSL_CanFdSetMsgBufCtrl(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, const AMBA_CAN_FD_MSG_s *pMessage)
{
    INT32 Ret;

    if ((pCanReg != NULL) && (pMessage != NULL)) {
        UINT32 MsgBufRegCtrl;
        UINT32 Value;

        /* Fill message control register */
        MsgBufRegCtrl = (UINT32)pMessage->DataLengthCode /*<< CAN_MBC_REG_DLC_SHIFT*/;
        MsgBufRegCtrl = MsgBufRegCtrl & CAN_MBC_REG_DLC_MASK;
        Value = 1UL << CAN_MBC_REG_EDL_SHIFT;
        MsgBufRegCtrl |= Value;
        Value = (pMessage->BitRateSwitch != 0U)?1UL:0UL;
        Value = Value << CAN_MBC_REG_BRS_SHIFT;
        MsgBufRegCtrl |= Value;

        /* Set message control register */
        AmbaCSL_CanSetMsgCtrl(pCanReg, MsgBufNo, MsgBufRegCtrl);
        Ret = 0;
    } else {
        Ret = -1;
    }

    return Ret;
}

void AmbaCSL_CanSetMsgBufId(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, UINT32 Id, UINT8 Extension)
{
    UINT32 MsgBufRegID;
    UINT32 Value;

    /* Fill message ID register */
    if (Extension != 0U) {
        MsgBufRegID = 1UL << CAN_MSG_BUF_ID_REG_MSGEXT_SHIFT;
        Value = Id & CAN_MSG_BUF_ID_REG_MSGID_MASK;
        /*CAN_MSG_BUF_ID_REG_MSGID_SHIFT is 0.*/
    } else {
        MsgBufRegID = 0;
        Value = Id & 0x7ffU;
        Value &= CAN_MSG_BUF_ID_REG_MSGID_MASK;
        Value = Value << 18U;
        /*CAN_MSG_BUF_ID_REG_MSGID_SHIFT is 0*/
    }

    MsgBufRegID |= Value;

    /* Set message ID register */
    AmbaCSL_CanSetMsgId(pCanReg, MsgBufNo, MsgBufRegID);
}

INT32 AmbaCSL_CanSetMsgBufData(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, const UINT8 *pData, UINT8 Size)
{
    INT32 Ret;

    if ((pCanReg != NULL) && (pData != NULL)) {
        volatile UINT32 *pMsgBufData = (volatile UINT32 *) pCanReg->MsgData[MsgBufNo];
        UINT32 CoupleData;
        UINT32 i = 0, j, k = 0, DataLength;

        /* Fill message data register */
        DataLength = AmbaCan_DlcTable[Size];
        while (i < DataLength) {
            CoupleData = 0;
            for (j = 0; (j < 4U) && (i < DataLength); j++) {
                UINT32 Offset = (UINT32)j * 8U;
                UINT32 Value = (UINT32)pData[i++] << Offset;
                CoupleData |= Value;
            }
            pMsgBufData[k] = CoupleData;
            k++;
        }
        Ret = 0;
    } else {
        Ret = -1;
    }

    return Ret;
}

void AmbaCSL_CanSetMsgBufPriority(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, UINT8 Priority)
{
    UINT32 TmpValue = pCanReg->MsgPriority[MsgBufNo >> 2U];
    UINT32 PriorityData;
    UINT32 SubLocation;
    UINT32 TmpPriority = (UINT32)Priority;

    /* Set message buffer priority */
    SubLocation = 24U - ((MsgBufNo & 0x3U) * 8U);
    PriorityData = (0x1fUL << SubLocation);
    PriorityData = ~PriorityData;
    PriorityData = TmpValue & PriorityData;
    TmpPriority = TmpPriority << SubLocation;
    pCanReg->MsgPriority[MsgBufNo >> 2U] = PriorityData | TmpPriority;
}

UINT32 AmbaCSL_CanGetMsgId(const AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo)
{
    UINT32 TmpId = pCanReg->MsgCtrl[MsgBufNo].MsgBufId;
    TmpId &= CAN_MSG_BUF_ID_REG_MSGID_MASK;
    /*CAN_MSG_BUF_ID_REG_MSGID_SHIFT is 0.*/
    return TmpId;
}

UINT8 AmbaCSL_CanGetMsgExtention(const AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo)
{
    UINT8 Ret;
    UINT32 TmpId = pCanReg->MsgCtrl[MsgBufNo].MsgBufId;
    TmpId &= (UINT32)CAN_MSG_BUF_ID_REG_MSGEXT_MASK;
    Ret = (TmpId != 0U)?1U:0U;

    return Ret;
}

void AmbaCSL_CanInit(void)
{
#ifdef __aarch64__
#else
        // 32-bit mode
        UINT32 SrcVirtAddr = AMBA_CORTEX_A53_CAN0_BASE_ADDR;
        AMBA_CAN_REG_s *RegPtr;

        // convert UINT32 to AMBA_CAN_REG_s * const
        AmbaMisra_TypeCast32(&RegPtr, &SrcVirtAddr);
        pAmbaCAN_Reg[AMBA_CAN_CHANNEL0] = RegPtr;
#endif
}

void AmbaCSL_CanFdEnable(AMBA_CAN_REG_s *pCanReg) {pCanReg->Ctrl.EnableFD = 1;}
void AmbaCSL_CanFdDisable(AMBA_CAN_REG_s *pCanReg) {pCanReg->Ctrl.EnableFD = 0;}
void AmbaCSL_CanSetAutoRespMode(AMBA_CAN_REG_s *pCanReg, UINT32 d) {pCanReg->Ctrl.AutoRespMode = (UINT8)d;}
void AmbaCSL_CanSetListenMode(AMBA_CAN_REG_s *pCanReg, UINT32 d) {pCanReg->Ctrl.ListenMode = (UINT8)d;}
void AmbaCSL_CanSetLoopbackMode(AMBA_CAN_REG_s *pCanReg, UINT32 d) {pCanReg->Ctrl.LoopbackMode = (UINT8)d;}
void AmbaCSL_CanSetNonIsoCanFdMode(AMBA_CAN_REG_s *pCanReg, UINT32 d) {pCanReg->Ctrl.NonIsoCanFdMode = (UINT8)d;}

void AmbaCSL_TtCanEnable(AMBA_CAN_REG_s *pCanReg) {pCanReg->EnableTtCan = 1;}
void AmbaCSL_TtCanDisable(AMBA_CAN_REG_s *pCanReg) {pCanReg->EnableTtCan = 0;}
void AmbaCSL_TtCanTtTimerEnable(AMBA_CAN_REG_s *pCanReg) {pCanReg->EnableTtTimer = 1;}
void AmbaCSL_TtCanTtTimerDisable(AMBA_CAN_REG_s *pCanReg) {pCanReg->EnableTtTimer = 0;}

void AmbaCSL_CanSetTqReg(AMBA_CAN_REG_s *pCanReg, UINT32 d) {pCanReg->Tq = d;}
void AmbaCSL_CanFdSetTqReg(AMBA_CAN_REG_s *pCanReg, UINT32 d) {pCanReg->TqFD = d;}

UINT32 AmbaCSL_CanGetTqReg(const AMBA_CAN_REG_s *pCanReg) {return pCanReg->Tq;}
UINT32 AmbaCSL_CanGetTqdReg(const AMBA_CAN_REG_s *pCanReg) {return pCanReg->TqFD;}

UINT32 AmbaCSL_CanGetMsgBufType(const AMBA_CAN_REG_s *pCanReg) {return pCanReg->MsgBufType;}
void AmbaCSL_CanSetMsgBufType(AMBA_CAN_REG_s *pCanReg, UINT32 d) {pCanReg->MsgBufType = d;}
void AmbaCSL_CanSetMsgBufConfigDone(AMBA_CAN_REG_s *pCanReg, UINT32 d) {pCanReg->MsgBufCfgDone = d;}
void AmbaCSL_CanSetIntMask(AMBA_CAN_REG_s *pCanReg, UINT32 d) {pCanReg->IntMask = d;}
void AmbaCSL_CanSetTxIntThreshold(AMBA_CAN_REG_s *pCanReg, UINT32 d) {pCanReg->TxIntCtrl.AccIntCtrl.CountTh = (UINT8)d;}
void AmbaCSL_CanSetTxIntTimeout(AMBA_CAN_REG_s *pCanReg, UINT32 d) {pCanReg->TxIntCtrl.TimeOut = d;}
void AmbaCSL_CanSetRxIntThreshold(AMBA_CAN_REG_s *pCanReg, UINT32 d) {pCanReg->RxIntCtrl.AccIntCtrl.CountTh = (UINT8)d;}
void AmbaCSL_CanSetRxIntTimeout(AMBA_CAN_REG_s *pCanReg, UINT32 d) {pCanReg->RxIntCtrl.TimeOut = d;}
UINT32 AmbaCSL_CanGetRxBufOverflowId(const AMBA_CAN_REG_s *pCanReg) {return  pCanReg->RxOverflowId;}
UINT32 AmbaCSL_CanGetRetryFail(const AMBA_CAN_REG_s *pCanReg) {return pCanReg->MsgBufRetryFail;}

void AmbaCSL_CanSetMsgBufRequest(AMBA_CAN_REG_s *pCanReg, UINT32 d) {pCanReg->MsgBufRequest = d;}
UINT32 AmbaCSL_CanGetMsgBufRequest(const AMBA_CAN_REG_s *pCanReg) {return pCanReg->MsgBufRequest;}
UINT32 AmbaCSL_CanGetMsgBufGrant(const AMBA_CAN_REG_s *pCanReg) {return pCanReg->MsgBufGrant;}

void AmbaCSL_CanSetMsgId(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, UINT32 d) {pCanReg->MsgCtrl[MsgBufNo].MsgBufId = d;}
void AmbaCSL_CanSetMsgCtrl(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, UINT32 d) {pCanReg->MsgCtrl[MsgBufNo].MsgBufCtrl = d;}
UINT32 AmbaCSL_CanGetMsgCtrl(const AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo) {return pCanReg->MsgCtrl[MsgBufNo].MsgBufCtrl;}

void AmbaCSL_CanEnable(AMBA_CAN_REG_s *pCanReg) {pCanReg->EnableCanc = 1;}
void AmbaCSL_CanDisable(AMBA_CAN_REG_s *pCanReg) {pCanReg->EnableCanc = 0;}
void AmbaCSL_CanReset(AMBA_CAN_REG_s *pCanReg, UINT32 d) {pCanReg->Reset.ResetAll = (UINT8)d;}
void AmbaCSL_CanWakeUp(AMBA_CAN_REG_s *pCanReg) {pCanReg->WakeUp = 1;}

UINT32 AmbaCSL_CanGetIntStatus(const AMBA_CAN_REG_s *pCanReg) {return pCanReg->IntStatus;}
void AmbaCSL_CanClearAllInterrupts(AMBA_CAN_REG_s *pCanReg) {pCanReg->IntRaw = 0x3fffff;}
UINT32 AmbaCSL_CanGetRxDoneStatus(const AMBA_CAN_REG_s *pCanReg) {return pCanReg->RxTransferDone;}
UINT32 AmbaCSL_CanGetTxDoneStatus(const AMBA_CAN_REG_s *pCanReg) {return pCanReg->TxTransferDone;}

void AmbaCSL_CanSetFilterId(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, UINT32 d) {pCanReg->IdFilter[MsgBufNo].IdFilter = d;}
void AmbaCSL_CanSetFilterMask(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, UINT32 d) {pCanReg->IdFilter[MsgBufNo].IdFilterMask = d;}

void AmbaCSL_CanDmaStart(AMBA_CAN_REG_s *pCanReg) {pCanReg->EnableRxDma = 1;}
void AmbaCSL_CanDmaSetDescAddr(AMBA_CAN_REG_s *pCanReg, UINT32 d) {pCanReg->RxDmaDescPtr = d;}
void AmbaCSL_CanDmaSetTimeout(AMBA_CAN_REG_s *pCanReg, UINT32 d) {pCanReg->RxDmaTimeOut = d;}
void AmbaCSL_CanDmaSetDescSize(AMBA_CAN_REG_s *pCanReg, UINT32 d)
{
    UINT8 value = (UINT8)(d-1U);
    pCanReg->RxDmaDescCtrl.CountTh = value;
}
void AmbaCSL_CanDmaClearDescCount(AMBA_CAN_REG_s *pCanReg) {pCanReg->RxDmaDescCtrl.Count = 0;}
void AmbaCSL_CanDmaEnableTimeout(AMBA_CAN_REG_s *pCanReg) {pCanReg->RxDmaDescCtrl.EnableTimeOut = 1;}
void AmbaCSL_CanDmaDisableTimeout(AMBA_CAN_REG_s *pCanReg) {pCanReg->RxDmaDescCtrl.EnableTimeOut = 0;}
