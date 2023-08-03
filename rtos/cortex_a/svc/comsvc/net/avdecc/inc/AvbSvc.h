/**
 * @file AvbSvc.h
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
#ifndef AMBA_AVB_SVC_H
#define AMBA_AVB_SVC_H

/**
 * Data type of vendor unique command
 */
typedef struct {
    UINT8 ProtocolId[6U];               /**< Vendor defined protocol */
    UINT8 PayloadSpecificData[508U];    /**< Vendor defined data */
} AMBA_AVB_VENDOR_UNIQUE_CMD_s;

/**
 * Data type of I/O vector
 */
typedef struct {
    UINT8 *pBase;                       /**< Base address of I/O vector */
    UINT32 Len;                         /**< Data length of I/O vector */
} AMBA_AVB_SVC_IOVEC_s;

#define AMBA_AVB_FRAME_TYPE_MJPEG        (0x00U) /**< Frame type, Motion JPG */
#define AMBA_AVB_FRAME_TYPE_H264         (0x01U) /**< Frame type, H264 */
#define AMBA_AVB_FRAME_TYPE_JPEG2000     (0x02U) /**< Frame type, JPG2000 */
#define AMBA_AVB_FRAME_TYPE_ACF_CARINFO  (0x03U) /**< Frame type, AVTP Control Format, car information */
#define AMBA_AVB_FRAME_TYPE_ACF_PSD      (0x04U) /**< Frame type, AVTP Control Format, PSD */
#define AMBA_AVB_FRAME_TYPE_ACF_OWS      (0x05U) /**< Frame type, AVTP Control Format, OWS */
#define AMBA_AVB_FRAME_TYPE_ACF_GRID     (0x06U) /**< Frame type, AVTP Control Format, GRID */

/**
 * Data type of avb frame information
 */
typedef struct {
    AMBA_AVB_SVC_IOVEC_s Vec[2]; /**< I/O vector */
    UINT8 VecNum;               /**< Number of I/O vector */
    UINT32 FrameType;           /**< Frame type */
    UINT32 FrameTimeStamp;      /**< Frame timestamp */
    UINT32 AvtpTimeStamp;       /**< AVTP timestamp */
    UINT8 Completed;            /**< The indicator of complete frame */
} AMBA_AVB_SVC_FRAME_INFO_s;

/**
 * Data type of task information
 */
typedef struct {
    UINT32 Priority;            /**< Task priority */
    UINT32 StackSize;           /**< Task stack size */
} AMBA_AVB_SVC_TASK_INFO_s;

/**
 * Data type of Avdecc configuration
 */
typedef struct {
    UINT32 MsgNum;                      /**< Max message number for Avdecc */
    UINT32 AcmpMaxConnectNum;           /**< Max connecting number for Acmp of Avdecc */
    UINT32 AecpVuCmdNum;                /**< Max vendor unique command number for Aecp of Avdecc */
    UINT32 AecpMsgNum;                  /**< Max message number for Aecp of Avdecc */
    UINT32 AdpMsgNum;                   /**< Max message number for Adp of Avdecc */
    AMBA_AVB_SVC_TASK_INFO_s TaskInfo;   /**< Task information for task of Avdecc */
} AMBA_AVB_SVC_AVDECC_CFG_s;

/**
 * Data type of Avb service initial configuration
 */
typedef struct {
    UINT32 MaxTalkerNum;                /**< Max Talker number */
    UINT32 MaxListenerNum;              /**< Max Listener number */
    UINT32 MaxControllerNum;            /**< Max Controller number */
    UINT8 Mac[6U];                      /**< Mac */
    UINT8 *Buffer;                      /**< Working buffer */
    UINT32 BufferSize;                  /**< Working buffer size */
    AMBA_AVB_SVC_TASK_INFO_s TaskInfo;   /**< Task information for task of Avb service */
    AMBA_AVB_SVC_AVDECC_CFG_s AvdeccCfg; /**< The initial configuration for Avdecc */
    UINT32 (*EventCb)(const UINT32 Event, UINT64 EntityId); /**< Event call back */
} AMBA_AVB_SVC_INIT_CFG_s;

