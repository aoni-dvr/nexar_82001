/**
 *  @file AmbaRTSL_CAN.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details CAN bus control APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaINT_Def.h"
#if defined(CONFIG_QNX)
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <sys/dispatch.h>
#include <termios.h>
#include "AmbaMMU.h"
#else
#include "AmbaRTSL_GIC.h"
#endif
#include "AmbaRTSL_PLL.h"
#include "AmbaCAN_Def.h"
#include "AmbaRTSL_CAN.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaCache.h"

#if defined(CONFIG_QNX)
#define CanMaxBufSize 4096
#define CanFdMaxBufSize 4096
static UINT8 *CanRingBuf;
static UINT8 *CanFdRingBuf;
#endif

static AMBA_RTSL_CAN_CTRL_s    AmbaRTSL_CanCtrl[AMBA_NUM_CAN_CHANNEL];

/* Call back function when completed transactions */
static void (*AmbaRTSL_CanRxIsrCallBack)(UINT32 CanCh, UINT32 MsgBufNo) = NULL;
static void (*AmbaRTSL_CanFdRxIsrCallBack)(UINT32 CanCh, UINT32 MsgBufNo) = NULL;
static void (*AmbaRTSL_CanDmaRxIsrCallBack)(UINT32 CanCh, UINT32 MsgBufNo) = NULL;
static void (*AmbaRTSL_CanTxIsrCallBack)(UINT32 CanCh, UINT32 MsgBufNo) = NULL;

void AmbaRTSL_CanSetRxIsrFunc(CanIsrCb pISR)
{
    AmbaRTSL_CanRxIsrCallBack = pISR;
}

void AmbaRTSL_CanFdSetRxIsrFunc(CanIsrCb pISR)
{
    AmbaRTSL_CanFdRxIsrCallBack = pISR;
}

void AmbaRTSL_CanSetTxIsrFunc(CanIsrCb pISR)
{
    AmbaRTSL_CanTxIsrCallBack = pISR;
}

static void CAN_FlushRxDmaDesc(const AMBA_CAN_DMA_DESC_s *Ptr, const UINT32 Range)
{
    const void *DesPtr;
    const AMBA_CAN_DMA_DESC_s *SrcPtr = Ptr;
    UINT32 Addr;
#ifdef CONFIG_ARM64
    UINT64 Rval;
#else
    UINT32 Rval;
#endif

    if (AmbaWrap_memcpy(&DesPtr, &SrcPtr, sizeof(void*)) != 0U) {
        /* Do nothing */
    }
#ifdef CONFIG_ARM64
    AmbaMisra_TypeCast64(&Rval, &DesPtr);
    if (Rval >= 0xFFFFFFFF00000000UL) {
        // address exceed 32-bit width
    }
    Addr = (UINT32)Rval;
#else
    AmbaMisra_TypeCast32(&Rval, &DesPtr);
    Addr = Rval;
#endif
    (void)AmbaCache_DataFlush(Addr, Range);
}

static void CAN_FlushRxDmaData(const UINT8 *Ptr, const UINT32 Range)
{
    const void *DesPtr;
    const UINT8 *SrcPtr = Ptr;
    UINT32 Addr;
#ifdef CONFIG_ARM64
    UINT64 Rval;
#else
    UINT32 Rval;
#endif

    if (AmbaWrap_memcpy(&DesPtr, &SrcPtr, sizeof(void*)) != 0U) {
        /* Do nothing */
    }
#ifdef CONFIG_ARM64
    AmbaMisra_TypeCast64(&Rval, &DesPtr);
    if (Rval >= 0xFFFFFFFF00000000UL) {
        // address exceed 32-bit width
    }
    Addr = (UINT32)Rval;
#else
    AmbaMisra_TypeCast32(&Rval, &DesPtr);
    Addr = Rval;
#endif

    (void)AmbaCache_DataFlush(Addr, Range);
}

static void CAN_InvalidRxDmaData(const AMBA_CAN_DMA_MESSAGE_s *Ptr, const UINT32 Range)
{
    const void *DesPtr;
    const AMBA_CAN_DMA_MESSAGE_s *SrcPtr = Ptr;
    UINT32 Addr;
#ifdef CONFIG_ARM64
    UINT64 Rval;
#else
    UINT32 Rval;
#endif

    if (AmbaWrap_memcpy(&DesPtr, &SrcPtr, sizeof(void*)) != 0U) {
        /* Do nothing */
    }
#ifdef CONFIG_ARM64
    AmbaMisra_TypeCast64(&Rval, &DesPtr);
    if (Rval >= 0xFFFFFFFF00000000UL ) {
        // address exceed 32-bit width
    }
    Addr = (UINT32)Rval;
#else
    AmbaMisra_TypeCast32(&Rval, &DesPtr);
    Addr = Rval;
#endif
    (void)AmbaCache_DataInvalidate(Addr, Range);
}

static UINT32 CAN_DmaDescp2U32(const AMBA_CAN_DMA_DESC_s *Ptr)
{
    const void *DesPtr;
    const AMBA_CAN_DMA_DESC_s *SrcPtr = Ptr;
    UINT32 Addr;
#ifdef CONFIG_ARM64
    UINT64 Rval;
#else
    UINT32 Rval;
#endif

    if (AmbaWrap_memcpy(&DesPtr, &SrcPtr, sizeof(void*)) != 0U) {
        /* Do nothing */
    }
#ifdef CONFIG_ARM64
    AmbaMisra_TypeCast64(&Rval, &DesPtr);
    if (Rval >= 0xFFFFFFFF00000000UL) {
        // address exceed 32-bit width
    }
    Addr = (UINT32)Rval;
#else
    AmbaMisra_TypeCast32(&Rval, &DesPtr);
    Addr = Rval;
#endif
    return Addr;
}


static const UINT32 AmbaCanIsrStatus[AMBA_NUM_CAN_STATUS] = {
    [AMBA_CAN_STATUS_BUS_OFF]               = 0x000001,
    [AMBA_CAN_STATUS_ERR_PASSIVE]           = 0x000002,
    [AMBA_CAN_STATUS_ACK_ERR]               = 0x000004,
    [AMBA_CAN_STATUS_FORM_ERR]              = 0x000008,
    [AMBA_CAN_STATUS_CRC_ERR]               = 0x000010,
    [AMBA_CAN_STATUS_STUFF_ERR]             = 0x000020,
    [AMBA_CAN_STATUS_BIT_ERR]               = 0x000040,
    [AMBA_CAN_STATUS_TIMEOUT]               = 0x000080,
    [AMBA_CAN_STATUS_RX_OVERFLOW]           = 0x000100,
    [AMBA_CAN_STATUS_RX_DONE]               = 0x000200,
    [AMBA_CAN_STATUS_TX_DONE]               = 0x000400,
    [AMBA_CAN_STATUS_TIMER_WRAP]            = 0x000800,
    [AMBA_CAN_STATUS_WAKE_UP]               = 0x001000,
    [AMBA_CAN_STATUS_RETRY_FAIL]            = 0x002000,
    [AMBA_CAN_STATUS_RX_DONE_TIMEOUT]       = 0x004000,
    [AMBA_CAN_STATUS_TX_DONE_TIMEOUT]       = 0x008000,
    [AMBA_CAN_STATUS_RX_DMA_DESC_DONE]      = 0x010000,
    [AMBA_CAN_STATUS_RX_DMA_DONE]           = 0x020000,
    [AMBA_CAN_STATUS_RX_DMA_TIMEOUT]        = 0x040000,
    [AMBA_CAN_STATUS_RX_DMA_GET_SPECIAL_ID] = 0x080000,
    [AMBA_CAN_STATUS_RX_DMA_GET_RTR]        = 0x100000,
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    [AMBA_CAN_STATUS_FD_STUFF_ERR]          = 0x200000,
    [AMBA_CAN_STATUS_AUTO_TX_DONE]          = 0x400000,
    [AMBA_CAN_STATUS_AUTO_RTR_MATCH]        = 0x800000
#else
    [AMBA_CAN_STATUS_FD_STUFF_ERR]          = 0x200000
#endif
};

static UINT8 CAN_UInt2UInt8(UINT32 Size)
{
    UINT32 Tmp32 = Size;
    UINT8 Tmp8;

    if (AmbaWrap_memcpy(&Tmp8, &Tmp32, 1) != 0U) {
        /* Do nothing */
    }

    return Tmp8;

}

static AMBA_CAN_DMA_MESSAGE_s* CAN_U8p2DmaMsgp(const UINT8* Ptr)
{
    const UINT8 *SrcPtr = Ptr;
    AMBA_CAN_DMA_MESSAGE_s *DesPtr;

    if (AmbaWrap_memcpy(&DesPtr, &SrcPtr, sizeof(UINT8*)) != 0U) {
        /* Do nothing */
    }

    return DesPtr;
}

