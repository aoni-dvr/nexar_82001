/**
 * @file DemoUdp.c
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
#include "AmbaPrint.h"

#define UDPLEN  (ETH_FRAMES_SIZE - 42U)

/* prototype*/
UINT32 DemoRxUdp(const void *frame, UINT16 framelen);
UINT32 DemoRxUdp(const void *frame, UINT16 framelen)
{
    const UINT32 UdpTxLen = UDPLEN;
    const UINT32 RXSIZE = (UdpTxLen*1024U*8U);
    const UDP_HDR_s *udphdr;
    UINT8 *buf;
    UINT32 udpdata_len;
    UINT32 tv2 = 0U;
    static UINT32 bytes = 0U, tv1 = 0U;
    static UINT32 datagrams = 0U, total = 0U;
    const UINT16 udphdrlen = (UINT16)sizeof(*udphdr);
    const AMBA_DEMO_STACK_IP_HDR_s *iphdr;
    static UINT16 nextid = 0xffffU;
    static UINT16 lost = 0U;
    UINT16 curid, j;
    UINT16 Len1 = framelen;
    void *ptr;

    AmbaMisra_TypeCast(&ptr, &frame);
    (void) AmbaNetStack_RemoveEthHdr(&ptr, &Len1);
    AmbaMisra_TypeCast(&iphdr, &ptr);
    AmbaMisra_TypeCast(&buf, &ptr);
    buf = &buf[sizeof(AMBA_DEMO_STACK_IP_HDR_s)];
    AmbaMisra_TypeCast(&udphdr, &buf);
    udpdata_len = (UINT32)AmbaNetStack_Ntohs(udphdr->len) - udphdrlen;

    if (bytes == 0U) {
        (void)AmbaKAL_GetSysTickCount(&tv1);
    }

    bytes += udpdata_len;
    if (udpdata_len == UdpTxLen) {
        total += udpdata_len;
        datagrams++;
        curid = AmbaNetStack_Ntohs(iphdr->ident);
        if ((nextid != curid) &&
            (nextid != 0xffffU)) {
            if (curid > nextid) {
                j = curid - nextid;
                lost += j;
                AmbaPrint_ModulePrintUInt5(DEMO_MODULE_ID, "%d -> %d lost %d/%d", (UINT32)nextid, (UINT32)curid, (UINT32)j, (UINT32)lost, 0U);
            } else {
                AmbaPrint_ModulePrintUInt5(DEMO_MODULE_ID, "Rx DMA Reverse %d -> %d", (UINT32)nextid, (UINT32)curid, 0U, 0U, 0U);
            }
        }
        nextid = (curid + 1U) % 0x8000U;
    }

    if(bytes >= RXSIZE) {
        UINT32 msec, kbits;
        UINT32 kbps;
        (void)AmbaKAL_GetSysTickCount(&tv2);
        // 8/1024 = 1/128
        kbits = bytes >> 7;
        msec = tv2 - tv1;
        EnetDebug("tv1=%u, tv2=%u, bytes=%u", tv1, tv2, bytes);
        EnetDebug( "recv %d bytes (%d kbits) in %d ms", bytes, kbits, msec);
        kbits = kbits*1000U;
        kbps = kbits/msec;
        bytes = 0;
        AmbaPrint_ModulePrintUInt5(DEMO_MODULE_ID, "[%d] %d %d/%d*1000= %d kbps", datagrams, total, kbits, msec, kbps);
    }

    return 0U;
}

/**
 * @ingroup enet
 * sending dummy UDP packet on port 5001, packet size equals to ENET MTU size
 *
 * @param [in] Idx     ENET controller index on AMBA SoC, value start from 0
 * @param [in] DstIp   destination address to send
 * @return enet error number
 */
