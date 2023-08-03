/**
 * @file DemoStack.c
 * Amba example UDP/IP stack (used only for driver unit test)
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

#include "AmbaKAL.h"
#include "AmbaENET.h"
#include "NetStack.h"
#include "EnetUtility.h"
#include "DemoStack.h"

static AMBA_DEMO_STACK_CONFIG_s *pAmbaDemoStackConfig[] = {
    NULL,
#if (DEMO_STACK_INSTANCES > 1U)
    NULL,
#endif
};

static UINT32 DemoRxIp(UINT32 Idx, const void *frame, UINT16 framelen)
{
    extern UINT32 DemoRxIcmp(UINT32 Idx, const void *frame, UINT16 framelen);
    extern UINT32 DemoRxUdp(const void *frame, UINT16 framelen);
    const AMBA_DEMO_STACK_CONFIG_s *pDemoConfig = pAmbaDemoStackConfig[Idx];
    const AMBA_DEMO_STACK_IP_HDR_s *iphdr;
    UINT16 Len1 = framelen;
    void *ptr;
    UINT32 Ret = 0U;

    AmbaMisra_TypeCast(&ptr, &frame);
    (void) AmbaNetStack_RemoveEthHdr(&ptr, &Len1);
    AmbaMisra_TypeCast(&iphdr, &ptr);

    if ((iphdr->dest != pDemoConfig->Ip) &&
        (iphdr->dest != 0xffffffffU)) {
        EnetDebug("ENET %s(%d) 0x%08x not my IP, drop", __func__, __LINE__, iphdr->dest);
    } else {
        switch (iphdr->protocol) {
        case 0x01:
            Ret = DemoRxIcmp(Idx, frame, framelen);
            break;
        case 0x11:
            Ret = DemoRxUdp(frame, framelen);
            break;
        default:
            EnetDebug("ENET %s(%d) drop ip.proto=0x%x", __func__, __LINE__, iphdr->protocol);
            break;
        }
    }

    return Ret;
}

static UINT32 DemoStackRxHandle(const UINT32 Idx, const void *frame,  const UINT16 Len, const UINT16 type)
{
    UINT32 Ret = 0U;

    if (Idx >= DEMO_STACK_INSTANCES) {
        EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = NET_ERR_EINVAL;
    } else if (frame == NULL) {
        EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_RED, "frame NULL!");
        Ret = NET_ERR_EINVAL;
    } else if (pAmbaDemoStackConfig[Idx] == NULL) {
        EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_RED, "please run AmbaDemoStack_SetConfig!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        extern UINT32 DemoRxArp(UINT32 Idx, const void *frame, UINT16 framelen);

        switch (type) {
        case ETH_ARP_TYPE:
            Ret = DemoRxArp(Idx, frame, Len);
            break;
        case ETH_IP_TYPE:
            Ret = DemoRxIp(Idx, frame, Len);
            break;
        default:
            /* TBD */
            break;
        }
    }

    return Ret;
}

#if defined(DISABLE_CHECKSUM_OFFLOAD)
/**
 * Compute check-sum for IP header.
 */
UINT16 DemoIPChksum(void *_buf, UINT32 len)
{
    UINT16 *buf = (UINT16 *) _buf;
    UINT32 sum  = 0;

    len >>= 1U;
    while (len-- != 0U) {
        sum += *(buf++);
        if (sum > 0xffffU) {
            sum -= 0xffffU;
        }
    }
    return (UINT16)((~sum) & 0x0000ffffU);
}

/**
 * Compute check-sum for UDP header.
 */
UINT16 DemoUDPChksum(AMBA_DEMO_STACK_IP_HDR_s *iphdr)
{
    UINT32 sum = 0x0;
    UINT16 *s;
    UINT16 size;
    UINT32 addr;
    UDP_HDR_s *udphdr = (void *)iphdr + sizeof(AMBA_DEMO_STACK_IP_HDR_s);

    addr = iphdr->src;
    sum += addr >> 16;
    sum += addr & 0xffffU;
    addr = iphdr->dest;
    sum += addr >> 16;
    sum += addr & 0xffffU;
    sum += (UINT32)(iphdr->protocol) << 8;         /* endian swap */
    size = udphdr->len;
    sum += size;

    size = AmbaNetStack_Ntohs(size);
    s    = (UINT16 *) ((UINT8 *) udphdr);
    while (size > 1U) {
        sum += *s;
        s++;
        size -= 2U;
    }
    if (size != 0U) {
        sum += *(UINT8 *) s;
    }

    sum = (sum & 0xffffU) + (sum >> 16);         /* add overflow counts */
    sum = (sum & 0xffffU) + (sum >> 16);         /* once again */

    return (UINT16)~sum;
}
#endif

/**
 * @ingroup enet
 * The function is used to set demo stack config
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] pDemoConfig Demo Stack Config Struct for ENET
 * @return enet error number
 */
