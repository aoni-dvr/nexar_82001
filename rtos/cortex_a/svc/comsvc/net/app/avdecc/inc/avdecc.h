/**
 *  @file avdecc.h
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
 *  @details AVDECC public headers.
 *
 */

#ifndef AVDECC_H
#define AVDECC_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif

#ifndef AMBA_KAL_H
#include <AmbaKAL.h>
#endif
#ifndef AMBA_PRINT_H
#include <AmbaPrint.h>
#endif

#include <NetStack.h>
#include <AvbStack.h>
#include <AmbaENET.h>



#ifndef TRUE
#define TRUE    (1U)
#endif
#ifndef FALSE
#define FALSE   (0U)
#endif


#define ACDECC_CORE (0x01u)

/*
 * Implementation
 */
/* Frame size of one avdecc packet */
#define AVDECC_FSIZE (1600U)
/* L3 packet size. */
#define AVDECC_PSIZE (AVDECC_FSIZE - ((sizeof(UINT32) * 6U) + (sizeof(UINT8) * 12U)))

typedef struct {
    UINT32 idx;
    UINT32 len;
    UINT32 free;
    UINT8  stype;       /**< AVDECC command, subtype. */
    UINT8  mtype;       /**< message type of subtype. */
    UINT8  unused[2];
    UINT8  smac[6];
    UINT8  dmac[6];
    UINT8  pkt[AVDECC_PSIZE];
} avdecc_pkt_t;

/*
 * Timer
 */
#define AVDECC_TIMER_MS     (5U)    /**< Timeout of SW timer in ms,
                                     *   AVDECC timeout would be 25ms, 240ms, etc.
                                     */
#define AVDECC_TIMER_NO     (10U)
/* SW timer */
typedef struct {
    UINT8  enable;
    void (*func)(void);     /**< Function MUST fast */
    UINT32 timer;           /**< Alarm timer, count down in AVDECC_TIMER_MS  */
    UINT32 reload_value;    /**< Reload value for cyclic running, in AVDECC_TIMER_MS */
    UINT32 cycle;           /**< How many times to run, 0 as unlimited. */
    UINT32 isTO;            /**> Is timeout? */
} avdecc_timer_t;

/* Flag to trigger entity to handle message */
#define EMSG_COMMON (0x8000000U)
#define EMSG_ADP    (0x4000000U)
#define EMSG_ACMP   (0x2000000U)
#define EMSG_AECP   (0x1000000U)
#define EMSG_TYPE   (0xF000000U)


/*
 * Specification implementation
 */

/** AVTP EtherType, i.e. ETH_P_TSN in Linux  */
#define ETYPE_AVTP (0x22F0U)



/* Clause 6 of IEEE Std 1722.1 - 2013 */

/*
 * AVTP Format
 * cf. Figure 11 —AVTPDU common control header of IEEE 1722 - 2016
 * ADP, ACMP, AECP have different definition of fields
 * (but the same size of header).
 */
typedef struct {
    UINT32 control_data_len : 11;    /**< Depends on subtype */
    UINT32 format_specific_data : 9; /**< Depends on subtype */
    UINT32 version : 3;              /**< Always set to 0 */
    UINT32 sv : 1;                   /**< StreamID Valid indicator: Always set to 0 */
    UINT32 subtype : 8;              /**< Data type of ADP, ACMP, AECP,
                                      *   cd bit (subtype[7]) always be 1.
                                      */

    UINT64 stream_id;
} __attribute__((aligned(4), packed)) avtpdu_hdr_t;

/* avtpdu_hdr.control_data_len */
#define AVDECC_CD_LEN_ADP              (56U)
#define AVDECC_CD_LEN_ACMP             (44U)
//#define AVDECC_CD_LEN_AECP_MAX (524U)

/* avtpdu_hdr.subtype with cd filed always as 1 */
#define AVTPDU_SUBTYPE_ADP             ((UINT8)(0x7Au | 0x80u)) /**< AVDECC Discovery Protocol */
#define AVTPDU_SUBTYPE_AECP            ((UINT8)(0x7Bu | 0x80u)) /**< AVDECC Enumeration and Control Protocol */
#define AVTPDU_SUBTYPE_ACMP            ((UINT8)(0x7Cu | 0x80u)) /**< AVDECC Connection Management Protocol */

/* All ADPDUs are transmitted to the ADP multicast destination MAC address defined in Table B.1 */
/* AVDECC Multicast MAC Address */
// 91-e0-f0-01-00-00, ADP and ACMP, 6.2.1 and Clause 8
#define ADP_MULTICAST_MAC              { 0x91, 0xE0, 0xF0, 0x01, 0x00, 0x00 }
#define ACMP_MULTICAST_MAC             { 0x91, 0xE0, 0xF0, 0x01, 0x00, 0x00 }
// 91-e0-f0-01-00-00, ID Notifications, 7.5.1



