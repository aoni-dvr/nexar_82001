/**
 * @file aecp.c
 *
 * Copyright (c) 2019 Ambarella International LP
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
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "avdecc.h"
#include "avdecc_private.h"

#define AECP_DBG (0)    /**< Debug flag */

#define AECP_TASK_PRIORITY_DEFAULT      (200U)      /**< Default task priority */
#define AECP_TASK_STACK_SIZE_DEFAULT    (0x2000U)   /**< Default task stack size */

#define AECP_MSGQ_MAX_NUM_DEFAULT       (2U)        /**< Default max message number */
#define AECP_VU_PKT_MAX_NUM_DEFAULT     (2U)        /**< Number of free buffer to store frame. */

#define AECP_TASK_EM    (0) /**< EM task */
#define AECP_TASK_EMC   (1) /**< EMC task */
#define AECP_TASK_AAE   (2) /**< AAE task */
#define AECP_TASK_AAC   (3) /**< AAC task */
#define AECP_TASK_VU    (4) /**< VU task */
#define AECP_TASK_VUC   (5) /**< VUC task */
#define AECP_TASK_NUM   (6) /**< Max task number */

#define AECP_EM_STATE_WAITING               (0U)    /**< EM State: waiting */
#define AECP_EM_STATE_UNSOLICITED_RESPONSE  (1U)    /**< EM State: unsolicited response */
#define AECP_EM_STATE_RECEIVED_COMMAND      (2U)    /**< EM State: received command */

#define AECP_EMC_STATE_WAITING              (0U)    /**< EMC State: waiting */
#define AECP_EMC_STATE_SEND_COMMAND         (1U)    /**< EMC State: send command */
#define AECP_EMC_STATE_RECEIVED_UNSOLICITED (2U)    /**< EMC State: received unsolicited */
#define AECP_EMC_STATE_RECEIVED_RESPONSE    (3U)    /**< EMC State: received response */
#define AECP_EMC_STATE_TIMEOUT              (4U)    /**< EMC State: timeout */

#define AECP_AAE_STATE_WAITING              (0U)    /**< AAE State: waiting */
#define AECP_AAE_STATE_RECEIVED_COMMAND     (1U)    /**< AAE State: received command */

#define AECP_AAC_STATE_WAITING              (0U)    /**< AAC State: waiting */
#define AECP_AAC_STATE_SEND_COMMAND         (1U)    /**< AAC State: send command */
#define AECP_AAC_STATE_RECEIVED_RESPONSE    (2U)    /**< AAC State: received response */
#define AECP_AAC_STATE_TIMEOUT              (3U)    /**< AAC State: timeout */

#define AECP_VU_STATE_WAITING               (0U)    /**< VU State: waiting */
#define AECP_VU_STATE_RECEIVED_COMMAND      (1U)    /**< VU State: received command */

#define AECP_VUC_STATE_WAITING              (0U)    /**< VUC State: waiting */
#define AECP_VUC_STATE_SEND_COMMAND         (1U)    /**< VUC State: send command */
#define AECP_VUC_STATE_RECEIVED_RESPONSE    (2U)    /**< VUC State: received response */
#define AECP_VUC_STATE_TIMEOUT              (3U)    /**< VUC State: timeout */

/**
 * Data type of AECP vendor unique buffer
 */
typedef struct AECP_VU_PKT {
    AECP_VU_AECPDU_s AecpVuPkt; /**< Aecp vendor unique packer */
    UINT32 Free;                /**< Free buffer indicator */
    struct AECP_VU_PKT *Next;   /**< Pointer to next buffer */
} AECP_VU_PKT_BUFFER_s;

/**
 * Data type of AECP information
 */
typedef struct {
    AMBA_KAL_MUTEX_t Mutex;                     /**< Mutex */
    AMBA_KAL_MUTEX_t TaskMutex[AECP_TASK_NUM];  /**< Task Mutex */
    AMBA_KAL_TASK_t Task[AECP_TASK_NUM];        /**< Task */
    UINT8 *TaskStack;                           /**< Task stack */
    UINT32 MsgNum;                              /**< Message number */
    AMBA_KAL_MSG_QUEUE_t *MsgQueue;             /**< Message queue */
    AMBA_KAL_MSG_QUEUE_t *MsgQueuePtr;          /**< Pointer for message queue */
    AEM_CMD_RESP_s *CmdMsgQBuffer;              /**< Command message queue */
    AEM_CMD_RESP_s *CmdMsgQBufferPtr;           /**< Pointer for command message queue */
    AECP_VU_PKT_BUFFER_s *VuCmdQBuffer;         /**< Vendor unique command queue buffer */
    AECP_VU_PKT_BUFFER_s *VuCmdQBufferPtr;      /**< Pointer for vendor unique command queue buffer */
    UINT32 MaxEntityNum;                        /**< Max entity number */
    UINT32 InflightCmdNum;                      /**< Inflight command number */
    AECP_INFLIGHT_CMD_s *InflightBuffer;        /**< Inflight command buffer */
    AECP_INFLIGHT_CMD_s *InflightCmd;           /**< Pointer for Inflight command */
    UINT32 VuCmdNum;                            /**< Vendor unique command number */
    AECP_VU_PKT_BUFFER_s *VuPktBuffer;          /**< Vendor unique command packet buffer */
    AECP_VU_PKT_BUFFER_s *VuPkt;                /**< Pointer for Vendor unique command packet buffer */
    AECP_EM_SM_s *EmSm;                         /**< EM state machine */
    AECP_EMC_SM_s *EmcSm;                       /**< EMC state machine */
    AECP_AAE_SM_s *AaeSm;                       /**< AAE state machine */
    AECP_AAC_SM_s *AacSm;                       /**< AAC state machine */
    AECP_VU_SM_s *VuSm;                         /**< VU state machine */
    AECP_VUC_SM_s *VucSm;                       /**< VUC state machine */
} AVDECC_AECP_s;

static AVDECC_AECP_s Aecp = {0}; /**< AECP manager */

/**
 *  Aecp inflight command initial
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_InflightCmdInit(void)
{
    UINT8 i;
    UINT32 Rval = OK;
    Aecp.InflightCmd = Aecp.InflightBuffer;
    for (i = 0U; i < (Aecp.InflightCmdNum - 1U); i++) {
        Aecp.InflightCmd[i].Retried = FALSE;
        Aecp.InflightCmd[i].Next = &(Aecp.InflightCmd[i + 1U]);
    }
    Aecp.InflightCmd[i].Retried = FALSE;
    Aecp.InflightCmd[i].Next = NULL;
    return Rval;
}

/**
 *  Get a new inflight command
 *  @return a pointer of inflight command
 */
static AECP_INFLIGHT_CMD_s *Aecp_NewInflightCmd(void)
{
    AECP_INFLIGHT_CMD_s *InflightCmd = Aecp.InflightCmd;
    if (Aecp.InflightCmd != NULL) {
        Aecp.InflightCmd = Aecp.InflightCmd->Next;
    }
    return InflightCmd;
}

/**
 *  Release a inflight command
 *  @param [in] InflightCmd The pointer use to release inflight command
 *  @return 0 - OK, others - Error
 */
static void Aecp_FreeInflightCmd(AECP_INFLIGHT_CMD_s *InflightCmd)
{
    if (InflightCmd != NULL) {
        InflightCmd->Next = Aecp.InflightCmd;
        Aecp.InflightCmd = InflightCmd;
    }
}

/**
 *  Add a inflight command to inflight command buffer
 *  @param [in] Head    The pointer of inflight command buffer
 *  @param [in] Cmd     The inflight command
 *  @param [in] Timeout The timeout time of the inflight command
 *  @return 0 - OK, others - Error
 */
static AECP_INFLIGHT_CMD_s *Aecp_AddInflightCmd(AECP_INFLIGHT_CMD_s *Head, AEM_CMD_RESP_s Cmd, UINT32 Timeout)
{
    AECP_INFLIGHT_CMD_s *InflightCmd = Aecp_NewInflightCmd();
    if (InflightCmd != NULL) {
        const UINT8 *Ptr;
        const AECP_PACKET_s *AecpPkt = NULL;
        Ptr = Cmd.Data;
        AmbaMisra_TypeCast(&AecpPkt, &Ptr);
        (void)AmbaKAL_GetSysTickCount(&InflightCmd->Timeout);
        InflightCmd->Timeout += Timeout;
        InflightCmd->Cmd = Cmd;
        InflightCmd->Original_sequence_id = AecpPkt->Aecpdu.Sequence_id;
        InflightCmd->Retried = FALSE;
        InflightCmd->Next = Head;
    }
    return InflightCmd;
}

/**
 *  Remove a inflight command to inflight command buffer
 *  @param [in] Head       The pointer of inflight command buffer
 *  @param [in] SequenceId The Sequence Id of the inflight command
 *  @return 0 - OK, others - Error
 */
static AECP_INFLIGHT_CMD_s *Aecp_RemoveInflightCmd(AECP_INFLIGHT_CMD_s *Head, UINT16 SequenceId)
{
    AECP_INFLIGHT_CMD_s *InflightCmd = NULL;
    AECP_INFLIGHT_CMD_s *PreInflightCmd = NULL;
    if (Head != NULL) {
        InflightCmd = Head;
        PreInflightCmd = Head;
        while (InflightCmd != NULL) {
            if (InflightCmd->Original_sequence_id == SequenceId) {
                break;
            }
            PreInflightCmd = InflightCmd;
            InflightCmd = InflightCmd->Next;
        }
        if (InflightCmd != NULL) {
            InflightCmd->Retried = FALSE;
            if (InflightCmd == Head) {
                InflightCmd = Head->Next;
                Aecp_FreeInflightCmd(Head);
            } else {
                PreInflightCmd->Next = InflightCmd->Next;
                Aecp_FreeInflightCmd(InflightCmd);
                InflightCmd = Head;
            }
        } else {
            InflightCmd = Head;
        }
    }
    return InflightCmd;
}

/**
 *  Get timeout inflight command
 *  @param [in] Head The pointer of inflight command buffer
 *  @return timeout inflight commans
 */
static AECP_INFLIGHT_CMD_s *Aecp_InflightCmdTimeout(AECP_INFLIGHT_CMD_s *Head)
{
    AECP_INFLIGHT_CMD_s *InflightCmd = Head;
    UINT32 CurrentTime;

    (void)AmbaKAL_GetSysTickCount(&CurrentTime);

    while (InflightCmd != NULL) {
        if (CurrentTime >= InflightCmd->Timeout) {
            break;
        }
        InflightCmd = InflightCmd->Next;
    }
    return InflightCmd;
}

/**
 *  Aecp vendor unique packet buffer initial
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_VuPktBufferInit(void)
{
    UINT32 i;
    UINT32 Rval = OK;
    Aecp.VuPkt = Aecp.VuPktBuffer;
    for (i = 0U; i < (Aecp.VuCmdNum - 1U); i++) {
        Aecp.VuPkt[i].Next = &(Aecp.VuPkt[i + 1U]);
    }
    Aecp.VuPkt[Aecp.VuCmdNum - 1U].Next = NULL;
    return Rval;
}

/**
 *  Get a new vendor unique packer
 *  @return a pointer of vu packet
 */
static AECP_VU_AECPDU_s *Aecp_AllocateVuPkt(void)
{
    AECP_VU_AECPDU_s *Pkt = NULL;
    if (Aecp.VuPkt != NULL) {
        AECP_VU_PKT_BUFFER_s *Buffer = Aecp.VuPkt;
        Aecp.VuPkt = Aecp.VuPkt->Next;
        Buffer->Free = FALSE;
        Pkt = &(Buffer->AecpVuPkt);
    }
    return Pkt;
}

