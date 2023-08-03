/**
 *  @file RefCode_Avdecc.c
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
 *  @details Test case for AVDECC.
 *
 */


#include <avdecc.h>

#include <RefCode_Avdecc.h>

/* Tx by ENet (1) or loopback (0) */
#define TX (1)

#if (TX == 0)

UINT32 AvbAvtp_Rx(UINT32 Idx, const void *frame, UINT16 framelen, const UINT8 smac[6]);
#endif

static AMBA_AVB_STACK_CONFIG_s *pAvbConfig = NULL;


static void test_tx(UINT32 Idx, const void *frame, UINT16 framelen)
{
#if TX
    /* Tx directly */
    (void) AmbaEnet_Tx(0u, framelen);
    (void)Idx;
    (void)frame;
#else
    /* loopback */
    const void *advPtr;
    UINT16 size = framelen;
    UINT32 s;
    UINT32 f;

#if VLAN
    s = sizeof(ETH_VLAN_HDR_s);
#else
    s = sizeof(ETH_HDR_s);
#endif
    /* advPtr = frame + s; */

    /*f = (UINT32)frame;*/
    if (AmbaWrap_memcpy(&f, &frame, sizeof(f))!= 0U) { }

    f += s;

    /*advPtr = (void *)f;*/
    advPtr = NULL;
    if (AmbaWrap_memcpy(&advPtr, &f, sizeof(f))!= 0U) { }

    size -= (UINT16)(s & 0xFFFFu);
    (void)AvbAvtp_Rx(Idx, advPtr, size, pAvbConfig->Mac);
#endif /* TX */
}


#define TEST_PKT (0)

#if TEST_PKT

UINT32 avdecc_adp_txEntityMsg_prep(UINT8 msgType, pkt_adp_t *p);
UINT32 avdecc_acmp_txMsg_prep(UINT8 msgType, pkt_acmp_t *p);
UINT32 avdecc_aecp_txMsg_prep(UINT8 msgType, pkt_aecp_t *p);
void avdecc_eth_prep(const void *buf, UINT8 vlan, const UINT8 *dmac, const UINT8 *smac);

static void test_adp(void)
{
    UINT8  adp_mac[6] = ADP_MULTICAST_MAC;
    UINT32 size;
    const char * buf;
    const char *pbuf;
    void *ptr;
    pkt_adp_t *advPtr;
    UINT32 err;

    (void)AmbaEnet_GetTxBuf(0, &ptr);
    AmbaMisra_TypeCast(&buf, &ptr);

#if VLAN
    size = sizeof(ETH_VLAN_HDR_s) ;
#else
    size = sizeof(ETH_HDR_s);
#endif
    pbuf = &buf[size];
    AmbaMisra_TypeCast(&advPtr, &pbuf);
    size += sizeof(pkt_adp_t);

    err = avdecc_adp_txEntityMsg_prep(ADP_MSG_ENTITY_DISCOVER, advPtr);
    if (err == 0U) {
        avdecc_eth_prep(buf, VLAN, adp_mac, pAvbConfig->Mac);
        test_tx(0, buf, (UINT16)(size & 0xFFFFu));
    }

}


static void test_acmp(void)
{
    UINT8  acmp_mac[6] = ACMP_MULTICAST_MAC;
    UINT32 size;
    const char * buf;
    const char *pbuf;
    void *ptr;
    pkt_acmp_t *advPtr;
    UINT32 err;

    (void)AmbaEnet_GetTxBuf(0, &ptr);
    AmbaMisra_TypeCast(&buf, &ptr);

#if VLAN
    size = sizeof(ETH_VLAN_HDR_s) ;
#else
    size = sizeof(ETH_HDR_s);
#endif
    pbuf = &buf[size];
    AmbaMisra_TypeCast(&advPtr, &pbuf);
    size += sizeof(pkt_acmp_t);

    err = avdecc_acmp_txMsg_prep(ACMP_MSG_CONNECT_TX_COMMAND, advPtr);
    if (err == 0U) {
        avdecc_eth_prep(buf, VLAN, acmp_mac, pAvbConfig->Mac);
        test_tx(0, buf, (UINT16)(size & 0xFFFFu));
    }
}


static void test_aecp(void)
{
    UINT8  acmp_mac[6] = ACMP_MULTICAST_MAC;
    UINT32 size;
    const char * buf;
    const char *pbuf;
    void *ptr;
    pkt_aecp_t *advPtr;
    UINT32 err;

    (void)AmbaEnet_GetTxBuf(0, &ptr);
    AmbaMisra_TypeCast(&buf, &ptr);

#if VLAN
    size = sizeof(ETH_VLAN_HDR_s) ;
#else
    size = sizeof(ETH_HDR_s);
#endif
    pbuf = &buf[size];
    AmbaMisra_TypeCast(&advPtr, &pbuf);
    size += sizeof(pkt_aecp_acquireE_t);

    err = avdecc_aecp_txMsg_prep(AECP_MSG_AEM_COMMAND, advPtr);
    if (err == 0U) {
        avdecc_eth_prep(buf, VLAN, acmp_mac, pAvbConfig->Mac);
        test_tx(0, buf, (UINT16)(size & 0xFFFFu));
    }
}

