/**
 *  @file listener.c
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
 *  @details Listener entity.
 *
 */


#include <avdecc.h>
#include <avdecc_private.h>


// TODO start --------------------------
#if 0
static UINT64 my_id = 0x1122334455667788U;
static UINT8 doTerminate(void)
{
    return FALSE;
}
#endif

#define TST_LPBK (0)
#if TST_LPBK
static UINT64 talker_id = 0x0011224000334455u;
#else
static UINT64 talker_id = 0;
#endif

static avdecc_listener_t myListener;

void listener_set_talkerEID(UINT64 eid);
/** Set EID of talker to monitor for Fast-Connect */
void listener_set_talkerEID(UINT64 eid)
{
    talker_id = eid;
}
// TODO end ----------------------------
void listener_cbf_adp_available(UINT8 idx);
void listener_cbf_adp_available(UINT8 idx)
{
    if (pAvdeccCfg->EventCb != NULL) {
        AVDECC_EVENT_INFO_s EventInfo;
        EventInfo.Event = AVDECC_EVENT_AVAILABLE;
        EventInfo.entity_id = pAvdeccCfg->rxEInfo[idx].adpdu.avtpdu_hdr.entity_id;
        pAvdeccCfg->EventCb(EventInfo);
    }
    /* Check entity_id for fast connect */
    if (pAvdeccCfg->rxEInfo[idx].adpdu.avtpdu_hdr.entity_id == talker_id) {
        if (myListener.sm.listenerStreamInfo.connected == 0U) {
            // Send CONNECT_TX_COMMAND
            avdecc_send_acmp_fastConnCmd(idx);
        }
    }
}

/**
 *  Example: call-back-function when get CONNECT_TX_RESPONSE
 *  @return
 *      cf. Table 8.2 of IEEE 1722.1 - 2013
 */
static UINT8 listener_cbf_connect(void)
{
    AmbaPrint_PrintStr5("Start to get streaming.....", NULL, NULL, NULL, NULL, NULL);

    return ACMP_SS_SUCCESS;
}

/**
 *  Example: call-back-function when get DISCONNECT_TX_RESPONSE
 */
static UINT8 listener_cbf_disconnect(void)
{
    AmbaPrint_PrintStr5("Stop geting streaming.....", NULL, NULL, NULL, NULL, NULL);

    return ACMP_SS_SUCCESS;
}

//#define S_ACMP_LSTN_IDLE                    (0U)
//#define S_ACMP_LSTN_WAITING                 (1U)
//#define S_ACMP_LSTN_CONNECT_TX_TIMEOUT      (2U)
//#define S_ACMP_LSTN_DISCONNECT_TX_TIMEOUT   (3U)
//#define S_ACMP_LSTN_CONNECT_RX_COMMAND      (4U)
//#define S_ACMP_LSTN_CONNECT_TX_RESPONSE     (5U)
//#define S_ACMP_LSTN_GET_STATE               (6U)
//#define S_ACMP_LSTN_DISCONNECT_RX_COMMAND   (7U)
//#define S_ACMP_LSTN_DISCONNECT_TX_RESPONSE  (8U)

#define PRIORITY_LISTENER    (200)
#define STK_SIZE_LISTENER    (0x2000)

