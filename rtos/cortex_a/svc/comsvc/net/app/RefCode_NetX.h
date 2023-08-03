/**
 *  @file RefCode_NetX.h
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details header for netx refcode
 *
 */


#ifndef AMBA_NETX_REFCODE_H
#define AMBA_NETX_REFCODE_H

#ifdef CONFIG_NETX_ENET
#define NX_TELNET_BUF_SIZE   (4U * 1024U)
#define NX_TELNET_INVALID    0xdeadbeefU

extern UINT32 NxTelnetServCurIdx;
extern UINT8 NxTelnetRxBuf[NX_TELNET_BUF_SIZE];
extern UINT32 NxTelnetCurUsrInput;

void NetXInit(UINT32 Idx);
void NetXDump(UINT32 Idx);
UINT32 NetXBSDExample_TxUdp(UINT32 DstIp, UINT32 TxPause);
UINT32 NetXBSDExample_RxUdp(void);
UINT32 NetXBSDExample_TxTcp(UINT32 DstIp, UINT32 TxPause);
UINT32 NetXBSDExample_RxTcp(void);
UINT32 NetXExample_TxUdp(UINT32 Idx, UINT32 DstIp, UINT32 TxPause);
UINT32 NetXExample_RxUdp(UINT32 Idx);
UINT32 NetXExample_TxTcp(UINT32 Idx, UINT32 DstIp, UINT32 TxPause);
UINT32 NetXExample_RxTcp(UINT32 Idx);

#ifdef CONFIG_NETX_TELNET
void TelnetServInit(UINT32 Idx);
UINT32 AmbaTelnet_Read(UINT8 *pNxTelnetRxBuf, UINT32 RxSize, UINT32 *pActualRxSize);
UINT32 AmbaTelnet_Write(const UINT8 *pTxBuf, UINT32 TxSize);
#endif /* CONFIG_NETX_ENET */

#endif /* CONFIG_NETX_TELNET */

#endif /* AMBA_NETX_REFCODE_H */
