/**
 *  @file talker.c
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
 *  @details Talker entity.
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
// TODO end ----------------------------



static void talker_cbf_adp_available(UINT8 idx)
{
    (void)idx;
}

/**
 *  Example: call-back-function when get CONNECT_TX_COMMAND
 *  cf. 8.2.2.6.2.2 connectTalker(command)
 *
 *  @return
 *      cf. Table 8.2 of IEEE 1722.1 - 2013
 */
static UINT8 talker_cbf_connect(void)
{
    AmbaPrint_PrintStr5("Start streaming.....", NULL, NULL, NULL, NULL, NULL);

    return ACMP_SS_SUCCESS;
}

/**
 *  Example: call-back-function when get DISCONNECT_TX_COMMAND
 */
static UINT8 talker_cbf_disconnect(void)
{
    AmbaPrint_PrintStr5("Stop streaming.....", NULL, NULL, NULL, NULL, NULL);

    return ACMP_SS_SUCCESS;
}


static avdecc_talker_t myTalker;
//#define S_ACMP_TALK_IDLE           (0U)
//#define S_ACMP_TALK_WAITING        (1U)
//#define S_ACMP_TALK_CONNECT        (2U)
//#define S_ACMP_TALK_DISCONNECT     (3U)
//#define S_ACMP_TALK_GET_STATE      (4U)
//#define S_ACMP_TALK_GET_CONNECTION (5U)


#define PRIORITY_TALKER            (200U)
#define STK_SIZE_TALKER            (0x2000)

static void *TalkerTsk(void *v)
{
    avdecc_pkt_t *pkt;
    void *pkt_ptr = NULL;
    UINT32 loop = 1U;

    (void)v;

    while (loop == 1U) {
        UINT32 err;

        (void)pkt_ptr;
        err = AmbaKAL_MsgQueueReceive(&myTalker.msgQ, &pkt_ptr, 10);
        if ((err == 0u) && (pkt_ptr != NULL)) {
            AmbaMisra_TypeCast(&pkt, &pkt_ptr);

            switch (pkt->stype) {
            case AVTPDU_SUBTYPE_ACMP:
                switch (pkt->mtype) {
                case ACMP_MSG_CONNECT_TX_COMMAND:
                    myTalker.sm.rcvdConnectTX = TRUE;
                    myTalker.sm.p             = pkt;
                    avdecc_sm_acmp_talker(&myTalker);
                    break;

                case ACMP_MSG_DISCONNECT_TX_COMMAND:
                    myTalker.sm.rcvdDisconnectTX = TRUE;
                    myTalker.sm.p                = pkt;
                    avdecc_sm_acmp_talker(&myTalker);
                    break;

                case ACMP_MSG_GET_TX_STATE_COMMAND:
                    myTalker.sm.rcvdGetTXState = TRUE;
                    myTalker.sm.p              = pkt;
                    avdecc_sm_acmp_talker(&myTalker);
                    break;

                case ACMP_MSG_GET_TX_CONNECTION_COMMAND:
                    myTalker.sm.rcvdGetTXConnection = TRUE;
                    myTalker.sm.p                   = pkt;
                    avdecc_sm_acmp_talker(&myTalker);
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

        avdecc_sm_acmp_talker(&myTalker);
    }

    return NULL;
}


UINT32 talker_init(void)
{
static UINT8 stk_talker[STK_SIZE_TALKER] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
static AMBA_KAL_TASK_t tsk_talker __attribute__((section(".bss.noinit")));
#define TALKER_MSGQ_SIZE (8)
static UINT8 talker_msgQ_pool[TALKER_MSGQ_SIZE] __attribute__((section(".bss.noinit")));

    UINT32 err;
    static char name_task[] = "Talker task";
    static char name_msgQ[] = "Talker msgQ";

    if (AmbaWrap_memset(&myTalker, 0, sizeof(myTalker))!= 0U) { }

    myTalker.uid = avdecc_GetUniqueId(AVDECC_TALKER, 0);

    myTalker.role = AVDECC_TALKER;
    (void)AmbaKAL_MsgQueueCreate(&myTalker.msgQ,
                                 name_msgQ,
                                 sizeof(void *),
                                 talker_msgQ_pool,
                                 TALKER_MSGQ_SIZE);

    myTalker.cbf_adp_available = talker_cbf_adp_available;

    myTalker.sm.state = 1;

    myTalker.sm.cbf_connect    = talker_cbf_connect;
    myTalker.sm.cbf_disconnect = talker_cbf_disconnect;

    avdecc_entity_reg(&myTalker, AVDECC_TALKER, 1,
            (UINT16)(TALKER_CAP_IMP | TALKER_CAP_AUDIO | TALKER_CAP_VIDEO));

    if (AmbaWrap_memset(&tsk_talker, 0, sizeof(tsk_talker))!= 0U) { }
    err = AmbaKAL_TaskCreate(&tsk_talker, name_task, PRIORITY_TALKER,
                             TalkerTsk, NULL,
                             stk_talker, STK_SIZE_TALKER,
                             AMBA_KAL_DONT_START);
    if (err != OK) {
        AmbaPrint_PrintUInt5("AmbaKAL_TaskCreate: 0x%X", err, 0u, 0u, 0u, 0u);
    } else {
        (void)AmbaKAL_TaskSetSmpAffinity(&tsk_talker, ACDECC_CORE);
        (void)AmbaKAL_TaskResume(&tsk_talker);
    }

    return 0;
}

