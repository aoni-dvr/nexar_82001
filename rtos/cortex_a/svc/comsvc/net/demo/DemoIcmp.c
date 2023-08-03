/**
 * @file DemoIcmp.c
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

#include "AmbaENET.h"
#include "NetStack.h"
#include "EnetUtility.h"
#include "DemoStack.h"

typedef struct
{
    AMBA_DEMO_STACK_IP_HDR_s iphdr;
    UINT8   type;
    UINT8   code;
    UINT16  chksum;
    UINT16  id;
    UINT16  seqno;
}__attribute__((packed)) ICMP_HDR_s;

static UINT8 my_icmp_data[] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
};

/* prototype*/
void DemoTxIcmpReq(UINT32 Idx, UINT32 Ip);
UINT32 DemoRxIcmp(UINT32 Idx, const void *frame, UINT16 framelen);

void DemoTxIcmpReq(UINT32 Idx, UINT32 Ip)
{
#ifdef TXFASTPATH
    static UINT8 DmaData[ETH_FRAMES_SIZE] __attribute__((aligned(AMBA_CACHE_LINE_SIZE))) = "ABCDEFGHIJKLMNOPQRSTUVWXYZ  ";
    const UINT8 *const p8 = DmaData;
#endif
    AMBA_DEMO_STACK_CONFIG_s *pDemoConfig;
    void *ptr;
    ETH_HDR_s *ethhdr;
    ICMP_HDR_s *icmp;
    UINT16 framelen = 60U;
    UINT8 *buf;

    (void)AmbaDemoStack_GetConfig(Idx, &pDemoConfig);
#ifdef TXFASTPATH
    AmbaMisra_TypeCast(&ptr, &p8);
#else
    (void)pDemoConfig->pGetTxBufCb(Idx, &ptr);
#endif
    AmbaMisra_TypeCast(&ethhdr, &ptr);
    (void)AmbaNetStack_InsertEthHdr(&ptr, &framelen, pDemoConfig->tci, 0x0800);
    AmbaMisra_TypeCast(&icmp, &ptr);
    AmbaMisra_TypeCast(&buf, &ptr);
    if (AmbaWrap_memcpy(ethhdr->dst, pDemoConfig->IcmpClnt.arp.Mac, 6)!= 0U) { }
    if (AmbaWrap_memcpy(ethhdr->src, pDemoConfig->Mac, 6)!= 0U) { }
    icmp->iphdr.verhdrlen = 0x45;
    icmp->iphdr.service   = 0x00;
    icmp->iphdr.len       = AmbaNetStack_Htons(60);
    icmp->iphdr.ident     = AmbaNetStack_Htons(pDemoConfig->IpIdCnt);
    icmp->iphdr.frags     = AmbaNetStack_Htons(0x0);
    icmp->iphdr.ttl       = 255;
    icmp->iphdr.protocol  = 0x01;
    icmp->iphdr.chksum    = 0x0;
    icmp->iphdr.src       = pDemoConfig->Ip;
    icmp->iphdr.dest      = Ip;
    icmp->type            = 0x08;
    icmp->code            = 0x00;
    icmp->chksum          = 0x0;
    icmp->id              = AmbaNetStack_Htons(0x0200);
    icmp->seqno           = AmbaNetStack_Htons(pDemoConfig->IcmpClnt.seqno);
    buf = &buf[sizeof(*icmp)];
    if (AmbaWrap_memcpy(buf, my_icmp_data, 32)!= 0U) { }
    pDemoConfig->IpIdCnt = (pDemoConfig->IpIdCnt + 1U) % 0x8000U;

#if defined(DISABLE_CHECKSUM_OFFLOAD)
    {
        extern UINT16 DemoIPChksum(void *_buf, UINT32 len);
        icmp->iphdr.chksum = DemoIPChksum((void *)&icmp->iphdr, sizeof(AMBA_DEMO_STACK_IP_HDR_s));

        icmp->chksum = DemoIPChksum(((UINT8 *)icmp) + sizeof(AMBA_DEMO_STACK_IP_HDR_s),
                                       ((sizeof(ICMP_HDR_s) - sizeof(AMBA_DEMO_STACK_IP_HDR_s)) + 32U));
    }
#endif

    EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_MAGENTA, "[TX] ICMP echo request");
#ifdef TXFASTPATH
    (void)AmbaEnet_SetupTxDesc(Idx, p8, framelen, 1U, 1U, 1U);
#else
    (void)pDemoConfig->pDoTxCb(Idx, framelen);
#endif
}