/**
 *  Release a vendor unique packer
 *  @param Pkt The pointer of vendor unique packet for release
 *  @return 0 - OK, others - Error
 */
static void Aecp_FreeVuPkt(const AECP_VU_AECPDU_s *Pkt)
{
    if (Pkt != NULL) {
        AECP_VU_PKT_BUFFER_s *Buffer;
        AmbaMisra_TypeCast(&Buffer, &Pkt);
        if (Buffer->Free == FALSE) {
            Buffer->Free = TRUE;
            Buffer->Next = Aecp.VuPkt;
            Aecp.VuPkt = Buffer;
        } else {
            AmbaPrint_PrintStr5("%s, Pkt already free!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("%s, Pkt is NULL!", __func__, NULL, NULL, NULL, NULL);
    }
}

/*
 * pp.165 of IEEE 1722.1 - 2013
 *
 * An AVDECC AvdeccTalker or Listener shall implement and respond to the
 * ACQUIRE_ENTITY, LOCK_ENTITY, and ENTITY_AVAILABLE commands.
 * All other commands are optional for an AVDECC AvdeccTalker or Listener.
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
static UINT32 Aecp_PrepareTxResp(const AECP_AEM_s *AecpAemPkt)
{
    UINT32 Rval = OK;
    const void *Vptr = &(AecpAemPkt->AecpPkt.AvtpduHeader);
    UINT32 *Ptr;
    AmbaMisra_TypeCast(&Ptr, &Vptr);
    *Ptr = AVDECC_HTONL(*Ptr);
    return Rval;
}

/**
 *  Transmit response
 *  @param [in] Resp The pointer of response data
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_TxResponse(const UINT8 *Resp)
{
    UINT32 Rval;
    UINT16 Size;
    UINT8 AcmpMulticastMac[6U] = ACMP_MULTICAST_MAC;
    AECP_AEM_s *AecpAemPkt;
    AmbaMisra_TypeCast(&AecpAemPkt, &Resp);
    if (AecpAemPkt->Command_type == AECP_CMD_START_STREAMING) {
        Size = (UINT16)sizeof(AECP_START_STREAMING_s);
    } else if (AecpAemPkt->Command_type == AECP_CMD_STOP_STREAMING) {
        Size = (UINT16)sizeof(AECP_STOP_STREAMING_s);
    } else {
        Size = (UINT16)sizeof(AECP_AEM_s);
    }
    AecpAemPkt->AecpPkt.AvtpduHeader.Message_type = AECP_MSG_AEM_RESPONSE;
    Rval = Aecp_PrepareTxResp(AecpAemPkt);
    if (Rval == OK) {
        AecpAemPkt->AecpPkt.AvtpduHeader.Target_entity_id = AVDECC_HTONLL(AecpAemPkt->AecpPkt.AvtpduHeader.Target_entity_id);
        AecpAemPkt->AecpPkt.Aecpdu.Controller_entity_id = AVDECC_HTONLL(AecpAemPkt->AecpPkt.Aecpdu.Controller_entity_id);
        AecpAemPkt->AecpPkt.Aecpdu.Sequence_id = AVDECC_HTONS(AecpAemPkt->AecpPkt.Aecpdu.Sequence_id);
        Rval = Avdecc_NetTx(0U, AecpAemPkt, Size, AcmpMulticastMac);
        if (Rval != ETH_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaEnet_Tx() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  9.2.2.3.1.3.1 acquireEntity(command)
 *  @param [in] Cmd  The pointer of command data
 *  @param [in] Resp The pointer of response data
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_AcquireEntity(const UINT8 *Cmd, const UINT8 *Resp)
{
    UINT32 Rval = OK;
    AmbaMisra_TouchUnused(Cmd);
    AmbaMisra_TouchUnused(Resp);
    return Rval;
}

/**
 *  9.2.2.3.1.3.2 lockEntity(command)
 *  @param [in] Cmd  The pointer of command data
 *  @param [in] Resp The pointer of response data
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_LockEntity(const UINT8 *Cmd, const UINT8 *Resp)
{
    UINT32 Rval = OK;
    AmbaMisra_TouchUnused(Cmd);
    AmbaMisra_TouchUnused(Resp);
    return Rval;
}

/**
 *  9.2.2.3.1.3.3 processCommand(command)
 *  @param [in] Cmd  The pointer of command data
 *  @param [in] Resp The pointer of response data
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_ProcessCmd(const UINT8 *Cmd, const UINT8 *Resp)
{
    UINT32 Rval = OK;
    const AECP_AEM_s *AecpAemPkt = NULL;
    AmbaMisra_TypeCast(&AecpAemPkt, &Cmd);
    if (AecpAemPkt->Command_type == AECP_CMD_START_STREAMING) {
        const AECP_START_STREAMING_s *StartStreamingCmd;
        AECP_START_STREAMING_s *StartStreamingResp;
        UINT32 EntityType;
        UINT32 Connect = 0U;
        UINT8 *ConnectInfo;
        AmbaMisra_TypeCast(&StartStreamingCmd, &Cmd);
        AmbaMisra_TypeCast(&StartStreamingResp, &Resp);
        AmbaPrint_PrintStr5("%s, AECP_CMD_START_STREAMING", __func__, NULL, NULL, NULL, NULL);
        (void) Avdecc_GetEntityType(StartStreamingCmd->AecpAem.AecpPkt.AvtpduHeader.Target_entity_id, &EntityType);
        if (EntityType == AVDECC_LISTENER) {
            Rval = Listener_GetConnectInfo(StartStreamingCmd->AecpAem.AecpPkt.AvtpduHeader.Target_entity_id, &Connect, &ConnectInfo);
        } else if (EntityType == AVDECC_TALKER) {
            Rval = Talker_GetConnectInfo(StartStreamingCmd->AecpAem.AecpPkt.AvtpduHeader.Target_entity_id, &Connect, &ConnectInfo);
        } else {
            AmbaPrint_PrintStr5("%s, unkonw entity type", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
        if (Rval == OK) {
            if (AmbaWrap_memcpy(StartStreamingResp, StartStreamingCmd, sizeof(AECP_START_STREAMING_s))!= 0U) { }
            StartStreamingResp->AecpAem.AecpPkt.AvtpduHeader.Control_data_len = (UINT16)(sizeof(AECP_START_STREAMING_s) - sizeof(AECP_AVTPDU_HEADER_s));
            StartStreamingResp->AecpAem.AecpPkt.AvtpduHeader.Message_type = AECP_MSG_AEM_RESPONSE;
            if (Connect == 1U) {
                Rval = Avdecc_SendEvent(AVDECC_EVENT_START_STREAMING, StartStreamingCmd->AecpAem.AecpPkt.AvtpduHeader.Target_entity_id);
                if (Rval == OK) {
                    StartStreamingResp->AecpAem.AecpPkt.AvtpduHeader.Status = AECP_STATUS_SUCCESS;
                } else {
                    StartStreamingResp->AecpAem.AecpPkt.AvtpduHeader.Status = AECP_STATUS_BAD_ARGUMENTS;
                }
            } else {
                StartStreamingResp->AecpAem.AecpPkt.AvtpduHeader.Status = AECP_STATUS_NO_SUCH_DESCRIPTOR;
            }
        }
    } else if (AecpAemPkt->Command_type == AECP_CMD_STOP_STREAMING) {
        const AECP_STOP_STREAMING_s *StopStreamingCmd;
        AECP_STOP_STREAMING_s *StopStreamingResp;
        UINT32 EntityType;
        UINT32 Connect = 0U;
        UINT8 *ConnectInfo;
        AmbaMisra_TypeCast(&StopStreamingCmd, &Cmd);
        AmbaMisra_TypeCast(&StopStreamingResp, &Resp);
        AmbaPrint_PrintStr5("%s, AECP_CMD_STOP_STREAMING", __func__, NULL, NULL, NULL, NULL);
        (void) Avdecc_GetEntityType(StopStreamingCmd->AecpAem.AecpPkt.AvtpduHeader.Target_entity_id, &EntityType);
        if (EntityType == AVDECC_LISTENER) {
            Rval = Listener_GetConnectInfo(StopStreamingCmd->AecpAem.AecpPkt.AvtpduHeader.Target_entity_id, &Connect, &ConnectInfo);
        } else if (EntityType == AVDECC_TALKER) {
            Rval = Talker_GetConnectInfo(StopStreamingCmd->AecpAem.AecpPkt.AvtpduHeader.Target_entity_id, &Connect, &ConnectInfo);
        } else {
            AmbaPrint_PrintStr5("%s, unkonw entity type", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
        if (Rval == OK) {
            if (AmbaWrap_memcpy(StopStreamingResp, StopStreamingCmd, sizeof(AECP_STOP_STREAMING_s))!= 0U) { }
            StopStreamingResp->AecpAem.AecpPkt.AvtpduHeader.Control_data_len = (UINT16)(sizeof(AECP_STOP_STREAMING_s) - sizeof(AECP_AVTPDU_HEADER_s));
            StopStreamingResp->AecpAem.AecpPkt.AvtpduHeader.Message_type = AECP_MSG_AEM_RESPONSE;
            if (Connect == 1U) {
                Rval = Avdecc_SendEvent(AVDECC_EVENT_STOP_STREAMING, StopStreamingCmd->AecpAem.AecpPkt.AvtpduHeader.Target_entity_id);
                if (Rval == OK) {
                    StopStreamingResp->AecpAem.AecpPkt.AvtpduHeader.Status = AECP_STATUS_SUCCESS;
                } else {
                    StopStreamingResp->AecpAem.AecpPkt.AvtpduHeader.Status = AECP_STATUS_BAD_ARGUMENTS;
                }
            } else {
                StopStreamingResp->AecpAem.AecpPkt.AvtpduHeader.Status = AECP_STATUS_NO_SUCH_DESCRIPTOR;
            }
        }
    } else {
        //nothing
    }
    return Rval;
}

/**
 *  Command process done
 *  @param [in] EntityId The entity id which command process done
 *  @return 0 - OK, others - Error
 */
UINT32 Aecp_ProcessCmdDone(UINT64 EntityId)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_EM], AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        for (i = 0U; i < Aecp.MaxEntityNum;i++) {
            if (Aecp.EmSm[i].MyEntityId == EntityId) {
                Aecp.EmSm[i].CmdProcessDone = 1U;
                break;
            }
        }
        if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_EM]) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  9.2.2.3.1 AVDECC Entity Model Entity State Machine
 *  @param [in] StateMachine The pointer of state machine
 */