static AMBA_CAN_FD_DMA_MESSAGE_s* CAN_U8p2FdDmaMsgp(const UINT8* Ptr)
{
    const UINT8 *SrcPtr = Ptr;
    AMBA_CAN_FD_DMA_MESSAGE_s *DesPtr;

    if (AmbaWrap_memcpy(&DesPtr, &SrcPtr, sizeof(UINT8*)) != 0U) {
        /* Do nothing */
    }

    return DesPtr;
}

static AMBA_CAN_MSG_s* CAN_U8p2Msgp(const UINT8* Ptr)
{
    const UINT8 *SrcPtr = Ptr;
    AMBA_CAN_MSG_s *DesPtr;

    if (AmbaWrap_memcpy(&DesPtr, &SrcPtr, sizeof(UINT8*)) != 0U) {
        /* Do nothing */
    }

    return DesPtr;
}

static AMBA_CAN_FD_MSG_s* CAN_U8p2FdMsgp(const UINT8* Ptr)
{
    const UINT8 *SrcPtr = Ptr;
    AMBA_CAN_FD_MSG_s *DesPtr;

    if (AmbaWrap_memcpy(&DesPtr, &SrcPtr, sizeof(UINT8*)) != 0U) {
        /* Do nothing */
    }

    return DesPtr;
}

static AMBA_CAN_DMA_DESC_s* CAN_U8p2DmaDescp(const UINT8* Ptr)
{
    const UINT8 *SrcPtr = Ptr;
    AMBA_CAN_DMA_DESC_s *DesPtr;

    if (AmbaWrap_memcpy(&DesPtr, &SrcPtr, sizeof(UINT8*)) != 0U) {
        /* Do nothing */
    }

    return DesPtr;
}

static UINT32 CAN_U8p2Uint32(const UINT8* Ptr)
{
    const void *DesPtr;
    const UINT8 *SrcPtr = Ptr;
    UINT32 Addr;
#ifdef CONFIG_ARM64
    UINT64 Rval;
#else
    UINT32 Rval;
#endif

    if (AmbaWrap_memcpy(&DesPtr, &SrcPtr, sizeof(UINT8*)) != 0U) {
        /* Do nothing */
    }

#ifdef CONFIG_ARM64
    AmbaMisra_TypeCast64(&Rval, &DesPtr);
    if (Rval >= 0xFFFFFFFF00000000UL) {
        // address exceed 32-bit width
    }
    Addr = (UINT32)Rval;
#else
    AmbaMisra_TypeCast32(&Rval, &DesPtr);
    Addr = Rval;
#endif
    return Addr;
}

UINT8  AmbaRTSL_CanGetFdMode(UINT32 CanCh)
{
    return AmbaRTSL_CanCtrl[CanCh].EnableCanFd;
}

UINT32 AmbaRTSL_CanTxTransfer(UINT32 CanCh, UINT32 MsgBufNo, const AMBA_CAN_MSG_s *pMessage)
{
    UINT32 RetStatus;
    AMBA_CAN_REG_s  *pCanReg = pAmbaCAN_Reg[CanCh];

    /* Check if the message buffer request is grant or not */
    if (AmbaCSL_CanRequestMsgBuf(pCanReg, MsgBufNo) > 0) { /* if grant to access buffer */
        /* Config message buffer */
        AmbaCSL_CanSetMsgBufId(pCanReg, MsgBufNo, pMessage->Id, pMessage->Extension);
        AmbaCSL_CanSetMsgBufPriority(pCanReg, MsgBufNo, pMessage->Priority);
        (void) AmbaCSL_CanSetMsgBufCtrl(pCanReg, MsgBufNo, pMessage);
        if (pMessage->RemoteTxReq == 0U) {
            (void) AmbaCSL_CanSetMsgBufData(pCanReg, MsgBufNo, pMessage->Data, pMessage->DataLengthCode);
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            AmbaCSL_CanSetAutoAnswerFlag(pCanReg, pMessage->AutoAnswer);
#endif
        }

        /* Set message buffer config done */
        AmbaCSL_CanConfigDone(pCanReg, MsgBufNo);

        if (0x0U != (AmbaRTSL_CanCtrl[CanCh].EnableCanFd)) {
            // enable CAN-FD but receive CAN-2.0 packet, it not allowed due to HW bug
            // still reserve above code for bug-free HW case.
            RetStatus = CAN_ERR_MIX_FORMAT;
        } else {
            RetStatus = CAN_ERR_NONE;
        }

    } else {
        RetStatus = CAN_ERR_NO_TX_BUFFER;
    }

    return RetStatus;
}

UINT32 AmbaRTSL_CanFdTxTransfer(UINT32 CanCh, UINT32 MsgBufNo, const AMBA_CAN_FD_MSG_s *pFdMessage)
{
    UINT32 RetStatus;
    AMBA_CAN_REG_s  *pCanReg = pAmbaCAN_Reg[CanCh];

    /* Check if the message buffer request is grant or not */
    if (AmbaCSL_CanRequestMsgBuf(pCanReg, MsgBufNo) > 0) { /* if grant to access buffer */
        /* Config message buffer */
        AmbaCSL_CanSetMsgBufId(pCanReg, MsgBufNo, pFdMessage->Id, pFdMessage->Extension);
        AmbaCSL_CanSetMsgBufPriority(pCanReg, MsgBufNo, pFdMessage->Priority);
        (void) AmbaCSL_CanFdSetMsgBufCtrl(pCanReg, MsgBufNo, pFdMessage);
        (void) AmbaCSL_CanSetMsgBufData(pCanReg, MsgBufNo, pFdMessage->Data, pFdMessage->DataLengthCode);
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        AmbaCSL_CanSetAutoAnswerFlag(pCanReg, pFdMessage->AutoAnswer);
#endif

        /* Set message buffer config done */
        AmbaCSL_CanConfigDone(pCanReg, MsgBufNo);
        RetStatus = CAN_ERR_NONE;
    } else {
        RetStatus = CAN_ERR_NO_TX_BUFFER;
    }

    return RetStatus;
}

UINT32 AmbaRTSL_CanReadOneRxTuple(UINT32 CanCh, AMBA_CAN_MSG_s *pMessage)
{
    AMBA_RTSL_CAN_RX_BUF_CTRL_s *pRxBufCtrl = &(AmbaRTSL_CanCtrl[CanCh].RxBufCtrl);
    AMBA_RTSL_CAN_RX_BUF_CTRL_s *pDmaRxBufCtrl = &(AmbaRTSL_CanCtrl[CanCh].DmaCtrl.RxBufCtrl);
    const AMBA_CAN_DMA_MESSAGE_s *pDmaMessage;
    UINT32 RetStatus;
    UINT8 Tmp = 0x0;

    if (0x0U != (AmbaRTSL_CanCtrl[CanCh].EnableRxDma)) {    /* DMA mode */
        /* In CAN-FD mode, featch the message from the CAN 2.0 Rx ring buffer. */
        /* Otherwise, featch the message from the DMA Rx ring buffer.          */
        if (0x0U != (AmbaRTSL_CanCtrl[CanCh].EnableCanFd)) {
            pDmaMessage = CAN_U8p2DmaMsgp(&pRxBufCtrl->pRingBuf[pRxBufCtrl->ReadPtr]);

            pRxBufCtrl->ReadPtr += pRxBufCtrl->PerMessageSize;

            if ((pRxBufCtrl->MaxRingBufSize - pRxBufCtrl->ReadPtr) < pRxBufCtrl->PerMessageSize) {    /* move farward Read Pointer */
                pRxBufCtrl->ReadPtr = 0;
            }
        } else {
            pDmaMessage = CAN_U8p2DmaMsgp(&pDmaRxBufCtrl->pRingBuf[pDmaRxBufCtrl->ReadPtr]);

            pDmaRxBufCtrl->ReadPtr += pDmaRxBufCtrl->PerMessageSize;
            if ((pDmaRxBufCtrl->MaxRingBufSize - pDmaRxBufCtrl->ReadPtr) < pDmaRxBufCtrl->PerMessageSize) {    /* move farward Read Pointer */
                pDmaRxBufCtrl->ReadPtr = 0;
            }
        }

        pMessage->Extension   = (UINT8)((pDmaMessage->IdCtrl & CAN_MSG_BUF_ID_REG_MSGEXT_MASK) >> CAN_MSG_BUF_ID_REG_MSGEXT_SHIFT);
        Tmp = (pMessage->Extension ^ 1U);
        pMessage->Id          = ((pDmaMessage->IdCtrl & CAN_MSG_BUF_ID_REG_MSGID_MASK) >> ((UINT8)18U * Tmp));
        pMessage->DataLengthCode  = (UINT8)(pDmaMessage->MsgCtrl & CAN_MBC_REG_DLC_MASK);   /* DLC */
        pMessage->RemoteTxReq = (UINT8)((pDmaMessage->MsgCtrl & CAN_MBC_REG_RTR_MASK) >> CAN_MBC_REG_RTR_SHIFT);   /* RTR */
        if (pMessage->RemoteTxReq == 0U) {
            if (AmbaWrap_memcpy(pMessage->Data, pDmaMessage->Data, 8) != 0U) {
                /* Do nothing */
            }
        }

    } else { /* non-DMA mode */
        UINT32 MsgSize = (UINT32)sizeof(AMBA_CAN_MSG_s);
        /* fetch one tuple data from the Rx Ring Buffer */
        if (AmbaWrap_memcpy(pMessage, &pRxBufCtrl->pRingBuf[pRxBufCtrl->ReadPtr], MsgSize) != 0U) {
            /* Do nothing */
        }

        pRxBufCtrl->ReadPtr += MsgSize;
        if ((pRxBufCtrl->MaxRingBufSize - pRxBufCtrl->ReadPtr) < MsgSize) {    /* move farward Read Pointer */
            pRxBufCtrl->ReadPtr = 0;
        }

    }

    // enable CAN-FD but receive CAN-2.0 packet, it not allowed due to HW bug
    // still reserve above code for bug-free HW case.
    if (0x0U != (AmbaRTSL_CanCtrl[CanCh].EnableCanFd)) {
        RetStatus = CAN_ERR_MIX_FORMAT;
    } else {
        RetStatus = CAN_ERR_NONE;
    }

    return RetStatus;
}

