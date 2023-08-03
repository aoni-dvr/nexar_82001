/**
 *  @file acmp.c
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
 *  @details AVDECC Connection Management Protocol
 *
 */

#include <avdecc.h>
#include <avdecc_private.h>


// TBD ---- start
#if 0
static UINT64 my_id = pAvdeccCfg->entity_descriptor.entity_id;
#endif

static UINT8 doTerminate(void)
{
    return avdecc_doTerminate();
}

static UINT8 doCommand(void)
{
    return FALSE;
}


/* TODO: cf. AVTP.c */
/* 91:e0:f0:00:0e:80: 91:e0:f0:00:00:00 ~ 91:e0:f0:00:fd:ff MAAP dynamic allocation pool */
static const UINT8 AVTP_DST[6] = { 0x91U, 0xe0U, 0xf0U, 0x00U, 0x0eU, 0x80U };

// TBD -- end



UINT32 avdecc_acmp_txMsg_prep(UINT8 msgType, pkt_acmp_t *p)
{
    if (AmbaWrap_memset(p, 0, sizeof(pkt_acmp_t))!= 0U) { }

    /* AVTP Format */
    switch (msgType) {
    case ACMP_MSG_CONNECT_TX_COMMAND:
    case ACMP_MSG_CONNECT_TX_RESPONSE:
    case ACMP_MSG_DISCONNECT_TX_COMMAND:
    case ACMP_MSG_DISCONNECT_TX_RESPONSE:
    case ACMP_MSG_GET_TX_STATE_COMMAND:
    case ACMP_MSG_GET_TX_STATE_RESPONSE:
    case ACMP_MSG_CONNECT_RX_COMMAND:
    case ACMP_MSG_CONNECT_RX_RESPONSE:
    case ACMP_MSG_DISCONNECT_RX_COMMAND:
    case ACMP_MSG_DISCONNECT_RX_RESPONSE:
    case ACMP_MSG_GET_RX_STATE_COMMAND:
    case ACMP_MSG_GET_RX_STATE_RESPONSE:
    case ACMP_MSG_GET_TX_CONNECTION_COMMAND:
    case ACMP_MSG_GET_TX_CONNECTION_RESPONSE:
        p->avtpdu_hdr.message_type = msgType;
        break;

    default:
        /* TODO */
        break;
    }

    p->avtpdu_hdr.control_data_len = AVDECC_CD_LEN_ACMP;
    p->avtpdu_hdr.status           = ACMP_SS_SUCCESS;
    p->avtpdu_hdr.version          = 0;
    p->avtpdu_hdr.sv               = 0;
    p->avtpdu_hdr.subtype          = AVTPDU_SUBTYPE_ACMP;
    {
        const void *vptr = &(p->avtpdu_hdr);
        UINT32 *ptr;
        AmbaMisra_TypeCast(&ptr, &vptr);

        *ptr = Amba_htonl(*ptr);
    }

    return 0;
}


static UINT16 avdecc_acmp_prepPktByClone(const avdecc_pkt_t *pkt,
                                         char **pbuf, pkt_acmp_t **pp)
{
    UINT32 size;
    char *buf;
    const char *bufL3;
    void *ptr;
    pkt_acmp_t *p;

    (void)AmbaEnet_GetTxBuf(0, &ptr);
    AmbaMisra_TypeCast(&buf, &ptr);

    if (avdecc_IsVlan() == TRUE) {
        bufL3 = &buf[sizeof(ETH_VLAN_HDR_s)];
        size = sizeof(ETH_VLAN_HDR_s);
    } else {
        bufL3 = &buf[sizeof(ETH_HDR_s)];
        size = sizeof(ETH_HDR_s);
    }

    AmbaMisra_TypeCast(&p, &bufL3);
    size += sizeof(pkt_acmp_t);

    if (AmbaWrap_memcpy(p, pkt->pkt, sizeof(pkt_acmp_t))!= 0U) { }

    *pbuf = buf;
    *pp   = p;

    (void)pbuf;
    (void)pp;

    return (UINT16)(size & 0xFFFFu);
}


/**
 * Send CONNECT_TX_COMMAND by Listener
 */
void avdecc_send_acmp_fastConnCmd(UINT8 idx)
{
    UINT8 adp_mac[6] = ADP_MULTICAST_MAC;

    UINT32 size;
    const char * buf;
    const char *pbuf;
    void *ptr;
    pkt_acmp_t *p;
    UINT32 err;

    (void)AmbaEnet_GetTxBuf(0, &ptr);
    AmbaMisra_TypeCast(&buf, &ptr);

    if (avdecc_IsVlan() == TRUE) {
        pbuf = &buf[sizeof(ETH_VLAN_HDR_s)];
        size = sizeof(ETH_VLAN_HDR_s);
    } else {
        pbuf = &buf[sizeof(ETH_HDR_s)];
        size = sizeof(ETH_HDR_s);
    }

    AmbaMisra_TypeCast(&p, &pbuf);
    size += sizeof(pkt_acmp_t);

    err = avdecc_acmp_txMsg_prep(ACMP_MSG_CONNECT_TX_COMMAND, p);
    if (err == 0U) {
        avdecc_listener_t *Listener;

        AmbaMisra_TypeCast(&Listener, &ptr);
        /* AVDECC Connection Management Protocol Data Unit (ACMPDU) Format */
        p->acmpdu.talker_entity_id   = Amba_htonll(pAvdeccCfg->rxEInfo[idx].adpdu.avtpdu_hdr.entity_id);
        p->acmpdu.listener_entity_id = Amba_htonll(pAvdeccCfg->entity_descriptor.entity_id);

        p->acmpdu.listener_unique_id = Amba_htons(Listener->uid);

        p->acmpdu.connection_count = Amba_htons(avdecc_GetConnection_Count());

        if (AmbaWrap_memcpy(p->acmpdu.stream_dest_mac, AVTP_DST, 6u)!= 0U) { }

        p->acmpdu.flags          = Amba_htons(ACMP_FLG_FAST_CONNECT);
        p->acmpdu.sequence_id    = Amba_htons(Listener->sm.sequence_id);
        Listener->sm.sequence_id = Listener->sm.sequence_id + 1u;
        p->acmpdu.stream_vlan_id = Amba_htons(avdecc_GetStreamVlanId());

        avdecc_eth_prep(buf, avdecc_IsVlan(), adp_mac, pAvdeccCfg->getSMac());
        {
            UINT16 sz = (UINT16)(size & 0xFFFFu);

            pAvdeccCfg->test_tx(0, buf, sz);
        }
    }
}