static void Aecp_EmSm(AECP_EM_SM_s *StateMachine)
{
    UINT32 Rval = OK;
    switch (StateMachine->State) {
    case AECP_EM_STATE_WAITING:
        if (StateMachine->DoTerminate == TRUE) {
            // TODO:
        } else if (StateMachine->DoUnsolicited == TRUE) {
            StateMachine->State = AECP_EM_STATE_UNSOLICITED_RESPONSE;
        } else if (StateMachine->RcvdAEMCommand == TRUE) {
            const UINT8 *AecpAemPktPtr = StateMachine->RcvdCommand.Data;
            const AECP_AEM_s *AecpAemPkt = NULL;
            AmbaMisra_TypeCast(&AecpAemPkt, &AecpAemPktPtr);
            StateMachine->RcvdAEMCommand = FALSE;
            if (AecpAemPkt->AecpPkt.AvtpduHeader.Target_entity_id == StateMachine->MyEntityId) {
                StateMachine->State = AECP_EM_STATE_RECEIVED_COMMAND;
            }
        } else {
            // TODO:
        }
        break;
    case AECP_EM_STATE_UNSOLICITED_RESPONSE:
#if 0
        txResponse(SM_Aecp_EM.unsolicited);
#endif
        StateMachine->State = AECP_EM_STATE_WAITING; /* UCT to AECP_EM_STATE_WAITING */
        break;
    case AECP_EM_STATE_RECEIVED_COMMAND:
    /*
        if (ACQUIRE_ENTITY == rcvdCommand.command_type) {
            response = acquireEntity(rcvdCommand);
        } else if (LOCK_ENTITY == rcvdCommand.command_type) {
            response = lockEntity(rcvdCommand);
        } else if (ENTITY_AVAILABLE == rcvdCommand.command_type) {
            response = rcvdCommand;
        } else {
            response = processCommand(rcvdCommand);
        }
        txResponse(response);
    */
        if (StateMachine->CmdProcessDone == 0U) {
            const UINT8 *AecpAemPktPtr = StateMachine->RcvdCommand.Data;
            AECP_AEM_s *AecpAemPkt = NULL;
            AmbaMisra_TypeCast(&AecpAemPkt, &AecpAemPktPtr);
            if (AecpAemPkt->AecpPkt.AvtpduHeader.Control_data_len != 0U) {
                if (AecpAemPkt->Command_type == AECP_CMD_ACQUIRE_ENTITY) {
                    Rval = Aecp_AcquireEntity(StateMachine->RcvdCommand.Data, StateMachine->Resp.Data);
                } else if (AecpAemPkt->Command_type == AECP_CMD_LOCK_ENTITY) {
                    Rval = Aecp_LockEntity(StateMachine->RcvdCommand.Data, StateMachine->Resp.Data);
                } else if (AecpAemPkt->Command_type == AECP_CMD_ENTITY_AVAILABLE) {
                    if (AmbaWrap_memcpy(StateMachine->Resp.Data, StateMachine->RcvdCommand.Data, sizeof(StateMachine->RcvdCommand.Data))!= 0U) { }
                    Rval = OK;
                } else {
                    Rval = Aecp_ProcessCmd(StateMachine->RcvdCommand.Data, StateMachine->Resp.Data);
                }
                if (Rval == OK) {
                    AecpAemPkt->AecpPkt.AvtpduHeader.Control_data_len = 0U;
                }
            }
        } else {
            Rval = Aecp_TxResponse(StateMachine->Resp.Data);
            if (Rval == OK) {
                StateMachine->CmdProcessDone = 0U;
            }
            StateMachine->State = AECP_EM_STATE_WAITING; /* UCT to AECP_EM_STATE_WAITING */
        }
        break;
    default:
        /* TODO */
        break;
    }
}

/**
 *  Aecp EM task entry.
 *  @param [in] Info The argument attached to entry function
 *  @return 0 - OK, others - Error
 */
static void *Aecp_EmTaskEntry(void *Info)
{
    (void)Info;
    for (;;) {
        UINT32 Busy = 0U;
        if (AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_EM], AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
            UINT8 i;
            for (i = 0U; i < Aecp.MaxEntityNum; i++) {
                if (Aecp.EmSm[i].MyEntityId != 0U) {
                    if (Aecp.EmSm[i].State == AECP_EMC_STATE_WAITING) {
                        AEM_CMD_RESP_s Msg;
                        if (AmbaKAL_MsgQueueReceive(&Aecp.MsgQueue[((UINT8)AECP_TASK_NUM * i)], &Msg, 10U) == KAL_ERR_NONE) {
                            if (AmbaWrap_memcpy(&Aecp.EmSm[i].RcvdCommand, &Msg, sizeof(AEM_CMD_RESP_s))!= 0U) { }
                            Aecp.EmSm[i].RcvdAEMCommand = TRUE;
                        }
                    }
                    Aecp_EmSm(&Aecp.EmSm[i]);
                    if (Aecp.EmcSm[i].State > AECP_EMC_STATE_WAITING) {
                        Busy++;
                    }
                }
            }
            if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_EM]) != KAL_ERR_NONE) {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake(AECP_TASK_EM) fail!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Busy == 0U) {
            (void) AmbaKAL_TaskSleep(10U);
        }
    }
}

/**
 *  Prepare command of Aecp for transmit
 *  @param [in] AecpPkt The Aecp packet
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_PrepareTxCmd(AECP_PACKET_s *AecpPkt)
{
    UINT32 Rval = OK;
    AecpPkt->AvtpduHeader.Subtype = AVTPDU_SUBTYPE_AECP;
    {
        const void *Vptr = &(AecpPkt->AvtpduHeader);
        UINT32 *Ptr;
        AmbaMisra_TypeCast(&Ptr, &Vptr);
        *Ptr = AVDECC_HTONL(*Ptr);
    }
    return Rval;
}

/**
 *  9.2.2.3.2.3.1 txCommand(command)
 *  @param [in] Cmd The pointer of command data
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_TxCommand(const UINT8 *Cmd)
{
    UINT32 Rval;
    UINT16 Size;
    UINT8 AcmpMulticastMac[6U] = ACMP_MULTICAST_MAC;
    AECP_AEM_s *AecpAemPkt;
    AmbaMisra_TypeCast(&AecpAemPkt, &Cmd);
    if (AecpAemPkt->Command_type == AECP_CMD_START_STREAMING) {
        Size = (UINT16)sizeof(AECP_START_STREAMING_s);
    } else if (AecpAemPkt->Command_type == AECP_CMD_STOP_STREAMING) {
        Size = (UINT16)sizeof(AECP_STOP_STREAMING_s);
    } else {
        Size = (UINT16)sizeof(AECP_AEM_s);
    }
    AecpAemPkt->AecpPkt.AvtpduHeader.Message_type = AECP_MSG_AEM_COMMAND;
    Rval = Aecp_PrepareTxCmd(&AecpAemPkt->AecpPkt);
    if (Rval == OK) {
        AecpAemPkt->AecpPkt.AvtpduHeader.Target_entity_id = AVDECC_HTONLL(AecpAemPkt->AecpPkt.AvtpduHeader.Target_entity_id);
        AecpAemPkt->AecpPkt.Aecpdu.Controller_entity_id = AVDECC_HTONLL(AecpAemPkt->AecpPkt.Aecpdu.Controller_entity_id);
        AecpAemPkt->AecpPkt.Aecpdu.Sequence_id = AVDECC_HTONS(AecpAemPkt->AecpPkt.Aecpdu.Sequence_id);
        Rval = Avdecc_NetTx(0U, AecpAemPkt, Size, AcmpMulticastMac);
        if (Rval != ETH_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaEnet_Tx() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/* 9.2.2.3.2.3.2 processUnsolicited(response) */

/**
 *  9.2.2.3.2.3.3 processResponse(response)
 *  @param [in] Resp The pointer of response data
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_ProcessResponse(const UINT8 *Resp)
{
    UINT32 Rval = OK;
    const AECP_AEM_s *AecpAemPkt = NULL;
    AmbaMisra_TypeCast(&AecpAemPkt, &Resp);
    if (AecpAemPkt->Command_type == AECP_CMD_START_STREAMING) {
        AECP_START_STREAMING_s *StartStreamingResp;
        AmbaMisra_TypeCast(&StartStreamingResp, &Resp);
        StartStreamingResp->AecpAem.AecpPkt.AvtpduHeader.Message_type = AECP_MSG_AEM_RESPONSE;
        if (StartStreamingResp->AecpAem.AecpPkt.AvtpduHeader.Status == ACMP_STATUS_SUCCESS) {
            Rval = Avdecc_SendEvent(AVDECC_EVENT_START_STREAMING_SUCCESS, StartStreamingResp->AecpAem.AecpPkt.AvtpduHeader.Target_entity_id);
        } else {
            Rval = Avdecc_SendEvent(AVDECC_EVENT_START_STREAMING_ERROR, StartStreamingResp->AecpAem.AecpPkt.AvtpduHeader.Target_entity_id);
        }
    } else if (AecpAemPkt->Command_type == AECP_CMD_STOP_STREAMING) {
        AECP_STOP_STREAMING_s *StopStreamingResp;
        AmbaMisra_TypeCast(&StopStreamingResp, &Resp);
        StopStreamingResp->AecpAem.AecpPkt.AvtpduHeader.Message_type = AECP_MSG_AEM_RESPONSE;
        if (StopStreamingResp->AecpAem.AecpPkt.AvtpduHeader.Status == ACMP_STATUS_SUCCESS) {
            Rval = Avdecc_SendEvent(AVDECC_EVENT_STOP_STREAMING_SUCCESS, StopStreamingResp->AecpAem.AecpPkt.AvtpduHeader.Target_entity_id);
        } else {
            Rval = Avdecc_SendEvent(AVDECC_EVENT_STOP_STREAMING_ERROR, StopStreamingResp->AecpAem.AecpPkt.AvtpduHeader.Target_entity_id);
        }
    } else {
        // nothing
    }
    return Rval;
}

/**
 *  9.2.2.3.2.3.4 timeout(inflight)
 *  @param [in] Inflight   The pointer of inflight command buffer
 *  @param [in] SequenceId The sequence id to search inflight command
 *  @return the pointer of inflight command
 */
static AECP_INFLIGHT_CMD_s *Aecp_Timeout(AECP_INFLIGHT_CMD_s *Inflight, UINT16 SequenceId)
{
    AECP_INFLIGHT_CMD_s *Head;
    Head = Aecp_RemoveInflightCmd(Inflight, SequenceId);
    return Head;
}

/**
 *  9.2.2.3.2 AVDECC Entity Model Controller State Machine
 *  @param [in] StateMachine The pointer of state machine
 */