UINT32 AmbaRTSL_CanFdReadOneRxTuple(UINT32 CanCh, AMBA_CAN_FD_MSG_s *pFdMessage)
{
    AMBA_RTSL_CAN_FD_RX_BUF_CTRL_s *pFdRxBufCtrl = &(AmbaRTSL_CanCtrl[CanCh].FDRxBufCtrl);
    AMBA_RTSL_CAN_RX_BUF_CTRL_s *pDmaRxBufCtrl = &(AmbaRTSL_CanCtrl[CanCh].DmaCtrl.RxBufCtrl);
    const AMBA_CAN_FD_DMA_MESSAGE_s *pDmaMessage;
    UINT32 RetStatus;
    UINT8 Tmp = 0;

    if (0x0U != (AmbaRTSL_CanCtrl[CanCh].EnableRxDma)) {    /* DMA mode */
        do {
            pDmaMessage = CAN_U8p2FdDmaMsgp(&pDmaRxBufCtrl->pRingBuf[pDmaRxBufCtrl->ReadPtr]);
            pDmaRxBufCtrl->ReadPtr += pDmaRxBufCtrl->PerMessageSize;
            if ((pDmaRxBufCtrl->MaxRingBufSize - pDmaRxBufCtrl->ReadPtr) < pDmaRxBufCtrl->PerMessageSize) {    /* move farward Read Pointer */
                pDmaRxBufCtrl->ReadPtr = 0;
            }
        } while ((pDmaMessage->MsgCtrl & CAN_MBC_REG_EDL_MASK) == 0U);

        pFdMessage->Extension = (UINT8)((pDmaMessage->IdCtrl & CAN_MSG_BUF_ID_REG_MSGEXT_MASK) >> CAN_MSG_BUF_ID_REG_MSGEXT_SHIFT);
        Tmp = pFdMessage->Extension ^ 1U;
        pFdMessage->Id = ((pDmaMessage->IdCtrl & CAN_MSG_BUF_ID_REG_MSGID_MASK) >> (18U * Tmp));
        pFdMessage->DataLengthCode = (UINT8)(pDmaMessage->MsgCtrl & CAN_MBC_REG_DLC_MASK);          /* DLC */
        pFdMessage->FlexibleDataRate = 1;                                   /* EDL */
        pFdMessage->BitRateSwitch = (UINT8)((pDmaMessage->MsgCtrl & CAN_MBC_REG_BRS_MASK) >> CAN_MBC_REG_BRS_SHIFT);       /* BRS */
        pFdMessage->ErrorStateIndicator = (UINT8)((pDmaMessage->MsgCtrl & CAN_MBC_REG_ESI_MASK) >> CAN_MBC_REG_ESI_SHIFT); /* ESI */
        if (AmbaWrap_memcpy(pFdMessage->Data, pDmaMessage->Data, 64) != 0U) {
            /* Do nothing */
        }

    } else { /* non-DMA mode */
        UINT32 MsgSize = (UINT32)sizeof(AMBA_CAN_FD_MSG_s);
        /* fetch one tuple data from the Rx Ring Buffer */
        if (AmbaWrap_memcpy(pFdMessage, &pFdRxBufCtrl->pRingBuf[pFdRxBufCtrl->ReadPtr], MsgSize) != 0U) {
            /* Do nothing */
        }

        pFdRxBufCtrl->ReadPtr += MsgSize;
        if ((pFdRxBufCtrl->MaxRingBufSize - pFdRxBufCtrl->ReadPtr) < MsgSize) {    /* move farward Read Pointer */
            pFdRxBufCtrl->ReadPtr = 0;
        }
    }
    RetStatus = CAN_ERR_NONE;

    return RetStatus;
}

static void CAN_SetIdFilter(AMBA_CAN_REG_s  *pCanReg, UINT32 MsgBufType, UINT8 NumIdFilter, const AMBA_CAN_FILTER_s *pIdFilter)
{
    UINT32 i, j = 0;
    UINT32 TmpBufType = MsgBufType;


    for (i = 0; i < AMBA_NUM_CAN_MSG_BUF; i++) {
        /* Check if the message buffer request is grant or not */
        if (AmbaCSL_CanRequestMsgBuf(pCanReg, i) > 0) {
            /* Set filter ID and mask */
            if ((TmpBufType & 0x1U) != 0U) {
                /* Note, for unknown hardware issue, it needs to set the ID filter of Tx message buffers. */
                AmbaCSL_CanSetIdFilter(pCanReg, i, 0x0, 0x1fffffffU);
                /* Set message buffer config done */
                AmbaCSL_CanConfigDone(pCanReg, i);
            } else {
                if (j < NumIdFilter) {
                    /* Check if base ID format (11 bits) and set ID Filter */
                    if (pIdFilter[j].IdFilter <= 0x7ffU) {
                        UINT32 TmpId = pIdFilter[j].IdFilter << 18U;
                        UINT32 TmpIdMask = pIdFilter[j].IdMask << 18U;
                        AmbaCSL_CanSetIdFilter(pCanReg, i, TmpId, TmpIdMask);
                    } else {
                        AmbaCSL_CanSetIdFilter(pCanReg, i, pIdFilter[j].IdFilter, pIdFilter[j].IdMask);
                    }

                    /* Set message buffer config done */
                    AmbaCSL_CanConfigDone(pCanReg, i);
#if !defined(CONFIG_SOC_CV5) && !defined(CONFIG_SOC_CV52) // Fixed in CV5, no workaround needed.
                } else {
                    /* Due to HW issue, we need to set assign buffer map as below
                     *
                     *           --------
                     *  buf_0   |        |     Message buffer to receive filtered packets
                     *           --------      filter_en = 1
                     *          |        |     IdFilterMask=setmask
                     *           --------      IdFilter.Bits.IdFilter=setid
                     *  buf_N   |        |
                     *           --------
                     *       +++++++++++++++++++++++++++++
                     *           --------
                     *  buf_N+1 |        |     Message buffer not to receive unfiltered packets
                     *           --------      filter_en = 1
                     *          |        |     IdFilterMask = 0
                     *           --------      ==> When mask is set to 0, it can be treated as buffer without filter
                     *  buf_31  |        |
                     *           --------
                     *
                     *  Not that although filter message won't be received by specific buf, it can be by other unfilter buf.
                     *  To totally filter the message, driver should not configure unfiltered(disable) buf.
                     *
                     * */
                    AmbaCSL_CanSetIdFilter(pCanReg, i, 0, 0);
#endif
                }
                j++;
            }


        }
        TmpBufType >>= 1U;
    }
}

