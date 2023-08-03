/**
 *  @file aecp.c
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
 *  @details AVDECC Enumeration and Control Protocol
 *
 */

#include <avdecc.h>
#include <avdecc_private.h>



// TBD ---- start
static UINT64 myEntityID __attribute__((section(".bss.noinit")));
static UINT8 doTerminate(void)
{
    return avdecc_doTerminate();
}
// TBD ---- end



/*
 * pp.165 of IEEE 1722.1 - 2013
 *
 * An AVDECC Talker or Listener shall implement and respond to the
 * ACQUIRE_ENTITY, LOCK_ENTITY, and ENTITY_AVAILABLE commands.
 * All other commands are optional for an AVDECC Talker or Listener.
 *
 * An AVDECC Controller shall implement and respond to
 * the CONTROLLER_AVAILABLE command.
 * All other commands are optional for an AVDECC Controller.
 */
/*
   #define AECP_CMD_ACQUIRE_ENTITY                          (0x0000)
   #define AECP_CMD_LOCK_ENTITY                             (0x0001)
   #define AECP_CMD_ENTITY_AVAILABLE                        (0x0002)
   #define AECP_CMD_CONTROLLER_AVAILABLE                    (0x0003)
 */

UINT32 avdecc_aecp_txMsg_prep(UINT8 msgType, pkt_aecp_t *p)
{
    pkt_aecp_acquireE_t *pkt;
    UINT8 missMType = FALSE;

    AmbaMisra_TypeCast(&pkt, &p);
    if (AmbaWrap_memset(pkt, 0, sizeof(pkt_aecp_acquireE_t))!= 0U) { }

    /* AVTP Format */
    switch (msgType) {
    case AECP_MSG_AEM_COMMAND:
    case AECP_MSG_AEM_RESPONSE:
    case AECP_MSG_ADDRESS_ACCESS_COMMAND:
    case AECP_MSG_ADDRESS_ACCESS_RESPONSE:
    case AECP_MSG_AVC_COMMAND:
    case AECP_MSG_AVC_RESPONSE:
    case AECP_MSG_VENDOR_UNIQUE_COMMAND:
    case AECP_MSG_VENDOR_UNIQUE_RESPONSE:
    case AECP_MSG_HDCP_APM_COMMAND:
    case AECP_MSG_HDCP_APM_RESPONSE:
    case AECP_MSG_EXTENDED_COMMAND:
    case AECP_MSG_EXTENDED_RESPONSE:
        p->avtpdu_hdr.message_type = msgType;
        break;

    default:
        /* TODO: default */
        missMType = TRUE;
        break;
    }

    if (missMType == FALSE) {
        p->avtpdu_hdr.control_data_len = (UINT16)(sizeof(pkt_aecp_acquireE_t) - sizeof(avtpdu_hdr_t));
        p->avtpdu_hdr.status           = AECP_SS_SUCCESS;
        p->avtpdu_hdr.version          = 0;
        p->avtpdu_hdr.sv               = 0;
        p->avtpdu_hdr.subtype          = AVTPDU_SUBTYPE_AECP;
        {
            const void *vptr = &(p->avtpdu_hdr);
            UINT32 *ptr;
            AmbaMisra_TypeCast(&ptr, &vptr);

            *ptr = Amba_htonl(*ptr);
        }
        p->avtpdu_hdr.target_entity_id = Amba_htonll(0x1122334455667788u);

        p->aecpdu.controller_entity_id = Amba_htonll(0x0123456789ABCDEFu);
        p->aecpdu.sequence_id          = Amba_htons(5566u);
        pkt->command_type              = AECP_CMD_ACQUIRE_ENTITY;
        pkt->flags                     = Amba_htonl(AECP_ACQUIRE_FLAG_PERSISTENT);
//        pkt->owner_id = 0; // 0 as command,  EntityID of controller as response;
        pkt->owner_id = Amba_htonll(0x123456789ABCDEF0u); // 0 as command,  EntityID of controller as response;

        pkt->descriptor_type  = Amba_htons(AVDECC_DT_CLOCK_SOURCE);
        pkt->descriptor_index = Amba_htons(0x0002);
    }

    return 0;
}

#if 0
typedef struct {
} AEMCommandResponse_t;
#endif


/* 9.2.2.3 AVDECC Entity Model State Machines */
/* global state machine variable that contains the AVDECC Entity’s Entity ID */
//static UINT64 myEntityID;
/* global variable contains the current time of a local clock that always advances forward. */
//UINT64 currentTime;
/* TODO */

