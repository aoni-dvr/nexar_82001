/**
 * @file talker.c
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

#define TALKER_TASK_PRIORITY_DEFAULT    (200U)
#define TALKER_TASK_STACK_SIZE_DEFAULT  (0x2000)

#define TALKER_STATE_WAITING            (0U)
#define TALKER_STATE_CONNECT            (1U)
#define TALKER_STATE_DISCONNECT         (2U)
#define TALKER_STATE_GET_STATE          (3U)
#define TALKER_STATE_GET_CONNECTION     (4U)

typedef struct {
    AMBA_KAL_MUTEX_t Mutex;
    AMBA_KAL_TASK_t Task;
    UINT8 *TaskStack;
    UINT32 MsgNum;
    AMBA_KAL_MSG_QUEUE_t MsgQueue;
    ADP_PACKET_s *MsgQBuffer;
    ACMP_TALKER_SM_s *StateMachine;
    UINT32 MaxEntityNum;
    UINT32 MaxConnectNum;
} AVDECC_TALKER_s;

static AVDECC_TALKER_s Talker = {0};

UINT32 Talker_GetConnectInfo(UINT64 EntityId, UINT32 *Connect, UINT8 **Info)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Talker.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        for (i = 0U; i < Talker.MaxEntityNum; i++) {
            if (Talker.StateMachine[i].My_id == EntityId) {
                UINT8 j;
                for (j = 0U; j < Talker.MaxConnectNum; j++) {
                    if (Talker.StateMachine[i].TalkerStreamInfos.Connected_listeners[j].Listener_entity_id != 0U) {
                        *Connect = 1U;
                        break;
                    } else {
                        *Connect = 0U;
                    }
                }
                AmbaMisra_TypeCast(Info, &Talker.StateMachine[i].TalkerStreamInfos.Connected_listeners);
                break;
            }
        }
        if (AmbaKAL_MutexGive(&Talker.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

UINT32 Talker_ResetConnectInfo(UINT64 EntityId)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Talker.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        for (i = 0U; i < Talker.MaxEntityNum; i++) {
            if (Talker.StateMachine[i].My_id != 0U) {
                UINT8 j;
                for (j = 0U; j < Talker.MaxConnectNum; j++) {
                    if (Talker.StateMachine[i].TalkerStreamInfos.Connected_listeners[j].Listener_entity_id == EntityId) {
                        UINT16 Connection_count = Talker.StateMachine[i].TalkerStreamInfos.Connection_count;
                        if (AmbaWrap_memset(&Talker.StateMachine[i].TalkerStreamInfos, 0, sizeof(AVDECC_TALKER_STREAM_INFO_s))!= 0U) { }
                        Talker.StateMachine[i].TalkerStreamInfos.Connection_count = (Connection_count - 1U);
                        break;
                    }
                }
            }
        }
        if (AmbaKAL_MutexGive(&Talker.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/* 8.2.2.6.2.1 validTalkerUnique(TalkerUniqueId) */
static UINT32 Talker_ValidTalkerUnique(UINT64 EntityId, UINT16 TalkerUniqueId)
{
    UINT8 i;
    UINT32 Rval = OK;
    for (i = 0; i < Talker.MaxEntityNum; i++) {
        if (Talker.StateMachine[i].My_id == EntityId) {
            if (Avdecc_GetUniqueId(Talker.StateMachine[i].My_id) == TalkerUniqueId) {
                Rval = 1U;
            }
        }
    }
    return Rval;
}

