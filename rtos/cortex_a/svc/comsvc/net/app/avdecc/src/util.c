/**
 *  @file util.c
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
 *  @details Utilities
 *
 */


#include <avdecc.h>
#include <avdecc_private.h>


/*
 *  vlan: Bool, support VLAN or not
 */
void avdecc_eth_prep(const void *buf, UINT8 vlan, const UINT8 *dmac, const UINT8 *smac)
{
    ETH_VLAN_HDR_s *evHdr;
    ETH_HDR_s *eHdr;

    if (vlan != 0U) {
        AmbaMisra_TypeCast(&evHdr, &buf);
        if (AmbaWrap_memcpy(evHdr->dst, dmac, (sizeof(UINT8) * 6u))!= 0U) { }
        if (AmbaWrap_memcpy(evHdr->src, smac, (sizeof(UINT8) * 6u))!= 0U) { }
        // TODO: VLAN
        evHdr->type = Amba_htons(0x22F0u);
    } else {
        AmbaMisra_TypeCast(&eHdr, &buf);
        if (AmbaWrap_memcpy(eHdr->dst, dmac, (sizeof(UINT8) * 6u))!= 0U) { }
        if (AmbaWrap_memcpy(eHdr->src, smac, (sizeof(UINT8) * 6u))!= 0U) { }
        eHdr->type = Amba_htons(0x22F0u);
    }
}


/*
 * 0 as the same, 1 as not the same
 * b is NULL as using zero_EID
 */
UINT8 cmp_eid(const void *a, const void *b)
{
// Empty Entity ID, broadcast
static UINT64 zero_EID = 0;
    const UINT8 *s;
    const UINT8 *d;
    UINT8 i;
    UINT8 err = 0;

    AmbaMisra_TypeCast(&s, &a);
    AmbaMisra_TypeCast(&d, &b);
    if (b == NULL) {
        d = (UINT8 *)&zero_EID;
    } else {
        AmbaMisra_TypeCast(&d, &b);
    }
    for (i = 0; i < sizeof(UINT64); i++) {
        if (s[i] != d[i]) {
            err = 1;
            break;
        }
    }
    return err;
}



static AMBA_KAL_MUTEX_t avdecc_timer_mutex;
static avdecc_timer_t avdecc_swtimer[AVDECC_TIMER_NO];

static void avdecc_timer_expFun(UINT32 arg)
{
    UINT8 i;
    avdecc_timer_t *p;

    (void)arg;

    // TODO: one cycle need to be less than AVDECC_TIMER_MS
    for (i = 0; i < AVDECC_TIMER_NO; i++) {
        p = &(avdecc_swtimer[i]);
        (void)AmbaKAL_MutexTake(&avdecc_timer_mutex, AMBA_KAL_WAIT_FOREVER);
        if (p->enable != 0U) {
            if (p->timer > 0U) {
                p->timer--;
                if (p->timer == 0U) {
                    p->isTO = 1;
                    if (p->func != NULL) {
                        p->func();
                    }
                    if (p->cycle != 0U) {
                        p->cycle--;
                        if (p->cycle != 0U) {
                            p->timer = p->reload_value;
                        } else {
                            p->enable = 0U;
                        }
                    } else {
                        p->timer = p->reload_value;
                    }
                }
            }

        }
        (void)AmbaKAL_MutexGive(&avdecc_timer_mutex);
    }
}

void avdecc_timer_init(void)
{
    static AMBA_KAL_TIMER_t avdecc_timer;
    UINT32 err;

    err = AmbaKAL_MutexCreate(&avdecc_timer_mutex, NULL);
    if (err != 0U) {
//        AmbaPrint("avdecc_timer_mutex create failed.");
        ;
//        return;
    } else {
        err = AmbaKAL_TimerCreate(&avdecc_timer,
                NULL,
                avdecc_timer_expFun,
                0,
                AVDECC_TIMER_MS,
                AVDECC_TIMER_MS,
                AMBA_KAL_AUTO_START);
    }

    if (err != 0U) {
//        AmbaPrint("avdecc_timer create failed.");
        ;
    } else {
        if (AmbaWrap_memset(avdecc_swtimer, 0, sizeof(avdecc_swtimer))!= 0U) { }
    }
}