static void avdecc_acmp_txPktByClone(const avdecc_pkt_t *pkt,
                                     UINT8 messageType, UINT8 status)
{
    UINT8 adp_mac[6] = ADP_MULTICAST_MAC;

    UINT16 sz;
    char *buf;
    pkt_acmp_t *p;

    sz = avdecc_acmp_prepPktByClone(pkt, &buf, &p);

    p->avtpdu_hdr.message_type = messageType;
    p->avtpdu_hdr.status       = status;

    avdecc_hton_acmp(p);
    avdecc_eth_prep(buf, avdecc_IsVlan(), adp_mac, pAvdeccCfg->getSMac());

    pAvdeccCfg->test_tx(0, buf, sz);
}


/* 8.2.2.4 ACMP Controller state machine */
typedef struct {
#if 0
    void *inflight;         /**< A dynamic list of InflightCommands that are in the process of being performed. */
#endif
    UINT8 rcvdResponse;     /**< set to TRUE when the rcvdCmdResp variable is set with an AVDECC Controller rsponse ACMPDU */
    UINT8 state;

#if 0
    UINT16 sequence_id;     /**< Used for ACMP, 8.2.1.8 stream_id field */
#endif

    // txCommand(messageType, command, retry)
    // cancelTimeout(commandResponse)
    // removeInflight(commandResponse)
    // processResponse(commandResponse)
    // makeCommand(params)

//    pkt_acmp_t *p;
} sm_acm_controller_t;

#define PRIORITY_ACMP_CONTROLLER (200u)
#define STK_SIZE_ACMP_CONTROLLER (0x2000u)

static sm_acm_controller_t SM_Acmp_Controller;
#define S_ACMP_CTRL_IDLE         (0U)
#define S_ACMP_CTRL_WAITING      (1U)
#define S_ACMP_CTRL_COMMAND      (2U)
#define S_ACMP_CTRL_TIMEOUT      (3U)
#define S_ACMP_CTRL_RESPONSE     (4U)


void *avdecc_sm_acmp_controller(void *a);
void *avdecc_sm_acmp_controller(void *a)
{
    UINT32 loop = 1U;

    (void)a;
    while (loop == 1U) {
        (void)AmbaKAL_TaskSleep(10);

        switch (SM_Acmp_Controller.state) {
        case S_ACMP_CTRL_IDLE:
            break;

        case S_ACMP_CTRL_WAITING:
            SM_Acmp_Controller.rcvdResponse = FALSE;
            for (;;) {
                UINT8 toBreak = 0u;

                if (doTerminate() == TRUE) {
                    SM_Acmp_Controller.state = S_ACMP_CTRL_IDLE;
                    toBreak = 1u;
                } else if (doCommand() == TRUE) {
                    SM_Acmp_Controller.state = S_ACMP_CTRL_COMMAND;
                    toBreak = 1u;
#if 0
                } else if (currentTime >= inflight[x].timeout) {
                    SM_Acmp_Controller.state = S_ACMP_CTRL_TIMEOUT;
                    toBreak = 1u;
                } else if ((SM_Acmp_Controller.rcvdResponse == TRUE) &&
                           (rcvdCmdResp.controller_entity_id == my_id) &&
                           (rcvdCmdResp.sequence_id == inflight[x].command.sequence_id) &&
                           (rcvdCmdResp.message_type == inflight[x].message_type)
                           ) {
                    SM_Acmp_Controller.state = S_ACMP_CTRL_RESPONSE;
                    toBreak = 1u;
#endif
                } else {
                    (void)AmbaKAL_TaskSleep(10);
                }
                if (toBreak == 1u) {
                    break;
                }
            }
            break;

        case S_ACMP_CTRL_COMMAND:
#if 0
            command = makeCommand(commandParams);
            txCommand(command.message_type, command, FALSE);
#endif
            /* UCT to next */
            SM_Acmp_Controller.state = S_ACMP_CTRL_WAITING;
            break;

        case S_ACMP_CTRL_TIMEOUT:
#if 0
            if (inflight[x].retried) {
                removeInflight(inflight[x].command);
                // timeout
            } else {
                txCommand(inflight[x].command.message_type, inflight[x].command, TRUE);
            }

#endif
            /* UCT to next */
            SM_Acmp_Controller.state = S_ACMP_CTRL_WAITING;
            break;

        case S_ACMP_CTRL_RESPONSE:
#if 0
            cancleTimeout(rcvdCmdResp);
            processResponse(rcvdCmdResp);
            removeInflight(rcvdCmdResp);
#endif
            /* UCT to next */
            SM_Acmp_Controller.state = S_ACMP_CTRL_WAITING;
            break;

        default:
            /* TODO */
            break;
        }
    }
    return NULL;
}



#define S_ACMP_LSTN_IDLE                   (0U)
#define S_ACMP_LSTN_WAITING                (1U)
#define S_ACMP_LSTN_CONNECT_TX_TIMEOUT     (2U)
#define S_ACMP_LSTN_DISCONNECT_TX_TIMEOUT  (3U)
#define S_ACMP_LSTN_CONNECT_RX_COMMAND     (4U)
#define S_ACMP_LSTN_CONNECT_TX_RESPONSE    (5U)
#define S_ACMP_LSTN_GET_STATE              (6U)
#define S_ACMP_LSTN_DISCONNECT_RX_COMMAND  (7U)
#define S_ACMP_LSTN_DISCONNECT_TX_RESPONSE (8U)



