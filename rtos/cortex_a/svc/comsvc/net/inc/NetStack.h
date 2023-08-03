/**
 * @file NetStack.h
 * networks stack common header
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

#ifndef AMBA_ENET_STACK_H
#define AMBA_ENET_STACK_H

#define NET_ERR_NONE                  0U
#define NET_ERR_BASE                  NETX_ERR_BASE
#define NETX_MODULE_ID                ((UINT16)(NETX_ERR_BASE >> 16U))
#define DEMO_MODULE_ID                NETX_MODULE_ID
#define AVB_MODULE_ID                 NETX_MODULE_ID
#define NET_ERR_EPERM                 (NET_ERR_BASE + 1U)      /* Operation not permitted */
#define NET_ERR_ENOMEM                (NET_ERR_BASE + 12U)     /* Out of memory */
#define NET_ERR_EFAULT                (NET_ERR_BASE + 14U)     /* Bad address */
#define NET_ERR_ENODEV                (NET_ERR_BASE + 16U)     /* No such device */
#define NET_ERR_EBUSY                 (NET_ERR_BASE + 19U)     /* Device or resource busy */
#define NET_ERR_EINVAL                (NET_ERR_BASE + 22U)     /* Invalid argument */
#define NET_ERR_ETIMEDOUT             (NET_ERR_BASE + 110U)    /* timed out */

#define ETH_ARP_TYPE                  0x0806U
#define ETH_IP_TYPE                   0x0800U
#define ETH_VLAN_TYPE                 0x8100U
#define ETH_PTP2_TYPE                 0x88f7U
#define ETH_AVTP_TYPE                 0x22f0U

#define DEMO_STACK_INSTANCES          ENET_INSTANCES
#define NETX_STACK_INSTANCES          ENET_INSTANCES
#define AVB_STACK_INSTANCES           ENET_INSTANCES

typedef UINT32 (*AMBA_NET_STACK_GETTXBUF_f)(const UINT32 Idx, void **Addr);
typedef UINT32 (*AMBA_NET_STACK_GETRXBUF_f)(const UINT32 Idx, void **Addr);
typedef UINT32 (*AMBA_NET_STACK_DOTX_f)(const UINT32 Idx, const UINT16 TxLen);
typedef UINT32 (*AMBA_NET_STACK_DOTXWAIT_f)(const UINT32 Idx, const UINT16 TxLen, UINT32 *pSec, UINT32 *pNs);
typedef UINT32 (*AMBA_NET_STACK_RX_f)(const UINT32 Idx, const void *frame,  const UINT16 RxLen, const UINT16 type);

typedef struct
{
    UINT16  tpid;                 /** Tag Protocol Identifier */
    UINT16  tci;                  /** Tag Control Information */
} __attribute__((packed)) VLAN_TAG_s;

/* ETH without VLAN tag */
typedef struct
{
    UINT8           dst[6];
    UINT8           src[6];
    UINT16          type;
} __attribute__((packed)) ETH_HDR_s;

/* ETH with VLAN tag */
typedef struct
{
    UINT8           dst[6];
    UINT8           src[6];
    VLAN_TAG_s      vlan;
    UINT16          type;
} __attribute__((packed)) ETH_VLAN_HDR_s;

#endif /* AMBA_ENET_STACK_H */