typedef struct {
#if 0
    AEMCommandResponse_t *rcvdCommand;    /**< set to the contents of a received AEM AECPDU. */
    AEMCommandResponse_t *unsolicited;    /**< set by the application when it wants to send an unsolicited AEM_RESPONSE to an AVDECC Controller. */
#endif
    UINT8                 rcvdAEMCommand; /**< set to TRUE when the rcvdCommand is set with an AECP message with a message_type of AEM_COMMAND */

    UINT8                 doUnsolicited;  /**< set to TRUE when the unsolicited is set by the application */

    UINT8                 state;

    // acquireEntity(command)
    // lockEntity(command)
    // processCommand(command)

    pkt_aecp_t *p;

} sm_aecp_em_t;


#define PRIORITY_AECP_EM               (200u)
#define STK_SIZE_AECP_EM               (0x2000u)

static sm_aecp_em_t SM_Aecp_EM;
#define S_AECP_EM_IDLE                 (0U)
#define S_AECP_EM_WAITING              (1U)
#define S_AECP_EM_UNSOLICITED_RESPONSE (2U)
#define S_AECP_EM_RECEIVED_COMMAND     (3U)



void *avdecc_sm_aecp_em(void *a);
void *avdecc_sm_aecp_em(void *a)
{
    UINT32 loop = 1U;
    (void)a;
    while (loop == 1U) {
        (void)AmbaKAL_TaskSleep(10);

        switch (SM_Aecp_EM.state) {
        case S_AECP_EM_IDLE:
            break;

        case S_AECP_EM_WAITING:

            SM_Aecp_EM.rcvdAEMCommand = FALSE;
#if 0
            rcvdId                    = 0
#endif
            SM_Aecp_EM.doUnsolicited  = FALSE;
            while (loop == 1U) {
                UINT8 toBreak = 0u;

                if (doTerminate() == TRUE) {
                    SM_Aecp_EM.state = S_AECP_EM_IDLE;
                    toBreak = 1u;
                } else if (SM_Aecp_EM.doUnsolicited == TRUE) {
                    SM_Aecp_EM.state = S_AECP_EM_UNSOLICITED_RESPONSE;
                    toBreak = 1u;
                } else if ((SM_Aecp_EM.rcvdAEMCommand == TRUE) &&
                           (SM_Aecp_EM.p->avtpdu_hdr.target_entity_id == myEntityID)) {
                    SM_Aecp_EM.state = S_AECP_EM_RECEIVED_COMMAND;
                    toBreak = 1u;
                } else if ((SM_Aecp_EM.rcvdAEMCommand == TRUE) &&
                           (SM_Aecp_EM.p->avtpdu_hdr.target_entity_id != myEntityID)) {
                    SM_Aecp_EM.state = S_AECP_EM_WAITING;
                    toBreak = 1u;
                } else {
                    (void)AmbaKAL_TaskSleep(10);
                }
                if (toBreak == 1u) {
                    break;
                }
            }
            /* UCT to next */
            SM_Aecp_EM.state = S_AECP_EM_WAITING;
            break;

        case S_AECP_EM_UNSOLICITED_RESPONSE:
#if 0
            txResponse(SM_Aecp_EM.unsolicited);
#endif
            /* UCT to next */
            SM_Aecp_EM.state = S_AECP_EM_WAITING;
            break;

        case S_AECP_EM_RECEIVED_COMMAND:
#if 0
            if (ACQUIRE_ENTITY == rcvdCommand.command_type) {
                response = acquireEntity(rcvdCommand);
            } else if (LOCK_ENTITY == rcvdCommand.command_type) {
                response = lockEntity(rcvdCommand);
            } else if (ENTITY_AVAILABLE == rcvdCommand.command_type) {
                response = rcvdCommand;
            } else {
                response = processCommand(rcvdCommand);
                txResponse(response);
            }
#endif
            /* UCT to next */
            SM_Aecp_EM.state = S_AECP_EM_WAITING;
            break;

        default:
            /* TODO */
            break;
        }
    }
    return NULL;
}




typedef struct {
#if 0
    AEMCommandResponse_t *rcvdResponse;          /**< set to the contents of a received AEM AECPDU. */
    InflightCommand_t * inflight;                /**< a dynamic list of InflightCommands that are in the process of being performed. */
    AEMCommandResponse_t *command;               /**< set by the application when it wants to send an AEM_COMMAND to an AVDECC Entity. */
#endif

    UINT8               rcvdNormalResponse;      /**< set to TRUE when the rcvdResponse is set with an AECP message with a message_type of AEM_RESPONSE without the unsolicited field set */
    UINT8               rcvdUnsolicitedResponse; /**< set to TRUE when the rcvdResponse is set with an AECP message with a message_type of AEM_RESPONSE with the unsolicited field set. */

    UINT8 doCommand;                             /**< set to TRUE when the command is set by the application */

    UINT8               state;

    // txCommand(command)
    // processUnsolicited(response)
    // processResponse(response)
    // timeout(inflight)

    pkt_aecp_t *p;

} sm_aecp_emc_t;