/** ADPDU format */
typedef struct {
    /* AVTP Format */
    struct {
        /*
         * cf. The IEEE Std 1722-2011 control AVTPDU
         */
        UINT32 control_data_len : 11;
        UINT32 valid_time : 5;           /**< status indicates how long the record will be avilable,
                                             in 2-second increments,
                                             default 62 seconds (i.e. value 31).
                                             status field.
                                          */
/* Table 6.1 */
#define ADP_MSG_ENTITY_AVAILABLE (0x00U) /**< The AVDECC entity is available */
#define ADP_MSG_ENTITY_DEPARTING (0x01U) /**< The AVDECC entity is departing */
#define ADP_MSG_ENTITY_DISCOVER  (0x02U) /**< Request to send an MSG_ENTITY_AVAILABLE message */
        UINT32 message_type : 4;         /**< ADP_MSG_ENTITY_XXX, control_data field */
        UINT32 version : 3;              /**< Always set to 0 */
        UINT32 sv : 1;                   /**< StreamID Valid indicator: Always set to 0 */
        UINT32 subtype : 8;              /**< Always set to 0x7A,
                                          *   in 1722-2016, subtype is 1 octet, i.e. including cd bit
                                          *   and always set to 1
                                          */
        UINT64 entity_id;                /**< stream_id field */
    } __attribute__((aligned(4), packed)) avtpdu_hdr;

    /* AVDECC Discovery Protocol Data Unit (ADPDU) Format */
    struct {
        /*
         * cf. Figure 6.1 of IEEE 1722.1 - 2013
         */
        UINT64 entity_model_id;       /**< EUI-64 ID of the AVDECC Entity data model from a vendor */

        /* TODO: Table 6.2 by structure. */
        UINT32 entity_cap;            /**< Capabilities to identify supported features and protocol */

        UINT16 talker_stream_src;     /**< Contain the number of simultabeous Talker streams a device can source */
        /* TODO: Table 6.3 */
#define TALKER_CAP_IMP     (0x0001U)  /**< Implements a Talker */
#define TALKER_CAP_AUDIO   (0x4000U)  /**< Talker has audio stream sources */
#define TALKER_CAP_VIDEO   (0x8000U)  /**< Talker has video stream sources */
        UINT16 talker_cap;            /**< Talker's capabilities */

        UINT16 listener_stream_sinks; /**< ID of streams a Listener can simultaneously sink */
        /* TODO: Table 6.4 */
#define LISTENER_CAP_IMP   (0x0001U)  /**< Implements a Listener */
#define LISTENER_CAP_AUDIO (0x4000U)  /**< Listener has audio stream sinks */
#define LISTENER_CAP_VIDEO (0x8000U)  /**< Listener has video stream sinks */
        UINT16 listener_cap;          /**< Listener's capabilities */

        /* TODO: Table 6.5 */
#define CTRL_CAP_IMP       (0x0001U)  /**< Implements a Controller */
        UINT32 ctrl_cap;

        UINT32 available_index;       /**< Increase after tx ENTITY_AVAILABLE messages,
                                          and reset to 0 on tx ENTITY_DEPARTING */
        UINT64 gptp_grandmaster_id;   /* 6.2.1.17 */
        UINT8  gptp_domain_no;        /* 6.2.1.18, not support  */
        UINT8  reserved0[3];
        UINT16 id_control_index;      /* 6.2.1.19 */
        UINT16 interface_index;       /* 6.2.1.20 */
        UINT64 association_id;        /* 6.2.1.21, not support */
        UINT32 reserved1;
    } __attribute__((aligned(1), packed)) adpdu;
} pkt_adp_t;


/* Table 6.2 entity_capabilities field */
#define AVDECC_entity_cap
#define ADP_ENTITY_CAP_EFU_MODE                         (0x00000001U)
#define ADP_ENTITY_CAP_ADDRESS_ACCESS_SUPPORTED         (0x00000002U)
#define ADP_ENTITY_CAP_GATEWAY_ENTITY                   (0x00000004U)
#define ADP_ENTITY_CAP_AEM_SUPPORTED                    (0x00000008U)
#define ADP_ENTITY_CAP_LEGACY_AVC                       (0x00000010U)
#define ADP_ENTITY_CAP_ASSOCIATION_ID_SUPPORTED         (0x00000020U)
#define ADP_ENTITY_CAP_ASSOCIATION_ID_VALID             (0x00000040U)
#define ADP_ENTITY_CAP_VENDOR_UNIQUE_SUPPORTED          (0x00000080U)
#define ADP_ENTITY_CAP_CLASS_A_SUPPORTED                (0x00000100U)
#define ADP_ENTITY_CAP_CLASS_B_SUPPORTED                (0x00000200U)
#define ADP_ENTITY_CAP_GPTP_SUPPORTED                   (0x00000400U)
#define ADP_ENTITY_CAP_AEM_AUTH_SUPPORTED               (0x00000800U)
#define ADP_ENTITY_CAP_AEM_AUTH_REQUIRED                (0x00001000U)
#define ADP_ENTITY_CAP_AEM_PERSISTENT_ACQUIRE_SUPPORTED (0x00002000U)
#define ADP_ENTITY_CAP_AEM_IDENTIFY_CONTROL_INDEX_VALID (0x00004000U)
#define ADP_ENTITY_CAP_AEM_INTERFACE_INDEX_VALID        (0x00008000U)
#define ADP_ENTITY_CAP_GENERAL_CONTROLLER_IGNORE        (0x00010000U)
#define ADP_ENTITY_CAP_ENTITY_NOT_READY                 (0x00020000U)


/* 6.2.3 Global state machine variables */
#if 0
typedef struct {
    UINT64    currentTime;      /**< the current time of a local clock */
    pkt_adp_t entityInfo;
} g_adp_var;
#endif

/** ADPDU format */
/**
 *  6.2.3.2 entityInfo
 *  The entityInfo variable is a structure containing all of
 *  the common information required to populate the fields of
 *  an ADPDU across all of the interfaces.
 *  This variable is only used for the Advertising Entity and
 *  Advertising Interface state machines.
 *  cf. 6.2.5.1.2 entityInfo
 *  cf. Figure 6.1 of IEEE 1722.1 - 2013
 */
typedef struct {
    /*
     * cf. Figure 6.1 of IEEE 1722.1 - 2013
     */
    UINT64 entity_model_id;       /**< EUI-64 ID of the AVDECC Entity data model from a vendor */

    /* TODO: Table 6.2 by structure. */
    UINT32 entity_cap;            /**< Capabilities to identify supported features and protocol */

    UINT16 talker_stream_src;     /**< Contain the number of simultabeous Talker streams a device can source */
    /* TODO: Table 6.3 */
    UINT16 talker_cap;            /**< Talker's capabilities */

    UINT16 listener_stream_sinks; /**< ID of streams a Listener can simultaneously sink */
    /* TODO: Table 6.4 */
    UINT16 listener_cap;          /**< Listener's capabilities */

    /* TODO: Table 6.5 */
    UINT32 ctrl_cap;

    UINT32 available_index;       /**< Increase after tx ENTITY_AVAILABLE messages,
                                      and reset to 0 on tx ENTITY_DEPARTING */
    UINT64 gptp_grandmaster_id;   /* 6.2.1.17 */
    UINT8  gptp_domain_no;        /* 6.2.1.18, not support  */
    UINT8  reserved0[3];
    UINT16 id_control_index;      /* 6.2.1.19 */
    UINT16 interface_index;       /* 6.2.1.20 */
    UINT64 association_id;        /* 6.2.1.21, not support */
    UINT32 reserved1;
} __attribute__((aligned(1), packed)) entityInfo_t;