/* 8.2.2.6.2.2 connectTalker(command) */
static UINT32 Talker_ConnectTalker(AVDECC_TALKER_STREAM_INFO_s *StreamInfo, const ACMP_COMMAND_RESPONSE_s *Cmd, ACMP_COMMAND_RESPONSE_s *Resp)
{
    UINT8 i;
    UINT32 Rval = ACMP_STATUS_SUCCESS;
    for (i = 0U; i < Talker.MaxConnectNum; i++) {
        if ((StreamInfo->Connected_listeners[i].Listener_entity_id == 0U) &&
            (StreamInfo->Connected_listeners[i].Listener_entity_id != Cmd->Listener_entity_id)){
            StreamInfo->Stream_id = Cmd->Stream_id;
            if (StreamInfo->Connection_count == 0U) {
                AMBA_AVB_AVTP_TALKER_s *AvtpTalker;
                UINT32 Diff = 0U;
                UINT8 NullMultiCastMac[6U] = {0U};
                (void) Avdecc_GetTalkerConfig(Cmd->Talker_entity_id, &AvtpTalker);
                Avdecc_EntityId2Mac(Cmd->Talker_entity_id, AvtpTalker->StreamID.Mac);
                if (AmbaWrap_memcmp(Cmd->Stream_dest_mac, NullMultiCastMac, 6U, &Diff)!= 0U) { }
                if (Diff == 0U) {
                    Avdecc_EntityId2Mac(Cmd->Listener_entity_id, AvtpTalker->MultiCastMac);
                    if (AmbaWrap_memcpy(StreamInfo->Stream_dest_mac, AvtpTalker->MultiCastMac, 6U)!= 0U) { }
                } else {
                    if (AmbaWrap_memcpy(AvtpTalker->MultiCastMac, Cmd->Stream_dest_mac, 6U)!= 0U) { }
                    if (AmbaWrap_memcpy(StreamInfo->Stream_dest_mac, Cmd->Stream_dest_mac, 6U)!= 0U) { }
                }
                AvtpTalker->StreamID.UniqueID = Avdecc_GetUniqueId(Cmd->Talker_entity_id);
                if (AmbaAvbStack_AvtpTalkerReg(AvtpTalker) != NET_ERR_NONE) {
                    AmbaPrint_PrintStr5("%s, AmbaAvbStack_AvtpTalkerReg() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            }
            StreamInfo->Connection_count++;
            StreamInfo->Connected_listeners[i].Listener_entity_id = Cmd->Listener_entity_id;
            StreamInfo->Connected_listeners[i].Listener_unique_id = Cmd->Listener_unique_id;
            StreamInfo->Stream_vlan_id = Cmd->Stream_vlan_id;
            Rval = ACMP_STATUS_SUCCESS;
            break;
        } else {
            Rval = ACMP_STATUS_TALKER_EXCLUSIVE;
        }
    }
    if (AmbaWrap_memcpy(Resp, Cmd, sizeof(ACMP_COMMAND_RESPONSE_s))!= 0U) { }
    return Rval;
}

/* 8.2.2.6.2.3 txResponse(messageType, response, error) */
static UINT32 Talker_TxResponse(UINT8 MsgType, const ACMP_COMMAND_RESPONSE_s *Resp, UINT32 Error)
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
    }
    return Rval;
}

/* 8.2.2.6.2.4 disconnectTalker(command) */
static UINT32 Talker_DisconnectTalker(AVDECC_TALKER_STREAM_INFO_s *StreamInfo, const ACMP_COMMAND_RESPONSE_s *Cmd, ACMP_COMMAND_RESPONSE_s *Resp)
{
    UINT8 i;
    UINT32 Rval = ACMP_STATUS_SUCCESS;
    for (i = 0U; i < Talker.MaxConnectNum; i++) {
        if ((StreamInfo->Connected_listeners[i].Listener_entity_id == Cmd->Listener_entity_id) &&
            (StreamInfo->Connected_listeners[i].Listener_unique_id == Cmd->Listener_unique_id)) {
                StreamInfo->Connected_listeners[i].Listener_entity_id = 0U;
                StreamInfo->Connected_listeners[i].Listener_unique_id = 0U;
                StreamInfo->Connection_count--;
                Rval = ACMP_STATUS_SUCCESS;
                if (StreamInfo->Connection_count == 0U) {
                    AMBA_AVB_AVTP_TALKER_s *AvtpTalker;
                    (void) Avdecc_GetTalkerConfig(Cmd->Talker_entity_id, &AvtpTalker);
                    if (AmbaAvbStack_AvtpTalkerUnReg(AvtpTalker) != NET_ERR_NONE) {
                        AmbaPrint_PrintStr5("%s, AmbaAvbStack_AvtpTalkerReg() failed!", __func__, NULL, NULL, NULL, NULL);
                        Rval = ACMP_STATUS_TALKER_MISBEHAVING;
                    }
                    StreamInfo->Stream_id = 0U;
                    if (AmbaWrap_memset(StreamInfo->Stream_dest_mac, 0, 6U)!= 0U) { }
                    StreamInfo->Stream_vlan_id = 0U;
                }
                break;
        } else {
            Rval = ACMP_STATUS_NOT_CONNECTED;
        }
    }
    if (AmbaWrap_memcpy(Resp, Cmd, sizeof(ACMP_COMMAND_RESPONSE_s))!= 0U) { }
    return Rval;
}

/* 8.2.2.6.2.5 getState(command) */
static UINT32 Talker_GetState(const ACMP_COMMAND_RESPONSE_s *Cmd, const ACMP_COMMAND_RESPONSE_s *Resp)
{
    UINT32 Rval = OK;
    AmbaMisra_TouchUnused(Cmd);
    AmbaMisra_TouchUnused(Resp);
    return Rval;
}

/* 8.2.2.6.2.6 getConnection(command) */
static UINT32 Talker_GetConnection(const ACMP_COMMAND_RESPONSE_s *Cmd, const ACMP_COMMAND_RESPONSE_s *Resp)
{
    UINT32 Rval = OK;
    AmbaMisra_TouchUnused(Cmd);
    AmbaMisra_TouchUnused(Resp);
    return Rval;
}

UINT32 Talker_SendAcmpPacket(const ACMP_PACKET_s *AcmpPkt)
{
    UINT32 Rval;
    ACMP_PACKET_s Msg;
    if (AmbaWrap_memcpy(&Msg, AcmpPkt, sizeof(ACMP_PACKET_s))!= 0U) { }
    Rval = AmbaKAL_MsgQueueSend(&Talker.MsgQueue, &Msg, AMBA_KAL_NO_WAIT);
    if (Rval != KAL_ERR_NONE) {
        AmbaPrint_PrintStr5("%s: cannot send message", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static void Talker_AcmpSm(ACMP_TALKER_SM_s *StateMachine)
{
    switch (StateMachine->State) {
    case TALKER_STATE_WAITING:
        if (StateMachine->DoTerminate == TRUE) {
            // TODO:
        } else {
            if (StateMachine->RcvdConnectTX == TRUE) {
                StateMachine->RcvdConnectTX = FALSE;
                StateMachine->State = TALKER_STATE_CONNECT;
            } else if (StateMachine->RcvdDisconnectTX == TRUE) {
                StateMachine->RcvdDisconnectTX = FALSE;
                StateMachine->State = TALKER_STATE_DISCONNECT;
            } else if (StateMachine->RcvdGetTXState == TRUE) {
                StateMachine->RcvdGetTXState = FALSE;
                StateMachine->State = TALKER_STATE_GET_STATE;
            } else if (StateMachine->RcvdGetTXConnection == TRUE) {
                StateMachine->RcvdGetTXConnection = FALSE;
                StateMachine->State = TALKER_STATE_GET_CONNECTION;
            } else {
                StateMachine->State = TALKER_STATE_WAITING; /* UCT to AVDECC_TALKER_ACMP_STATE_WAITING */
            }
        }
        break;
    case TALKER_STATE_CONNECT:
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
            UINT32 Error;
            if (Talker_ValidTalkerUnique(StateMachine->My_id, StateMachine->RcvdCmdResp.Talker_unique_id) == 1U){
                // TODO: Multiple connection?!
                Error = Talker_ConnectTalker(&StateMachine->TalkerStreamInfos, &StateMachine->RcvdCmdResp, &StateMachine->Resp);
            } else {
                if (AmbaWrap_memcpy(&StateMachine->Resp, &StateMachine->RcvdCmdResp, sizeof(ACMP_COMMAND_RESPONSE_s))!= 0U) { }
                Error = ACMP_STATUS_TALKER_UNKNOWN_ID;
            }
            if (Talker_TxResponse(ACMP_MSG_CONNECT_TX_RESPONSE, &StateMachine->Resp, Error) != OK) {
                AmbaPrint_PrintStr5("%s, Talker_TxResponse(ACMP_MSG_CONNECT_TX_RESPONSE) failed!", __func__, NULL, NULL, NULL, NULL);
            }
            StateMachine->State = TALKER_STATE_WAITING; /* UCT to AVDECC_TALKER_ACMP_STATE_WAITING */
        }
        break;
    case TALKER_STATE_DISCONNECT:
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
            UINT32 Error;
            if (Talker_ValidTalkerUnique(StateMachine->My_id, StateMachine->RcvdCmdResp.Talker_unique_id) == 1U){
                // TODO: Multiple connection?!
                Error = Talker_DisconnectTalker(&StateMachine->TalkerStreamInfos, &StateMachine->RcvdCmdResp, &StateMachine->Resp);
            } else {
                if (AmbaWrap_memcpy(&StateMachine->Resp, &StateMachine->RcvdCmdResp, sizeof(ACMP_COMMAND_RESPONSE_s))!= 0U) { }
                Error = ACMP_STATUS_TALKER_UNKNOWN_ID;
            }
            if (Talker_TxResponse(ACMP_MSG_DISCONNECT_TX_RESPONSE, &StateMachine->Resp, Error) != OK) {
                AmbaPrint_PrintStr5("%s, Talker_TxResponse(ACMP_MSG_CONNECT_TX_RESPONSE) failed!", __func__, NULL, NULL, NULL, NULL);
            }
            StateMachine->State = TALKER_STATE_WAITING; /* UCT to AVDECC_TALKER_ACMP_STATE_WAITING */
        }
        break;
    case TALKER_STATE_GET_STATE:
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
            UINT32 Error;
            if (Talker_ValidTalkerUnique(StateMachine->My_id, StateMachine->RcvdCmdResp.Talker_unique_id) == 1U){
                // TODO: Multiple connection?!
                Error = Talker_GetState(&StateMachine->RcvdCmdResp, &StateMachine->Resp);
            } else {
                if (AmbaWrap_memcpy(&StateMachine->Resp, &StateMachine->RcvdCmdResp, sizeof(ACMP_COMMAND_RESPONSE_s))!= 0U) { }
                Error = ACMP_STATUS_TALKER_UNKNOWN_ID;
            }
            if (Talker_TxResponse(ACMP_MSG_GET_TX_STATE_RESPONSE, &StateMachine->Resp, Error) != OK) {
                AmbaPrint_PrintStr5("%s, Talker_TxResponse(ACMP_MSG_CONNECT_TX_RESPONSE) failed!", __func__, NULL, NULL, NULL, NULL);
            }
            StateMachine->State = TALKER_STATE_WAITING; /* UCT to AVDECC_TALKER_ACMP_STATE_WAITING */
        }
        break;
    case TALKER_STATE_GET_CONNECTION:
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
            UINT32 Error;
            if (Talker_ValidTalkerUnique(StateMachine->My_id, StateMachine->RcvdCmdResp.Talker_unique_id) == 1U){
                // TODO: Multiple connection?!
                Error = Talker_GetConnection(&StateMachine->RcvdCmdResp, &StateMachine->Resp);
            } else {
                if (AmbaWrap_memcpy(&StateMachine->Resp, &StateMachine->RcvdCmdResp, sizeof(ACMP_COMMAND_RESPONSE_s))!= 0U) { }
                Error = ACMP_STATUS_TALKER_UNKNOWN_ID;
            }
            if (Talker_TxResponse(ACMP_MSG_GET_TX_CONNECTION_RESPONSE, &StateMachine->Resp, Error) != OK) {
                AmbaPrint_PrintStr5("%s, Talker_TxResponse(ACMP_MSG_CONNECT_TX_RESPONSE) failed!", __func__, NULL, NULL, NULL, NULL);
            }
            StateMachine->State = TALKER_STATE_WAITING; /* UCT to AVDECC_TALKER_ACMP_STATE_WAITING */
        }
        break;
    default:
        /* TODO */
        break;
    }
}