#define PRIORITY_AECP_EMC               (200u)
#define STK_SIZE_AECP_EMC               (0x2000u)

static sm_aecp_emc_t SM_Aecp_EMC;
#define S_AECP_EMC_IDLE                 (0U)
#define S_AECP_EMC_WAITING              (1U)
#define S_AECP_EMC_SEND_COMMAND         (2U)
#define S_AECP_EMC_RECEIVED_UNSOLICITED (3U)
#define S_AECP_EMC_RECEIVED_RESPONSE    (4U)
#define S_AECP_EMC_TIMEOUT              (5U)


void *avdecc_sm_aecp_emc(void *a);
void *avdecc_sm_aecp_emc(void *a)
{
    UINT32 loop = 1U;
    (void)a;
    while (loop == 1U) {
        (void)AmbaKAL_TaskSleep(10);

        switch (SM_Aecp_EMC.state) {
        case S_AECP_EMC_IDLE :
            break;

        case S_AECP_EMC_WAITING:
            SM_Aecp_EMC.rcvdUnsolicitedResponse = FALSE;
            SM_Aecp_EMC.rcvdNormalResponse = FALSE;
            while (loop == 1U) {
                UINT8 toBreak = 0u;

                if (doTerminate() == TRUE) {
                    SM_Aecp_EMC.state = S_AECP_EM_IDLE;
                    toBreak = 1u;
                } else if (SM_Aecp_EMC.doCommand == TRUE) {
                    SM_Aecp_EMC.state = S_AECP_EMC_SEND_COMMAND;
                    toBreak = 1u;
                } else if ((SM_Aecp_EMC.rcvdUnsolicitedResponse == TRUE) &&
                        (SM_Aecp_EMC.p->aecpdu.controller_entity_id == myEntityID)) {
                    SM_Aecp_EMC.state = S_AECP_EMC_RECEIVED_UNSOLICITED;
                    toBreak = 1u;
                } else if ((SM_Aecp_EMC.rcvdNormalResponse == TRUE) &&
                        (SM_Aecp_EMC.p->aecpdu.controller_entity_id == myEntityID)) {
                    SM_Aecp_EMC.state = S_AECP_EMC_RECEIVED_RESPONSE;
                    toBreak = 1u;
#if 0
                } else if (currentTime >= inflight[x].timeout) {
                    SM_Aecp_EMC.state = S_AECP_EMC_TIMEOUT;
                    toBreak = 1u;
#endif
                } else if (((SM_Aecp_EMC.rcvdUnsolicitedResponse == TRUE) ||
                            (SM_Aecp_EMC.rcvdNormalResponse == TRUE)) &&
                           (SM_Aecp_EMC.p->aecpdu.controller_entity_id != myEntityID)) {
                    SM_Aecp_EMC.state = S_AECP_EMC_WAITING;
                    toBreak = 1u;
                } else {
                    (void)AmbaKAL_TaskSleep(10);
                }
                if (toBreak == 1u) {
                    break;
                }
            }
            /* UCT to next */
            SM_Aecp_EMC.state = S_AECP_EMC_WAITING;
            break;

        case S_AECP_EMC_SEND_COMMAND:
#if 0
            txCommand(command);
            SM_Aecp_EMC.doCommand = FALSE;
#endif
            /* UCT to next */
            SM_Aecp_EMC.state = S_AECP_EMC_WAITING;
            break;

        case S_AECP_EMC_RECEIVED_UNSOLICITED:
#if 0
            processUnsolicited(rcvdResponse);
#endif
            /* UCT to next */
            SM_Aecp_EMC.state = S_AECP_EMC_WAITING;
            break;

        case S_AECP_EMC_RECEIVED_RESPONSE:
#if 0
            processResponse(rcvdResponse);
#endif
            /* UCT to next */
            SM_Aecp_EMC.state = S_AECP_EMC_WAITING;
            break;

        case S_AECP_EMC_TIMEOUT:
#if 0
            if (inflight[x].retried) {
                timeout(inflight[x]);
            } else {
                txCommand(inflight[x].command)
            }
#endif
            /* UCT to next */
            SM_Aecp_EMC.state = S_AECP_EMC_WAITING;
            break;

        default:
            /* TODO */
            break;
        }
    }
    return NULL;
}



/* 9.2.2.6.1 Address Access Entity State Machine */

#if 0
/* 9.2.2.6.1.1.1 AACommandResponse */
typedef struct {
} AACommandResponse_t;
#endif

