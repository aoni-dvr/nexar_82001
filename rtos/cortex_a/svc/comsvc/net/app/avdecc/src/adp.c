/**
 *  @file adp.c
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
 *  @details AVDECC Discovery Protocol
 *
 */

#include <avdecc.h>
#include <avdecc_private.h>


UINT32 avdecc_adp_txEntityMsg_prep(UINT8 msgType, pkt_adp_t *p)
{
    UINT32 err = 0u;
    const entityInfo_t *pE = &(pAvdeccCfg->entityInfo);
    const descriptor_entity_t *pDE = &(pAvdeccCfg->entity_descriptor);

    if (AmbaWrap_memset(p, 0, sizeof(pkt_adp_t))!= 0U) { }

    /* AVTP Format */
    if (msgType == ADP_MSG_ENTITY_DISCOVER) {
        p->avtpdu_hdr.message_type = ADP_MSG_ENTITY_DISCOVER;
    } else if (msgType == ADP_MSG_ENTITY_AVAILABLE) {
        // txEntityAvailable()
        p->avtpdu_hdr.message_type = ADP_MSG_ENTITY_AVAILABLE;
    } else if (msgType == ADP_MSG_ENTITY_DEPARTING) {
        // txEntityDeparting
        p->avtpdu_hdr.message_type = ADP_MSG_ENTITY_DEPARTING ;
    } else {
        err = 1u;
    }

    if (err == 0u) {
        p->avtpdu_hdr.control_data_len = AVDECC_CD_LEN_ADP;
        p->avtpdu_hdr.valid_time = (UINT8)(pAvdeccCfg->valid_time & 0x1Fu);
        p->avtpdu_hdr.version = 0;
        p->avtpdu_hdr.sv = 0;
        p->avtpdu_hdr.subtype = AVTPDU_SUBTYPE_ADP;
        {
            const void *vptr = &(p->avtpdu_hdr);
            UINT32 *ptr;
            AmbaMisra_TypeCast(&ptr, &vptr);

            *ptr = Amba_htonl(*ptr);
        }
        p->avtpdu_hdr.entity_id = Amba_htonll(pDE->entity_id);

        /* AVDECC Discovery Protocol Data Unit (ADPDU) Format */
        p->adpdu.entity_model_id = Amba_htonll(pE->entity_model_id);
        p->adpdu.entity_cap = Amba_htonl(pE->entity_cap);

        p->adpdu.talker_cap = Amba_htons(pE->talker_cap);
        p->adpdu.talker_stream_src = Amba_htons(pE->talker_stream_src);

        p->adpdu.listener_cap = Amba_htons(pE->listener_cap);
        p->adpdu.listener_stream_sinks = Amba_htons(pE->listener_stream_sinks);

        p->adpdu.ctrl_cap = Amba_htonl(pE->ctrl_cap);
        p->adpdu.available_index = Amba_htonl(pE->available_index);
        p->adpdu.gptp_grandmaster_id = Amba_htonll(pE->gptp_grandmaster_id);
        p->adpdu.gptp_domain_no = pE->gptp_domain_no;
        p->adpdu.interface_index = Amba_htons(pE->interface_index);
        p->adpdu.id_control_index = Amba_htons(pE->id_control_index);
        p->adpdu.association_id = Amba_htonll(pE->association_id);
    }

    return err;
}



void advecc_send_adp_pkt(UINT8 msgType)
{
    UINT8  adp_mac[6] = ADP_MULTICAST_MAC;
    UINT32 size;
    char * buf;
    void * pbuf;
    pkt_adp_t *advPtr;
    UINT32 err;


    (void)AmbaEnet_GetTxBuf(0, &pbuf);
    AmbaMisra_TypeCast(&buf, &pbuf);

    if (avdecc_IsVlan() == TRUE) {
        pbuf = &buf[sizeof(ETH_VLAN_HDR_s)];
        size = sizeof(ETH_VLAN_HDR_s) ;
    } else {
        pbuf = &buf[sizeof(ETH_HDR_s)];
        size = sizeof(ETH_HDR_s);
    }

    AmbaMisra_TypeCast(&advPtr, &pbuf);
    size += sizeof(pkt_adp_t);

    err = avdecc_adp_txEntityMsg_prep(msgType, advPtr);
    if (err == 0U) {
        avdecc_eth_prep(buf, avdecc_IsVlan(), adp_mac, pAvdeccCfg->getSMac());
        pAvdeccCfg->test_tx(0u, buf, (UINT16)(size & 0xFFFFu));
    }
}


