/**
 * @file NetXStack.c
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

#include "AmbaKAL.h"
#include "AmbaENET.h"
#include "NetStack.h"
#include "EnetUtility.h"
#include "NetXStack.h"
#include "AmbaPrint.h"
#include "nx_bsd.h"

static NETX_STACK_CONFIG_s *pNetXStackConfig[] = {
    NULL,
#if (NETX_STACK_INSTANCES > 1U)
    NULL,
#endif
};

static UINT32 Digits2IP(UINT32 a, UINT32 b, UINT32 c, UINT32 d)
{
  return ((a & 0xffU) << 24) |
         ((b & 0xffU) << 16) |
         ((c & 0xffU) << 8) |
         ((d & 0xffU));
}

void NetXEth0Driver(struct NX_IP_DRIVER_STRUCT *driver_req);
void NetXEth0Driver(struct NX_IP_DRIVER_STRUCT *driver_req)
{
    extern VOID nx_enet_network_driver(NX_IP_DRIVER *driver_req_ptr, const NETX_STACK_CONFIG_s *pNetXConfig);
    const NETX_STACK_CONFIG_s *const pNetXConfig = pNetXStackConfig[0];
    nx_enet_network_driver(driver_req, pNetXConfig);
}

#if (NETX_STACK_INSTANCES > 1U)
void NetXEth1Driver(struct NX_IP_DRIVER_STRUCT *driver_req);
void NetXEth1Driver(struct NX_IP_DRIVER_STRUCT *driver_req)
{
    extern VOID nx_enet_network_driver(NX_IP_DRIVER *driver_req_ptr, const NETX_STACK_CONFIG_s *pNetXConfig);
    const NETX_STACK_CONFIG_s *pNetXConfig = pNetXStackConfig[1];
    nx_enet_network_driver(driver_req, pNetXConfig);
}
#endif

extern NX_DRIVER NetXDriver[NETX_STACK_INSTANCES];
NX_DRIVER NetXDriver[] = {
    NetXEth0Driver,
#if (NETX_STACK_INSTANCES > 1U)
    NetXEth1Driver,
#endif
};

static UINT32 NetXStackRxHandle(const UINT32 Idx, const void *frame,  const UINT16 Len, const UINT16 type)
{
    UINT32 Ret = 0U;

    if (Idx >= NETX_STACK_INSTANCES) {
        EnetInfo(__func__, __LINE__, NETX_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = NET_ERR_EINVAL;
    } else if (frame == NULL) {
        EnetInfo(__func__, __LINE__, NETX_MODULE_ID, ANSI_RED, "frame NULL!");
        Ret = NET_ERR_EINVAL;
    } else if (pNetXStackConfig[Idx] == NULL) {
        EnetInfo(__func__, __LINE__, NETX_MODULE_ID, ANSI_RED, "please run NetXStack_SetConfig!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        void nx_enet_network_driver_receive(NX_IP *ip_ptr, const NETX_STACK_CONFIG_s *pNetXConfig, const void *frame, UINT16 RxLen, const UINT16 type);
        const NETX_STACK_CONFIG_s *pNetXConfig = pNetXStackConfig[Idx];

        nx_enet_network_driver_receive(pNetXConfig->pNxIp, pNetXConfig, frame, Len, type);
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to set netx config
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] pNetXConfig NetX Config Struct for ENET
 * @return enet error number
 */
