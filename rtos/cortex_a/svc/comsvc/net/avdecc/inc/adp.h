/**
 * @file adp.h
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
#ifndef ADP_H
#define ADP_H

/* Clause 6 of IEEE Std 1722.1 - 2013 */
#define ADP_CD_LEN   (56U)

/* 6.2.1.5 - Table 6.1 */
#define ADP_MSG_ENTITY_AVAILABLE (0x00U) /**< The AVDECC entity is available */
#define ADP_MSG_ENTITY_DEPARTING (0x01U) /**< The AVDECC entity is departing */
#define ADP_MSG_ENTITY_DISCOVER  (0x02U) /**< Request to send an MSG_ENTITY_AVAILABLE message */

/* 6.2.1.10 - Table 6.2 ¡Xentity_capabilities field */
#define ADP_ENTITY_CAP_EFU_MODE                         (0x00000001)
#define ADP_ENTITY_CAP_ADDRESS_ACCESS_SUPPORTED         (0x00000002)
#define ADP_ENTITY_CAP_GATEWAY_ENTITY                   (0x00000004)
#define ADP_ENTITY_CAP_AEM_SUPPORTED                    (0x00000008)
#define ADP_ENTITY_CAP_LEGACY_AVC                       (0x00000010)
#define ADP_ENTITY_CAP_ASSOCIATION_ID_SUPPORTED         (0x00000020)
#define ADP_ENTITY_CAP_ASSOCIATION_ID_VALID             (0x00000040)
#define ADP_ENTITY_CAP_VENDOR_UNIQUE_SUPPORTED          (0x00000080)
#define ADP_ENTITY_CAP_CLASS_A_SUPPORTED                (0x00000100)
#define ADP_ENTITY_CAP_CLASS_B_SUPPORTED                (0x00000200)
#define ADP_ENTITY_CAP_GPTP_SUPPORTED                   (0x00000400)
#define ADP_ENTITY_CAP_AEM_AUTH_SUPPORTED               (0x00000800)
#define ADP_ENTITY_CAP_AEM_AUTH_REQUIRED                (0x00001000)
#define ADP_ENTITY_CAP_AEM_PERSISTENT_ACQUIRE_SUPPORTED (0x00002000)
#define ADP_ENTITY_CAP_AEM_IDENTIFY_CONTROL_INDEX_VALID (0x00004000)
#define ADP_ENTITY_CAP_AEM_INTERFACE_INDEX_VALID        (0x00008000)
#define ADP_ENTITY_CAP_GENERAL_CONTROLLER_IGNORE        (0x00010000)
#define ADP_ENTITY_CAP_ENTITY_NOT_READY                 (0x00020000)

/* 6.2.1.12 - Table 6.3 - talker_capabilities field */
#define ADP_TALKER_CAP_IMPLEMENTED          (0x0001U)
#define ADP_TALKER_CAP_OTHER_SOURCE         (0x0200U)
#define ADP_TALKER_CAP_CONTROL_SOURCE       (0x0400U)
#define ADP_TALKER_CAP_MEDIA_CLOCK_SOURCE   (0x0800U)
#define ADP_TALKER_CAP_SMPTE_SOURCE         (0x1000U)
#define ADP_TALKER_CAP_MIDI_SOURCE          (0x2000U)
#define ADP_TALKER_CAP_AUDIO                (0x4000U)
#define ADP_TALKER_CAP_VIDEO                (0x8000U)

/* 6.2.1.14 - Table 6.4 - Listener capabilities field */
#define ADP_LISTENER_CAP_IMPLEMENTED        (0x0001U)
#define ADP_LISTENER_CAP_OTHER_SINK         (0x0200U)
#define ADP_LISTENER_CAP_CONTROL_SINK       (0x0400U)
#define ADP_LISTENER_CAP_MEDIA_CLOCK_SINK   (0x0800U)
#define ADP_LISTENER_CAP_SMPTE_SINK         (0x1000U)
#define ADP_LISTENER_CAP_MIDI_SINK          (0x2000U)
#define ADP_LISTENER_CAP_AUDIO              (0x4000U)
#define ADP_LISTENER_CAP_VIDEO              (0x8000U)

/* 6.2.1.15 - Table 6.5 - controller_capabilities field*/
#define ADP_CTRL_CAP_IMPLEMENTED            (0x0001U)

/* AVTPDU - The IEEE Std 1722-2011 control AVTPDU */
typedef struct {
    UINT32 Control_data_len : 11;   /**< 6.2.1.7 */
    UINT32 Valid_time : 5;          /**< 6.2.1.6 */
    UINT32 Message_type : 4;        /**< 6.2.1.5 - Table 6.1 */
    UINT32 Version : 3;             /**< 6.2.1.4 Always set to 0 */
    UINT32 Sv : 1;                  /**< 6.2.1.3 StreamID Valid indicator: Always set to 0 */
    UINT32 Subtype : 8;             /**< 6.2.1.2 Always set to 0x7A */
    UINT64 Entity_id;               /**< 6.2.1.1 stream_id field */
} __attribute__((aligned(4), packed)) ADP_AVTPDU_HEADER_s;