static sm_adp_advEntity_t SM_Adp_AdvE __attribute__((section(".bss.noinit")));
#define S_ADP_ADVE_IDLE         (0U)
#define S_ADP_ADVE_INITIALIZE   (1U)
#define S_ADP_ADVE_ADVERTISE    (2U)
#define S_ADP_ADVE_WAITING      (3U)


static sm_adp_advInterface_t SM_Adp_AdvI __attribute__((section(".bss.noinit")));
#define S_ADP_ADVI_IDLE         (0U)
#define S_ADP_ADVI_INITIALIZE   (1U)
#define S_ADP_ADVI_WAITING      (2U)
#define S_ADP_ADVI_DEPARTING    (3U)
#define S_ADP_ADVI_ADVERTISE    (4U)
#define S_ADP_ADVI_RECEIVED_DISCOVER (5U)
#define S_ADP_ADVI_UPDATE_GM    (6U)
#define S_ADP_ADVI_LINK_STATE_CHANGE (7U)
#define S_ADP_ADVI_WAITING_EVT  (0xFEU)     ///< prevent available_index to be increased under waiting state.


static sm_adp_discovery_t SM_Adp_Disc __attribute__((section(".bss.noinit")));
#define S_ADP_DISC_IDLE         (0U)
#define S_ADP_DISC_WAITING      (1U)
#define S_ADP_DISC_DISCOVER     (2U)
#define S_ADP_DISC_AVAILABLE    (3U)
#define S_ADP_DISC_DEPARTING    (4U)
#define S_ADP_DISC_TIMEOUT      (5U)




/*
 *  AVDECC Entity operation
 */

/**
 *  Set sm_adp_advEntity.needsAdvertise of Entity.
 *
 *  @param [in] uid     0 for all, !0 for specific one.
 */
static void AEO_adp_Set_needsAdvertise(UINT64 uid)
{
    if ((uid == 0u) ||
        (uid == pAvdeccCfg->entity_descriptor.entity_id)) {
        SM_Adp_AdvE.needsAdvertise = TRUE;
    }
}


static void AEO_adp_txEntityAvailable(void)
{
    if (pAvdeccCfg != NULL) {
        advecc_send_adp_pkt(ADP_MSG_ENTITY_AVAILABLE);
        /*
         *  cf. 6.2.1.16,
         *  incremented after transmitting an ENTITY_AVAILABLE message
         */
        pAvdeccCfg->entityInfo.available_index += 1u;
    }
}

#if 0
static void smf_sendAvailable(void)
{
    // TODO: Set all interface
    SM_Adp_AdvI.doAdvertise = TRUE;
}
#endif

static void avdecc_sm_adp_AdvertiseEntity(void)
{
    entityInfo_t *ei = &(pAvdeccCfg->entityInfo);
    sm_adp_advEntity_t *sm = &SM_Adp_AdvE;
    UINT32 curT;

    switch (sm->state) {
        case S_ADP_ADVE_IDLE:
            // TODO: how to trigger to S_ADP_ADVE_INITIALIZE
            if (sm->needsAdvertise == TRUE) {
                sm->state = S_ADP_ADVE_INITIALIZE;
            }
            break;

        case S_ADP_ADVE_INITIALIZE:
        case S_ADP_ADVE_ADVERTISE:
        case S_ADP_ADVE_WAITING:
            if (sm->state == S_ADP_ADVE_INITIALIZE) {
                ei->available_index = 0u;
                /* UCT to next */
                sm->state = S_ADP_ADVE_ADVERTISE;
            }
            /* fall through */
            if ((sm->state == S_ADP_ADVE_INITIALIZE) ||
                (sm->state == S_ADP_ADVE_ADVERTISE)) {
                /* Do smf_sendAvailable(); */
                AEO_adp_txEntityAvailable();
                /* reannounceTimerTimeout = MAX(1, entityInfo.valid_time / 4), valid_time in 2sec. */
                sm->reannounceTimerTimeout = pAvdeccCfg->valid_time;
                sm->reannounceTimerTimeout *= 2u;
                if (sm->reannounceTimerTimeout < 4u) {
                    sm->reannounceTimerTimeout = 1u;
                } else {
                    sm->reannounceTimerTimeout /= 4u;
                }

                (void)AmbaKAL_GetSysTickCount(&curT);
                sm->reannounceTimerTimeout = (sm->reannounceTimerTimeout * 1000u) + curT;
                /* UCT to next */
                sm->state = S_ADP_ADVE_WAITING;
            }
            /* fall through */
            {

                (void)AmbaKAL_GetSysTickCount(&curT);
                if ((sm->needsAdvertise == TRUE) ||
                    (curT > sm->reannounceTimerTimeout)) {
                    sm->needsAdvertise = FALSE;
                    sm->state = S_ADP_ADVE_ADVERTISE;
                }
                if (sm->doTerminate == TRUE) {
                    sm->doTerminate = FALSE;
                    sm->state = S_ADP_ADVE_IDLE;
                }
            }
            break;

        default:
            /* TODO */
            break;
    }
}