static void Aecp_EmcSm(AECP_EMC_SM_s *StateMachine)
{
    switch (StateMachine->State) {
    case AECP_EMC_STATE_WAITING:
        {
            const AECP_INFLIGHT_CMD_s *TimeoutCmd = Aecp_InflightCmdTimeout(StateMachine->Inflight);
            if (TimeoutCmd != NULL) {// Check inflight status and handle timeout immediately.
                StateMachine->State = AECP_EMC_STATE_TIMEOUT;
            } else {
                if (StateMachine->DoTerminate == TRUE) {
                    // TODO:
                } else if (StateMachine->DoCommand == TRUE) {
                    StateMachine->DoCommand = FALSE;
                    StateMachine->State = AECP_EMC_STATE_SEND_COMMAND;
                } else if (StateMachine->RcvdUnsolicitedResponse == TRUE) {
                    const UINT8 *AecpAemPktPtr = StateMachine->RcvdResponse.Data;
                    const AECP_AEM_s *AecpAemPkt = NULL;
                    StateMachine->RcvdUnsolicitedResponse = FALSE;
                    AmbaMisra_TypeCast(&AecpAemPkt, &AecpAemPktPtr);
                    if (AecpAemPkt->AecpPkt.Aecpdu.Controller_entity_id == StateMachine->MyEntityId) {
                        StateMachine->State = AECP_EMC_STATE_RECEIVED_UNSOLICITED;
                    }
                } else if (StateMachine->RcvdNormalResponse == TRUE) {
                    const UINT8 *AecpAemPktPtr = StateMachine->RcvdResponse.Data;
                    const AECP_AEM_s *AecpAemPkt = NULL;
                    StateMachine->RcvdNormalResponse = FALSE;
                    AmbaMisra_TypeCast(&AecpAemPkt, &AecpAemPktPtr);
                    if (AecpAemPkt->AecpPkt.Aecpdu.Controller_entity_id == StateMachine->MyEntityId) {
                        StateMachine->State = AECP_EMC_STATE_RECEIVED_RESPONSE;
                    }
                } else {
                    // TODO: state or sleep
                }
            }
        }
        break;
    case AECP_EMC_STATE_SEND_COMMAND:
        {
            UINT32 Rval;
            const UINT8 *Cmd;
            AECP_AEM_s *AecpAemPkt;
            Cmd = StateMachine->Command.Data;
            AmbaMisra_TypeCast(&AecpAemPkt, &Cmd);
            AecpAemPkt->AecpPkt.Aecpdu.Controller_entity_id = StateMachine->MyEntityId;
            Rval = Aecp_TxCommand(StateMachine->Command.Data);
            if (Rval == OK) {
                StateMachine->Inflight = Aecp_AddInflightCmd(StateMachine->Inflight, StateMachine->Command, AVDECC_CMD_TIMEOUT);
            } else {
                AmbaPrint_PrintStr5("%s, Aecp_TxCommand() failed!", __func__, NULL, NULL, NULL, NULL);
            }
            StateMachine->State = AECP_EMC_STATE_WAITING; /* UCT to AECP_EMC_STATE_WAITING */
        }
        break;
    case AECP_EMC_STATE_RECEIVED_UNSOLICITED:
#if 0
        processUnsolicited(rcvdResponse);
#endif
        StateMachine->State = AECP_EMC_STATE_WAITING; /* UCT to AECP_EMC_STATE_WAITING */
        break;
    case AECP_EMC_STATE_RECEIVED_RESPONSE:
        {
            UINT32 Rval = Aecp_ProcessResponse(StateMachine->RcvdResponse.Data);
            if (Rval == OK) {
                const UINT8 *Ptr;
                const AECP_PACKET_s *AecpPkt = NULL;
                Ptr = StateMachine->RcvdResponse.Data;
                AmbaMisra_TypeCast(&AecpPkt, &Ptr);
                StateMachine->Inflight = Aecp_Timeout(StateMachine->Inflight, AecpPkt->Aecpdu.Sequence_id);
            }
            StateMachine->State = AECP_EMC_STATE_WAITING; /* UCT to AECP_EMC_STATE_WAITING */
        }
        break;
    case AECP_EMC_STATE_TIMEOUT:
        {
            AECP_INFLIGHT_CMD_s *TimeoutCmd = Aecp_InflightCmdTimeout(StateMachine->Inflight);
            UINT32 CurrentTime;
            if (TimeoutCmd != NULL) {
                if (TimeoutCmd->Retried == TRUE) {
                    const UINT8 *Ptr;
                    const AECP_PACKET_s *AecpPkt = NULL;
                    Ptr = TimeoutCmd->Cmd.Data;
                    AmbaMisra_TypeCast(&AecpPkt, &Ptr);
                    StateMachine->Inflight = Aecp_Timeout(StateMachine->Inflight, AecpPkt->Aecpdu.Sequence_id);
                    if (Avdecc_SendEvent(AVDECC_EVENT_CMD_TIMEOUT, AecpPkt->AvtpduHeader.Target_entity_id) != OK) {
                        AmbaPrint_PrintStr5("%s, Avdecc_SendEvent(AVDECC_EVENT_CMD_TIMEOUT) failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    if (Aecp_TxCommand(StateMachine->Inflight->Cmd.Data) == OK) {
                        TimeoutCmd->Retried = TRUE;
                        (void)AmbaKAL_GetSysTickCount(&CurrentTime);
                        TimeoutCmd->Timeout = CurrentTime + AVDECC_CMD_TIMEOUT;
                    }
                }
            }
            StateMachine->State = AECP_EMC_STATE_WAITING; /* UCT to AECP_EMC_STATE_WAITING */
        }
        break;
    default:
        /* TODO */
        break;
    }
}

/**
 *  Send Aecp command
 *  @param [in] Cmd The pointer of command data
 *  @return 0 - OK, others - Error
 */
UINT32 Aecp_SendAecpCmd(const AEM_CMD_RESP_s *Cmd)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Aecp.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_EMC], AMBA_KAL_WAIT_FOREVER);
        if (Rval == KAL_ERR_NONE) {
            UINT8 i;
            const AECP_PACKET_s *AecpPkt = NULL;
            AmbaMisra_TypeCast(&AecpPkt, &Cmd);
            for (i = 0U; i < Aecp.MaxEntityNum; i++) {
                if (Aecp.EmcSm[i].MyEntityId == AecpPkt->Aecpdu.Controller_entity_id) {
                    AEM_CMD_RESP_s Msg;
                    if (AmbaWrap_memcpy(&Msg, Cmd, sizeof(AEM_CMD_RESP_s))!= 0U) { }
                    Rval = AmbaKAL_MsgQueueSend(&Aecp.MsgQueue[(((UINT8)AECP_TASK_NUM * i) + (UINT8)AECP_TASK_EMC)], &Msg, AMBA_KAL_NO_WAIT);
                    if (Rval != KAL_ERR_NONE) {
                        AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueSend() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                    break;
                }
            }
            if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_EMC]) != KAL_ERR_NONE) {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = ERR_ARG;
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
        }
        if (AmbaKAL_MutexGive(&Aecp.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Aecp EMC task entry.
 *  @param [in] Info The argument attached to entry function
 *  @return 0 - OK, others - Error
 */
static void *Aecp_EmcTaskEntry(void *Info)
{
    (void)Info;
    for (;;) {
        UINT32 Busy = 0U;
        if (AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_EMC], AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
            UINT8 i;
            for (i = 0U; i < Aecp.MaxEntityNum; i++) {
                if (Aecp.EmcSm[i].MyEntityId != 0U) {
                    if (Aecp.EmcSm[i].State == AECP_EMC_STATE_WAITING) {
                        AEM_CMD_RESP_s Msg;
                        if (AmbaKAL_MsgQueueReceive(&Aecp.MsgQueue[(((UINT8)AECP_TASK_NUM * i) + (UINT8)AECP_TASK_EMC)], &Msg, 10U) == KAL_ERR_NONE) {
                            const AECP_AEM_s *AecpAem;
                            const AEM_CMD_RESP_s *Aem = &Msg;
                            AmbaMisra_TypeCast(&AecpAem, &Aem);
                            if (AecpAem->AecpPkt.AvtpduHeader.Message_type == AECP_MSG_AEM_COMMAND) { /* Process cmd */
                                if (AmbaWrap_memcpy(&Aecp.EmcSm[i].Command, &Msg, sizeof(AEM_CMD_RESP_s))!= 0U) { }
                                Aecp.EmcSm[i].DoCommand = TRUE;
                            }
                            if (AecpAem->AecpPkt.AvtpduHeader.Message_type == AECP_MSG_AEM_RESPONSE) { /* Process resp */
                                if (AmbaWrap_memcpy(&Aecp.EmcSm[i].RcvdResponse, &Msg, sizeof(AEM_CMD_RESP_s))!= 0U) { }
                                if (AecpAem->U == 1U) {
                                    Aecp.EmcSm[i].RcvdUnsolicitedResponse = TRUE;
                                } else {
                                    Aecp.EmcSm[i].RcvdNormalResponse = TRUE;
                                }
                            }
                        }
                    }
                    Aecp_EmcSm(&Aecp.EmcSm[i]);
                    if (Aecp.EmcSm[i].State > AECP_EMC_STATE_WAITING) {
                        Busy++;
                    }
                }
            }
            if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_EMC]) != KAL_ERR_NONE) {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake(AECP_TASK_EMC) fail!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Busy == 0U) {
            (void) AmbaKAL_TaskSleep(10U);
        }
    }
}

/**
 *  9.2.2.6.1 Address Access Entity State Machine
 *  @param [in] StateMachine The pointer of state machine
 */
static void Aecp_AaeSm(AECP_AAE_SM_s *StateMachine)
{
    switch (StateMachine->State) {
    case AECP_AAE_STATE_WAITING:
        if (StateMachine->RcvdAACommand == TRUE) {
            // TODO:
        }
        (void) AmbaKAL_TaskSleep(100U);
        StateMachine->State = AECP_AAE_STATE_WAITING; /* UCT to AECP_AAE_STATE_WAITING */
        break;
    case AECP_AAE_STATE_RECEIVED_COMMAND:
#if 0
        response = processCommand(rcvdCommand);
        txResponse(response)
#endif
        StateMachine->State = AECP_AAE_STATE_WAITING; /* UCT to AECP_AAE_STATE_WAITING */
        break;
    default:
        /* TODO */
        break;
    }
}

/**
 *  Aecp AAE task entry.
 *  @param [in] Info The argument attached to entry function
 *  @return 0 - OK, others - Error
 */
static void *Aecp_AaeTaskEntry(void *Info)
{
    (void)Info;
    for (;;) {
        UINT32 Busy = 0U;
        if (AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_AAE], AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
            UINT8 i;
            for (i = 0U; i < Aecp.MaxEntityNum; i++) {
                if (Aecp.AaeSm[i].MyEntityId != 0U) {
                    if (Aecp.AaeSm[i].State == AECP_AAE_STATE_WAITING) {
                        AEM_CMD_RESP_s Msg;
                        if (AmbaKAL_MsgQueueReceive(&Aecp.MsgQueue[(((UINT8)AECP_TASK_NUM * i) + (UINT8)AECP_TASK_AAE)], &Msg, 10U) == KAL_ERR_NONE) {
                            if (AmbaWrap_memcpy(&Aecp.AaeSm[i].RcvdCommand, &Msg, sizeof(AEM_CMD_RESP_s))!= 0U) { }
                            Aecp.AaeSm[i].RcvdAACommand = TRUE;
                        }
                    }
                    Aecp_AaeSm(&Aecp.AaeSm[i]);
                    if (Aecp.AaeSm[i].State > AECP_AAE_STATE_WAITING) {
                        Busy++;
                    }
                }
            }
            if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_AAE]) != KAL_ERR_NONE) {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake(AECP_TASK_AAE) fail!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Busy == 0U) {
            (void) AmbaKAL_TaskSleep(10U);
        }
    }
}

/**
 *  9.2.2.6.2 Address Access Controller State Machine
 *  @param [in] StateMachine The pointer of state machine
 */
static void Aecp_AacSm(AECP_AAC_SM_s *StateMachine)
{
    switch (StateMachine->State) {
    case AECP_AAC_STATE_WAITING:
        if (StateMachine->DoTerminate == TRUE) {
            // TODO:
        } else if (StateMachine->DoCommand == TRUE) {
            StateMachine->State = AECP_AAC_STATE_SEND_COMMAND;
        } else if (StateMachine->RcvdNormalResponse == TRUE) {
            const UINT8 *AecpAemPktPtr = StateMachine->RcvdResponse.Data;
            const AECP_AEM_s *AecpAemPkt = NULL;
            StateMachine->RcvdNormalResponse = FALSE;
            AmbaMisra_TypeCast(&AecpAemPkt, &AecpAemPktPtr);
            if (AecpAemPkt->AecpPkt.Aecpdu.Controller_entity_id == StateMachine->MyEntityId) {
                StateMachine->State = AECP_AAC_STATE_RECEIVED_RESPONSE;
            }
#if 0
        } else if (currentTime > inflight[x].timeout) {
            SM_Aecp_AAC.state = AECP_AAC_STATE_TIMEOUT;
            toBreak = 1u;
#endif
        } else {
            //nothing
        }
        StateMachine->State = AECP_AAC_STATE_WAITING; /* UCT to AECP_AAC_STATE_WAITING */
        break;
    case AECP_AAC_STATE_SEND_COMMAND:
#if 0
        txCommand(command)
        SM_Aecp_AAC.doCommand = FALSE
#endif
        StateMachine->State = AECP_AAC_STATE_WAITING; /* UCT to AECP_AAC_STATE_WAITING */
        break;
    case AECP_AAC_STATE_RECEIVED_RESPONSE:
#if 0
        processResponse(rcvdResponse)
#endif
        StateMachine->State = AECP_AAC_STATE_WAITING; /* UCT to AECP_AAC_STATE_WAITING */
        break;
    case AECP_AAC_STATE_TIMEOUT:
#if 0
        if (inflight[x].retried) {
            timeout(inflight[x])
        } else {
            txCommand(inflight[x].command)
        }
#endif
        StateMachine->State = AECP_AAC_STATE_WAITING; /* UCT to AECP_AAC_STATE_WAITING */
        break;
    default:
        /* TODO */
        break;
    }
}