static void CAN_RxTransfer(UINT32 CanCh, UINT32 MsgBufNo, UINT32 *IsFDMsg)
{
    AMBA_CAN_REG_s  *pCanReg = pAmbaCAN_Reg[CanCh];
    AMBA_RTSL_CAN_RX_BUF_CTRL_s *pRxBufCtrl = &(AmbaRTSL_CanCtrl[CanCh].RxBufCtrl);
    AMBA_RTSL_CAN_FD_RX_BUF_CTRL_s *pFdRxBufCtrl = &(AmbaRTSL_CanCtrl[CanCh].FDRxBufCtrl);
    AMBA_CAN_MSG_s *pMessage = NULL;
    AMBA_CAN_FD_MSG_s *pFdMessage = NULL;
    UINT32 MsgCtrlData = AmbaCSL_CanGetMsgCtrl(pCanReg, MsgBufNo);
    UINT32 MsgId = AmbaCSL_CanGetMsgId(pCanReg, MsgBufNo);
    UINT8 MsgExtention = AmbaCSL_CanGetMsgExtention(pCanReg, MsgBufNo);
    UINT32 ExOffset;
    UINT8 SizeCode;

    if (MsgExtention != 0U) {
        ExOffset = 0;
    } else {
        ExOffset = 18;
    }

    SizeCode = CAN_UInt2UInt8(MsgCtrlData & 0x0fU);

    if ((MsgCtrlData & 0x40U) != 0U) {
        UINT32 BRS = MsgCtrlData & 0x20U;
        UINT32 ESI = MsgCtrlData & 0x10U;

        BRS = BRS >> 5U;
        ESI = ESI >> 4U;
        /* Fill the CAN FD message */
        pFdMessage = CAN_U8p2FdMsgp(&(pFdRxBufCtrl->pRingBuf[pFdRxBufCtrl->WritePtr]));
        pFdMessage->Id = MsgId >> ExOffset;                                 /* ID */
        pFdMessage->Extension = MsgExtention;                               /* IDE */
        pFdMessage->DataLengthCode = SizeCode;                              /* DLC */
        pFdMessage->FlexibleDataRate = 1;                                   /* EDL */
        if (BRS != 0U) {                                                    /* BRS */
            pFdMessage->BitRateSwitch = 1;
        } else {
            pFdMessage->BitRateSwitch = 0;
        }
        if (ESI != 0U) {                                                    /* ESI */
            pFdMessage->ErrorStateIndicator = 1;
        } else {
            pFdMessage->ErrorStateIndicator = 0;
        }
        AmbaCSL_CanGetRxMsgBufData(pCanReg, MsgBufNo, pFdMessage->Data, pFdMessage->DataLengthCode);

        pFdRxBufCtrl->WritePtr += pFdRxBufCtrl->PerMessageSize;
        if ((pFdRxBufCtrl->MaxRingBufSize - pFdRxBufCtrl->WritePtr) < pFdRxBufCtrl->PerMessageSize) { /* move farward Write Pointer */
            pFdRxBufCtrl->WritePtr = 0;
        }

        *IsFDMsg = 1;
    } else {
        UINT32 RTR = MsgCtrlData & 0x80U;

        RTR = RTR >> 7U;
        /* Fill the CAN 2.0 message */
        pMessage = CAN_U8p2Msgp(&(pRxBufCtrl->pRingBuf[pRxBufCtrl->WritePtr]));
        pMessage->Id = MsgId >> ExOffset;                                   /* ID */
        pMessage->Extension = MsgExtention;                                 /* IDE */
        pMessage->DataLengthCode = SizeCode;                                /* DLC */
        if (RTR != 0U) {                                                    /* RTR */
            pMessage->RemoteTxReq = 1;
        } else {
            pMessage->RemoteTxReq = 0;
        }
        if (pMessage->RemoteTxReq == 0U) {
            AmbaCSL_CanGetRxMsgBufData(pCanReg, MsgBufNo, pMessage->Data, pMessage->DataLengthCode);
        }

        pRxBufCtrl->WritePtr += pRxBufCtrl->PerMessageSize;
        if ((pRxBufCtrl->MaxRingBufSize - pRxBufCtrl->WritePtr) < pRxBufCtrl->PerMessageSize) { /* move farward Write Pointer */
            pRxBufCtrl->WritePtr = 0;
        }

        *IsFDMsg = 0;
    }

    /* Set config done to release the Rx buffer resource */
    AmbaCSL_CanConfigDone(pCanReg, MsgBufNo);
}

static void CAN_RxDmaRestart(UINT32 CanCh)
{
    AMBA_CAN_REG_s  *pCanReg = pAmbaCAN_Reg[CanCh];
    AMBA_RTSL_CAN_DMA_CTRL_s *pDmaCtrl = &(AmbaRTSL_CanCtrl[CanCh].DmaCtrl);
    const AMBA_RTSL_CAN_RX_BUF_CTRL_s *pDmaRxBufCtrl = &(AmbaRTSL_CanCtrl[CanCh].DmaCtrl.RxBufCtrl);
    AMBA_CAN_DMA_DESC_s *pNextDmaDesc;
    UINT32 i;
    UINT32 Offset[2];
#if defined(CONFIG_QNX)
    ULONG Src;
#endif

    /* Reset DMA engine */
    AmbaCSL_CanResetRxDma(pCanReg, 1);
    AmbaCSL_CanResetRxDma(pCanReg, 0);

    AmbaCSL_CanDmaClearDescCount(pCanReg);
#if defined(CONFIG_QNX)
    AmbaMisra_TypeCast(&Src, &pDmaCtrl->pDmaDesc[pDmaCtrl->NextDmaDesc]);
    AmbaMMU_VirtToPhys(Src, &Src);
    AmbaCSL_CanDmaSetDescAddr(pCanReg, Src);
#else
    AmbaCSL_CanDmaSetDescAddr(pCanReg, CAN_DmaDescp2U32(pDmaCtrl->pDmaDesc[pDmaCtrl->NextDmaDesc]));
#endif

    AmbaCSL_CanDmaStart(pCanReg);

    /* Prepare the next DMA descriptor */
    pDmaCtrl->NextDmaDesc ^= 1U;

    pNextDmaDesc = pDmaCtrl->pDmaDesc[pDmaCtrl->NextDmaDesc];
    for (i = 0; i < pDmaCtrl->MaxDescSize; i++) {
        pNextDmaDesc[i].Status = 0;
    }

    Offset[0] = 0;
    Offset[1] = pDmaCtrl->MaxDescSize * pDmaRxBufCtrl->PerMessageSize;

    // flush cache memory
#if !defined(CONFIG_QNX) //not use cache memory for qnx, since we should do invalidate before flush. why?
    CAN_FlushRxDmaDesc(pDmaCtrl->pDmaDesc[pDmaCtrl->NextDmaDesc], (UINT32)sizeof(AMBA_CAN_DMA_DESC_s) * pDmaCtrl->MaxDescSize);
    CAN_FlushRxDmaData(&(pDmaRxBufCtrl->pRingBuf[Offset[pDmaCtrl->NextDmaDesc]]), pDmaCtrl->MaxDescSize * pDmaRxBufCtrl->PerMessageSize);
#endif

}

static void CAN_IsrHandler_RxDmaDescDone(UINT32 CanCh, UINT32 IsrStatus)
{
    AMBA_CAN_REG_s *pCanReg = pAmbaCAN_Reg[CanCh];
    UINT32 RxDoneStatus;
    AMBA_RTSL_CAN_RX_BUF_CTRL_s *pRxBufCtrl = &(AmbaRTSL_CanCtrl[CanCh].RxBufCtrl);
    AMBA_RTSL_CAN_RX_BUF_CTRL_s *pDmaRxBufCtrl = &(AmbaRTSL_CanCtrl[CanCh].DmaCtrl.RxBufCtrl);
    const AMBA_CAN_DMA_MESSAGE_s *pDmaMessage;
    UINT32 i;

    (void)IsrStatus;

    /* Copy CAN 2.0 message from the DMA ring buffer to the CAN 2.0 ring buffer */
    pDmaMessage = CAN_U8p2DmaMsgp(&pDmaRxBufCtrl->pRingBuf[pDmaRxBufCtrl->WritePtr]);
    // invalid cache memory
#if !defined(CONFIG_QNX) //not use cache memory for qnx, since we should do flush after invalidate. why?
    CAN_InvalidRxDmaData(pDmaMessage, (UINT32)sizeof(AMBA_CAN_DMA_MESSAGE_s));
#endif

    if ((0x0U != AmbaRTSL_CanCtrl[CanCh].EnableCanFd) && ((pDmaMessage->MsgCtrl & CAN_MBC_REG_EDL_MASK) == 0U)) {
        if (AmbaWrap_memcpy(&pRxBufCtrl->pRingBuf[pRxBufCtrl->WritePtr], &pDmaRxBufCtrl->pRingBuf[pDmaRxBufCtrl->WritePtr], pRxBufCtrl->PerMessageSize) != 0U) {
            /* Do nothing */
        }

        pRxBufCtrl->WritePtr += pRxBufCtrl->PerMessageSize;
        if ((pRxBufCtrl->MaxRingBufSize - pRxBufCtrl->WritePtr) < pRxBufCtrl->PerMessageSize) { /* move farward Write Pointer */
            pRxBufCtrl->WritePtr = 0;
        }

        pDmaRxBufCtrl->WritePtr += pDmaRxBufCtrl->PerMessageSize;
        if ((pDmaRxBufCtrl->MaxRingBufSize - pDmaRxBufCtrl->WritePtr) < pDmaRxBufCtrl->PerMessageSize) { /* move farward Write Pointer */
            pDmaRxBufCtrl->WritePtr = 0;
        }

        AmbaRTSL_CanRxIsrCallBack(CanCh, AMBA_NUM_CAN_MSG_BUF);

    } else {
        pDmaRxBufCtrl->WritePtr += pDmaRxBufCtrl->PerMessageSize;
        if ((pDmaRxBufCtrl->MaxRingBufSize - pDmaRxBufCtrl->WritePtr) < pDmaRxBufCtrl->PerMessageSize) { /* move farward Write Pointer */
            pDmaRxBufCtrl->WritePtr = 0;
        }

        AmbaRTSL_CanDmaRxIsrCallBack(CanCh, AMBA_NUM_CAN_MSG_BUF);
    }

    /* DMA done means one buffer is rx done */
    RxDoneStatus = AmbaCSL_CanGetRxDoneStatus(pCanReg);
    for (i = 0; i < AMBA_NUM_CAN_MSG_BUF; i++) {
        if ((RxDoneStatus & 0x1U) != 0U) {
            AmbaCSL_CanConfigDone(pCanReg, i);
        }
        RxDoneStatus >>= 1U;
    }
}

