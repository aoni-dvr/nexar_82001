/**
 * @file aecp.h
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
#ifndef AECP_H
#define AECP_H

#define AVDECC_CMD_TIMEOUT   (250U) /**< 9.2.1.2.5 AVDECC Command Timeouts */

/* 9.2.1.6.7 HDCP APM Message Timeouts */

/* Clause 9 of IEEE Std 1722.1 - 2013 */
/* 9.2.1.1.5 - Table 9.1 - message_type field */
#define AECP_MSG_AEM_COMMAND             (0U)   /**< Aecp message : AEM_COMMAND */
#define AECP_MSG_AEM_RESPONSE            (1U)   /**< Aecp message : AEM_RESPONSE */
#define AECP_MSG_ADDRESS_ACCESS_COMMAND  (2U)   /**< Aecp message : ADDRESS_ACCESS_COMMAND */
#define AECP_MSG_ADDRESS_ACCESS_RESPONSE (3U)   /**< Aecp message : ADDRESS_ACCESS_RESPONSE */
#define AECP_MSG_AVC_COMMAND             (4U)   /**< Aecp message : AVC_COMMAND */
#define AECP_MSG_AVC_RESPONSE            (5U)   /**< Aecp message : AVC_RESPONSE */
#define AECP_MSG_VENDOR_UNIQUE_COMMAND   (6U)   /**< Aecp message : VENDOR_UNIQUE_COMMAND */
#define AECP_MSG_VENDOR_UNIQUE_RESPONSE  (7U)   /**< Aecp message : VENDOR_UNIQUE_RESPONSE */
#define AECP_MSG_HDCP_APM_COMMAND        (8U)   /**< Aecp message : HDCP_APM_COMMAND */
#define AECP_MSG_HDCP_APM_RESPONSE       (9U)   /**< Aecp message : HDCP_APM_RESPONSE */
#define AECP_MSG_EXTENDED_COMMAND        (14U)  /**< Aecp message : EXTENDED_COMMAND */
#define AECP_MSG_EXTENDED_RESPONSE       (15U)  /**< Aecp message : EXTENDED_RESPONSE */

/* 9.2.1.1.6 - Table 9.2, Table 7.126 status field */
#define AECP_STATUS_SUCCESS                 (0U)    /**< Aecp status : SUCCESS */
#define AECP_STATUS_NOT_IMPLEMENTED         (1U)    /**< Aecp status : NOT_IMPLEMENTED */
#define AECP_STATUS_NO_SUCH_DESCRIPTOR      (2U)    /**< Aecp status : NO_SUCH_DESCRIPTOR */
#define AECP_STATUS_ENTITY_LOCKED           (3U)    /**< Aecp status : ENTITY_LOCKED */
#define AECP_STATUS_ENTITY_ACQUIRED         (4U)    /**< Aecp status : ENTITY_ACQUIRED  */
#define AECP_STATUS_NOT_AUTHENTICATED       (5U)    /**< Aecp status : NOT_AUTHENTICATED */
#define AECP_STATUS_AUTHENTICATION_DISABLED (6U)    /**< Aecp status : AUTHENTICATION_DISABLED */
#define AECP_STATUS_BAD_ARGUMENTS           (7U)    /**< Aecp status : BAD_ARGUMENTS */
#define AECP_STATUS_NO_RESOURCES            (8U)    /**< Aecp status : NO_RESOURCES */
#define AECP_STATUS_IN_PROGRESS             (9U)    /**< Aecp status : IN_PROGRESS */
#define AECP_STATUS_ENTITY_MISBEHAVING      (10U)   /**< Aecp status : ENTITY_MISBEHAVING */
#define AECP_STATUS_NOT_SUPPORTED           (11U)   /**< Aecp status : NOT_SUPPORTED */
#define AECP_STATUS_STREAM_IS_RUNNING       (12U)   /**< Aecp status : STREAM_IS_RUNNING */

/**
 * Data type of AVTPDU
 */