static void *Talker_TaskEntry(void *Info)
{
    UINT8 i;
    ACMP_PACKET_s Msg = {0};
    (void)Info;
    AmbaPrint_PrintStr5("%s, start", __func__, NULL, NULL, NULL, NULL);
    for (;;) {
        UINT32 Busy = 0U;
        if (AmbaKAL_MutexTake(&Talker.Mutex, AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
            UINT32 Rval = AmbaKAL_MsgQueueReceive(&Talker.MsgQueue, &Msg, 10U);
            for (i = 0U; i < Talker.MaxEntityNum; i++) {
                if (Talker.StateMachine[i].My_id != 0U) {
                    if (Talker.StateMachine[i].State == TALKER_STATE_WAITING) {
                        if (Rval == KAL_ERR_NONE) {
                            if (Msg.Acmpdu.Talker_entity_id == Talker.StateMachine[i].My_id) {
                                Talker.StateMachine[i].RcvdCmdResp.Message_type = Msg.AvtpduHeader.Message_type;
                                Talker.StateMachine[i].RcvdCmdResp.Status = Msg.AvtpduHeader.Status;
                                Talker.StateMachine[i].RcvdCmdResp.Stream_id = Msg.AvtpduHeader.Stream_id;
                                Talker.StateMachine[i].RcvdCmdResp.Controller_entity_id = Msg.Acmpdu.Controller_entity_id;
                                Talker.StateMachine[i].RcvdCmdResp.Talker_entity_id = Msg.Acmpdu.Talker_entity_id;
                                Talker.StateMachine[i].RcvdCmdResp.Listener_entity_id = Msg.Acmpdu.Listener_entity_id;
                                Talker.StateMachine[i].RcvdCmdResp.Talker_unique_id = Msg.Acmpdu.Talker_unique_id;
                                Talker.StateMachine[i].RcvdCmdResp.Listener_unique_id = Msg.Acmpdu.Listener_unique_id;
                                if (AmbaWrap_memcpy(Talker.StateMachine[i].RcvdCmdResp.Stream_dest_mac, Msg.Acmpdu.Stream_dest_mac, 6U)!= 0U) { }
                                Talker.StateMachine[i].RcvdCmdResp.Connection_count = Msg.Acmpdu.Connection_count;
                                Talker.StateMachine[i].RcvdCmdResp.Sequence_id = Msg.Acmpdu.Sequence_id;
                                Talker.StateMachine[i].RcvdCmdResp.Flags = Msg.Acmpdu.Flags;
                                Talker.StateMachine[i].RcvdCmdResp.Stream_vlan_id = Msg.Acmpdu.Stream_vlan_id;
                                switch (Msg.AvtpduHeader.Message_type) {
                                case ACMP_MSG_CONNECT_TX_COMMAND:
                                    Talker.StateMachine[i].RcvdConnectTX = TRUE;
                                    break;
                                case ACMP_MSG_DISCONNECT_TX_COMMAND:
                                    Talker.StateMachine[i].RcvdDisconnectTX = TRUE;
                                    break;
                                case ACMP_MSG_GET_TX_STATE_COMMAND:
                                    Talker.StateMachine[i].RcvdGetTXState = TRUE;
                                    break;
                                case ACMP_MSG_GET_TX_CONNECTION_COMMAND:
                                    Talker.StateMachine[i].RcvdGetTXConnection = TRUE;
                                    break;
                                default:
                                    // TODO:
                                    break;
                                }
                            }
                        }
                    }
                    Talker_AcmpSm(&Talker.StateMachine[i]);
                    if (Talker.StateMachine[i].State > TALKER_STATE_WAITING) {
                        Busy++;
                    }
                }
            }
            if (AmbaKAL_MutexGive(&Talker.Mutex) != KAL_ERR_NONE) {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Busy != 0U) {
            (void) AmbaKAL_TaskSleep(10U);
        }
    }
}

UINT32 Talker_AddEntity(const AVDECC_ENTITY_INFO_s *EntityInfo)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Talker.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        for (i = 0U; i < Talker.MaxEntityNum; i++) {
            if (Talker.StateMachine[i].My_id == 0U) {
                Talker.StateMachine[i].My_id = EntityInfo->EntityDesc.Entity_id;
                break;
            }
        }
        if (AmbaKAL_MutexGive(&Talker.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

UINT32 Talker_RemoveEntity(const AVDECC_ENTITY_INFO_s *EntityInfo)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Talker.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        for (i = 0U; i < Talker.MaxEntityNum; i++) {
            if (Talker.StateMachine[i].My_id == EntityInfo->EntityDesc.Entity_id) {
                Talker.StateMachine[i].My_id = 0U;
                break;
            }
        }
        if (AmbaKAL_MutexGive(&Talker.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

UINT32 Talker_GetRequiredBufferSize(const AVDECC_TALKER_INIT_CFG_s *Cfg, UINT32 *Size)
{
    UINT32 Rval = OK;
    UINT32 BufferSize = 0U;
    BufferSize += (GetAlignedValU32(Cfg->TaskInfo.StackSize, (UINT8)AMBA_CACHE_LINE_SIZE)); /* Task stack */
    BufferSize += (Cfg->MsgNum * sizeof(ACMP_PACKET_s)); /* MsgQ buffer */
    BufferSize += (Cfg->MaxEntityNum * sizeof(ACMP_TALKER_SM_s)); /* StateMachine */
    BufferSize += (Cfg->MaxEntityNum * Cfg->MaxConnectNum * sizeof(AVDECC_LISTENER_PAIR_s)); /* Listener pairs */
    BufferSize = GetAlignedValU32(BufferSize, AMBA_CACHE_LINE_SIZE);
    *Size = BufferSize;
    return Rval;
}

UINT32 Talker_GetDefaultInitCfg(AVDECC_TALKER_INIT_CFG_s *Cfg)
{
    UINT32 Rval;
    if (AmbaWrap_memset(Cfg, 0, sizeof(AVDECC_TALKER_INIT_CFG_s))!= 0U) { }
    Cfg->MaxEntityNum = AVDECC_ENTITY_TALKER_MAX_NUM_DEFAULT;
    Cfg->MaxConnectNum = ACMP_TALKER_MAX_STREAM_INFO_DEFAULT_NUM;
    Cfg->TaskInfo.Priority = TALKER_TASK_PRIORITY_DEFAULT;
    Cfg->TaskInfo.StackSize = TALKER_TASK_STACK_SIZE_DEFAULT;
    Cfg->MsgNum = ACMP_MSG_MAX_NUM_DEFAULT;
    Rval = Talker_GetRequiredBufferSize(Cfg, &Cfg->BufferSize);
    return Rval;
}

static UINT32 Talker_InitImpl(const AVDECC_TALKER_INIT_CFG_s *Cfg)
{
    static char MutexName[] = "AvdeccTalkerMutex";
    static char MsgQueueName[] = "AvdeccTalkerMsgQueue";
    static char TaskName[] = "AvdeccTalker";
    UINT32 Rval = AmbaKAL_MutexCreate(&Talker.Mutex, MutexName);
    if (Rval == KAL_ERR_NONE) {
        Rval = AmbaKAL_MsgQueueCreate(&Talker.MsgQueue, MsgQueueName,
                                     sizeof(ACMP_PACKET_s), Talker.MsgQBuffer,
                                     (Talker.MsgNum * sizeof(ACMP_PACKET_s)));
        if (Rval == KAL_ERR_NONE) {
            Rval = AmbaKAL_TaskCreate(&Talker.Task, TaskName, Cfg->TaskInfo.Priority,
                                     Talker_TaskEntry, NULL, Talker.TaskStack,
                                     Cfg->TaskInfo.StackSize, AMBA_KAL_DONT_START);
            if (Rval == KAL_ERR_NONE) {
                Rval = AmbaKAL_TaskSetSmpAffinity(&Talker.Task, AVDECC_CORE);
                if (Rval == KAL_ERR_NONE) {
                    Rval = AmbaKAL_TaskResume(&Talker.Task);
                    if (Rval != KAL_ERR_NONE) {
                        AmbaPrint_PrintStr5("%s, AmbaKAL_TaskResume(Talker.Task) failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_TaskSetSmpAffinity(Talker.Task) failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_PrintStr5("%s, AmbaKAL_TaskCreate(Talker.Task) failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueCreate(AvdeccTalkerMsgQueue) failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexCreate() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

UINT32 Talker_Init(const AVDECC_TALKER_INIT_CFG_s *Cfg)
{
    UINT32 Rval;
    UINT32 BufferSize;
    Rval = Talker_GetRequiredBufferSize(Cfg, &BufferSize);
    if (Rval == OK) {
        if (BufferSize == Cfg->BufferSize) {
            ULONG Addr;
            AmbaMisra_TypeCast(&Addr, &Cfg->Buffer);
            if ((Addr % AMBA_CACHE_LINE_SIZE) == 0U) {
                UINT8 *Buffer = Cfg->Buffer;
                UINT8 i;
                Talker.MsgNum = ACMP_MSG_MAX_NUM_DEFAULT;
                Talker.MaxEntityNum = Cfg->MaxEntityNum;
                Talker.MaxConnectNum = Cfg->MaxConnectNum;
                Talker.TaskStack = Buffer; /* Task Stack */
                Addr += (GetAlignedValU32(Cfg->TaskInfo.StackSize, (UINT8)AMBA_CACHE_LINE_SIZE));
                AmbaMisra_TypeCast(&Talker.MsgQBuffer, &Addr); /* MsgQ Buffer */
                Addr += (Talker.MsgNum * sizeof(ACMP_PACKET_s));
                AmbaMisra_TypeCast(&Talker.StateMachine, &Addr); /* StateMachine */
                Addr += (Talker.MaxEntityNum * sizeof(ACMP_TALKER_SM_s));
                for (i = 0U; i < Talker.MaxEntityNum; i++) {
                    AmbaMisra_TypeCast(&Talker.StateMachine[i].TalkerStreamInfos.Connected_listeners, &Addr);
                    Addr += (Talker.MaxConnectNum * sizeof(AVDECC_LISTENER_PAIR_s));
                }
                Rval = Talker_InitImpl(Cfg);
            } else {
                AmbaPrint_PrintStr5("%s, Buffer address not alignd!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, BufferSize != Cfg->BufferSize", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