static void smf_txEntityDeparting(void)
{
    advecc_send_adp_pkt(ADP_MSG_ENTITY_DEPARTING);
}

static void smf_txEntityAvailable(void)
{
    AEO_adp_txEntityAvailable();
}

static void avdecc_sm_adp_AdvertiseIF(void)
{
#if 0 // cf. 6.2.1.16, incremented after transmitting an ENTITY_AVAILABLE message
    entityInfo_t *pEI = &(pAvdeccCfg->entityInfo);
#endif
    static AMBA_ENET_CONFIG_s *pEnetConfig = NULL;

    (void)AmbaEnet_GetConfig(0, &pEnetConfig);

    switch (SM_Adp_AdvI.state) {
        case S_ADP_ADVI_IDLE:
            break;

        case S_ADP_ADVI_INITIALIZE:
        case S_ADP_ADVI_WAITING:
        case S_ADP_ADVI_WAITING_EVT:
            if (SM_Adp_AdvI.state == S_ADP_ADVI_INITIALIZE) {
                SM_Adp_AdvI.lastLinkIsUp = FALSE;
                // SM_Adp_AdvI.advertisedGrandmasterID = currentGrandmasterID
                /* UCT to next */
                SM_Adp_AdvI.state = S_ADP_ADVI_WAITING;
            }
            /* fall through, -fallthrough */
            if (SM_Adp_AdvI.state == S_ADP_ADVI_WAITING) {
                SM_Adp_AdvI.rcvdDiscover = FALSE;
#if 0 // cf. 6.2.1.16, incremented after transmitting an ENTITY_AVAILABLE message
                pEI->available_index += 1u;
#endif
                SM_Adp_AdvI.state = S_ADP_ADVI_WAITING_EVT;
            }
            /* fall through, -fallthrough, Waiting events. */
            if (SM_Adp_AdvI.doTerminate == TRUE) {
                SM_Adp_AdvI.state = S_ADP_ADVI_DEPARTING;
                SM_Adp_AdvI.doTerminate = FALSE;
            } else if (SM_Adp_AdvI.doAdvertise == TRUE) {
                SM_Adp_AdvI.state = S_ADP_ADVI_ADVERTISE;
                SM_Adp_AdvI.doAdvertise = FALSE;
            } else if (SM_Adp_AdvI.rcvdDiscover == TRUE) {
                // cf. avdecc_process_adp(). Trigger Entity to send ASAP.
                SM_Adp_AdvI.state = S_ADP_ADVI_RECEIVED_DISCOVER;
                SM_Adp_AdvI.rcvdDiscover = FALSE;
#if 0
            } else if (currentGrandmasterID != SM_Adp_AdvI.advertisedGrandmasterID ) {
                SM_Adp_AdvI.state = S_ADP_ADVI_UPDATE_GM;
#endif
            } else {
#if 1
                if (pEnetConfig->LinkSpeed != 0u) {
                    SM_Adp_AdvI.linkIsUp = TRUE;
                } else {
                    SM_Adp_AdvI.linkIsUp = FALSE;
                }
#else
                SM_Adp_AdvI.linkIsUp = TRUE;
#endif
                if (SM_Adp_AdvI.lastLinkIsUp != SM_Adp_AdvI.linkIsUp) {
                    SM_Adp_AdvI.state = S_ADP_ADVI_LINK_STATE_CHANGE;
                } else {
                    (void)AmbaKAL_TaskSleep(10);
                }
            }
            break;

        case S_ADP_ADVI_DEPARTING:
            smf_txEntityDeparting();
            SM_Adp_AdvI.state = S_ADP_ADVI_IDLE;
            break;

        case S_ADP_ADVI_ADVERTISE:
            smf_txEntityAvailable();
            /* UCT to next */
            SM_Adp_AdvI.state = S_ADP_ADVI_WAITING;
            break;

        case S_ADP_ADVI_RECEIVED_DISCOVER:
            if (SM_Adp_AdvI.entityID == 0u) {
                AEO_adp_Set_needsAdvertise(0ULL);
            } else {
                AEO_adp_Set_needsAdvertise(SM_Adp_AdvI.entityID);
            }
            /* UCT to next */
            SM_Adp_AdvI.entityID = 0;
            SM_Adp_AdvI.state = S_ADP_ADVI_WAITING;
            break;

        case S_ADP_ADVI_UPDATE_GM:
            // TODO: SM_Adp_AdvI.advertisedGrandmasterID = currentGrandmasterID;
            AEO_adp_Set_needsAdvertise(0ULL);
            /* UCT to next */
            SM_Adp_AdvI.state = S_ADP_ADVI_WAITING;
            break;

        case S_ADP_ADVI_LINK_STATE_CHANGE:
            SM_Adp_AdvI.lastLinkIsUp = SM_Adp_AdvI.linkIsUp;
            if (SM_Adp_AdvI.linkIsUp == TRUE) {
                AEO_adp_Set_needsAdvertise(0ULL);
            } else {
                /* Timeout will remove listed entities */
                /* TODO: should notification?! */
            }
            /* UCT to next */
            SM_Adp_AdvI.state = S_ADP_ADVI_WAITING;
            break;

        default:
            /* TODO */
            break;
    }
}