typedef struct {
    UINT32 Control_data_len : 11;   /**< 9.2.1.1.7 octets following the Target_entity_id, max. 524 */
    UINT32 Status : 5;              /**< 9.2.1.1.6 - Table 9.1 */
    UINT32 Message_type : 4;        /**< 9.2.1.1.5 AECP_MSG_ENTITY_XXX, control_data field */
    UINT32 Version : 3;             /**< 9.2.1.1.4 Always set to 0 */
    UINT32 Sv : 1;                  /**< 9.2.1.1.3 StreamID Valid indicator: Always set to 0 */
    UINT32 Subtype : 8;             /**< 9.2.1.1.2 Always set to 0x7B */
    UINT64 Target_entity_id;        /**< 9.2.1.1.8 stream_id field */
} __attribute__((packed)) AECP_AVTPDU_HEADER_s;

/**
 * Data type of AECPDU
 * 9.2.1 AVDECC Enumeration and Control Protocol Data Unit format
 * Figure 9.1 - AECPDU common format
 */
typedef struct {
    UINT64 Controller_entity_id;    /**< 9.2.1.1.9 */
    UINT16 Sequence_id;             /**< 9.2.1.1.10 */
} __attribute__((aligned(1), packed)) AECPDU_s;

/**
 * Data type of Aecp packet
 */
typedef struct {
    AECP_AVTPDU_HEADER_s AvtpduHeader;  /**< Avtpdu */
    AECPDU_s Aecpdu;                    /**< Aecpdu */
} __attribute__((aligned(1), packed)) AECP_PACKET_s;

/**
 * Data type of AEM command and response
 */
typedef struct {
    UINT8 Data[32U];    /**< Data */
} AEM_CMD_RESP_s;

/**
 * 9.2.2.3.1.2 State Machine Variables
 */
typedef struct {
    AEM_CMD_RESP_s RcvdCommand;     /**< 9.2.2.3.1.2.1 */
    UINT8 RcvdAEMCommand;           /**< 9.2.2.3.1.2.2 */
    UINT64 MyEntityId;              /**< 9.2.2.3.1.2.3 */
    AEM_CMD_RESP_s Unsolicited;     /**< 9.2.2.3.1.2.4 */
    UINT8 DoUnsolicited;            /**< 9.2.2.3.1.2.5 */
    UINT8 DoTerminate;              /**< Boolean indicating that a request has been made to terminate. */
    UINT8 State;                    /**< Current state of state machine */
    UINT8 CmdProcessDone;           /**< A flag that indicate command process dont */
    AEM_CMD_RESP_s Resp;            /**< Response data */
} AECP_EM_SM_s;

#define AECP_MAX_INFLIGHT_CMD_NUM_DEFAULT    (5U)   /**< Max inflight command number */
/**
 * 9.2.2.3.2.1.2 InflightCommand
 */
typedef struct AECP_INFLIGHT_CMD_t {
    UINT32 Timeout;                     /**< A timer (timeout value) for when the command will timeout */
    UINT8 Retried;                      /**< 1 bit (boolean) indicating if a retry has been sent */
    AEM_CMD_RESP_s Cmd;                 /**< Command inflight */
    UINT16 Original_sequence_id;        /**< 16 bits, the sequence_id which was used when the command was sent */
    struct AECP_INFLIGHT_CMD_t *Next;   /**< Pointer to the next inflight command */
} AECP_INFLIGHT_CMD_s;

/**
 * 9.2.2.3.2.2 State Machine Variables
 */
typedef struct {
    AEM_CMD_RESP_s RcvdResponse;    /**< 9.2.2.3.2.2.1 */
    UINT8 RcvdNormalResponse;       /**< 9.2.2.3.2.2.2 */
    UINT8 RcvdUnsolicitedResponse;  /**< 9.2.2.3.2.2.3 */
    UINT64 MyEntityId;              /**< 9.2.2.3.2.2.4 */
    UINT32 CurrentTime;             /**< 9.2.2.3.2.2.5 */
    AECP_INFLIGHT_CMD_s *Inflight;  /**< 9.2.2.3.2.2.6 */
    AEM_CMD_RESP_s Command;         /**< 9.2.2.3.2.2.7 */
    UINT8 DoCommand;                /**< 9.2.2.3.2.2.8 */
    UINT8 DoTerminate;              /**< Boolean indicating that a request has been made to terminate. */
    UINT8 State;                    /**< Current state of state machine */
} AECP_EMC_SM_s;

