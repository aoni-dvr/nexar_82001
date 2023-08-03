/**
 *  @file AmbaCAN.h
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

#ifndef AMBA_CAN_H
#define AMBA_CAN_H

#include "AmbaCAN_Def.h"

UINT32 AmbaCAN_Enable(UINT32 CanCh, const AMBA_CAN_CONFIG_s *pCanConfig);
UINT32 AmbaCAN_Read(UINT32 CanCh, AMBA_CAN_MSG_s *pMessage, UINT32 Timeout);
UINT32 AmbaCAN_ReadFd(UINT32 CanCh, AMBA_CAN_FD_MSG_s *pFdMessage, UINT32 Timeout);
UINT32 AmbaCAN_Write(UINT32 CanCh, const AMBA_CAN_MSG_s *pMessage, UINT32 Timeout);
UINT32 AmbaCAN_WriteFd(UINT32 CanCh, const AMBA_CAN_FD_MSG_s *pFdMessage, UINT32 Timeout);
UINT32 AmbaCAN_Disable(UINT32 CanCh);
UINT32 AmbaCAN_GetInfo(UINT32 CanCh, AMBA_CAN_BIT_INFO_s * pBitInfo, UINT32 * pNumFilter, AMBA_CAN_FILTER_s * pFilter);

UINT32 AmbaCANPeer_SetBaudRate(UINT32 CanCh,
                               UINT32 BRP,
                               UINT32 PropSeg,
                               UINT32 PhaseSeg1,
                               UINT32 PhaseSeg2,
                               UINT32 SJW,
                               UINT8  *pRingBuf,
                               UINT32 BufSize);

UINT32 AmbaCANPeer_SetFDBaudRate(UINT32 CanCh,
                                 UINT32 BRP,
                                 UINT32 PropSeg,
                                 UINT32 PhaseSeg1,
                                 UINT32 PhaseSeg2,
                                 UINT32 SJW,
                                 UINT8  *pRingBuf,
                                 UINT32 BufSize);

UINT32 AmbaCANPeer_AddFilter(UINT32 CanCh, UINT32 IdFilter, UINT32 IdMask, UINT32 CleanBefore);
UINT32 AmbaCANPeer_Enable(UINT32 CanCh);
UINT32 AmbaCANPeer_Read(UINT32 CanCh, UINT32 *pId, UINT8 *pBuf, UINT32 BufLEN, UINT32 *pActualLEN, UINT32 Timeout);
UINT32 AmbaCANPeer_FDRead(UINT32 CanCh, UINT32 *pId, UINT8 *pBuf, UINT32 BufLEN, UINT32 *pActualLEN, UINT32 Timeout);
UINT32 AmbaCANPeer_Disable(UINT32 CanCh);

#endif /* AMBA_CAN_H */