static void CAN_IsrHandler_RxDone(UINT32 CanCh)
{
    const AMBA_CAN_REG_s *pCanReg = pAmbaCAN_Reg[CanCh];
    UINT32 RxDoneStatus;
    UINT32 IsCanFdMsg;
    UINT32 i;

    /* Handle Rx Event */
    RxDoneStatus = AmbaCSL_CanGetRxDoneStatus(pCanReg);
    for (i = 0; i < AMBA_NUM_CAN_MSG_BUF; i++) {
        if ((RxDoneStatus & 0x1U) != 0U) {
            CAN_RxTransfer(CanCh, i, &IsCanFdMsg);

            /* Call the ISR callback function by CAN 2.0 or CAN FD */
            if (IsCanFdMsg != 0U) {
                AmbaRTSL_CanFdRxIsrCallBack(CanCh, i);
            } else {
                AmbaRTSL_CanRxIsrCallBack(CanCh, i);
            }
        }
        RxDoneStatus >>= 1U;
    }
}

static void CAN_IsrHandler(UINT32 CanCh)
{
    AMBA_CAN_REG_s *pCanReg = pAmbaCAN_Reg[CanCh];
    UINT32 TxDoneStatus;
    UINT32 IsrStatus, MsgBufType;
    UINT32 i;
#ifdef CAN_DEBUG
    AMBA_RTSL_CAN_RX_BUF_CTRL_s *pRxBufCtrl = (AMBA_RTSL_CAN_RX_BUF_CTRL_s *) &(AmbaRTSL_CanCtrl[CanCh].RxBufCtrl);
    AMBA_CAN_MSG_s *pMessage;
    UINT32 ErrStatus;
#endif

    /* Read interrupt register */
    IsrStatus = AmbaCSL_CanGetIntStatus(pCanReg);

    /* Handle Rx dma desc done interrupt */
    if ((IsrStatus & AmbaCanIsrStatus[AMBA_CAN_STATUS_RX_DMA_DESC_DONE]) != 0U) {
        CAN_IsrHandler_RxDmaDescDone(CanCh, IsrStatus);
    }

    if (0x0U != (IsrStatus & AmbaCanIsrStatus[AMBA_CAN_STATUS_RX_DMA_DONE])) {
        CAN_RxDmaRestart(CanCh);
    }

    /* Handle Rx done interrupt */
    if ((IsrStatus & AmbaCanIsrStatus[AMBA_CAN_STATUS_RX_DONE]) != 0U) {
        CAN_IsrHandler_RxDone(CanCh);
    }

    /* Handle Tx done interrupt */
    if ((IsrStatus & AmbaCanIsrStatus[AMBA_CAN_STATUS_TX_DONE]) != 0U) {
        /* Handle Tx Event */
        TxDoneStatus = AmbaCSL_CanGetTxDoneStatus(pCanReg);
        for (i = 0; i < AMBA_NUM_CAN_MSG_BUF; i++) {
            if ((TxDoneStatus & 0x1U) != 0U) {
                AmbaRTSL_CanTxIsrCallBack(CanCh, i);
            }
            TxDoneStatus >>= 1U;
        }
    }

#if defined(CAN_ENABLE_SW_SAFETY_MECHANISM)
    if (((IsrStatus & AmbaCanIsrStatus[AMBA_CAN_STATUS_ACK_ERR])     != 0U) ||
        ((IsrStatus & AmbaCanIsrStatus[AMBA_CAN_STATUS_FORM_ERR])    != 0U) ||
        ((IsrStatus & AmbaCanIsrStatus[AMBA_CAN_STATUS_CRC_ERR])     != 0U) ||
        ((IsrStatus & AmbaCanIsrStatus[AMBA_CAN_STATUS_STUFF_ERR])   != 0U) ||
        ((IsrStatus & AmbaCanIsrStatus[AMBA_CAN_STATUS_BIT_ERR])     != 0U) ||
        ((IsrStatus & AmbaCanIsrStatus[AMBA_CAN_STATUS_BUS_OFF])     != 0U) ||
        ((IsrStatus & AmbaCanIsrStatus[AMBA_CAN_STATUS_ERR_PASSIVE]) != 0U)) {
        /* Per safety manual, set CAN_SM_ERROR to enable CEHU error */
        AmbaCSL_CanSetFaultInj(pCanReg);
    }
#endif

    if ((IsrStatus & AmbaCanIsrStatus[AMBA_CAN_STATUS_RETRY_FAIL]) != 0U) {    /* Release Tx buffer resource once retry fail */
        MsgBufType = AmbaCSL_CanGetMsgBufType(pCanReg);
        TxDoneStatus = AmbaCSL_CanGetRetryFail(pCanReg) & MsgBufType;
        for (i = 0; i < AMBA_NUM_CAN_MSG_BUF; i++) {
            if ((TxDoneStatus & 0x1U) != 0U) {
                AmbaRTSL_CanTxIsrCallBack(CanCh, i);
            }
            TxDoneStatus >>= 1U;
        }
    } else {
        /* The following code segment is useful for debuging CANC error status by */
        /* adding an message with interrupt and error status into Rx ring buffer. */
#ifdef CAN_DEBUG
        ErrStatus         = AmbaCSL_CanGetErrorStatus();
        pMessage          = &(pRxBufCtrl->pRingBuf[pRxBufCtrl->WritePtr++]);
        pMessage->Id      = AmbaCSL_CanGetRxBufOverflowId();
        pMessage->Dlc     = AMBA_CAN_DATA_BYTE_6;
        pMessage->Type    = AMBA_CAN_MSG_STATUS;
        pMessage->Data[0] = IsrStatus & 0xff;
        pMessage->Data[1] = (IsrStatus >> 8) & 0x1f;

        pMessage->Data[2] = ErrStatus & 0xff;
        pMessage->Data[3] = (ErrStatus >> 8)  & 0xff;
        pMessage->Data[4] = (ErrStatus >> 16) & 0xff;
        pMessage->Data[5] = (ErrStatus >> 24) & 0xff;

        if (pRxBufCtrl->WritePtr >= pRxBufCtrl->MaxRingMsgNum) { /* move farward Write Pointer */
            pRxBufCtrl->WritePtr = 0;
        }

        if (AmbaRTSL_CanRxIsrCallBack != NULL)
            AmbaRTSL_CanRxIsrCallBack(CanCh, AMBA_NUM_CAN_MSG_BUF);   /* Note: fake the buffer ID to the buffer number */
#endif
    }

    AmbaCSL_CanClearInterrupts(pCanReg, IsrStatus);
}

static UINT32 CAN_Ch2IntID(UINT32 CanCh)
{
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    UINT32 IntID = AMBA_INT_SPI_ID039_CAN0;
#elif defined(CONFIG_SOC_CV28)
    UINT32 IntID = AMBA_INT_SPI_ID118_CANC;
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 IntID = AMBA_INT_SPI_ID39_CAN0;
#elif defined(CONFIG_SOC_CV2)
    UINT32 IntID = AMBA_INT_SPI_ID118_CAN0;
#else
#error "Unsupported chip version"
#endif

    if (CanCh == AMBA_CAN_CHANNEL0) {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        IntID = AMBA_INT_SPI_ID039_CAN0;
    } else if (CanCh == AMBA_CAN_CHANNEL1) {
        IntID = AMBA_INT_SPI_ID040_CAN1;
    } else if (CanCh == AMBA_CAN_CHANNEL2) {
        IntID = AMBA_INT_SPI_ID041_CAN2;
    } else if (CanCh == AMBA_CAN_CHANNEL3) {
        IntID = AMBA_INT_SPI_ID042_CAN3;
    } else if (CanCh == AMBA_CAN_CHANNEL4) {
        IntID = AMBA_INT_SPI_ID043_CAN4;
    } else {
        IntID = AMBA_INT_SPI_ID044_CAN5;
#elif defined(CONFIG_SOC_CV28)
        IntID = AMBA_INT_SPI_ID118_CANC;
    } else {
        IntID = AMBA_INT_SPI_ID118_CANC; // FIXME
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        IntID = AMBA_INT_SPI_ID39_CAN0;
    } else {
        IntID = AMBA_INT_SPI_ID40_CAN1;
#elif defined(CONFIG_SOC_CV2)
        IntID = AMBA_INT_SPI_ID118_CAN0;
    } else {
        IntID = AMBA_INT_SPI_ID119_CAN1;
#else
#error "Unsupported chip version"
#endif
    }

    return IntID;
}