void avdecc_sm_acmp_listener(avdecc_listener_t *l)
{
    InflightCommand_t *TimeoutCmd = avdecc_ifly_timeout(l->sm.ifly_acmp);
    UINT32 curT;

    // Check inflight status and handle timeout immediately.
    if (TimeoutCmd != NULL) {
        if (TimeoutCmd->cmd == ACMP_MSG_CONNECT_TX_COMMAND) {
            // S_ACMP_LSTN_CONNECT_TX_TIMEOUT
            /*
             *  if (inflight[x].retried == TRUE) {
             *      response             = inflight[x].command;
             *      response.sequence_id = inflight[x].original_sequence_id;
             *      txResponse(ACMP_MSG_CONNECT_RX_RESPONSE, response, LISTENER_TALKER_TIMEOUT);
             *      removeInflight(inflight[x].command);
             *  } else {
             *      txCommand(ACMP_MSG_CONNECT_TX_COMMAND, inflight[x].command, TRUE);
             *  }
             */

            if (TimeoutCmd->retried == TRUE) {
                avdecc_acmp_txPktByClone(TimeoutCmd->pkt,
                                         ACMP_MSG_CONNECT_RX_RESPONSE,
                                         ACMP_SS_LISTENER_TALKER_TIMEOUT);

                l->sm.ifly_acmp = avdecc_ifly_deQ(l->sm.ifly_acmp,
                                                  TimeoutCmd->no, 0);
            } else {
                // Try again
                avdecc_acmp_txPktByClone(TimeoutCmd->pkt,
                                         TimeoutCmd->cmd,
                                         0);
                TimeoutCmd->retried = TRUE;
                (void)AmbaKAL_GetSysTickCount(&curT);
                TimeoutCmd->timeout = curT + AVDECC_TO_ACMP_CONNECT_TX_COMMAND;
            }
        } else if (TimeoutCmd->cmd == ACMP_MSG_DISCONNECT_TX_COMMAND) {
            // S_ACMP_LSTN_DISCONNECT_TX_TIMEOUT
            /*
             *  if (inflight[x].retried == TRUE) {
             *      response             = inflight[x].command;
             *      response.sequence_id = inflight[x].original_sequence_id;
             *      txResponse(ACMP_MSG_DISCONNECT_RX_RESPONSE, response, LISTENER_TALKER_TIMEOUT);
             *      removeInflight(inflight[x].command);
             *  } else {
             *      txCommand(ACMP_MSG_DISCONNECT_TX_COMMAND, inflight[x].command, TRUE);
             *  }
             */
            if (TimeoutCmd->retried == TRUE) {
                avdecc_acmp_txPktByClone(TimeoutCmd->pkt,
                                         ACMP_MSG_DISCONNECT_RX_RESPONSE,
                                         ACMP_SS_LISTENER_TALKER_TIMEOUT);

                l->sm.ifly_acmp = avdecc_ifly_deQ(l->sm.ifly_acmp,
                                                  TimeoutCmd->no, 0);
            } else {
                // Try again
                avdecc_acmp_txPktByClone(TimeoutCmd->pkt,
                                         TimeoutCmd->cmd,
                                         0);
                TimeoutCmd->retried = TRUE;
                (void)AmbaKAL_GetSysTickCount(&curT);
                TimeoutCmd->timeout = curT + AVDECC_TO_ACMP_DISCONNECT_TX_COMMAND;
            }
        } else {
            // TODO: MUST not be here.
        }
    }

    switch (l->sm.state) {
    case S_ACMP_LSTN_IDLE:
        break;

    case S_ACMP_LSTN_WAITING:
#if 0
        l->sm.rcvdConnectRXCmd     = FALSE;
        l->sm.rcvdDisconnectRXCmd  = FALSE;
        l->sm.rcvdConnectTXResp    = FALSE;
        l->sm.rcvdDisconnectTXResp = FALSE;
        l->sm.rcvdGetRXState       = FALSE;
#endif
        if (doTerminate() == TRUE) {
            l->sm.state = S_ACMP_LSTN_IDLE;
            // TODO: free enqueued inflight command.
        } else {
            if (l->sm.p != NULL) {
                const pkt_acmp_t *acmp;
                const UINT8 *cp = l->sm.p->pkt;

                AmbaMisra_TypeCast(&acmp, &cp);
                if (acmp->acmpdu.listener_entity_id == pAvdeccCfg->entity_descriptor.entity_id) {
                    if (l->sm.rcvdConnectRXCmd == TRUE) {
                        l->sm.rcvdConnectRXCmd = FALSE;
                        l->sm.state            = S_ACMP_LSTN_CONNECT_RX_COMMAND;
                        break;
                    } else if (l->sm.rcvdConnectTXResp == TRUE) {
                        l->sm.rcvdConnectTXResp = FALSE;
                        l->sm.state             = S_ACMP_LSTN_CONNECT_TX_RESPONSE;
                        break;
                    } else if (l->sm.rcvdGetRXState == TRUE) {
                        l->sm.rcvdGetRXState = FALSE;
                        l->sm.state          = S_ACMP_LSTN_GET_STATE;
                        break;
                    } else if (l->sm.rcvdDisconnectRXCmd == TRUE) {
                        l->sm.rcvdDisconnectRXCmd = FALSE;
                        l->sm.state               = S_ACMP_LSTN_DISCONNECT_RX_COMMAND;
                        break;
                    } else if (l->sm.rcvdDisconnectTXResp == TRUE) {
                        l->sm.rcvdDisconnectTXResp = FALSE;
                        l->sm.state                = S_ACMP_LSTN_DISCONNECT_TX_RESPONSE;
                        break;
                    } else {
                        // TODO: state or sleep
                        avdecc_free(l->sm.p);
                        l->sm.p = NULL;
                    }
                } else {
                    // TODO: state or sleep
                    avdecc_free(l->sm.p);
                    l->sm.p = NULL;
                }
            }
        }
        break;

    case S_ACMP_LSTN_CONNECT_TX_TIMEOUT:
        /* Handle timeout before checking state-machine. */
        /* UCT to next */
        l->sm.state = S_ACMP_LSTN_WAITING;
        break;

    case S_ACMP_LSTN_DISCONNECT_TX_TIMEOUT:
        /* Handle timeout before checking state-machine. */
        /* UCT to next */
        l->sm.state = S_ACMP_LSTN_WAITING;
        break;

    case S_ACMP_LSTN_CONNECT_RX_COMMAND:
        /* Send CONNECT_TX_COMMAND to talker */
        /*
         *  if (validListenerUnique(rcvdCmdResp.listener_unique_id)) {
         *      if (listenerIsConnected(rcvdCmdResp)) {
         *          txCommand(ACMP_MSG_CONNECT_TX_COMMAND, rcvdCmdResp, FALSE);
         *      } else {
         *          txResponse(ACMP_MSG_CONNECT_RX_RESPONSE, rcvdCmdResp, LISTENER_EXCLUSIVE);
         *      }
         *  } else {
         *      txResponse(ACMP_MSG_CONNECT_RX_RESPONSE, rcvdCmdResp, LISTENER_UNKNOW_ID);
         *  }
         */
        {
            const pkt_acmp_t *acmp;
            const UINT8 *cp = l->sm.p->pkt;

            AmbaMisra_TypeCast(&acmp, &cp);

            // validListenerUnique(rcvdCmdResp.listener_unique_id)
            if ((acmp->acmpdu.listener_entity_id == pAvdeccCfg->entity_descriptor.entity_id) &&
                (acmp->acmpdu.listener_unique_id == l->uid)) {
                if (l->connected == FALSE) {
                    // txCommand(ACMP_MSG_CONNECT_TX_COMMAND, rcvdCmdResp, FALSE);
                    avdecc_acmp_txPktByClone(l->sm.p,
                                             ACMP_MSG_CONNECT_TX_COMMAND,
                                             0);
                    // Pass pkt to inflight
                    l->sm.ifly_acmp = avdecc_ifly_enQ(l->sm.ifly_acmp,
                                                      ACMP_MSG_CONNECT_TX_COMMAND,
                                                      AVDECC_TO_ACMP_CONNECT_TX_COMMAND,
                                                      l->sm.p,
                                                      acmp->acmpdu.sequence_id);
                    l->sm.p = NULL;
                } else {
                    // txResponse(ACMP_MSG_CONNECT_RX_RESPONSE, rcvdCmdResp, LISTENER_EXCLUSIVE);
                    avdecc_acmp_txPktByClone(l->sm.p,
                                             ACMP_MSG_CONNECT_RX_RESPONSE,
                                             ACMP_SS_LISTENER_EXCLUSIVE);
                    avdecc_free(l->sm.p);
                    l->sm.p = NULL;
                }
            } else {
                // txResponse(ACMP_MSG_CONNECT_RX_RESPONSE, rcvdCmdResp, LISTENER_UNKNOW_ID);
                avdecc_acmp_txPktByClone(l->sm.p,
                                         ACMP_MSG_CONNECT_RX_RESPONSE,
                                         ACMP_SS_LISTENER_UNKNOWN_ID);
                avdecc_free(l->sm.p);
                l->sm.p = NULL;
            }
        }
        /* UCT to next */
        l->sm.state = S_ACMP_LSTN_WAITING;
        break;

    case S_ACMP_LSTN_CONNECT_TX_RESPONSE:
        /*
         *  if (validListenerUnique(rcvdCmdResp.listener_unique_id)) {
         *      if (SUCCESS == rcvdCmdResp.status) {
         *          (response, status) = connectListener(rcvdCmdResp);
         *      } else {
         *          response = rcvdCmdResp;
         *          status   = rcvdCmdResp.status;
         *      }
         *      response.sequence_id = inflight[x].original_sequence_id;
         *      cancelTimeout(rcvdCmdResp);
         *      removeInflight(rcvdCmdResp);
         *      txResponse(ACMP_MSG_CONNECT_RX_RESPONSE, response, status);
         *  }
         */
        {
            const pkt_acmp_t *acmp;
            const UINT8 *cp = l->sm.p->pkt;
            UINT8 status;

            AmbaMisra_TypeCast(&acmp, &cp);

            // validListenerUnique(rcvdCmdResp.listener_unique_id)
            if ((acmp->acmpdu.listener_entity_id == pAvdeccCfg->entity_descriptor.entity_id) &&
                (acmp->acmpdu.listener_unique_id == l->uid)) {
                if (acmp->avtpdu_hdr.status == ACMP_SS_SUCCESS) {
                    if (l->sm.cbf_connect != NULL) {
                        status = l->sm.cbf_connect();
                    } else {
                        // TODO: or LISTENER_MISBEHAVING
                        status = ACMP_SS_SUCCESS;
                    }
                    l->connected = TRUE;
                } else {
                    status = acmp->avtpdu_hdr.status;
                }
                // TODO: check flag for ACMP_FLG_FAST_CONNECT
                avdecc_acmp_txPktByClone(l->sm.p,
                                         ACMP_MSG_CONNECT_RX_RESPONSE,
                                         status);
                l->sm.ifly_acmp = avdecc_ifly_deQ(l->sm.ifly_acmp, 0xFFu,
                                                  acmp->acmpdu.sequence_id);
                // TODO: Store more
                {
                    l->sm.listenerStreamInfo.talker_eid = acmp->acmpdu.talker_entity_id;
                    l->sm.listenerStreamInfo.stream_id = acmp->avtpdu_hdr.stream_id;
                    l->sm.listenerStreamInfo.controller_eid = acmp->acmpdu.controller_entity_id;
                    l->sm.listenerStreamInfo.talker_uid = acmp->acmpdu.talker_unique_id;
                    l->sm.listenerStreamInfo.stream_vlan_id = acmp->acmpdu.stream_vlan_id;
                    l->sm.listenerStreamInfo.flags = acmp->acmpdu.flags;
                    l->sm.listenerStreamInfo.connected = 1;
                    if (AmbaWrap_memcpy(l->sm.listenerStreamInfo.stream_dmac, acmp->acmpdu.stream_dest_mac, 6)!= 0U) { }
                }
            } else {
                // TODO: not mentioned in spec.
            }
            avdecc_free(l->sm.p);
            l->sm.p = NULL;
        }
        /* UCT to next */
        l->sm.state = S_ACMP_LSTN_WAITING;
        break;

    case S_ACMP_LSTN_GET_STATE:
        /*
         *  if (validListenerUnique(rcvdCmdResp.listener_unique_id)) {
         *      (response, error) = getState(rcvdCmdResp);
         *  } else {
         *      response = rcvdCmdResp;
         *      error    = LISTENER_UNKNOW_ID;
         *  }
         *  txResponse(ACMP_MSG_GET_RX_STATE_RESPONSE, response, error);
         */
        {
            const pkt_acmp_t *acmp;
            const UINT8 *cp = l->sm.p->pkt;
            UINT8 status;

            AmbaMisra_TypeCast(&acmp, &cp);

            // validListenerUnique(rcvdCmdResp.listener_unique_id)
            if ((acmp->acmpdu.listener_entity_id == pAvdeccCfg->entity_descriptor.entity_id) &&
                (acmp->acmpdu.listener_unique_id == l->uid)) {
                // TODO: Multiple connection?!
                if (l->connected == TRUE) {
                    // TODO: get state from listenerStreamInfo.
                    status = ACMP_SS_NOT_SUPPORTED;
                } else {
                    // TODO: not mentioned in spec. Or TALKER_MISBEHAVING
                    status = ACMP_SS_NOT_CONNECTED;
                }
            } else {
                status = ACMP_SS_LISTENER_UNKNOWN_ID;
            }
            avdecc_acmp_txPktByClone(l->sm.p,
                                     ACMP_MSG_GET_RX_STATE_RESPONSE, status);
            avdecc_free(l->sm.p);
            l->sm.p = NULL;
        }
        /* UCT to next */
        l->sm.state = S_ACMP_LSTN_WAITING;
        break;

    case S_ACMP_LSTN_DISCONNECT_RX_COMMAND:
        /*
         *  if (validListenerUnique(rcvdCmdResp.listener_unique_id)) {
         *      if (listenerIsConnected(rcvdCmdResp)) {
         *          (response, status) = disconnectListener(rcvdCmdResp);
         *          if (SUCCESS == status) {
         *              txCommand(ACMP_MSG_DISCONNECT_TX_COMMAND, rcvdCmdResp, FALSE);
         *          }
         *          ekse {
         *              txResponse(ACMP_MSG_DISCONNECT_RX_RESPONSE, response, status);
         *          }
         *      } else {
         *          txResponse(ACMP_MSG_DISCONNECT_RX_RESPONSE, rcvdCmdResp, NOT_CONNECTED);
         *      }
         *  } else {
         *      txResponse(ACMP_MSG_DISCONNECT_RX_RESPONSE, rcvdCmdResp, LISTENER_UNKNOW_ID);
         *  }
         */
        {
            const pkt_acmp_t *acmp;
            const UINT8 *cp = l->sm.p->pkt;

            AmbaMisra_TypeCast(&acmp, &cp);

            // validListenerUnique(rcvdCmdResp.listener_unique_id)
            if ((acmp->acmpdu.listener_entity_id == pAvdeccCfg->entity_descriptor.entity_id) &&
                (acmp->acmpdu.listener_unique_id == l->uid)) {
                if (l->connected == TRUE) {
                    // txCommand(ACMP_MSG_DISCONNECT_TX_COMMAND, rcvdCmdResp, FALSE);
                    avdecc_acmp_txPktByClone(l->sm.p,
                                             ACMP_MSG_DISCONNECT_TX_COMMAND,
                                             0);
                    // Pass pkt to inflight
                    l->sm.ifly_acmp = avdecc_ifly_enQ(l->sm.ifly_acmp,
                                                      ACMP_MSG_DISCONNECT_TX_COMMAND,
                                                      AVDECC_TO_ACMP_DISCONNECT_TX_COMMAND,
                                                      l->sm.p,
                                                      acmp->acmpdu.sequence_id);
                    l->sm.p = NULL;
                } else {
                    // txResponse(ACMP_MSG_DISCONNECT_RX_RESPONSE, rcvdCmdResp, NOT_CONNECTED);
                    avdecc_acmp_txPktByClone(l->sm.p,
                                             ACMP_MSG_DISCONNECT_RX_RESPONSE,
                                             ACMP_SS_NOT_CONNECTED);
                    avdecc_free(l->sm.p);
                    l->sm.p = NULL;
                }
            } else {
                // txResponse(ACMP_MSG_DISCONNECT_RX_RESPONSE, rcvdCmdResp, LISTENER_UNKNOW_ID);
                avdecc_acmp_txPktByClone(l->sm.p,
                                         ACMP_MSG_DISCONNECT_RX_RESPONSE,
                                         ACMP_SS_LISTENER_UNKNOWN_ID);
                avdecc_free(l->sm.p);
                l->sm.p = NULL;
            }
        }
        /* UCT to next */
        l->sm.state = S_ACMP_LSTN_WAITING;
        break;

    case S_ACMP_LSTN_DISCONNECT_TX_RESPONSE:
        /*
         *  if (validListenerUnique(rcvdCmdResp.listener_unique_id)) {
         *      response             = rcvdCmdResp;
         *      status               = rcvdCmdResp.status;
         *      response.sequence_id = inflight[x].original_sequence_id;
         *      cancelTimeout(rcvdCmdResp);
         *      removeInflight(rcvdCmdResp);
         *      txResponse(ACMP_MSG_DISCONNECT_RX_RESPONSE, response, status);
         *  }
         */

        {
            const pkt_acmp_t *acmp;
            const UINT8 *cp = l->sm.p->pkt;
            UINT8 status;

            AmbaMisra_TypeCast(&acmp, &cp);

            // validListenerUnique(rcvdCmdResp.listener_unique_id)
            if ((acmp->acmpdu.listener_entity_id == pAvdeccCfg->entity_descriptor.entity_id) &&
                (acmp->acmpdu.listener_unique_id == l->uid)) {
                status = acmp->avtpdu_hdr.status;
                if (acmp->avtpdu_hdr.status == ACMP_SS_SUCCESS) {
                    if (l->sm.cbf_disconnect != NULL) {
                        // TODO:
                        status = l->sm.cbf_disconnect();
                    } else {
                        // TODO: or LISTENER_MISBEHAVING
                        status = ACMP_SS_SUCCESS;
                    }
                    l->connected = FALSE;
                } else {
                    status = acmp->avtpdu_hdr.status;
                }
                // TODO: check flag for ACMP_FLG_FAST_DISCONNECT
                avdecc_acmp_txPktByClone(l->sm.p,
                                         ACMP_MSG_DISCONNECT_RX_RESPONSE,
                                         status);
                l->sm.ifly_acmp = avdecc_ifly_deQ(l->sm.ifly_acmp, 0xFFu,
                                                  acmp->acmpdu.sequence_id);
                // TODO: Store more
                {
                    l->sm.listenerStreamInfo.talker_eid = 0;
                    l->sm.listenerStreamInfo.stream_id = 0;
//                    l->sm.listenerStreamInfo.controller_eid = acmp->acmpdu.controller_entity_id;
                    l->sm.listenerStreamInfo.talker_uid = 0;
//                    l->sm.listenerStreamInfo.stream_vlan_id = acmp->acmpdu.stream_vlan_id;
                    l->sm.listenerStreamInfo.flags = 0;
                    l->sm.listenerStreamInfo.connected = 0;
                    if (AmbaWrap_memset(l->sm.listenerStreamInfo.stream_dmac, 0, 6)!= 0U) { }
                }
            }
            avdecc_free(l->sm.p);
            l->sm.p = NULL;
        }
        /* UCT to next */
        l->sm.state = S_ACMP_LSTN_WAITING;
        break;

    default:
        /* TODO */
        break;
    }
}