// 9.2.2.6.1.2.3 myEntityID
//myEntityID is a global state machine variable that contains the AVDECC Entity’s Entity ID.

typedef struct {
#if 0
    AACommandResponse_t *rcvdCommand; /**< set to the contents of a received Address Access AECPD */
#endif
    UINT8 rcvdAACommand; /**< set to TRUE when the rcvdCommand is set with an AECP message with a message_type of ADDRESS_ACCESS_COMMAND */

    UINT8               state;

    // processCommand(command)

    pkt_aecp_t *p;

} sm_aecp_aae_t;


#define PRIORITY_AECP_AAE               (200u)
#define STK_SIZE_AECP_AAE               (0x2000u)

static sm_aecp_aae_t SM_Aecp_AAE;
#define S_AECP_AAE_IDLE                 (0U)
#define S_AECP_AAE_WAITING              (1U)
#define S_AECP_AAE_RECEIVED_COMMAND     (2U)


void *avdecc_sm_aecp_aae(void *a);
void *avdecc_sm_aecp_aae(void *a)
{
    UINT32 loop = 1U;
    (void)a;
    while (loop == 1U) {
        (void)AmbaKAL_TaskSleep(10);

        switch (SM_Aecp_AAE.state) {
        case S_AECP_AAE_IDLE :
            break;

        case S_AECP_AAE_WAITING:
            SM_Aecp_AAE.rcvdAACommand = FALSE;
#if 0
            while (loop == 1U) {
                UINT8 toBreak = 0u;

                if (doTerminate() == TRUE) {
                    SM_Aecp_AAE.state = S_AECP_AAE_IDLE;
                    toBreak = 1u;
                } else if ((SM_Aecp_AAE.rcvdAACommand == TRUE) &&
                        (rcvdCommand.rcvdCommand == myEntityID)) {
                    SM_Aecp_AAE.state = S_AECP_AAE_RECEIVED_COMMAND;
                    toBreak = 1u;
                } else if ((SM_Aecp_AAE.rcvdAACommand == TRUE) &&
                        (rcvdCommand.rcvdCommand != myEntityID)) {
                    SM_Aecp_AAE.state = S_AECP_AAE_WAITING;
                    toBreak = 1u;
                } else {
                    (void)AmbaKAL_TaskSleep(10);
                }
                if (toBreak == 1u) {
                    break;
                }
            }
#endif
            /* UCT to next */
            SM_Aecp_AAE.state = S_AECP_AAE_WAITING;
            break;

        case S_AECP_AAE_RECEIVED_COMMAND:
#if 0
            response = processCommand(rcvdCommand);
            txResponse(response)
#endif
            /* UCT to next */
            SM_Aecp_AAE.state = S_AECP_AAE_WAITING;
            break;

        default:
            /* TODO */
            break;
        }
    }
    return NULL;
}



/* 9.2.2.6.2 Address Access Controller State Machine */

// 9.2.2.6.2.2.3 myEntityID
//myEntityID is a global state machine variable that contains the AVDECC Entity’s Entity ID.
// 9.2.2.6.2.2.4 currentTime
//The currentTime global variable contains the current time of a local clock that always advances forward.


typedef struct {
#if 0
    AACommandResponse_t *rcvdResponse; /**< set to the contents of a received Address Access AECPDU */
    InflightCommand_t *inflight;
    AACommandResponse_t *command;
#endif
    UINT8 doCommand;    /**< set to TRUE when the command is set by the application */
    UINT8 rcvdNormalResponse; /**< set to TRUE when the rcvdResponse is set with an AECP message with a message_type of ADDRESS_ACCESS_RESPONSE. */

    UINT8               state;

    // txCommand(command)
    // processResponse(response)
    // timeout(inflight)

    pkt_aecp_t *p;

} sm_aecp_aac_t;


#define PRIORITY_AECP_AAC               (200u)
#define STK_SIZE_AECP_AAC               (0x2000u)

static sm_aecp_aac_t SM_Aecp_AAC;
#define S_AECP_AAC_IDLE                 (0U)
#define S_AECP_AAC_WAITING              (1U)
#define S_AECP_AAC_SEND_COMMAND         (2U)
#define S_AECP_AAC_RECEIVED_RESPONSE    (3U)
#define S_AECP_AAC_TIMEOUT              (4U)