UINT32 NetXStack_SetConfig(UINT32 Idx, NETX_STACK_CONFIG_s *pNetXConfig)
{
    UINT32 Ret = 0U;
#ifdef AMBA_MISRA_FIX_H
    struct sockaddr_in SockIn;
    struct sockaddr Sock;
    struct timeval tv;
    AmbaMisra_TouchUnused(&SockIn.sin_addr.s_addr);
    AmbaMisra_TouchUnused(&Sock.sa_family);
    AmbaMisra_TouchUnused(&tv.tv_sec);
#endif

    if (Idx >= NETX_STACK_INSTANCES) {
        EnetInfo(__func__, __LINE__, NETX_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = NET_ERR_EINVAL;
    } else if (pNetXConfig == NULL) {
        EnetInfo(__func__, __LINE__, NETX_MODULE_ID, ANSI_RED, "pNetXConfig NULL!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        pNetXStackConfig[Idx] = pNetXConfig;
    }
    return Ret;
}

/**
 * @ingroup enet
 * The function is used to get netx config
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [out] pNetXConfig NetX Config Struct for ENET
 * @return enet error number
 */
UINT32 NetXStack_GetConfig(UINT32 Idx, NETX_STACK_CONFIG_s **pNetXConfig)
{
    UINT32 Ret = 0U;

    if (Idx >= NETX_STACK_INSTANCES) {
        EnetInfo(__func__, __LINE__, NETX_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = NET_ERR_EINVAL;
    } else if (pNetXConfig == NULL) {
        EnetInfo(__func__, __LINE__, NETX_MODULE_ID, ANSI_RED, "pNetXConfig NULL!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        *pNetXConfig = pNetXStackConfig[Idx];
    }
    return Ret;
}

/**
 * @ingroup enet
 * The function is used to configure and init NetX by Ethernet driver
 *
 * @param [in] pNetXConfig NetX Config Struct for ENET
 * @return enet error number
 */
UINT32 NetXStack_EnetInit(NETX_STACK_CONFIG_s *pNetXConfig)
{
    UINT32 Idx = pNetXConfig->Idx;
    static char nx_pkt_pool_name[] = "nx_pkt_pool";
    static char nx_ip_name[] = "nx_ip";
    static INT32 amba_netx_inited = 0;
    UINT32 Ret = 0U;
    /* arg check */
    if (Idx >= NETX_STACK_INSTANCES) {
        EnetInfo(__func__, __LINE__, NETX_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        pNetXStackConfig[Idx] = pNetXConfig;
        pNetXConfig->pGetTxBufCb = AmbaEnet_GetTxBuf;
        pNetXConfig->pGetRxBufCb = AmbaEnet_GetRxBuf;
        pNetXConfig->pDoTxCb = AmbaEnet_Tx;
        pNetXConfig->pRxCb = NetXStackRxHandle;

        if (amba_netx_inited == 0) {
            nx_system_initialize();

            Ret = nx_packet_pool_create(pNetXConfig->pNxPktPool, nx_pkt_pool_name, pNetXConfig->PyldSize,
                pNetXConfig->pPool, pNetXConfig->PoolSize);
            NetXDebug("nx_packet_pool_create() returned %d", Ret);

            Ret = nx_ip_create(pNetXConfig->pNxIp, nx_ip_name, Digits2IP(127, 0, 0, 1), Digits2IP(255, 0, 0, 0),
                               pNetXConfig->pNxPktPool, NetXDriver[Idx], pNetXConfig->pIpMem, pNetXConfig->IpMemSize, pNetXConfig->NxIPPri);
            NetXDebug("nx_ip_create() returned %d", Ret);

            Ret = nx_arp_enable(pNetXConfig->pNxIp, pNetXConfig->pArpMem, pNetXConfig->ArpMemSize);
            NetXDebug("nx_arp_enable() returned %d", Ret);

            Ret = nx_tcp_enable(pNetXConfig->pNxIp);
            NetXDebug("nx_tcp_enable() returned %d", Ret);

            Ret = nx_udp_enable(pNetXConfig->pNxIp);
            NetXDebug("nx_udp_enable() returned %d", Ret);

            Ret = nx_ip_fragment_enable(pNetXConfig->pNxIp);
            NetXDebug("nx_ip_fragment_enable() returned %d", Ret);

            Ret = nx_icmp_enable(pNetXConfig->pNxIp);
            NetXDebug("nx_icmp_enable() returned %d", Ret);

            Ret = (UINT32)bsd_initialize(pNetXConfig->pNxIp, pNetXConfig->pNxPktPool,
                               (char *)pNetXConfig->pBSDMem, pNetXConfig->BsdMemSize, pNetXConfig->NxBSDPri);
            NetXDebug("bsd_initialize() returned %d", Ret);

            amba_netx_inited = 1;
        }
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to configure IP address and IP mask for NetX
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] IpAddr IP Address value
 * @param [in] IpMask IP Mask value
 * @return enet error number
 */
UINT32 NetXStack_EnetIfconfig(UINT32 Idx, UINT32 IpAddr, UINT32 IpMask)
{
    UINT32 Ret = 0U;
    const NETX_STACK_CONFIG_s *pNetXConfig = pNetXStackConfig[Idx];
    UINT16 i;
    UINT8 attached_flag = 0U;
    UINT16 eth0_attached = 0U;
#if (NETX_STACK_INSTANCES > 1U)
    UINT16 eth1_attached = 0U;
#endif

    /* arg check */
    if (Idx >= NETX_STACK_INSTANCES) {
        EnetInfo(__func__, __LINE__, NETX_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = NET_ERR_EINVAL;
    } else if((IpAddr == 0U)||(IpMask == 0U)) {
        EnetInfo(__func__, __LINE__, NETX_MODULE_ID, ANSI_RED, "Ip NULL!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        for (i = 0; i < NETX_STACK_INSTANCES; i++) {
            if ((pNetXConfig->pNxIp->nx_ip_interface[i].nx_interface_link_driver_entry) == NetXDriver[0]) {
                NetXDebug("eth0 attached to nx_idx[%d]", i);
                eth0_attached = i;
                attached_flag = 1;
            }
#if (NETX_STACK_INSTANCES > 1U)
            else if ((pNetXConfig->pNxIp->nx_ip_interface[i].nx_interface_link_driver_entry) == NetXDriver[1]) {
                NetXDebug("eth1 attached to nx_idx[%d]", i);
                eth1_attached = i;
                attached_flag = 1;
            }
#endif
            else {
                /* TBD */
            }
        }

#if (NETX_STACK_INSTANCES >= 2U)
        if (0U == Idx)
#endif
        {
            if (attached_flag == 0U) {
                static char name_eth0[] = "eth0";
                Ret = nx_ip_interface_attach(pNetXConfig->pNxIp, name_eth0, IpAddr, IpMask,
                                             NetXDriver[Idx]);
                NetXDebug("nx_ip_interface_attach eth%d", Idx);
                if (Ret != 0U) {
                    AmbaPrint_ModulePrintUInt5(NETX_MODULE_ID, "eth0 nx_ip_interface_attach() returned %d", Ret, 0U, 0U, 0U, 0U);
                }
            } else {
                Ret = nx_ip_interface_address_set(pNetXConfig->pNxIp, (ULONG)eth0_attached,
                                                  IpAddr, IpMask);
                NetXDebug("nx_ip_interface_address_set eth%d", Idx);
                if (Ret != 0U) {
                    AmbaPrint_ModulePrintUInt5(NETX_MODULE_ID, "eth0 nx_idx[%d] nx_ip_interface_address_set() returned %d", (UINT32)eth0_attached, Ret, 0U, 0U, 0U);
                }
            }
        }
#if (NETX_STACK_INSTANCES == 2U)
        else {
#elif (NETX_STACK_INSTANCES > 2U)
        else if (1U == Idx) {
#endif
#if (NETX_STACK_INSTANCES >= 2U)
            if (attached_flag == 0U) {
                static char name_eth1[] = "eth1";
                Ret = nx_ip_interface_attach(pNetXConfig->pNxIp, name_eth1, IpAddr, IpMask,
                                             NetXDriver[Idx]);
                NetXDebug("nx_ip_interface_attach eth%d", Idx);
                if (Ret != 0U) {
                    AmbaPrint_ModulePrintUInt5(NETX_MODULE_ID, "eth1 nx_ip_interface_attach() returned %d", Ret, 0U, 0U, 0U, 0U);
                }
            } else {
                Ret = nx_ip_interface_address_set(pNetXConfig->pNxIp, (ULONG)eth1_attached,
                                                  IpAddr, IpMask);
                NetXDebug("nx_ip_interface_address_set eth%d", Idx);
                if (Ret != 0U) {
                    AmbaPrint_ModulePrintUInt5(NETX_MODULE_ID, "eth1 nx_idx[%d] nx_ip_interface_address_set() returned %d", (UINT32)eth1_attached, Ret, 0U, 0U, 0U);
                }
            }
        }
#endif
#if (NETX_STACK_INSTANCES > 2U)
        else {
            Ret = nx_ip_interface_address_set(pNetXConfig->pNxIp, (ULONG)Idx,
                                              IpAddr, IpMask);
            NetXDebug("nx_ip_interface_address_set eth%d", Idx);
            if (Ret != 0U) {
                AmbaPrint_ModulePrintUInt5(NETX_MODULE_ID, "nx_ip_interface_address_set() returned %d", Ret, 0U, 0U, 0U, 0U);
            }
        }
#endif
    }
    return Ret;
}

/**
 * @ingroup enet
 * The function is used to ping IP address
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] DstIp IP Address to ping
 * @param [in] TMO Timeout value (ms) to wait for echo reply
 * @return enet error number
 */
UINT32 NetXStack_Ping(const UINT32 Idx, const UINT32 DstIp, const UINT32 TMO)
{
    const NETX_STACK_CONFIG_s *const pNetXConfig = pNetXStackConfig[Idx];
    UINT32 Ret;
    NX_PACKET *Resp;
    UINT32 ts, te;
    CHAR my_icmp_data[] = "0123456789";
    const ULONG icmp_data_len = sizeof(my_icmp_data);

    /* arg check */
    if (Idx >= NETX_STACK_INSTANCES) {
        EnetInfo(__func__, __LINE__, NETX_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = NET_ERR_EINVAL;
    } else if ((DstIp == 0U)||(TMO == 0U)) {
        EnetInfo(__func__, __LINE__, NETX_MODULE_ID, ANSI_RED, "DstIp or TMO NULL!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        (void)AmbaKAL_GetSysTickCount(&ts);

        Ret = nx_icmp_ping(pNetXConfig->pNxIp, DstIp, my_icmp_data, icmp_data_len, &Resp, TMO);
        NetXDebug("nx_icmp_ping() returned %d", Ret);
        if (Ret == 0U) {
            (void)AmbaKAL_GetSysTickCount(&te);
            if (Resp != NULL) {
                Ret = nx_packet_release(Resp);
                NetXDebug("nx_packet_release() returned %d", Ret);
            }
            AmbaPrint_ModulePrintUInt5(NETX_MODULE_ID, "time=%d ms", te - ts, 0U, 0U, 0U, 0U);
        } else {
            AmbaPrint_ModulePrintUInt5(NETX_MODULE_ID, "nx_icmp_ping() returned %d", Ret, 0U, 0U, 0U, 0U);
            Ret = NET_ERR_ETIMEDOUT;
        }
    }

    return Ret;
}