/**
 * Data type of Talker create configuration
 */
typedef struct {
    AMBA_AVB_SVC_TASK_INFO_s TaskInfo;   /**< Task information for task of Talker */
    void (*VuCmdCb)(const UINT32 Cmd);  /**< Vendor Unique Command callback */
} AMBA_AVB_SVC_TALKER_CFG_s;

/**
 * Data type of Listener create configuration
 */
typedef struct {
    AMBA_AVB_SVC_TASK_INFO_s TaskInfo;   /**< Task information for task of Talker */
    UINT8 *FrameBuffer;                 /**< Buffer for avtp receive frame */
    UINT32 FrameBufferSize;             /**< Size of buffer for avtp receive frame */
    AMBA_AVB_SVC_FRAME_INFO_s *FrameInfo;/**< Buffer for avb frame queue */
    UINT32 FrameInfoNum;                /**< Max number for avb frame queue */
    void (*VuCmdCb)(const UINT32 Cmd);  /**< Vendor Unique Command callback */
} AMBA_AVB_SVC_LISTENER_CFG_s;

/**
 * Data type of Controller create configuration
 */
typedef struct {
    AMBA_AVB_SVC_TASK_INFO_s TaskInfo;   /**< Task information for task of Talker */
} AMBA_AVB_SVC_CONTROLLER_CFG_s;

#define AMBA_AVB_STREAM_TYPE_OUTPUT  (0x00U)     /* Stream type: output */
#define AMBA_AVB_STREAM_TYPE_INPUT   (0x01U)     /* Stream type: input */
#define AMBA_AVB_STREAM_TYPE_CONTROL (0x02U)     /* Stream type: control */
#define AMBA_AVB_STREAM_TYPE_INVALID (0xFFU)     /* Invalid stream type */

#define AMBA_AVB_STATUS_IDLE         (0x0000U)   /**< Status: idle */
#define AMBA_AVB_STATUS_STREAMING    (0x0001U)   /**< Status: streaming */

/**
 * Data type of entity information
 */
typedef struct {
    UINT64 EntityId;            /**< Entity Id */
    UINT64 RemoteEntityId;      /**< Connected Entity Id */
    UINT32 StreamType;          /**< Stream type */
    UINT32 Status;              /**< Status */
} AMBA_AVB_SVC_ENTITY_INFO_s;

#define AMBA_AVB_EVENT_START_STREAMING          (0x0000U)   /**< The event raised in a stream is start streaming */
#define AMBA_AVB_EVENT_STOP_STREAMING           (0x0001U)   /**< The event raised in a stream is stop streaming */
#define AMBA_AVB_EVENT_MASK_CONTROLL            (0x1000U)   /**< The event mask for control event */
#define AMBA_AVB_EVENT_ENTITY_AVAILABLE         (AMBA_AVB_EVENT_MASK_CONTROLL | 0x0001U) /**< The event raised in receive a entity available message */
#define AMBA_AVB_EVENT_ENTITY_DEPARTING         (AMBA_AVB_EVENT_MASK_CONTROLL | 0x0002U) /**< The event raised in a entity is departing */
#define AMBA_AVB_EVENT_CONNECT_SUCCESS          (AMBA_AVB_EVENT_MASK_CONTROLL | 0x0003U) /**< The event raised in a connecting is created */
#define AMBA_AVB_EVENT_CONNECT_ERROR            (AMBA_AVB_EVENT_MASK_CONTROLL | 0x0004U) /**< The event raised in can not create the connecting */
#define AMBA_AVB_EVENT_DISCONNECT_SUCCESS       (AMBA_AVB_EVENT_MASK_CONTROLL | 0x0005U) /**< The event raised in a connecting is disconnected */
#define AMBA_AVB_EVENT_DISCONNECT_ERROR         (AMBA_AVB_EVENT_MASK_CONTROLL | 0x0006U) /**< The event raised in can not disconnect a connecting */
#define AMBA_AVB_EVENT_START_SUCCESS            (AMBA_AVB_EVENT_MASK_CONTROLL | 0x0007U) /**< The event raised in a stream start success */
#define AMBA_AVB_EVENT_START_ERROR              (AMBA_AVB_EVENT_MASK_CONTROLL | 0x0008U) /**< The event raised in a stream start fail */
#define AMBA_AVB_EVENT_STOP_STREAMING_SUCCESS   (AMBA_AVB_EVENT_MASK_CONTROLL | 0x0009U) /**< The event raised in a stream stop success */
#define AMBA_AVB_EVENT_STOP_STREAMING_ERROR     (AMBA_AVB_EVENT_MASK_CONTROLL | 0x000AU) /**< The event raised in a stream stop fail */
#define AMBA_AVB_EVENT_CMD_TIMEOUT              (AMBA_AVB_EVENT_MASK_CONTROLL | 0x000BU) /**< The event raised in command timeout */