// disable NEON registers usage in ISR
#pragma GCC push_options
#if !defined(GCOVR_CAN_ENABLE) // avoid to compile error
#pragma GCC target("general-regs-only")
#endif
#if defined(CONFIG_QNX)
static void CAN_ISR(UINT32 CanCh)
{
    UINT32 IntID = CAN_Ch2IntID(CanCh);
    struct sigevent event;
    int iid;

    int mask = 0x00000001;
    ThreadCtl(_NTO_TCTL_RUNMASK, (void *)mask);
    ThreadCtl( _NTO_TCTL_IO, 0 );

    SIGEV_INTR_INIT(&event);
    iid = InterruptAttachEvent(IntID, &event, 0U);
    InterruptUnmask(IntID, iid);

    while(1) {
        InterruptWait(0, NULL);
        CAN_IsrHandler(CanCh);
        InterruptUnmask(IntID, iid);
    }

    return NULL;
#else
static void CAN_ISR(UINT32 IntID, UINT32 UserArg)
{
    (void) UserArg;

    switch(IntID) {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    case AMBA_INT_SPI_ID039_CAN0:
        CAN_IsrHandler(AMBA_CAN_CHANNEL0);
        break;
    case AMBA_INT_SPI_ID040_CAN1:
        CAN_IsrHandler(AMBA_CAN_CHANNEL1);
        break;
    case AMBA_INT_SPI_ID041_CAN2:
        CAN_IsrHandler(AMBA_CAN_CHANNEL2);
        break;
    case AMBA_INT_SPI_ID042_CAN3:
        CAN_IsrHandler(AMBA_CAN_CHANNEL3);
        break;
    case AMBA_INT_SPI_ID043_CAN4:
        CAN_IsrHandler(AMBA_CAN_CHANNEL4);
        break;
    default: //AMBA_INT_SPI_ID044_CAN5
        CAN_IsrHandler(AMBA_CAN_CHANNEL5);
        break;
#elif defined(CONFIG_SOC_CV28)
    case AMBA_INT_SPI_ID118_CANC:
        CAN_IsrHandler(AMBA_CAN_CHANNEL0);
        break;
    default: //FIXME
        CAN_IsrHandler(AMBA_CAN_CHANNEL0);
        break;
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    case AMBA_INT_SPI_ID39_CAN0:
        CAN_IsrHandler(AMBA_CAN_CHANNEL0);
        break;
    default: //AMBA_INT_SPI_ID40_CAN1
        CAN_IsrHandler(AMBA_CAN_CHANNEL1);
        break;
#else
#error "Unsupported chip version"
#endif
    }
#endif
}

// function for gcovr
#if defined(GCOVR_CAN_ENABLE)
void can_ctest_isr(UINT32 IntID)
{
    CAN_ISR(IntID, 0);
}

#endif

#pragma GCC pop_options
#if !defined(CONFIG_QNX)
static inline UINT32 CAN_CheckFD(const AMBA_CAN_CONFIG_s *pCanConfig)
{
    UINT32 Ret;
    if ((pCanConfig->pFdRxRingBuf != NULL) && (pCanConfig->FdMaxRxRingBufSize > 0U)) {
        Ret = 1;
    } else {
        Ret = 0;
    }

    return Ret;
}
#endif

static void CAN_DmaConfig(UINT32 CanCh, const AMBA_CAN_CONFIG_s *pCanConfig)
{
    AMBA_CAN_REG_s *pCanReg = pAmbaCAN_Reg[CanCh];
    AMBA_RTSL_CAN_DMA_CTRL_s *pDmaCtrl = &(AmbaRTSL_CanCtrl[CanCh].DmaCtrl);
    AMBA_RTSL_CAN_RX_BUF_CTRL_s *pDmaRxBufCtrl = &(AmbaRTSL_CanCtrl[CanCh].DmaCtrl.RxBufCtrl);
    UINT32 i, Offset[2], MaxRxRingBufSize;
    UINT8 *pRxRingBuf;
#if defined(CONFIG_QNX)
    ULONG Src;
    UINT8 *buf;
    AMBA_CAN_DMA_DESC_s *desc;
#endif

    /* Set the DMA ISR callback function and the per message size */
    if (0x0U != AmbaRTSL_CanCtrl[CanCh].EnableCanFd) {
#if defined(CONFIG_QNX) // get data buffer from the driver, instead of app.
        MaxRxRingBufSize = CanFdMaxBufSize;
        pRxRingBuf = CanFdRingBuf;
#else
        MaxRxRingBufSize = pCanConfig->FdMaxRxRingBufSize;
        pRxRingBuf = pCanConfig->pFdRxRingBuf;
#endif
        AmbaRTSL_CanDmaRxIsrCallBack = AmbaRTSL_CanFdRxIsrCallBack;
        pDmaRxBufCtrl->PerMessageSize = (UINT32)sizeof(AMBA_CAN_FD_DMA_MESSAGE_s);
    } else {
#if defined(CONFIG_QNX) // get data buffer from the driver, instead of app.
        MaxRxRingBufSize = CanMaxBufSize;
        pRxRingBuf = CanRingBuf;
#else
        MaxRxRingBufSize = pCanConfig->MaxRxRingBufSize;
        pRxRingBuf = pCanConfig->pRxRingBuf;
#endif
        AmbaRTSL_CanDmaRxIsrCallBack = AmbaRTSL_CanRxIsrCallBack;
        pDmaRxBufCtrl->PerMessageSize = (UINT32)sizeof(AMBA_CAN_DMA_MESSAGE_s);
    }

    // reset r/w index
    pDmaRxBufCtrl->WritePtr = 0;
    pDmaRxBufCtrl->ReadPtr = 0;

    /*   Split the Rx ring buffer into the DMA descriptor chain and the DMA data buffer   */
    /*                               Entire Rx ring buffer                                */
    /* |<==============================================================================>| */
    /*                                            v                                       */
    /*    DMA descriptor chain [0]     DMA descriptor chain [1]       DMA data buffer     */
    /* |<==========================>|<==========================>|<=========><=========>| */
    /*               |                            |                     |         |       */
    /*               |                            \---------------------+---------/       */
    /*               \--------------------------------------------------/                 */
    if (AmbaWrap_memset(pRxRingBuf, 0, MaxRxRingBufSize) != 0U) {
        /* Do nothing */
    }
    pDmaCtrl->MaxDescSize = (MaxRxRingBufSize >> 1) / ((UINT32)sizeof(AMBA_CAN_DMA_DESC_s) + pDmaRxBufCtrl->PerMessageSize);
    pDmaRxBufCtrl->MaxRingBufSize = (pDmaCtrl->MaxDescSize << 1) * pDmaRxBufCtrl->PerMessageSize;
    pDmaCtrl->pDmaDesc[0] = CAN_U8p2DmaDescp(pRxRingBuf);
    pDmaCtrl->pDmaDesc[1] = CAN_U8p2DmaDescp(&pRxRingBuf[(sizeof(AMBA_CAN_DMA_DESC_s) * pDmaCtrl->MaxDescSize)]);

    pDmaRxBufCtrl->pRingBuf = (&pRxRingBuf[((UINT32)sizeof(AMBA_CAN_DMA_DESC_s) * (pDmaCtrl->MaxDescSize << 1))]);


    /* Compose the DMA descriptor chain */
    Offset[0] = 0;
    Offset[1] = pDmaCtrl->MaxDescSize * pDmaRxBufCtrl->PerMessageSize;
    for (i = 0; i < (pDmaCtrl->MaxDescSize - 1U); i++) {
#if defined(CONFIG_QNX)
        buf = &(pDmaRxBufCtrl->pRingBuf[Offset[0]]);
        AmbaMisra_TypeCast(&Src, &buf);
        AmbaMMU_VirtToPhys(Src, &Src);
        pDmaCtrl->pDmaDesc[0][i].DestAddr = Src;
        desc = &(pDmaCtrl->pDmaDesc[0][i + 1U]);
        AmbaMisra_TypeCast(&Src, &desc);
        AmbaMMU_VirtToPhys(Src, &Src);
        pDmaCtrl->pDmaDesc[0][i].pNextDesc = Src;
        buf = &(pDmaRxBufCtrl->pRingBuf[Offset[1]]);
        AmbaMisra_TypeCast(&Src, &buf);
        AmbaMMU_VirtToPhys(Src, &Src);
        pDmaCtrl->pDmaDesc[1][i].DestAddr = Src;
        desc = &(pDmaCtrl->pDmaDesc[1][i + 1U]);
        AmbaMisra_TypeCast(&Src, &desc);
        AmbaMMU_VirtToPhys(Src, &Src);
        pDmaCtrl->pDmaDesc[1][i].pNextDesc = Src;

#else
        pDmaCtrl->pDmaDesc[0][i].DestAddr = CAN_U8p2Uint32(&(pDmaRxBufCtrl->pRingBuf[Offset[0]]));
        pDmaCtrl->pDmaDesc[0][i].pNextDesc = &(pDmaCtrl->pDmaDesc[0][i + 1U]);
        pDmaCtrl->pDmaDesc[1][i].DestAddr = CAN_U8p2Uint32(&(pDmaRxBufCtrl->pRingBuf[Offset[1]]));
        pDmaCtrl->pDmaDesc[1][i].pNextDesc = &(pDmaCtrl->pDmaDesc[1][i + 1U]);
#endif
        Offset[0] += pDmaRxBufCtrl->PerMessageSize;
        Offset[1] += pDmaRxBufCtrl->PerMessageSize;
    }
#if defined(CONFIG_QNX)
    buf = &(pDmaRxBufCtrl->pRingBuf[Offset[0]]);
    AmbaMisra_TypeCast(&Src, &buf);
    AmbaMMU_VirtToPhys(Src, &Src);
    pDmaCtrl->pDmaDesc[0][i].DestAddr = Src;
    pDmaCtrl->pDmaDesc[0][i].pNextDesc = NULL;
    buf = &(pDmaRxBufCtrl->pRingBuf[Offset[1]]);
    AmbaMisra_TypeCast(&Src, &buf);
    AmbaMMU_VirtToPhys(Src, &Src);
    pDmaCtrl->pDmaDesc[1][i].DestAddr = Src;
    pDmaCtrl->pDmaDesc[1][i].pNextDesc = NULL;
#else
    pDmaCtrl->pDmaDesc[0][i].DestAddr = CAN_U8p2Uint32(&(pDmaRxBufCtrl->pRingBuf[Offset[0]]));
    pDmaCtrl->pDmaDesc[0][i].pNextDesc = NULL;
    pDmaCtrl->pDmaDesc[1][i].DestAddr = CAN_U8p2Uint32(&(pDmaRxBufCtrl->pRingBuf[Offset[1]]));
    pDmaCtrl->pDmaDesc[1][i].pNextDesc = NULL;
#endif

#if !defined(CONFIG_QNX) //not use cache memory for qnx, since we should do invalidate before flush. why?
    // flush cache memory
    CAN_FlushRxDmaDesc(CAN_U8p2DmaDescp(pRxRingBuf), (UINT32)sizeof(AMBA_CAN_DMA_DESC_s) * (pDmaCtrl->MaxDescSize << 1));
    CAN_FlushRxDmaData(pDmaRxBufCtrl->pRingBuf, pDmaCtrl->MaxDescSize * (pDmaRxBufCtrl->PerMessageSize << 1));
#endif

    pDmaCtrl->NextDmaDesc = 1;

    // reset controller to make sure DMA engine is off, or we can't reconfigure descriptor
    // especially for start/stop case
    AmbaCSL_CanResetRxDma(pCanReg, 1);
    AmbaCSL_CanResetRxDma(pCanReg, 0);
    AmbaCSL_CanDmaClearDescCount(pCanReg);

    /* Fill DMA related registers */
    AmbaCSL_CanDmaSetDescSize(pCanReg, pDmaCtrl->MaxDescSize);
#if defined(CONFIG_QNX)
    AmbaMisra_TypeCast(&Src, &pDmaCtrl->pDmaDesc[0]);
    AmbaMMU_VirtToPhys(Src, &Src);
    AmbaCSL_CanDmaSetDescAddr(pCanReg, Src);
#else
    AmbaCSL_CanDmaSetDescAddr(pCanReg, CAN_DmaDescp2U32(pDmaCtrl->pDmaDesc[0]));
#endif
    AmbaCSL_CanDmaDisableTimeout(pCanReg);
    AmbaCSL_CanDmaStart(pCanReg);

}

UINT32 AmbaRTSL_CanConfig(UINT32 CanCh, const AMBA_CAN_CONFIG_s *pCanConfig, UINT32 MsgBufType)
{
    AMBA_CAN_REG_s  *pCanReg = pAmbaCAN_Reg[CanCh];
    AMBA_RTSL_CAN_RX_BUF_CTRL_s *pRxBufCtrl = &(AmbaRTSL_CanCtrl[CanCh].RxBufCtrl);
    AMBA_RTSL_CAN_FD_RX_BUF_CTRL_s *pFdRxBufCtrl = &(AmbaRTSL_CanCtrl[CanCh].FDRxBufCtrl);
    AMBA_RTSL_CAN_ID_FILTER_CTRL_s *pIdFilterCtrl = &(AmbaRTSL_CanCtrl[CanCh].IdFilterCtrl);

#if defined(CONFIG_QNX)
    // create share non-cache memory
    /* Map in a shared memory region */
    int p_rfd;

    /* Create shared memory object and set its size */
    p_rfd = shm_open("/CAN", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (p_rfd == -1) {
        /* Handle error */;
        printf("error");
    }

    if (ftruncate(p_rfd, CanMaxBufSize) == -1) {
        /* Handle error */;
        printf("error");
    }

    /* Map shared memory object */
    CanRingBuf = mmap(NULL, CanMaxBufSize,
                      PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_SHARED, p_rfd, 0);
    if (CanRingBuf == MAP_FAILED) {
        /* Handle error */;
        printf("error");
    }

    /* Set Rx Ring Buffer */
    pRxBufCtrl->ReadPtr  = 0;                                   /* Read pointer to the Ring Buffer */
    pRxBufCtrl->WritePtr = 0;                                   /* Write pointer to the Ring Buffer */


    pRxBufCtrl->MaxRingBufSize = CanMaxBufSize;    /* maximum receive ring-buffer size */
    pRxBufCtrl->pRingBuf = CanRingBuf;              /* pointer to the receive ring-buffer */
#else
    pRxBufCtrl->MaxRingBufSize = pCanConfig->MaxRxRingBufSize;    /* maximum receive ring-buffer size */
    pRxBufCtrl->pRingBuf = pCanConfig->pRxRingBuf;              /* pointer to the receive ring-buffer */
#endif

    if (pCanConfig->OpMode == AMBA_CAN_OP_MODE_INSIDE_LPBK) {
        AmbaCSL_CanSetLoopbackMode(pCanReg, 1);       /* Config loopback mode */
    } else if (pCanConfig->OpMode == AMBA_CAN_OP_MODE_OUTSIDE_LPBK) {
        AmbaCSL_CanSetLoopbackMode(pCanReg, 2);       /* Config loopback mode */
    } else {
        /* do nothing here. */
    }

    /* Config listen mode and message type */
    if (pCanConfig->OpMode == AMBA_CAN_OP_MODE_LISTEN) {
        /* If all message buffers are set to Rx buffers, let's enable listen mode */
        AmbaCSL_CanSetListenMode(pCanReg, 1);
        AmbaCSL_CanSetMsgBufType(pCanReg, 0);
        /* Config Rx message filter */
        if (pCanConfig->NumIdFilter > 0U) {
            pIdFilterCtrl->NumIdFilter = pCanConfig->NumIdFilter;
            pIdFilterCtrl->pIdFilter   = pCanConfig->pIdFilter;
            CAN_SetIdFilter(pCanReg, 0, pIdFilterCtrl->NumIdFilter, pIdFilterCtrl->pIdFilter);
        } else {
            pIdFilterCtrl->NumIdFilter = 0;
            pIdFilterCtrl->pIdFilter   = NULL;
            AmbaCSL_CanSetMsgBufConfigDone(pCanReg, 0xffffffffU);
        }
    } else {
        AmbaCSL_CanSetListenMode(pCanReg, 0);
        AmbaCSL_CanSetMsgBufType(pCanReg, MsgBufType);          /* Set message buffer type */
        /* Config Rx message filter */
        if (pCanConfig->NumIdFilter > 0U) {
            pIdFilterCtrl->NumIdFilter = pCanConfig->NumIdFilter;
            pIdFilterCtrl->pIdFilter   = pCanConfig->pIdFilter;
            CAN_SetIdFilter(pCanReg, MsgBufType, pIdFilterCtrl->NumIdFilter, pIdFilterCtrl->pIdFilter);
        } else {
            UINT32 ConfigDone = MsgBufType ^ 0xffffffffU;
            pIdFilterCtrl->NumIdFilter = 0;
            pIdFilterCtrl->pIdFilter   = NULL;
            AmbaCSL_CanSetMsgBufConfigDone(pCanReg, ConfigDone);    /* Set Rx message buffer config done */
        }
    }

    /* Set time qunta */
    AmbaCSL_CanSetTq(pCanReg, &pCanConfig->TimeQuanta);

    /* Configurate CAN FD */
    /* Note: The CAN-FD in A9AQ only support non-ISO mode, that is, the implementation follows */
    /*       the Bosch White Paper proposed in April 2012.                                     */
#if defined(CONFIG_QNX)
    if (1) {
#else
    if (CAN_CheckFD(pCanConfig) != 0U) {
#endif

#if defined(CONFIG_QNX)
        /* Map in a shared memory region */
        int p_rfd;

        /* Create shared memory object and set its size */
        p_rfd = shm_open("/CANFD", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if (p_rfd == -1) {
            /* Handle error */;
            printf("error");
        }

        if (ftruncate(p_rfd, CanFdMaxBufSize) == -1) {
            /* Handle error */;
            printf("error");
        }

        /* Map shared memory object */
        CanFdRingBuf = mmap(NULL, CanFdMaxBufSize,
                            PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_SHARED, p_rfd, 0);
        if (CanFdRingBuf == MAP_FAILED) {
            /* Handle error */;
            printf("error");
        }

        pFdRxBufCtrl->MaxRingBufSize = CanFdMaxBufSize;    /* maximum receive ring-buffer size */
        pFdRxBufCtrl->pRingBuf = CanFdRingBuf;              /* pointer to the receive ring-buffer */
#else
        pFdRxBufCtrl->MaxRingBufSize = pCanConfig->FdMaxRxRingBufSize;  /* maximum receive ring-buffer size */
        pFdRxBufCtrl->pRingBuf = pCanConfig->pFdRxRingBuf;              /* pointer to the receive ring-buffer */
#endif

        /* Set Rx Ring Buffer */
        pFdRxBufCtrl->ReadPtr  = 0;                             /* Read pointer to the Ring Buffer */
        pFdRxBufCtrl->WritePtr = 0;                             /* Write pointer to the Ring Buffer */

        AmbaRTSL_CanCtrl[CanCh].EnableCanFd = 1;
        AmbaCSL_CanSetFdTq(pCanReg, &pCanConfig->FdTimeQuanta);
#if !defined(CONFIG_SOC_CV2) //cv2 not support this
        AmbaCSL_CanSetFdSsp(pCanReg, &pCanConfig->FdSsp);
#endif
        AmbaCSL_CanFdEnable(pCanReg);
    } else {
        AmbaRTSL_CanCtrl[CanCh].EnableCanFd = 0;
        AmbaCSL_CanFdDisable(pCanReg);
    }

    /* Config DMA mode and the corresponding interrupt settings */
    if (0x0U != pCanConfig->EnableDMA) {
        AmbaRTSL_CanCtrl[CanCh].EnableRxDma = 1;
        pRxBufCtrl->PerMessageSize = (UINT32)sizeof(AMBA_CAN_DMA_MESSAGE_s);
        pFdRxBufCtrl->PerMessageSize = (UINT32)sizeof(AMBA_CAN_FD_DMA_MESSAGE_s);
        CAN_DmaConfig(CanCh, pCanConfig);
        // Don't enable Rx Done interrupt in DMA mode, only check if rx transfer is done by Dma Desc Done
        AmbaCSL_CanSetIntMask(pCanReg,
#if defined(CAN_ENABLE_SW_SAFETY_MECHANISM)
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_ACK_ERR]          |
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_FORM_ERR]         |
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_CRC_ERR]          |
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_STUFF_ERR]        |
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_BIT_ERR]          |
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_BUS_OFF]          |
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_ERR_PASSIVE]      |
#endif
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_RETRY_FAIL]       |
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_TX_DONE]          |
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_RX_DMA_DESC_DONE] |
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_RX_DMA_DONE]);
    } else {
        AmbaRTSL_CanCtrl[CanCh].EnableRxDma = 0;
        pRxBufCtrl->PerMessageSize = (UINT32)sizeof(AMBA_CAN_MSG_s);
        pFdRxBufCtrl->PerMessageSize = (UINT32)sizeof(AMBA_CAN_FD_MSG_s);
        AmbaRTSL_CanDmaRxIsrCallBack = NULL;
        AmbaCSL_CanSetIntMask(pCanReg,
#if defined(CAN_ENABLE_SW_SAFETY_MECHANISM)
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_ACK_ERR]          |
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_FORM_ERR]         |
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_CRC_ERR]          |
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_STUFF_ERR]        |
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_BIT_ERR]          |
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_BUS_OFF]          |
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_ERR_PASSIVE]      |
#endif
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_RETRY_FAIL] |
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_RX_DONE]    |
                              AmbaCanIsrStatus[AMBA_CAN_STATUS_TX_DONE]);
    }

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    /* Config Auto Answer mode */
    AmbaCSL_CanSetAutoRespMode(pCanReg, pCanConfig->EnableAa);
    /* need to enable related interrupt?? or share rx/tx done interrupt? */
