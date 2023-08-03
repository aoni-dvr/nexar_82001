/**
 * @file acmp.h
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
#ifndef ACMP_H
#define ACMP_H

#define ACMP_MSG_MAX_NUM_DEFAULT            (8U)

/* Clause 8 of IEEE Std 1722.1 - 2013 */
#define ACMP_CD_LEN                         (44U)

/* 8.2.1.5 - Table 8.1 - message_type field */
#define ACMP_MSG_CONNECT_TX_COMMAND         (0U)
#define ACMP_MSG_CONNECT_TX_RESPONSE        (1U)
#define ACMP_MSG_DISCONNECT_TX_COMMAND      (2U)
#define ACMP_MSG_DISCONNECT_TX_RESPONSE     (3U)
#define ACMP_MSG_GET_TX_STATE_COMMAND       (4U)
#define ACMP_MSG_GET_TX_STATE_RESPONSE      (5U)
#define ACMP_MSG_CONNECT_RX_COMMAND         (6U)
#define ACMP_MSG_CONNECT_RX_RESPONSE        (7U)
#define ACMP_MSG_DISCONNECT_RX_COMMAND      (8U)
#define ACMP_MSG_DISCONNECT_RX_RESPONSE     (9U)
#define ACMP_MSG_GET_RX_STATE_COMMAND       (10U)
#define ACMP_MSG_GET_RX_STATE_RESPONSE      (11U)
#define ACMP_MSG_GET_TX_CONNECTION_COMMAND  (12U)
#define ACMP_MSG_GET_TX_CONNECTION_RESPONSE (13U)

/* 8.2.1.6 - Table 8.2 - status field */
#define ACMP_STATUS_SUCCESS                     (0U)
#define ACMP_STATUS_LISTENER_UNKNOWN_ID         (1U)
#define ACMP_STATUS_TALKER_UNKNOWN_ID           (2U)
#define ACMP_STATUS_TALKER_DEST_MAC_FAIL        (3U)
#define ACMP_STATUS_TALKER_NO_STREAM_INDEX      (4U)
#define ACMP_STATUS_TALKER_NO_BANDWIDTH         (5U)
#define ACMP_STATUS_TALKER_EXCLUSIVE            (6U)
#define ACMP_STATUS_LISTENER_TALKER_TIMEOUT     (7U)
#define ACMP_STATUS_LISTENER_EXCLUSIVE          (8U)
#define ACMP_STATUS_STATE_UNAVAILABLE           (9U)
#define ACMP_STATUS_NOT_CONNECTED               (10U)
#define ACMP_STATUS_NO_SUCH_CONNECTION          (11U)
#define ACMP_STATUS_COULD_NOT_SEND_MESSAGE      (12U)
#define ACMP_STATUS_TALKER_MISBEHAVING          (13U)
#define ACMP_STATUS_LISTENER_MISBEHAVING        (14U)
#define ACMP_STATUS_CONTROLLER_NOT_AUTHORIZED   (16U)
#define ACMP_STATUS_INCOMPATIBLE_REQUEST        (17U)
#define ACMP_STATUS_NOT_SUPPORTED               (31U)

/* The IEEE Std 1722-2011 control AVTPDU */
typedef struct {
    UINT32 Control_data_len : 11;   /**< 8.2.1.7 Always to be 44, CD_LEN_ACMP */
    UINT32 Status : 5;              /**< 8.2.1.6 - Table 8.2 */
    UINT32 Message_type : 4;        /**< 8.2.1.5 - Table 8.1 */
    UINT32 Version : 3;             /**< 8.2.1.4 Always set to 0 */
    UINT32 Sv : 1;                  /**< 8.2.1.3 StreamID Valid indicator: Always set to 0 */
    UINT32 Subtype : 8;             /**< 8.2.1.2 Always set to 0x7C */
    UINT64 Stream_id;               /**< 8.2.1.8 */
} __attribute__((aligned(4), packed)) ACMP_AVTPDU_HEADER_s;

/* 8.2.1 AVDECC Connection Management Protocol Data Unit (ACMPDU) Format */
/* 8.2.1.17 - Table 8.3 - flags field */
#define ACMP_FLG_CLASS_B            (0x0001U)
#define ACMP_FLG_FAST_CONNECT       (0x0002U)
#define ACMP_FLG_SAVED_STATE        (0x0004U)
#define ACMP_FLG_STREAMING_WAIT     (0x0008U)
#define ACMP_FLG_SUPPORTS_ENCRYPTED (0x0010U)
#define ACMP_FLG_ENCRYPTED_PDU      (0x0020U)
#define ACMP_FLG_TALKER_FAILED      (0x0040U)