/* 6.2.4 Advertise Entity State Machine */
typedef struct {
    UINT64 reannounceTimerTimeout;  /**< the time relative to currentTime */
    UINT8  needsAdvertise;
    UINT8  doTerminate;

    /* 6.2.4.3 State machine diagram */
    /* BEGIN -> INITIALIZE -> ADVERTISE -> WAITING -> END */
    UINT8  state;

    /* sendAvailable() */
} sm_adp_advEntity_t;

/* 6.2.5 Advertise Interface State Machine */
typedef struct {
    UINT64     advertisedGrandmasterID;
//    g_adp_var *entityInfo;
    UINT8      rcvdDiscover;
    UINT64     entityID;
    UINT8      doTerminate;
    UINT8      doAdvertise;
    UINT8      linkIsUp;
    UINT8      lastLinkIsUp;

    UINT8      state;

    /*
       txEntityAvailable()
       txEntityDeparting()
     */
} sm_adp_advInterface_t;

/* 6.2.6 Discovery State machine */
typedef struct {
    UINT8  rcvdAvailable;
    UINT8  rcvdDeparting;
    UINT8  doDiscover;
    UINT8  doTerminate;
    UINT64 discoverID;

    UINT8  state;

    /*
       txDiscover(entityID)
       haveEntity(entityID)
       updateEntity(entityInfo)
       addEntity(entityInfo)
       removeEntity(eui64)
     */

    pkt_adp_t *entities;       ///< TODO: replaced by rcvdEntityInfo
    pkt_adp_t  rcvdEntityInfo; ///< TODO: Using buffer List
    void *     p;              // Packets to be parsed
} sm_adp_discovery_t;


#define MAX_RX_EI (4U)

// 6.2.6.1.1 rcvdEntityInfo
typedef struct rcvdEntityInfo_s {
    pkt_adp_t                adpdu;
    UINT8                    smac[6];       // Source MAC that ADPDU sent from
    UINT8                    port;          // Port # that get ADPDU
    UINT8                    rcvdAvailable; /* A Boolean indicating that the ADPDU data
                                             *  in rcvdEntityInfo is from a
                                             * ENTITY_AVAILABLE message. */
    UINT8                    rcvdDeparting; /* A Boolean indicating that the ADPDU data
                                             * in rcvdEntityInfo is from a
                                             * ENTITY_DEPARTING message. */
    UINT32                   timer;         // cf. 6.2.6.1.7 entities
    struct rcvdEntityInfo_s *next;
} rcvdEntityInfo_t;


/* Clause 7 of IEEE Std 1722.1 - 2013 */

// Table 7.1—Descriptor Types
#define AVDECC_DT_ENTITY               (0x0000)
#define AVDECC_DT_CONFIGURATION        (0x0001)
#define AVDECC_DT_AUDIO_UNIT           (0x0002)
#define AVDECC_DT_VIDEO_UNIT           (0x0003)
#define AVDECC_DT_SENSOR_UNIT          (0x0004)
#define AVDECC_DT_STREAM_INPUT         (0x0005)
#define AVDECC_DT_STREAM_OUTPUT        (0x0006)
#define AVDECC_DT_JACK_INPUT           (0x0007)
#define AVDECC_DT_JACK_OUTPUT          (0x0008)
#define AVDECC_DT_AVB_INTERFACE        (0x0009)
#define AVDECC_DT_CLOCK_SOURCE         (0x000A)
#define AVDECC_DT_MEMORY_OBJECT        (0x000B)
#define AVDECC_DT_LOCALE               (0x000C)
#define AVDECC_DT_STRINGS              (0x000D)
#define AVDECC_DT_STREAM_PORT_INPUT    (0x000E)
#define AVDECC_DT_STREAM_PORT_OUTPUT   (0x000F)
#define AVDECC_DT_EXTERNAL_PORT_INPUT  (0x0010)
#define AVDECC_DT_EXTERNAL_PORT_OUTPUT (0x0011)
#define AVDECC_DT_INTERNAL_PORT_INPUT  (0x0012)
#define AVDECC_DT_INTERNAL_PORT_OUTPUT (0x0013)
#define AVDECC_DT_AUDIO_CLUSTER        (0x0014)
#define AVDECC_DT_VIDEO_CLUSTER        (0x0015)
#define AVDECC_DT_SENSOR_CLUSTER       (0x0016)
#define AVDECC_DT_AUDIO_MAP            (0x0017)
#define AVDECC_DT_VIDEO_MAP            (0x0018)
#define AVDECC_DT_SENSOR_MAP           (0x0019)
#define AVDECC_DT_CONTROL              (0x001A)
#define AVDECC_DT_SIGNAL_SELECTOR      (0x001B)
#define AVDECC_DT_MIXER                (0x001C)
#define AVDECC_DT_MATRIX               (0x001D)
#define AVDECC_DT_MATRIX_SIGNAL        (0x001E)
#define AVDECC_DT_SIGNAL_SPLITTER      (0x001F)
#define AVDECC_DT_SIGNAL_COMBINER      (0x0020)
#define AVDECC_DT_SIGNAL_DEMULTIPLEXER (0x0021)
#define AVDECC_DT_SIGNAL_MULTIPLEXER   (0x0022)
#define AVDECC_DT_SIGNAL_TRANSCODER    (0x0023)
#define AVDECC_DT_CLOCK_DOMAIN         (0x0024)
#define AVDECC_DT_CONTROL_BLOCK        (0x0025)
#define AVDECC_DT_INVALID              (0xFFFF)

