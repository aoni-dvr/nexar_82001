/**
 *  @file avdecc_private.h
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
 *  @details AVDECC pprivate headers.
 *
 */

#ifndef AVDECC_PRIVATE_H
#define AVDECC_PRIVATE_H

extern AvdeccCfg_t *pAvdeccCfg;

#ifdef  __cplusplus
extern  "C" {
#endif


/*
 *  util.c
 */
void avdecc_eth_prep(const void *buf, UINT8 vlan, const UINT8 *dmac, const UINT8 *smac);
UINT8 cmp_eid(const void *a, const void *b);
void avdecc_timer_init(void);
UINT32 avdecc_timer_reg(void (*func)(void), UINT32 ms, UINT32 cycle);
UINT32 avdecc_clone_msg(const AMBA_KAL_MSG_QUEUE_t *msgQ, const avdecc_pkt_t *msg,
                        UINT8 stype, UINT8 mtype);
UINT16 avdecc_GetStreamVlanId(void);
UINT16 avdecc_GetUniqueId(UINT32 role, UINT16 index);
UINT64 avdecc_GenEID(UINT16 uid);
UINT16 avdecc_GetConnection_Count(void);
UINT64 avdecc_GetEntityModelId(void);
UINT8 avdecc_doTerminate(void);
UINT8 avdecc_IsVlan(void);


/*
 *  avdecc.c
 */
void avdecc_entity_reg(const void *pE, UINT8 role,
                       UINT16 stream, UINT16 capabilities);
UINT32 avdecc_process(const UINT32 Idx,
                      const UINT8 dmac[6],
                      const UINT8 smac[6],
                      const void *L3Frame,
                      UINT16 L3Len);
avdecc_pkt_t *avdecc_alloc(void);
void avdecc_free(avdecc_pkt_t *pkt);
InflightCommand_t * avdecc_ifly_enQ(InflightCommand_t *pHead, UINT8 cmd,
                                    UINT32 timeout, avdecc_pkt_t *pkt,
                                    UINT16 sid);
InflightCommand_t * avdecc_ifly_deQ(InflightCommand_t *pHead,
                                    UINT8 no, UINT16 sid);
InflightCommand_t * avdecc_ifly_timeout(InflightCommand_t *pHead);


/* ADP and ACMP multicast destination MAC address */
extern const UINT8 Avdecc_AdpAcmp_DstMac[6];
/* ID Notifications */
extern const UINT8 Avdecc_IdNotify_DstMac[6];

void avdecc_AddMcastRxHash(const UINT32 ethIdx);


/*
 *  talker.c
 */
UINT32 talker_init(void);

/*
 *  listener.c
 */
UINT32 listener_init(void);

/*
 * adp.c
 */
UINT32 avdecc_adp_txEntityMsg_prep(UINT8 msgType, pkt_adp_t *p);
void avdecc_ntoh_adp(const void *frame);
void avdecc_process_adp(const void *packet);
void avdecc_adp_init(void);
void advecc_send_adp_pkt(UINT8 msgType);

/*
 *  acmp.c
 */
UINT32 avdecc_acmp_txMsg_prep(UINT8 msgType, pkt_acmp_t *p);
void avdecc_ntoh_acmp(const void *frame);
void avdecc_process_acmp(const void *packet);
void avdecc_acmp_init(void);
void avdecc_send_acmp_fastConnCmd(UINT8 idx);
static inline void avdecc_hton_acmp(const void *frame)
{
    /* Change order */
    avdecc_ntoh_acmp(frame);
}
void avdecc_sm_acmp_listener(avdecc_listener_t *l);
void avdecc_sm_acmp_talker(avdecc_talker_t *t);

/*
 *  aecp.c
 */
UINT32 avdecc_aecp_txMsg_prep(UINT8 msgType, pkt_aecp_t *p);
void avdecc_ntoh_aecp(const void *frame);
void avdecc_process_aecp(const void *packet);
void avdecc_aecp_init(void);

#ifdef  __cplusplus
}
#endif

#endif /* AVDECC_PRIVATE_H */