#define S_ACMP_TALK_IDLE           (0U)
#define S_ACMP_TALK_WAITING        (1U)
#define S_ACMP_TALK_CONNECT        (2U)
#define S_ACMP_TALK_DISCONNECT     (3U)
#define S_ACMP_TALK_GET_STATE      (4U)
#define S_ACMP_TALK_GET_CONNECTION (5U)



void avdecc_sm_acmp_talker(avdecc_talker_t *t)
{
    switch (t->sm.state) {
    case S_ACMP_TALK_IDLE:
        break;

    case S_ACMP_TALK_WAITING:
#if 0
        t->sm.rcvdConnectTX       = FALSE;
        t->sm.rcvdDisconnectTX    = FALSE;
        t->sm.rcvdGetTXState      = FALSE;
        t->sm.rcvdGetTXConnection = FALSE;
#endif
        if (doTerminate() == TRUE) {
            t->sm.state = S_ACMP_TALK_IDLE;
            break;
        } else if (t->sm.p != NULL) {
            const pkt_acmp_t *acmp;
            const UINT8 *cp = t->sm.p->pkt;

            AmbaMisra_TypeCast(&acmp, &cp);
            if (acmp->acmpdu.talker_entity_id == pAvdeccCfg->entity_descriptor.entity_id) {
                if (t->sm.rcvdConnectTX == TRUE) {
                    t->sm.rcvdConnectTX = FALSE;
                    t->sm.state         = S_ACMP_TALK_CONNECT;
                    break;
                } else if (t->sm.rcvdDisconnectTX == TRUE) {
                    t->sm.rcvdDisconnectTX = FALSE;
                    t->sm.state            = S_ACMP_TALK_DISCONNECT;
                    break;
                } else if (t->sm.rcvdGetTXState == TRUE) {
                    t->sm.rcvdGetTXState = FALSE;
                    t->sm.state          = S_ACMP_TALK_GET_STATE;
                    break;
                } else if (t->sm.rcvdGetTXConnection == TRUE) {
                    t->sm.rcvdGetTXConnection = FALSE;
                    t->sm.state               = S_ACMP_TALK_GET_CONNECTION;
                    break;
                } else {
                    // TODO: state or sleep
                    avdecc_free(t->sm.p);
                    t->sm.p = NULL;
                }
            } else {
                // TODO: state or sleep
                avdecc_free(t->sm.p);
                t->sm.p = NULL;
            }
        } else {
            /* TODO */
            ;
        }
        /* UCT to next */
        t->sm.state = S_ACMP_TALK_WAITING;
        break;

    case S_ACMP_TALK_CONNECT:
        /*
         *  if (validTalkerUnique(rcvdCmdResp.talker_entity_id)) {
         *      (response, error) = connectTalker(rcvdCmdResp);
         *  } else {
         *      response = rcvdCmdResp;
         *      error    = TALKER_UNKNOWN_ID;
         *  }
         *  txResponse(ACMP_MSG_CONNECT_TX_RESPONSE, response, error);
         */
        {
            const pkt_acmp_t *acmp;
            const UINT8 *cp = t->sm.p->pkt;
            UINT8 status;

            AmbaMisra_TypeCast(&acmp, &cp);

            // validListenerUnique(rcvdCmdResp.listener_unique_id)
            if ((acmp->acmpdu.talker_entity_id == pAvdeccCfg->entity_descriptor.entity_id) &&
                (acmp->acmpdu.talker_unique_id == t->uid)) {
                // TODO: check flag for ACMP_FLG_FAST_CONNECT
                // TODO: Multiple connection?!
                if (t->connected == FALSE) {
                    if (t->sm.cbf_connect != NULL) {
                        // TODO: Get stream_dest_mac
                        {
                            UINT8 i, j;

                            j = 0;
                            for (i = 0; i < 6u; i++) {
                                j |= acmp->acmpdu.stream_dest_mac[i];
                            }
                            if (j == 0u) {
                                void *p;

                                cp = acmp->acmpdu.stream_dest_mac;
                                AmbaMisra_TypeCast(&p, &cp);
                                if (AmbaWrap_memcpy(p, AVTP_DST, 6u)!= 0U) { }
                            }
                        }
                        // TODO: Set acmp->acmpdu.connection_count += 1
                        // TODO: Set acmp->acmpdu.stream_vlan_id
                        status = t->sm.cbf_connect();
                    } else {
                        // TODO: or TALKER_MISBEHAVING
                        status = ACMP_SS_SUCCESS;
                    }
                    // TODO: if status != OK
                    t->connected = TRUE;
                } else {
                    status = ACMP_SS_TALKER_EXCLUSIVE;
                }
            } else {
                status = ACMP_SS_TALKER_UNKNOWN_ID;
            }
            avdecc_acmp_txPktByClone(t->sm.p,
                                     ACMP_MSG_CONNECT_TX_RESPONSE, status);
            // TODO: Store more, Multiple Connection
            {
                t->sm.talkerStreamInfo.stream_id = acmp->avtpdu_hdr.stream_id;
                (void)AmbaWrap_memcpy(t->sm.talkerStreamInfo.stream_dmac,
                                      acmp->acmpdu.stream_dest_mac, 6);
                t->sm.talkerStreamInfo.connection_count = 1;
                t->sm.talkerStreamInfo.stream_vlan_id = acmp->acmpdu.stream_vlan_id;
                t->sm.talkerStreamInfo.lp[0].listener_entity_id = acmp->acmpdu.listener_entity_id;
                t->sm.talkerStreamInfo.lp[0].listener_unique_id = acmp->acmpdu.listener_unique_id;
            }

            avdecc_free(t->sm.p);
            t->sm.p = NULL;
        }
        /* UCT to next */
        t->sm.state = S_ACMP_TALK_WAITING;
        break;

    case S_ACMP_TALK_DISCONNECT:
        /*
         *  if (validTalkerUnique(rcvdCmdResp.talker_entity_id)) {
         *      (response, error) = disconnectTalker(rcvdCmdResp);
         *  } else {
         *      response = rcvdCmdResp;
         *      error    = TALKER_UNKNOWN_ID;
         *  }
         *  txResponse(ACMP_MSG_DISCONNECT_TX_RESPONSE, response, error);
         */
        {
            const pkt_acmp_t *acmp;
            const UINT8 *cp = t->sm.p->pkt;
            UINT8 status;

            AmbaMisra_TypeCast(&acmp, &cp);

            // validListenerUnique(rcvdCmdResp.listener_unique_id)
            if ((acmp->acmpdu.talker_entity_id == pAvdeccCfg->entity_descriptor.entity_id) &&
                (acmp->acmpdu.talker_unique_id == t->uid)) {
                // TODO: check flag for ACMP_FLG_FAST_CONNECT
                // TODO: Multiple connection?!
                if (t->connected == TRUE) {
                    if (t->sm.cbf_disconnect != NULL) {
                        // TODO: Get stream_dest_mac
                        // TODO: Set acmp->acmpdu.connection_count += 1
                        // TODO: Set acmp->acmpdu.stream_vlan_id
                        status = t->sm.cbf_disconnect();
                    } else {
                        // TODO: or TALKER_MISBEHAVING
                        status = ACMP_SS_SUCCESS;
                    }
                    t->connected = FALSE;
                } else {
                    // TODO: not mentioned in spec. Or TALKER_MISBEHAVING
                    status = ACMP_SS_NOT_CONNECTED;
                }
            } else {
                status = ACMP_SS_TALKER_UNKNOWN_ID;
            }
            avdecc_acmp_txPktByClone(t->sm.p,
                                     ACMP_MSG_DISCONNECT_TX_RESPONSE, status);
            // TODO: Store more, Multiple Connection
            {
                t->sm.talkerStreamInfo.stream_id = 0;
                if (AmbaWrap_memset(t->sm.talkerStreamInfo.stream_dmac, 0, 6)!= 0U) { }
                t->sm.talkerStreamInfo.connection_count = 0;
//                t->sm.talkerStreamInfo.stream_vlan_id = acmp->acmpdu.stream_vlan_id;
                t->sm.talkerStreamInfo.lp[0].listener_entity_id = 0;
                t->sm.talkerStreamInfo.lp[0].listener_unique_id = 0;
            }

            avdecc_free(t->sm.p);
            t->sm.p = NULL;
        }
        /* UCT to next */
        t->sm.state = S_ACMP_TALK_WAITING;
        break;

    case S_ACMP_TALK_GET_STATE:
        /*
         *  if (validTalkerUnique(rcvdCmdResp.talker_entity_id)) {
         *      (response, error) = getState(rcvdCmdResp);
         *  } else {
         *      response = rcvdCmdResp;
         *      error    = TALKER_UNKNOWN_ID;
         *  }
         *  txResponse(ACMP_MSG_GET_TX_STATE_RESPONSE, response, error);
         */
        {
            const pkt_acmp_t *acmp;
            const UINT8 *cp = t->sm.p->pkt;
            UINT8 status;

            AmbaMisra_TypeCast(&acmp, &cp);

            // validListenerUnique(rcvdCmdResp.listener_unique_id)
            if ((acmp->acmpdu.talker_entity_id == pAvdeccCfg->entity_descriptor.entity_id) &&
                (acmp->acmpdu.talker_unique_id == t->uid)) {
                // TODO: Multiple connection?!
                if (t->connected == TRUE) {
                    // TODO: get state from talkerStreamInfo.
                    status = ACMP_SS_STATE_UNAVAILABLE;
                } else {
                    // TODO: not mentioned in spec. Or TALKER_MISBEHAVING
                    status = ACMP_SS_NOT_CONNECTED;
                }
            } else {
                status = ACMP_SS_TALKER_UNKNOWN_ID;
            }
            avdecc_acmp_txPktByClone(t->sm.p,
                                     ACMP_MSG_GET_TX_STATE_RESPONSE, status);

            avdecc_free(t->sm.p);
            t->sm.p = NULL;
        }
        /* UCT to next */
        t->sm.state = S_ACMP_TALK_WAITING;
        break;

    case S_ACMP_TALK_GET_CONNECTION:
        /*
         *  if (validTalkerUnique(rcvdCmdResp.talker_entity_id)) {
         *      (response, error) = getConnection(rcvdCmdResp);
         *  } else {
         *      response = rcvdCmdResp;
         *      error    = TALKER_UNKNOWN_ID;
         *  }
         *  txResponse(ACMP_MSG_GET_TX_CONNECTION_RESPONSE, response, error);
         */
        {
            const pkt_acmp_t *acmp;
            const UINT8 *cp = t->sm.p->pkt;
            UINT8 status;

            AmbaMisra_TypeCast(&acmp, &cp);

            // validListenerUnique(rcvdCmdResp.listener_unique_id)
            if ((acmp->acmpdu.talker_entity_id == pAvdeccCfg->entity_descriptor.entity_id) &&
                (acmp->acmpdu.talker_unique_id == t->uid)) {
                // TODO: Multiple connection?!
                if (t->connected == TRUE) {
                    // TODO: get state from talkerStreamInfo.
                    status = ACMP_SS_NOT_SUPPORTED;
                } else {
                    // TODO: not mentioned in spec. Or TALKER_MISBEHAVING
                    status = ACMP_SS_NOT_CONNECTED;
                }
            } else {
                status = ACMP_SS_TALKER_UNKNOWN_ID;
            }
            avdecc_acmp_txPktByClone(t->sm.p,
                                     ACMP_MSG_GET_TX_CONNECTION_RESPONSE,
                                     status);

            avdecc_free(t->sm.p);
            t->sm.p = NULL;
        }
        /* UCT to next */
        t->sm.state = S_ACMP_TALK_WAITING;
        break;

    default:
        /* TODO */
        break;
    }
}