void *avdecc_sm_aecp_aac(void *a);
void *avdecc_sm_aecp_aac(void *a)
{
    UINT32 loop = 1U;
    (void)a;
    while (loop == 1U) {
        (void)AmbaKAL_TaskSleep(10);

        switch (SM_Aecp_AAC.state) {
        case S_AECP_AAC_IDLE :
            break;

        case S_AECP_AAC_WAITING:
            SM_Aecp_AAC.rcvdNormalResponse = FALSE;
            while (loop == 1U) {
                UINT8 toBreak = 0u;

                if (doTerminate() == TRUE) {
                    SM_Aecp_AAC.state = S_AECP_AAC_IDLE;
                    toBreak = 1u;
                } else if (SM_Aecp_AAC.doCommand == TRUE) {
                    SM_Aecp_AAC.state = S_AECP_AAC_SEND_COMMAND;
                    toBreak = 1u;
                } else if ((SM_Aecp_AAC.rcvdNormalResponse == TRUE) &&
                        (SM_Aecp_AAC.p->aecpdu.controller_entity_id == myEntityID)) {
                    SM_Aecp_AAC.state = S_AECP_AAC_RECEIVED_RESPONSE;
                    toBreak = 1u;
#if 0
                } else if (currentTime > inflight[x].timeout) {
                    SM_Aecp_AAC.state = S_AECP_AAC_TIMEOUT;
                    toBreak = 1u;
#endif
                } else if ((SM_Aecp_AAC.rcvdNormalResponse == TRUE) &&
                        (SM_Aecp_AAC.p->aecpdu.controller_entity_id != myEntityID)) {
                    SM_Aecp_AAC.state = S_AECP_AAC_WAITING;
                    toBreak = 1u;
                } else {
                    (void)AmbaKAL_TaskSleep(10);
                }
                if (toBreak == 1u) {
                    break;
                }
            }
            /* UCT to next */
            SM_Aecp_AAC.state = S_AECP_AAC_WAITING;
            break;

        case S_AECP_AAC_SEND_COMMAND:
#if 0
            txCommand(command)
            SM_Aecp_AAC.doCommand = FALSE
#endif
            /* UCT to next */
            SM_Aecp_AAC.state = S_AECP_AAC_WAITING;
            break;

        case S_AECP_AAC_RECEIVED_RESPONSE:
#if 0
            processResponse(rcvdResponse)
#endif
            /* UCT to next */
            SM_Aecp_AAC.state = S_AECP_AAC_WAITING;
            break;

        case S_AECP_AAC_TIMEOUT:
#if 0
            if (inflight[x].retried) {
                timeout(inflight[x])
            } else {
                txCommand(inflight[x].command)
            }
#endif
            /* UCT to next */
            SM_Aecp_AAC.state = S_AECP_AAC_WAITING;
            break;

        default:
            /* TODO */
            break;
        }
    }
    return NULL;
}


