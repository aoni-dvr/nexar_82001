/**
 *  @file avdecc.c
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
 *  @details Processing AVDECC packets.
 *
 */

#include <avdecc.h>
#include <avdecc_private.h>



AvdeccCfg_t *pAvdeccCfg = NULL;


void avdecc_entity_reg(const void *pE, UINT8 role,
                       UINT16 stream, UINT16 capabilities)
{
    if (pE != NULL) {
        switch (role) {
        case AVDECC_TALKER:
            if (pAvdeccCfg != NULL) {
                AmbaMisra_TypeCast(&(pAvdeccCfg->talker), &pE);
                pAvdeccCfg->entityInfo.talker_stream_src = stream;
                pAvdeccCfg->entityInfo.talker_cap = capabilities;
            }
            break;

        case AVDECC_LISTENER:
            if (pAvdeccCfg != NULL) {
                AmbaMisra_TypeCast(&(pAvdeccCfg->listener), &pE);
                pAvdeccCfg->entityInfo.listener_stream_sinks = stream;
                pAvdeccCfg->entityInfo.listener_cap = capabilities;
            }
            break;

        case AVDECC_CONTROLLER:
        case AVDECC_RESPONDER:
        default:
            /* TODO */
            break;
        }
    }
}

void avdecc_entity_unreg(void *pE);
void avdecc_entity_unreg(void *pE)
{
    (void)pE;
}


static AMBA_KAL_MSG_QUEUE_t avdecc_msgQ __attribute__((section(".bss.noinit")));
#define AVDECC_MSGQ_NUM     (32U)
//#define AVDECC_MSGQ_SIZE    ((UINT32)sizeof(void *) * AVDECC_MSGQ_NUM)
#define AVDECC_MSGQ_SIZE    (4U * AVDECC_MSGQ_NUM)
/* Lock when updating count of message handler, when zero, message should be freed. */
//static AMBA_KAL_MUTEX_t avdecc_msgQ_lk __attribute__((section(".bss.noinit")));

#define PRIORITY_EHE    (200u)
#define STK_SIZE_EHE    (0x2000u)

/* Pass message to entities */
static void *avdecc_entity_hdlEvent(void *arg)
{
    avdecc_pkt_t *pkt;
    UINT8 subtype;
    void *pkt_ptr = NULL;
    UINT32 loop = 1U;

    (void)arg;
    while (loop == 1U) {
        (void)pkt_ptr;
        (void)AmbaKAL_MsgQueueReceive(&avdecc_msgQ, &pkt_ptr, AMBA_KAL_WAIT_FOREVER);
        AmbaMisra_TypeCast(&pkt, &pkt_ptr);

        /* Note: for IEEE 1722.1 - 2013, here is 1-bit 'cd' field */
        if (AmbaWrap_memcpy(&subtype, pkt->pkt, sizeof(UINT8))!= 0U) { }

        switch (subtype) {
        case AVTPDU_SUBTYPE_ADP:
//            AmbaPrint("AVTPDU_SUBTYPE_ADP");
            avdecc_ntoh_adp(pkt->pkt);
            avdecc_process_adp(pkt);
            avdecc_free(pkt);
            break;

        case AVTPDU_SUBTYPE_AECP:
//            AmbaPrint("AVTPDU_SUBTYPE_AECP");
            avdecc_ntoh_aecp(pkt->pkt);
            avdecc_process_aecp(pkt);
            avdecc_free(pkt);
            break;

        case AVTPDU_SUBTYPE_ACMP:
//            AmbaPrint("AVTPDU_SUBTYPE_ACMP");
            avdecc_ntoh_acmp(pkt->pkt);
            avdecc_process_acmp(pkt);
            avdecc_free(pkt);
            break;

        default:
//            AmbaPrint("TODO: %s, %d", __func__, __LINE__);
            avdecc_free(pkt);
            break;
        }
    }
    return NULL;
}