#endif

    AmbaCSL_CanSetTxIntThreshold(pCanReg, 0);
    AmbaCSL_CanSetRxIntThreshold(pCanReg, 0);
    AmbaCSL_CanSetTxIntTimeout(pCanReg, 0);
    AmbaCSL_CanSetRxIntTimeout(pCanReg, 0);

    return CAN_ERR_NONE;
}

UINT32 AmbaRTSL_CanInit(UINT32 CanCh)
{
    UINT32 Ret;
#if defined(CONFIG_QNX)
    static pthread_t  QnxIsrTask;
    static UINT32 task_init = 0;

    if (task_init == 0) {
        // hook-up isr task only one time
        Ret = pthread_create(&QnxIsrTask, NULL, CAN_ISR, CanCh);
        if (Ret == OK) {
            task_init = 1;
            Ret = pthread_setschedprio(QnxIsrTask, 128U);
        }
    } else {
        Ret = CAN_ERR_NONE;
    }
#else
    AMBA_INT_CONFIG_s IntConfig;
    UINT32 IntID = CAN_Ch2IntID(CanCh);

    AmbaCSL_CanInit(CanCh);

    IntConfig.TriggerType   = AMBA_INT_HIGH_LEVEL_TRIGGER;
    IntConfig.IrqType    = AMBA_INT_FIQ;
    IntConfig.CpuTargets = 0x01;

    if (AmbaRTSL_GicGetISR(IntID) != NULL) {
        /* Already hook-up in previous init flow, skip it.
         * It's because we can't remove hook-up function during CANC disable.*/
        Ret = CAN_ERR_NONE;
    } else {
        Ret = AmbaRTSL_GicIntConfig(IntID, &IntConfig, CAN_ISR, 0);
        if (Ret == INT_ERR_NONE) {
            Ret = AmbaRTSL_GicIntEnable(IntID);
            if (Ret == INT_ERR_NONE) {
                Ret = CAN_ERR_NONE;
            } else {
                Ret = CAN_ERR_ARG;
            }
        } else {
            Ret = CAN_ERR_ARG;
        }
    }
#endif
    return Ret;
}