void avdecc_aecp_init(void)
{
static AMBA_KAL_TASK_t tsk_aecp_em __attribute__((section(".bss.noinit")));
static UINT8 stk_aecp_em[STK_SIZE_AECP_EM] __attribute__((section(".bss.noinit")));
static AMBA_KAL_TASK_t tsk_aecp_emc __attribute__((section(".bss.noinit")));
static UINT8 stk_aecp_emc[STK_SIZE_AECP_EMC] __attribute__((section(".bss.noinit")));
static AMBA_KAL_TASK_t tsk_aecp_aae __attribute__((section(".bss.noinit")));
static UINT8 stk_aecp_aae[STK_SIZE_AECP_AAE] __attribute__((section(".bss.noinit")));
static AMBA_KAL_TASK_t tsk_aecp_aac __attribute__((section(".bss.noinit")));
static UINT8 stk_aecp_aac[STK_SIZE_AECP_AAC] __attribute__((section(".bss.noinit")));

    myEntityID = pAvdeccCfg->entity_descriptor.entity_id;
    SM_Aecp_EM.state = S_AECP_EM_IDLE;
    SM_Aecp_EMC.state = S_AECP_EMC_IDLE;

    {
        UINT32 err;
        static char name_acmp_em[] = "acmp EM";
        static char name_acmp_emc[] = "acmp EM C";
        static char name_acmp_aae[] = "acmp AAE";
        static char name_acmp_aac[] = "acmp AAC";

        if (AmbaWrap_memset(&tsk_aecp_em, 0, sizeof(tsk_aecp_em))!= 0U) { }
        err = AmbaKAL_TaskCreate(&tsk_aecp_em, name_acmp_em, PRIORITY_AECP_EM,
                                 avdecc_sm_aecp_em, NULL,
                                 stk_aecp_em, STK_SIZE_AECP_EM,
                                 AMBA_KAL_DONT_START);
        if (err != OK) {
            AmbaPrint_PrintUInt5("AmbaKAL_TaskCreate: 0x%X", err, 0u, 0u, 0u, 0u);
        } else {
            (void)AmbaKAL_TaskSetSmpAffinity(&tsk_aecp_em, ACDECC_CORE);
//            (void)AmbaKAL_TaskResume(&tsk_aecp_em);
        }

        if (AmbaWrap_memset(&tsk_aecp_emc, 0, sizeof(tsk_aecp_emc))!= 0U) { }
        err = AmbaKAL_TaskCreate(&tsk_aecp_emc, name_acmp_emc, PRIORITY_AECP_EMC,
                                 avdecc_sm_aecp_emc, NULL,
                                 stk_aecp_emc, STK_SIZE_AECP_EMC,
                                 AMBA_KAL_DONT_START);
        if (err != OK) {
            AmbaPrint_PrintUInt5("AmbaKAL_TaskCreate: 0x%X", err, 0u, 0u, 0u, 0u);
        } else {
            (void)AmbaKAL_TaskSetSmpAffinity(&tsk_aecp_emc, ACDECC_CORE);
//            (void)AmbaKAL_TaskResume(&tsk_aecp_emc);
        }

        if (AmbaWrap_memset(&tsk_aecp_aae, 0, sizeof(tsk_aecp_aae))!= 0U) { }
        err = AmbaKAL_TaskCreate(&tsk_aecp_aae, name_acmp_aae, PRIORITY_AECP_AAE,
                                 avdecc_sm_aecp_aae, NULL,
                                 stk_aecp_aae, STK_SIZE_AECP_AAE,
                                 AMBA_KAL_DONT_START);
        if (err != OK) {
            AmbaPrint_PrintUInt5("AmbaKAL_TaskCreate: 0x%X", err, 0u, 0u, 0u, 0u);
        } else {
            (void)AmbaKAL_TaskSetSmpAffinity(&tsk_aecp_aae, ACDECC_CORE);
//            (void)AmbaKAL_TaskResume(&tsk_aecp_aae);
        }

        if (AmbaWrap_memset(&tsk_aecp_aac, 0, sizeof(tsk_aecp_aac))!= 0U) { }
        err = AmbaKAL_TaskCreate(&tsk_aecp_aac, name_acmp_aac, PRIORITY_AECP_AAC,
                                 avdecc_sm_aecp_aac, NULL,
                                 stk_aecp_aac, STK_SIZE_AECP_AAC,
                                 AMBA_KAL_DONT_START);
        if (err != OK) {
            AmbaPrint_PrintUInt5("AmbaKAL_TaskCreate: 0x%X", err, 0u, 0u, 0u, 0u);
        } else {
            (void)AmbaKAL_TaskSetSmpAffinity(&tsk_aecp_aac, ACDECC_CORE);
//            (void)AmbaKAL_TaskResume(&tsk_aecp_aac);
        }
    }
}

#define AECP_DBG (0)

/**
 *  @note: Only convert common part. (Figure 9.1)
 */
void avdecc_ntoh_aecp(const void *frame)
{
    pkt_aecp_t *p;

    AmbaMisra_TypeCast(&p, &frame);
    {
        const void *vptr = &(p->avtpdu_hdr);
        UINT32 *ptr;
        AmbaMisra_TypeCast(&ptr, &vptr);

        *ptr = Amba_ntohl(*ptr);
    }
#if AECP_DBG
    AmbaPrint("control_data_len: %d", p->avtpdu_hdr.control_data_len);
    AmbaPrint("status: %d", p->avtpdu_hdr.status);
    AmbaPrint("message_type: %d", p->avtpdu_hdr.message_type);
    AmbaPrint("version: %d", p->avtpdu_hdr.version);
    AmbaPrint("sv: %d", p->avtpdu_hdr.sv);
    AmbaPrint("subtype: %d", p->avtpdu_hdr.subtype);
#endif // AECP_DBG

    p->avtpdu_hdr.target_entity_id =  Amba_ntohll(p->avtpdu_hdr.target_entity_id);
#if AECP_DBG
    AmbaPrint("target_entity_id: %llX", p->avtpdu_hdr.target_entity_id);
#endif // AECP_DBG

    p->aecpdu.controller_entity_id = Amba_ntohll(p->aecpdu.controller_entity_id);
#if AECP_DBG
    AmbaPrint("controller_entity_id: %llX", p->aecpdu.controller_entity_id);
#endif // AECP_DBG
    p->aecpdu.sequence_id = Amba_ntohs(p->aecpdu.sequence_id);
#if AECP_DBG
    AmbaPrint("sequence_id: %X", p->aecpdu.sequence_id);
#endif // AECP_DBG
}