UINT32 AmbaDemoStack_TxUdp(UINT32 Idx, UINT32 DstIp)
{
#ifdef TXFASTPATH
    static UINT8 DmaData[ETH_FRAMES_SIZE] __attribute__((aligned(AMBA_CACHE_LINE_SIZE))) = "ABCDEFGHIJKLMNOPQRSTUVWXYZ  ";
    const UINT8 *const p8 = DmaData;
#endif
    AMBA_DEMO_STACK_CONFIG_s *pDemoConfig;
    const UINT16 UdpTxLen = (UINT16)UDPLEN;
    void *ptr;
    UINT8 *buf;
    ETH_HDR_s *ethhdr;
    UDP_HDR_s *udphdr;
    AMBA_DEMO_STACK_IP_HDR_s *iphdr;
    UINT16 ipHdrSize = (UINT16)sizeof(AMBA_DEMO_STACK_IP_HDR_s);
    UINT16 udpHdrSize = (UINT16)sizeof(UDP_HDR_s);
    const UINT16 UdpDstPort = 5001U;
    const UINT16 UdpSrcPort = 50000U;
    UINT16 Len1 = ipHdrSize + udpHdrSize + UdpTxLen;
    UINT32 Ret;

    /* arg check */
    if (Idx >= DEMO_STACK_INSTANCES) {
        EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = NET_ERR_EINVAL;
    } else if (DstIp == 0U) {
        EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_RED, "DstIp NULL");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        (void)AmbaDemoStack_GetConfig(Idx, &pDemoConfig);
#ifdef TXFASTPATH
        AmbaMisra_TypeCast(&ptr, &p8);
#else
        (void)pDemoConfig->pGetTxBufCb(Idx, &ptr);
#endif
        AmbaMisra_TypeCast(&ethhdr, &ptr);
        (void)AmbaNetStack_InsertEthHdr(&ptr, &Len1, pDemoConfig->tci, 0x0800);
        AmbaMisra_TypeCast(&iphdr, &ptr);
        AmbaMisra_TypeCast(&buf, &ptr);
        buf = &buf[sizeof(*iphdr)];
        AmbaMisra_TypeCast(&udphdr, &buf);

        if (AmbaWrap_memcpy(ethhdr->dst, pDemoConfig->IcmpClnt.arp.Mac, 6)!= 0U) { }
        if (AmbaWrap_memcpy(ethhdr->src, pDemoConfig->Mac, 6)!= 0U) { }

        iphdr->verhdrlen = 0x45;
        iphdr->service = 0x00;
        iphdr->len = AmbaNetStack_Htons(ipHdrSize + udpHdrSize + UdpTxLen);
        iphdr->ident = AmbaNetStack_Htons(pDemoConfig->IpIdCnt);
        iphdr->frags = AmbaNetStack_Htons(0x0);
        iphdr->ttl = 64;
        iphdr->protocol = 0x11;
        iphdr->chksum = 0x0;
        iphdr->src = pDemoConfig->Ip;
        iphdr->dest = DstIp;

        udphdr->src = AmbaNetStack_Htons(UdpSrcPort);
        udphdr->dest = AmbaNetStack_Htons(UdpDstPort);
        udphdr->len = AmbaNetStack_Htons(UdpTxLen + udpHdrSize);
        udphdr->chksum = 0x0;
        pDemoConfig->IpIdCnt = (pDemoConfig->IpIdCnt + 1U) % 0x8000U;

#if defined(DISABLE_CHECKSUM_OFFLOAD)
        {
            extern UINT16 DemoIPChksum(void *_buf, UINT32 len);
            extern UINT16 DemoUDPChksum(AMBA_DEMO_STACK_IP_HDR_s *iphdr);
            iphdr->chksum = DemoIPChksum((void *)iphdr, sizeof(AMBA_DEMO_STACK_IP_HDR_s));
            udphdr->chksum = DemoUDPChksum(iphdr);
        }
#endif
#ifdef TXFASTPATH
        Ret = AmbaEnet_SetupTxDesc(Idx, p8, Len1, 1U, 1U, 1U);
#else
        Ret = pDemoConfig->pDoTxCb(Idx, Len1);
#endif
    }

    return Ret;
}
