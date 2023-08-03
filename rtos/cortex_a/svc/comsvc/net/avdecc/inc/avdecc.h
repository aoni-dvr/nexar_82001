/**
 * @file avdecc.h
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
#ifndef AVDECC_H
#define AVDECC_H

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaPrint.h>
#include <AmbaDef.h>
#include <NetStack.h>
#include <AvbStack.h>
#include <AmbaENET.h>

#ifndef TRUE
#define TRUE    (1U)    /**< Boolean : true */
#endif
#ifndef FALSE
#define FALSE   (0U)    /**< Boolean : false */
#endif

#if defined(CONFIG_KAL_THREADX_SMP) && (CONFIG_KAL_THREADX_SMP_NUM_CORES > 1)
#define AVDECC_CORE   (0x00000001)              /**< Default core */
#else
#define AVDECC_CORE   AMBA_KAL_CPU_CORE_MASK    /**< Default core */
#endif

#define AVDECC_CONTROLLER   (0U)    /**< Entity Type: Controller */
#define AVDECC_TALKER       (1U)    /**< Entity Type: Talker */
#define AVDECC_LISTENER     (2U)    /**< Entity Type: Listener */
#define AVDECC_RESPONDER    (3U)    /**< Entity Type: Responder */
#define AVDECC_UNKNOW       (4U)    /**< Entity Type: Unknow */

#define AVDECC_ENTITY_TALKER_MAX_NUM_DEFAULT        (3U)    /**< Default max Talker number */
#define AVDECC_ENTITY_LISTENER_MAX_NUM_DEFAULT      (3U)    /**< Default max Listener number */
#define AVDECC_ENTITY_CONTROLLER_MAX_NUM_DEFAULT    (1U)    /**< Default max Controller number */
#define AVDECC_RX_ENTITY_INFO_MAX_NUM_DEFAULT (AVDECC_ENTITY_TALKER_MAX_NUM_DEFAULT + \
                                               AVDECC_ENTITY_LISTENER_MAX_NUM_DEFAULT + \
                                               AVDECC_ENTITY_CONTROLLER_MAX_NUM_DEFAULT) /**< Default max entity number */

#include "aem.h"
#include "adp.h"
#include "acmp.h"
#include "aecp.h"

#define AVTPDU_SUBTYPE_ADP  ((UINT8)(0x7Au | 0x80u)) /**< AVDECC Discovery Protocol */
#define AVTPDU_SUBTYPE_AECP ((UINT8)(0x7Bu | 0x80u)) /**< AVDECC Enumeration and Control Protocol */
#define AVTPDU_SUBTYPE_ACMP ((UINT8)(0x7Cu | 0x80u)) /**< AVDECC Connection Management Protocol */

#define ADP_MULTICAST_MAC   {0x91, 0xE0, 0xF0, 0x01, 0x00, 0x00}        /**< 91-e0-f0-01-00-00, ADP and ACMP, 6.2.1 and Clause 8 */
#define ACMP_MULTICAST_MAC  {0x91, 0xE0, 0xF0, 0x01, 0x00, 0x00}        /**< 91-e0-f0-01-00-00, ADP and ACMP, 6.2.1 and Clause 8 */
#define AVTP_DST            {0x91U, 0xE0U, 0xF0U, 0x00U, 0x0EU, 0x80U}  /**< 91:e0:f0:00:0e:80 MAAP dynamic allocation pool */

/**
 * Data type of avdecc task information
 */
typedef struct {
    UINT32 Priority;    /**< Task priority */
    UINT32 StackSize;   /**< Task stack size */
} AVDECC_TASK_INFO_s;

/**
 * Data type of avdecc entity desctiptor
 */
typedef struct {
    UINT16 Descriptor_type;         /**< 7.2 - Table 7.1 */
    UINT16 Descriptor_index;        /**< Descriptor index */
    UINT64 Entity_id;               /**< 6.2.1.8 */
    UINT64 Entity_model_id;         /**< 6.2.1.9 */
    UINT32 Entity_capabilities;     /**< 6.2.1.10 - Table 6.2  */
    UINT16 Talker_stream_sources;   /**< 6.2.1.11 */
    UINT16 Talker_capabilities;     /**< 6.2.1.12 - Table 6.3 */
    UINT16 Listener_stream_sinks;   /**< 6.2.1.13 */
    UINT16 Listener_capabilities;   /**< 6.2.1.14 - Table 6.4 */
    UINT32 Controller_capabilities; /**< 6.2.1.15 - Table 6.5 */
    UINT32 Available_index;         /**< 6.2.1.16 */
    UINT64 Association_id;          /**< 6.2.1.21, not support */
    UINT8 Entity_name[64U];         /**< Entity name */
    UINT16 Vendor_name_string;      /**< 7.3.6 */
    UINT16 Model_name_string;       /**< 7.3.6 */
    UINT8 Firmware_version[64U];    /**< Firmware version */
    UINT8 Group_name[64U];          /**< Group name */
    UINT8 Serial_number[64U];       /**< Serial number */
    UINT16 Configurations_count;    /**< Configuration count */
    UINT16 Current_configuration;   /**< Current configuration */
} AVDECC_ENTITY_DESCRIPTOR_s;

/**
 * Data type of avdecc entity information
 */
typedef struct {
    AEM_ENTITY_DESCRIPTOR_s EntityDesc; /**< Entity decriptor */
    ACMPDU_s Acmpdu;                    /**< ACMP data unit */
    void (*VuCmdCb)(const UINT32 Cmd);  /**< Vendor unique command callback */
} AVDECC_ENTITY_INFO_s;

/**
 * Data type of avdecc configuration
 */