UINT32 DemoRxIcmp(UINT32 Idx, const void *frame, UINT16 framelen)
{
    AMBA_DEMO_STACK_CONFIG_s *pDemoConfig;
    const UINT8 *buf;
    const ETH_HDR_s *ethhdr;
    const ICMP_HDR_s *icmp;
    void *ptr;
    UINT16 LenPld = framelen;
    UINT16 TxLen;

    (void) AmbaDemoStack_GetConfig(Idx, &pDemoConfig);
    AmbaMisra_TypeCast(&ethhdr, &frame);
    AmbaMisra_TypeCast(&ptr, &frame);
    (void) AmbaNetStack_RemoveEthHdr(&ptr, &LenPld);
    TxLen = LenPld;
    AmbaMisra_TypeCast(&icmp, &ptr);
    AmbaMisra_TypeCast(&buf, &ptr);
    if ((icmp->type == 0x08U) &&
        (icmp->code == 0x00U)) {
#ifdef TXFASTPATH
        static UINT8 DmaData[ETH_FRAMES_SIZE] __attribute__((aligned(AMBA_CACHE_LINE_SIZE))) = "ABCDEFGHIJKLMNOPQRSTUVWXYZ  ";
        const UINT8 *const p8 = DmaData;
#endif
        ETH_HDR_s *ethhdr_rep;
        ICMP_HDR_s *icmp_rep;

        EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_MAGENTA, "[RX] ICMP echo request");
#ifdef TXFASTPATH
        AmbaMisra_TypeCast(&ptr, &p8);
#else
        (void)pDemoConfig->pGetTxBufCb(Idx, &ptr);
#endif
        AmbaMisra_TypeCast(&ethhdr_rep, &ptr);
        (void)AmbaNetStack_InsertEthHdr(&ptr, &TxLen, pDemoConfig->tci, 0x0800);
        AmbaMisra_TypeCast(&icmp_rep, &ptr);
        if (AmbaWrap_memcpy(icmp_rep, icmp, LenPld)!= 0U) { }
        if (AmbaWrap_memcpy(ethhdr_rep->dst, ethhdr->src, 6)!= 0U) { }
        if (AmbaWrap_memcpy(ethhdr_rep->src, ethhdr->dst, 6)!= 0U) { }
        icmp_rep->iphdr.ident  = AmbaNetStack_Htons(pDemoConfig->IpIdCnt);
        icmp_rep->iphdr.ttl    = 255;
        icmp_rep->iphdr.chksum = 0x0;
        icmp_rep->iphdr.src    = icmp->iphdr.dest;
        icmp_rep->iphdr.dest   = icmp->iphdr.src;
        icmp_rep->type         = 0;
        icmp_rep->chksum       = 0x0;

        pDemoConfig->IpIdCnt = (pDemoConfig->IpIdCnt + 1U) % 0x8000U;
#if defined(DISABLE_CHECKSUM_OFFLOAD)
    {
        extern UINT16 DemoIPChksum(void *_buf, UINT32 len);
        icmp_rep->iphdr.chksum = DemoIPChksum((void *)&icmp_rep->iphdr, sizeof(AMBA_DEMO_STACK_IP_HDR_s));

        icmp_rep->chksum = DemoIPChksum(((UINT8 *)icmp_rep) + sizeof(AMBA_DEMO_STACK_IP_HDR_s),
                                       ((sizeof(ICMP_HDR_s) - sizeof(AMBA_DEMO_STACK_IP_HDR_s)) + 32U));
    }
#endif
        EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_MAGENTA, "[TX] ICMP echo reply");
#ifdef TXFASTPATH
        (void)AmbaEnet_SetupTxDesc(Idx, p8, TxLen, 1U, 1U, 1U);
#else
        (void)pDemoConfig->pDoTxCb(Idx, TxLen);
#endif
    } else if ((icmp->type == 0x00U) &&
               (icmp->code == 0x00U)) {
        const UINT8 *icmpdata;
        INT32 v;

        EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_MAGENTA, "[RX] ICMP echo reply");
        icmpdata = &buf[sizeof(*icmp)];
        if (LenPld == 60U) {
            if (AmbaNetStack_Ntohs(icmp->id) == 0x0200U) {
                if (AmbaNetStack_Ntohs(icmp->seqno) == pDemoConfig->IcmpClnt.seqno) {
                    if (AmbaWrap_memcmp(icmpdata, my_icmp_data, 32, &v)!= 0U) { }
                    if (v == 0) {
                        pDemoConfig->IcmpClnt.poll_var = 1;
                    }
                }
            }
        }
    } else {
        EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_RED, "unknown ICMP packet");
    }

    return 0U;
}