// Table 7.2—ENTITY Descriptor
typedef struct {
    UINT16                       descriptor_type;
    /** The index of the descriptor.
     *  This is always set to zero (0) for the ENTITY descriptor as
     *  there is only ever one in an AVDECC Entity.
     */
    UINT16                       descriptor_index;
    UINT64                       entity_id;
    UINT64                       entity_model_id;
    UINT32                       entity_capabilities;
    UINT16                       talker_stream_sources;
    UINT16                       talker_capabilities;
    UINT16                       listener_stream_sinks;
    UINT16                       listener_capabilities;
    UINT32                       controller_capabilities;
    UINT32                       available_index;
    UINT64                       association_id;
#define AVDECC_ENTITY_NAME_LEN      (64)
    UINT8                        entity_name[AVDECC_ENTITY_NAME_LEN];
    UINT16                       vendor_name_string;
    UINT16                       model_name_string;
#define AVDECC_FIRMWARE_VERSION_LEN (64)
    UINT8                        firmware_version[AVDECC_FIRMWARE_VERSION_LEN];
#define AVDECC_GROUP_NAME_LEN       (64)
    UINT8                        group_name[AVDECC_GROUP_NAME_LEN];
#define AVDECC_SERIAL_NUMBER_LEN    (64)
    UINT8                        serial_number[AVDECC_SERIAL_NUMBER_LEN];
    UINT16                       configurations_count;
    UINT16                       current_configuration;
} descriptor_entity_t;

// Table 7.4—descriptor_counts Format
typedef struct {
    UINT16 descriptor_type;
    UINT16 count;
} descriptor_count_t;

// Table 7.3—CONFIGURATION Descriptor
typedef struct {
    UINT16                      descriptor_type; // AVDECC_DT_xxx, cf. Table 7.1
    UINT16                      descriptor_index;
#define AVDECC_OBJECT_NAME_LEN (64)
    UINT8                       object_name[AVDECC_OBJECT_NAME_LEN];
    UINT16                      localized_description;
    UINT16                      descriptor_counts_count;
    UINT16                      descriptor_counts_offset;
    UINT16                      padding;
    struct  descriptor_count_s *dc;
} configuration_t;

/* Table 7.125—Command Codes */
#define AECP_CMD_ACQUIRE_ENTITY                         (0x0000u)
#define AECP_CMD_LOCK_ENTITY                            (0x0001u)
#define AECP_CMD_ENTITY_AVAILABLE                       (0x0002u)
#define AECP_CMD_CONTROLLER_AVAILABLE                   (0x0003u)
#define AECP_CMD_READ_DESCRIPTOR                        (0x0004u)
#define AECP_CMD_WRITE_DESCRIPTOR                       (0x0005u)
#define AECP_CMD_SET_CONFIGURATION                      (0x0006u)
#define AECP_CMD_GET_CONFIGURATION                      (0x0007u)
#define AECP_CMD_SET_STREAM_FORMAT                      (0x0008u)
#define AECP_CMD_GET_STREAM_FORMAT                      (0x0009u)
#define AECP_CMD_SET_VIDEO_FORMAT                       (0x000au)
#define AECP_CMD_GET_VIDEO_FORMAT                       (0x000bu)
#define AECP_CMD_SET_SENSOR_FORMAT                      (0x000cu)
#define AECP_CMD_GET_SENSOR_FORMAT                      (0x000du)
#define AECP_CMD_SET_STREAM_INFO                        (0x000eu)
#define AECP_CMD_GET_STREAM_INFO                        (0x000fu)
#define AECP_CMD_SET_NAME                               (0x0010u)
#define AECP_CMD_GET_NAME                               (0x0011u)
#define AECP_CMD_SET_ASSOCIATION_ID                     (0x0012u)
#define AECP_CMD_GET_ASSOCIATION_ID                     (0x0013u)
#define AECP_CMD_SET_SAMPLING_RATE                      (0x0014u)
#define AECP_CMD_GET_SAMPLING_RATE                      (0x0015u)
#define AECP_CMD_SET_CLOCK_SOURCE                       (0x0016u)
#define AECP_CMD_GET_CLOCK_SOURCE                       (0x0017u)
#define AECP_CMD_SET_CONTROL                            (0x0018u)
#define AECP_CMD_GET_CONTROL                            (0x0019u)
#define AECP_CMD_INCREMENT_CONTROL                      (0x001au)
#define AECP_CMD_DECREMENT_CONTROL                      (0x001bu)
#define AECP_CMD_SET_SIGNAL_SELECTOR                    (0x001cu)
#define AECP_CMD_GET_SIGNAL_SELECTOR                    (0x001du)
#define AECP_CMD_SET_MIXER                              (0x001eu)
#define AECP_CMD_GET_MIXER                              (0x001fu)
#define AECP_CMD_SET_MATRIX                             (0x0020u)
#define AECP_CMD_GET_MATRIX                             (0x0021u)
#define AECP_CMD_START_STREAMING                        (0x0022u)
#define AECP_CMD_STOP_STREAMING                         (0x0023u)
#define AECP_CMD_REGISTER_UNSOLICITED_NOTIFICATION      (0x0024u)
#define AECP_CMD_DEREGISTER_UNSOLICITED_NOTIFICATION    (0x0025u)
#define AECP_CMD_IDENTIFY_NOTIFICATION                  (0x0026u)
#define AECP_CMD_GET_AVB_INFO                           (0x0027u)
#define AECP_CMD_GET_AS_PATH                            (0x0028u)
#define AECP_CMD_GET_COUNTERS                           (0x0029u)
#define AECP_CMD_REBOOT                                 (0x002au)
#define AECP_CMD_GET_AUDIO_MAP                          (0x002bu)
#define AECP_CMD_ADD_AUDIO_MAPPINGS                     (0x002cu)
#define AECP_CMD_REMOVE_AUDIO_MAPPINGS                  (0x002du)
#define AECP_CMD_GET_VIDEO_MAP                          (0x002eu)
#define AECP_CMD_ADD_VIDEO_MAPPINGS                     (0x002fu)
#define AECP_CMD_REMOVE_VIDEO_MAPPINGS                  (0x0030u)
#define AECP_CMD_GET_SENSOR_MAP                         (0x0031u)
#define AECP_CMD_ADD_SENSOR_MAPPINGS                    (0x0032u)
#define AECP_CMD_REMOVE_SENSOR_MAPPINGS                 (0x0033u)
#define AECP_CMD_START_OPERATION                        (0x0034u)
#define AECP_CMD_ABORT_OPERATION                        (0x0035u)
#define AECP_CMD_OPERATION_STATUS                       (0x0036u)
#define AECP_CMD_AUTH_ADD_KEY                           (0x0037u)
#define AECP_CMD_AUTH_DELETE_KEY                        (0x0038u)
#define AECP_CMD_AUTH_GET_KEY_LIST                      (0x0039u)
#define AECP_CMD_AUTH_GET_KEY                           (0x003au)
#define AECP_CMD_AUTH_ADD_KEY_TO_CHAIN                  (0x003bu)
#define AECP_CMD_AUTH_DELETE_KEY_FROM_CHAIN             (0x003cu)
#define AECP_CMD_AUTH_GET_KEYCHAIN_LIST                 (0x003du)
#define AECP_CMD_AUTH_GET_IDENTITY                      (0x003eu)
#define AECP_CMD_AUTH_ADD_TOKEN                         (0x003fu)
#define AECP_CMD_AUTH_DELETE_TOKEN                      (0x0040u)
#define AECP_CMD_AUTHENTICATE                           (0x0041u)
#define AECP_CMD_DEAUTHENTICATE                         (0x0042u)
#define AECP_CMD_ENABLE_TRANSPORT_SECURITY              (0x0043u)
#define AECP_CMD_DISABLE_TRANSPORT_SECURITY             (0x0044u)
#define AECP_CMD_ENABLE_STREAM_ENCRYPTION               (0x0045u)
#define AECP_CMD_DISABLE_STREAM_ENCRYPTION              (0x0046u)
#define AECP_CMD_SET_MEMORY_OBJECT_LENGTH               (0x0047u)
#define AECP_CMD_GET_MEMORY_OBJECT_LENGTH               (0x0048u)
#define AECP_CMD_SET_STREAM_BACKUP                      (0x0049u)
#define AECP_CMD_GET_STREAM_BACKUP                      (0x004au)
#define AECP_CMD_EXPANSION                              (0x7fffu)