typedef struct {
    AVDECC_TASK_INFO_s TaskInfo;                    /**< Task information */
    UINT64 Gptp_grandmaster_id;                     /**< 6.2.1.17 */
    UINT8 Gptp_domain_number;                       /**< 6.2.1.18, not support  */
    UINT16 Identify_control_index;                  /**< 6.2.1.19 */
    UINT16 Interface_index;                         /**< 6.2.1.20 */
    UINT16 UniqueId;                                /**< Unique number for the entity */
    UINT8 Reserved[2U];                             /**< Reserve */
    AVDECC_ENTITY_DESCRIPTOR_s EntityDescriptor;    /**< 7.2.1 Entity Descriptor */
    UINT8 *FrameBuffer;                             /**< Frame buffer for avb stream. */
    UINT32 FrameBufferSize;                         /**< size of avb frame buffer. */
    AMBA_AVB_AVTP_FRAME_INFO_s *FrameInfo;          /**< Frame information for avb frame */
    UINT32 FrameInfoNum;                            /**< Number of frame information */
    void (*VuCmdCb)(const UINT32 Cmd);              /**< Vendor uniqur command callback */
} AVDECC_CFG_s;

/**
 * Data type of avdecc initial configuration
 */
typedef struct {
    UINT32 MaxTalkerNum;            /**< Max Talker number */
    UINT32 MaxListenerNum;          /**< Max Listener number */
    UINT32 MaxControllerNum;        /**< Max Controller number */
    UINT32 MsgNum;                  /**< Number of message of Avdecc */
    AVDECC_TASK_INFO_s TaskInfo;    /**< Task information for Avdecc */
    UINT32 AcmpInflightCmdNum;      /**< Max inflight command number for ACMP */
    UINT32 AcmpMaxConnectNum;       /**< Max connecting number for ACMP */
    UINT32 AecpInflightCmdNum;      /**< Max inflight command number for AECP */
    UINT32 AecpVuCmdNum;            /**< Max vendor unique command number for AECP */
    UINT32 AecpMsgNum;              /**< Max message number for AECP */
    UINT32 AdpMsgNum;               /**< Max message number for ADP */
    UINT8 *Buffer;                  /**< Buffer for Avdecc */
    UINT32 BufferSize;              /**< Buffer size for Avdecc */
    UINT32 Valid_time;              /**< Valid_time in 2 sec, Time To Live, reannounceTimerTimeout 6.2.4.1.1(ADP) */
    UINT8 Mac[6U];                  /**< Mac for Avdecc */
    UINT8 DoTerminate;              /**< Boolean indicating that a request has been made to terminate. */
    UINT8 IsSupportVPN;             /**< Boolean indicating VPN support or not. */
    UINT32 (*EventCb)(const UINT32 Event, UINT64 Info); /**< Event callback */
} AVDECC_INIT_CFG_s;

/**
 *  Reverse the UINT64 order.
 *  @param [in] vi The number for reverse.
 *  @return the number of reversed.
 */
static inline UINT64 Order64(const UINT64 vi)
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

/**
 *  Reverse the UINT32 order.
 *  @param [in] vi The number for reverse.
 *  @return the number of reversed.
 */
static inline UINT32 Order32(const UINT32 vi)
{
    UINT32 vo = ((UINT32)(vi >> 24) |
                 ((vi >> 8) & 0x0000FF00U) |
                 ((vi << 8) & 0x00FF0000U) |
                 ((vi << 24) & 0xFF000000U));
    return vo;
}

/**
 *  Reverse the UINT16 order.
 *  @param [in] vi The number for reverse.
 *  @return the number of reversed.
 */
static inline UINT16 Order16(const UINT16 vi)
{
    UINT16 vo = ((UINT16)(vi >> 8) | ((vi << 8) & 0xFF00U));

    return vo;
}

#define AVDECC_HTONS Order16    /**< convert values between host and network byte order */
#define AVDECC_NTOHS Order16    /**< convert values between host and network byte order */
#define AVDECC_HTONL Order32    /**< convert values between host and network byte order */
#define AVDECC_NTOHL Order32    /**< convert values between host and network byte order */
#define AVDECC_HTONLL Order64   /**< convert values between host and network byte order */
#define AVDECC_NTOHLL Order64   /**< convert values between host and network byte order */

UINT32 Avdecc_GetRequiredBufferSize(const AVDECC_INIT_CFG_s *Cfg, UINT32 *Size);
UINT32 Avdecc_GetDefaultInitCfg(AVDECC_INIT_CFG_s *Cfg);
UINT32 Avdecc_Init(const AVDECC_INIT_CFG_s *Cfg);
UINT32 Avdecc_CreateTalker(const AVDECC_CFG_s *Cfg);
UINT32 Avdecc_DeleteTalker(UINT64 EntityId);
UINT32 Avdecc_CreateListener(const AVDECC_CFG_s *Cfg);
UINT32 Avdecc_DeleteListener(UINT64 EntityId);
UINT32 Avdecc_CreateController(const AVDECC_CFG_s *Cfg);
UINT32 Avdecc_SendAdpCmd(UINT32 Cmd);
UINT32 Avdecc_SendAcmpCmd(const ACMP_COMMAND_PARAMS_s *Cmd, const UINT8 DstMac[6U]);
UINT32 Avdecc_SendAecpCmd(const AEM_CMD_RESP_s *Cmd);
UINT32 Avdecc_ProcessAecpCmdDone(UINT64 EntityId);
UINT32 Avdecc_SendAecpVuCmd(const AECP_VU_AECPDU_s *Cmd);
UINT32 Avdecc_GetEntityList(AVDECC_ENTITY_INFO_s **EntityInfo);
UINT32 Avdecc_GetConnectInfo(UINT64 EntityId, UINT32 *Connect, UINT8 **Info);
UINT32 Avdecc_ShowAvailableEntity(void);
#endif /**< AVDECC_H */