UINT32 AmbaAvbSvc_GetRequiredBufferSize(const AMBA_AVB_SVC_INIT_CFG_s *Cfg, UINT32 *Size);
UINT32 AmbaAvbSvc_GetDefaultInitCfg(AMBA_AVB_SVC_INIT_CFG_s *Cfg);
UINT32 AmbaAvbSvc_Init(const AMBA_AVB_SVC_INIT_CFG_s *Cfg);
UINT32 AmbaAvbSvc_GetDefaultTalkerCfg(AMBA_AVB_SVC_TALKER_CFG_s *Cfg);
UINT32 AmbaAvbSvc_CreateTalker(const AMBA_AVB_SVC_TALKER_CFG_s *Cfg, UINT64 *Hdlr);
UINT32 AmbaAvbSvc_GetDefaultListenerCfg(AMBA_AVB_SVC_LISTENER_CFG_s *Cfg);
UINT32 AmbaAvbSvc_CreateListener(const AMBA_AVB_SVC_LISTENER_CFG_s *Cfg, UINT64 *Hdlr);
UINT32 AmbaAvbSvc_GetDefaultControllerCfg(AMBA_AVB_SVC_CONTROLLER_CFG_s *Cfg);
UINT32 AmbaAvbSvc_CreateController(const AMBA_AVB_SVC_CONTROLLER_CFG_s *Cfg, UINT64 *Hdlr);
UINT32 AmbaAvbSvc_GetEntityList(UINT64 Hdlr, UINT32 *EntityNum, AMBA_AVB_SVC_ENTITY_INFO_s *Info);
UINT32 AmbaAvbSvc_Connect(UINT64 Hdlr, UINT64 ListenerId, UINT64 TalkerId, const UINT8 DstMac[6U]);
UINT32 AmbaAvbSvc_Disconnect(UINT64 Hdlr, UINT64 ListenerId, UINT64 TalkerId);
UINT32 AmbaAvbSvc_StartStreaming(UINT64 Hdlr, UINT64 ListenerId);
UINT32 AmbaAvbSvc_StopStreaming(UINT64 Hdlr, UINT64 ListenerId);
UINT32 AmbaAvbSvc_SendFrame(UINT64 Hdlr, const AMBA_AVB_SVC_FRAME_INFO_s *Frame);
UINT32 AmbaAvbSvc_PeekFrame(UINT64 Hdlr, UINT32 Idx, UINT32 Timeout, AMBA_AVB_SVC_FRAME_INFO_s **Frame);
UINT32 AmbaAvbSvc_RemoveFrame(UINT64 Hdlr, UINT32 Count);
UINT32 AmbaAvbSvc_SendVendorCmd(UINT64 Hdlr, const AMBA_AVB_VENDOR_UNIQUE_CMD_s *Cmd);

#endif /* AMBA_AVB_SVC_H */
