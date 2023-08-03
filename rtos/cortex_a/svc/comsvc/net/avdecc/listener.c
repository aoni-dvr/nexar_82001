/**
 * @file listener.c
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

#define LISTENER_TASK_PRIORITY_DEFAULT          (200U)
#define LISTENER_TASK_STACK_SIZE_DEFAULT        (0x2000U)

#define LISTENER_STATE_WAITING                  (0U)
#define LISTENER_STATE_CONNECT_TX_TIMEOUT       (1U)
#define LISTENER_STATE_DISCONNECT_TX_TIMEOUT    (2U)
#define LISTENER_STATE_CONNECT_RX_COMMAND       (3U)
#define LISTENER_STATE_CONNECT_TX_RESPONSE      (4U)
#define LISTENER_STATE_GET_STATE                (5U)
#define LISTENER_STATE_DISCONNECT_RX_COMMAND    (6U)
#define LISTENER_STATE_DISCONNECT_TX_RESPONSE   (7U)

typedef struct {
    AMBA_KAL_MUTEX_t Mutex;
    AMBA_KAL_TASK_t Task;
    UINT8 *TaskStack;
    UINT32 MsgNum;
    AMBA_KAL_MSG_QUEUE_t MsgQueue;
    ADP_PACKET_s *MsgQBuffer;
    ACMP_LISTENER_SM_s *StateMachine;
    UINT32 MaxEntityNum;
} AVDECC_LISTENER_s;

static AVDECC_LISTENER_s Listener = {0};

UINT32 Listener_GetConnectInfo(UINT64 EntityId, UINT32 *Connect, UINT8 **Info)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Listener.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        for (i = 0U; i < Listener.MaxEntityNum; i++) {
            if (Listener.StateMachine[i].My_id == EntityId) {
                const AVDECC_LISTENER_STREAM_INFO_s *InfoPtr = &Listener.StateMachine[i].ListenerStreamInfos;
                if (Listener.StateMachine[i].ListenerStreamInfos.Talker_entity_id != 0U) {
                    *Connect = 1;
                } else {
                    *Connect = 0;
                }
                AmbaMisra_TypeCast(Info, &InfoPtr);
                break;
            }
        }
        if (AmbaKAL_MutexGive(&Listener.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

UINT32 Listener_ResetConnectInfo(UINT64 EntityId)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Listener.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        for (i = 0U; i < Listener.MaxEntityNum; i++) {
            if (Listener.StateMachine[i].ListenerStreamInfos.Talker_entity_id == EntityId) {
                if (AmbaWrap_memset(&Listener.StateMachine[i].ListenerStreamInfos, 0, sizeof(AVDECC_LISTENER_STREAM_INFO_s))!= 0U) { }
                break;
            }
        }
        if (AmbaKAL_MutexGive(&Listener.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/* 8.2.2.5.2.1 validListenerUnique(ListenerUniqueId) */
static UINT32 Listener_ValidListenerUnique(UINT64 EntityId, UINT16 ListenerUniqueId)
{
    UINT32 Rval = 0U;
    UINT8 i;
    for (i = 0U; i < Listener.MaxEntityNum; i++) {
        if (Listener.StateMachine[i].My_id == EntityId) {
            if (Avdecc_GetUniqueId(Listener.StateMachine[i].My_id) == ListenerUniqueId) {
                Rval = 1U;
            }
            break;
        }
    }
    return Rval;
}

/* 8.2.2.5.2.2 listenerIsConnected(command) */
static UINT32 Listener_IsConnected(const AVDECC_LISTENER_STREAM_INFO_s *StreamInfo, const ACMP_COMMAND_RESPONSE_s *Cmd)
{
    UINT32 Rval = 0U;
    if (StreamInfo->Connected == TRUE) {
        if ((StreamInfo->Talker_entity_id != Cmd->Talker_entity_id) &&
            (StreamInfo->Talker_unique_id != Cmd->Talker_unique_id)) {
            Rval = 1U;
        }
    }
    return Rval;
}

/* 8.2.2.5.2.3 listenerIsConnectedTo(command) */
static UINT32 Listener_IsConnectedTo(const AVDECC_LISTENER_STREAM_INFO_s *StreamInfo, const ACMP_COMMAND_RESPONSE_s *Cmd)
{
    UINT32 Rval = 0U;
    if (StreamInfo->Connected == TRUE) {
        if ((StreamInfo->Talker_entity_id == Cmd->Talker_entity_id) &&
            (StreamInfo->Talker_unique_id == Cmd->Talker_unique_id)) {
            Rval = 1U;
        }
    }
    return Rval;
}

