/**
 * @file aem.h
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
#ifndef AEM_H
#define AEM_H

/* Clause 7 of IEEE Std 1722.1 - 2013 */
/* Table 7.1 - Descriptor Types */
#define AEM_DESC_TYPE_ENTITY               (0x0000)
#define AEM_DESC_TYPE_CONFIGURATION        (0x0001)
#define AEM_DESC_TYPE_AUDIO_UNIT           (0x0002)
#define AEM_DESC_TYPE_VIDEO_UNIT           (0x0003)
#define AEM_DESC_TYPE_SENSOR_UNIT          (0x0004)
#define AEM_DESC_TYPE_STREAM_INPUT         (0x0005)
#define AEM_DESC_TYPE_STREAM_OUTPUT        (0x0006)
#define AEM_DESC_TYPE_JACK_INPUT           (0x0007)
#define AEM_DESC_TYPE_JACK_OUTPUT          (0x0008)
#define AEM_DESC_TYPE_AVB_INTERFACE        (0x0009)
#define AEM_DESC_TYPE_CLOCK_SOURCE         (0x000A)
#define AEM_DESC_TYPE_MEMORY_OBJECT        (0x000B)
#define AEM_DESC_TYPE_LOCALE               (0x000C)
#define AEM_DESC_TYPE_STRINGS              (0x000D)
#define AEM_DESC_TYPE_STREAM_PORT_INPUT    (0x000E)
#define AEM_DESC_TYPE_STREAM_PORT_OUTPUT   (0x000F)
#define AEM_DESC_TYPE_EXTERNAL_PORT_INPUT  (0x0010)
#define AEM_DESC_TYPE_EXTERNAL_PORT_OUTPUT (0x0011)
#define AEM_DESC_TYPE_INTERNAL_PORT_INPUT  (0x0012)
#define AEM_DESC_TYPE_INTERNAL_PORT_OUTPUT (0x0013)
#define AEM_DESC_TYPE_AUDIO_CLUSTER        (0x0014)
#define AEM_DESC_TYPE_VIDEO_CLUSTER        (0x0015)
#define AEM_DESC_TYPE_SENSOR_CLUSTER       (0x0016)
#define AEM_DESC_TYPE_AUDIO_MAP            (0x0017)
#define AEM_DESC_TYPE_VIDEO_MAP            (0x0018)
#define AEM_DESC_TYPE_SENSOR_MAP           (0x0019)
#define AEM_DESC_TYPE_CONTROL              (0x001A)
#define AEM_DESC_TYPE_SIGNAL_SELECTOR      (0x001B)
#define AEM_DESC_TYPE_MIXER                (0x001C)
#define AEM_DESC_TYPE_MATRIX               (0x001D)
#define AEM_DESC_TYPE_MATRIX_SIGNAL        (0x001E)
#define AEM_DESC_TYPE_SIGNAL_SPLITTER      (0x001F)
#define AEM_DESC_TYPE_SIGNAL_COMBINER      (0x0020)
#define AEM_DESC_TYPE_SIGNAL_DEMULTIPLEXER (0x0021)
#define AEM_DESC_TYPE_SIGNAL_MULTIPLEXER   (0x0022)
#define AEM_DESC_TYPE_SIGNAL_TRANSCODER    (0x0023)
#define AEM_DESC_TYPE_CLOCK_DOMAIN         (0x0024)
#define AEM_DESC_TYPE_CONTROL_BLOCK        (0x0025)
#define AEM_DESC_TYPE_INVALID              (0xFFFF)

/* 7.2.1 - Table 7.2 - ENTITY Descriptor */
typedef struct {
    UINT16 Descriptor_type;         /* 7.2 - Table 7.1 */
    UINT16 Descriptor_index;
    UINT64 Entity_id;               /* 6.2.1.8 */
    UINT64 Entity_model_id;         /* 6.2.1.9 */
    UINT32 Entity_capabilities;     /* 6.2.1.10 - Table 6.2  */
    UINT16 Talker_stream_sources;   /* 6.2.1.11 */
    UINT16 Talker_capabilities;     /* 6.2.1.12 - Table 6.3 */
    UINT16 Listener_stream_sinks;   /* 6.2.1.13 */
    UINT16 Listener_capabilities;   /* 6.2.1.14 - Table 6.4 */
    UINT32 Controller_capabilities; /* 6.2.1.15 - Table 6.5 */
    UINT32 Available_index;         /* 6.2.1.16 */
    UINT64 Association_id;          /* 6.2.1.21, not support */
    UINT8 Entity_name[64U];
    UINT16 Vendor_name_string;      /* 7.3.6 */
    UINT16 Model_name_string;       /* 7.3.6 */
    UINT8 Firmware_version[64U];
    UINT8 Group_name[64U];
    UINT8 Serial_number[64U];
    UINT16 Configurations_count;
    UINT16 Current_configuration;
} AEM_ENTITY_DESCRIPTOR_s;