/* 6.2.1 AVDECC Discovery Protocol Data Unit (Adpdu) Format */
/* 6.2.3.2 entityInfo */
/* 6.2.5.1.2 entityInfo */
typedef struct {
    UINT64 Entity_model_id;         /**< 6.2.1.9 */
    UINT32 Entity_capabilities;     /**< 6.2.1.10 - Table 6.2  */
    UINT16 Talker_stream_sources;   /**< 6.2.1.11 */
    UINT16 Talker_capabilities;     /**< 6.2.1.12 - Table 6.3 */
    UINT16 Listener_stream_sinks;   /**< 6.2.1.13 */
    UINT16 Listener_capabilities;   /**< 6.2.1.14 - Table 6.4 */
    UINT32 Controller_capabilities; /**< 6.2.1.15 - Table 6.5 */
    UINT32 Available_index;         /**< 6.2.1.16 */
    UINT64 Gptp_grandmaster_id;     /**< 6.2.1.17 */
    UINT8 Gptp_domain_number;       /**< 6.2.1.18, not support  */
    UINT8 Reserved0[3];
    UINT16 Identify_control_index;  /**< 6.2.1.19 */
    UINT16 Interface_index;         /**< 6.2.1.20 */
    UINT64 Association_id;          /**< 6.2.1.21, not support */
    UINT32 Reserved1;
} __attribute__((aligned(1), packed)) ADPDU_s;

/* Figure 6.1 */
typedef struct {
    ADP_AVTPDU_HEADER_s AvtpduHeader;
    ADPDU_s Adpdu;
} ADP_PACKET_s;

/* 6.2.3 Global state machine variables */
typedef struct {
    UINT32 CurrentTime;         /**< 6.2.3.1 currentTime */
    ADP_PACKET_s EntityInfo;    /**< 6.2.3.2 entityInfo */
} ADP_GLOBAL_SM_s;

/* 6.2.4 Advertise Entity State Machine */
typedef struct {
    UINT32 CurrentTime;             /**< 6.2.3.1 currentTime */
    ADP_PACKET_s EntityInfo;        /**< 6.2.3.2 entityInfo */
    UINT64 ReannounceTimerTimeout;  /**< 6.2.4.1.1 reannounceTimerTimeout */
    UINT8  NeedsAdvertise;          /**< 6.2.4.1.2 needsAdvertise */
    UINT8  DoTerminate;             /**< 6.2.4.1.3 doTerminate */
    UINT8  State;
} ADP_ADV_ENTITY_SM_s;

/* 6.2.5 Advertise Interface State Machine */
typedef struct {
    UINT64 AdvertisedGrandmasterID; /**< 6.2.5.1.1 advertisedGrandmasterID */
    ADP_PACKET_s EntityInfo;        /**< 6.2.5.1.2 entityInfo */
    UINT8 RcvdDiscover;             /**< 6.2.5.1.3 rcvdDiscover */
    UINT64 EntityId;                /**< 6.2.5.1.4 entityID */
    UINT8 DoTerminate;              /**< 6.2.5.1.5 doTerminate */
    UINT8 DoAdvertise;              /**< 6.2.5.1.6 doAdvertise */
    UINT8 LinkIsUp;                 /**< 6.2.5.1.7 linkIsUp */
    UINT8 LastLinkIsUp;             /**< 6.2.5.1.8 lastLinkIsUp */
    UINT8 State;
} ADP_ADV_INTERFACE_SM_s;

/* 6.2.6.1.1 rcvdEntityInfo */
typedef struct {
    UINT8 SrcMac[6U];       /* Source MAC that ADPDU sent from */
    UINT8 Port;             /* Port # that get ADPDU */
    UINT8 RcvdAvailable;    /* 6.2.6.1.2 rcvdAvailable */
    UINT8 RcvdDeparting;    /* 6.2.6.1.3 rcvdDeparting */
    ADP_PACKET_s AdpduPkt;  /* 6.2.6.1.7 entities */
} ADP_RCVDENTITYINFO_s;

/* 6.2.6.1.7 entities */
typedef struct {
    ADP_RCVDENTITYINFO_s EntityInfo;
    UINT32 Timer;           /* 6.2.6.1.7 entities, timer */
    UINT32 Timeout;
} ADP_ENTITY_s;

/* 6.2.6 Discovery State machine */
typedef struct {
    ADP_PACKET_s EntityInfo;                /**< 6.2.5.1.2 entityInfo */
    ADP_RCVDENTITYINFO_s RcvdEntityInfo;    /**< 6.2.6.1.1 rcvdEntityInfo */ //TODO: Using buffer List
    UINT8 DoDiscover;
    UINT64 DiscoverID;
    UINT8 DoTerminate;
    ADP_ENTITY_s *Entities;                 /* 6.2.6.1.7 entities */
    UINT8 State;
} ADP_DISCOVERY_SM_s;

#endif /* ADP_H */