/* 8.2.2.5.2.4 txCommand(messageType, command, retry) */
static UINT32 Listener_TxCommand(UINT8 MsgType, const ACMP_COMMAND_RESPONSE_s *Cmd, UINT32 Retry)
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
        AcmpPkt.Acmpdu.Controller_entity_id = AVDECC_HTONLL(Cmd->Controller_entity_id);
        AcmpPkt.Acmpdu.Talker_entity_id = AVDECC_HTONLL(Cmd->Talker_entity_id);
        AcmpPkt.Acmpdu.Listener_entity_id = AVDECC_HTONLL(Cmd->Listener_entity_id);
        AcmpPkt.Acmpdu.Talker_unique_id = AVDECC_HTONS(Cmd->Talker_unique_id);
        AcmpPkt.Acmpdu.Listener_unique_id = AVDECC_HTONS(Cmd->Listener_unique_id);
        if (AmbaWrap_memcpy(AcmpPkt.Acmpdu.Stream_dest_mac, Cmd->Stream_dest_mac, 6U)!= 0U) { }
        AcmpPkt.Acmpdu.Connection_count = AVDECC_HTONS(Cmd->Connection_count);
        AcmpPkt.Acmpdu.Sequence_id = AVDECC_HTONS(Cmd->Sequence_id);
        AcmpPkt.Acmpdu.Flags = AVDECC_HTONS(Cmd->Flags);
        AcmpPkt.Acmpdu.Stream_vlan_id =  AVDECC_HTONS(Cmd->Stream_vlan_id);
        Rval = Avdecc_NetTx(0U, Buffer, Size, AcmpMulticastMac);
        if (Rval != ETH_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaEnet_Tx() failed!", __func__, NULL, NULL, NULL, NULL);
        }
        Rval = ACMP_STATUS_SUCCESS;
    }
    return Rval;
}

/* 8.2.2.5.2.5 txResponse(messageType, response, error) */
static UINT32 Listener_TxResponse(UINT8 MsgType, const ACMP_COMMAND_RESPONSE_s *Resp, UINT32 Error)
{
    UINT32 Rval;
    UINT16 Size = (UINT16)sizeof(ACMP_PACKET_s);
    UINT8 AcmpMulticastMac[6U] = ACMP_MULTICAST_MAC;
    ACMP_PACKET_s AcmpPkt;
    const ACMP_PACKET_s *Ptr;
    const UINT8 *Buffer;
    Ptr = &AcmpPkt;
    AmbaMisra_TypeCast(&Buffer, &Ptr);
    Rval = Acmp_PrepareTxCmd(MsgType, &AcmpPkt, Error);
    if (Rval == OK) {
        AcmpPkt.Acmpdu.Controller_entity_id =  AVDECC_HTONLL(Resp->Controller_entity_id);
        AcmpPkt.Acmpdu.Talker_entity_id =  AVDECC_HTONLL(Resp->Talker_entity_id);
        AcmpPkt.Acmpdu.Listener_entity_id =  AVDECC_HTONLL(Resp->Listener_entity_id);
        AcmpPkt.Acmpdu.Talker_unique_id =  AVDECC_HTONS(Resp->Talker_unique_id);
        AcmpPkt.Acmpdu.Listener_unique_id =  AVDECC_HTONS(Resp->Listener_unique_id);
        if (AmbaWrap_memcpy(AcmpPkt.Acmpdu.Stream_dest_mac, Resp->Stream_dest_mac, 6U)!= 0U) { }
        AcmpPkt.Acmpdu.Connection_count =  AVDECC_HTONS(Resp->Connection_count);
        AcmpPkt.Acmpdu.Sequence_id =  AVDECC_HTONS(Resp->Sequence_id);
        AcmpPkt.Acmpdu.Flags =  AVDECC_HTONS(Resp->Flags);
        AcmpPkt.Acmpdu.Stream_vlan_id =  AVDECC_HTONS(Resp->Stream_vlan_id);
        Rval = Avdecc_NetTx(0U, Buffer, Size, AcmpMulticastMac);
        if (Rval != ETH_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaEnet_Tx() failed!", __func__, NULL, NULL, NULL, NULL);
        }
        Rval = OK;
    }
    return Rval;
}

/* 8.2.2.5.2.6 connectListener(response) */
static UINT32 Listener_ConnectListener(AVDECC_LISTENER_STREAM_INFO_s *StreamInfo, const ACMP_COMMAND_RESPONSE_s *RespIn, ACMP_COMMAND_RESPONSE_s *RespOut)
{
    UINT32 Rval = ACMP_STATUS_SUCCESS;
    if (StreamInfo->Talker_entity_id == 0U) {
        AMBA_AVB_AVTP_LISTENER_s *AvtpListener;
        UINT32 Diff = 0U;
        UINT8 NullMultiCastMac[6U] = {0U};
        (void) Avdecc_GetListenerConfig(RespIn->Listener_entity_id, &AvtpListener);
        Avdecc_EntityId2Mac(RespIn->Talker_entity_id, AvtpListener->StreamID.Mac);
        if (AmbaWrap_memcmp(RespIn->Stream_dest_mac, NullMultiCastMac, 6U, &Diff)!= 0U) { }
        if (Diff == 0U) {
            Avdecc_EntityId2Mac(RespIn->Listener_entity_id, AvtpListener->MultiCastMac);
            if (AmbaWrap_memcpy(StreamInfo->Stream_dest_mac, AvtpListener->MultiCastMac, 6U)!= 0U) { }
        } else {
            if (AmbaWrap_memcpy(AvtpListener->MultiCastMac, RespIn->Stream_dest_mac, 6U)!= 0U) { }
            if (AmbaWrap_memcpy(StreamInfo->Stream_dest_mac, RespIn->Stream_dest_mac, 6U)!= 0U) { }
        }
        AvtpListener->StreamID.UniqueID = Avdecc_GetUniqueId(RespIn->Talker_entity_id);
        if (AmbaAvbStack_AvtpListenerReg(AvtpListener) != NET_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaAvbStack_AvtpListenerReg() failed!", __func__, NULL, NULL, NULL, NULL);
        }
        StreamInfo->Talker_entity_id = RespIn->Talker_entity_id;
        StreamInfo->Talker_unique_id = RespIn->Talker_unique_id;
        StreamInfo->Connected = TRUE;
        StreamInfo->Stream_id = RespIn->Stream_id;
        StreamInfo->Controller_entity_id = RespIn->Controller_entity_id;
        StreamInfo->Flags = RespIn->Flags;
        StreamInfo->Stream_vlan_id = RespIn->Stream_vlan_id;
    } else {
        Rval = ACMP_STATUS_LISTENER_EXCLUSIVE;
    }
    if (AmbaWrap_memcpy(RespOut, RespIn, sizeof(ACMP_COMMAND_RESPONSE_s))!= 0U) { }
    return Rval;
}