/* Pass message to entities */
UINT32 avdecc_process(const UINT32 Idx,
                      const UINT8 dmac[6],
                      const UINT8 smac[6],
                      const void *L3Frame,
                      UINT16 L3Len)
{
    avdecc_pkt_t *pkt;
    UINT32 err = 0;

    pkt = avdecc_alloc();
    if (pkt != NULL) {
        pkt->idx = Idx;
        pkt->len = L3Len;
        if (AmbaWrap_memcpy(pkt->dmac, dmac, 6)!= 0U) { }
        if (AmbaWrap_memcpy(pkt->smac, smac, 6)!= 0U) { }
        if (AmbaWrap_memcpy(pkt->pkt, L3Frame, L3Len)!= 0U) { }
        // Trigger Entities to handle events.
        (void)AmbaKAL_MsgQueueSend(&avdecc_msgQ, &pkt, AMBA_KAL_NO_WAIT);
    } else {
        // TODO
        err = 1;
    }

    return err;
}

/* Number of free InflightCommand_t bag to store frame. */
#define FREE_IFNUM    (20U)
static InflightCommand_t *freeIFly = NULL;

/** Initialize InflightCommand bags */
static void avdecc_ifly_init(void)
{
static InflightCommand_t avdecc_ifly_pool[FREE_IFNUM] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
    UINT8 i;

    freeIFly = avdecc_ifly_pool;
    for (i = 0U; i < (FREE_IFNUM - 1U); i++) {
        avdecc_ifly_pool[i].no = i;
        avdecc_ifly_pool[i].retried = FALSE;
        avdecc_ifly_pool[i].next = &(avdecc_ifly_pool[i+1u]);
    }
    avdecc_ifly_pool[i].no = i;
    avdecc_ifly_pool[i].retried = FALSE;
    avdecc_ifly_pool[i].next = NULL;
}

InflightCommand_t *avdecc_ifly_alloc(void);
/** Allocate one InflightCommand_t bag */
InflightCommand_t *avdecc_ifly_alloc(void)
{
    InflightCommand_t *p = freeIFly;

    if (freeIFly != NULL) {
        freeIFly = freeIFly->next;
    }
    return p;
}

void avdecc_ifly_free(InflightCommand_t *p);
/** Release/Free one InflightCommand_t bag */
void avdecc_ifly_free(InflightCommand_t *p)
{
    if (p != NULL) {
        p->next = freeIFly;
        freeIFly = p;
    }
}

/**
 *  Enqueue a InflightCommand_t bag with a AVDECC packet.
 *
 *  @param [in] pHead   Owner's head of InflightCommand_t bag/list.
 *  @param [in] timeout Timeout of waiting response of command.
 *  @param [in] cmd     Command sent
 *  @param [in] pkt     A packet refered to.
 *  @return
 *      New pHead of list
 */
InflightCommand_t * avdecc_ifly_enQ(InflightCommand_t *pHead, UINT8 cmd,
                                    UINT32 timeout, avdecc_pkt_t *pkt,
                                    UINT16 sid)
{
    InflightCommand_t *ifly = NULL;

    if (pkt != NULL) {
        ifly = avdecc_ifly_alloc();
        if (ifly != NULL) {
            UINT32 curT;

            (void)AmbaKAL_GetSysTickCount(&curT);
            ifly->timeout =  curT + timeout;
            ifly->cmd = cmd;
            ifly->pkt = pkt;
            ifly->sid = sid;
            ifly->retried = FALSE;
            ifly->next = pHead;
        }
    }

    return ifly;
}

/**
 *  Dequeue a InflightCommand_t bag and free a AVDECC packet.
 *  Search bag by no or sequence_id (sid, and set no as 0xFFu)
 *
 *  @param [in] pHead   Owner's head of InflightCommand_t bag/list.
 *  @param [in] no      InflightCommand_t.no that will be dequeued.
 *  @param [in] sid     InflightCommand_t.sid that will be dequeued.
 *  @return
 *      New pHead of list
 */