/**
 * 9.2.2.6.1.2 State Machine Variables
 */
typedef struct {
    AEM_CMD_RESP_s RcvdCommand;     /**< 9.2.2.6.1.2.1 */
    UINT8 RcvdAACommand;            /**< 9.2.2.6.1.2.2 */
    UINT64 MyEntityId;              /**< 9.2.2.6.1.2.3 */
    UINT8 State;                    /**< Current state of state machine */
} AECP_AAE_SM_s;

/**
 * 9.2.2.6.2.2 State Machine Variables
 */
typedef struct {
    AEM_CMD_RESP_s RcvdResponse;    /**< 9.2.2.6.2.2.1 */
    UINT8 RcvdNormalResponse;       /**< 9.2.2.6.2.2.2 */
    UINT64 MyEntityId;              /**< 9.2.2.6.2.2.3 */
    UINT32 CurrentTime;             /**< 9.2.2.6.2.2.4 */
//    InflightCommand_t *Inflight;       9.2.2.6.2.2.5
    AEM_CMD_RESP_s Command;         /**< 9.2.2.6.2.2.6 */
    UINT8 DoCommand;                /**< 9.2.2.6.2.2.7 */
    UINT8 DoTerminate;              /**< Boolean indicating that a request has been made to terminate. */
    UINT8 State;                    /**< Current state of state machine */
} AECP_AAC_SM_s;

typedef void (*AECP_VENDOR_UNIQUE_CMD_CB)(const void *Pkt); /**< AECP Call-Back Functions */

/**
 * 9.2.1.2 AVDECC Entity Model format
 */
typedef struct {
    AECP_PACKET_s AecpPkt;
    UINT32 U:1U;               /**< 9.2.1.2.1 */
    UINT32 Command_type:15U;   /**< 9.2.1.2.2 - Table 7.125 - Command Codes */
} __attribute__((aligned(1), packed)) AECP_AEM_s;

/* 9.2.1.3 Address Access format */

/* 9.2.1.4 Legacy AV/C format */

/**
 * 9.2.1.5 Vendor Unique format
 */
typedef struct {
    UINT8 Protocol_id[6U];              /**< 9.2.1.5.1 */
    UINT8 Payload_specific_data[508U];  /**< 9.2.1.5.2 */
} __attribute__((aligned(1), packed)) AECP_VU_CMD_s;

/**
 * 9.2.1.5 Vendor Unique format
 */
typedef struct {
    AECP_PACKET_s AecpPkt;  /**< Aecp packet */
    AECP_VU_CMD_s VuCmd;    /**< Vendor unique command */
} __attribute__((aligned(1), packed)) AECP_VU_AECPDU_s;

/**
 * 9.2.2.12.1.2 State Machine Variables
 */
typedef struct {
    AECP_VU_AECPDU_s RcvdCommand;   /**< 9.2.2.12.1.2.1 */
    UINT8 RcvdVendorCommand;        /**< 9.2.2.12.1.2.2 */
    UINT64 MyEntityId;              /**< 9.2.2.12.1.2.3 */
    UINT8 State;                    /**< Current state of state machine */
    AECP_VU_AECPDU_s Resp;          /**< Vendor unique command response */
    void (*VuCmdCb)(const UINT32 Cmd);
} AECP_VU_SM_s;

/**
 * 9.2.2.12.2.2 State Machine Variables
 */