/**
 *  Aecp AAC task entry.
 *  @param [in] Info The argument attached to entry function
 *  @return 0 - OK, others - Error
 */
static void *Aecp_AacTaskEntry(void *Info)
{
    (void)Info;
    for (;;) {
        UINT32 Busy = 0U;
        if (AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_AAC], AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
            UINT8 i;
            for (i = 0U; i < Aecp.MaxEntityNum; i++) {
                if (Aecp.AacSm[i].MyEntityId != 0U) {
                    if (Aecp.AacSm[i].State == AECP_AAC_STATE_WAITING) {
                        AEM_CMD_RESP_s Msg;
                        if (AmbaKAL_MsgQueueReceive(&Aecp.MsgQueue[(((UINT8)AECP_TASK_NUM * i) + (UINT8)AECP_TASK_AAC)], &Msg, 10U) == KAL_ERR_NONE) {
                            if (AmbaWrap_memcpy(&Aecp.AacSm[i].RcvdResponse, &Msg, sizeof(AEM_CMD_RESP_s))!= 0U) { }
                            Aecp.AacSm[i].RcvdNormalResponse = TRUE;
                        }
                    }
                    Aecp_AacSm(&Aecp.AacSm[i]);
                    if (Aecp.AacSm[i].State > AECP_AAC_STATE_WAITING) {
                        Busy++;
                    }
                }
            }
            if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_AAC]) != KAL_ERR_NONE) {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake(AECP_TASK_AAC) fail!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Busy == 0U) {
            (void) AmbaKAL_TaskSleep(10U);
        }
    }
}

/**
 *  Process vendor unique command
 *  @param [in] Cmd  The pointer of command data
 *  @param [in] Resp The pointer of response data
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_ProcessVuCmd(const AECP_VU_AECPDU_s *Cmd, const AECP_VU_AECPDU_s *Resp)
{
    UINT32 Rval = OK;
    if ((Cmd != NULL) && (Resp != NULL)) {
        UINT8 i;
        for (i = 0U; i < Aecp.MaxEntityNum; i++) {
            if (Aecp.VuSm[i].MyEntityId == Cmd->AecpPkt.AvtpduHeader.Target_entity_id) {
                const AECP_VU_CMD_s *VuCmd = &Cmd->VuCmd;
                if (Aecp.VuSm[i].VuCmdCb != NULL) {
                    ULONG VuCmdAddr;
                    AmbaMisra_TypeCast(&VuCmdAddr, &VuCmd);
                    Aecp.VuSm[i].VuCmdCb(VuCmdAddr);
                }
                break;
            }
        }
    }
    return Rval;
}

/**
 *  Transmit vendor unique response
 *  @param [in] Resp The pointer of response data
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_TxVuResponse(const AECP_VU_AECPDU_s *Resp)
{
    UINT32 Rval;
    UINT16 Size = (UINT16)sizeof(AECP_VU_AECPDU_s);
    UINT8 AcmpMulticastMac[6U] = ACMP_MULTICAST_MAC;
    AECP_AEM_s *AecpAemPkt;
    AmbaMisra_TypeCast(&AecpAemPkt, &Resp);
    AecpAemPkt->AecpPkt.AvtpduHeader.Message_type = AECP_MSG_VENDOR_UNIQUE_RESPONSE;
    AecpAemPkt->AecpPkt.AvtpduHeader.Status = AECP_STATUS_SUCCESS;
    Rval = Aecp_PrepareTxResp(AecpAemPkt);
    if (Rval == OK) {
        AecpAemPkt->AecpPkt.AvtpduHeader.Target_entity_id = AVDECC_HTONLL(AecpAemPkt->AecpPkt.AvtpduHeader.Target_entity_id);
        AecpAemPkt->AecpPkt.Aecpdu.Controller_entity_id = AVDECC_HTONLL(AecpAemPkt->AecpPkt.Aecpdu.Controller_entity_id);
        AecpAemPkt->AecpPkt.Aecpdu.Sequence_id = AVDECC_HTONS(AecpAemPkt->AecpPkt.Aecpdu.Sequence_id);
        Rval = Avdecc_NetTx(0U, AecpAemPkt, Size, AcmpMulticastMac);
        if (Rval != ETH_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaEnet_Tx() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  9.2.2.12.1 Vendor Unique Entity State Machine
 *  @param [in] StateMachine The pointer of state machine
 */
static void Aecp_VuSm(AECP_VU_SM_s *StateMachine)
{
    switch (StateMachine->State) {
    case AECP_VU_STATE_WAITING:
        if (StateMachine->RcvdVendorCommand == TRUE) {
            StateMachine->RcvdVendorCommand = FALSE;
            StateMachine->State = AECP_VU_STATE_RECEIVED_COMMAND;
        } else {
            //Nothing
        }
        break;
    case AECP_VU_STATE_RECEIVED_COMMAND:
        {
            UINT32 Rval;
            if (AmbaWrap_memcpy(&StateMachine->Resp, &StateMachine->RcvdCommand, sizeof(AECP_VU_AECPDU_s))!= 0U) { }
            Rval = Aecp_TxVuResponse(&StateMachine->Resp);
            if (Rval == OK) {
                Rval = Aecp_ProcessVuCmd(&StateMachine->RcvdCommand, &StateMachine->Resp);
                if (Rval != OK) {
                    AmbaPrint_PrintStr5("%s, Aecp_ProcessVuCmd() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_PrintStr5("%s, Aecp_TxVuResponse() failed!", __func__, NULL, NULL, NULL, NULL);
            }
            StateMachine->State = AECP_VU_STATE_WAITING; /* UCT to AECP_VU_STATE_WAITING */
        }
        break;
    default:
        /* TODO */
        break;
    }
}

/**
 *  Aecp VU task entry.
 *  @param [in] Info The argument attached to entry function
 *  @return 0 - OK, others - Error
 */
static void *Aecp_VuTaskEntry(void *Info)
{
    (void)Info;
    for (;;) {
        UINT32 Busy = 0U;
        if (AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_VU], AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
            UINT8 i;
            for (i = 0U; i < Aecp.MaxEntityNum; i++) {
                if (Aecp.VuSm[i].MyEntityId != 0U) {
                    if (Aecp.VuSm[i].State == AECP_VU_STATE_WAITING) {
                        const AECP_VU_AECPDU_s *Msg;
                        if (AmbaKAL_MsgQueueReceive(&Aecp.MsgQueue[(((UINT8)AECP_TASK_NUM * i) + (UINT8)AECP_TASK_VU)], &Msg, 10U) == KAL_ERR_NONE) {
                            if (AmbaWrap_memcpy(&Aecp.VuSm[i].RcvdCommand, Msg, sizeof(AECP_VU_AECPDU_s))!= 0U) { }
                            Aecp.VuSm[i].RcvdVendorCommand = TRUE;
                            Aecp_FreeVuPkt(Msg);
                        }
                    }
                    Aecp_VuSm(&Aecp.VuSm[i]);
                    if (Aecp.VuSm[i].State > AECP_VU_STATE_WAITING) {
                        Busy++;
                    }
                }
            }
            if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_VU]) != KAL_ERR_NONE) {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake(AECP_TASK_VU) fail!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Busy == 0U) {
            (void) AmbaKAL_TaskSleep(10U);
        }
    }
}

/**
 *  Transmit vendor unique command
 *  @param [in] Cmd The pointer of command data
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_TxVuCommand(const AECP_VU_AECPDU_s *Cmd)
{
    UINT32 Rval;
    UINT16 Size = (UINT16)sizeof(AECP_VU_AECPDU_s);
    UINT8 AcmpMulticastMac[6U] = ACMP_MULTICAST_MAC;
    AECP_VU_AECPDU_s *VuAecpdu;
    AmbaMisra_TypeCast(&VuAecpdu, &Cmd);
    VuAecpdu->AecpPkt.AvtpduHeader.Message_type = AECP_MSG_VENDOR_UNIQUE_COMMAND;
    Rval = Aecp_PrepareTxCmd(&VuAecpdu->AecpPkt);
    if (Rval == OK) {
        VuAecpdu->AecpPkt.AvtpduHeader.Target_entity_id = AVDECC_HTONLL(VuAecpdu->AecpPkt.AvtpduHeader.Target_entity_id);
        VuAecpdu->AecpPkt.Aecpdu.Controller_entity_id = AVDECC_HTONLL(VuAecpdu->AecpPkt.Aecpdu.Controller_entity_id);
        VuAecpdu->AecpPkt.Aecpdu.Sequence_id = AVDECC_HTONS(VuAecpdu->AecpPkt.Aecpdu.Sequence_id);
        Rval = Avdecc_NetTx(0U, VuAecpdu, Size, AcmpMulticastMac);
        if (Rval != ETH_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaEnet_Tx() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  9.2.2.12.2 Vendor Unique Controller State Machine
 *  @param [in] StateMachine The pointer of state machine
 */
static void Aecp_VucSm(AECP_VUC_SM_s *StateMachine)
{
    switch (StateMachine->State) {
    case AECP_VUC_STATE_WAITING:
        if (StateMachine->DoCommand == TRUE) {
            StateMachine->DoCommand = FALSE;
            StateMachine->State = AECP_VUC_STATE_SEND_COMMAND;
        } else if (StateMachine->RcvdNormalResponse == TRUE) {
            StateMachine->RcvdNormalResponse = FALSE;
            if (StateMachine->RcvdResponse.AecpPkt.Aecpdu.Controller_entity_id == StateMachine->MyEntityId) {
                StateMachine->State = AECP_VUC_STATE_RECEIVED_RESPONSE;
            }
#if 0
        } else if (currentTime > inflight[x].timeout) {
            SM_Aecp_VUC.state = AECP_VUC_STATE_TIMEOUT;
            toBreak = 1u;
#endif
        } else {
            //nothing
        }
        break;
    case AECP_VUC_STATE_SEND_COMMAND:
        {
            UINT32 Rval = Aecp_TxVuCommand(&StateMachine->Command);
            if (Rval != OK) {
                AmbaPrint_PrintStr5("%s, Aecp_TxVuCommand() failed!", __func__, NULL, NULL, NULL, NULL);
            }
            StateMachine->State = AECP_VUC_STATE_WAITING; /* UCT to AECP_VUC_STATE_WAITING */
        }
        break;
    case AECP_VUC_STATE_RECEIVED_RESPONSE:
#if 0
        processResponse(rcvdResponse)
#endif
        StateMachine->State = AECP_VUC_STATE_WAITING; /* UCT to AECP_VUC_STATE_WAITING */
        break;
    case AECP_VUC_STATE_TIMEOUT:
#if 0
        if (inflight[x].retried) {
            timeout(inflight[x])
        } else {
            txCommand(inflight[x].command)
        }
#endif
        StateMachine->State = AECP_VUC_STATE_WAITING; /* UCT to AECP_VUC_STATE_WAITING */
        break;
    default:
        /* TODO */
        break;
    }
}