static void smf_txDiscover(UINT64 discoverId)
{
    (void)discoverId;
    // discoverId: 0 to look for all devices Or specific one.
    advecc_send_adp_pkt(ADP_MSG_ENTITY_DISCOVER);
    // TBD
}

#if 0
/* 6.2.6.2.1 performDiscover */
static void avdecc_adp_performDiscover(void);
static void avdecc_adp_performDiscover(void)
{
//    SM_Adp_Disc.doDiscover = TRUE;

    /* TBD: sets discoverID to either zero (0) to search for all AVDECC Entities
       or to the entity_id of an AVDECC Entity to search for. */
//    SM_Adp_Disc.discoverID = 0;
    ;
}
#endif

/*
 * idx to be returned, MAX_RX_EI as failed
 *
 * 6.2.6.3.2 haveEntity(entityID)
 * 6.2.6.3.3 updateEntity(entityInfo)
 * 6.2.6.3.4 addEntity(entityInfo)
 */
UINT8 avdecc_adp_EI_add(rcvdEntityInfo_t *prxEInfo, UINT32 Idx, const pkt_adp_t *p);
UINT8 avdecc_adp_EI_add(rcvdEntityInfo_t *prxEInfo, UINT32 Idx, const pkt_adp_t *p)
{
    UINT8 i;
    UINT8 haveIt = 0U;
    UINT8 addIt = 0U;
    UINT8 err;
    UINT8 toBreak = 0;

    for (i = 0U; i < MAX_RX_EI; i++) {
        if (prxEInfo[i].adpdu.avtpdu_hdr.entity_id == 0u) {
            /* Free to add */
            addIt = 1U;
            toBreak = 1u;
        } else if (prxEInfo[i].adpdu.avtpdu_hdr.entity_id == p->avtpdu_hdr.entity_id) {
            /* Added */
            haveIt = 1U;
            toBreak = 1u;
        } else {
            /* TODO */
            ;
        }
        if (toBreak == 1u) {
            break;
        }
    }
    if (i >= MAX_RX_EI) {
        /* Can not be added. */
        err = MAX_RX_EI;
    } else {
        if (addIt != 0U) {
            // Update
            prxEInfo[i].adpdu.avtpdu_hdr.valid_time = p->avtpdu_hdr.valid_time;
            prxEInfo[i].adpdu.avtpdu_hdr.entity_id = p->avtpdu_hdr.entity_id;
            prxEInfo[i].adpdu.adpdu.entity_model_id = p->adpdu.entity_model_id;
            prxEInfo[i].adpdu.adpdu.entity_cap = p->adpdu.entity_cap;
            prxEInfo[i].adpdu.adpdu.talker_stream_src = p->adpdu.talker_stream_src;
            prxEInfo[i].adpdu.adpdu.talker_cap = p->adpdu.talker_cap;
            prxEInfo[i].adpdu.adpdu.listener_stream_sinks = p->adpdu.listener_stream_sinks;
            prxEInfo[i].adpdu.adpdu.listener_cap = p->adpdu.listener_cap;
            prxEInfo[i].adpdu.adpdu.ctrl_cap = p->adpdu.ctrl_cap;
            prxEInfo[i].adpdu.adpdu.available_index = p->adpdu.available_index;
            prxEInfo[i].adpdu.adpdu.gptp_grandmaster_id = p->adpdu.gptp_grandmaster_id;
            prxEInfo[i].adpdu.adpdu.gptp_domain_no = p->adpdu.gptp_domain_no;
            prxEInfo[i].adpdu.adpdu.id_control_index = p->adpdu.id_control_index;
            prxEInfo[i].adpdu.adpdu.interface_index = p->adpdu.interface_index;
            prxEInfo[i].adpdu.adpdu.association_id = p->adpdu.association_id;
            prxEInfo[i].port = (UINT8)(Idx & 0xFFu);
            prxEInfo[i].rcvdAvailable = TRUE;
        } else if (haveIt != 0U) {
            // TODO: update timer
            ;
        } else {
            // Impossible
            ;
        }
        err = i;
    }

    return err;
}

