/**
 * @file acmp.c
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

#define ACMP_DBG (0)

/**
 * The internal variables of ACMP
 */
typedef struct {
    UINT32 InflightCmdNum;                  /**< number of inflight commands */
    ACMP_INFLIGHT_CMD_s *InflightBuffer;    /**< buffer for inflight commands */
    ACMP_INFLIGHT_CMD_s *InflightCmd;       /**< pointer of inflight commands */
} AVDECC_ACMP_s;

static AVDECC_ACMP_s Acmp = {0};    /**< Acmp manager */

/**
 *  Acmp inflight command initial
 *  @return 0 - OK, others - Error
 */
static UINT32 Acmp_InflightCmdInit(void)
{
    UINT8 i;
    UINT32 Rval = OK;
    for (i = 0U; i < (Acmp.InflightCmdNum - 1U); i++) {
        Acmp.InflightCmd[i].Retried = FALSE;
        Acmp.InflightCmd[i].Next = &Acmp.InflightCmd[i + 1U];
    }
    Acmp.InflightCmd[i].Retried = FALSE;
    Acmp.InflightCmd[i].Next = NULL;
    return Rval;
}

/**
 *  Get a new inflight command
 *  @return a pointer of inflight command
 */
static ACMP_INFLIGHT_CMD_s *Acmp_NewInflightCmd(void)
{
    ACMP_INFLIGHT_CMD_s *InflightCmd = Acmp.InflightCmd;
    if (Acmp.InflightCmd != NULL) {
        Acmp.InflightCmd = Acmp.InflightCmd->Next;
    }
    return InflightCmd;
}

/**
 *  Release a inflight command
 *  @param [in] InflightCmd The pointer use to release inflight command
 *  @return 0 - OK, others - Error
 */
static void Acmp_FreeInflightCmd(ACMP_INFLIGHT_CMD_s *InflightCmd)
{
    if (InflightCmd != NULL) {
        InflightCmd->Next = Acmp.InflightCmd;
        Acmp.InflightCmd = InflightCmd;
    }
}

/**
 *  Add a inflight command to inflight command buffer
 *  @param [in] Head    The pointer of inflight command buffer
 *  @param [in] Cmd     The inflight command
 *  @param [in] Timeout The timeout time of the inflight command
 *  @return 0 - OK, others - Error
 */