typedef struct {
    AECP_VU_AECPDU_s RcvdResponse;  /**< 9.2.2.12.2.2.1 */
    UINT8 RcvdNormalResponse;       /**< 9.2.2.12.2.2.2 */
    UINT64 MyEntityId;              /**< 9.2.2.12.2.2.3 */
    UINT32 CurrentTime;             /**< 9.2.2.12.2.2.4 */
//    Inflight;                          9.2.2.12.2.2.5
    AECP_VU_AECPDU_s Command;       /**< 9.2.2.12.2.2.6 */
    UINT8 DoCommand;                /**< 9.2.2.12.2.2.7 */
    UINT8 State;                    /**< Current state of state machine */
} AECP_VUC_SM_s;

/* 9.2.1.6 HDCP IIA Authentication Protocol format */

/* Clause 7 for AECP */
/**
 * 7.4.5 READ_DESCRIPTOR Command
 */
typedef struct {
    AECP_AEM_s AecpAem;             /**< Aecp Aem */
    UINT16 Configuration_index;     /**< 7.4.5.1 */
    UINT16 Reserved;                /**< 7.4.5.1 */
    UINT16 Descriptor_type;         /**< 7.4.5.1 */
    UINT16 Descriptor_index;        /**< 7.4.5.1 */
} __attribute__((aligned(1), packed)) AECP_READ_DESCRIPTOR_s;

#define AECP_ACQUIRE_ENTITY_FLAG_PERSISTENT (0x00000001U)   /**< Table 7.127 - ACQUIRE_ENTITY Flags */
#define AECP_ACQUIRE_ENTITY_FLAG_RELEASE    (0x80000000U)   /**< Table 7.127 - ACQUIRE_ENTITY Flags */
/**
 * 7.4.1 ACQUIRE_ENTITY Command
 */
typedef struct {
    AECP_AEM_s AecpAem;         /**< Aecp Aem */
    UINT32 Flags;               /**< 7.4.1.1 */
    UINT64 Owner_id;            /**< 7.4.1.1 */
    UINT16 Descriptor_type;     /**< 7.4.1.1 */
    UINT16 Descriptor_index;    /**< 7.4.1.1 */
} __attribute__((aligned(1), packed)) AECP_ACQUIRE_ENTITY_s;

#define AECP_LOCK_ENTITY_FLAG_UNLOCK (0x0001U)  /**< Table 7.128 - LOCK_ENTITY Flags */
/**
 * 7.4.2 LOCK_ENTITY Command
 */
typedef struct {
    AECP_AEM_s AecpAem;         /**< Aecp Aem */
    UINT32 Flags;               /**< 7.4.2.1 */
    UINT64 Locked_id;           /**< 7.4.2.1 */
    UINT16 Descriptor_type;     /**< 7.4.2.1 */
    UINT16 Descriptor_index;    /**< 7.4.2.1 */
} __attribute__((aligned(1), packed)) AECP_LOCK_ENTITY_s;

/**
 * 7.4.35 START_STREAMING Command
 */
typedef struct {
    AECP_AEM_s AecpAem;         /**< Aecp Aem */
    UINT16 Descriptor_type;     /**< 7.4.35.1 */
    UINT16 Descriptor_index;    /**< 7.4.35.1 */
} __attribute__((aligned(1), packed)) AECP_START_STREAMING_s;

/**
 * 7.4.36 STOP_STREAMING Command
 */
typedef struct {
    AECP_AEM_s AecpAem;         /**< Aecp Aem */
    UINT16 Descriptor_type;     /**< 7.4.36.1 */
    UINT16 Descriptor_index;    /**< 7.4.36.1 */
} __attribute__((aligned(1), packed)) AECP_STOP_STREAMING_s;

/**
 * 7.4.43 REBOOT Command
 */
typedef struct {
    AECP_AEM_s AecpAem;         /**< Aecp Aem */
    UINT16 Descriptor_type;     /**< 7.4.43.1 */
    UINT16 Descriptor_index;    /**< 7.4.43.1 */
} __attribute__((aligned(1), packed)) AECP_REBOOT_s;

#endif /* AECP_H */