/* Table 7.126—status field */
#define AECP_SS_SUCCESS                                 (0u)
#define AECP_SS_NOT_IMPLEMENTED                         (1u)
#define AECP_SS_NO_SUCH_DESCRIPTOR                      (2u)
#define AECP_SS_ENTITY_LOCKED                           (3u)
#define AECP_SS_ENTITY_ACQUIRED                         (4u)
#define AECP_SS_NOT_AUTHENTICATED                       (5u)
#define AECP_SS_AUTHENTICATION_DISABLED                 (6u)
#define AECP_SS_BAD_ARGUMENTS                           (7u)
#define AECP_SS_NO_RESOURCES                            (8u)
#define AECP_SS_IN_PROGRESS                             (9u)
#define AECP_SS_ENTITY_MISBEHAVING                      (10u)
#define AECP_SS_NOT_SUPPORTED                           (11u)
#define AECP_SS_STREAM_IS_RUNNING                       (12u)



/* Clause 8 of IEEE Std 1722.1 - 2013 */

typedef struct {
    /* AVTP Format */
    struct {
        /*
         * cf. The IEEE Std 1722-2011 control AVTPDU
         */
        UINT32 control_data_len : 11; /**< Always to be 44, CD_LEN_ACMP */

/* Table 8.2 status field */
#define ACMP_SS_SUCCESS                     (0U)
#define ACMP_SS_LISTENER_UNKNOWN_ID         (1U)
#define ACMP_SS_TALKER_UNKNOWN_ID           (2U)
#define ACMP_SS_TALKER_DEST_MAC_FAIL        (3U)
#define ACMP_SS_TALKER_NO_STREAM_INDEX      (4U)
#define ACMP_SS_TALKER_NO_BANDWIDTH         (5U)
#define ACMP_SS_TALKER_EXCLUSIVE            (6U)
#define ACMP_SS_LISTENER_TALKER_TIMEOUT     (7U)
#define ACMP_SS_LISTENER_EXCLUSIVE          (8U)
#define ACMP_SS_STATE_UNAVAILABLE           (9U)
#define ACMP_SS_NOT_CONNECTED               (10U)
#define ACMP_SS_NO_SUCH_CONNECTION          (11U)
#define ACMP_SS_COULD_NOT_SEND_MESSAGE      (12U)
#define ACMP_SS_TALKER_MISBEHAVING          (13U)
#define ACMP_SS_LISTENER_MISBEHAVING        (14U)
//#define ACMP_SS_                            (15U)
#define ACMP_SS_CONTROLLER_NOT_AUTHORIZED   (16U)
#define ACMP_SS_INCOMPATIBLE_REQUEST        (17U)
#define ACMP_SS_NOT_SUPPORTED               (31U)
        UINT32 status : 5;    /**< cf. Table 8.2 of IEEE 1722.1 - 2013 */

/* cf. Table 8.1 of IEEE 1722.1 - 2013 */
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
        UINT32 message_type : 4; /**< ACMP_MSG_XXX, control_data field */
        UINT32 version : 3;      /**< Always set to 0 */
        UINT32 sv : 1;           /**< StreamID Valid indicator: Always set to 0 */
        UINT32 subtype : 8;      /**< Always set to 0x7C,
                                     in 1722-2016, subtype is 1 octet, i.e. including cd bit
                                     and cd bit alwaays be 1
                                  */
        UINT64 stream_id;
    } __attribute__((aligned(4), packed)) avtpdu_hdr;

    /* AVDECC Connection Management Protocol Data Unit (ACMPDU) Format */
    struct {
        UINT64 controller_entity_id;
        UINT64 talker_entity_id;
        UINT64 listener_entity_id;

        UINT16 talker_unique_id;
        UINT16 listener_unique_id;

        UINT8  stream_dest_mac[6];
        UINT16 connection_count;

        UINT16 sequence_id;
/* Table 8.3 flags field */
#define ACMP_FLG_CLASS_B            (0x0001U)
#define ACMP_FLG_FAST_CONNECT       (0x0002U)
#define ACMP_FLG_SAVED_STATE        (0x0004U)
#define ACMP_FLG_STREAMING_WAIT     (0x0008U)
#define ACMP_FLG_SUPPORTS_ENCRYPTED (0x0010U)
#define ACMP_FLG_ENCRYPTED_PDU      (0x0020U)
#define ACMP_FLG_TALKER_FAILED      (0x0040U)
        UINT16 flags;

        UINT16 stream_vlan_id;
        UINT16 reserved;
    } __attribute__((aligned(1), packed)) acmpdu;
} pkt_acmp_t;


