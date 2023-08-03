/**
 * @file avdecc_private.h
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
#ifndef AVDECC_PRIVATE_H
#define AVDECC_PRIVATE_H

/* util.c */
void Avdecc_PrepareEth(const void *Buffer, UINT8 Vlan, const UINT8 *DstMac, const UINT8 *SrcMac);
UINT16 Avdecc_GetStreamVlanId(void);
UINT16 Avdecc_GetUniqueId(UINT64 EntityId);
UINT8 Avdecc_DoTerminate(void);
UINT8 Avdecc_IsVlan(void);

/* avdecc.c */
typedef struct {
    UINT8 *Buffer;
    UINT32 Size;
    UINT32 Addr;
} AVDECC_BUFFER_MGR;

UINT32 Avdecc_Process(const UINT32 Idx, const UINT8 DstMac[6U], const UINT8 SrcMac[6U],
                      const void *L3Frame, UINT16 L3Len);
UINT32 Avdecc_GetValidTime(UINT32 *Valid_time);
UINT32 Avdecc_GetMac(const UINT8 *Mac);
UINT32 Avdecc_GetEntityNum(UINT32 Role, UINT32 *Num);
UINT32 Avdecc_GetTalkerConfig(UINT64 EntityId, AMBA_AVB_AVTP_TALKER_s **Talker);
UINT32 Avdecc_GetListenerConfig(UINT64 EntityId, AMBA_AVB_AVTP_LISTENER_s **Listener);
UINT32 Avdecc_GetEntityType(UINT64 EntityId, UINT32 *EntityType);

UINT32 Avdecc_NetTx(UINT32 Idx, const void *Data, UINT32 Size, const UINT8 *DstMac);

#define AVDECC_EVENT_START_STREAMING            (0x0000U)
#define AVDECC_EVENT_STOP_STREAMING             (0x0001U)

#define AVDECC_EVENT_MASK_CONTROLL              (0x1000U)
#define AVDECC_EVENT_ENTITY_AVAILABLE           (AVDECC_EVENT_MASK_CONTROLL | 0x0001U)
#define AVDECC_EVENT_ENTITY_DEPARTING           (AVDECC_EVENT_MASK_CONTROLL | 0x0002U)
#define AVDECC_EVENT_CONNECT_SUCCESS            (AVDECC_EVENT_MASK_CONTROLL | 0x0003U)
#define AVDECC_EVENT_CONNECT_ERROR              (AVDECC_EVENT_MASK_CONTROLL | 0x0004U)
#define AVDECC_EVENT_DISCONNECT_SUCCESS         (AVDECC_EVENT_MASK_CONTROLL | 0x0005U)
#define AVDECC_EVENT_DISCONNECT_ERROR           (AVDECC_EVENT_MASK_CONTROLL | 0x0006U)
#define AVDECC_EVENT_START_STREAMING_SUCCESS    (AVDECC_EVENT_MASK_CONTROLL | 0x0007U)
#define AVDECC_EVENT_START_STREAMING_ERROR      (AVDECC_EVENT_MASK_CONTROLL | 0x0008U)
#define AVDECC_EVENT_STOP_STREAMING_SUCCESS     (AVDECC_EVENT_MASK_CONTROLL | 0x0009U)
#define AVDECC_EVENT_STOP_STREAMING_ERROR       (AVDECC_EVENT_MASK_CONTROLL | 0x000AU)
#define AVDECC_EVENT_CMD_TIMEOUT                (AVDECC_EVENT_MASK_CONTROLL | 0x000BU)
UINT32 Avdecc_SendEvent(UINT32 Event, UINT64 Info);

UINT64 Avdecc_GenEntityId(const UINT8 *Mac, UINT16 StreamId);
void Avdecc_EntityId2Mac(UINT64 EntityId, UINT8 *Mac);
void Avdecc_ShowEntityInfo(AEM_ENTITY_DESCRIPTOR_s *EntityDesc);

/* adp.c */
typedef struct {
    UINT32 MaxEntityNum;
    UINT32 MsgNum;
    AVDECC_TASK_INFO_s TaskInfo;
    UINT8 *Buffer;
    UINT32 BufferSize;
} AVDECC_ADP_INIT_CFG_s;
UINT32 Adp_GetRequiredBufferSize(const AVDECC_ADP_INIT_CFG_s *Cfg, UINT32 *Size);
UINT32 Adp_GetDefaultInitCfg(AVDECC_ADP_INIT_CFG_s *Cfg);
UINT32 Adp_Init(const AVDECC_ADP_INIT_CFG_s *Cfg);
UINT32 Adp_AddEntity(const AVDECC_ENTITY_INFO_s *EntityInfo);
UINT32 Adp_RemoveEntity(const AVDECC_ENTITY_INFO_s *EntityInfo);
UINT32 Adp_Process(ADP_PACKET_s *AdpPkt);
UINT32 Adp_GetEntityInfo(ADP_ENTITY_s **EntityInfo);
void Adp_ShowEntityInfo(void);