void avdecc_acmp_init(void)
{
static AMBA_KAL_TASK_t tsk_acmp_controller __attribute__((section(".bss.noinit")));
static UINT8 stk_acmp_controller[STK_SIZE_ACMP_CONTROLLER] __attribute__((section(".bss.noinit")));
    static char name_acmp_ctrl[] = "acmp controller";
    SM_Acmp_Controller.state = S_ACMP_CTRL_IDLE;

    {
        UINT32 err;

        if (AmbaWrap_memset(&tsk_acmp_controller, 0, sizeof(tsk_acmp_controller))!= 0U) { }
        err = AmbaKAL_TaskCreate(&tsk_acmp_controller, name_acmp_ctrl, PRIORITY_ACMP_CONTROLLER,
                                 avdecc_sm_acmp_controller, NULL,
                                 stk_acmp_controller, STK_SIZE_ACMP_CONTROLLER,
                                 AMBA_KAL_DONT_START);
        if (err != OK) {
            AmbaPrint_PrintUInt5("AmbaKAL_TaskCreate: 0x%X", err, 0u, 0u, 0u, 0u);
        } else {
            (void)AmbaKAL_TaskSetSmpAffinity(&tsk_acmp_controller, ACDECC_CORE);
//            (void)AmbaKAL_TaskResume(&tsk_acmp_controller);
        }
    }
}