ACMP_INFLIGHT_CMD_s *Acmp_AddInflightCmd(ACMP_INFLIGHT_CMD_s *Head, ACMP_COMMAND_RESPONSE_s Cmd, UINT32 Timeout)
{
    ACMP_INFLIGHT_CMD_s *InflightCmd;
    InflightCmd = Acmp_NewInflightCmd();
    if (InflightCmd != NULL) {
        UINT32 CurrentTime;
        (void)AmbaKAL_GetSysTickCount(&CurrentTime);
        InflightCmd->Timeout = CurrentTime + Timeout;
        InflightCmd->Cmd = Cmd;
        InflightCmd->Original_sequence_id = InflightCmd->Cmd.Sequence_id;
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
ACMP_INFLIGHT_CMD_s *Acmp_RemoveInflightCmd(ACMP_INFLIGHT_CMD_s *Head, UINT16 SequenceId)
{
    ACMP_INFLIGHT_CMD_s *InflightCmd = NULL;
    ACMP_INFLIGHT_CMD_s *PreInflightCmd = NULL;
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
                Acmp_FreeInflightCmd(Head);
            } else {
                PreInflightCmd->Next = InflightCmd->Next;
                Acmp_FreeInflightCmd(InflightCmd);
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
ACMP_INFLIGHT_CMD_s *Acmp_InflightCmdTimeout(ACMP_INFLIGHT_CMD_s *Head)
{
    ACMP_INFLIGHT_CMD_s *InflightCmd = Head;
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
 *  Get sequence id of Acmp response data
 *  @param [in]  Head       The pointer of inflight command buffer
 *  @param [in]  Resp       The Acmp response data
 *  @param [in]  MsgType    The message type
 *  @param [out] SequenceId The sequence id of the Acmp response data
 *  @return 0 - OK, others - Error
 */
UINT32 Acmp_GetInflightCmdId(const ACMP_INFLIGHT_CMD_s *Head, const ACMP_COMMAND_RESPONSE_s *Resp, UINT8 MsgType, UINT16 *SequenceId)
{
    UINT32 Rval = OK;
    const ACMP_INFLIGHT_CMD_s *InflightCmd = Head;
    if (InflightCmd != NULL) {
        UINT8 Searched = 0U;
        while (InflightCmd != NULL) {
            if ((InflightCmd->Cmd.Message_type == MsgType) &&
                (InflightCmd->Cmd.Controller_entity_id == Resp->Controller_entity_id) &&
                (InflightCmd->Cmd.Listener_entity_id == Resp->Listener_entity_id) &&
                (InflightCmd->Cmd.Talker_entity_id == Resp->Talker_entity_id) ){
                Searched = 1U;
                break;
            }
            InflightCmd = InflightCmd->Next;
        }
        if (Searched == 1U) {
            if (InflightCmd != NULL) {
                *SequenceId = InflightCmd->Original_sequence_id;
            }
        } else {
            Rval = ERR_ARG;
        }
    }
    return Rval;
}

/**
 *  Get timeout time of Acmp command
 *  @param [in]  Cmd The command to get timeout time
 *  @return timeout time
 */
UINT32 Acmp_GetCmdTimeoutTime(UINT32 Cmd)
{
    UINT32 TimeoutTime = 0U;
    switch (Cmd) {
    case ACMP_MSG_CONNECT_TX_COMMAND :
        TimeoutTime = ACMP_CMD_TIMEOUT_CONNECT_TX_COMMAND;
        break;
    case ACMP_MSG_DISCONNECT_TX_COMMAND :
        TimeoutTime = ACMP_CMD_TIMEOUT_DISCONNECT_TX_COMMAND;
        break;
    case ACMP_MSG_GET_TX_STATE_COMMAND :
        TimeoutTime = ACMP_CMD_TIMEOUT_GET_TX_STATE_COMMAND;
        break;
    case ACMP_MSG_CONNECT_RX_COMMAND :
        TimeoutTime = ACMP_CMD_TIMEOUT_CONNECT_RX_COMMAND;
        break;
    case ACMP_MSG_DISCONNECT_RX_COMMAND :
        TimeoutTime = ACMP_CMD_TIMEOUT_DISCONNECT_RX_COMMAND;
        break;
    case ACMP_MSG_GET_RX_STATE_COMMAND :
        TimeoutTime = ACMP_CMD_TIMEOUT_GET_RX_STATE_COMMAND;
        break;
    case ACMP_MSG_GET_TX_CONNECTION_COMMAND :
        TimeoutTime = ACMP_CMD_TIMEOUT_GET_TX_CONNECTION_COMMAND;
        break;
    default :
        TimeoutTime = 250U;
        break;
    }
    return TimeoutTime;
}

/**
 *  Prepare command of Acmp for transmit
 *  @param [in]  MsgType The message type
 *  @param [out] AcmpPkt The Acmp packet
 *  @param [in]  Status  The status of Avtp header of Acmp command
 *  @return 0 - OK, others - Error
 */
UINT32 Acmp_PrepareTxCmd(UINT8 MsgType, ACMP_PACKET_s *AcmpPkt, UINT32 Status)
{
    UINT32 Rval = OK;
    if (MsgType <= ACMP_MSG_GET_TX_CONNECTION_RESPONSE) { /* AVTP Format */
        AcmpPkt->AvtpduHeader.Message_type = MsgType;
    } else {
        Rval = ERR_ARG;
    }
    if (Rval == OK) {
        AcmpPkt->AvtpduHeader.Control_data_len = ACMP_CD_LEN;
        AcmpPkt->AvtpduHeader.Status = (UINT8)Status;
        AcmpPkt->AvtpduHeader.Version = 0;
        AcmpPkt->AvtpduHeader.Sv = 0;
        AcmpPkt->AvtpduHeader.Subtype = AVTPDU_SUBTYPE_ACMP;
        {
            const void *Vptr = &(AcmpPkt->AvtpduHeader);
            UINT32 *Ptr;
            AmbaMisra_TypeCast(&Ptr, &Vptr);
            *Ptr = AVDECC_HTONL(*Ptr);
        }
    }
    return Rval;
}

/**
 *  Convert values between host and network byte order
 *  @param [in] AcmpPkt The Acmp packet
 */
static void Acmp_Ntoh(ACMP_PACKET_s *AcmpPkt)
{
    UINT32 *Ptr;
    const void *AvtpHeaderPtr = &AcmpPkt->AvtpduHeader;
    AmbaMisra_TypeCast(&Ptr, &AvtpHeaderPtr);
    *Ptr = AVDECC_NTOHL(*Ptr);
    AcmpPkt->AvtpduHeader.Stream_id = AVDECC_NTOHLL(AcmpPkt->AvtpduHeader.Stream_id);
    AcmpPkt->Acmpdu.Controller_entity_id = AVDECC_NTOHLL(AcmpPkt->Acmpdu.Controller_entity_id);
    AcmpPkt->Acmpdu.Talker_entity_id = AVDECC_NTOHLL(AcmpPkt->Acmpdu.Talker_entity_id);
    AcmpPkt->Acmpdu.Listener_entity_id = AVDECC_NTOHLL(AcmpPkt->Acmpdu.Listener_entity_id);
    AcmpPkt->Acmpdu.Listener_unique_id = AVDECC_NTOHS(AcmpPkt->Acmpdu.Listener_unique_id);
    AcmpPkt->Acmpdu.Talker_unique_id = AVDECC_NTOHS(AcmpPkt->Acmpdu.Talker_unique_id);
    AcmpPkt->Acmpdu.Connection_count = AVDECC_NTOHS(AcmpPkt->Acmpdu.Connection_count);
    AcmpPkt->Acmpdu.Flags = AVDECC_NTOHS(AcmpPkt->Acmpdu.Flags);
    AcmpPkt->Acmpdu.Sequence_id = AVDECC_NTOHS(AcmpPkt->Acmpdu.Sequence_id);
    AcmpPkt->Acmpdu.Stream_vlan_id = AVDECC_NTOHS(AcmpPkt->Acmpdu.Stream_vlan_id);
#if ACMP_DBG
    {
        UINT32 Tmp1, Tmp2;
        AmbaPrint_PrintUInt5("Control_data_len: %u", AcmpPkt->AvtpduHeader.Control_data_len, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Status: %u", AcmpPkt->AvtpduHeader.Status, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Message_type: %u", AcmpPkt->AvtpduHeader.Message_type, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Version: %u", AcmpPkt->AvtpduHeader.Version, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Sv: %u", AcmpPkt->AvtpduHeader.Sv, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Subtype: %u", AcmpPkt->AvtpduHeader.Subtype, 0U, 0U, 0U, 0U);
        Tmp1 = ((AcmpPkt->AvtpduHeader.Stream_id & 0xFFFFFFFF00000000U) >> 32U);
        Tmp2 = ((AcmpPkt->AvtpduHeader.Stream_id & 0x00000000FFFFFFFFU));
        AmbaPrint_PrintUInt5("Stream_id: %X%X", Tmp1, Tmp2, 0U, 0U, 0U);
        Tmp1 = ((AcmpPkt->Acmpdu.Controller_entity_id & 0xFFFFFFFF00000000U) >> 32U);
        Tmp2 = ((AcmpPkt->Acmpdu.Controller_entity_id & 0x00000000FFFFFFFFU));
        AmbaPrint_PrintUInt5("Controller_entity_id: %X%X", Tmp1, Tmp2, 0U, 0U, 0U);
        Tmp1 = ((AcmpPkt->Acmpdu.Talker_entity_id & 0xFFFFFFFF00000000U) >> 32U);
        Tmp2 = ((AcmpPkt->Acmpdu.Talker_entity_id & 0x00000000FFFFFFFFU));
        AmbaPrint_PrintUInt5("Talker_entity_id: %X%X", Tmp1, Tmp2, 0U, 0U, 0U);
        Tmp1 = ((AcmpPkt->Acmpdu.Listener_entity_id & 0xFFFFFFFF00000000U) >> 32U);
        Tmp2 = ((AcmpPkt->Acmpdu.Listener_entity_id & 0x00000000FFFFFFFFU));
        AmbaPrint_PrintUInt5("Listener_entity_id: %X%X", Tmp1, Tmp2, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Listener_unique_id: %X", AcmpPkt->Acmpdu.Listener_unique_id, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Talker_unique_id: %X", AcmpPkt->Acmpdu.Talker_unique_id, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Connection_count: %X", AcmpPkt->Acmpdu.Connection_count, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Stream_dest_mac[0:4]: 0x%X%X%X%X%X", AcmpPkt->Acmpdu.Stream_dest_mac[0],
                                                                      AcmpPkt->Acmpdu.Stream_dest_mac[1],
                                                                      AcmpPkt->Acmpdu.Stream_dest_mac[2],
                                                                      AcmpPkt->Acmpdu.Stream_dest_mac[3],
                                                                      AcmpPkt->Acmpdu.Stream_dest_mac[4]);
        AmbaPrint_PrintUInt5("Stream_dest_mac[5]: 0x%X%X%X%X%X", AcmpPkt->Acmpdu.Stream_dest_mac[5], 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Flags: 0x%X", AcmpPkt->Acmpdu.Flags, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Sequence_id: 0x%X", AcmpPkt->Acmpdu.Sequence_id, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Stream_vlan_id: 0x%X", AcmpPkt->Acmpdu.Stream_vlan_id, 0U, 0U, 0U, 0U);
    }
#endif
}

/**
 *  Process Acmp packet
 *  @param [in] AcmpPkt The pointer of Aecp packet
 *  @return 0 - OK, others - Error
 */
UINT32 Acmp_Process(ACMP_PACKET_s *AcmpPkt)
{
    UINT32 Rval = OK;
    Acmp_Ntoh(AcmpPkt);
    switch (AcmpPkt->AvtpduHeader.Message_type) { /* Table 8.1 Message_type field */
    /* Listener */
    case ACMP_MSG_CONNECT_TX_RESPONSE:
    case ACMP_MSG_DISCONNECT_TX_RESPONSE:
    case ACMP_MSG_CONNECT_RX_COMMAND:
    case ACMP_MSG_DISCONNECT_RX_COMMAND:
    case ACMP_MSG_GET_RX_STATE_COMMAND:
        {
            UINT32 ListenerNum;
            Rval = Avdecc_GetEntityNum(AVDECC_LISTENER, &ListenerNum);
            if (Rval == OK) {
                if (ListenerNum > 0U) {
                    Rval = Listener_SendAcmpPacket(AcmpPkt);
                }
            }
        }
        break;
    /* Talker */
    case ACMP_MSG_CONNECT_TX_COMMAND:
    case ACMP_MSG_DISCONNECT_TX_COMMAND:
    case ACMP_MSG_GET_TX_STATE_COMMAND:
    case ACMP_MSG_GET_TX_CONNECTION_COMMAND:
        {
            UINT32 TalkerNum;
            Rval = Avdecc_GetEntityNum(AVDECC_TALKER, &TalkerNum);
            if (Rval == OK) {
                if (TalkerNum > 0U) {
                    Rval = Talker_SendAcmpPacket(AcmpPkt);
                }
            }
        }
        break;
    /* Controller */
    case ACMP_MSG_GET_TX_STATE_RESPONSE:
    case ACMP_MSG_CONNECT_RX_RESPONSE:
    case ACMP_MSG_DISCONNECT_RX_RESPONSE:
    case ACMP_MSG_GET_RX_STATE_RESPONSE:
    case ACMP_MSG_GET_TX_CONNECTION_RESPONSE:
        {
            UINT32 ControllerNum;
            Rval = Avdecc_GetEntityNum(AVDECC_CONTROLLER, &ControllerNum);
            if (Rval == OK) {
                if (ControllerNum > 0U) {
                    Rval = Ctrller_SendAcmpPacket(AcmpPkt);
                }
            }
        }
        break;
    default:
        /* TODO */
        break;
    }
    return Rval;
}

UINT32 Acmp_GetRequiredBufferSize(const AVDECC_ACMP_INIT_CFG_s *Cfg, UINT32 *Size)
{
    UINT32 Rval = OK;
    *Size = GetAlignedValU32((Cfg->InflightCmdNum * sizeof(ACMP_INFLIGHT_CMD_s)), AMBA_CACHE_LINE_SIZE);
    return Rval;
}

UINT32 Acmp_GetDefaultInitCfg(AVDECC_ACMP_INIT_CFG_s *Cfg)
{
    UINT32 Rval;
    if (AmbaWrap_memset(Cfg, 0, sizeof(AVDECC_ACMP_INIT_CFG_s))!= 0U) { }
    Cfg->InflightCmdNum = ACMP_INFLIGHT_CMD_DEFAULT_NUM;
    Rval = Acmp_GetRequiredBufferSize(Cfg, &Cfg->BufferSize);
    return Rval;
}

UINT32 Acmp_Init(const AVDECC_ACMP_INIT_CFG_s *Cfg)
{
    UINT32 Rval;
    UINT32 BufferSize;
    Rval = Acmp_GetRequiredBufferSize(Cfg, &BufferSize);
    if (Rval == OK) {
        if (BufferSize == Cfg->BufferSize) {
            ULONG Addr;
            AmbaMisra_TypeCast(&Addr, &Cfg->Buffer);
            if ((Addr % AMBA_CACHE_LINE_SIZE) == 0U) {
                AmbaMisra_TypeCast(&Acmp.InflightBuffer, &Cfg->Buffer);
                Acmp.InflightCmd = Acmp.InflightBuffer;
                Acmp.InflightCmdNum = Cfg->InflightCmdNum;
                Rval = Acmp_InflightCmdInit();
            } else {
                AmbaPrint_PrintStr5("%s, Buffer address not alignd!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, BufferSize != Cfg->BufferSize", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