/*
 * Always 0
 *
 * 6.2.6.3.5 removeEntity(eui64)
 */
UINT8 avdecc_adp_EI_rm(rcvdEntityInfo_t *prxEInfo, UINT32 Idx, const pkt_adp_t *p);
UINT8 avdecc_adp_EI_rm(rcvdEntityInfo_t *prxEInfo, UINT32 Idx, const pkt_adp_t *p)
{
    UINT8 i;

    (void)Idx;
    if ((prxEInfo != NULL) &&
        (p != NULL)) {
        for (i = 0U; i < MAX_RX_EI; i++) {
            if (prxEInfo[i].adpdu.avtpdu_hdr.entity_id == p->avtpdu_hdr.entity_id) {
                /* Found */
                break;
            }
        }
        if (i < MAX_RX_EI) {
            if (AmbaWrap_memset(&(prxEInfo[i]), 0, sizeof(rcvdEntityInfo_t))!= 0U) { }
        }
    }

    return 0;
}


static void avdecc_sm_adp_Discovery(void)
{
    sm_adp_discovery_t *sm = &SM_Adp_Disc;

    switch (sm->state) {
    case S_ADP_DISC_IDLE:
        break;

    case S_ADP_DISC_WAITING:
        if (sm->doTerminate == TRUE) {
            sm->state = S_ADP_DISC_IDLE;
            sm->doTerminate = FALSE;
            break;
        } else if (sm->doDiscover == TRUE) {
            sm->state = S_ADP_DISC_DISCOVER;
            sm->doDiscover = FALSE;
            break;
        } else if (sm->rcvdAvailable == TRUE) {
            sm->state = S_ADP_DISC_AVAILABLE;
            sm->rcvdAvailable = FALSE;
            break;
        } else if (sm->rcvdDeparting == TRUE) {
            sm->state = S_ADP_DISC_DEPARTING;
            sm->rcvdDeparting = FALSE;
            break;
#if 0
        } else if (currentTime >= entities[x].timeout) {
            sm->state = S_ADP_DISC_TIMEOUT;
            break;
#endif
        } else {
#if 1
            /* TODO */
            ;
#else
           (void)AmbaKAL_TaskSleep(10);
           sm->rcvdAvailable = FALSE;
           sm->rcvdDeparting = FALSE;
           sm->doDiscover = FALSE;
#endif
        }
        break;

    case S_ADP_DISC_DISCOVER:
        smf_txDiscover((UINT64)0);
        /* UCT to next */
        sm->state = S_ADP_DISC_WAITING;
        break;

    case S_ADP_DISC_AVAILABLE:
        // TODO: net index
#if 0
        {
            avdecc_pkt_t *pkt = sm->p;
            avdecc_pkt_t *tmp;
            UINT8 cnt;

            while (pkt != NULL) {
                pkt_adp_t *p = (pkt_adp_t *)(pkt->pkt);
                if (avdecc_adp_EI_add(ei->rxEInfo, pkt->idx, p) == 0) {
                    // TBD: Hook cbf for Adding/Updating EI
                    ;
                }
                cnt = avdecc_msgCntInc(pkt);
                tmp = pkt;
                pkt = pkt->next;
                if (cnt == 0) {
                    avdecc_free(tmp);
                }
            }
        }
#endif
        /* UCT to next */
        sm->state = S_ADP_DISC_WAITING;
        break;

    case S_ADP_DISC_DEPARTING:
        // TODO: net index
#if 0
            {
                avdecc_pkt_t *pkt = sm->p;
                avdecc_pkt_t *tmp;
                UINT8 cnt;

                while (pkt != NULL) {
                    pkt_adp_t *p = (pkt_adp_t *)(pkt->pkt);
                    if (avdecc_adp_EI_rm(ei->rxEInfo, pkt->idx, p) == 0) {
                        // TBD: Hook cbf for Removing EI
                        ;
                    }
                    cnt = avdecc_msgCntInc(pkt);
                    tmp = pkt;
                    pkt = pkt->next;
                    if (cnt == 0) {
                        avdecc_free(tmp);
                    }
                }
            }
#endif
        /* UCT to next */
        sm->state = S_ADP_DISC_WAITING;
        break;

    case S_ADP_DISC_TIMEOUT:
        // removeEntity(rcvdEntityInfo.entity_id)
        /* UCT to next */
        sm->state = S_ADP_DISC_WAITING;
        break;

    default:
        /* TODO */
        break;
    }
}