#endif

void test_avdecc(void);
void test_avdecc(void)
{
#if TEST_PKT
    test_adp();
    test_acmp();
    test_aecp();
#endif
}


static UINT8 *getSMac(void)
{
    return pAvbConfig->Mac;
}


/* Get EID from MAC */
static UINT64 getEID(void)
{
    UINT64 eid;

    eid  = ((UINT64)(pAvbConfig->Mac[0]) << 56);
    eid |= ((UINT64)(pAvbConfig->Mac[1]) << 48);
    eid |= ((UINT64)(pAvbConfig->Mac[2]) << 40);
    eid |= ((UINT64)0xFFFFU << 24);
    eid |= ((UINT64)(pAvbConfig->Mac[3]) << 16);
    eid |= ((UINT64)(pAvbConfig->Mac[4]) << 8);
    eid |= (UINT64)(pAvbConfig->Mac[5]);

    return eid;
}

void avdecc_init(UINT8 t, UINT8 l);
void avdecc_init(UINT8 t, UINT8 l)
{
    static AvdeccCfg_t AvdeccCfg;
    AvdeccCfg_t *pCfg = &AvdeccCfg;
    entityInfo_t *pE = &(AvdeccCfg.entityInfo);
    descriptor_entity_t *pDE = &(AvdeccCfg.entity_descriptor);
    const char *chr1 = "myAVB";
    const char *chr2 = "0.0.2";
    const char *chr3 = "myGrp";
    const char *chr4 = "16899";

    /* TODO: Multi-Instance */
    (void)AmbaAvbStack_GetConfig(0, &pAvbConfig);

    /* Some default value should be 0 */
    if (AmbaWrap_memset(&AvdeccCfg, 0, sizeof(AvdeccCfg_t))!= 0U) { }

    pDE->entity_id = getEID();
    pDE->entity_model_id = 0x123456789ABCDEF0u;
#if 0 // TODO by User
    pDE->entity_capabilities = ADP_ENTITY_CAP_AEM_SUPPORTED;
    pDE->entity_capabilities |= ADP_ENTITY_CAP_CLASS_A_SUPPORTED;
    pDE->entity_capabilities |= ADP_ENTITY_CAP_CLASS_B_SUPPORTED;
#endif // TODO by User

    if (t != 0u) {
        // talker_capabilities
        pDE->talker_stream_sources = 1u;
        pDE->talker_capabilities = (UINT16)(TALKER_CAP_IMP | TALKER_CAP_VIDEO | TALKER_CAP_AUDIO);
    }
    if (l != 0u) {
        // listener_capabilities
        pDE->listener_stream_sinks = 1u;
        pDE->listener_capabilities = (UINT16)(LISTENER_CAP_IMP | LISTENER_CAP_VIDEO | LISTENER_CAP_AUDIO);
    }
    pDE->controller_capabilities = 0;
    pDE->available_index = 0;
    pDE->association_id = 0;
    if (AmbaWrap_memcpy(pDE->entity_name, chr1, 5)!= 0U) { }
    pDE->vendor_name_string = 0;
    pDE->model_name_string = 1;
    if (AmbaWrap_memcpy(pDE->firmware_version, chr2, 5)!= 0U) { }
    if (AmbaWrap_memcpy(pDE->group_name, chr3, 5)!= 0U) { }
    if (AmbaWrap_memcpy(pDE->serial_number, chr4, 5)!= 0U) { }
    // A device is required to have at least one (1) Configuration. i.e. index = 0
    pDE->configurations_count = 1;

    pE->entity_model_id = pDE->entity_model_id;
    pE->available_index = 0u;
#if 0
    pE->gptp_grandmaster_id = 0x0123456789ABCDEFu;
    // Not support
    pE->gptp_domain_no = 0u;
    pE->id_control_index = 0u;
    pE->interface_index = 0u;
    // Not support
    pE->association_id = 0u;

#endif

    pCfg->valid_time = 31u;

    pCfg->test_tx = test_tx;
    pCfg->getSMac = getSMac;
    pCfg->getEID = getEID;

    /* Init Entities */
    avdecc_aem_init(pCfg, t, l);
}


void AvdeccTLStatus(UINT32 TStatus, UINT32 LStatus)
{
    UINT8 t = (UINT8)(TStatus & 0xFFu);
    UINT8 l = (UINT8)(LStatus & 0xFFu);

    avdecc_init(t, l);
}

void AvdeccFCId(UINT64 eid)
{
    extern void listener_set_talkerEID(UINT64 eid);

    listener_set_talkerEID(eid);
}

void AvdeccAdpMsg(UINT32 msgId)
{
    extern void advecc_send_adp_pkt(UINT8 msgType);

    switch (msgId) {
    case 0u:
        /* Available */
        advecc_send_adp_pkt(ADP_MSG_ENTITY_AVAILABLE);
        break;

    case 1u:
        /* Departing */
        advecc_send_adp_pkt(ADP_MSG_ENTITY_DEPARTING);
        break;

    case 2u:
        /* Discover all */
        advecc_send_adp_pkt(ADP_MSG_ENTITY_DISCOVER);
        break;

    default:
        /* Nothing */
        break;
    }
}