static void aecp_send_aem_rsp(const avdecc_pkt_t *pkt)
{
    UINT16 size;
    void * vptr;
    const char * buf;
    const char * pbuf;
    pkt_aecp_t *rsp;
    UINT32 err;
    const UINT8 *cp;
    const pkt_aecp_t *p;
    UINT8  dmac[6];

    cp = pkt->pkt;
    AmbaMisra_TypeCast(&p, &cp);

    (void)AmbaEnet_GetTxBuf(0, &vptr);
    AmbaMisra_TypeCast(&buf, &vptr);

    if (avdecc_IsVlan() == TRUE) {
        pbuf = &buf[sizeof(ETH_VLAN_HDR_s)];
        size = (UINT16)sizeof(ETH_VLAN_HDR_s) ;
    } else {
        pbuf = &buf[sizeof(ETH_HDR_s)];
        size = (UINT16)sizeof(ETH_HDR_s);
    }
    AmbaMisra_TypeCast(&rsp, &pbuf);
    size += (UINT16)sizeof(pkt_aecp_t);

    err = avdecc_aecp_txMsg_prep(AECP_MSG_AEM_RESPONSE, rsp);
    if (err == 0U) {
        d_aecp_read_descriptor_t *d;

        AmbaMisra_TypeCast(&pbuf, &rsp);
        pbuf = &pbuf[sizeof(pkt_aecp_t)];

        AmbaMisra_TypeCast(&d, &pbuf);
        rsp->avtpdu_hdr.target_entity_id = Amba_ntohll(p->avtpdu_hdr.target_entity_id);
        rsp->aecpdu.controller_entity_id = Amba_ntohll(p->aecpdu.controller_entity_id);
        rsp->aecpdu.sequence_id = Amba_htons(p->aecpdu.sequence_id);

        size += (UINT16)sizeof(d_aecp_read_descriptor_t);
        d->command_type = Amba_htons((UINT16)(0x8000u|AECP_CMD_OPERATION_STATUS)) ;
        d->configuration_index = 0;
        d->descriptor_type = AVDECC_DT_ENTITY;
        d->descriptor_index = 0;

        if (AmbaWrap_memcpy(dmac, pkt->smac, 6)!= 0U) { }
        avdecc_eth_prep(buf, avdecc_IsVlan(), dmac, pAvdeccCfg->getSMac());
        (void) AmbaEnet_Tx(0u, size);
        //test_tx(0, buf, size);
        (void)size;
    }
}

// TODO: from global DB
extern UINT8 entity_locked;
extern UINT8 entity_acquired;
extern UINT8 haveAuth;
UINT8 entity_locked   = 0u;
UINT8 entity_acquired = 1u;
UINT8 haveAuth        = 1u;
static UINT8 isControllerEidMatched(UINT64 eid)
{
    UINT8 matched = FALSE;

    if (eid != 0u) {
        matched = TRUE;
    }

    return matched;
}

/* vendor call back */
cbf_vendor_unique_command_f cbf_vendor_unique_command = NULL;