#define AVDECC_CONTROLLER (1U)
#define AVDECC_TALKER     (2U)
#define AVDECC_LISTENER   (3U)
#define AVDECC_RESPONDER  (4U)

#if 0
typedef struct {
    message_type         : 4 bits
    status               : 5 bits
    stream_id            : 64 bits
    controller_entity_id : 64 bits
    talker_entity_id     : 64 bits
    listener_entity_id   : 64 bits
    talker_unique_id     : 16 bits
    listener_unique_id   : 16 bits
    stream_dest_mac      : 48 bits
    connection_count     : 16 bits
    sequence_id          : 16 bits
    flags                : 16 bits
    stream_vlan_id       : 16 bits
} ACMPCommandResponse_t;
#endif

typedef struct {
    UINT64 talker_eid;          /**< Talker Entity ID */
    UINT64 stream_id;
    UINT64 controller_eid;      /**< Controller Entity ID */
    UINT16 talker_uid;          /**< talker Unique ID */
    UINT16 stream_vlan_id;
    UINT16 flags;
    UINT8  connected;
    UINT8  stream_dmac[6];      /**< stream destination MAC */
} ListenerStreamInfo_t;

typedef struct ListenerPair_s {
    UINT64                 listener_entity_id;
    UINT16                 listener_unique_id;
    struct ListenerPair_s *next;
} ListenerPair_t;

typedef struct {
    UINT64          stream_id;
    UINT8           stream_dmac[6];
    UINT16          connection_count;
#define MAX_TSI_NUM (1)
    ListenerPair_t  lp[MAX_TSI_NUM];
    UINT16          stream_vlan_id;
} TalkerStreamInfo_t;

/* 8.2.2.2.5 InflightCommand, cf. 9.2.2.3.2.1.2 InflightCommand */
typedef struct InflightCommand_s {
    UINT8  no;                                  /**< No of this InflightCommand_t bag. */
    UINT8  cmd;                                 /**< Command inflight */
    UINT8  retried;                             /**< 1 bit (boolean) indicating if a retry has been sent */
    UINT16 sid;                                 /**< Sequence ID of pkt */
    UINT32 timeout;                             /**< A timer (timeout value) for when the command will timeout */
    avdecc_pkt_t    *pkt;                       /**< Packet that is waiting response */
    struct InflightCommand_s *next;
} InflightCommand_t;

/* Timeout in ms, Table 8.4—ACMP command timeouts */
#define AVDECC_TO_ACMP_CONNECT_TX_COMMAND           (2000u)
#define AVDECC_TO_ACMP_DISCONNECT_TX_COMMAND        (200u)
#define AVDECC_TO_ACMP_GET_TX_STATE_COMMAND         (200u)
#define AVDECC_TO_ACMP_CONNECT_RX_COMMAND           (4500u)
#define AVDECC_TO_ACMP_DISCONNECT_RX_COMMAND        (500u)
#define AVDECC_TO_ACMP_GET_RX_STATE_COMMAND         (200u)
#define AVDECC_TO_ACMP_GET_TX_CONNECTION_COMMAND    (200u)

/* 9.2.1.2.5 AVDECC Command Timeouts */
#define AVDECC_TO_COMMAND                           (250u)

/* 9.2.1.6.7 HDCP APM Message Timeouts */

/* 8.2.2.2.6 ACMPCommandParams */
#if 0
typedef struct {
    message_type       : 4 bits
    talker_entity_id   : 64 bits
    listener_entity_id : 64 bits
    talker_unique_id   : 16 bits
    listener_unique_id : 16 bits
    connection_count   : 16 bits
    flags              : 16 bits
    stream_vlan_id     : 16 bits
} ACMPCommandParams_t;
#endif

/* 8.2.2.5 ACMP Listener State Machine */
typedef struct {
    InflightCommand_t    *ifly_acmp;           /**< cf. Table 8.4—ACMP command timeouts */
    ListenerStreamInfo_t listenerStreamInfo;
    UINT8                rcvdConnectRXCmd;     /**< set to TRUE when the rcvdCmdResp variable is set with a CONNECT_RX_COMMAND ACMPDU */
    UINT8                rcvdDisconnectRXCmd;  /**< set to TRUE when the rcvdCmdResp variable is set with a DISCONNECT_RX_COMMAND ACMPDU */
    UINT8                rcvdConnectTXResp;    /**< set to TRUE when the rcvdCmdResp variable is set with a CONNECT_TX_RESPONSE ACMPDU */
    UINT8                rcvdDisconnectTXResp; /**< set to TRUE when the rcvdCmdResp variable is set with a DISCONNECT_TX_RESPONSE ACMPDU */
    UINT8                rcvdGetRXState;       /**< set to TRUE when the rcvdCmdResp variable is set with a GET_RX_STATE_COMMAND ACMPDU */

    UINT8                state;

    UINT16               sequence_id;          /**< Used for ACMP, 8.2.1.8 stream_id field */

    // validListenerUnique(ListenerUniqueId)
    // listenerIsConnected(command)
    // listenerIsConnectedTo(command)
    // txCommand(messageType, command, retry)

    avdecc_pkt_t *p;

    // Call-Back Functions
    UINT8 (*cbf_connect)(void);
    UINT8 (*cbf_disconnect)(void);
} sm_acm_listener_t;