/* Figure 8.1 - ACMPDU format*/
typedef struct {
    UINT64 Controller_entity_id;    /**< 8.2.1.9 */
    UINT64 Talker_entity_id;        /**< 8.2.1.10 */
    UINT64 Listener_entity_id;      /**< 8.2.1.11 */
    UINT16 Talker_unique_id;        /**< 8.2.1.12 */
    UINT16 Listener_unique_id;      /**< 8.2.1.13 */
    UINT8  Stream_dest_mac[6U];     /**< 8.2.1.14 */
    UINT16 Connection_count;        /**< 8.2.1.15 */
    UINT16 Sequence_id;             /**< 8.2.1.16 */
    UINT16 Flags;                   /**< 8.2.1.17 */
    UINT16 Stream_vlan_id;          /**< 8.2.1.18 */
    UINT16 Reserved;
} __attribute__((aligned(1), packed)) ACMPDU_s;

typedef struct {
    ACMP_AVTPDU_HEADER_s AvtpduHeader;
    ACMPDU_s Acmpdu;
} ACMP_PACKET_s;

/* Table 8.4 - ACMP command timeouts(ms) */
#define ACMP_CMD_TIMEOUT_CONNECT_TX_COMMAND         (2000U)
#define ACMP_CMD_TIMEOUT_DISCONNECT_TX_COMMAND      (200U)
#define ACMP_CMD_TIMEOUT_GET_TX_STATE_COMMAND       (200U)
#define ACMP_CMD_TIMEOUT_CONNECT_RX_COMMAND         (4500U)
#define ACMP_CMD_TIMEOUT_DISCONNECT_RX_COMMAND      (500U)
#define ACMP_CMD_TIMEOUT_GET_RX_STATE_COMMAND       (200U)
#define ACMP_CMD_TIMEOUT_GET_TX_CONNECTION_COMMAND  (200U)

/* 8.2.2.2.1 ACMPCommandResponse */
typedef struct {
    UINT32 Message_type:4U;
    UINT32 Status:5U;
    UINT64 Stream_id;
    UINT64 Controller_entity_id;
    UINT64 Talker_entity_id;
    UINT64 Listener_entity_id;
    UINT16 Talker_unique_id;
    UINT16 Listener_unique_id;
    UINT8 Stream_dest_mac[6U];
    UINT16 Connection_count;
    UINT16 Sequence_id;
    UINT16 Flags;
    UINT16 Stream_vlan_id;
} ACMP_COMMAND_RESPONSE_s;

/* 8.2.2.2.2 ListenerStreamInfo */
typedef struct {
    UINT64 Talker_entity_id;
    UINT16 Talker_unique_id;
    UINT8 Connected;
    UINT64 Stream_id;
    UINT8 Stream_dest_mac[6U];
    UINT64 Controller_entity_id;
    UINT16 Flags;
    UINT16 Stream_vlan_id;
} AVDECC_LISTENER_STREAM_INFO_s;

/* 8.2.2.2.3 ListenerPair */
typedef struct {
    UINT64 Listener_entity_id;
    UINT16 Listener_unique_id;
} AVDECC_LISTENER_PAIR_s;

/* 8.2.2.2.4 TalkerStreamInfo */
#define ACMP_TALKER_MAX_STREAM_INFO_DEFAULT_NUM (64)
typedef struct {
    UINT64 Stream_id;
    UINT8 Stream_dest_mac[6U];
    UINT16 Connection_count;
    AVDECC_LISTENER_PAIR_s *Connected_listeners;
    UINT16 Stream_vlan_id;
} AVDECC_TALKER_STREAM_INFO_s;

/* 8.2.2.2.5 InflightCommand, cf. 9.2.2.3.2.1.2 InflightCommand */
#define ACMP_INFLIGHT_CMD_DEFAULT_NUM   (5U)
typedef struct ACMP_INFLIGHT_CMD_t {
    UINT32 Timeout;                     /**< A timer (timeout value) for when the command will timeout */
    UINT8 Retried;                      /**< 1 bit (boolean) indicating if a retry has been sent */
    ACMP_COMMAND_RESPONSE_s Cmd;        /**< Command inflight */
    UINT16 Original_sequence_id;        /**< 16 bits, the sequence_id which was used when the command was sent */
    struct ACMP_INFLIGHT_CMD_t *Next;
} ACMP_INFLIGHT_CMD_s;