void avdecc_process_aecp(const void *packet)
{
//    sm_aecp_em_t *sm = &(ei->sm_aecp_em);
    UINT32 loop = 1U;
    const avdecc_pkt_t *pkt;
    const UINT8 *cp;
    pkt_aecp_t *p;

    AmbaMisra_TypeCast(&pkt, &packet);
    cp = pkt->pkt;
    AmbaMisra_TypeCast(&p, &cp);
    /* Table 9.1 message_type field */
    switch (p->avtpdu_hdr.message_type) {
    case AECP_MSG_AEM_COMMAND:
        /*
         * Example for reboot only
         */
        if (p->avtpdu_hdr.target_entity_id == pAvdeccCfg->entity_descriptor.entity_id) {
            const pkt_aecp_aem_t *pAem;
            UINT8 status = 0;
            UINT16 cmd_type;

            AmbaMisra_TypeCast(&pAem, &cp);
            cmd_type = Amba_ntohs(pAem->command_type) & 0x7FFFu;
            if (cmd_type == AECP_CMD_REBOOT) {
                UINT8 doIt = 0u;

                // cf. 7.4.43.2 Restrictions
                if (isControllerEidMatched(pAem->aecp_hdr.aecpdu.controller_entity_id) == TRUE) {
                    doIt = 1u;
                } else {
                    if (entity_locked == 1u) {
                        status = AECP_SS_ENTITY_LOCKED;
                    } else if (entity_acquired == 1u) {
                        status = AECP_SS_ENTITY_ACQUIRED;
                    } else {
                        if (haveAuth != 0u) {
                            doIt = 1u;
                        } else {
                            status = AECP_SS_NOT_AUTHENTICATED;
                        }
                    }
                }

                if (doIt != 0u) {
                    const pkt_aecp_arm_reboot_t *aem_reboot;
                    UINT16 des_type;
                    UINT16 des_idx;

                    AmbaMisra_TypeCast(&aem_reboot, &cp);
                    des_type = Amba_ntohs(aem_reboot->descriptor_type);
                    des_idx = Amba_ntohs(aem_reboot->descriptor_index);
                    // descriptor_type == AVDECC_DT_MEMORY_OBJECT (0x000B)
                    // TODO: ...
                    AmbaPrint_PrintUInt5("AEM, REBOOT, descriptor_type: 0x%X, descriptor_index: 0x%X",
                            des_type,
                            des_idx,
                            0, 0, 0);
                    status = AECP_SS_NOT_SUPPORTED;
                } else {
                    // Do nothing
                }
            } else {
                status = AECP_SS_NOT_SUPPORTED;
            }

            /* sends an unsolicited notification with status. */
            (void)status;
            aecp_send_aem_rsp(pkt);
        }
#if 0
        SM_Aecp_EM.p = p;
        SM_Aecp_EM.rcvdAEMCommand = TRUE;
        while (loop == 1U) {
            (void)AmbaKAL_TaskSleep(10);
            if ((SM_Aecp_EM.state == S_AECP_EM_WAITING) ||
                (SM_Aecp_EM.state == S_AECP_EM_IDLE)) {
                    break;
            }
        }
#endif
        break;

    case AECP_MSG_AEM_RESPONSE:
        SM_Aecp_EMC.p = p;
        // if p-> unsolicited == NULL {
        SM_Aecp_EMC.rcvdNormalResponse = TRUE;
        //} else {
        SM_Aecp_EMC.rcvdUnsolicitedResponse = TRUE;
        //}
        while (loop == 1U) {
            (void)AmbaKAL_TaskSleep(10);
            if ((SM_Aecp_EMC.state == S_AECP_EM_WAITING) ||
                (SM_Aecp_EMC.state == S_AECP_EM_IDLE)) {
                    break;
            }
        }
        break;

    case AECP_MSG_ADDRESS_ACCESS_COMMAND:
        SM_Aecp_AAE.p = p;
        SM_Aecp_AAE.rcvdAACommand = TRUE;
        while (loop == 1U) {
            (void)AmbaKAL_TaskSleep(10);
            if ((SM_Aecp_AAE.state == S_AECP_AAE_WAITING) ||
                (SM_Aecp_AAE.state == S_AECP_AAE_IDLE)) {
                    break;
            }
        }
        break;

    case AECP_MSG_ADDRESS_ACCESS_RESPONSE:
        SM_Aecp_AAC.p = p;
        SM_Aecp_AAC.rcvdNormalResponse = TRUE;
        while (loop == 1U) {
            (void)AmbaKAL_TaskSleep(10);
            if ((SM_Aecp_AAC.state == S_AECP_AAC_WAITING) ||
                (SM_Aecp_AAC.state == S_AECP_AAC_IDLE)) {
                    break;
            }
        }
        break;

    /*
     * 9.2.2.9 Legacy AV/C State Machines
     */
    case AECP_MSG_AVC_COMMAND:
        /* 9.2.2.9.1 Legacy A/VC Entity State Machine */
        break;

    case AECP_MSG_AVC_RESPONSE:
        /* 9.2.2.9.2 Legacy AV/C Controller State Machine */
        break;

    /*
     * 9.2.2.12 Vendor Unique State Machines
     */
    case AECP_MSG_VENDOR_UNIQUE_COMMAND:
        /* 9.2.2.12.1 Vendor Unique Entity State Machine */
        if (cbf_vendor_unique_command != NULL) {
            cbf_vendor_unique_command(packet);
        }
        else {
            const pkt_aecp_vendor_t *vendor;

            AmbaMisra_TypeCast(&vendor, &cp);

            AmbaPrint_PrintUInt5("AECP_MSG_VENDOR_UNIQUE_COMMAND len 0x%x payload[0] 0x%x",
                    vendor->aecp_hdr.avtpdu_hdr.control_data_len, vendor->payload[0], 0, 0, 0);
        }

        break;

    case AECP_MSG_VENDOR_UNIQUE_RESPONSE:
        /* 9.2.2.12.2 Vendor Unique Controller State Machine */
        break;

    /*
     * 9.2.2.15 HDCP APM State Machines
     */
    case AECP_MSG_HDCP_APM_COMMAND:
        /* 9.2.2.15.1 HDCP APM Entity State Machine */
        break;

    case AECP_MSG_HDCP_APM_RESPONSE:
        /* 9.2.2.15.2 HDCP APM Controller State Machine */
        break;

    case AECP_MSG_EXTENDED_COMMAND:
        /* Extended command, reserved for future use. */
        break;

    case AECP_MSG_EXTENDED_RESPONSE:
        /* Extended response, reserved for future use. */
        break;

    default:
        /* default */
        break;
    }
}