typedef struct {
    UINT32                role;
    entityInfo_t          *ei;
    UINT16                uid;      /**< Unique ID */
    UINT8                 unused[2];
    UINT64                controller_entity_id; /**< Current/toConnect eid of Controller */

    sm_acm_listener_t     sm;       /**< Listener State Machine */
    /* Packet to be handled. */
    AMBA_KAL_MSG_QUEUE_t  msgQ;     /**< Packets to be handled. */

    /*
     * Call-Back Functions
     */
    void (*cbf_adp_available)(UINT8 idx);

    UINT8                 connected; /**< Connected to talker. */
} avdecc_listener_t;

/* 8.2.2.6 ACMP Talker State Machine */
typedef struct {
    InflightCommand_t  *ifly_acmp;          /**< cf. Table 8.4—ACMP command timeouts */
    TalkerStreamInfo_t talkerStreamInfo;
    UINT8              rcvdConnectTX;       /**< set to TRUE when the rcvdCmdResp variable is set with a CONNECT_TX_COMMAND ACMPDU */
    UINT8              rcvdDisconnectTX;    /**< set to TRUE when the rcvdCmdResp variable is set with a DISCONNECT_TX_COMMAND ACMPDU */
    UINT8              rcvdGetTXState;      /**< set to TRUE when the rcvdCmdResp variable is set with a GET_TX_STATE_COMMAND ACMPDU */
    UINT8              rcvdGetTXConnection; /**< set to TRUE when the rcvdCmdResp variable is set with a GET_TX_CONNECTION_COMMAND ACMPDU */

    UINT8              state;

    UINT16             sequence_id;         /**< Used for ACMP, 8.2.1.8 stream_id field */

    // validTalkerUnique(TalkerUniqueId)
    // connectTalker(command)
    // txResponse(messageType, response, error)
    // disconnectTalker(command)
    // getState(command)
    // getConnection(command)

    avdecc_pkt_t *p;

    // Call-Back Functions
    UINT8 (*cbf_connect)(void);
    UINT8 (*cbf_disconnect)(void);
} sm_acm_talker_t;

typedef struct {
    UINT32                role;
    entityInfo_t          *ei;
    UINT16                uid;      /**< Unique ID */
    UINT8                 unused[2];
    UINT64                controller_entity_id; /**< Current/toConnect eid of Controller */

    sm_acm_talker_t       sm;       /**< Talker State Machine */
    /* Packet to be handled. */
    AMBA_KAL_MSG_QUEUE_t  msgQ;     /**< Packets to be handled. */

    /*
     * Call-Back Functions
     */
    void (*cbf_adp_available)(UINT8 idx);


    UINT8                 connected; /**< Connected to listened. */
} avdecc_talker_t;



/* Clause 9 of IEEE Std 1722.1 - 2013 */

typedef struct {
    /* AVTP Format */
    struct {
        /*
         * cf. The IEEE Std 1722-2011 control AVTPDU
         */
        UINT32 control_data_len : 11; /**< octets following the target_entity_id,
                                            max. 524 */

        UINT32 status : 5;            /**< cf. Table 9.2, 7.126 of IEEE 1722.1 - 2013 */

/* cf. Table 9.1 of IEEE 1722.1 - 2013 */
#define AECP_MSG_AEM_COMMAND             (0U)
#define AECP_MSG_AEM_RESPONSE            (1U)
#define AECP_MSG_ADDRESS_ACCESS_COMMAND  (2U)
#define AECP_MSG_ADDRESS_ACCESS_RESPONSE (3U)
#define AECP_MSG_AVC_COMMAND             (4U)
#define AECP_MSG_AVC_RESPONSE            (5U)
#define AECP_MSG_VENDOR_UNIQUE_COMMAND   (6U)
#define AECP_MSG_VENDOR_UNIQUE_RESPONSE  (7U)
#define AECP_MSG_HDCP_APM_COMMAND        (8U)
#define AECP_MSG_HDCP_APM_RESPONSE       (9U)
#define AECP_MSG_EXTENDED_COMMAND        (14U)
#define AECP_MSG_EXTENDED_RESPONSE       (15U)
        UINT32 message_type : 4; /**< AECP_MSG_ENTITY_XXX, control_data field */
        UINT32 version : 3;      /**< Always set to 0 */
        UINT32 sv : 1;           /**< StreamID Valid indicator: Always set to 0 */
        UINT32 subtype : 8;      /**< Always set to 0x7B,
                                      in 1722-2016, subtype is 1 octet,
                                      i.e. including cd bit,
                                      and always set to 1.
                                  */

        UINT64 target_entity_id; /**< stream_id field */
    } __attribute__((packed)) avtpdu_hdr;

    /*
     * AVDECC Enumeration and Control Protocol Data Unit (AECPDU) Format
     * AECP Common Data part only
     */
    struct {
        UINT64 controller_entity_id;
        UINT16 sequence_id;
    } __attribute__((aligned(1), packed)) aecpdu;
} __attribute__((aligned(1), packed)) pkt_aecp_t;

/*
 * AECP Call-Back Functions
 */
typedef void (*cbf_vendor_unique_command_f)(const void *packet);
extern cbf_vendor_unique_command_f cbf_vendor_unique_command;
typedef struct {
    pkt_aecp_t aecp_hdr;
    UINT8     vendor_protocol_id[6];
    UINT8     payload[1478];
} __attribute__((aligned(1), packed)) pkt_aecp_vendor_t;

/*
 * AECP Message Specific Data
 */
/* Figure 7.36 READ_DESCRIPTOR Command Format */
typedef struct {
#if 0
    UINT16 command_type : 15; /**< Should be READ_DESCRIPTOR */
    UINT16 u : 1; /**< A flag indicating that it is an unsolicited response. */
#else
    UINT16 command_type;
#endif
    UINT16 configuration_index;
    UINT16 reserved;
    UINT16 descriptor_type;
    UINT16 descriptor_index;
} __attribute__((aligned(1), packed)) d_aecp_read_descriptor_t;


