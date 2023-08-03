/**
 *  @file AmbaRTSL_CAN.h
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
 *  @details CAN bus control APIs
 *
 */

#ifndef AMBA_RTSL_CAN_H
#define AMBA_RTSL_CAN_H

#include "AmbaCAN_Def.h"
#include "AmbaCSL_CAN.h"

#define AMBA_CAN_STATUS_BUS_OFF                     0U
#define AMBA_CAN_STATUS_ERR_PASSIVE                 1U
#define AMBA_CAN_STATUS_ACK_ERR                     2U
#define AMBA_CAN_STATUS_FORM_ERR                    3U
#define AMBA_CAN_STATUS_CRC_ERR                     4U
#define AMBA_CAN_STATUS_STUFF_ERR                   5U
#define AMBA_CAN_STATUS_BIT_ERR                     6U
#define AMBA_CAN_STATUS_TIMEOUT                     7U
#define AMBA_CAN_STATUS_RX_OVERFLOW                 8U
#define AMBA_CAN_STATUS_RX_DONE                     9U
#define AMBA_CAN_STATUS_TX_DONE                     10U
#define AMBA_CAN_STATUS_TIMER_WRAP                  11U
#define AMBA_CAN_STATUS_WAKE_UP                     12U
#define AMBA_CAN_STATUS_RETRY_FAIL                  13U
#define AMBA_CAN_STATUS_RX_DONE_TIMEOUT             14U
#define AMBA_CAN_STATUS_TX_DONE_TIMEOUT             15U
#define AMBA_CAN_STATUS_RX_DMA_DESC_DONE            16U
#define AMBA_CAN_STATUS_RX_DMA_DONE                 17U
#define AMBA_CAN_STATUS_RX_DMA_TIMEOUT              18U
#define AMBA_CAN_STATUS_RX_DMA_GET_SPECIAL_ID       19U
#define AMBA_CAN_STATUS_RX_DMA_GET_RTR              20U
#define AMBA_CAN_STATUS_FD_STUFF_ERR                21U
#define AMBA_NUM_CAN_STATUS                         22U

/*-----------------------------------------------------------------------------------------------*\
 * Definitions of CAN Rx message buffer control
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32                      ReadPtr;        /* Read pointer to the Ring Buffer */
    UINT32                      WritePtr;       /* Write pointer to the Ring Buffer */
    UINT32                      MaxRingBufSize; /* maximum receive ring-buffer size in Byte */
    UINT8                       *pRingBuf;      /* pointer to the receive ring-buffer */
    UINT32                      PerMessageSize;
} AMBA_RTSL_CAN_RX_BUF_CTRL_s;

typedef struct {
    UINT32                      ReadPtr;        /* Read pointer to the Ring Buffer */
    UINT32                      WritePtr;       /* Write pointer to the Ring Buffer */
    UINT32                      MaxRingBufSize; /* maximum receive ring-buffer size in Byte */
    UINT8                       *pRingBuf;      /* pointer to the receive ring-buffer */
    UINT32                      PerMessageSize;
} AMBA_RTSL_CAN_FD_RX_BUF_CTRL_s;

typedef struct {
    UINT8                       NumIdFilter;    /* Number of Rx message filters */
    AMBA_CAN_FILTER_s           *pIdFilter;     /* Pointer to ID filters */
} AMBA_RTSL_CAN_ID_FILTER_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions of CAN DMA descriptor format
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32                      Status;         /* Rx DMA status */
    void                        *pDestAddr;     /* pointer to the destination base Address */
    void                        *pNextDesc;     /* pointer to the next descriptor address */
    UINT32                      Reserved[13];   /* Used to keep the address is 64 byte aligned */
} AMBA_CAN_DMA_DESC_s;

typedef struct {
    UINT32                      IdCtrl;
    UINT32                      MsgCtrl;
    UINT32                      MsgOrder;
    UINT8                       Data[8];
    UINT32                      Reserved[11];   /* Used to keep the address is 64 byte aligned */
} AMBA_CAN_DMA_MESSAGE_s;

typedef struct {
    UINT32                      IdCtrl;
    UINT32                      MsgCtrl;
    UINT32                      MsgOrder;
    UINT8                       Data[64];
    UINT32                      Reserved[13];   /* Used to keep the address is 64 byte aligned */
} AMBA_CAN_FD_DMA_MESSAGE_s;

typedef struct {
    AMBA_RTSL_CAN_RX_BUF_CTRL_s RxBufCtrl;
    AMBA_CAN_DMA_DESC_s         *pDmaDesc[2];
    UINT32                      MaxDescSize;
    UINT32                      NextDmaDesc;
} AMBA_RTSL_CAN_DMA_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * RTSL CAN Management Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    AMBA_RTSL_CAN_RX_BUF_CTRL_s     RxBufCtrl;      /* CAN 2.0 */
    AMBA_RTSL_CAN_FD_RX_BUF_CTRL_s  FDRxBufCtrl;      /* CAN FD */
    AMBA_RTSL_CAN_ID_FILTER_CTRL_s  IdFilterCtrl;
    AMBA_RTSL_CAN_DMA_CTRL_s        DmaCtrl;
    UINT8                           EnableCanFd;    /* 0: CAN 2.0 mode, 1: CAN FD mode*/
    UINT8                           EnableRxDma;    /* 0: Non-DMA mode, 1: Rx DMA mode*/
} AMBA_RTSL_CAN_CTRL_s;

typedef void (*CanIsrCb)(UINT32 CanCh, UINT32 MsgBufNo);

void AmbaRTSL_CanSetRxIsrFunc(CanIsrCb pISR);
void AmbaRTSL_CanFdSetRxIsrFunc(CanIsrCb pISR);
void AmbaRTSL_CanSetTxIsrFunc(CanIsrCb pISR);

UINT32 AmbaRTSL_CanInit(void);
UINT32 AmbaRTSL_CanConfig(UINT32 CanCh, const AMBA_CAN_CONFIG_s *pCanConfig, UINT32 MsgBufType);
UINT32 AmbaRTSL_CanTxTransfer(UINT32 CanCh, UINT32 MsgBufNo, const AMBA_CAN_MSG_s *pMessage);
UINT32 AmbaRTSL_CanFdTxTransfer(UINT32 CanCh, UINT32 MsgBufNo, const AMBA_CAN_FD_MSG_s *pFdMessage);
UINT32 AmbaRTSL_CanReadOneRxTuple(UINT32 CanCh, AMBA_CAN_MSG_s *pMessage);
UINT32 AmbaRTSL_CanFdReadOneRxTuple(UINT32 CanCh, AMBA_CAN_FD_MSG_s *pFdMessage);
UINT32 AmbaRTSL_CanGetInfo(UINT32 CanCh, AMBA_CAN_BIT_INFO_s * pBitInfo, UINT32 * pNumFilter, AMBA_CAN_FILTER_s * pFilter);
#endif /* AMBA_RTSL_CAN_H */
