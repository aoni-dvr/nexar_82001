/**
 *  @file AmbaCSL_CAN.h
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
 *  @details Definitions & Constants for CAN Control APIs
 *
 */

#ifndef AMBA_CSL_CAN_H
#define AMBA_CSL_CAN_H

#include "AmbaCAN_Def.h"
#include "AmbaReg_CAN.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaCSL_CAN.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_CAN_REG_s *pAmbaCAN_Reg[AMBA_NUM_CAN_CHANNEL];

void AmbaCSL_CanSetTq(AMBA_CAN_REG_s *pCanReg, const AMBA_CAN_TQ_s *pTq);
void AmbaCSL_CanSetFdTq(AMBA_CAN_REG_s *pCanReg, const AMBA_CAN_TQ_s *pTq);
void AmbaCSL_CanGetRxMsgBufData(const AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, UINT8 *pData, UINT8 SizeCode);
void AmbaCSL_CanSetIdFilter(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, UINT8 Enable, UINT32 Id, UINT32 IdMask);
void AmbaCSL_CanConfigDone(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo);
INT32 AmbaCSL_CanRequestMsgBuf(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo);
INT32 AmbaCSL_CanSetMsgBufCtrl(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, const AMBA_CAN_MSG_s *pMessage);
INT32 AmbaCSL_CanFdSetMsgBufCtrl(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, const AMBA_CAN_FD_MSG_s *pMessage);
void AmbaCSL_CanSetMsgBufId(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, UINT32 Id, UINT8 Extension);
INT32 AmbaCSL_CanSetMsgBufData(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, const UINT8 *pData, UINT8 Size);
void AmbaCSL_CanSetMsgBufPriority(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, UINT8 Priority);
void AmbaCSL_CanGetTq(const  AMBA_CAN_REG_s *pCanReg, AMBA_CAN_TQ_s *pTq);
void AmbaCSL_CanGetFdTq(const AMBA_CAN_REG_s *pCanReg, AMBA_CAN_TQ_s *pTq);
UINT32 AmbaCSL_CanGetMsgId(const AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo);
UINT8 AmbaCSL_CanGetMsgExtention(const AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo);
void AmbaCSL_CanInit(void);

void AmbaCSL_CanFdEnable(AMBA_CAN_REG_s *pCanReg);
void AmbaCSL_CanFdDisable(AMBA_CAN_REG_s *pCanReg);
void AmbaCSL_CanSetAutoRespMode(AMBA_CAN_REG_s *pCanReg, UINT32 d);
void AmbaCSL_CanSetListenMode(AMBA_CAN_REG_s *pCanReg, UINT32 d);
void AmbaCSL_CanSetLoopbackMode(AMBA_CAN_REG_s *pCanReg, UINT32 d);
void AmbaCSL_CanSetNonIsoCanFdMode(AMBA_CAN_REG_s *pCanReg, UINT32 d);

void AmbaCSL_TtCanEnable(AMBA_CAN_REG_s *pCanReg);
void AmbaCSL_TtCanDisable(AMBA_CAN_REG_s *pCanReg);
void AmbaCSL_TtCanTtTimerEnable(AMBA_CAN_REG_s *pCanReg);
void AmbaCSL_TtCanTtTimerDisable(AMBA_CAN_REG_s *pCanReg);

void AmbaCSL_CanSetTqReg(AMBA_CAN_REG_s *pCanReg, UINT32 d);
void AmbaCSL_CanFdSetTqReg(AMBA_CAN_REG_s *pCanReg, UINT32 d);

UINT32 AmbaCSL_CanGetTqReg(const AMBA_CAN_REG_s *pCanReg);
UINT32 AmbaCSL_CanGetTqdReg(const AMBA_CAN_REG_s *pCanReg);

UINT32 AmbaCSL_CanGetMsgBufType(const AMBA_CAN_REG_s *pCanReg);
void AmbaCSL_CanSetMsgBufType(AMBA_CAN_REG_s *pCanReg, UINT32 d);
void AmbaCSL_CanSetMsgBufConfigDone(AMBA_CAN_REG_s *pCanReg, UINT32 d);
void AmbaCSL_CanSetIntMask(AMBA_CAN_REG_s *pCanReg, UINT32 d);
void AmbaCSL_CanSetTxIntThreshold(AMBA_CAN_REG_s *pCanReg, UINT32 d);
void AmbaCSL_CanSetTxIntTimeout(AMBA_CAN_REG_s *pCanReg, UINT32 d);
void AmbaCSL_CanSetRxIntThreshold(AMBA_CAN_REG_s *pCanReg, UINT32 d);
void AmbaCSL_CanSetRxIntTimeout(AMBA_CAN_REG_s *pCanReg, UINT32 d);
UINT32 AmbaCSL_CanGetRxBufOverflowId(const AMBA_CAN_REG_s *pCanReg);
UINT32 AmbaCSL_CanGetRetryFail(const AMBA_CAN_REG_s *pCanReg);

void AmbaCSL_CanSetMsgBufRequest(AMBA_CAN_REG_s *pCanReg, UINT32 d);
UINT32 AmbaCSL_CanGetMsgBufRequest(const AMBA_CAN_REG_s *pCanReg);
UINT32 AmbaCSL_CanGetMsgBufGrant(const AMBA_CAN_REG_s *pCanReg);

void AmbaCSL_CanSetMsgId(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, UINT32 d);
void AmbaCSL_CanSetMsgCtrl(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, UINT32 d);
UINT32 AmbaCSL_CanGetMsgCtrl(const AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo);

void AmbaCSL_CanEnable(AMBA_CAN_REG_s *pCanReg);
void AmbaCSL_CanDisable(AMBA_CAN_REG_s *pCanReg);
void AmbaCSL_CanReset(AMBA_CAN_REG_s *pCanReg, UINT32 d);
void AmbaCSL_CanWakeUp(AMBA_CAN_REG_s *pCanReg);

UINT32 AmbaCSL_CanGetIntStatus(const AMBA_CAN_REG_s *pCanReg);
void AmbaCSL_CanClearAllInterrupts(AMBA_CAN_REG_s *pCanReg);
UINT32 AmbaCSL_CanGetRxDoneStatus(const AMBA_CAN_REG_s *pCanReg);
UINT32 AmbaCSL_CanGetTxDoneStatus(const AMBA_CAN_REG_s *pCanReg);

void AmbaCSL_CanSetFilterId(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, UINT32 d);
void AmbaCSL_CanSetFilterMask(AMBA_CAN_REG_s *pCanReg, UINT32 MsgBufNo, UINT32 d);

void AmbaCSL_CanDmaStart(AMBA_CAN_REG_s *pCanReg);
void AmbaCSL_CanDmaSetDescAddr(AMBA_CAN_REG_s *pCanReg, UINT32 d);
void AmbaCSL_CanDmaSetTimeout(AMBA_CAN_REG_s *pCanReg, UINT32 d);
void AmbaCSL_CanDmaSetDescSize(AMBA_CAN_REG_s *pCanReg, UINT32 d);
void AmbaCSL_CanDmaClearDescCount(AMBA_CAN_REG_s *pCanReg);
void AmbaCSL_CanDmaEnableTimeout(AMBA_CAN_REG_s *pCanReg);
void AmbaCSL_CanDmaDisableTimeout(AMBA_CAN_REG_s *pCanReg);
#endif /* AMBA_CSL_CAN_H */