/* Table 7.125 Command Codes */
#define AECP_CMD_ACQUIRE_ENTITY                         (0x0000U)
#define AECP_CMD_LOCK_ENTITY                            (0x0001U)
#define AECP_CMD_ENTITY_AVAILABLE                       (0x0002U)
#define AECP_CMD_CONTROLLER_AVAILABLE                   (0x0003U)
#define AECP_CMD_READ_DESCRIPTOR                        (0x0004U)
#define AECP_CMD_WRITE_DESCRIPTOR                       (0x0005U)
#define AECP_CMD_SET_CONFIGURATION                      (0x0006U)
#define AECP_CMD_GET_CONFIGURATION                      (0x0007U)
#define AECP_CMD_SET_STREAM_FORMAT                      (0x0008U)
#define AECP_CMD_GET_STREAM_FORMAT                      (0x0009U)
#define AECP_CMD_SET_VIDEO_FORMAT                       (0x000aU)
#define AECP_CMD_GET_VIDEO_FORMAT                       (0x000bU)
#define AECP_CMD_SET_SENSOR_FORMAT                      (0x000cU)
#define AECP_CMD_GET_SENSOR_FORMAT                      (0x000dU)
#define AECP_CMD_SET_STREAM_INFO                        (0x000eU)
#define AECP_CMD_GET_STREAM_INFO                        (0x000fU)
#define AECP_CMD_SET_NAME                               (0x0010U)
#define AECP_CMD_GET_NAME                               (0x0011U)
#define AECP_CMD_SET_ASSOCIATION_ID                     (0x0012U)
#define AECP_CMD_GET_ASSOCIATION_ID                     (0x0013U)
#define AECP_CMD_SET_SAMPLING_RATE                      (0x0014U)
#define AECP_CMD_GET_SAMPLING_RATE                      (0x0015U)
#define AECP_CMD_SET_CLOCK_SOURCE                       (0x0016U)
#define AECP_CMD_GET_CLOCK_SOURCE                       (0x0017U)
#define AECP_CMD_SET_CONTROL                            (0x0018U)
#define AECP_CMD_GET_CONTROL                            (0x0019U)
#define AECP_CMD_INCREMENT_CONTROL                      (0x001aU)
#define AECP_CMD_DECREMENT_CONTROL                      (0x001bU)
#define AECP_CMD_SET_SIGNAL_SELECTOR                    (0x001cU)
#define AECP_CMD_GET_SIGNAL_SELECTOR                    (0x001dU)
#define AECP_CMD_SET_MIXER                              (0x001eU)
#define AECP_CMD_GET_MIXER                              (0x001fU)
#define AECP_CMD_SET_MATRIX                             (0x0020U)
#define AECP_CMD_GET_MATRIX                             (0x0021U)
#define AECP_CMD_START_STREAMING                        (0x0022U)
#define AECP_CMD_STOP_STREAMING                         (0x0023U)
#define AECP_CMD_REGISTER_UNSOLICITED_NOTIFICATION      (0x0024U)
#define AECP_CMD_DEREGISTER_UNSOLICITED_NOTIFICATION    (0x0025U)
#define AECP_CMD_IDENTIFY_NOTIFICATION                  (0x0026U)
#define AECP_CMD_GET_AVB_INFO                           (0x0027U)
#define AECP_CMD_GET_AS_PATH                            (0x0028U)
#define AECP_CMD_GET_COUNTERS                           (0x0029U)
#define AECP_CMD_REBOOT                                 (0x002aU)
#define AECP_CMD_GET_AUDIO_MAP                          (0x002bU)
#define AECP_CMD_ADD_AUDIO_MAPPINGS                     (0x002cU)
#define AECP_CMD_REMOVE_AUDIO_MAPPINGS                  (0x002dU)
#define AECP_CMD_GET_VIDEO_MAP                          (0x002eU)
#define AECP_CMD_ADD_VIDEO_MAPPINGS                     (0x002fU)
#define AECP_CMD_REMOVE_VIDEO_MAPPINGS                  (0x0030U)
#define AECP_CMD_GET_SENSOR_MAP                         (0x0031U)
#define AECP_CMD_ADD_SENSOR_MAPPINGS                    (0x0032U)
#define AECP_CMD_REMOVE_SENSOR_MAPPINGS                 (0x0033U)
#define AECP_CMD_START_OPERATION                        (0x0034U)
#define AECP_CMD_ABORT_OPERATION                        (0x0035U)
#define AECP_CMD_OPERATION_STATUS                       (0x0036U)
#define AECP_CMD_AUTH_ADD_KEY                           (0x0037U)
#define AECP_CMD_AUTH_DELETE_KEY                        (0x0038U)
#define AECP_CMD_AUTH_GET_KEY_LIST                      (0x0039U)
#define AECP_CMD_AUTH_GET_KEY                           (0x003aU)
#define AECP_CMD_AUTH_ADD_KEY_TO_CHAIN                  (0x003bU)
#define AECP_CMD_AUTH_DELETE_KEY_FROM_CHAIN             (0x003cU)
#define AECP_CMD_AUTH_GET_KEYCHAIN_LIST                 (0x003dU)
#define AECP_CMD_AUTH_GET_IDENTITY                      (0x003eU)
#define AECP_CMD_AUTH_ADD_TOKEN                         (0x003fU)
#define AECP_CMD_AUTH_DELETE_TOKEN                      (0x0040U)
#define AECP_CMD_AUTHENTICATE                           (0x0041U)
#define AECP_CMD_DEAUTHENTICATE                         (0x0042U)
#define AECP_CMD_ENABLE_TRANSPORT_SECURITY              (0x0043U)
#define AECP_CMD_DISABLE_TRANSPORT_SECURITY             (0x0044U)
#define AECP_CMD_ENABLE_STREAM_ENCRYPTION               (0x0045U)
#define AECP_CMD_DISABLE_STREAM_ENCRYPTION              (0x0046U)
#define AECP_CMD_SET_MEMORY_OBJECT_LENGTH               (0x0047U)
#define AECP_CMD_GET_MEMORY_OBJECT_LENGTH               (0x0048U)
#define AECP_CMD_SET_STREAM_BACKUP                      (0x0049U)
#define AECP_CMD_GET_STREAM_BACKUP                      (0x004aU)
#define AECP_CMD_EXPANSION                              (0x7fffU)

#endif /* AEM_H */