InflightCommand_t * avdecc_ifly_deQ(InflightCommand_t *pHead,
                                    UINT8 no, UINT16 sid)
{
    InflightCommand_t *ifly = NULL;
    InflightCommand_t *prev;

    if (pHead != NULL) {
        ifly = pHead;
        prev = pHead;
        for (;;) {
            UINT8 toBreak = 0;

            if (ifly == NULL) {
                toBreak = 1u;
            } else if (ifly->no == no) {
                toBreak = 1u;
            } else if (ifly->sid == sid) {
                toBreak = 1u;
            } else {
                prev = ifly;
                ifly = ifly->next;
            }

            if (toBreak == 1u) {
                break;
            }
        }
        if (ifly != NULL) {
            avdecc_free(ifly->pkt);
            ifly->pkt = NULL;
            ifly->retried = FALSE;

            if (ifly == pHead) {
                ifly = pHead->next;
                avdecc_ifly_free(pHead);
            } else {
                prev->next = ifly->next;
                avdecc_ifly_free(ifly);
                ifly = pHead;
            }
        } else {
            ifly = pHead;
        }
    }

    return ifly;
}

/**
 *  Find the 1st timeout InflightCommand_t bag.
 *
 *  @param [in] pHead   Owner's head of InflightCommand_t bag/list.
 *  @return
 *      1st timeout InflightCommand_t bag or NULL.
 */
InflightCommand_t * avdecc_ifly_timeout(InflightCommand_t *pHead)
{
    InflightCommand_t *ifly = pHead;
    UINT32 curT;

    (void)AmbaKAL_GetSysTickCount(&curT);

    for (;;) {
        UINT8 toBreak = 0u;

        if (ifly == NULL) {
            toBreak = 1u;
        } else if (curT >= ifly->timeout) {
            toBreak = 1u;
        } else {
            ifly = ifly->next;
        }

        if (toBreak == 1u) {
            break;
        }
    }

    return ifly;
}


/* Number of free buffer to store frame. */
#define FREE_FNUM    (20U)

typedef struct avdecc_buf_s {
    avdecc_pkt_t pkt;
    struct avdecc_buf_s *next;
} avdecc_buf_t;

static avdecc_buf_t *freePool = NULL;