/* 8.2.2.5.2.7 disconnectListener(response) */
static UINT32 Listener_DisconnectListener(AVDECC_LISTENER_STREAM_INFO_s *StreamInfo, const ACMP_COMMAND_RESPONSE_s *RespIn, ACMP_COMMAND_RESPONSE_s *RespOut)
{
    UINT32 Rval = ACMP_STATUS_SUCCESS;
    AMBA_AVB_AVTP_LISTENER_s *AvtpListener;
    (void) Avdecc_GetListenerConfig(RespIn->Listener_entity_id, &AvtpListener);
    if (AmbaAvbStack_AvtpListenerUnReg(AvtpListener) != NET_ERR_NONE) {
        AmbaPrint_PrintStr5("%s, AmbaAvbStack_AvtpListenerReg() failed!", __func__, NULL, NULL, NULL, NULL);
        Rval = ACMP_STATUS_LISTENER_MISBEHAVING;
    }
    if (AmbaWrap_memcpy(RespOut, RespIn, sizeof(ACMP_COMMAND_RESPONSE_s))!= 0U) { }
    if (AmbaWrap_memset(StreamInfo, 0, sizeof(AVDECC_LISTENER_STREAM_INFO_s))!= 0U) { }
    return Rval;
}

/* 8.2.2.5.2.8 cancelTimeout(commandResponse) */
static UINT32 Listener_CancelTimeout(const ACMP_COMMAND_RESPONSE_s *CmdResp)
{
    UINT32 Rval = ACMP_STATUS_SUCCESS;
    AmbaMisra_TouchUnused(CmdResp);
    return Rval;
}

/* 8.2.2.5.2.9 removeInflight(commandResponse) */
static ACMP_INFLIGHT_CMD_s *Listener_RemoveInflight(ACMP_INFLIGHT_CMD_s *Inflight, UINT16 SequenceId)
{
    ACMP_INFLIGHT_CMD_s *Head;
    Head = Acmp_RemoveInflightCmd(Inflight, SequenceId);
    return Head;
}

/* 8.2.2.5.2.10 getState(command) */
static UINT32 Listener_GetState(const AVDECC_LISTENER_STREAM_INFO_s *StreamInfo, const ACMP_COMMAND_RESPONSE_s *Cmd, ACMP_COMMAND_RESPONSE_s *Resp)
{
    UINT32 Rval = ACMP_STATUS_SUCCESS;
    if (AmbaWrap_memcpy(Resp, Cmd, sizeof(ACMP_COMMAND_RESPONSE_s))!= 0U) { }
    Resp->Talker_entity_id = StreamInfo->Talker_entity_id;
    Resp->Talker_unique_id = StreamInfo->Talker_unique_id;
    Resp->Stream_id = StreamInfo->Stream_id;
    if (AmbaWrap_memcpy(Resp->Stream_dest_mac, StreamInfo->Stream_dest_mac, 6U)!= 0U) { }
    Resp->Controller_entity_id = StreamInfo->Controller_entity_id;
    Resp->Flags = StreamInfo->Flags;
    Resp->Stream_vlan_id = StreamInfo->Stream_vlan_id;
    return Rval;
}