#define ADP_DBG (0)

void avdecc_ntoh_adp(const void *frame)
{
    pkt_adp_t *p;
    UINT32 *ptr;

    /* TODO: frame is not NULL */
    AmbaMisra_TypeCast(&p, &frame);
    {
        const void *vptr = &(p->avtpdu_hdr);
        AmbaMisra_TypeCast(&ptr, &vptr);

        *ptr = Amba_ntohl(*ptr);
    }

#if ADP_DBG
    AmbaPrint_PrintUInt5("control_data_len: %d", p->avtpdu_hdr.control_data_len, 0u, 0u, 0u, 0u);
    AmbaPrint_PrintUInt5("valid_time: %d", p->avtpdu_hdr.valid_time, 0u, 0u, 0u, 0u);
    AmbaPrint_PrintUInt5("message_type: %d", p->avtpdu_hdr.message_type, 0u, 0u, 0u, 0u);
    AmbaPrint_PrintUInt5("version: %d", p->avtpdu_hdr.version, 0u, 0u, 0u, 0u);
    AmbaPrint_PrintUInt5("sv: %d", p->avtpdu_hdr.sv, 0u, 0u, 0u, 0u);
    AmbaPrint_PrintUInt5("subtype: %d", p->avtpdu_hdr.subtype, 0u, 0u, 0u, 0u);
    AmbaPrint_Flush();
#endif

    p->avtpdu_hdr.entity_id = Amba_ntohll(p->avtpdu_hdr.entity_id);
#if ADP_DBG
    {
        UINT32 Tmp1 = ((p->avtpdu_hdr.entity_id & 0xFFFFFF0000000000U) >> 40U);
        UINT32 Tmp2 = ((p->avtpdu_hdr.entity_id & 0x0000000000FFFFFFU));
        AmbaPrint_PrintUInt5("entity_id: %X%X", Tmp1, Tmp2, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
#endif

    /* AVDECC Discovery Protocol Data Unit (ADPDU) Format */
    p->adpdu.entity_model_id = Amba_ntohll(p->adpdu.entity_model_id);
#if ADP_DBG
    AmbaPrint_PrintUInt5("entity_model_id: %llX", p->adpdu.entity_model_id, 0u, 0u, 0u, 0u);
    AmbaPrint_Flush();
#endif
    p->adpdu.entity_cap = Amba_ntohl(p->adpdu.entity_cap);
#if ADP_DBG
    AmbaPrint_PrintUInt5("entity_cap: %lX", p->adpdu.entity_cap, 0u, 0u, 0u, 0u);
    AmbaPrint_Flush();
#endif

    p->adpdu.talker_cap = Amba_ntohs(p->adpdu.talker_cap);
#if ADP_DBG
    AmbaPrint_PrintUInt5("talker_cap: %X", p->adpdu.talker_cap, 0u, 0u, 0u, 0u);
    AmbaPrint_Flush();
#endif
    p->adpdu.talker_stream_src = Amba_ntohs(p->adpdu.talker_stream_src);
#if ADP_DBG
    AmbaPrint_PrintUInt5("talker_stream_src: %X", p->adpdu.talker_stream_src, 0u, 0u, 0u, 0u);
    AmbaPrint_Flush();
#endif

    p->adpdu.listener_cap = Amba_ntohs(p->adpdu.listener_cap);
#if ADP_DBG
    AmbaPrint_PrintUInt5("listener_cap: %X", p->adpdu.listener_cap, 0u, 0u, 0u, 0u);
    AmbaPrint_Flush();
#endif
    p->adpdu.listener_stream_sinks = Amba_ntohs(p->adpdu.listener_stream_sinks);
#if ADP_DBG
    AmbaPrint_PrintUInt5("listener_stream_sinks: %X", p->adpdu.listener_stream_sinks, 0u, 0u, 0u, 0u);
    AmbaPrint_Flush();
#endif

    p->adpdu.ctrl_cap = Amba_ntohl(p->adpdu.ctrl_cap);
#if ADP_DBG
    AmbaPrint_PrintUInt5("ctrl_cap: %lX", p->adpdu.ctrl_cap, 0u, 0u, 0u, 0u);
    AmbaPrint_Flush();
#endif
    p->adpdu.available_index = Amba_ntohl(p->adpdu.available_index);
#if ADP_DBG
    AmbaPrint_PrintUInt5("available_index: %lX", p->adpdu.available_index, 0u, 0u, 0u, 0u);
    AmbaPrint_Flush();
#endif
    p->adpdu.gptp_grandmaster_id = Amba_ntohll(p->adpdu.gptp_grandmaster_id);
#if ADP_DBG
    AmbaPrint_PrintUInt5("gptp_grandmaster_id: %llX", p->adpdu.gptp_grandmaster_id, 0u, 0u, 0u, 0u);
    AmbaPrint_PrintUInt5("gptp_domain_no: %X", p->adpdu.gptp_domain_no, 0u, 0u, 0u, 0u);
    AmbaPrint_Flush();
#endif
    p->adpdu.interface_index = Amba_ntohs(p->adpdu.interface_index);
#if ADP_DBG
    AmbaPrint_PrintUInt5("interface_index: %X", p->adpdu.interface_index, 0u, 0u, 0u, 0u);
    AmbaPrint_Flush();
#endif
    p->adpdu.id_control_index = Amba_ntohs(p->adpdu.id_control_index);
#if ADP_DBG
    AmbaPrint_PrintUInt5("id_control_index: %X", p->adpdu.id_control_index, 0u, 0u, 0u, 0u);
    AmbaPrint_Flush();
#endif
    p->adpdu.association_id = Amba_ntohll(p->adpdu.association_id);
#if ADP_DBG
    AmbaPrint_PrintUInt5("association_id: %llX", p->adpdu.association_id, 0u, 0u, 0u, 0u);
    AmbaPrint_Flush();
#endif
}


void avdecc_process_adp(const void *packet)
{
    const sm_adp_discovery_t *sm = NULL;
    const avdecc_pkt_t *pkt = NULL;
    const pkt_adp_t *p = NULL;
    const UINT8 *cp = NULL;
    const descriptor_entity_t *pED = &(pAvdeccCfg->entity_descriptor);

    if (packet != NULL) {
        sm = &SM_Adp_Disc;
        AmbaMisra_TypeCast(&pkt, &packet);
        cp = pkt->pkt;
        AmbaMisra_TypeCast(&p, &cp);

        (void)sm;

        if ((pkt != NULL) &&
                (p != NULL)) {
            /* Table 6.1message_type field */
            switch (p->avtpdu_hdr.message_type) {
                case ADP_MSG_ENTITY_AVAILABLE:
                    // Add new or update entity
#if 0 // TODO: enqueue packet for later handle
                    sm->rcvdAvailable = TRUE;
#else
                    // Suppose in wating state.
                    if ((pED->entity_id != p->avtpdu_hdr.entity_id) &&
                            (p->avtpdu_hdr.entity_id != 0u)) {
                        UINT8 idx = avdecc_adp_EI_add(pAvdeccCfg->rxEInfo, pkt->idx, p);
                        if (idx != MAX_RX_EI) {
                            if ((pAvdeccCfg->talker != NULL) &&
                                (pAvdeccCfg->talker->cbf_adp_available != NULL)) {
                                pAvdeccCfg->talker->cbf_adp_available(idx);
                            }
                            if ((pAvdeccCfg->listener != NULL) &&
                                (pAvdeccCfg->listener->cbf_adp_available != NULL)) {
                                pAvdeccCfg->listener->cbf_adp_available(idx);
                            }
                        }
                    }
#endif
                    break;

                case ADP_MSG_ENTITY_DEPARTING:
                    // Remove entity
#if 0 // TODO enqueue packet for later handle
                    sm->rcvdDeparting = TRUE;
#else
                    // Suppose in wating state.
                    if ((pED->entity_id != p->avtpdu_hdr.entity_id) &&
                            (p->avtpdu_hdr.entity_id != 0u)) {
                        if (avdecc_adp_EI_rm(pAvdeccCfg->rxEInfo, pkt->idx, p) == 0u) {
                            // TODO: Hook cbf for Removing EI
                            ;
                        }
                    }
#endif
                    break;

                case ADP_MSG_ENTITY_DISCOVER:
                    // cf. RECEIVED DISCOVER of Figure 6.3—Advertise Interface State Machine
#if 0
                    SM_Adp_AdvI.entityID = p->avtpdu_hdr.entity_id;
                    SM_Adp_AdvI.rcvdDiscover = TRUE;
#else
                    /*
                     * Send packet ASAP.
                     */
                    AEO_adp_Set_needsAdvertise(p->avtpdu_hdr.entity_id);
#endif
                    break;

                default:
                    /* TODO */
                    break;
            }
        } else {
            /* TODO */
            ;
        }
    }
}




static void *avdecc_adp_runSM(void *a)
{
    UINT32 loop = 1U;
    (void)a;

    SM_Adp_AdvE.state = S_ADP_ADVE_IDLE;
    SM_Adp_Disc.state = S_ADP_DISC_IDLE;

    while (loop == 1U) {
        UINT8 doTerminate;

        (void)AmbaKAL_TaskSleep(10);

        doTerminate = avdecc_doTerminate();
        SM_Adp_AdvI.doTerminate = doTerminate;
        SM_Adp_AdvE.doTerminate = doTerminate;
        SM_Adp_Disc.doTerminate = doTerminate;

        avdecc_sm_adp_AdvertiseIF();
        avdecc_sm_adp_AdvertiseEntity();
        avdecc_sm_adp_Discovery();

        // TODO: if doTerminate then kill.
    }
    return NULL;
}



#define PRIORITY_ADP_SM         (200u)
#define STK_SIZE_ADP_SM         (0x2000u)

void avdecc_adp_init(void)
{
    static AMBA_KAL_TASK_t tsk_adp_sm __attribute__((section(".bss.noinit")));
    static UINT8 stk_adp_sm[STK_SIZE_ADP_SM] __attribute__((section(".bss.noinit")));
    static char name_adp_sm[] = "adp state-machine";

    UINT32 err;

    if (AmbaWrap_memset(&SM_Adp_AdvI, 0, sizeof(SM_Adp_AdvI))!= 0U) { }
//    SM_Adp_AdvI.state = S_ADP_ADVI_IDLE;
    SM_Adp_AdvI.state = S_ADP_ADVI_INITIALIZE;
    SM_Adp_AdvI.linkIsUp = FALSE;
    SM_Adp_AdvI.lastLinkIsUp = FALSE;
    // SM_Adp_AdvI.advertisedGrandmasterID = currentGrandmasterID;
    SM_Adp_AdvI.advertisedGrandmasterID = 0;

    if (AmbaWrap_memset(&SM_Adp_AdvE, 0, sizeof(SM_Adp_AdvE))!= 0U) { }
    if (AmbaWrap_memset(&SM_Adp_Disc, 0, sizeof(SM_Adp_Disc))!= 0U) { }

    if (AmbaWrap_memset(&tsk_adp_sm, 0, sizeof(tsk_adp_sm))!= 0U) { }
    err = AmbaKAL_TaskCreate(&tsk_adp_sm, name_adp_sm, PRIORITY_ADP_SM,
            avdecc_adp_runSM, NULL,
            stk_adp_sm, STK_SIZE_ADP_SM,
            AMBA_KAL_DONT_START);
    if (err != OK) {
        AmbaPrint_PrintUInt5("AmbaKAL_TaskCreate: 0x%X", err, 0u, 0u, 0u, 0u);
    } else {
        (void)AmbaKAL_TaskSetSmpAffinity(&tsk_adp_sm, ACDECC_CORE);
        (void)AmbaKAL_TaskResume(&tsk_adp_sm);
    }
}

