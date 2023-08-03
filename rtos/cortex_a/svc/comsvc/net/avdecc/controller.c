/**
 * @file controller.c
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

#define CONTROLLER_TASK_PRIORITY_DEFAULT          (200U)
#define CONTROLLER_TASK_STACK_SIZE_DEFAULT        (0x2000U)

#define CONTROLLER_STATE_WAITING   (0U)
#define CONTROLLER_STATE_COMMAND   (1U)
#define CONTROLLER_STATE_TIMEOUT   (2U)
#define CONTROLLER_STATE_RESPONSE  (3U)

typedef struct {
    AMBA_KAL_MUTEX_t Mutex;
    AMBA_KAL_TASK_t Task;
    UINT8 *TaskStack;
    UINT32 MsgNum;
    AMBA_KAL_MSG_QUEUE_t MsgQueue;
    ADP_PACKET_s *MsgQBuffer;
    ACMP_CONTROLLER_SM_s *StateMachine;
    UINT32 MaxEntityNum;
} AVDECC_CONTROLLER_s;

static AVDECC_CONTROLLER_s Ctrller = {0};

/* 8.2.2.4.2.1 txCommand(messageType, command, retry) */
static UINT32 Ctrller_TxCommand(UINT8 MsgType, const ACMP_COMMAND_RESPONSE_s *Cmd, UINT32 Retry)
{
    UINT32 Rval;
    UINT16 Size = (UINT16)sizeof(ACMP_PACKET_s);
    UINT8 AcmpMulticastMac[6U] = ACMP_MULTICAST_MAC;
    ACMP_PACKET_s AcmpPkt;
    const ACMP_PACKET_s *Ptr;
    const UINT8 *Buffer;
    AmbaMisra_TouchUnused(&Retry);
    Ptr = &AcmpPkt;
    AmbaMisra_TypeCast(&Buffer, &Ptr);
    Rval = Acmp_PrepareTxCmd(MsgType, &AcmpPkt, ACMP_STATUS_SUCCESS);
    if (Rval == OK) {
        AcmpPkt.AvtpduHeader.Stream_id = 0U;
        AcmpPkt.Acmpdu.Controller_entity_id = AVDECC_HTONLL(Cmd->Controller_entity_id);
        AcmpPkt.Acmpdu.Talker_entity_id = AVDECC_HTONLL(Cmd->Talker_entity_id);
        AcmpPkt.Acmpdu.Listener_entity_id = AVDECC_HTONLL(Cmd->Listener_entity_id);
        AcmpPkt.Acmpdu.Talker_unique_id = AVDECC_HTONS(Cmd->Talker_unique_id);
        AcmpPkt.Acmpdu.Listener_unique_id = AVDECC_HTONS(Cmd->Listener_unique_id);
        if (AmbaWrap_memcpy(AcmpPkt.Acmpdu.Stream_dest_mac, Cmd->Stream_dest_mac, 6U)!= 0U) { }
        AcmpPkt.Acmpdu.Connection_count = AVDECC_HTONS(Cmd->Connection_count);
        AcmpPkt.Acmpdu.Sequence_id = AVDECC_HTONS(Cmd->Sequence_id);
        AcmpPkt.Acmpdu.Flags = AVDECC_HTONS(Cmd->Flags);
        AcmpPkt.Acmpdu.Stream_vlan_id = AVDECC_HTONS(Cmd->Stream_vlan_id);
        Rval = Avdecc_NetTx(0U, Buffer, Size, AcmpMulticastMac);
        if (Rval != ETH_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaEnet_Tx() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/* 8.2.2.4.2.2 cancelTimeout(commandResponse) */
static UINT32 Ctrller_CancelTimeout(const ACMP_COMMAND_RESPONSE_s *CmdResp)
{
    UINT32 Rval = OK;
    AmbaMisra_TouchUnused(CmdResp);
    return Rval;
}

/* 8.2.2.4.2.3 removeInflight(commandResponse) */
static ACMP_INFLIGHT_CMD_s *Ctrller_RemoveInflight(ACMP_INFLIGHT_CMD_s *Inflight, UINT16 SequenceId)
{
    ACMP_INFLIGHT_CMD_s *Head;
    Head = Acmp_RemoveInflightCmd(Inflight, SequenceId);
    return Head;
}

/* 8.2.2.4.2.4 processResponse(commandResponse) */
static UINT32 Ctrller_ProcessResponse(const ACMP_COMMAND_RESPONSE_s *CmdResp)
{
    UINT32 Rval = OK;
    if (CmdResp->Message_type == ACMP_MSG_CONNECT_RX_RESPONSE) {
        if (CmdResp->Status == ACMP_STATUS_SUCCESS) {
            Rval = Avdecc_SendEvent(AVDECC_EVENT_CONNECT_SUCCESS, CmdResp->Listener_entity_id);
        } else {
            Rval = Avdecc_SendEvent(AVDECC_EVENT_CONNECT_ERROR, CmdResp->Listener_entity_id);
        }
    }
    if (CmdResp->Message_type == ACMP_MSG_DISCONNECT_RX_RESPONSE) {
        if (CmdResp->Status == ACMP_STATUS_SUCCESS) {
            Rval = Avdecc_SendEvent(AVDECC_EVENT_DISCONNECT_SUCCESS, CmdResp->Listener_entity_id);
        } else {
            Rval = Avdecc_SendEvent(AVDECC_EVENT_DISCONNECT_ERROR, CmdResp->Listener_entity_id);
        }
    }
    return Rval;
}

/* 8.2.2.4.2.5 makeCommand(params) */
static UINT32 Ctrller_MakeCommand(UINT64 ControllerEntityId, UINT16 SequenceId, const ACMP_COMMAND_RESPONSE_s *Params, ACMP_COMMAND_RESPONSE_s *Cmd)
{
    UINT32 Rval = OK;
    Cmd->Message_type = Params->Message_type;
    Cmd->Controller_entity_id = ControllerEntityId;
    Cmd->Talker_entity_id = Params->Talker_entity_id;
    Cmd->Listener_entity_id = Params->Listener_entity_id;
    Cmd->Talker_unique_id = Params->Talker_unique_id;
    Cmd->Listener_unique_id = Params->Listener_unique_id;
    if (AmbaWrap_memcpy(Cmd->Stream_dest_mac, Params->Stream_dest_mac, 6U)!= 0U) { }
    Cmd->Connection_count = Params->Connection_count;
    Cmd->Sequence_id = SequenceId;
    Cmd->Flags = ACMP_FLG_SAVED_STATE;
    Cmd->Stream_vlan_id = Avdecc_GetStreamVlanId();
    return Rval;
}

static UINT32 Ctrller_CheckInflight(const ACMP_COMMAND_RESPONSE_s *Resp, const ACMP_INFLIGHT_CMD_s *Head)
{
    UINT32 Exist = 0U;
    const ACMP_INFLIGHT_CMD_s *InflightCmd = Head;
    while (InflightCmd != NULL) {
        if ((InflightCmd->Original_sequence_id == Resp->Sequence_id) &&
            ((InflightCmd->Cmd.Message_type + 1U) == Resp->Message_type)) {
            Exist = 1U;
            break;
        }
        InflightCmd = InflightCmd->Next;
    }
    return Exist;
}

static void Ctrller_AcmpSm(ACMP_CONTROLLER_SM_s *StateMachine)
{
    ACMP_INFLIGHT_CMD_s *TimeoutCmd = Acmp_InflightCmdTimeout(StateMachine->Inflight);
    if (TimeoutCmd != NULL) {
        StateMachine->State = CONTROLLER_STATE_TIMEOUT;
    }
    switch (StateMachine->State) {
    case CONTROLLER_STATE_WAITING:
        if (StateMachine->DoTerminate == TRUE) {
            //TODO:
        } else if (StateMachine->DoCommand == TRUE) {
            StateMachine->DoCommand = FALSE;
            StateMachine->State = CONTROLLER_STATE_COMMAND;
        } else if ((StateMachine->RcvdResponse == TRUE) &&
                   (StateMachine->RcvdCmdResp.Controller_entity_id == StateMachine->My_id) &&
                   (Ctrller_CheckInflight(&StateMachine->RcvdCmdResp, StateMachine->Inflight) == 1U)) {
            StateMachine->RcvdResponse = FALSE;
            StateMachine->State = CONTROLLER_STATE_RESPONSE;
        } else {
            //nothing
        }
        break;
    case CONTROLLER_STATE_COMMAND:
        /* command = makeCommand(commandParams);
         * txCommand(command.Message_type, command, FALSE);
         */
        {
            UINT32 Rval;
            ACMP_COMMAND_RESPONSE_s AcmpCmd = {0};
            Rval = Ctrller_MakeCommand(StateMachine->My_id, StateMachine->Sequence_id, &StateMachine->CmdParams, &AcmpCmd);
            if (Rval == OK) {
                StateMachine->Sequence_id++;
                Rval = Ctrller_TxCommand(AcmpCmd.Message_type, &AcmpCmd, FALSE);
                if (Rval == OK) {
                    StateMachine->Inflight = Acmp_AddInflightCmd(StateMachine->Inflight, AcmpCmd, Acmp_GetCmdTimeoutTime(StateMachine->CmdParams.Message_type));
                }
            }
            StateMachine->State = CONTROLLER_STATE_WAITING; /* UCT to AVDECC_CONTROLLER_ACMP_STATE_WAITING */
        }
        break;
    case CONTROLLER_STATE_TIMEOUT:
        /*
         *  if (inflight[x].retried) {
         *      removeInflight(inflight[x].command);
         *      // timeout
         *  } else {
         *      txCommand(inflight[x].command.Message_type, inflight[x].command, TRUE);
         *  }
         */
        if (TimeoutCmd != NULL) {
            if (TimeoutCmd->Retried == TRUE) {
                StateMachine->Inflight = Ctrller_RemoveInflight(StateMachine->Inflight, TimeoutCmd->Cmd.Sequence_id);
                if (Avdecc_SendEvent(AVDECC_EVENT_CMD_TIMEOUT, StateMachine->My_id) != OK) {
                    AmbaPrint_PrintStr5("%s, Avdecc_SendEvent(AVDECC_EVENT_CMD_TIMEOUT) failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                UINT32 Rval = Ctrller_TxCommand(TimeoutCmd->Cmd.Message_type, &TimeoutCmd->Cmd, TRUE);
                if (Rval == OK) {
                    UINT32 CurrentTime;
                    TimeoutCmd->Retried = TRUE;
                    (void)AmbaKAL_GetSysTickCount(&CurrentTime);
                    TimeoutCmd->Timeout = CurrentTime + Acmp_GetCmdTimeoutTime(TimeoutCmd->Cmd.Message_type);
                } else {
                    AmbaPrint_PrintStr5("%s, Ctrller_TxCommand() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
        StateMachine->State = CONTROLLER_STATE_WAITING; /* UCT to AVDECC_CONTROLLER_ACMP_STATE_WAITING */
        break;
    case CONTROLLER_STATE_RESPONSE:
        {
            UINT32 Rval = Ctrller_CancelTimeout(&StateMachine->RcvdCmdResp);
            if (Rval == OK) {
                Rval = Ctrller_ProcessResponse(&StateMachine->RcvdCmdResp);
                if (Rval == OK) {
                    StateMachine->Inflight = Ctrller_RemoveInflight(StateMachine->Inflight, StateMachine->RcvdCmdResp.Sequence_id);
                } else {
                    AmbaPrint_PrintStr5("%s, Ctrller_ProcessResponse() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_PrintStr5("%s, Ctrller_CancelTimeout() failed!", __func__, NULL, NULL, NULL, NULL);
            }
            StateMachine->State = CONTROLLER_STATE_WAITING; /* UCT to AVDECC_CONTROLLER_ACMP_STATE_WAITING */
        }
        break;
    default:
        /* TODO */
        break;
    }
}

UINT32 Ctrller_SendAcmpPacket(const ACMP_PACKET_s *AcmpPkt)
{
    UINT32 Rval;
    ACMP_PACKET_s Msg;
    if (AmbaWrap_memcpy(&Msg, AcmpPkt, sizeof(ACMP_PACKET_s))!= 0U) { }
    Rval = AmbaKAL_MsgQueueSend(&Ctrller.MsgQueue, &Msg, AMBA_KAL_NO_WAIT);
    if (Rval != KAL_ERR_NONE) {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueSend() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static void *Ctrller_TaskEntry(void *Info)
{
    UINT8 i;
    ACMP_PACKET_s Msg;
    (void)Info;
    AmbaPrint_PrintStr5("%s, start", __func__, NULL, NULL, NULL, NULL);
    for (;;) {
        UINT32 Busy = 0U;
        if (AmbaKAL_MutexTake(&Ctrller.Mutex, AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
            for (i = 0U; i < Ctrller.MaxEntityNum; i++) {
                if (Ctrller.StateMachine[i].My_id != 0U) {
                    if (Ctrller.StateMachine[i].State == CONTROLLER_STATE_WAITING) {
                        if (AmbaKAL_MsgQueueReceive(&Ctrller.MsgQueue, &Msg, 10U) == KAL_ERR_NONE) {
                            switch (Msg.AvtpduHeader.Message_type) {
                            case ACMP_MSG_CONNECT_TX_COMMAND :
                            case ACMP_MSG_DISCONNECT_TX_COMMAND :
                            case ACMP_MSG_GET_TX_STATE_COMMAND :
                            case ACMP_MSG_CONNECT_RX_COMMAND :
                            case ACMP_MSG_DISCONNECT_RX_COMMAND :
                            case ACMP_MSG_GET_RX_STATE_COMMAND :
                            case ACMP_MSG_GET_TX_CONNECTION_COMMAND :
                                {
                                    Ctrller.StateMachine[i].CmdParams.Message_type = Msg.AvtpduHeader.Message_type;
                                    Ctrller.StateMachine[i].CmdParams.Talker_entity_id = Msg.Acmpdu.Talker_entity_id;
                                    Ctrller.StateMachine[i].CmdParams.Listener_entity_id = Msg.Acmpdu.Listener_entity_id;
                                    Ctrller.StateMachine[i].CmdParams.Talker_unique_id = Msg.Acmpdu.Talker_unique_id;
                                    Ctrller.StateMachine[i].CmdParams.Listener_unique_id = Msg.Acmpdu.Listener_unique_id;
                                    Ctrller.StateMachine[i].CmdParams.Connection_count = Msg.Acmpdu.Connection_count;
                                    if (AmbaWrap_memcpy(Ctrller.StateMachine[i].CmdParams.Stream_dest_mac, Msg.Acmpdu.Stream_dest_mac, 6U)!= 0U) { }
                                    Ctrller.StateMachine[i].CmdParams.Flags = Msg.Acmpdu.Flags;
                                    Ctrller.StateMachine[i].CmdParams.Stream_vlan_id = Msg.Acmpdu.Stream_vlan_id;
                                    Ctrller.StateMachine[i].DoCommand = TRUE;
                                }
                                break;
                            case ACMP_MSG_CONNECT_TX_RESPONSE :
                            case ACMP_MSG_DISCONNECT_TX_RESPONSE :
                                break;
                            case ACMP_MSG_GET_TX_STATE_RESPONSE :
                            case ACMP_MSG_CONNECT_RX_RESPONSE :
                            case ACMP_MSG_DISCONNECT_RX_RESPONSE :
                            case ACMP_MSG_GET_RX_STATE_RESPONSE :
                            case ACMP_MSG_GET_TX_CONNECTION_RESPONSE :
                                {
                                    Ctrller.StateMachine[i].RcvdCmdResp.Message_type = Msg.AvtpduHeader.Message_type;
                                    Ctrller.StateMachine[i].RcvdCmdResp.Status = Msg.AvtpduHeader.Status;
                                    Ctrller.StateMachine[i].RcvdCmdResp.Stream_id = Msg.AvtpduHeader.Stream_id;
                                    Ctrller.StateMachine[i].RcvdCmdResp.Controller_entity_id = Msg.Acmpdu.Controller_entity_id;
                                    Ctrller.StateMachine[i].RcvdCmdResp.Talker_entity_id = Msg.Acmpdu.Talker_entity_id;
                                    Ctrller.StateMachine[i].RcvdCmdResp.Listener_entity_id = Msg.Acmpdu.Listener_entity_id;
                                    Ctrller.StateMachine[i].RcvdCmdResp.Talker_unique_id = Msg.Acmpdu.Talker_unique_id;
                                    Ctrller.StateMachine[i].RcvdCmdResp.Listener_unique_id = Msg.Acmpdu.Listener_unique_id;
                                    if (AmbaWrap_memcpy(Ctrller.StateMachine[i].RcvdCmdResp.Stream_dest_mac, Msg.Acmpdu.Stream_dest_mac, 6U)!= 0U) { }
                                    Ctrller.StateMachine[i].RcvdCmdResp.Connection_count = Msg.Acmpdu.Connection_count;
                                    Ctrller.StateMachine[i].RcvdCmdResp.Sequence_id = Msg.Acmpdu.Sequence_id;
                                    Ctrller.StateMachine[i].RcvdCmdResp.Flags = Msg.Acmpdu.Flags;
                                    Ctrller.StateMachine[i].RcvdCmdResp.Stream_vlan_id = Msg.Acmpdu.Stream_vlan_id;
                                    Ctrller.StateMachine[i].RcvdResponse = TRUE;
                                }
                                break;
                            default:
                                /* TODO */
                                break;
                            }
                        }
                    }
                    Ctrller_AcmpSm(&Ctrller.StateMachine[i]);
                    if (Ctrller.StateMachine[i].State > CONTROLLER_STATE_WAITING) {
                        Busy++;
                    }
                }
            }
            if (AmbaKAL_MutexGive(&Ctrller.Mutex) != KAL_ERR_NONE) {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            }
        }
        if (Busy == 0U) {
            (void) AmbaKAL_TaskSleep(10U);
        }
    }
}

UINT32 Ctrller_AddEntity(const AVDECC_ENTITY_INFO_s *EntityInfo)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Ctrller.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        for (i = 0U; i < Ctrller.MaxEntityNum; i++) {
            if (Ctrller.StateMachine[i].My_id == 0U) {
                Ctrller.StateMachine[i].My_id = EntityInfo->EntityDesc.Entity_id;
                break;
            }
        }
        if (AmbaKAL_MutexGive(&Ctrller.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

UINT32 Ctrller_RemoveEntity(const AVDECC_ENTITY_INFO_s *EntityInfo)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Ctrller.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        for (i = 0U; i < Ctrller.MaxEntityNum; i++) {
            if (Ctrller.StateMachine[i].My_id == EntityInfo->EntityDesc.Entity_id) {
                Ctrller.StateMachine[i].My_id = 0U;
                break;
            }
        }
        if (AmbaKAL_MutexGive(&Ctrller.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

UINT32 Ctrller_GetRequiredBufferSize(const AVDECC_CONTROLLER_INIT_CFG_s *Cfg, UINT32 *Size)
{
    UINT32 Rval = OK;
    UINT32 BufferSize = 0U;
    BufferSize += (GetAlignedValU32(Cfg->TaskInfo.StackSize, (UINT8)AMBA_CACHE_LINE_SIZE)); /* Task stack */
    BufferSize += (Cfg->MsgNum * sizeof(ACMP_PACKET_s)); /* MsgQ buffer */
    BufferSize += (Cfg->MaxEntityNum * sizeof(ACMP_CONTROLLER_SM_s)); /* StateMachine */
    BufferSize = GetAlignedValU32(BufferSize, AMBA_CACHE_LINE_SIZE);
    *Size = BufferSize;
    return Rval;
}

UINT32 Ctrller_GetDefaultInitCfg(AVDECC_CONTROLLER_INIT_CFG_s *Cfg)
{
    UINT32 Rval;
    if (AmbaWrap_memset(Cfg, 0, sizeof(AVDECC_CONTROLLER_INIT_CFG_s))!= 0U) { }
    Cfg->MaxEntityNum = AVDECC_ENTITY_CONTROLLER_MAX_NUM_DEFAULT;
    Cfg->TaskInfo.Priority = CONTROLLER_TASK_PRIORITY_DEFAULT;
    Cfg->TaskInfo.StackSize = CONTROLLER_TASK_STACK_SIZE_DEFAULT;
    Cfg->MsgNum = ACMP_MSG_MAX_NUM_DEFAULT;
    Rval = Ctrller_GetRequiredBufferSize(Cfg, &Cfg->BufferSize);
    return Rval;
}

static UINT32 Ctrller_InitImpl(const AVDECC_CONTROLLER_INIT_CFG_s *Cfg)
{
    static char MutexName[] = "AvdeccControllerMutex";
    static char MsgQueueName[] = "AvdeccControllerMsgQueue";
    static char TaskName[] = "AvdeccController";
    UINT32 Rval = AmbaKAL_MutexCreate(&Ctrller.Mutex, MutexName);
    if (Rval == KAL_ERR_NONE) {
        Rval = AmbaKAL_MsgQueueCreate(&Ctrller.MsgQueue, MsgQueueName,
                                     sizeof(ACMP_PACKET_s), Ctrller.MsgQBuffer,
                                     (Ctrller.MsgNum * sizeof(ACMP_PACKET_s)));
        if (Rval == KAL_ERR_NONE) {
            Rval = AmbaKAL_TaskCreate(&Ctrller.Task, TaskName, Cfg->TaskInfo.Priority,
                                     Ctrller_TaskEntry, NULL, Ctrller.TaskStack,
                                     Cfg->TaskInfo.StackSize, AMBA_KAL_DONT_START);
            if (Rval == KAL_ERR_NONE) {
                Rval = AmbaKAL_TaskSetSmpAffinity(&Ctrller.Task, AVDECC_CORE);
                if (Rval == KAL_ERR_NONE) {
                    Rval = AmbaKAL_TaskResume(&Ctrller.Task);
                    if (Rval != KAL_ERR_NONE) {
                        AmbaPrint_PrintStr5("%s, AmbaKAL_TaskResume(Ctrller.Task) failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_TaskSetSmpAffinity(Ctrller.Task) failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_PrintStr5("%s, AmbaKAL_TaskCreate(Ctrller.Task) failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueCreate(AvdeccControllerMsgQueue) failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexCreate() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

UINT32 Ctrller_Init(const AVDECC_CONTROLLER_INIT_CFG_s *Cfg)
{
    UINT32 Rval;
    UINT32 BufferSize;
    Rval = Ctrller_GetRequiredBufferSize(Cfg, &BufferSize);
    if (Rval == OK) {
        if (BufferSize == Cfg->BufferSize) {
            ULONG Addr;
            AmbaMisra_TypeCast(&Addr, &Cfg->Buffer);
            if ((Addr % AMBA_CACHE_LINE_SIZE) == 0U) {
                UINT8 *Buffer = Cfg->Buffer;
                Ctrller.MsgNum = Cfg->MsgNum;
                Ctrller.MaxEntityNum = Cfg->MaxEntityNum;
                Ctrller.TaskStack = Buffer; /* Task Stack */
                Addr += (GetAlignedValU32(Cfg->TaskInfo.StackSize, (UINT8)AMBA_CACHE_LINE_SIZE));
                AmbaMisra_TypeCast(&Ctrller.MsgQBuffer, &Addr); /* MsgQ Buffer */
                Addr += (Ctrller.MsgNum * sizeof(ACMP_PACKET_s));
                AmbaMisra_TypeCast(&Ctrller.StateMachine, &Addr); /* StateMachine */
                Rval = Ctrller_InitImpl(Cfg);
            } else {
                AmbaPrint_PrintStr5("%s, Buffer address not alignd!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, BufferSize != Cfg->BufferSize", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