#if defined(CONFIG_QNX)
UINT32 AmbaRTSL_CanGetInfo(UINT32 CanCh, AMBA_CAN_BIT_INFO_s * pBitInfo, UINT32 * pNumFilter, AMBA_CAN_FILTER_s * pFilter, UINT32 RefClk)
#else
UINT32 AmbaRTSL_CanGetInfo(UINT32 CanCh, AMBA_CAN_BIT_INFO_s * pBitInfo, UINT32 * pNumFilter, AMBA_CAN_FILTER_s * pFilter)
#endif
{
    UINT32 Ret;
    const AMBA_CAN_REG_s  *pCanReg = pAmbaCAN_Reg[CanCh];
    UINT32 CanRefClk;
    const AMBA_CAN_FILTER_s *pTmpFilter = AmbaRTSL_CanCtrl[CanCh].IdFilterCtrl.pIdFilter;
    UINT32 FilterNum = (UINT32)AmbaRTSL_CanCtrl[CanCh].IdFilterCtrl.NumIdFilter;
    AMBA_CAN_TQ_s Tq;

    /* decide the maximum buffer size. */
    *pNumFilter = FilterNum;

    /* Fill the filter information. */
    if (AmbaWrap_memcpy(pFilter, pTmpFilter, (*pNumFilter)*sizeof(AMBA_CAN_FILTER_s)) != 0U) {
        /* Do nothing */
    }
#if defined(CONFIG_QNX)
    CanRefClk = RefClk;
#else
    CanRefClk = AmbaRTSL_PllGetCanClk();
#endif
    AmbaCSL_CanGetTq(pCanReg, &Tq);
    pBitInfo->NominalBitLen = 1U + (UINT32)Tq.PropSeg + (UINT32)Tq.PhaseSeg1 + 1U + (UINT32)Tq.PhaseSeg2 + 1U;
    /* Check if division by 0*/
    if ((CanRefClk/((UINT32)Tq.BRP+1U)) > 0U) {
        pBitInfo->NominalTQ = 1000000000U/(CanRefClk/((UINT32)Tq.BRP+1U));
    } else {
        pBitInfo->NominalTQ = 0xFFFFFFFFU;
    }
    pBitInfo->NominalBitRate = (CanRefClk/(UINT32)(Tq.BRP+1U))/pBitInfo->NominalBitLen;

    AmbaCSL_CanGetFdTq(pCanReg, &Tq);
    pBitInfo->DataBitLen = 1U + (UINT32)Tq.PropSeg + (UINT32)Tq.PhaseSeg1 + 1U + (UINT32)Tq.PhaseSeg2 + 1U;
    /* Check if division by 0*/
    if ((CanRefClk/((UINT32)Tq.BRP+1U)) > 0U) {
        pBitInfo->DataTQ = 1000000000U/(CanRefClk/((UINT32)Tq.BRP+1U));
    } else {
        pBitInfo->DataTQ = 0xFFFFFFFFU;
    }
    pBitInfo->DataBitRate = (CanRefClk/((UINT32)Tq.BRP+1U))/pBitInfo->DataBitLen;
    pBitInfo->SysClkFreq = CanRefClk;
    Ret = CAN_ERR_NONE;

    return Ret;
}