/* 8.2.2.2.6 ACMPCommandParams */
typedef struct {
    UINT32 Message_type:4U;     /**< 8.2.1.5 - Table 8.1 */
    UINT64 Talker_entity_id;
    UINT64 Listener_entity_id;
    UINT16 Talker_unique_id;
    UINT16 Listener_unique_id;
    UINT16 Connection_count;
    UINT16 Flags;
    UINT16 Stream_vlan_id;
} ACMP_COMMAND_PARAMS_s;

/* 8.2.2.4.1 State machine variables */
typedef struct {
    UINT64 My_id;                           /**< 8.2.2.3.1 */
    ACMP_COMMAND_RESPONSE_s RcvdCmdResp;    /**< 8.2.2.3.2 */
    ACMP_INFLIGHT_CMD_s *Inflight;          /**< 8.2.2.4.1.1 */
    UINT8 RcvdResponse;                     /**< 8.2.2.4.1.2 */
    UINT8 DoCommand;
    ACMP_COMMAND_RESPONSE_s CmdParams;
    UINT8 DoTerminate;
    UINT16 Sequence_id;                     /**< Used for ACMP, 8.2.1.8 stream_id field */
    UINT8 State;
} ACMP_CONTROLLER_SM_s;

/* 8.2.2.5.1 State machine variables */
typedef struct {
    UINT64 My_id;                                       /**< 8.2.2.3.1 */
    ACMP_COMMAND_RESPONSE_s RcvdCmdResp;                /**< 8.2.2.3.2 */
    ACMP_INFLIGHT_CMD_s *Inflight;                      /**< 8.2.2.5.1.1 */
    AVDECC_LISTENER_STREAM_INFO_s ListenerStreamInfos;  /**< 8.2.2.5.1.2 */
    UINT8 RcvdConnectRXCmd;                             /**< 8.2.2.5.1.3 */
    UINT8 RcvdDisconnectRXCmd;                          /**< 8.2.2.5.1.4 */
    UINT8 RcvdConnectTXResp;                            /**< 8.2.2.5.1.5 */
    UINT8 RcvdDisconnectTXResp;                         /**< 8.2.2.5.1.6 */
    UINT8 RcvdGetRXState;                               /**< 8.2.2.5.1.7 */
    UINT8 State;
    UINT16 Sequence_id;                                 /**< Used for ACMP, 8.2.1.8 stream_id field */
    ACMP_COMMAND_RESPONSE_s Resp;
    UINT8 DoTerminate;
} ACMP_LISTENER_SM_s;

/* 8.2.2.6.1 State machine variables */
typedef struct {
    UINT64 My_id;                                   /**< 8.2.2.3.1 */
    ACMP_COMMAND_RESPONSE_s RcvdCmdResp;            /**< 8.2.2.3.2 */
    AVDECC_TALKER_STREAM_INFO_s TalkerStreamInfos;  /**< 8.2.2.6.1.1 */
    UINT8 RcvdConnectTX;                            /**< 8.2.2.6.1.2 */
    UINT8 RcvdDisconnectTX;                         /**< 8.2.2.6.1.3 */
    UINT8 RcvdGetTXState;                           /**< 8.2.2.6.1.4 */
    UINT8 RcvdGetTXConnection;                      /**< 8.2.2.6.1.5 */
    UINT8 State;
    UINT16 Sequence_id;                             /**< Used for ACMP, 8.2.1.8 stream_id field */
    ACMP_COMMAND_RESPONSE_s Resp;
    UINT8 DoTerminate;
} ACMP_TALKER_SM_s;

ACMP_INFLIGHT_CMD_s *Acmp_AddInflightCmd(ACMP_INFLIGHT_CMD_s *Head, ACMP_COMMAND_RESPONSE_s Cmd, UINT32 Timeout);
ACMP_INFLIGHT_CMD_s *Acmp_RemoveInflightCmd(ACMP_INFLIGHT_CMD_s *Head, UINT16 SequenceId);
ACMP_INFLIGHT_CMD_s *Acmp_InflightCmdTimeout(ACMP_INFLIGHT_CMD_s *Head);
UINT32 Acmp_GetInflightCmdId(const ACMP_INFLIGHT_CMD_s *Head, const ACMP_COMMAND_RESPONSE_s *Resp, UINT8 MsgType, UINT16 *SequenceId);
UINT32 Acmp_GetCmdTimeoutTime(UINT32 Cmd);
#endif /* ACMP_H */