UINT32 AmbaDemoStack_SetConfig(UINT32 Idx, AMBA_DEMO_STACK_CONFIG_s *pDemoConfig)
{
    UINT32 Ret = 0U;

    if (pDemoConfig == NULL) {
        EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_RED, "pDemoConfig NULL!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        pAmbaDemoStackConfig[Idx] = pDemoConfig;
    }
    return Ret;
}

/**
 * @ingroup enet
 * The function is used to get demo stack config
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [out] pDemoConfig Demo Stack Config Struct for ENET
 * @return enet error number
 */
UINT32 AmbaDemoStack_GetConfig(UINT32 Idx, AMBA_DEMO_STACK_CONFIG_s **pDemoConfig)
{
    UINT32 Ret = 0U;

    if (Idx >= DEMO_STACK_INSTANCES) {
        EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = NET_ERR_EINVAL;
    } else if (pDemoConfig == NULL) {
        EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_RED, "pDemoConfig NULL!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        *pDemoConfig = pAmbaDemoStackConfig[Idx];
    }
    return Ret;
}

/**
 * @ingroup enet
 * The function is used to configure and init DemoStack by Ethernet driver
 *
 * @param [in] pDemoConfig DemoStack Config
 * @return enet error number
 */
UINT32 AmbaDemoStack_EnetInit(AMBA_DEMO_STACK_CONFIG_s *pDemoConfig)
{
    UINT32 Ret = NET_ERR_NONE;

    if (pDemoConfig == NULL) {
        EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_RED, "pDemoConfig NULL!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        UINT32 Idx = pDemoConfig->Idx;
        pAmbaDemoStackConfig[Idx] = pDemoConfig;
        pDemoConfig->pGetTxBufCb = AmbaEnet_GetTxBuf;
        pDemoConfig->pGetRxBufCb = AmbaEnet_GetRxBuf;
        pDemoConfig->pDoTxCb = AmbaEnet_Tx;
        pDemoConfig->pRxCb = DemoStackRxHandle;
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to ping
 *
 * @param [in] Idx     ENET controller index on AMBA SoC, value start from 0
 * @param [in] DstIp   destination address to ping
 * @param [in] TMO     ICMP timeout value millisecond
 * @return enet error number
 */
UINT32 AmbaDemoStack_Ping(const UINT32 Idx, const UINT32 DstIp, const UINT32 TMO)
{
    extern void DemoTxArpReq(UINT32 Idx, UINT32 Ip);
    AMBA_DEMO_STACK_CONFIG_s *pDemoConfig = pAmbaDemoStackConfig[Idx];
    UINT32 arpok = TMO, icmpok = TMO;
    UINT32 Ret = NET_ERR_NONE;

    /* arg check */
    if (Idx >= DEMO_STACK_INSTANCES) {
        EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = NET_ERR_EINVAL;
    } else if ((DstIp == 0U)||(TMO == 0U))  {
        EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_RED, "DstIp or TMO NULL!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        /* Send ARP request if entry is not already in table */
        if ((pDemoConfig->IcmpClnt.Ip != DstIp) ||
            ((pDemoConfig->IcmpClnt.arp.Mac[0] == 0x0U) &&
             (pDemoConfig->IcmpClnt.arp.Mac[1] == 0x0U) &&
             (pDemoConfig->IcmpClnt.arp.Mac[2] == 0x0U) &&
             (pDemoConfig->IcmpClnt.arp.Mac[3] == 0x0U) &&
             (pDemoConfig->IcmpClnt.arp.Mac[4] == 0x0U) &&
             (pDemoConfig->IcmpClnt.arp.Mac[5] == 0x0U))) {
            pDemoConfig->IcmpClnt.Ip = DstIp;
            if (AmbaWrap_memset(&pDemoConfig->IcmpClnt.arp, 0, sizeof(pDemoConfig->IcmpClnt.arp))!= 0U) { }
            DemoTxArpReq(Idx, DstIp);

            do {
                (void) AmbaKAL_TaskSleep(1);
                if (--arpok == 0U) {
                    break;
                }
                if ((arpok % 500U) == 0U) {
                    DemoTxArpReq(Idx, DstIp);
                }
            } while (pDemoConfig->IcmpClnt.arp.Ip != DstIp);
            if (arpok == 0U) {
                EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_WHITE, "no arp response");
            }
        }

        if (arpok > 0U) {
            extern void DemoTxIcmpReq(UINT32 Idx, UINT32 Ip);
            /* Send ICMP request */
            pDemoConfig->IcmpClnt.seqno++;
            pDemoConfig->IcmpClnt.poll_var = 0;
            DemoTxIcmpReq(Idx, DstIp);
            do {
                (void) AmbaKAL_TaskSleep(1);
                if (--icmpok == 0U) {
                    break;
                }
            } while (pDemoConfig->IcmpClnt.poll_var == 0);
        }

        if (icmpok == 0U) {
            EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_WHITE, "no echo reply");
            Ret = NET_ERR_ETIMEDOUT;
        }
    }

    return Ret;
}