static void avdecc_pool_init(void)
{
    static avdecc_buf_t avdecc_pool[FREE_FNUM] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
    static UINT8 stk_ehe[STK_SIZE_EHE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
    static AMBA_KAL_TASK_t tsk_EHE;
    static UINT8 avdecc_msgQ_pool[AVDECC_MSGQ_SIZE] __attribute__((section(".bss.noinit")));

    UINT32 i;
    UINT32 err;
    static char name_msgQ[] = "AVDECC msgQ";
    static char name_task[] = "AVDECC evt hdler";

    freePool = avdecc_pool;
    for (i = 0U; i < (FREE_FNUM - 1U); i++) {
        avdecc_pool[i].next = &(avdecc_pool[i+1u]);
    }
    avdecc_pool[FREE_FNUM - 1U].next = NULL;

    avdecc_ifly_init();

    err = AmbaKAL_MsgQueueCreate(&avdecc_msgQ,
                                 name_msgQ,
                                 sizeof(void *),
                                 avdecc_msgQ_pool,
                                 AVDECC_MSGQ_SIZE);
    /* TODO: err */
    (void)err;
//    err = AmbaKAL_MutexCreate(&avdecc_msgQ_lk, "AVDECC msgQ");
    err = AmbaKAL_TaskCreate(&tsk_EHE, name_task, PRIORITY_EHE,
                             avdecc_entity_hdlEvent, NULL,
                             stk_ehe, STK_SIZE_EHE,
                             AMBA_KAL_DONT_START);
    if (err != OK) {
        AmbaPrint_PrintUInt5("AmbaKAL_TaskCreate: 0x%X", err, 0u, 0u, 0u, 0u);
    } else {
        (void)AmbaKAL_TaskSetSmpAffinity(&tsk_EHE, ACDECC_CORE);
        (void)AmbaKAL_TaskResume(&tsk_EHE);
    }
}

avdecc_pkt_t *avdecc_alloc(void)
{
    avdecc_buf_t *p = freePool;
    avdecc_pkt_t *ppkt = NULL;

    if (freePool != NULL) {
        freePool = freePool->next;
        p->pkt.free = FALSE;
        ppkt = &(p->pkt);
    }
    return ppkt;
}

void avdecc_free(avdecc_pkt_t *pkt)
{
    avdecc_buf_t *p;

    if (pkt == NULL) {
//        return;
        ;
    } else {
        if (pkt->free == TRUE) {
//        AmbaPrint("Re-free buffer");
            ;
        } else {
            pkt->free = TRUE;
            AmbaMisra_TypeCast(&p, &pkt);
            p->next = freePool;
            freePool = p;
        }
    }
}


#if 0
void avdecc_msgLk(void)
{
    (void)AmbaKAL_MutexTake(&avdecc_msgQ_lk, AMBA_KAL_WAIT_FOREVER);
}

void avdecc_msgUnlk(void)
{
    (void)AmbaKAL_MutexGive(&avdecc_msgQ_lk);
}

UINT8 avdecc_msgCntInc(avdecc_pkt_t *pkt)
{
    UINT8 cnt;

    avdecc_msgLk();
    pkt->cnt += 1;
    cnt = pkt->cnt;
    avdecc_msgUnlk();

    return cnt;
}

UINT8 avdecc_msgCntDec(avdecc_pkt_t *pkt)
{
    UINT8 cnt;

    avdecc_msgLk();
    if (pkt->cnt > 0U) {
        pkt->cnt -= 1;
    }
    cnt = pkt->cnt;
    avdecc_msgUnlk();

    return cnt;
}

void avdecc_msgPopHdler(avdecc_pkt_t *pkt)
{
    UINT8 cnt;

    cnt = avdecc_msgCntDec(pkt);
    if (cnt == 0U) {
        avdecc_free(pkt);
    }
}
#endif



/*
 * cf. IEEE Std 1722.1-2013
 */
/*
 * Table B.1—AVDECC Multicast MAC Addresses
 */
/* ADP and ACMP multicast destination MAC address */
const UINT8 Avdecc_AdpAcmp_DstMac[6] = { 0x91u, 0xE0u, 0xF0u, 0x01u, 0x00u, 0x00u };
/* ID Notifications */
const UINT8 Avdecc_IdNotify_DstMac[6] = { 0x91u, 0xE0u, 0xF0u, 0x01u, 0x00u, 0x01u };

void avdecc_AddMcastRxHash(const UINT32 ethIdx)
{
    (void)AmbaEnet_McastRxHash(ethIdx, Avdecc_AdpAcmp_DstMac);
    (void)AmbaEnet_McastRxHash(ethIdx, Avdecc_IdNotify_DstMac);
}

/**
 *  Init AVDECC entities
 *  @param [in] pCfg    Configuration
 *  @param [in] t       enable talker or not
 *  @param [in] l       enable listener or not
 */
void avdecc_aem_init(AvdeccCfg_t *pCfg, UINT8 t, UINT8 l)
{
    if (pCfg != NULL) {
        pAvdeccCfg = pCfg;
        avdecc_timer_init();
        avdecc_pool_init();

        /* ACMP */
        avdecc_acmp_init();

        /* AECP */
        avdecc_aecp_init();

        if (t != 0u) {
            AmbaPrint_PrintStr5("Start Talker", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            (void)talker_init();
        }
        if (l != 0u) {
            AmbaPrint_PrintStr5("Start Listener", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            (void)listener_init();
        }

        /* MultiCast */
        avdecc_AddMcastRxHash(0);

        /* ADP, start it at last step */
        avdecc_adp_init();
    } else {
        AmbaPrint_PrintStr5("AVDECC got failed: Empty AvdeccCfg",
                    NULL, NULL, NULL, NULL, NULL);
    }
}

