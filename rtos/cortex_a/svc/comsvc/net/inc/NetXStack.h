/**
 * @file NetXStack.h
 * NetX stack
 *
 * @ingroup enet
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
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AMBA_ENET_NETXSTACK_H
#define AMBA_ENET_NETXSTACK_H

//for consistent sizeof(NX_TCP_SOCKET)
#define NX_ENABLE_EXTENDED_NOTIFY_SUPPORT
//for misra depress
#define NX_PATCH_MISRA
#include "nx_api.h"

typedef void (*NX_DRIVER)(struct NX_IP_DRIVER_STRUCT *driver_req);

#define NETX_STACK_GETTXBUF_f  AMBA_NET_STACK_GETTXBUF_f
#define NETX_STACK_GETRXBUF_f  AMBA_NET_STACK_GETRXBUF_f
#define NETX_STACK_DOTX_f      AMBA_NET_STACK_DOTX_f
#define NETX_STACK_RX_f        AMBA_NET_STACK_RX_f

typedef struct
{
    NETX_STACK_GETTXBUF_f  pGetTxBufCb;
    NETX_STACK_GETRXBUF_f  pGetRxBufCb;
    NETX_STACK_DOTX_f      pDoTxCb;
    NETX_STACK_RX_f        pRxCb;
    UINT32                 Idx;
    UINT8                  Mac[6];
    UINT32                 NxIPPri;
    UINT32                 NxBSDPri;
    UINT32                 PyldSize;
    UINT32                 ArpMemSize;
    UINT32                 IpMemSize;
    UINT32                 BsdMemSize;
    UINT32                 PoolSize;
    UINT16                 tci;
    UINT8                 *pArpMem;
    UINT8                 *pIpMem;
    UINT8                 *pPool;
    UINT8                 *pBSDMem;
    NX_PACKET_POOL        *pNxPktPool;
    NX_IP                 *pNxIp;
} NETX_STACK_CONFIG_s;

/* API */
UINT32 NetXStack_SetConfig(UINT32 Idx, NETX_STACK_CONFIG_s *pNetXConfig);
UINT32 NetXStack_GetConfig(UINT32 Idx, NETX_STACK_CONFIG_s **pNetXConfig);
UINT32 NetXStack_EnetInit(NETX_STACK_CONFIG_s *pNetXConfig);
UINT32 NetXStack_EnetIfconfig(UINT32 Idx, UINT32 IpAddr, UINT32 IpMask);
UINT32 NetXStack_Ping(const UINT32 Idx, const UINT32 DstIp, const UINT32 TMO);

#endif /* AMBA_ENET_NETXSTACK_H */
