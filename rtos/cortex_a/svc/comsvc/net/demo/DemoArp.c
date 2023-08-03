/**
 * @file DemoArp.c
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

//#define ARP_PKT_SIZE 42U
typedef struct
{
    UINT16    hwtype;
    UINT16    protocol;
    UINT8     hwlen;
    UINT8     protolen;
    UINT16    opcode;
    UINT8     sMac[6];
    UINT32    sipaddr;
    UINT8     tMac[6];
    UINT32    tipaddr;
} __attribute__((packed)) arpreq_s;

/* prototype*/
void DemoTxArpReq(UINT32 Idx, UINT32 Ip);
UINT32 DemoRxArp(UINT32 Idx, const void *frame, UINT16 framelen);

void DemoTxArpReq(UINT32 Idx, UINT32 Ip)
{
#ifdef TXFASTPATH
    static UINT8 DmaData[ETH_FRAMES_SIZE] __attribute__((aligned(AMBA_CACHE_LINE_SIZE))) = "ABCDEFGHIJKLMNOPQRSTUVWXYZ  ";
    const UINT8 *const p8 = DmaData;
#endif
    AMBA_DEMO_STACK_CONFIG_s *pDemoConfig;
    void *ptr;
    arpreq_s *arpreq;
    ETH_HDR_s *ethhdr;
    UINT16 framelen = (UINT16)sizeof(*arpreq);

    (void)AmbaDemoStack_GetConfig(Idx, &pDemoConfig);
#ifdef TXFASTPATH
    AmbaMisra_TypeCast(&ptr, &p8);
#else
    (void)pDemoConfig->pGetTxBufCb(Idx, &ptr);
#endif
    AmbaMisra_TypeCast(&ethhdr, &ptr);
    (void)AmbaNetStack_InsertEthHdr(&ptr, &framelen, pDemoConfig->tci, 0x0806);
    AmbaMisra_TypeCast(&arpreq, &ptr);

    ethhdr->dst[0] = 0xffU;
    ethhdr->dst[1] = 0xffU;
    ethhdr->dst[2] = 0xffU;
    ethhdr->dst[3] = 0xffU;
    ethhdr->dst[4] = 0xffU;
    ethhdr->dst[5] = 0xffU;
    if (AmbaWrap_memcpy(ethhdr->src, pDemoConfig->Mac, 6)!= 0U) { }
    arpreq->hwtype      = AmbaNetStack_Htons(0x0001);
    arpreq->protocol    = AmbaNetStack_Htons(0x0800);
    arpreq->hwlen       = 6;
    arpreq->protolen    = 4;
    arpreq->opcode      = AmbaNetStack_Htons(0x0001);
    if (AmbaWrap_memcpy(arpreq->sMac, pDemoConfig->Mac, 6)!= 0U) { }
    arpreq->sipaddr    = pDemoConfig->Ip;
    arpreq->tMac[0] = 0xffU;
    arpreq->tMac[1] = 0xffU;
    arpreq->tMac[2] = 0xffU;
    arpreq->tMac[3] = 0xffU;
    arpreq->tMac[4] = 0xffU;
    arpreq->tMac[5] = 0xffU;
    arpreq->tipaddr    = Ip;

    EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_MAGENTA, "[TX] ARP request");
#ifdef TXFASTPATH
    (void)AmbaEnet_SetupTxDesc(Idx, p8, framelen, 1U, 1U, 1U);
#else
    (void)pDemoConfig->pDoTxCb(Idx, framelen);
#endif
}

UINT32 DemoRxArp(UINT32 Idx, const void *frame, UINT16 framelen)
{
#ifdef TXFASTPATH
    static UINT8 DmaData[ETH_FRAMES_SIZE] __attribute__((aligned(AMBA_CACHE_LINE_SIZE))) = "ABCDEFGHIJKLMNOPQRSTUVWXYZ  ";
    const UINT8 *const p8 = DmaData;
#endif
    AMBA_DEMO_STACK_CONFIG_s *pDemoConfig;
    void *ptr;
    const arpreq_s *arp;
    UINT16 Len1 = framelen;
    const ETH_HDR_s *ethhdr;

    (void) AmbaDemoStack_GetConfig(Idx, &pDemoConfig);
    AmbaMisra_TypeCast(&ethhdr, &frame);
    AmbaMisra_TypeCast(&ptr, &frame);
    (void) AmbaNetStack_RemoveEthHdr(&ptr, &Len1);
    AmbaMisra_TypeCast(&arp, &ptr);

    if ((AmbaNetStack_Ntohs(arp->opcode) == 0x0001U) && (arp->tipaddr == pDemoConfig->Ip)) {
        arpreq_s *arpans;
        ETH_HDR_s *txethhdr;
        UINT16 TxLen = (UINT16)sizeof(*arpans);

        EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_MAGENTA, "[RX] ARP request");
#ifdef TXFASTPATH
        AmbaMisra_TypeCast(&ptr, &p8);
#else
        (void)pDemoConfig->pGetTxBufCb(Idx, &ptr);
#endif
        AmbaMisra_TypeCast(&txethhdr, &ptr);
        (void)AmbaNetStack_InsertEthHdr(&ptr, &TxLen, pDemoConfig->tci, 0x0806);
        AmbaMisra_TypeCast(&arpans, &ptr);

        if (AmbaWrap_memcpy(txethhdr->dst, ethhdr->src, 6)!= 0U) { }
        if (AmbaWrap_memcpy(txethhdr->src, pDemoConfig->Mac, 6)!= 0U) { }
        arpans->hwtype      = AmbaNetStack_Htons(0x0001);
        arpans->protocol    = AmbaNetStack_Htons(0x0800);
        arpans->hwlen       = 6;
        arpans->protolen    = 4;
        arpans->opcode      = AmbaNetStack_Htons(0x0002);
        if (AmbaWrap_memcpy(arpans->sMac, pDemoConfig->Mac, 6)!= 0U) { }
        arpans->sipaddr = pDemoConfig->Ip;
        if (AmbaWrap_memcpy(arpans->tMac, ethhdr->src, 6)!= 0U) { }
        arpans->tipaddr = arp->sipaddr;

        EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_MAGENTA, "[TX] ARP reply");
#ifdef TXFASTPATH
        (void)AmbaEnet_SetupTxDesc(Idx, p8, TxLen, 1U, 1U, 1U);
#else
        (void)pDemoConfig->pDoTxCb(Idx, TxLen);
#endif
    } else if (AmbaNetStack_Ntohs(arp->opcode) == 0x0002U) {
        UINT32 Ip = arp->sipaddr;

        EnetInfo(__func__, __LINE__, DEMO_MODULE_ID, ANSI_MAGENTA, "[RX] ARP reply");

        if ((pDemoConfig->IcmpClnt.arp.Ip == 0x0U) && (pDemoConfig->IcmpClnt.Ip == Ip)) {
            pDemoConfig->IcmpClnt.arp.Ip = Ip;
            if (AmbaWrap_memcpy(pDemoConfig->IcmpClnt.arp.Mac, arp->sMac, 6)!= 0U) { }
        }
    } else {
        EnetDebug("ENET %s(%d) not my ARP, drop", __func__, __LINE__);
    }

    return 0U;
}