static void *ListenerTsk(void *v)
{
    avdecc_pkt_t *pkt;
    void *pkt_ptr = NULL;
    UINT32 loop = 1U;

    (void)v;

    while (loop == 1U) {
        UINT32 err;

        (void)pkt_ptr;
        err = AmbaKAL_MsgQueueReceive(&myListener.msgQ, &pkt_ptr, 10);
        if ((err == 0u) && (pkt_ptr != NULL)) {
            AmbaMisra_TypeCast(&pkt, &pkt_ptr);

            switch (pkt->stype) {
            case AVTPDU_SUBTYPE_ACMP:
                switch (pkt->mtype) {
                case ACMP_MSG_CONNECT_TX_RESPONSE:
                    myListener.sm.rcvdConnectTXResp = TRUE;
                    myListener.sm.p                 = pkt;
                    avdecc_sm_acmp_listener(&myListener);
                    break;

                case ACMP_MSG_DISCONNECT_TX_RESPONSE:
                    myListener.sm.rcvdDisconnectTXResp = TRUE;
                    myListener.sm.p                    = pkt;
                    avdecc_sm_acmp_listener(&myListener);
                    break;

                case ACMP_MSG_CONNECT_RX_COMMAND:
                    myListener.sm.rcvdConnectRXCmd = TRUE;
                    myListener.sm.p                = pkt;
                    avdecc_sm_acmp_listener(&myListener);
                    break;

                case ACMP_MSG_DISCONNECT_RX_COMMAND:
                    myListener.sm.rcvdDisconnectRXCmd = TRUE;
                    myListener.sm.p                   = pkt;
                    avdecc_sm_acmp_listener(&myListener);
                    break;

                case ACMP_MSG_GET_RX_STATE_COMMAND:
                    myListener.sm.rcvdGetRXState = TRUE;
                    myListener.sm.p              = pkt;
                    avdecc_sm_acmp_listener(&myListener);
                    break;

                default:
                    /* TODO */
                    avdecc_free(pkt);
                    break;
                }
                break;

            case AVTPDU_SUBTYPE_ADP:
                /* TODO */
                avdecc_free(pkt);
                break;

            case AVTPDU_SUBTYPE_AECP:
                /* TODO */
                avdecc_free(pkt);
                break;

            default:
                /* TODO */
                avdecc_free(pkt);
                break;
            }
        }

        avdecc_sm_acmp_listener(&myListener);
    }
    return NULL;
}

UINT32 listener_init(void)
{
static UINT8 stk_listener[STK_SIZE_LISTENER] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
static AMBA_KAL_TASK_t tsk_listener __attribute__((section(".bss.noinit")));
#define LISTENER_MSGQ_SIZE (8)
static UINT8 listener_msgQ_pool[LISTENER_MSGQ_SIZE] __attribute__((section(".bss.noinit")));

    UINT32 err;
    static char name_task[] = "Listener Task";
    static char name_msgQ[] = "Listener msgQ";

    if (AmbaWrap_memset(&myListener, 0, sizeof(myListener))!= 0U) { }

    myListener.uid = avdecc_GetUniqueId(AVDECC_LISTENER, 0);

    myListener.role = AVDECC_LISTENER;
    (void)AmbaKAL_MsgQueueCreate(&myListener.msgQ,
                                 name_msgQ,
                                 sizeof(void *),
                                 listener_msgQ_pool,
                                 LISTENER_MSGQ_SIZE);

    myListener.cbf_adp_available = listener_cbf_adp_available;

    myListener.sm.state = 1;

    myListener.sm.cbf_connect    = listener_cbf_connect;
    myListener.sm.cbf_disconnect = listener_cbf_disconnect;

    avdecc_entity_reg(&myListener, AVDECC_LISTENER, 1,
            (UINT16)(LISTENER_CAP_IMP | LISTENER_CAP_AUDIO | LISTENER_CAP_VIDEO));

    if (AmbaWrap_memset(&tsk_listener, 0, sizeof(tsk_listener))!= 0U) { }
    err = AmbaKAL_TaskCreate(&tsk_listener, name_task, PRIORITY_LISTENER,
                             ListenerTsk, NULL,
                             stk_listener, STK_SIZE_LISTENER,
                             AMBA_KAL_DONT_START);
    if (err != OK) {
        AmbaPrint_PrintUInt5("AmbaKAL_TaskCreate: 0x%X", err, 0u, 0u, 0u, 0u);
    } else {
        (void)AmbaKAL_TaskSetSmpAffinity(&tsk_listener, ACDECC_CORE);
        (void)AmbaKAL_TaskResume(&tsk_listener);
    }

    return 0;
}