static void Listener_AcmpSm(ACMP_LISTENER_SM_s *StateMachine)
{
    ACMP_INFLIGHT_CMD_s *TimeoutCmd = Acmp_InflightCmdTimeout(StateMachine->Inflight);
    if (TimeoutCmd != NULL) {
        if (TimeoutCmd->Cmd.Message_type == ACMP_MSG_CONNECT_TX_COMMAND) {
            StateMachine->State = LISTENER_STATE_CONNECT_TX_TIMEOUT;
        }
        if (TimeoutCmd->Cmd.Message_type == ACMP_MSG_DISCONNECT_TX_COMMAND) {
            StateMachine->State = LISTENER_STATE_DISCONNECT_TX_TIMEOUT;
        }
    }
    switch (StateMachine->State) {
    case LISTENER_STATE_WAITING:
        if (StateMachine->DoTerminate == TRUE) {
            // TODO: free enqueued inflight command.
        } else {
            if (StateMachine->RcvdConnectRXCmd == TRUE) {
                StateMachine->RcvdConnectRXCmd = FALSE;
                StateMachine->State = LISTENER_STATE_CONNECT_RX_COMMAND;
            } else if (StateMachine->RcvdConnectTXResp == TRUE) {
                StateMachine->RcvdConnectTXResp = FALSE;
                StateMachine->State = LISTENER_STATE_CONNECT_TX_RESPONSE;
            } else if (StateMachine->RcvdGetRXState == TRUE) {
                StateMachine->RcvdGetRXState = FALSE;
                StateMachine->State = LISTENER_STATE_GET_STATE;
            } else if (StateMachine->RcvdDisconnectRXCmd == TRUE) {
                StateMachine->RcvdDisconnectRXCmd = FALSE;
                StateMachine->State = LISTENER_STATE_DISCONNECT_RX_COMMAND;
            } else if (StateMachine->RcvdDisconnectTXResp == TRUE) {
                StateMachine->RcvdDisconnectTXResp = FALSE;
                StateMachine->State = LISTENER_STATE_DISCONNECT_TX_RESPONSE;
            } else {
                // TODO: state or sleep
            }
        }
        break;
    case LISTENER_STATE_CONNECT_TX_TIMEOUT:
        /*
         *  if (inflight[x].retried == TRUE) {
         *      response = inflight[x].command;
         *      response.sequence_id = inflight[x].original_sequence_id;
         *      txResponse(ACMP_MSG_CONNECT_RX_RESPONSE, response, LISTENER_TALKER_TIMEOUT);
         *      removeInflight(inflight[x].command);
         *  } else {
         *      txCommand(ACMP_MSG_CONNECT_TX_COMMAND, inflight[x].command, TRUE);
         *  }
         */
        if (TimeoutCmd != NULL) {
            if (TimeoutCmd->Retried == TRUE) {
                UINT32 Rval;
                if (AmbaWrap_memcpy(&StateMachine->Resp, &TimeoutCmd->Cmd, sizeof(ACMP_COMMAND_RESPONSE_s))!= 0U) { }
                StateMachine->Resp.Sequence_id = TimeoutCmd->Original_sequence_id;
                Rval = Listener_TxResponse(ACMP_MSG_CONNECT_RX_RESPONSE, &StateMachine->RcvdCmdResp, ACMP_STATUS_LISTENER_TALKER_TIMEOUT);
                if (Rval == OK) {
                    StateMachine->Inflight = Listener_RemoveInflight(StateMachine->Inflight, TimeoutCmd->Cmd.Sequence_id);
                }
            } else {
                if (Listener_TxCommand(ACMP_MSG_CONNECT_TX_COMMAND, &TimeoutCmd->Cmd, TRUE) == OK) {
                    UINT32 CurrentTime;
                    TimeoutCmd->Retried = TRUE;
                    (void)AmbaKAL_GetSysTickCount(&CurrentTime);
                    TimeoutCmd->Timeout = CurrentTime + ACMP_CMD_TIMEOUT_CONNECT_TX_COMMAND;
                }
            }
        }
        StateMachine->State = LISTENER_STATE_WAITING; /* UCT to AVDECC_LISTENER_ACMP_STATE_WAITING */
        break;
    case LISTENER_STATE_DISCONNECT_TX_TIMEOUT:
        /*
         *  if (inflight[x].retried == TRUE) {
         *      response = inflight[x].command;
         *      response.sequence_id = inflight[x].original_sequence_id;
         *      txResponse(ACMP_MSG_DISCONNECT_RX_RESPONSE, response, LISTENER_TALKER_TIMEOUT);
         *      removeInflight(inflight[x].command);
         *  } else {
         *      txCommand(ACMP_MSG_DISCONNECT_TX_COMMAND, inflight[x].command, TRUE);
         *  }
         */
        if (TimeoutCmd != NULL) {
            if (TimeoutCmd->Retried == TRUE) {
                UINT32 Rval;
                if (AmbaWrap_memcpy(&StateMachine->Resp, &TimeoutCmd->Cmd, sizeof(ACMP_COMMAND_RESPONSE_s))!= 0U) { }
                StateMachine->Resp.Sequence_id = TimeoutCmd->Original_sequence_id;
                Rval = Listener_TxResponse(ACMP_MSG_DISCONNECT_RX_RESPONSE, &StateMachine->RcvdCmdResp, ACMP_STATUS_LISTENER_TALKER_TIMEOUT);
                if (Rval == OK) {
                    StateMachine->Inflight = Listener_RemoveInflight(StateMachine->Inflight, TimeoutCmd->Cmd.Sequence_id);
                }
            } else {
                if (Listener_TxCommand(ACMP_MSG_DISCONNECT_TX_COMMAND, &TimeoutCmd->Cmd, TRUE) == OK) {
                    UINT32 CurrentTime;
                    TimeoutCmd->Retried = TRUE;
                    (void)AmbaKAL_GetSysTickCount(&CurrentTime);
                    TimeoutCmd->Timeout = CurrentTime + ACMP_CMD_TIMEOUT_DISCONNECT_TX_COMMAND;
                }
            }
        }
        StateMachine->State = LISTENER_STATE_WAITING; /* UCT to AVDECC_LISTENER_ACMP_STATE_WAITING */
        break;
    case LISTENER_STATE_CONNECT_RX_COMMAND:
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
        if (Listener_ValidListenerUnique(StateMachine->My_id, StateMachine->RcvdCmdResp.Listener_unique_id) == 1U) {
            if (Listener_IsConnected(&StateMachine->ListenerStreamInfos, &StateMachine->RcvdCmdResp) == 0U) {
                StateMachine->RcvdCmdResp.Message_type = ACMP_MSG_CONNECT_TX_COMMAND;
                if (Listener_TxCommand(ACMP_MSG_CONNECT_TX_COMMAND, &StateMachine->RcvdCmdResp, FALSE) == OK) {
                    StateMachine->Inflight = Acmp_AddInflightCmd(StateMachine->Inflight, StateMachine->RcvdCmdResp, ACMP_CMD_TIMEOUT_CONNECT_TX_COMMAND);
                }
            } else {
                if (Listener_TxResponse(ACMP_MSG_CONNECT_RX_RESPONSE, &StateMachine->RcvdCmdResp, ACMP_STATUS_LISTENER_EXCLUSIVE) != OK) {
                    AmbaPrint_PrintStr5("%s, Listener_TxResponse(ACMP_MSG_CONNECT_RX_RESPONSE) failed!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        } else {
            if (Listener_TxResponse(ACMP_MSG_CONNECT_RX_RESPONSE, &StateMachine->RcvdCmdResp, ACMP_STATUS_LISTENER_UNKNOWN_ID) != OK) {
                AmbaPrint_PrintStr5("%s, Listener_TxResponse(ACMP_MSG_CONNECT_RX_RESPONSE) failed!", __func__, NULL, NULL, NULL, NULL);
            }
        }
        StateMachine->State = LISTENER_STATE_WAITING; /* UCT to AVDECC_LISTENER_ACMP_STATE_WAITING */
        break;

    case LISTENER_STATE_CONNECT_TX_RESPONSE:
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
        if (Listener_ValidListenerUnique(StateMachine->My_id, StateMachine->RcvdCmdResp.Listener_unique_id) == 1U) {
            UINT32 Status;
            if (StateMachine->RcvdCmdResp.Status == ACMP_STATUS_SUCCESS) {
                Status = Listener_ConnectListener(&StateMachine->ListenerStreamInfos, &StateMachine->RcvdCmdResp, &StateMachine->Resp);
            } else {
                if (AmbaWrap_memcpy(&StateMachine->Resp, &StateMachine->RcvdCmdResp, sizeof(ACMP_COMMAND_RESPONSE_s))!= 0U) { }
                Status = StateMachine->RcvdCmdResp.Status;
            }
            if (StateMachine->Inflight != NULL) {
                if (Acmp_GetInflightCmdId(StateMachine->Inflight, &StateMachine->RcvdCmdResp, ACMP_MSG_CONNECT_TX_COMMAND, &StateMachine->Resp.Sequence_id) != OK) {
                    AmbaPrint_PrintStr5("Can't find Sequence_Id of InflightCmd", NULL, NULL, NULL, NULL, NULL);
                }
            }
            if (Listener_CancelTimeout(&StateMachine->RcvdCmdResp) == OK) {
                StateMachine->Inflight = Listener_RemoveInflight(StateMachine->Inflight, StateMachine->RcvdCmdResp.Sequence_id);
                if (Listener_TxResponse(ACMP_MSG_CONNECT_RX_RESPONSE, &StateMachine->Resp, Status) != OK) {
                    AmbaPrint_PrintStr5("%s, Listener_TxResponse(ACMP_MSG_CONNECT_RX_RESPONSE) failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_PrintStr5("%s, Listener_CancelTimeout() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        }
        StateMachine->State = LISTENER_STATE_WAITING; /* UCT to AVDECC_LISTENER_ACMP_STATE_WAITING */
        break;
    case LISTENER_STATE_GET_STATE:
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
            UINT32 Error;
            if (Listener_ValidListenerUnique(StateMachine->My_id, StateMachine->RcvdCmdResp.Listener_unique_id) == 1U) {
                // TODO: Multiple connection?!
                Error = Listener_GetState(&StateMachine->ListenerStreamInfos, &StateMachine->RcvdCmdResp, &StateMachine->Resp);
            } else {
                if (AmbaWrap_memcpy(&StateMachine->Resp, &StateMachine->RcvdCmdResp, sizeof(ACMP_COMMAND_RESPONSE_s))!= 0U) { }
                Error = ACMP_STATUS_LISTENER_UNKNOWN_ID;
            }
            if (Listener_TxResponse(ACMP_MSG_GET_RX_STATE_RESPONSE, &StateMachine->Resp, Error) != OK) {
                AmbaPrint_PrintStr5("%s, Listener_TxResponse(ACMP_MSG_GET_RX_STATE_RESPONSE) failed!", __func__, NULL, NULL, NULL, NULL);
            }
            StateMachine->State = LISTENER_STATE_WAITING; /* UCT to AVDECC_LISTENER_ACMP_STATE_WAITING */
        }
        break;
    case LISTENER_STATE_DISCONNECT_RX_COMMAND:
        /*
         *  if (validListenerUnique(rcvdCmdResp.listener_unique_id)) {
         *      if (listenerIsConnected(rcvdCmdResp)) {
         *          (response, status) = disconnectListener(rcvdCmdResp);
         *          if (SUCCESS == status) {
         *              txCommand(ACMP_MSG_DISCONNECT_TX_COMMAND, rcvdCmdResp, FALSE);
         *          } else {
         *              txResponse(ACMP_MSG_DISCONNECT_RX_RESPONSE, response, status);
         *          }
         *      } else {
         *          txResponse(ACMP_MSG_DISCONNECT_RX_RESPONSE, rcvdCmdResp, NOT_CONNECTED);
         *      }
         *  } else {
         *      txResponse(ACMP_MSG_DISCONNECT_RX_RESPONSE, rcvdCmdResp, LISTENER_UNKNOW_ID);
         *  }
         */
        if (Listener_ValidListenerUnique(StateMachine->My_id, StateMachine->RcvdCmdResp.Listener_unique_id) == TRUE) {
            if (Listener_IsConnectedTo(&StateMachine->ListenerStreamInfos, &StateMachine->RcvdCmdResp) == TRUE) {
                UINT32 Status = Listener_DisconnectListener(&StateMachine->ListenerStreamInfos, &StateMachine->RcvdCmdResp, &StateMachine->Resp);
                if (Status == ACMP_STATUS_SUCCESS) {
                    StateMachine->RcvdCmdResp.Message_type = ACMP_MSG_DISCONNECT_TX_COMMAND;
                    if (Listener_TxCommand(ACMP_MSG_DISCONNECT_TX_COMMAND, &StateMachine->RcvdCmdResp, FALSE) == OK) {
                        StateMachine->Inflight = Acmp_AddInflightCmd(StateMachine->Inflight, StateMachine->RcvdCmdResp,
                                                                            ACMP_CMD_TIMEOUT_DISCONNECT_TX_COMMAND);
                    }
                } else {
                    if (Listener_TxResponse(ACMP_MSG_DISCONNECT_RX_RESPONSE, &StateMachine->Resp, Status) != OK) {
                        AmbaPrint_PrintStr5("%s, Listener_TxResponse(ACMP_MSG_DISCONNECT_RX_COMMAND) failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            } else {
                if (Listener_TxResponse(ACMP_MSG_DISCONNECT_RX_RESPONSE, &StateMachine->RcvdCmdResp, ACMP_STATUS_NOT_CONNECTED) != OK) {
                    AmbaPrint_PrintStr5("%s, Listener_TxResponse(ACMP_MSG_DISCONNECT_RX_COMMAND) failed!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        } else {
            if (Listener_TxResponse(ACMP_MSG_DISCONNECT_RX_RESPONSE, &StateMachine->RcvdCmdResp, ACMP_STATUS_LISTENER_UNKNOWN_ID) != OK) {
                AmbaPrint_PrintStr5("%s, Listener_TxResponse(ACMP_MSG_DISCONNECT_RX_COMMAND) failed!", __func__, NULL, NULL, NULL, NULL);
            }
        }
        StateMachine->State = LISTENER_STATE_WAITING; /* UCT to AVDECC_LISTENER_ACMP_STATE_WAITING */
        break;
    case LISTENER_STATE_DISCONNECT_TX_RESPONSE:
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
        if (Listener_ValidListenerUnique(StateMachine->My_id, StateMachine->RcvdCmdResp.Listener_unique_id) == TRUE) {
            UINT8 Status = StateMachine->RcvdCmdResp.Status;
            if (AmbaWrap_memcpy(&StateMachine->Resp, &StateMachine->RcvdCmdResp, sizeof(ACMP_COMMAND_RESPONSE_s))!= 0U) { }
            if (StateMachine->Inflight != NULL) {
                if (Acmp_GetInflightCmdId(StateMachine->Inflight, &StateMachine->RcvdCmdResp, ACMP_MSG_DISCONNECT_TX_COMMAND, &StateMachine->Resp.Sequence_id) != OK) {
                    AmbaPrint_PrintStr5("Can't find Sequence_Id of InflightCmd", NULL, NULL, NULL, NULL, NULL);
                }
            }
            if (Listener_CancelTimeout(&StateMachine->RcvdCmdResp) == OK) {
                StateMachine->Inflight = Listener_RemoveInflight(StateMachine->Inflight, StateMachine->RcvdCmdResp.Sequence_id);
                if (Listener_TxResponse(ACMP_MSG_DISCONNECT_RX_RESPONSE, &StateMachine->Resp, Status) != OK) {
                    AmbaPrint_PrintStr5("%s, Listener_TxResponse(ACMP_MSG_DISCONNECT_RX_RESPONSE) failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_PrintStr5("%s, Listener_CancelTimeout() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        }
        StateMachine->State = LISTENER_STATE_WAITING; /* UCT to AVDECC_LISTENER_ACMP_STATE_WAITING */
        break;
    default:
        /* TODO */
        break;
    }
}

UINT32 Listener_SendAcmpPacket(const ACMP_PACKET_s *AcmpPkt)
{
    UINT32 Rval;
    ACMP_PACKET_s Msg;
    if (AmbaWrap_memcpy(&Msg, AcmpPkt, sizeof(ACMP_PACKET_s))!= 0U) { }
    Rval = AmbaKAL_MsgQueueSend(&Listener.MsgQueue, &Msg, AMBA_KAL_NO_WAIT);
    if (Rval != KAL_ERR_NONE) {
        AmbaPrint_PrintStr5("%s: cannot send message", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static void *Listener_TaskEntry(void *Info)
{
    UINT8 i;
    ACMP_PACKET_s Msg;
    (void)Info;
    AmbaPrint_PrintStr5("%s, start", __func__, NULL, NULL, NULL, NULL);
    for (;;) {
        UINT32 Busy = 0U;
        if (AmbaKAL_MutexTake(&Listener.Mutex, AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
            UINT32 Rval = AmbaKAL_MsgQueueReceive(&Listener.MsgQueue, &Msg, 10U);
            for (i = 0U; i < Listener.MaxEntityNum; i++) {
                if (Listener.StateMachine[i].My_id != 0U) {
                    if (Listener.StateMachine[i].State == LISTENER_STATE_WAITING) {
                        if (Rval == KAL_ERR_NONE) {
                            if (Msg.Acmpdu.Listener_entity_id == Listener.StateMachine[i].My_id) {
                                Listener.StateMachine[i].RcvdCmdResp.Message_type = Msg.AvtpduHeader.Message_type;
                                Listener.StateMachine[i].RcvdCmdResp.Status = Msg.AvtpduHeader.Status;
                                Listener.StateMachine[i].RcvdCmdResp.Stream_id = Msg.AvtpduHeader.Stream_id;
                                Listener.StateMachine[i].RcvdCmdResp.Controller_entity_id = Msg.Acmpdu.Controller_entity_id;
                                Listener.StateMachine[i].RcvdCmdResp.Talker_entity_id = Msg.Acmpdu.Talker_entity_id;
                                Listener.StateMachine[i].RcvdCmdResp.Listener_entity_id = Msg.Acmpdu.Listener_entity_id;
                                Listener.StateMachine[i].RcvdCmdResp.Talker_unique_id = Msg.Acmpdu.Talker_unique_id;
                                Listener.StateMachine[i].RcvdCmdResp.Listener_unique_id = Msg.Acmpdu.Listener_unique_id;
                                if (AmbaWrap_memcpy(Listener.StateMachine[i].RcvdCmdResp.Stream_dest_mac, Msg.Acmpdu.Stream_dest_mac, 6U)!= 0U) { }
                                Listener.StateMachine[i].RcvdCmdResp.Connection_count = Msg.Acmpdu.Connection_count;
                                Listener.StateMachine[i].RcvdCmdResp.Sequence_id = Msg.Acmpdu.Sequence_id;
                                Listener.StateMachine[i].RcvdCmdResp.Flags = Msg.Acmpdu.Flags;
                                Listener.StateMachine[i].RcvdCmdResp.Stream_vlan_id = Msg.Acmpdu.Stream_vlan_id;
                                switch (Msg.AvtpduHeader.Message_type) {
                                case ACMP_MSG_CONNECT_TX_RESPONSE:
                                    Listener.StateMachine[i].RcvdConnectTXResp = TRUE;
                                    break;
                                case ACMP_MSG_DISCONNECT_TX_RESPONSE:
                                    Listener.StateMachine[i].RcvdDisconnectTXResp = TRUE;
                                    break;
                                case ACMP_MSG_CONNECT_RX_COMMAND:
                                    Listener.StateMachine[i].RcvdConnectRXCmd = TRUE;
                                    break;
                                case ACMP_MSG_DISCONNECT_RX_COMMAND:
                                    Listener.StateMachine[i].RcvdDisconnectRXCmd = TRUE;
                                    break;
                                case ACMP_MSG_GET_RX_STATE_COMMAND:
                                    Listener.StateMachine[i].RcvdGetRXState = TRUE;
                                    break;
                                default:
                                    /* TODO */
                                    break;
                                }
                            }
                        }
                    }
                    Listener_AcmpSm(&Listener.StateMachine[i]);
                    if (Listener.StateMachine[i].State > LISTENER_STATE_WAITING) {
                        Busy++;
                    }
                }
            }
            if (AmbaKAL_MutexGive(&Listener.Mutex) != KAL_ERR_NONE) {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() failed!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Busy == 0U) {
            (void) AmbaKAL_TaskSleep(10U);
        }
    }
}

UINT32 Listener_AddEntity(const AVDECC_ENTITY_INFO_s *EntityInfo)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Listener.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        for (i = 0U; i < Listener.MaxEntityNum; i++) {
            if (Listener.StateMachine[i].My_id == 0U) {
                Listener.StateMachine[i].My_id = EntityInfo->EntityDesc.Entity_id;
                break;
            }
        }
        if (AmbaKAL_MutexGive(&Listener.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

UINT32 Listener_RemoveEntity(const AVDECC_ENTITY_INFO_s *EntityInfo)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Listener.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        for (i = 0U; i < Listener.MaxEntityNum; i++) {
            if (Listener.StateMachine[i].My_id == EntityInfo->EntityDesc.Entity_id) {
                Listener.StateMachine[i].My_id = 0U;
                break;
            }
        }
        if (AmbaKAL_MutexGive(&Listener.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

UINT32 Listener_GetRequiredBufferSize(const AVDECC_LISTENER_INIT_CFG_s *Cfg, UINT32 *Size)
{
    UINT32 Rval = OK;
    UINT32 BufferSize = 0U;
    BufferSize += (GetAlignedValU32(Cfg->TaskInfo.StackSize, (UINT8)AMBA_CACHE_LINE_SIZE)); /* Task stack */
    BufferSize += (Cfg->MsgNum * sizeof(ACMP_PACKET_s)); /* MsgQ buffer */
    BufferSize += (Cfg->MaxEntityNum * sizeof(ACMP_LISTENER_SM_s)); /* StateMachine */
    BufferSize = GetAlignedValU32(BufferSize, AMBA_CACHE_LINE_SIZE);
    *Size = BufferSize;
    return Rval;
}

UINT32 Listener_GetDefaultInitCfg(AVDECC_LISTENER_INIT_CFG_s *Cfg)
{
    UINT32 Rval;
    if (AmbaWrap_memset(Cfg, 0, sizeof(AVDECC_LISTENER_INIT_CFG_s))!= 0U) { }
    Cfg->MaxEntityNum = AVDECC_ENTITY_LISTENER_MAX_NUM_DEFAULT;
    Cfg->TaskInfo.Priority = LISTENER_TASK_PRIORITY_DEFAULT;
    Cfg->TaskInfo.StackSize = LISTENER_TASK_STACK_SIZE_DEFAULT;
    Cfg->MsgNum = ACMP_MSG_MAX_NUM_DEFAULT;
    Rval = Listener_GetRequiredBufferSize(Cfg, &Cfg->BufferSize);
    return Rval;
}

static UINT32 Listener_InitImpl(const AVDECC_LISTENER_INIT_CFG_s *Cfg)
{
    static char MutexName[] = "AvdeccListenerMutex";
    static char MsgQueueName[] = "AvdeccListenerMsgQueue";
    static char TaskName[] = "AvdeccListener";
    UINT32 Rval = AmbaKAL_MutexCreate(&Listener.Mutex, MutexName);
    if (Rval == KAL_ERR_NONE) {
        Rval = AmbaKAL_MsgQueueCreate(&Listener.MsgQueue, MsgQueueName,
                                     sizeof(ACMP_PACKET_s), Listener.MsgQBuffer,
                                     (Listener.MsgNum * sizeof(ACMP_PACKET_s)));
        if (Rval == KAL_ERR_NONE) {
            Rval = AmbaKAL_TaskCreate(&Listener.Task, TaskName, Cfg->TaskInfo.Priority,
                                     Listener_TaskEntry, NULL, Listener.TaskStack,
                                     Cfg->TaskInfo.StackSize, AMBA_KAL_DONT_START);
            if (Rval == KAL_ERR_NONE) {
                Rval = AmbaKAL_TaskSetSmpAffinity(&Listener.Task, AVDECC_CORE);
                if (Rval == KAL_ERR_NONE) {
                    Rval = AmbaKAL_TaskResume(&Listener.Task);
                    if (Rval != KAL_ERR_NONE) {
                        AmbaPrint_PrintStr5("%s, AmbaKAL_TaskResume(Listener.Task) failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_TaskSetSmpAffinity(Listener.Task) failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_PrintStr5("%s, AmbaKAL_TaskCreate(Listener.Task) failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueCreate(AvdeccListenerMsgQueue) failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexCreate() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

UINT32 Listener_Init(const AVDECC_LISTENER_INIT_CFG_s *Cfg)
{
    UINT32 Rval;
    UINT32 BufferSize;
    Rval = Listener_GetRequiredBufferSize(Cfg, &BufferSize);
    if (Rval == OK) {
        if (BufferSize == Cfg->BufferSize) {
            ULONG Addr;
            AmbaMisra_TypeCast(&Addr, &Cfg->Buffer);
            if ((Addr % AMBA_CACHE_LINE_SIZE) == 0U) {
                UINT8 *Buffer = Cfg->Buffer;
                Listener.MsgNum = Cfg->MsgNum;
                Listener.MaxEntityNum = Cfg->MaxEntityNum;
                Listener.TaskStack = Buffer; /* Task Stack */
                Addr += (GetAlignedValU32(Cfg->TaskInfo.StackSize, (UINT8)AMBA_CACHE_LINE_SIZE));
                AmbaMisra_TypeCast(&Listener.MsgQBuffer, &Addr); /* MsgQ Buffer */
                Addr += (Listener.MsgNum * sizeof(ACMP_PACKET_s));
                AmbaMisra_TypeCast(&Listener.StateMachine, &Addr); /* StateMachine */
                Rval = Listener_InitImpl(Cfg);
            } else {
                AmbaPrint_PrintStr5("%s, Buffer address not alignd!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, BufferSize != Cfg->BufferSize", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