#define ACMP_DBG (0)

void avdecc_ntoh_acmp(const void *frame)
{
    pkt_acmp_t *p;

    AmbaMisra_TypeCast(&p, &frame);
    {
        const void *vptr = &(p->avtpdu_hdr);
        UINT32 *ptr;
        AmbaMisra_TypeCast(&ptr, &vptr);

        *ptr = Amba_ntohl(*ptr);
    }

#if ACMP_DBG
    AmbaPrint("control_data_len: %d", p->avtpdu_hdr.control_data_len);
    AmbaPrint("status: %d", p->avtpdu_hdr.status);
    AmbaPrint("message_type: %d", p->avtpdu_hdr.message_type);
    AmbaPrint("version: %d", p->avtpdu_hdr.version);
    AmbaPrint("sv: %d", p->avtpdu_hdr.sv);
    AmbaPrint("subtype: %d", p->avtpdu_hdr.subtype);
#endif

    p->avtpdu_hdr.stream_id = Amba_ntohll(p->avtpdu_hdr.stream_id);
#if ACMP_DBG
    AmbaPrint("stream_id: %llX", p->avtpdu_hdr.stream_id);
#endif

    /* AVDECC Connection Management Protocol Data Unit (ACMPDU) Format */
    p->acmpdu.controller_entity_id = Amba_ntohll(p->acmpdu.controller_entity_id);
#if ACMP_DBG
    AmbaPrint("controller_entity_id: %llX", p->acmpdu.controller_entity_id);
#endif
    p->acmpdu.talker_entity_id = Amba_ntohll(p->acmpdu.talker_entity_id);
#if ACMP_DBG
    AmbaPrint("talker_entity_id: %llX", p->acmpdu.talker_entity_id);
#endif
    p->acmpdu.listener_entity_id = Amba_ntohll(p->acmpdu.listener_entity_id);
#if ACMP_DBG
    AmbaPrint("listener_entity_id: %llX", p->acmpdu.listener_entity_id);
#endif

    p->acmpdu.listener_unique_id = Amba_ntohs(p->acmpdu.listener_unique_id);
#if ACMP_DBG
    AmbaPrint("listener_unique_id: %X", p->acmpdu.listener_unique_id);
#endif
    p->acmpdu.talker_unique_id = Amba_ntohs(p->acmpdu.talker_unique_id);
#if ACMP_DBG
    AmbaPrint("talker_unique_id: %X", p->acmpdu.talker_unique_id);
#endif

    p->acmpdu.connection_count = Amba_ntohs(p->acmpdu.connection_count);
#if ACMP_DBG
    AmbaPrint("connection_count: %X", p->acmpdu.connection_count);
    AmbaPrint("stream_dest_mac[0:5]: %02X:%02X:%02X:%02X:%02X:%02X",
              p->acmpdu.stream_dest_mac[0],
              p->acmpdu.stream_dest_mac[1],
              p->acmpdu.stream_dest_mac[2],
              p->acmpdu.stream_dest_mac[3],
              p->acmpdu.stream_dest_mac[4],
              p->acmpdu.stream_dest_mac[5]);
#endif

    p->acmpdu.flags = Amba_ntohs(p->acmpdu.flags);
#if ACMP_DBG
    AmbaPrint("flags: %04X", p->acmpdu.flags);
#endif
    p->acmpdu.sequence_id = Amba_ntohs(p->acmpdu.sequence_id);
#if ACMP_DBG
    AmbaPrint("sequence_id: %04X", p->acmpdu.sequence_id);
#endif
    p->acmpdu.stream_vlan_id = Amba_ntohs(p->acmpdu.stream_vlan_id);
#if ACMP_DBG
    AmbaPrint("stream_vlan_id: %04X", p->acmpdu.stream_vlan_id);
#endif
}