/**
 *  Send vendor unique command
 *  @param [in] Cmd The pointer of command data
 *  @return 0 - OK, others - Error
 */
UINT32 Aecp_SendAecpVuCmd(const AECP_VU_AECPDU_s *Cmd)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Aecp.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_VUC], AMBA_KAL_WAIT_FOREVER);
        if (Rval == KAL_ERR_NONE) {
            UINT8 i;
            for (i = 0U; i < Aecp.MaxEntityNum; i++) {
                if (Aecp.VucSm[i].MyEntityId == Cmd->AecpPkt.Aecpdu.Controller_entity_id) {
                    AECP_VU_AECPDU_s *Msg = Aecp_AllocateVuPkt();
                    if (Msg != NULL) {
                        if (AmbaWrap_memcpy(Msg, Cmd, sizeof(AECP_VU_AECPDU_s))!= 0U) { }
                        Rval = AmbaKAL_MsgQueueSend(&Aecp.MsgQueue[(((UINT8)AECP_TASK_NUM * i) + (UINT8)AECP_TASK_VUC)], &Msg, AMBA_KAL_NO_WAIT);
                        if (Rval != KAL_ERR_NONE) {
                            AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueSend() failed!", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_PrintStr5("%s, Aecp_AllocateVuPkt() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                    break;
                }
            }
            if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_VUC]) != KAL_ERR_NONE) {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = ERR_ARG;
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
        }
        if (AmbaKAL_MutexGive(&Aecp.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Aecp VUC task entry.
 *  @param [in] Info The argument attached to entry function
 *  @return 0 - OK, others - Error
 */
static void *Aecp_VucTaskEntry(void *Info)
{
    (void)Info;
    for (;;) {
        UINT32 Busy = 0U;
        if (AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_VUC], AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
            UINT8 i;
            for (i = 0U; i < Aecp.MaxEntityNum; i++) {
                if (Aecp.VucSm[i].MyEntityId != 0U) {
                    if (Aecp.VucSm[i].State == AECP_VUC_STATE_WAITING) {
                        const AECP_VU_AECPDU_s *Msg;
                        if (AmbaKAL_MsgQueueReceive(&Aecp.MsgQueue[(((UINT8)AECP_TASK_NUM * i) + (UINT8)AECP_TASK_VUC)], &Msg, 10U) == KAL_ERR_NONE) {
                            if (Msg->AecpPkt.AvtpduHeader.Message_type == AECP_MSG_VENDOR_UNIQUE_COMMAND) {
                                if (AmbaWrap_memcpy(&Aecp.VucSm[i].Command, Msg, sizeof(AECP_VU_AECPDU_s))!= 0U) { }
                                Aecp.VucSm[i].DoCommand = TRUE;
                            }
                            if (Msg->AecpPkt.AvtpduHeader.Message_type == AECP_MSG_VENDOR_UNIQUE_RESPONSE) {
                                if (AmbaWrap_memcpy(&Aecp.VucSm[i].RcvdResponse, Msg, sizeof(AECP_VU_AECPDU_s))!= 0U) { }
                                Aecp.VucSm[i].RcvdNormalResponse = TRUE;
                            }
                            Aecp_FreeVuPkt(Msg);
                        }
                    }
                    Aecp_VucSm(&Aecp.VucSm[i]);
                    if (Aecp.VucSm[i].State > AECP_VUC_STATE_WAITING) {
                        Busy++;
                    }
                }
            }
            if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_VUC]) != KAL_ERR_NONE) {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake(AECP_TASK_VUC) fail!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Busy == 0U) {
            (void) AmbaKAL_TaskSleep(10U);
        }
    }
}

/**
 *  Send Aecp packet
 *  @param [in] MsgQId  The message queue id to send Aecp packet
 *  @param [in] AecpPkt The pointer of Aecp packet
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_SendAecpPacket(UINT32 MsgQId, const AECP_PACKET_s *AecpPkt)
{
    UINT32 Rval;
    if (((MsgQId % (UINT8)AECP_TASK_NUM) == (UINT8)AECP_TASK_VU) || ((MsgQId % (UINT8)AECP_TASK_NUM) == (UINT8)AECP_TASK_VUC)) {
        AECP_VU_AECPDU_s *Msg = Aecp_AllocateVuPkt();
        if (Msg != NULL) {
            if (AmbaWrap_memcpy(Msg, AecpPkt, sizeof(AECP_VU_AECPDU_s))!= 0U) { }
            Rval = AmbaKAL_MsgQueueSend(&Aecp.MsgQueue[MsgQId], &Msg, AMBA_KAL_NO_WAIT);
            if (Rval != KAL_ERR_NONE) {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueSend() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, Aecp_AllocateVuPkt() failed!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AEM_CMD_RESP_s Msg;
        if (AmbaWrap_memcpy(&Msg, AecpPkt, sizeof(AEM_CMD_RESP_s))!= 0U) { }
        Rval = AmbaKAL_MsgQueueSend(&Aecp.MsgQueue[MsgQId], &Msg, AMBA_KAL_NO_WAIT);
        if (Rval != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueSend() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  Convert values between host and network byte order
 *  @param [in] AecpPkt The Acmp packet
 */
static void Aecp_Ntoh(AECP_PACKET_s *AecpPkt)
{
    UINT32 *Ptr;
    const void *Vptr = &(AecpPkt->AvtpduHeader);
    AmbaMisra_TypeCast(&Ptr, &Vptr);
    *Ptr = AVDECC_NTOHL(*Ptr);
    AecpPkt->AvtpduHeader.Target_entity_id = AVDECC_NTOHLL(AecpPkt->AvtpduHeader.Target_entity_id);
    AecpPkt->Aecpdu.Controller_entity_id = AVDECC_NTOHLL(AecpPkt->Aecpdu.Controller_entity_id);
    AecpPkt->Aecpdu.Sequence_id = AVDECC_NTOHS(AecpPkt->Aecpdu.Sequence_id);
#if AECP_DBG
    {
        UINT32 Tmp1, Tmp2;
        AmbaPrint_PrintUInt5("Control_data_len: %d", AecpPkt->AvtpduHeader.Control_data_len, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("status: %d", AecpPkt->AvtpduHeader.Status, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Message_type: %d", AecpPkt->AvtpduHeader.Message_type, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Version: %d", AecpPkt->AvtpduHeader.Version, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Sv: %d", AecpPkt->AvtpduHeader.Sv, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Subtype: %d", AecpPkt->AvtpduHeader.Subtype, 0U, 0U, 0U, 0U);
        Tmp1 = ((AecpPkt->AvtpduHeader.Target_entity_id & 0xFFFFFFFF00000000U) >> 32U);
        Tmp2 = ((AecpPkt->AvtpduHeader.Target_entity_id & 0x00000000FFFFFFFFU));
        AmbaPrint_PrintUInt5("Target_entity_id: %X%X", Tmp1, Tmp2, 0U, 0U, 0U);
        Tmp1 = ((AecpPkt->Aecpdu.Controller_entity_id & 0xFFFFFFFF00000000U) >> 32U);
        Tmp2 = ((AecpPkt->Aecpdu.Controller_entity_id & 0x00000000FFFFFFFFU));
        AmbaPrint_PrintUInt5("Controller_entity_id: %X%X", Tmp1, Tmp2, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Sequence_id: %X", AecpPkt->Aecpdu.Sequence_id, 0U, 0U, 0U, 0U);
    }
#endif
}

/**
 *  Process Aecp packet
 *  @param [in] AecpPkt The pointer of Aecp packet
 *  @return 0 - OK, others - Error
 */
UINT32 Aecp_Process(AECP_PACKET_s *AecpPkt)
{
    UINT32 Rval;
    Aecp_Ntoh(AecpPkt);
    Rval = AmbaKAL_MutexTake(&Aecp.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        switch (AecpPkt->AvtpduHeader.Message_type) { /* Table 9.1 Message_type field */
        case AECP_MSG_AEM_COMMAND:
            {
                Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_EM], AMBA_KAL_WAIT_FOREVER);
                if (Rval == KAL_ERR_NONE) {
                    UINT8 i;
                    for (i = 0U; i < (UINT8)Aecp.MaxEntityNum; i++) {
                        if (Aecp.EmSm[i].MyEntityId == AecpPkt->AvtpduHeader.Target_entity_id) {
                            Rval = Aecp_SendAecpPacket(((UINT32)AECP_TASK_NUM * i), AecpPkt);
                            break;
                        }
                    }
                    if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_EM]) != KAL_ERR_NONE) {
                        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
                        Rval = ERR_ARG;
                    }
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
                }
            }
            break;
        case AECP_MSG_AEM_RESPONSE:
            {
                Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_EMC], AMBA_KAL_WAIT_FOREVER);
                if (Rval == KAL_ERR_NONE) {
                    UINT8 i;
                    for (i = 0U; i < (UINT8)Aecp.MaxEntityNum; i++) {
                        if (Aecp.EmcSm[i].MyEntityId == AecpPkt->Aecpdu.Controller_entity_id) {
                            Rval = Aecp_SendAecpPacket((UINT32)(((UINT32)AECP_TASK_NUM * i) + (UINT8)AECP_TASK_EMC), AecpPkt);
                            break;
                        }
                    }
                    if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_EMC]) != KAL_ERR_NONE) {
                        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
                        Rval = ERR_ARG;
                    }
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
                }
            }
            break;
        case AECP_MSG_ADDRESS_ACCESS_COMMAND:
        case AECP_MSG_ADDRESS_ACCESS_RESPONSE:
            {
                Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_AAC], AMBA_KAL_WAIT_FOREVER);
                if (Rval == KAL_ERR_NONE) {
                    UINT8 i;
                    for (i = 0U; i < (UINT8)Aecp.MaxEntityNum; i++) {
                        if (Aecp.AacSm[i].MyEntityId == AecpPkt->AvtpduHeader.Target_entity_id) {
                            Rval = Aecp_SendAecpPacket((UINT32)(((UINT32)AECP_TASK_NUM * i) + (UINT8)AECP_TASK_AAC), AecpPkt);
                            break;
                        }
                    }
                    if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_AAC]) != KAL_ERR_NONE) {
                        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
                        Rval = ERR_ARG;
                    }
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
                }
            }
            break;
        case AECP_MSG_AVC_COMMAND:
        case AECP_MSG_AVC_RESPONSE:
            break;
        case AECP_MSG_VENDOR_UNIQUE_COMMAND:
            {
                Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_VU], AMBA_KAL_WAIT_FOREVER);
                if (Rval == KAL_ERR_NONE) {
                    UINT8 i;
                    for (i = 0U; i < (UINT8)Aecp.MaxEntityNum; i++) {
                        if (Aecp.VuSm[i].MyEntityId == AecpPkt->AvtpduHeader.Target_entity_id) {
                            Rval = Aecp_SendAecpPacket((UINT32)(((UINT32)AECP_TASK_NUM * i) + (UINT8)AECP_TASK_VU), AecpPkt);
                            break;
                        }
                    }
                    if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_VU]) != KAL_ERR_NONE) {
                        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
                        Rval = ERR_ARG;
                    }
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
                }
            }
            break;
        case AECP_MSG_VENDOR_UNIQUE_RESPONSE:
            {
                Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_VUC], AMBA_KAL_WAIT_FOREVER);
                if (Rval == KAL_ERR_NONE) {
                    UINT8 i;
                    for (i = 0U; i < (UINT8)Aecp.MaxEntityNum; i++) {
                        if (Aecp.VucSm[i].MyEntityId == AecpPkt->Aecpdu.Controller_entity_id) {
                            Rval = Aecp_SendAecpPacket((UINT32)(((UINT32)AECP_TASK_NUM * i) + (UINT8)AECP_TASK_VUC), AecpPkt);
                            break;
                        }
                    }
                    if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_VUC]) != KAL_ERR_NONE) {
                        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
                        Rval = ERR_ARG;
                    }
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
                }
            }
            break;
        case AECP_MSG_HDCP_APM_COMMAND:
        case AECP_MSG_HDCP_APM_RESPONSE:
        case AECP_MSG_EXTENDED_COMMAND:
        case AECP_MSG_EXTENDED_RESPONSE:
        default:
            // TODO:
            break;
        }
        if (AmbaKAL_MutexGive(&Aecp.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Add entity to EM state machine
 *  @param [in] SmId       The state machine id to add
 *  @param [in] EntityInfo The pointer of entity information
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_AddEmEntity(UINT32 SmId, const AVDECC_ENTITY_INFO_s *EntityInfo)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_EM], AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        Aecp.EmSm[SmId].MyEntityId = EntityInfo->EntityDesc.Entity_id;
        if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_EM]) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Remove entity from EM state machine
 *  @param [in] EntityInfoId The entity information id to remove
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_RemoveEmEntity(UINT32 EntityInfoId)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_EM], AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        Aecp.EmSm[EntityInfoId].MyEntityId = 0U;
        if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_EM]) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Add entity to EMC state machine
 *  @param [in] SmId       The state machine id to add
 *  @param [in] EntityInfo The pointer of entity information
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_AddEmcEntity(UINT32 SmId, const AVDECC_ENTITY_INFO_s *EntityInfo)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_EMC], AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        Aecp.EmcSm[SmId].MyEntityId = EntityInfo->EntityDesc.Entity_id;
        if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_EMC]) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Remove entity from EMC state machine
 *  @param [in] EntityInfoId The entity information id to remove
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_RemoveEmcEntity(UINT32 EntityInfoId)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_EMC], AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        Aecp.EmcSm[EntityInfoId].MyEntityId = 0U;
        if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_EMC]) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Add entity to AAE state machine
 *  @param [in] SmId       The state machine id to add
 *  @param [in] EntityInfo The pointer of entity information
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_AddAaeEntity(UINT32 SmId, const AVDECC_ENTITY_INFO_s *EntityInfo)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_AAE], AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        Aecp.AaeSm[SmId].MyEntityId = EntityInfo->EntityDesc.Entity_id;
        if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_AAE]) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Remove entity from AAe state machine
 *  @param [in] EntityInfoId The entity information id to remove
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_RemoveAaeEntity(UINT32 EntityInfoId)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_AAE], AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        Aecp.AaeSm[EntityInfoId].MyEntityId = 0U;
        if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_AAE]) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Add entity to AAC state machine
 *  @param [in] SmId       The state machine id to add
 *  @param [in] EntityInfo The pointer of entity information
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_AddAacEntity(UINT32 SmId, const AVDECC_ENTITY_INFO_s *EntityInfo)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_AAC], AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        Aecp.AacSm[SmId].MyEntityId = EntityInfo->EntityDesc.Entity_id;
        if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_AAC]) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Remove entity from AAC state machine
 *  @param [in] EntityInfoId The entity information id to remove
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_RemoveAacEntity(UINT32 EntityInfoId)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_AAC], AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        Aecp.AacSm[EntityInfoId].MyEntityId = 0U;
        if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_AAC]) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Add entity to VU state machine
 *  @param [in] SmId       The state machine id to add
 *  @param [in] EntityInfo The pointer of entity information
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_AddVuEntity(UINT32 SmId, const AVDECC_ENTITY_INFO_s *EntityInfo)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_VU], AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        Aecp.VuSm[SmId].MyEntityId = EntityInfo->EntityDesc.Entity_id;
        Aecp.VuSm[SmId].VuCmdCb = EntityInfo->VuCmdCb;
        if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_VU]) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Remove entity from VU state machine
 *  @param [in] EntityInfoId The entity information id to remove
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_RemoveVuEntity(UINT32 EntityInfoId)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_VU], AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        Aecp.VuSm[EntityInfoId].MyEntityId = 0U;
        if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_VU]) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Add entity to VUC state machine
 *  @param [in] SmId       The state machine id to add
 *  @param [in] EntityInfo The pointer of entity information
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_AddVucEntity(UINT32 SmId, const AVDECC_ENTITY_INFO_s *EntityInfo)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_VUC], AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        Aecp.VucSm[SmId].MyEntityId = EntityInfo->EntityDesc.Entity_id;
        if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_VUC]) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Remove entity from VUC state machine
 *  @param [in] EntityInfoId The entity information id to remove
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_RemoveVucEntity(UINT32 EntityInfoId)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_VUC], AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        Aecp.VucSm[EntityInfoId].MyEntityId = 0U;
        if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_VUC]) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Add entity to Aecp
 *  @param [in] EntityInfo The pointer of entity information
 *  @return 0 - OK, others - Error
 */