UINT32 avdecc_timer_reg(void (*func)(void), UINT32 ms, UINT32 cycle)
{
    UINT8 i;
    avdecc_timer_t *p;
    UINT32 err;

    for (i = 0; i < AVDECC_TIMER_NO; i++) {
        p = &(avdecc_swtimer[i]);
        (void)AmbaKAL_MutexTake(&avdecc_timer_mutex, AMBA_KAL_WAIT_FOREVER);
        if (p->enable == 0U) {
            p->enable = 1U;
            p->func = func;
            p->timer = ms / AVDECC_TIMER_MS;
            p->reload_value = p->timer;
            p->cycle = cycle;
            p->isTO = 0;
            (void)AmbaKAL_MutexGive(&avdecc_timer_mutex);
            break;
        }
        (void)AmbaKAL_MutexGive(&avdecc_timer_mutex);
    }

    if (i < AVDECC_TIMER_NO) {
        err = 0u;
    } else {
        err = 1u;
    }

    return err;
}


UINT32 avdecc_clone_msg(const AMBA_KAL_MSG_QUEUE_t *msgQ, const avdecc_pkt_t *msg,
                        UINT8 stype, UINT8 mtype)
{
    UINT32 err = 0;
    avdecc_pkt_t *pkt;

    if ((msgQ != NULL) && (msgQ->tx_queue_id != 0u)) {
        pkt = avdecc_alloc();
        if (pkt != NULL) {
            AMBA_KAL_MSG_QUEUE_t *q;

            if (AmbaWrap_memcpy(pkt, msg, (AVDECC_FSIZE - AVDECC_PSIZE))!= 0U) { }
            if (AmbaWrap_memcpy(pkt->pkt, msg->pkt, msg->len)!= 0U) { }
            pkt->stype = stype;
            pkt->mtype = mtype;
            AmbaMisra_TypeCast(&q, &msgQ);
            (void)AmbaKAL_MsgQueueSend(q, &pkt, AMBA_KAL_WAIT_FOREVER);
        } else {
            err = 1u;
        }
    } else {
        err = 1u;
    }

    return err;
}

/**
 *  Get stream_vlan_id
 */
UINT16 avdecc_GetStreamVlanId(void)
{
    return 0x4567u;
}

/**
 *  Get xxx_unique_id
 *
 *  @param [in] role    AVDECC_XXXX
 *  @param [in] index   Index of role, start from 0
 */
UINT16 avdecc_GetUniqueId(UINT32 role, UINT16 index)
{
    UINT16 id = 0;

    switch (role) {
    case AVDECC_CONTROLLER:
        id |= 0x8000u;
        break;

    case AVDECC_TALKER:
        id |= 0x4000u;
        break;

    case AVDECC_LISTENER:
        id |= 0x2000u;
        break;

    case AVDECC_RESPONDER:
        id |= 0x1000u;
        break;

    default:
        /* TODO */
        break;
    }

    id += (index & 0x00FFu);

    return id;
}


/**
 *  Generate Entity ID
 *
 *  param [in] uid      Unique ID
 */
UINT64 avdecc_GenEID(UINT16 uid)
{
    UINT64 id = 0u;
    AMBA_ENET_CONFIG_s *pEnetConfig;
    UINT32 err;

    err = AmbaEnet_GetConfig(0, &pEnetConfig);
    if (err == 0u) {
        UINT8 i;

        id = 0u;
        for (i = 0; i < 3u; i++) {
            id *= 0x100u;
            id += pEnetConfig->Mac[i];
        }
        id *= 0x10000u;
        id += uid;
        for (; i < 6u; i++) {
            id *= 0x100u;
            id += pEnetConfig->Mac[i];
        }
    }

    return id;
}


/**
 *  Get connection_count
 */
UINT16 avdecc_GetConnection_Count(void)
{
    return 0x5678u;
}


/*
 * Get entity_model_id, cf. 6.2.1.9 entity_model_id field
 */
UINT64 avdecc_GetEntityModelId(void)
{
    return 0x445566778899AABBu;
}


/**
 *  a Boolean indicating that a request has been made to terminate AVDECC.
 *  @return
 *      TRUE (YES), FALSE (NO)
 */
UINT8 avdecc_doTerminate(void)
{
    UINT8 v = FALSE;

    if (pAvdeccCfg->doTerminate != 0u) {
        v = TRUE;
    }
    return v;
}


/**
 * Is VLAN supported.
 * @return
 *      TRUE (YES), FALSE (NO)
 */
UINT8 avdecc_IsVlan(void)
{
    UINT8 v = FALSE;

    if (pAvdeccCfg->IsSupportVPN != 0u) {
        v = TRUE;
    }
    return v;
}

