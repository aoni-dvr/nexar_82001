/**
*  @file ArmEth.h
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
*   @details The Arm Ethernet access utility
*
*/

#ifndef ARM_ETH_H
#define ARM_ETH_H

#include "AmbaTypes.h"

#define ARM_ETH_MAX_IO  8

typedef struct _ARM_ETH_SIZE_INFO {
  UINT32 Num;
  UINT32 Size[ARM_ETH_MAX_IO];
} ARM_ETH_SIZE_INFO_s;

typedef struct _ARM_ETH_DATA_INFO {
  UINT32  SeqNum;
  UINT64  TimeStamp;
  char    *pBuf[ARM_ETH_MAX_IO];
} ARM_ETH_DATA_INFO_s;

/******************************************************************************
 *  Defined in ArmEth.c
 ******************************************************************************/

UINT32 ArmEth_Init(UINT32 *pCh);
UINT32 ArmEth_TxRxTest(UINT32 Ch, char *pBuff, UINT32 Size);
UINT32 ArmEth_GetSize(UINT32 Ch, ARM_ETH_SIZE_INFO_s *pSizeInfo);
UINT32 ArmEth_Recv(UINT32 Ch, const ARM_ETH_SIZE_INFO_s *pSizeInfo, ARM_ETH_DATA_INFO_s *pDataInfo);
UINT32 ArmEth_Send(UINT32 Ch, const ARM_ETH_SIZE_INFO_s *pSizeInfo, const ARM_ETH_DATA_INFO_s *pDataInfo);

#endif /* ARM_ETH_H */