void avdecc_process_acmp(const void *packet)
{
    const avdecc_pkt_t *pkt;
    const UINT8 *cp;
    const pkt_acmp_t *p;
    const avdecc_talker_t *t = pAvdeccCfg->talker;
    const avdecc_listener_t *l = pAvdeccCfg->listener;

    AmbaMisra_TypeCast(&pkt, &packet);
    cp = pkt->pkt;
    AmbaMisra_TypeCast(&p, &cp);
    /* Table 8.1 message_type field */
    switch (p->avtpdu_hdr.message_type) {
        /*
         * To Listener
         */
    case ACMP_MSG_CONNECT_TX_RESPONSE:
        if (l != NULL) {
            // TODO: Check stream_id
            (void)avdecc_clone_msg(&(l->msgQ), pkt,
                    AVTPDU_SUBTYPE_ACMP,
                    ACMP_MSG_CONNECT_TX_RESPONSE);
        }
        break;

    case ACMP_MSG_DISCONNECT_TX_RESPONSE:
        if (l != NULL) {
            // TODO: Check stream_id
            (void)avdecc_clone_msg(&(l->msgQ), pkt,
                    AVTPDU_SUBTYPE_ACMP,
                    ACMP_MSG_DISCONNECT_TX_RESPONSE);
        }
        break;

    case ACMP_MSG_CONNECT_RX_COMMAND:
        if (l != NULL) {
            // TODO: Check stream_id
            (void)avdecc_clone_msg(&(l->msgQ), pkt,
                    AVTPDU_SUBTYPE_ACMP,
                    ACMP_MSG_CONNECT_RX_COMMAND);
        }
        break;

    case ACMP_MSG_DISCONNECT_RX_COMMAND:
        if (l != NULL) {
            // TODO: Check stream_id
            (void)avdecc_clone_msg(&(l->msgQ), pkt,
                    AVTPDU_SUBTYPE_ACMP,
                    ACMP_MSG_DISCONNECT_RX_COMMAND);
        }
        break;

    case ACMP_MSG_GET_RX_STATE_COMMAND:
        if (l != NULL) {
            // TODO: Check stream_id
            (void)avdecc_clone_msg(&(l->msgQ), pkt,
                    AVTPDU_SUBTYPE_ACMP,
                    ACMP_MSG_GET_RX_STATE_COMMAND);
        }
        break;

        /*
         * To Talker
         */
    case ACMP_MSG_CONNECT_TX_COMMAND:
        if (t != NULL) {
            // TODO: Check stream_id
            (void)avdecc_clone_msg(&(t->msgQ), pkt,
                    AVTPDU_SUBTYPE_ACMP,
                    ACMP_MSG_CONNECT_TX_COMMAND);
        }
        break;

    case ACMP_MSG_DISCONNECT_TX_COMMAND:
        if (t != NULL) {
            // TODO: Check stream_id
            (void)avdecc_clone_msg(&(t->msgQ), pkt,
                    AVTPDU_SUBTYPE_ACMP,
                    ACMP_MSG_DISCONNECT_TX_COMMAND);
        }
        break;

    case ACMP_MSG_GET_TX_STATE_COMMAND:
        if (t != NULL) {
            // TODO: Check stream_id
            (void)avdecc_clone_msg(&(t->msgQ), pkt,
                    AVTPDU_SUBTYPE_ACMP,
                    ACMP_MSG_GET_TX_STATE_COMMAND);
        }
        break;

    case ACMP_MSG_GET_TX_CONNECTION_COMMAND:
        if (t != NULL) {
            // TODO: Check stream_id
            (void)avdecc_clone_msg(&(t->msgQ), pkt,
                    AVTPDU_SUBTYPE_ACMP,
                    ACMP_MSG_GET_TX_CONNECTION_COMMAND);
        }
        break;

    case ACMP_MSG_GET_TX_STATE_RESPONSE:
    case ACMP_MSG_CONNECT_RX_RESPONSE:
    case ACMP_MSG_DISCONNECT_RX_RESPONSE:
    case ACMP_MSG_GET_RX_STATE_RESPONSE:
    case ACMP_MSG_GET_TX_CONNECTION_RESPONSE:
//        SM_Acmp_Controller.p            = p;
        SM_Acmp_Controller.rcvdResponse = TRUE;
        for (;;) {
            (void)AmbaKAL_TaskSleep(10);
            if ((SM_Acmp_Controller.state == S_ACMP_CTRL_WAITING) ||
                (SM_Acmp_Controller.state == S_ACMP_CTRL_IDLE)) {
                break;
            }
        }
        break;

    default:
        /* TODO */
        break;
    }
}