/*
 * Figure 9.2—AEM Common Format
 * e.g. For ENTITY_AVAILABLE, ONTROLLER_AVAILABLE, no command_specific_data.
 */
typedef struct {
    pkt_aecp_t aecp_hdr;
    UINT16     command_type; /**< b15 as 'u' bit */
    /* command_specific_data */
} __attribute__((aligned(1), packed)) pkt_aecp_aem_t;


/* Clause 7 for AECP */

/* Figure 7.34—ACQUIRE_ENTITY Command and Response Format */
typedef struct {
    pkt_aecp_t aecp_hdr;
    UINT16     command_type; /**< b15 as 'u' bit */

/* Table 7.127 */
#define AECP_ACQUIRE_FLAG_PERSISTENT (0x0001U)
#define AECP_ACQUIRE_FLAG_RELEASE    (0x8000U)
    UINT32     flags;
    UINT64     owner_id;
    UINT16     descriptor_type; // Table 7.1—Descriptor Types
    UINT16     descriptor_index;
} __attribute__((aligned(1), packed)) pkt_aecp_acquireE_t;


/* Figure 7.35—LOCK_ENTITY Command and Response Format */
typedef struct {
    pkt_aecp_t aecp_hdr;
    UINT16     command_type; /**< b15 as 'u' bit */

/* Table 7.128 */
#define AECP_LOCK_FLAG_UNLOCK (0x0001U)
    UINT32     flags;
    UINT64     locked_id;
    UINT16     descriptor_type; // Table 7.1—Descriptor Types
    UINT16     descriptor_index;
} __attribute__((aligned(1), packed)) pkt_aecp_lockE_t;


/* Figure 7.73—REBOOT Command and Response Format  */
typedef struct {
    pkt_aecp_aem_t aem_hdr;
    UINT16  descriptor_type;
    UINT16  descriptor_index;
} __attribute__((aligned(1), packed)) pkt_aecp_arm_reboot_t;

#define AVDECC_EVENT_AVAILABLE  (0x00000001U)

typedef struct {
    UINT32 Event;
    UINT64 entity_id;
} AVDECC_EVENT_INFO_s;

typedef struct {
    UINT32              valid_time;            /**< valid_time in 2 sec, Time To Live */
    entityInfo_t        entityInfo;
    descriptor_entity_t entity_descriptor;     /**< Entity Descriptor, 7.2.1 */

    void                *jack_descriptor;      /**< Jack Descriptor, 7.2.7 */
    void                *avbif_descriptor;     /**< AVB Interface Descriptor, 7.2.8 */
    void                *logo_descriptor;      /**< Manufacturer Descriptor, 7.2.10 */
    void                *name;
    UINT64              gptp_grandmaster_id;   /* 6.2.1.17 */
    UINT8               gptp_domain_no;        /* 6.2.1.18, not support */
    UINT16              id_control_index;      /* 6.2.1.19 */
    UINT16              interface_index;       /* 6.2.1.20 */
    UINT64              association_id;        /* 6.2.1.21, not support */

    UINT32 (*process)(void *p, UINT32 Idx, const void *frame, UINT16 framelen);

    rcvdEntityInfo_t    rxEInfo[MAX_RX_EI];

    /*
     * ACMP entities
     */
    avdecc_talker_t     *talker;
    avdecc_listener_t   *listener;


    /*
     * Call-back function provided by refcode
     */
    void (*test_tx)(UINT32 Idx, const void *frame, UINT16 framelen);
    UINT8 *(*getSMac)(void);
    UINT64 (*getEID)(void);
    void (*EventCb)(AVDECC_EVENT_INFO_s EventInfo);

    /*
     * Status
     */
    UINT8 doTerminate;  ///< Boolean indicating that a request has been made to terminate.
    UINT8 IsSupportVPN; ///< Boolean indicating VPN support or not.

} AvdeccCfg_t;



#ifdef  __cplusplus
extern  "C" {
#endif

#if 0
static inline void avdecc_errno(const char *fn, const int ln)
{
#if 0
    printf("%s (%d): %s", fn, ln, strerror(errno));
#else
    (void)fn;
    (void)ln;
#endif
}

#define AVDECC_ERRNO() avdecc_errno(__func__, __LINE__)
#endif


/* util.c *criptor_counts_offsetorder64(const UINT64 vi); */
static inline UINT64 order64(const UINT64 vi)
{
    UINT64 vo = ((UINT64)(vi >> 56) |
                 ((vi >> 40) & 0x000000000000FF00U) |
                 ((vi >> 24) & 0x0000000000FF0000U) |
                 ((vi >> 8)  & 0x00000000FF000000U) |
                 ((vi << 8)  & 0x000000FF00000000U) |
                 ((vi << 24) & 0x0000FF0000000000U) |
                 ((vi << 40) & 0x00FF000000000000U) |
                 (vi << 56)
                 );

    return vo;
}

static inline UINT32 order32(const UINT32 vi)
{
    UINT32 vo = ((UINT32)(vi >> 24) |
                 ((vi >> 8) & 0x0000FF00U) |
                 ((vi << 8) & 0x00FF0000U) |
                 ((vi << 24) & 0xFF000000U));

    return vo;
}

static inline UINT16 order16(const UINT16 vi)
{
    UINT16 vo = ((UINT16)(vi >> 8) | ((vi << 8) & 0xFF00U));

    return vo;
}

#ifndef Amba_htons
#define Amba_htons order16
#endif
#ifndef Amba_ntohs
#define Amba_ntohs order16
#endif
#ifndef Amba_htonl
#define Amba_htonl order32
#endif
#ifndef Amba_ntohl
#define Amba_ntohl order32
#endif
#ifndef Amba_htonll
#define Amba_htonll order64
#endif
#ifndef Amba_ntohll
#define Amba_ntohll order64
#endif


/*
 *  avdecc.c
 */
void avdecc_aem_init(AvdeccCfg_t *pCfg, UINT8 t, UINT8 l);



#ifdef  __cplusplus
}
#endif


#endif /* AVDECC_H */