UINT32 Aecp_AddEntity(const AVDECC_ENTITY_INFO_s *EntityInfo)
{
    static char MsgQueueName[AECP_TASK_NUM][32U] = {"AecpEmMsgQ", "AecpEmcMsgQ", "AecpAaeMsgQ", "AecpAacMsgQ", "AecpVuMsgQ", "AecpVucMsgQ"};
    UINT32 Rval = AmbaKAL_MutexTake(&Aecp.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT32 i = 0U;
        UINT32 Searched = 0U;
        Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_EM], AMBA_KAL_WAIT_FOREVER);
        if (Rval == KAL_ERR_NONE) {
            for (i = 0U; i < Aecp.MaxEntityNum; i++) {
                if ((Aecp.EmSm[i].MyEntityId == 0U) &&
                    (Aecp.EmSm[i].MyEntityId != EntityInfo->EntityDesc.Entity_id)){
                    Searched = 1U;
                    break;
                }
            }
            if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_EM]) != KAL_ERR_NONE) {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = ERR_ARG;
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Searched == 1U) {
            Rval = Aecp_AddEmEntity(i, EntityInfo);
            if (Rval == OK) {
                Rval = Aecp_AddEmcEntity(i, EntityInfo);
                if (Rval == OK) {
                    Rval = Aecp_AddAaeEntity(i, EntityInfo);
                    if (Rval == OK) {
                        Rval = Aecp_AddAacEntity(i, EntityInfo);
                        if (Rval == OK) {
                            Rval = Aecp_AddVuEntity(i, EntityInfo);
                            if (Rval == OK) {
                                Rval = Aecp_AddVucEntity(i, EntityInfo);
                                if (Rval == OK) {
                                    UINT32 j;
                                    AMBA_KAL_MSG_QUEUE_t *MsgQ;
                                    ULONG MsgQAddr;
                                    UINT8 *CmdMsgQBuffer;
                                    ULONG CmdMsgQBufferAddr;
                                    UINT8 *VuCmdQBuffer;
                                    ULONG VuCmdQBufferAddr;
                                    AmbaMisra_TypeCast(&MsgQ, &Aecp.MsgQueuePtr);
                                    AmbaMisra_TypeCast(&MsgQAddr, &MsgQ);
                                    AmbaMisra_TypeCast(&CmdMsgQBuffer, &Aecp.CmdMsgQBufferPtr);
                                    AmbaMisra_TypeCast(&CmdMsgQBufferAddr, &CmdMsgQBuffer);
                                    AmbaMisra_TypeCast(&VuCmdQBuffer, &Aecp.VuCmdQBufferPtr);
                                    AmbaMisra_TypeCast(&VuCmdQBufferAddr, &VuCmdQBuffer);
                                    for (j = (i * (UINT32)AECP_TASK_NUM); j < ((i * (UINT32)AECP_TASK_NUM) + (UINT32)AECP_TASK_NUM); j++) {
                                        if ((j != ((i * (UINT32)AECP_TASK_NUM) + (UINT32)AECP_TASK_VU)) && (j != ((i * (UINT32)AECP_TASK_NUM) + (UINT32)AECP_TASK_VUC))) {
                                            Rval = AmbaKAL_MsgQueueCreate(MsgQ, MsgQueueName[(j % (UINT8)AECP_TASK_NUM)],
                                                                             sizeof(AEM_CMD_RESP_s), CmdMsgQBuffer,
                                                                             (Aecp.MsgNum * sizeof(AEM_CMD_RESP_s)));
                                            if (Rval == KAL_ERR_NONE) {
                                                MsgQAddr += sizeof(AMBA_KAL_MSG_QUEUE_t);
                                                AmbaMisra_TypeCast(&MsgQ, &MsgQAddr);
                                                CmdMsgQBufferAddr += (Aecp.MsgNum * sizeof(AEM_CMD_RESP_s));
                                                AmbaMisra_TypeCast(&CmdMsgQBuffer, &CmdMsgQBufferAddr);
                                            } else {
                                                AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueCreate(AEM_CMD_RESP_s) failed!", __func__, NULL, NULL, NULL, NULL);
                                            }
                                        } else {
                                            Rval = AmbaKAL_MsgQueueCreate(MsgQ, MsgQueueName[(j % (UINT32)AECP_TASK_NUM)],
                                                                             sizeof(AECP_VU_PKT_BUFFER_s *), VuCmdQBuffer,
                                                                             (Aecp.MsgNum * sizeof(AECP_VU_PKT_BUFFER_s *)));
                                            if (Rval == KAL_ERR_NONE) {
                                                MsgQAddr += sizeof(AMBA_KAL_MSG_QUEUE_t);
                                                AmbaMisra_TypeCast(&MsgQ, &MsgQAddr);
                                                VuCmdQBufferAddr += (Aecp.MsgNum * sizeof(AECP_VU_PKT_BUFFER_s *));
                                                AmbaMisra_TypeCast(&VuCmdQBuffer, &VuCmdQBufferAddr);
                                            } else {
                                                AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueCreate(AECP_VU_PKT_BUFFER_s) failed!", __func__, NULL, NULL, NULL, NULL);
                                            }
                                        }
                                    }
                                    AmbaMisra_TypeCast(&Aecp.MsgQueuePtr, &MsgQAddr);
                                    AmbaMisra_TypeCast(&Aecp.CmdMsgQBufferPtr, &CmdMsgQBufferAddr);
                                    AmbaMisra_TypeCast(&Aecp.VuCmdQBufferPtr, &VuCmdQBufferAddr);
                                }
                            }
                        }
                    }
                }
            }
        }
        if (AmbaKAL_MutexGive(&Aecp.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Remove entity from Aecp
 *  @param [in] EntityInfo The pointer of entity information to remove
 *  @return 0 - OK, others - Error
 */
UINT32 Aecp_RemoveEntity(const AVDECC_ENTITY_INFO_s *EntityInfo)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Aecp.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT32 i = 0U;
        UINT32 Searched = 0U;
        Rval = AmbaKAL_MutexTake(&Aecp.TaskMutex[AECP_TASK_EM], AMBA_KAL_WAIT_FOREVER);
        if (Rval == KAL_ERR_NONE) {
            for (i = 0U; i < Aecp.MaxEntityNum; i++) {
                if ((Aecp.EmSm[i].MyEntityId != 0U) &&
                    (Aecp.EmSm[i].MyEntityId == EntityInfo->EntityDesc.Entity_id)){
                    Searched = 1U;
                    break;
                }
            }
            if (AmbaKAL_MutexGive(&Aecp.TaskMutex[AECP_TASK_EM]) != KAL_ERR_NONE) {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = ERR_ARG;
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Searched == 1U) {
            Rval = Aecp_RemoveEmEntity(i);
            if (Rval == OK) {
                Rval = Aecp_RemoveEmcEntity(i);
                if (Rval == OK) {
                    Rval = Aecp_RemoveAaeEntity(i);
                    if (Rval == OK) {
                        Rval = Aecp_RemoveAacEntity(i);
                        if (Rval == OK) {
                            Rval = Aecp_RemoveVuEntity(i);
                            if (Rval == OK) {
                                Rval = Aecp_RemoveVucEntity(i);
                                if (Rval == OK) {
                                    UINT32 j;
                                    for (j = (i * (UINT32)AECP_TASK_NUM); j < ((i * (UINT32)AECP_TASK_NUM) + (UINT32)AECP_TASK_NUM); j++) {
                                        Rval = AmbaKAL_MsgQueueDelete(&Aecp.MsgQueue[j]);
                                        if (Rval != KAL_ERR_NONE) {
                                            AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueDelete() failed!", __func__, NULL, NULL, NULL, NULL);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (AmbaKAL_MutexGive(&Aecp.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Get the required buffer size for initializing the Aecp module.
 *  @param [in]  Cfg  The configuration of the Aecp module
 *  @param [out] Size The required buffer size
 *  @return 0 - OK, others - Error
 */
UINT32 Aecp_GetRequiredBufferSize(const AVDECC_AECP_INIT_CFG_s *Cfg, UINT32 *Size)
{
    UINT32 Rval = OK;
    UINT32 BufferSize = 0U;
    BufferSize += (GetAlignedValU32(Cfg->TaskInfo.StackSize, (UINT8)AMBA_CACHE_LINE_SIZE) * (UINT8)AECP_TASK_NUM);    /* Task stack */
    BufferSize += (Cfg->MaxEntityNum * (UINT8)AECP_TASK_NUM * sizeof(AMBA_KAL_MSG_QUEUE_t)); /* MsgQ */
    BufferSize += (Cfg->MaxEntityNum * (UINT8)AECP_TASK_NUM * Cfg->MsgNum * sizeof(AEM_CMD_RESP_s)); /* MsgQ buffer */
    BufferSize += (Cfg->MaxEntityNum * 2U * Cfg->MsgNum * sizeof(AECP_VU_PKT_BUFFER_s *));  /* Vu CmdQ buffer */
    BufferSize += (Cfg->InflightCmdNum * sizeof(AECP_INFLIGHT_CMD_s)); /* Inflight cmd */
    BufferSize += (Cfg->VuCmdNum * sizeof(AECP_VU_PKT_BUFFER_s));   /* Vu cmd */
    BufferSize += (Cfg->MaxEntityNum * sizeof(AECP_EM_SM_s)); /* EM */
    BufferSize += (Cfg->MaxEntityNum * sizeof(AECP_EMC_SM_s)); /* EMC */
    BufferSize += (Cfg->MaxEntityNum * sizeof(AECP_AAE_SM_s)); /* AAE */
    BufferSize += (Cfg->MaxEntityNum * sizeof(AECP_AAC_SM_s)); /* AAC */
    BufferSize += (Cfg->MaxEntityNum * sizeof(AECP_VU_SM_s)); /* VU */
    BufferSize += (Cfg->MaxEntityNum * sizeof(AECP_VUC_SM_s)); /* VUC */
    BufferSize = GetAlignedValU32(BufferSize, AMBA_CACHE_LINE_SIZE);
    *Size = BufferSize;
    return Rval;
}

/**
 *  Get the default configuration for initializing the Aecp module.
 *  @param [out] Cfg The returned configuration of the Aecp module
 *  @return 0 - OK, others - Error
 */
UINT32 Aecp_GetDefaultInitCfg(AVDECC_AECP_INIT_CFG_s *Cfg)
{
    UINT32 Rval;
    if (AmbaWrap_memset(Cfg, 0, sizeof(AVDECC_AECP_INIT_CFG_s))!= 0U) { }
    Cfg->MaxEntityNum = AVDECC_RX_ENTITY_INFO_MAX_NUM_DEFAULT;
    Cfg->InflightCmdNum = AECP_MAX_INFLIGHT_CMD_NUM_DEFAULT;
    Cfg->VuCmdNum = AECP_VU_PKT_MAX_NUM_DEFAULT;
    Cfg->TaskInfo.Priority = AECP_TASK_PRIORITY_DEFAULT;
    Cfg->TaskInfo.StackSize = AECP_TASK_STACK_SIZE_DEFAULT;
    Cfg->MsgNum = AECP_MSGQ_MAX_NUM_DEFAULT;
    Rval = Aecp_GetRequiredBufferSize(Cfg, &Cfg->BufferSize);
    return Rval;
}

/**
 *  Implement initialize the Aecp module.
 *  @param [in] Cfg The configuration used to initialize the Aecp module
 *  @return 0 - OK, others - Error
 */
static UINT32 Aecp_InitImpl(const AVDECC_AECP_INIT_CFG_s *Cfg)
{
    static char MutexName[AECP_TASK_NUM][32U] = {"AecpEmMutex", "AecpEmcMutex", "AecpAaeMutex", "AecpAacMutex", "AecpVuMutex", "AecpVucMutex"};
    static char TaskName[AECP_TASK_NUM][32U] = {"AvdeccAecpEm", "AvdeccAecpEmc", "AvdeccAecpAae", "AvdeccAecpAac", "AvdeccAecpVu", "AvdeccAecpVuc"};
    static void* (*TaskEntry[AECP_TASK_NUM])(void *Info) = {Aecp_EmTaskEntry, Aecp_EmcTaskEntry,
                                                            Aecp_AaeTaskEntry, Aecp_AacTaskEntry,
                                                            Aecp_VuTaskEntry, Aecp_VucTaskEntry};
    UINT32 Rval = KAL_ERR_NONE;
    UINT8 i;
    UINT8 *Buffer = Aecp.TaskStack;
    ULONG Addr;
    AmbaMisra_TypeCast(&Addr, &Buffer);
    for (i = 0U; i < (UINT8)AECP_TASK_NUM; i++) {
        if (Rval == KAL_ERR_NONE) {
            Rval = AmbaKAL_MutexCreate(&Aecp.TaskMutex[i], MutexName[i]);
            if (Rval == KAL_ERR_NONE) {
                Rval = AmbaKAL_TaskCreate(&Aecp.Task[i], TaskName[i], Cfg->TaskInfo.Priority,
                                         TaskEntry[i], NULL, Buffer,
                                         Cfg->TaskInfo.StackSize, AMBA_KAL_DONT_START);
                if (Rval == KAL_ERR_NONE) {
                    Addr += GetAlignedValU32(Cfg->TaskInfo.StackSize, AMBA_CACHE_LINE_SIZE);
                    AmbaMisra_TypeCast(&Buffer, &Addr);
                    Rval = AmbaKAL_TaskSetSmpAffinity(&Aecp.Task[i], AVDECC_CORE);
                    if (Rval == KAL_ERR_NONE) {
                        Rval = AmbaKAL_TaskResume(&Aecp.Task[i]);
                        if (Rval != KAL_ERR_NONE) {
                            AmbaPrint_PrintStr5("%s, AmbaKAL_TaskResume() failed!", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_PrintStr5("%s, AmbaKAL_TaskSetSmpAffinity() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_TaskCreate() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueCreate() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            break;
        }
    }
    return Rval;
}

/**
 *  Initialize the Aecp module.
 *  @param [in] Cfg The configuration used to initialize the Aecp module
 *  @return 0 - OK, others - Error
 */
UINT32 Aecp_Init(const AVDECC_AECP_INIT_CFG_s *Cfg)
{
    static char MutexName[] = {"AecpMutex"};
    UINT32 Rval;
    UINT32 BufferSize;
    Rval = Aecp_GetRequiredBufferSize(Cfg, &BufferSize);
    if (BufferSize == Cfg->BufferSize) {
        ULONG Addr;
        AmbaMisra_TypeCast(&Addr, &Cfg->Buffer);
        if ((Addr % AMBA_CACHE_LINE_SIZE) == 0U) {
            UINT8 *Buffer = Cfg->Buffer;
            Aecp.MaxEntityNum = Cfg->MaxEntityNum;
            Aecp.InflightCmdNum = Cfg->InflightCmdNum;
            Aecp.VuCmdNum = Cfg->VuCmdNum;
            Aecp.MsgNum = Cfg->MsgNum;
            Aecp.TaskStack = Buffer; /* Task Stack */
            Addr += (GetAlignedValU32(Cfg->TaskInfo.StackSize, (UINT8)AMBA_CACHE_LINE_SIZE) * (UINT8)AECP_TASK_NUM);
            AmbaMisra_TypeCast(&Aecp.MsgQueue, &Addr); /* MsgQ */
            Aecp.MsgQueuePtr = Aecp.MsgQueue;
            Addr += (Aecp.MaxEntityNum * (UINT8)AECP_TASK_NUM * sizeof(AMBA_KAL_MSG_QUEUE_t));
            AmbaMisra_TypeCast(&Aecp.CmdMsgQBuffer, &Addr); /* MsgQ buffer */
            Aecp.CmdMsgQBufferPtr = Aecp.CmdMsgQBuffer;
            Addr += (Aecp.MaxEntityNum * (UINT8)AECP_TASK_NUM * Aecp.MsgNum * sizeof(AEM_CMD_RESP_s));
            AmbaMisra_TypeCast(&Aecp.VuCmdQBuffer, &Addr); /* Vu CmdQ buffer */
            Aecp.VuCmdQBufferPtr = Aecp.VuCmdQBuffer;
            Addr += (Aecp.MaxEntityNum * 2U * Aecp.MsgNum * sizeof(AECP_VU_PKT_BUFFER_s *));
            AmbaMisra_TypeCast(&Aecp.InflightBuffer, &Addr); /* Inflight */
            Addr += (Aecp.InflightCmdNum * sizeof(AECP_INFLIGHT_CMD_s));
            AmbaMisra_TypeCast(&Aecp.VuPktBuffer, &Addr); /* Vu Cmd buffer */
            Addr += (Aecp.VuCmdNum * sizeof(AECP_VU_PKT_BUFFER_s));
            AmbaMisra_TypeCast(&Aecp.EmSm, &Addr); /* EM */
            Addr += (Aecp.MaxEntityNum * sizeof(AECP_EM_SM_s));
            AmbaMisra_TypeCast(&Aecp.EmcSm, &Addr); /* EMC */
            Addr += (Aecp.MaxEntityNum * sizeof(AECP_EMC_SM_s));
            AmbaMisra_TypeCast(&Aecp.AaeSm, &Addr); /* AAE */
            Addr += (Aecp.MaxEntityNum * sizeof(AECP_AAE_SM_s));
            AmbaMisra_TypeCast(&Aecp.AacSm, &Addr); /* AAC */
            Addr += (Aecp.MaxEntityNum * sizeof(AECP_AAC_SM_s));
            AmbaMisra_TypeCast(&Aecp.VuSm, &Addr); /* VU */
            Addr += (Aecp.MaxEntityNum * sizeof(AECP_VU_SM_s));
            AmbaMisra_TypeCast(&Aecp.VucSm, &Addr); /* VUC */
            Rval = AmbaKAL_MutexCreate(&Aecp.Mutex, MutexName);
            if (Rval == KAL_ERR_NONE) {
                Rval = Aecp_InitImpl(Cfg);
                if (Rval == OK) {
                    Rval = Aecp_InflightCmdInit();
                    if (Rval == OK) {
                        Rval = Aecp_VuPktBufferInit();
                    }
                }
            } else {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexCreate() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, Buffer address not alignd!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("%s, BufferSize != Cfg->BufferSize", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