/* acmp.c */
typedef struct {
    UINT32 InflightCmdNum;
    UINT8 *Buffer;
    UINT32 BufferSize;
} AVDECC_ACMP_INIT_CFG_s;
UINT32 Acmp_GetRequiredBufferSize(const AVDECC_ACMP_INIT_CFG_s *Cfg, UINT32 *Size);
UINT32 Acmp_GetDefaultInitCfg(AVDECC_ACMP_INIT_CFG_s *Cfg);
UINT32 Acmp_Init(const AVDECC_ACMP_INIT_CFG_s *Cfg);
UINT32 Acmp_Process(ACMP_PACKET_s *AcmpPkt);
UINT32 Acmp_PrepareTxCmd(UINT8 MsgType, ACMP_PACKET_s *AcmpPkt, UINT32 Status);

/* aecp.c */
typedef struct {
    UINT32 MaxEntityNum;
    UINT32 MsgNum;
    UINT32 InflightCmdNum;
    UINT32 VuCmdNum;
    AVDECC_TASK_INFO_s TaskInfo;
    UINT8 *Buffer;
    UINT32 BufferSize;
} AVDECC_AECP_INIT_CFG_s;
UINT32 Aecp_GetRequiredBufferSize(const AVDECC_AECP_INIT_CFG_s *Cfg, UINT32 *Size);
UINT32 Aecp_GetDefaultInitCfg(AVDECC_AECP_INIT_CFG_s *Cfg);
UINT32 Aecp_Init(const AVDECC_AECP_INIT_CFG_s *Cfg);
UINT32 Aecp_Process(AECP_PACKET_s *AecpPkt);
UINT32 Aecp_AddEntity(const AVDECC_ENTITY_INFO_s *EntityInfo);
UINT32 Aecp_RemoveEntity(const AVDECC_ENTITY_INFO_s *EntityInfo);
UINT32 Aecp_SendAecpCmd(const AEM_CMD_RESP_s *Cmd);
UINT32 Aecp_ProcessCmdDone(UINT64 EntityId);
UINT32 Aecp_SendAecpVuCmd(const AECP_VU_AECPDU_s *Cmd);

/* talker.c */
typedef struct {
    UINT32 MaxEntityNum;
    UINT32 MsgNum;
    UINT32 MaxConnectNum;
    AVDECC_TASK_INFO_s TaskInfo;
    UINT8 *Buffer;
    UINT32 BufferSize;
} AVDECC_TALKER_INIT_CFG_s;
UINT32 Talker_GetRequiredBufferSize(const AVDECC_TALKER_INIT_CFG_s *Cfg, UINT32 *Size);
UINT32 Talker_GetDefaultInitCfg(AVDECC_TALKER_INIT_CFG_s *Cfg);
UINT32 Talker_Init(const AVDECC_TALKER_INIT_CFG_s *Cfg);
UINT32 Talker_AddEntity(const AVDECC_ENTITY_INFO_s *EntityInfo);
UINT32 Talker_RemoveEntity(const AVDECC_ENTITY_INFO_s *EntityInfo);
UINT32 Talker_SendAcmpPacket(const ACMP_PACKET_s *AcmpPkt);
UINT32 Talker_GetConnectInfo(UINT64 EntityId, UINT32 *Connect, UINT8 **Info);
UINT32 Talker_ResetConnectInfo(UINT64 EntityId);

/* listener.c */
typedef struct {
    UINT32 MaxEntityNum;
    UINT32 MsgNum;
    AVDECC_TASK_INFO_s TaskInfo;
    UINT8 *Buffer;
    UINT32 BufferSize;
} AVDECC_LISTENER_INIT_CFG_s;
UINT32 Listener_GetRequiredBufferSize(const AVDECC_LISTENER_INIT_CFG_s *Cfg, UINT32 *Size);
UINT32 Listener_GetDefaultInitCfg(AVDECC_LISTENER_INIT_CFG_s *Cfg);
UINT32 Listener_Init(const AVDECC_LISTENER_INIT_CFG_s *Cfg);
UINT32 Listener_AddEntity(const AVDECC_ENTITY_INFO_s *EntityInfo);
UINT32 Listener_RemoveEntity(const AVDECC_ENTITY_INFO_s *EntityInfo);
UINT32 Listener_SendAcmpPacket(const ACMP_PACKET_s *AcmpPkt);
UINT32 Listener_GetConnectInfo(UINT64 EntityId, UINT32 *Connect, UINT8 **Info);
UINT32 Listener_ResetConnectInfo(UINT64 EntityId);

/* controller.c */
typedef struct {
    UINT32 MaxEntityNum;
    UINT32 MsgNum;
    AVDECC_TASK_INFO_s TaskInfo;
    UINT8 *Buffer;
    UINT32 BufferSize;
} AVDECC_CONTROLLER_INIT_CFG_s;
UINT32 Ctrller_GetRequiredBufferSize(const AVDECC_CONTROLLER_INIT_CFG_s *Cfg, UINT32 *Size);
UINT32 Ctrller_GetDefaultInitCfg(AVDECC_CONTROLLER_INIT_CFG_s *Cfg);
UINT32 Ctrller_Init(const AVDECC_CONTROLLER_INIT_CFG_s *Cfg);
UINT32 Ctrller_AddEntity(const AVDECC_ENTITY_INFO_s *EntityInfo);
UINT32 Ctrller_RemoveEntity(const AVDECC_ENTITY_INFO_s *EntityInfo);
UINT32 Ctrller_SendAcmpPacket(const ACMP_PACKET_s *AcmpPkt);

#endif /* AVDECC_PRIVATE_H */

