/**
 * @file AvbSvc.c
 *
 * Copyright (c) 2020 Ambarella International LP
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
#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaPrint.h>
#include <AmbaDef.h>
#include <AmbaMisraFix.h>
#include <AmbaUtility.h>
#include "NetStack.h"
#include "AvbSvc.h"
#include "AvbStack.h"
#include "avdecc.h"
#include "avdecc_private.h"

extern AMBA_AVB_STACK_CONFIG_s AvbConfig[ENET_INSTANCES];

#define AVB_SVC_DEFAULT_TASK_PRIORITY   (200U)      /**< Default task priority */
#define AVB_SVC_DEFAULT_TASK_STACK_SIZE (0x2000U)   /**< Default tast stack size */

/**
 * Data type of local entity information
 */
typedef struct {
    UINT64 EntityId;    /**< Entity Id */
    UINT32 StreamType;  /**< Stream type */
} AMBA_AVB_SVC_LOCAL_ENTITY_INFO_s;

/**
 * Data type of message
 */
typedef struct {
    UINT32 Event;   /**< Event */
    UINT64 Info;    /**< Information */
} AMBA_AVB_SVC_MSG_s;

#define AMBA_AVB_SVC_FRAME_QUEUE_NUM (5U)   /**< Number of frame queue */
/**
 * Data type of frame queue
 */
typedef struct {
    UINT64 EntityId;    /**< Entity Id */
    UINT32 FrameNum;    /**< Number frame */
    AMBA_AVB_SVC_FRAME_INFO_s Frames[AMBA_AVB_SVC_FRAME_QUEUE_NUM]; /**< Frame queue */
} AMBA_AVB_SVC_FRAME_QUEUE_s;

/**
 * Data type of AVB SVC manager
 */
typedef struct {
    AMBA_KAL_MUTEX_t Mutex;         /**< Mutex */
    AMBA_KAL_TASK_t Task;           /**< Task */
    AMBA_KAL_MSG_QUEUE_t MsgQueue;  /**< Message queue */
    UINT8 Mac[6U];                  /**< Mac */
    UINT32 MaxTalkerNum;            /**< Max number of Talker */
    UINT32 MaxListenerNum;          /**< Max number of Listener */
    UINT32 MaxControllerNum;        /**< Max number of Controller */
    UINT32 MaxTotalEntityNum;       /**< Max number of total entity */
    UINT32 MaxConnectNum;           /**< Max number of connecting */
    UINT32 TalkerNum;               /**< Created Talker number */
    UINT32 ListenerNum;             /**< Created Listener number */
    UINT32 ControllerNum;           /**< Created Controller number */
    UINT16 StreamId;                /**< Stream Id */
    UINT64 ControllerId;            /**< Controller Id */
    AMBA_AVB_SVC_ENTITY_INFO_s *EntityInfo; /**< Entiry Information of AVB net */
    AMBA_AVB_SVC_LOCAL_ENTITY_INFO_s *LocalEntityInfo;  /**< Local entity information */
    AMBA_AVB_SVC_FRAME_QUEUE_s *FrameQueue; /**< Frame queue for Listener */
    UINT32 (*EventCb)(const UINT32 Event, UINT64 Info); /**< Event callback */
} AMBA_AVB_SVC_MGR_s;

static AMBA_AVB_SVC_MGR_s AvbSvcMgr = {0}; /**< AVB Svc manager */

/**
 *  Set the connect of entity.
 *  @param [in] EntityId       The entity id that want to change to connect
 *  @param [in] RemoteEntityId The remote entity id that connect the entity
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaAvbSvc_SetConnect(UINT64 EntityId, UINT64 RemoteEntityId)
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvbSvcMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        UINT32 Ret;
        for (i = 0U; i < AvbSvcMgr.MaxTotalEntityNum; i++) {
            if (AvbSvcMgr.EntityInfo[i].EntityId != 0U) {
                if (AvbSvcMgr.EntityInfo[i].EntityId == EntityId) {
                    AvbSvcMgr.EntityInfo[i].RemoteEntityId = RemoteEntityId;
                }
                if (AvbSvcMgr.EntityInfo[i].EntityId == RemoteEntityId) {
                    AvbSvcMgr.EntityInfo[i].RemoteEntityId = EntityId;
                }
            }
        }
        Ret = AmbaKAL_MutexGive(&AvbSvcMgr.Mutex);
        if (Ret != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = Ret;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Set the disconnect of entity.
 *  @param [in] EntityId The entity id that want to change to disconnect
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaAvbSvc_SetDisconnect(UINT64 EntityId)
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvbSvcMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        UINT32 Ret;
        for (i = 0U; i < AvbSvcMgr.MaxTotalEntityNum; i++) {
            if (AvbSvcMgr.EntityInfo[i].EntityId != 0U) {
                if (AvbSvcMgr.EntityInfo[i].EntityId == EntityId) {
                    UINT8 j;
                    for (j = 0U; j < AvbSvcMgr.MaxTotalEntityNum; j++) {
                        if (AvbSvcMgr.EntityInfo[j].EntityId == AvbSvcMgr.EntityInfo[i].RemoteEntityId) {
                            AvbSvcMgr.EntityInfo[j].RemoteEntityId = 0U;
                            break;
                        }
                    }
                    AvbSvcMgr.EntityInfo[i].RemoteEntityId = 0U;
                    break;
                }
            }
        }
        Ret = AmbaKAL_MutexGive(&AvbSvcMgr.Mutex);
        if (Ret != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = Ret;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Set the status of entity.
 *  @param [in] EntityId The entity id that want to change status
 *  @param [in] Status   The new status that want to change
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaAvbSvc_SetStatus(UINT64 EntityId, UINT32 Status)
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvbSvcMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        UINT32 Ret;
        for (i = 0U; i < AvbSvcMgr.MaxTotalEntityNum; i++) {
            if (AvbSvcMgr.EntityInfo[i].EntityId == EntityId) {
                AvbSvcMgr.EntityInfo[i].Status = Status;
                break;
            }
        }
        Ret = AmbaKAL_MutexGive(&AvbSvcMgr.Mutex);
        if (Ret != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = Ret;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Get the available entity list.
 *  @param [in]  Hdlr      The Controller handler use to get entity list
 *  @param [out] EntityNum The numbers of entity
 *  @param [out] Info      The list of available entities.
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaAvbSvc_GetEntityList(UINT64 Hdlr, UINT32 *EntityNum, AMBA_AVB_SVC_ENTITY_INFO_s *Info)
{
    UINT32 Rval;
    UINT32 EntityType;
    AmbaMisra_TouchUnused(EntityNum);
    AmbaMisra_TouchUnused(Info);
    Rval = Avdecc_GetEntityType(Hdlr, &EntityType);
    if (Rval == OK) {
        if (EntityType == AVDECC_CONTROLLER) {
            Rval = AmbaKAL_MutexTake(&AvbSvcMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
            if (Rval == KAL_ERR_NONE) {
                UINT8 i;
                UINT32 Num = 0U;
                UINT32 Ret;
                for (i = 0; i < AvbSvcMgr.MaxTotalEntityNum; i++) {
                    if (AvbSvcMgr.EntityInfo[i].EntityId != 0U) {
                        if (AmbaWrap_memcpy(&Info[Num], &AvbSvcMgr.EntityInfo[i], sizeof(AMBA_AVB_SVC_ENTITY_INFO_s))!= 0U) { }
                        Num++;
                    }
                }
                Ret = AmbaKAL_MutexGive(&AvbSvcMgr.Mutex);
                if (Ret != KAL_ERR_NONE) {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
                    Rval = Ret;
                }
                *EntityNum = Num;
            } else {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, cmd deny", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  Connect Listener and Talker
 *  @param [in] Hdlr       The Controller handler use to connect Listener and Talker
 *  @param [in] ListenerId The entity id of Listener
 *  @param [in] TalkerId   The entity id of Talker
 *  @param [in] DstMac     The destination of MAC for streaming
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaAvbSvc_Connect(UINT64 Hdlr, UINT64 ListenerId, UINT64 TalkerId, const UINT8 DstMac[6U])
{
    UINT32 Rval;
    UINT32 EntityType;
    AmbaPrint_PrintStr5("%s", __func__, NULL, NULL, NULL, NULL);
    //1. Controller sends CONNECT_RX_COMMAND to listener
    //2. Listener sends CONNECT_TX_COMMAND to talker
    Rval = Avdecc_GetEntityType(Hdlr, &EntityType);
    if (Rval == OK) {
        if (EntityType == AVDECC_CONTROLLER) {
            ACMP_COMMAND_PARAMS_s CmdParams = {0};
            CmdParams.Message_type = ACMP_MSG_CONNECT_RX_COMMAND;
            CmdParams.Talker_entity_id = TalkerId;
            CmdParams.Listener_entity_id = ListenerId;
            CmdParams.Talker_unique_id = Avdecc_GetUniqueId(TalkerId);
            CmdParams.Listener_unique_id = Avdecc_GetUniqueId(ListenerId);
            Rval = Avdecc_SendAcmpCmd(&CmdParams, DstMac);
            if (Rval == OK) {
                Rval = AmbaAvbSvc_SetConnect(ListenerId, TalkerId);
            }
        } else {
            AmbaPrint_PrintStr5("%s, cmd deny", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  Disconnect Listener and Talker implement
 *  @param [in] ControllerId The controller id that use to send command
 *  @param [in] StreamType   The stream type of the Listener
 *  @param [in] ListenerId   The entity id of Listener
 *  @param [in] TalkerId     The entity id of Talker
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaAvbSvc_DisconnectImpl(UINT64 ControllerId, UINT32 StreamType, UINT64 ListenerId, UINT64 TalkerId)
{
    UINT32 Rval = OK;
    if (ControllerId != 0U) {
        ACMP_COMMAND_PARAMS_s CmdParams = {0};
        UINT8 DstMac[6U] = {0};
        if (StreamType == AMBA_AVB_STREAM_TYPE_INPUT) {
            CmdParams.Message_type = ACMP_MSG_DISCONNECT_RX_COMMAND;
        }
        if (StreamType == AMBA_AVB_STREAM_TYPE_OUTPUT) {
            CmdParams.Message_type = ACMP_MSG_DISCONNECT_TX_COMMAND;
        }
        CmdParams.Talker_entity_id = TalkerId;
        CmdParams.Listener_entity_id = ListenerId;
        CmdParams.Talker_unique_id = Avdecc_GetUniqueId(TalkerId);
        CmdParams.Listener_unique_id = Avdecc_GetUniqueId(ListenerId);
        Rval = Avdecc_SendAcmpCmd(&CmdParams, DstMac);
    }
    return Rval;
}

/**
 *  Disconnect Listener and Talker
 *  @param [in] Hdlr       The Controller handler use to disconnect Listener and Talker
 *  @param [in] ListenerId The entity id of Listener
 *  @param [in] TalkerId   The entity id of Talker
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaAvbSvc_Disconnect(UINT64 Hdlr, UINT64 ListenerId, UINT64 TalkerId)
{
    UINT32 Rval;
    UINT32 EntityType;
    AmbaPrint_PrintStr5("%s", __func__, NULL, NULL, NULL, NULL);
    Rval = Avdecc_GetEntityType(Hdlr, &EntityType);
    if (Rval == OK) {
        if (EntityType == AVDECC_CONTROLLER) {
            //1. Controller sends DISCONNECT_RX_COMMAND to listener
            //2. Listener sends DISCONNECT_TX_COMMAND to talker
            Rval = AmbaAvbSvc_DisconnectImpl(Hdlr, AMBA_AVB_STREAM_TYPE_INPUT, ListenerId, TalkerId);
        } else {
            AmbaPrint_PrintStr5("%s, cmd deny", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  Start streaming implement
 *  @param [in] ControllerId The controller id that use to send command
 *  @param [in] TargetId     The target entity id that receive command
 *  @param [in] DescType     The descript type of AECP command
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaAvbSvc_StartStreamingImpl(UINT64 ControllerId, UINT64 TargetId, UINT16 DescType)
{
    UINT32 Rval = OK;
    if (ControllerId != 0U) {
        AECP_START_STREAMING_s StartStreaming = {0};
        const AECP_START_STREAMING_s *StartStreamingPtr;
        const AEM_CMD_RESP_s *AecpCmd;
        StartStreaming.AecpAem.AecpPkt.AvtpduHeader.Control_data_len = (UINT16)(sizeof(AECP_START_STREAMING_s) - sizeof(AECP_AVTPDU_HEADER_s));
        StartStreaming.AecpAem.AecpPkt.AvtpduHeader.Message_type = AECP_MSG_AEM_COMMAND;
        StartStreaming.AecpAem.AecpPkt.AvtpduHeader.Target_entity_id = TargetId;
        StartStreaming.AecpAem.AecpPkt.Aecpdu.Controller_entity_id = ControllerId;
        StartStreaming.AecpAem.Command_type = AECP_CMD_START_STREAMING;
        StartStreaming.Descriptor_type = DescType;
        StartStreaming.Descriptor_index = 0U;
        StartStreamingPtr = &StartStreaming;
        AmbaMisra_TypeCast(&AecpCmd, &StartStreamingPtr);
        Rval = Avdecc_SendAecpCmd(AecpCmd);
    }
    return Rval;
}

/**
 *  Start streaming
 *  @param [in] Hdlr       The Controller handler use to start streaming
 *  @param [in] ListenerId The entity id of Listener for start streaming
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaAvbSvc_StartStreaming(UINT64 Hdlr, UINT64 ListenerId)
{
    UINT32 Rval;
    UINT32 EntityType;
    AmbaPrint_PrintStr5("%s", __func__, NULL, NULL, NULL, NULL);
    //1. Send 7.4.35 START_STREAMING Command
    Rval = Avdecc_GetEntityType(Hdlr, &EntityType);
    if (Rval == OK) {
        if (EntityType == AVDECC_CONTROLLER) {
            Rval = AmbaKAL_MutexTake(&AvbSvcMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
            if (Rval == KAL_ERR_NONE) {
                UINT8 i;
                UINT32 Ret;
                UINT64 RemoteEntityId = 0U;
                for (i = 0U; i < AvbSvcMgr.MaxTotalEntityNum; i++) {
                    if ((AvbSvcMgr.EntityInfo[i].EntityId != 0U) &&
                        (AvbSvcMgr.EntityInfo[i].EntityId == ListenerId)) {
                        RemoteEntityId = AvbSvcMgr.EntityInfo[i].RemoteEntityId;
                        break;
                    }
                }
                Ret = AmbaKAL_MutexGive(&AvbSvcMgr.Mutex);
                if (Ret == KAL_ERR_NONE) {
                    if (RemoteEntityId != 0U) {
                        /* Let Listener start streaming first */
                        Rval = AmbaAvbSvc_StartStreamingImpl(Hdlr, ListenerId, AEM_DESC_TYPE_STREAM_INPUT);
                    } else {
                        AmbaPrint_PrintStr5("%s, Please connect first!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
                    Rval = Ret;
                }
            } else {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, cmd deny", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  Stop streaming implement
 *  @param [in] ControllerId The controller id that use to send command
 *  @param [in] TargetId     The target entity id that receive command
 *  @param [in] DescType     The descript type of AECP command
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaAvbSvc_StopStreamingImpl(UINT64 ControllerId, UINT64 TargetId, UINT16 DescType)
{
    UINT32 Rval = OK;
    if (ControllerId != 0U) {
        AECP_STOP_STREAMING_s StopStreaming = {0};
        const AECP_STOP_STREAMING_s *StopStreamingPtr;
        const AEM_CMD_RESP_s *AecpCmd;
        StopStreaming.AecpAem.AecpPkt.AvtpduHeader.Control_data_len = (UINT16)(sizeof(AECP_STOP_STREAMING_s) - sizeof(AECP_AVTPDU_HEADER_s));
        StopStreaming.AecpAem.AecpPkt.AvtpduHeader.Message_type = AECP_MSG_AEM_COMMAND;
        StopStreaming.AecpAem.AecpPkt.AvtpduHeader.Target_entity_id = TargetId;
        StopStreaming.AecpAem.AecpPkt.Aecpdu.Controller_entity_id = ControllerId;
        StopStreaming.AecpAem.Command_type = AECP_CMD_STOP_STREAMING;
        StopStreaming.Descriptor_type = DescType;
        StopStreaming.Descriptor_index = 0U;
        StopStreamingPtr = &StopStreaming;
        AmbaMisra_TypeCast(&AecpCmd, &StopStreamingPtr);
        Rval = Avdecc_SendAecpCmd(AecpCmd);
    }
    return Rval;
}

/**
 *  Stop streaming
 *  @param [in] Hdlr       The Controller handler use to stop streaming
 *  @param [in] ListenerId The entity id of Listener for stop streaming
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaAvbSvc_StopStreaming(UINT64 Hdlr, UINT64 ListenerId)
{
    UINT32 Rval;
    UINT32 EntityType;
    AmbaPrint_PrintStr5("%s", __func__, NULL, NULL, NULL, NULL);
    //1. Send 7.4.36 STOP_STREAMING Command
    Rval = Avdecc_GetEntityType(Hdlr, &EntityType);
    if (Rval == OK) {
        if (EntityType == AVDECC_CONTROLLER) {
            Rval = AmbaKAL_MutexTake(&AvbSvcMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
            if (Rval == KAL_ERR_NONE) {
                UINT8 i;
                UINT32 Ret;
                UINT64 RemoteEntityId = 0U;
                for (i = 0U; i < AvbSvcMgr.MaxTotalEntityNum; i++) {
                    if (AvbSvcMgr.EntityInfo[i].EntityId != 0U) {
                        if (AvbSvcMgr.EntityInfo[i].EntityId == ListenerId) {
                            RemoteEntityId = AvbSvcMgr.EntityInfo[i].RemoteEntityId;
                            break;
                        }
                    }
                }
                Ret = AmbaKAL_MutexGive(&AvbSvcMgr.Mutex);
                if (Ret == KAL_ERR_NONE) {
                    if (RemoteEntityId != 0U) {
                        /* Let Talker stop streaming first */
                        Rval = AmbaAvbSvc_StopStreamingImpl(Hdlr, RemoteEntityId, AEM_DESC_TYPE_STREAM_OUTPUT);
                    } else {
                        AmbaPrint_PrintStr5("%s, Please connect first!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
                    Rval = Ret;
                }
            } else {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, cmd deny", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  Send avb frame
 *  @param [in] Hdlr  The Talker handler use to send avb frame
 *  @param [in] Frame The sent avb frame
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaAvbSvc_SendFrame(UINT64 Hdlr, const AMBA_AVB_SVC_FRAME_INFO_s *Frame)
{
    UINT32 Rval;
    UINT32 Connect;
    UINT8 *ConnectList;
    Rval = Avdecc_GetConnectInfo(Hdlr, &Connect, &ConnectList);
    if (Rval == OK) {
        if (Connect == 1U) {
            AMBA_AVB_AVTP_TALKER_s *Talker;
            const AMBA_AVB_AVTP_FRAME_INFO_s *AvtpFrame;
            AmbaMisra_TypeCast(&AvtpFrame, &Frame);
            (void) Avdecc_GetTalkerConfig(Hdlr, &Talker);
            Rval = AmbaAvbStack_AvtpTalkerTx(Talker, AvtpFrame);
        } else {
            AmbaPrint_PrintStr5("Please connect first!", NULL, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("%s, Avdecc_GetConnectInfo() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Peek avb frame implement
 *  @param [in]  Hdlr    The Listener handler use to peek avb frame
 *  @param [in]  Timeout The timeout period for peek frame
 *  @param [out] Frame   The peeked avb frame
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaAvbSvc_PeekFrameImpl(UINT64 Hdlr, const AMBA_AVB_SVC_FRAME_INFO_s *Frame, UINT32 Timeout)
{
    UINT32 Rval;
    AMBA_AVB_AVTP_LISTENER_s *Listener;
    const AMBA_AVB_AVTP_FRAME_INFO_s *AvtpFrame;
    AmbaMisra_TypeCast(&AvtpFrame, &Frame);
    (void) Avdecc_GetListenerConfig(Hdlr, &Listener);
    Rval = AmbaAvbStack_AvtpListenerRx(Listener, AvtpFrame, Timeout);
    return Rval;
}

/**
 *  Peek avb frame
 *  @param [in]  Hdlr    The Listener handler use to peek avb frame
 *  @param [in]  Idx     The index of frame
 *  @param [in]  Timeout The timeout period for peek frame
 *  @param [out] Frame   The peeked avb frame
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaAvbSvc_PeekFrame(UINT64 Hdlr, UINT32 Idx, UINT32 Timeout, AMBA_AVB_SVC_FRAME_INFO_s **Frame)
{
    UINT32 Rval = OK;
    if (Idx < AMBA_AVB_SVC_FRAME_QUEUE_NUM) {
        UINT32 Connect;
        UINT8 *ConnectList;
        Rval = Avdecc_GetConnectInfo(Hdlr, &Connect, &ConnectList);
        if (Rval == OK) {
            if (Connect == 1U) {
                Rval = AmbaKAL_MutexTake(&AvbSvcMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
                if (Rval == KAL_ERR_NONE) {
                    UINT8 i;
                    UINT32 Ret;
                    for (i = 0U; i < AvbSvcMgr.MaxListenerNum; i++) {
                        if ((AvbSvcMgr.FrameQueue[i].EntityId != 0U) &&
                            (AvbSvcMgr.FrameQueue[i].EntityId == Hdlr)) {
                            if (AvbSvcMgr.FrameQueue[i].FrameNum == 0U) {
                                UINT32 j;
                                for (j = 0U; j < (Idx + 1U); j++) {
                                    Rval = AmbaAvbSvc_PeekFrameImpl(Hdlr, &AvbSvcMgr.FrameQueue[i].Frames[j], Timeout);
                                    if (Rval == OK) {
                                        AvbSvcMgr.FrameQueue[i].FrameNum++;
                                    }
                                }
                                *Frame = &AvbSvcMgr.FrameQueue[i].Frames[Idx];
                            } else {
                                if (Idx > (AvbSvcMgr.FrameQueue[i].FrameNum - 1U)) {
                                    UINT32 j;
                                    for (j = (AvbSvcMgr.FrameQueue[i].FrameNum - 1U); j < (Idx + 1U); j++) {
                                        Rval = AmbaAvbSvc_PeekFrameImpl(Hdlr, &AvbSvcMgr.FrameQueue[i].Frames[j], Timeout);
                                        if (Rval == OK) {
                                            *Frame = &AvbSvcMgr.FrameQueue[i].Frames[Idx];
                                            AvbSvcMgr.FrameQueue[i].FrameNum++;
                                        }
                                    }
                                } else {
                                    *Frame = &AvbSvcMgr.FrameQueue[i].Frames[Idx];
                                }
                            }
                            break;
                        }
                    }
                    Ret = AmbaKAL_MutexGive(&AvbSvcMgr.Mutex);
                    if (Ret != KAL_ERR_NONE) {
                        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
                        Rval = Ret;
                    }
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_PrintStr5("%s, Please connect first!", __func__, NULL, NULL, NULL, NULL);
                Rval = ERR_ARG;
            }
        } else {
            AmbaPrint_PrintStr5("%s, Avdecc_GetConnectInfo() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("Idx over max queue size", NULL, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Remove avb frame
 *  @param [in] Hdlr  The Listener handler use to peek avb frame
 *  @param [in] Count The frame number for remove
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaAvbSvc_RemoveFrame(UINT64 Hdlr, UINT32 Count)
{
    UINT32 Rval = OK;
    if (Count <= AMBA_AVB_SVC_FRAME_QUEUE_NUM) {
        UINT32 Connect = 0U;
        UINT8 *ConnectList;
        Rval = Avdecc_GetConnectInfo(Hdlr, &Connect, &ConnectList);
        if (Rval == OK) {
            if (Connect == 1U) {
                Rval = AmbaKAL_MutexTake(&AvbSvcMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
                if (Rval == KAL_ERR_NONE) {
                    UINT8 i;
                    UINT32 Ret;
                    for (i = 0U; i < AvbSvcMgr.MaxListenerNum; i++) {
                        if ((AvbSvcMgr.FrameQueue[i].EntityId != 0U) &&
                            (AvbSvcMgr.FrameQueue[i].EntityId == Hdlr)) {
                            UINT8 j;
                            AMBA_AVB_AVTP_LISTENER_s *Listener;
                            (void) Avdecc_GetListenerConfig(Hdlr, &Listener);
                            for (j = 0U; j < Count; j++) {
                                if (AvbSvcMgr.FrameQueue[i].FrameNum > 0U) {
                                    const AMBA_AVB_SVC_FRAME_INFO_s *AvbSvcFrame = &AvbSvcMgr.FrameQueue[i].Frames[j];
                                    const AMBA_AVB_AVTP_FRAME_INFO_s *AvtpFrame;
                                    AmbaMisra_TypeCast(&AvtpFrame, &AvbSvcFrame);
                                    Rval = AmbaAvbStack_AvtpListenerRxDone(Listener, AvtpFrame);
                                    if (Rval == NET_ERR_NONE) {
                                        AvbSvcMgr.FrameQueue[i].FrameNum--;
                                    } else {
                                        AmbaPrint_PrintStr5("%s, AmbaAvbStack_AvtpListenerRxDone() failed!", __func__, NULL, NULL, NULL, NULL);
                                    }
                                } else {
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    Ret = AmbaKAL_MutexGive(&AvbSvcMgr.Mutex);
                    if (Ret != KAL_ERR_NONE) {
                        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
                        Rval = Ret;
                    }
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_PrintStr5("%s, Please connect first!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, Avdecc_GetConnectInfo() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("Count over max queue size", NULL, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Send vendor unique command implement
 *  @param [in] ControllerId The controller id that use to send command
 *  @param [in] TargetId     The target entity id that receive command
 *  @param [in] Cmd          The vendor unique command
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaAvbSvc_SendVendorCmdImpl(UINT64 ControllerId, UINT64 TargetId, const AMBA_AVB_VENDOR_UNIQUE_CMD_s *Cmd)
{
    UINT32 Rval;
    AECP_VU_AECPDU_s AecpVuCmd;
    AecpVuCmd.AecpPkt.AvtpduHeader.Control_data_len = (UINT16)(sizeof(AECP_VU_AECPDU_s) - sizeof(AECP_AVTPDU_HEADER_s));
    AecpVuCmd.AecpPkt.AvtpduHeader.Message_type = AECP_MSG_VENDOR_UNIQUE_COMMAND;
    AecpVuCmd.AecpPkt.AvtpduHeader.Target_entity_id = TargetId;
    AecpVuCmd.AecpPkt.Aecpdu.Controller_entity_id = ControllerId;
    if (AmbaWrap_memcpy(&AecpVuCmd.VuCmd, Cmd, sizeof(AECP_VU_CMD_s))!= 0U) { }
    Rval = Avdecc_SendAecpVuCmd(&AecpVuCmd);
    return Rval;
}

/**
 *  Send vendor unique command
 *  @param [in] Hdlr The handler for send command
 *  @param [in] Cmd  The vendor unique command
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaAvbSvc_SendVendorCmd(UINT64 Hdlr, const AMBA_AVB_VENDOR_UNIQUE_CMD_s *Cmd)
{
    UINT32 Rval = OK;
    AmbaMisra_TouchUnused(Cmd);
    if (Cmd != NULL) {
        Rval = AmbaKAL_MutexTake(&AvbSvcMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
        if (Rval == KAL_ERR_NONE) {
            UINT8 i;
            UINT32 Ret;
            UINT32 StreamType = AMBA_AVB_STREAM_TYPE_OUTPUT;
            UINT32 Connect = 0U;
            UINT8 *ConnectInfo = NULL;
            for (i = 0U; i < AvbSvcMgr.MaxTotalEntityNum; i++) {
                if ((AvbSvcMgr.LocalEntityInfo[i].EntityId != 0U) &&
                    (AvbSvcMgr.LocalEntityInfo[i].EntityId == Hdlr)) {
                    if (AvbSvcMgr.LocalEntityInfo[i].StreamType == AMBA_AVB_STREAM_TYPE_OUTPUT) {
                        StreamType = AMBA_AVB_STREAM_TYPE_OUTPUT;
                        Rval = Talker_GetConnectInfo(Hdlr, &Connect, &ConnectInfo);
                    }
                    if (AvbSvcMgr.LocalEntityInfo[i].StreamType == AMBA_AVB_STREAM_TYPE_INPUT) {
                        StreamType = AMBA_AVB_STREAM_TYPE_INPUT;
                        Rval = Listener_GetConnectInfo(Hdlr, &Connect, &ConnectInfo);
                    }
                    break;
                }
            }
            Ret = AmbaKAL_MutexGive(&AvbSvcMgr.Mutex);
            if (Ret == KAL_ERR_NONE) {
                if (Connect == 1U) {
                    if (StreamType == AMBA_AVB_STREAM_TYPE_OUTPUT) {
                        const AVDECC_LISTENER_PAIR_s *PairList;
                        AmbaMisra_TypeCast(&PairList, &ConnectInfo);
                        for (i = 0; i < AvbSvcMgr.MaxConnectNum; i++) {
                            if (PairList[i].Listener_entity_id != 0U) {
                                Rval = AmbaAvbSvc_SendVendorCmdImpl(Hdlr, PairList[i].Listener_entity_id, Cmd);
                            }
                        }
                    }
                    if (StreamType == AMBA_AVB_STREAM_TYPE_INPUT) {
                        const AVDECC_LISTENER_STREAM_INFO_s *StreamInfo;
                        AmbaMisra_TypeCast(&StreamInfo, &ConnectInfo);
                        Rval = AmbaAvbSvc_SendVendorCmdImpl(Hdlr, StreamInfo->Talker_entity_id, Cmd);
                    }
                } else {
                    AmbaPrint_PrintStr5("%s, Please connect first!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = Ret;
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  Event callback
 *  @param [in] Event The event that retruned.
 *  @param [in] Info  The information that returned.
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaAvbSvc_EventCb(UINT32 Event, UINT64 Info)
{
    UINT32 Rval = OK;
    if (AvbSvcMgr.EventCb != NULL) {
        AMBA_AVB_SVC_MSG_s Msg;
        Msg.Event = Event;
        Msg.Info = Info;
        Rval = AmbaKAL_MsgQueueSend(&AvbSvcMgr.MsgQueue, &Msg, AMBA_KAL_NO_WAIT);
        if (Rval != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueSend() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  Add entity
 *  @param [in] EntityId The EntityId which want to add.
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaAvbSvc_AddEntity(UINT64 EntityId)
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvbSvcMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        UINT8 Exist = 0U;
        UINT32 Ret;
        for (i = 0U; i < AvbSvcMgr.MaxTotalEntityNum; i++) {
            if (AvbSvcMgr.EntityInfo[i].EntityId == EntityId) {
                Exist = 1U;
                break;
            }
        }
        if (Exist == 0U) {
            for (i = 0U; i < AvbSvcMgr.MaxTotalEntityNum; i++) {
                if (AvbSvcMgr.EntityInfo[i].EntityId == 0U) {
                    AvbSvcMgr.EntityInfo[i].EntityId = EntityId;
                    break;
                }
            }
        }
        Ret = AmbaKAL_MutexGive(&AvbSvcMgr.Mutex);
        if (Ret != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = Ret;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Remove entity
 *  @param [in] EntityId The EntityId which want to remove.
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaAvbSvc_RemoveEntity(UINT64 EntityId)
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvbSvcMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        UINT32 Ret;
        for (i = 0U; i < AvbSvcMgr.MaxTotalEntityNum; i++) {
            if (AvbSvcMgr.EntityInfo[i].EntityId != 0U) {
                if (AvbSvcMgr.EntityInfo[i].EntityId == EntityId) {
                    if (AvbSvcMgr.EntityInfo[i].RemoteEntityId != 0U) {
                        if (AvbSvcMgr.EntityInfo[i].Status == AMBA_AVB_STATUS_STREAMING) { /* Connected, Let remote stop streaming */
                            UINT16 DescType = AEM_DESC_TYPE_STREAM_OUTPUT;
                            if (AvbSvcMgr.EntityInfo[i].StreamType == AMBA_AVB_STREAM_TYPE_OUTPUT) {
                                DescType = AEM_DESC_TYPE_STREAM_INPUT;
                            }
                            Rval = AmbaAvbSvc_StopStreamingImpl(AvbSvcMgr.ControllerId, AvbSvcMgr.EntityInfo[i].RemoteEntityId, DescType);
                        } else { /* local already stop streaming, then disconnect */
                            if (AvbSvcMgr.EntityInfo[i].StreamType == AMBA_AVB_STREAM_TYPE_INPUT) { /* Talker departing, disconnect Listener */
                                Rval = AmbaAvbSvc_DisconnectImpl(AvbSvcMgr.ControllerId, AvbSvcMgr.EntityInfo[i].StreamType,
                                                                AvbSvcMgr.EntityInfo[i].EntityId, AvbSvcMgr.EntityInfo[i].RemoteEntityId);
                            }
                            if (AvbSvcMgr.EntityInfo[i].StreamType == AMBA_AVB_STREAM_TYPE_OUTPUT) { /* Listener departing, disconnect Talker */
                                Rval = AmbaAvbSvc_DisconnectImpl(AvbSvcMgr.ControllerId, AvbSvcMgr.EntityInfo[i].StreamType,
                                                                AvbSvcMgr.EntityInfo[i].RemoteEntityId, AvbSvcMgr.EntityInfo[i].EntityId);
                            }
                        }
                    }
                    if (AmbaWrap_memset(&AvbSvcMgr.EntityInfo[i], 0, sizeof(AMBA_AVB_SVC_ENTITY_INFO_s))!= 0U) { } /* Erase entity info */
                }
            }
        }
        Ret = AmbaKAL_MutexGive(&AvbSvcMgr.Mutex);
        if (Ret != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = Ret;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Avb svc task entry.
 *  @param [in] Info The argument attached to entry function
 *  @return 0 - OK, others - Error
 */
static void *AmbaAvbSvc_TaskEntry(void *Info)
{
    UINT32 Rval = OK;
    AMBA_AVB_SVC_MSG_s Msg;
    (void)Info;
    for (;;) {
        Rval = AmbaKAL_MsgQueueReceive(&AvbSvcMgr.MsgQueue, &Msg, 10U);
        if (Rval == KAL_ERR_NONE) {
            switch (Msg.Event) {
            case AVDECC_EVENT_START_STREAMING :
                Rval = AvbSvcMgr.EventCb(AMBA_AVB_EVENT_START_STREAMING, Msg.Info);
                if (Rval == OK) {
                    Rval = Avdecc_ProcessAecpCmdDone(Msg.Info);
                }
                break;
            case AVDECC_EVENT_STOP_STREAMING :
                Rval = AvbSvcMgr.EventCb(AMBA_AVB_EVENT_STOP_STREAMING, Msg.Info);
                if (Rval == OK) {
                    Rval = Avdecc_ProcessAecpCmdDone(Msg.Info);
                }
                break;
            case AVDECC_EVENT_ENTITY_AVAILABLE :
                if (AmbaAvbSvc_AddEntity(Msg.Info) == OK) {
                    Rval = AvbSvcMgr.EventCb(AMBA_AVB_EVENT_ENTITY_AVAILABLE, Msg.Info);
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaAvbSvc_AddEntity() failed!", __func__, NULL, NULL, NULL, NULL);
                }
                break;
            case AVDECC_EVENT_ENTITY_DEPARTING :
                if (AmbaAvbSvc_RemoveEntity(Msg.Info) == OK) {
                    Rval = AvbSvcMgr.EventCb(AMBA_AVB_EVENT_ENTITY_DEPARTING, Msg.Info);
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaAvbSvc_RemoveEntity() failed!", __func__, NULL, NULL, NULL, NULL);
                }
                break;
            case AVDECC_EVENT_CONNECT_SUCCESS :
                Rval = AvbSvcMgr.EventCb(AMBA_AVB_EVENT_CONNECT_SUCCESS, Msg.Info);
                break;
            case AVDECC_EVENT_CONNECT_ERROR :
                Rval = AmbaAvbSvc_SetDisconnect(Msg.Info);
                if (Rval == OK) {
                    Rval = AvbSvcMgr.EventCb(AMBA_AVB_EVENT_CONNECT_ERROR, Msg.Info);
                }
                break;
            case AVDECC_EVENT_DISCONNECT_SUCCESS :
                Rval = AmbaAvbSvc_SetDisconnect(Msg.Info);
                if (Rval == OK) {
                    Rval = AvbSvcMgr.EventCb(AMBA_AVB_EVENT_DISCONNECT_SUCCESS, Msg.Info);
                }
                break;
            case AVDECC_EVENT_DISCONNECT_ERROR :
                Rval = AmbaAvbSvc_SetDisconnect(Msg.Info);
                if (Rval == OK) {
                    Rval = AvbSvcMgr.EventCb(AMBA_AVB_EVENT_DISCONNECT_ERROR, Msg.Info);
                }
                break;
            case AVDECC_EVENT_START_STREAMING_SUCCESS :
                Rval = AmbaKAL_MutexTake(&AvbSvcMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
                if (Rval == KAL_ERR_NONE) {
                    UINT8 i;
                    UINT32 Ret;
                    for (i = 0U; i < AvbSvcMgr.MaxTotalEntityNum; i++) {
                        if (AvbSvcMgr.EntityInfo[i].EntityId == Msg.Info) {
                            if (AvbSvcMgr.EntityInfo[i].StreamType == AMBA_AVB_STREAM_TYPE_INPUT) { /* Receive Listener start streaming success */
                                Rval = AmbaAvbSvc_StartStreamingImpl(AvbSvcMgr.ControllerId, AvbSvcMgr.EntityInfo[i].RemoteEntityId, AEM_DESC_TYPE_STREAM_OUTPUT);
                            }
                            if (AvbSvcMgr.EntityInfo[i].StreamType == AMBA_AVB_STREAM_TYPE_OUTPUT) { /* Receive Talker start streaming success */
                                Rval = AvbSvcMgr.EventCb(AMBA_AVB_EVENT_START_SUCCESS, Msg.Info);
                            }
                            if (Rval == OK) {
                                Rval = AmbaAvbSvc_SetStatus(Msg.Info, AMBA_AVB_STATUS_STREAMING);
                            }
                            break;
                        }
                    }
                    Ret = AmbaKAL_MutexGive(&AvbSvcMgr.Mutex);
                    if (Ret != KAL_ERR_NONE) {
                        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
                        Rval = Ret;
                    }
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
                }
                break;
            case AVDECC_EVENT_START_STREAMING_ERROR :
                Rval = AvbSvcMgr.EventCb(AMBA_AVB_EVENT_START_ERROR, Msg.Info);
                break;
            case AVDECC_EVENT_STOP_STREAMING_SUCCESS :
                Rval = AmbaKAL_MutexTake(&AvbSvcMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
                if (Rval == KAL_ERR_NONE) {
                    UINT8 i;
                    UINT8 Exist = 0U;
                    UINT32 Ret;
                    for (i = 0U; i < AvbSvcMgr.MaxTotalEntityNum; i++) {
                        if ((AvbSvcMgr.EntityInfo[i].EntityId != 0U) &&
                            (AvbSvcMgr.EntityInfo[i].EntityId == Msg.Info)) {
                            UINT8 j;
                            for (j = 0U; j < AvbSvcMgr.MaxTotalEntityNum; j++) {
                                if ((AvbSvcMgr.EntityInfo[j].EntityId != 0U) &&
                                    (AvbSvcMgr.EntityInfo[j].EntityId == AvbSvcMgr.EntityInfo[i].RemoteEntityId)) {
                                    if (AvbSvcMgr.EntityInfo[j].Status == AMBA_AVB_STATUS_STREAMING) { /* Connected, Let Listener stop streaming */
                                        Rval = AmbaAvbSvc_StopStreamingImpl(AvbSvcMgr.ControllerId, AvbSvcMgr.EntityInfo[j].EntityId, AEM_DESC_TYPE_STREAM_INPUT);
                                    }
                                    Exist = 1U;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    if (Exist == 0U) { /* Entity is departing and it's remote already stop streaming, then disconnect */
                        if (AvbSvcMgr.EntityInfo[(i % AvbSvcMgr.MaxTotalEntityNum)].StreamType == AMBA_AVB_STREAM_TYPE_INPUT) { /* Talker departing, disconnect Listener */
                            Rval = AmbaAvbSvc_DisconnectImpl(AvbSvcMgr.ControllerId, AvbSvcMgr.EntityInfo[(i % AvbSvcMgr.MaxTotalEntityNum)].StreamType,
                                                            AvbSvcMgr.EntityInfo[(i % AvbSvcMgr.MaxTotalEntityNum)].EntityId, AvbSvcMgr.EntityInfo[(i % AvbSvcMgr.MaxTotalEntityNum)].RemoteEntityId);
                        }
                        if (AvbSvcMgr.EntityInfo[(i % AvbSvcMgr.MaxTotalEntityNum)].StreamType == AMBA_AVB_STREAM_TYPE_OUTPUT) { /* Listener departing, disconnect Talker */
                            Rval = AmbaAvbSvc_DisconnectImpl(AvbSvcMgr.ControllerId, AvbSvcMgr.EntityInfo[(i % AvbSvcMgr.MaxTotalEntityNum)].StreamType,
                                                            AvbSvcMgr.EntityInfo[(i % AvbSvcMgr.MaxTotalEntityNum)].RemoteEntityId, AvbSvcMgr.EntityInfo[(i % AvbSvcMgr.MaxTotalEntityNum)].EntityId);
                        }
                    }
                    if (Rval == OK) {
                        Rval = AmbaAvbSvc_SetStatus(Msg.Info, AMBA_AVB_STATUS_IDLE);
                        if (Rval == OK) {
                            Rval = AvbSvcMgr.EventCb(AMBA_AVB_EVENT_STOP_STREAMING_SUCCESS, Msg.Info);
                        }
                    }
                    Ret = AmbaKAL_MutexGive(&AvbSvcMgr.Mutex);
                    if (Ret != KAL_ERR_NONE) {
                        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
                        Rval = Ret;
                    }
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
                }
                break;
            case AVDECC_EVENT_STOP_STREAMING_ERROR :
                Rval = AvbSvcMgr.EventCb(AMBA_AVB_EVENT_STOP_STREAMING_ERROR, Msg.Info);
                break;
            case AVDECC_EVENT_CMD_TIMEOUT :
                Rval = AvbSvcMgr.EventCb(AMBA_AVB_EVENT_CMD_TIMEOUT, Msg.Info);
                break;
            default:
                AmbaPrint_PrintStr5("Not support event!", NULL, NULL, NULL, NULL, NULL);
                break;
            }
        }
    }
}

/**
 *  Create the Talker of AVB service module.
 *  @param [in]  Cfg  The configuration used to create the Talker of AVB service module
 *  @param [out] Hdlr The handler of created Talker
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaAvbSvc_CreateTalker(const AMBA_AVB_SVC_TALKER_CFG_s *Cfg, UINT64 *Hdlr)
{
    UINT32 Rval;
    AmbaMisra_TouchUnused(Cfg);
    AmbaMisra_TouchUnused(Hdlr);
    Rval = AmbaKAL_MutexTake(&AvbSvcMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT32 Ret;
        if (AvbSvcMgr.TalkerNum < AvbSvcMgr.MaxTalkerNum) {
            AVDECC_CFG_s AvdeccCfg = {0};
            AvdeccCfg.UniqueId = AvbSvcMgr.StreamId;
            AvdeccCfg.EntityDescriptor.Descriptor_type = AEM_DESC_TYPE_STREAM_OUTPUT;
            AvdeccCfg.EntityDescriptor.Entity_id = Avdecc_GenEntityId(AvbSvcMgr.Mac, AvdeccCfg.UniqueId);
            AvdeccCfg.EntityDescriptor.Entity_capabilities = (UINT32)((UINT32)ADP_ENTITY_CAP_ADDRESS_ACCESS_SUPPORTED |
                                                                      (UINT32)ADP_ENTITY_CAP_AEM_SUPPORTED |
                                                                      (UINT32)ADP_ENTITY_CAP_VENDOR_UNIQUE_SUPPORTED);
            AvdeccCfg.EntityDescriptor.Talker_capabilities = (UINT16)(ADP_TALKER_CAP_IMPLEMENTED |
                                                                      ADP_TALKER_CAP_OTHER_SOURCE |
                                                                      ADP_TALKER_CAP_AUDIO |
                                                                      ADP_TALKER_CAP_VIDEO);
            AvdeccCfg.VuCmdCb = Cfg->VuCmdCb;
            Rval = Avdecc_CreateTalker(&AvdeccCfg);
            if (Rval == OK) {
                AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].EntityId = AvdeccCfg.EntityDescriptor.Entity_id;
                AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].StreamType = AMBA_AVB_STREAM_TYPE_OUTPUT;
                AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].Status = AMBA_AVB_STATUS_IDLE;
                AvbSvcMgr.LocalEntityInfo[AvbSvcMgr.StreamId].EntityId = AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].EntityId;
                AvbSvcMgr.LocalEntityInfo[AvbSvcMgr.StreamId].StreamType = AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].StreamType;
                *Hdlr = AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].EntityId;
                AvbSvcMgr.TalkerNum++;
                AvbSvcMgr.StreamId++;
            }
        } else {
            AmbaPrint_PrintStr5("TalkerNum > MaxTalkerNum", NULL, NULL, NULL, NULL, NULL);
        }
        Ret = AmbaKAL_MutexGive(&AvbSvcMgr.Mutex);
        if (Ret != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = Ret;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Create the Listener of AVB service module.
 *  @param [in]  Cfg  The configuration used to create the Listener of AVB service module
 *  @param [out] Hdlr The handler of created Listener
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaAvbSvc_CreateListener(const AMBA_AVB_SVC_LISTENER_CFG_s *Cfg, UINT64 *Hdlr)
{
    UINT32 Rval;
    AmbaMisra_TouchUnused(Cfg);
    AmbaMisra_TouchUnused(Hdlr);
    Rval = AmbaKAL_MutexTake(&AvbSvcMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT32 Ret;
        if (AvbSvcMgr.ListenerNum < AvbSvcMgr.MaxListenerNum) {
            AVDECC_CFG_s AvdeccCfg = {0};
            AvdeccCfg.UniqueId = AvbSvcMgr.StreamId;
            AvdeccCfg.EntityDescriptor.Descriptor_type = AEM_DESC_TYPE_STREAM_INPUT;
            AvdeccCfg.EntityDescriptor.Entity_id = Avdecc_GenEntityId(AvbSvcMgr.Mac, AvdeccCfg.UniqueId);
            AvdeccCfg.EntityDescriptor.Entity_capabilities = (UINT32)((UINT32)ADP_ENTITY_CAP_ADDRESS_ACCESS_SUPPORTED |
                                                                      (UINT32)ADP_ENTITY_CAP_AEM_SUPPORTED |
                                                                      (UINT32)ADP_ENTITY_CAP_VENDOR_UNIQUE_SUPPORTED);
            AvdeccCfg.EntityDescriptor.Listener_capabilities = (UINT16)(ADP_LISTENER_CAP_IMPLEMENTED |
                                                                        ADP_LISTENER_CAP_OTHER_SINK |
                                                                        ADP_LISTENER_CAP_AUDIO |
                                                                        ADP_LISTENER_CAP_VIDEO);
            AvdeccCfg.FrameBuffer = Cfg->FrameBuffer;
            AvdeccCfg.FrameBufferSize = Cfg->FrameBufferSize;
            AmbaMisra_TypeCast(&AvdeccCfg.FrameInfo, &Cfg->FrameInfo);
            AvdeccCfg.FrameInfoNum = Cfg->FrameInfoNum;
            AvdeccCfg.VuCmdCb = Cfg->VuCmdCb;
            Rval = Avdecc_CreateListener(&AvdeccCfg);
            if (Rval == OK) {
                UINT8 i;
                AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].EntityId = AvdeccCfg.EntityDescriptor.Entity_id;
                AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].StreamType = AMBA_AVB_STREAM_TYPE_INPUT;
                for (i = 0U; i < AvbSvcMgr.MaxListenerNum; i++) {
                    if (AvbSvcMgr.FrameQueue[i].EntityId == 0U) {
                        AvbSvcMgr.FrameQueue[i].EntityId = AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].EntityId;
                        AvbSvcMgr.FrameQueue[i].FrameNum = 0U;
                        break;
                    }
                }
                AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].Status = AMBA_AVB_STATUS_IDLE;
                AvbSvcMgr.LocalEntityInfo[AvbSvcMgr.StreamId].EntityId = AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].EntityId;
                AvbSvcMgr.LocalEntityInfo[AvbSvcMgr.StreamId].StreamType = AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].StreamType;
                *Hdlr = AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].EntityId;
                AvbSvcMgr.ListenerNum++;
                AvbSvcMgr.StreamId++;
            }
        } else {
            AmbaPrint_PrintStr5("ListenerNum > MaxListenerNum", NULL, NULL, NULL, NULL, NULL);
        }
        Ret = AmbaKAL_MutexGive(&AvbSvcMgr.Mutex);
        if (Ret != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = Ret;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Create the Controller of AVB service module.
 *  @param [in]  Cfg  The configuration used to create the Controller of AVB service module
 *  @param [out] Hdlr The handler of created Controller
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaAvbSvc_CreateController(const AMBA_AVB_SVC_CONTROLLER_CFG_s *Cfg, UINT64 *Hdlr)
{
    UINT32 Rval;
    AmbaMisra_TouchUnused(Cfg);
    AmbaMisra_TouchUnused(Hdlr);
    Rval = AmbaKAL_MutexTake(&AvbSvcMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT32 Ret;
        if (AvbSvcMgr.ControllerNum < AvbSvcMgr.MaxControllerNum) {
            AVDECC_CFG_s AvdeccCfg = {0};
            AvdeccCfg.UniqueId = AvbSvcMgr.StreamId;
            AvdeccCfg.EntityDescriptor.Entity_id = Avdecc_GenEntityId(AvbSvcMgr.Mac, AvdeccCfg.UniqueId);
            AvdeccCfg.EntityDescriptor.Controller_capabilities = ADP_CTRL_CAP_IMPLEMENTED;
            Rval = Avdecc_CreateController(&AvdeccCfg);
            if (Rval == OK) {
                AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].EntityId = AvdeccCfg.EntityDescriptor.Entity_id;
                AvbSvcMgr.ControllerId = AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].EntityId;
                AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].StreamType = (UINT32)AMBA_AVB_STREAM_TYPE_CONTROL;
                AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].Status = AMBA_AVB_STATUS_IDLE;
                AvbSvcMgr.LocalEntityInfo[AvbSvcMgr.StreamId].EntityId = AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].EntityId;
                AvbSvcMgr.LocalEntityInfo[AvbSvcMgr.StreamId].StreamType = AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].StreamType;
                *Hdlr = AvbSvcMgr.EntityInfo[AvbSvcMgr.StreamId].EntityId;
                AvbSvcMgr.ControllerNum++;
                AvbSvcMgr.StreamId++;
            }
        } else {
            AmbaPrint_PrintStr5("ControllerNum > MaxControllerNum", NULL, NULL, NULL, NULL, NULL);
        }
        Ret = AmbaKAL_MutexGive(&AvbSvcMgr.Mutex);
        if (Ret != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = Ret;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Get the required buffer size for initializing the AVB service module.
 *  @param [in]  Cfg  The configuration of the AVB service module
 *  @param [out] Size The required buffer size
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaAvbSvc_GetRequiredBufferSize(const AMBA_AVB_SVC_INIT_CFG_s *Cfg, UINT32 *Size)
{
    UINT32 Rval;
    UINT32 AvdeccBufferSize;
    UINT32 BufferSize = 0U;
    AVDECC_INIT_CFG_s AvdeccInitCfg;
    AvdeccInitCfg.MaxTalkerNum = Cfg->MaxTalkerNum;
    AvdeccInitCfg.MaxListenerNum = Cfg->MaxListenerNum;
    AvdeccInitCfg.MaxControllerNum = Cfg->MaxControllerNum;
    AvdeccInitCfg.MsgNum = Cfg->AvdeccCfg.MsgNum;
    AvdeccInitCfg.TaskInfo.StackSize = Cfg->AvdeccCfg.TaskInfo.StackSize;
    AvdeccInitCfg.AcmpMaxConnectNum = Cfg->AvdeccCfg.AcmpMaxConnectNum;
    AvdeccInitCfg.AecpVuCmdNum = Cfg->AvdeccCfg.AecpVuCmdNum;
    AvdeccInitCfg.AecpMsgNum = Cfg->AvdeccCfg.AecpMsgNum;
    AvdeccInitCfg.AdpMsgNum = Cfg->AvdeccCfg.AdpMsgNum;
    Rval = Avdecc_GetRequiredBufferSize(&AvdeccInitCfg, &AvdeccBufferSize);
    if (Rval == OK) {
        UINT32 MaxMsgNum;
        UINT32 MaxProbEntityNum = AvdeccInitCfg.MaxControllerNum;
        MaxProbEntityNum += (AvdeccInitCfg.MaxTalkerNum * 2U);
        MaxProbEntityNum += (AvdeccInitCfg.MaxListenerNum * 2U);
        MaxMsgNum = (MaxProbEntityNum * AvdeccInitCfg.AecpVuCmdNum);
        MaxMsgNum += (MaxProbEntityNum * AvdeccInitCfg.AecpMsgNum);
        MaxMsgNum += (MaxProbEntityNum * AvdeccInitCfg.AdpMsgNum);
        BufferSize += GetAlignedValU32((MaxMsgNum * sizeof(AMBA_AVB_SVC_MSG_s)), AMBA_CACHE_LINE_SIZE);
        BufferSize += GetAlignedValU32(Cfg->TaskInfo.StackSize, AMBA_CACHE_LINE_SIZE);
        BufferSize += GetAlignedValU32((MaxProbEntityNum * sizeof(AMBA_AVB_SVC_ENTITY_INFO_s)), AMBA_CACHE_LINE_SIZE);
        BufferSize += GetAlignedValU32((MaxProbEntityNum * sizeof(AMBA_AVB_SVC_LOCAL_ENTITY_INFO_s)), AMBA_CACHE_LINE_SIZE);
        BufferSize += GetAlignedValU32((AvdeccInitCfg.MaxListenerNum * sizeof(AMBA_AVB_SVC_FRAME_QUEUE_s)), AMBA_CACHE_LINE_SIZE);
        BufferSize += GetAlignedValU32(AvdeccBufferSize, AMBA_CACHE_LINE_SIZE);
        BufferSize = GetAlignedValU32(BufferSize, AMBA_CACHE_LINE_SIZE);
    }
    *Size = BufferSize;
    return Rval;
}

/**
 *  Get the default configuration for initializing the AVB service module.
 *  @param [out] Cfg The returned configuration of the AVB service module
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaAvbSvc_GetDefaultInitCfg(AMBA_AVB_SVC_INIT_CFG_s *Cfg)
{
    UINT32 Rval;
    AVDECC_INIT_CFG_s AvdeccInitCfg;
    if (AmbaWrap_memset(Cfg, 0, sizeof(AMBA_AVB_SVC_INIT_CFG_s))!= 0U) { }
    Rval = Avdecc_GetDefaultInitCfg(&AvdeccInitCfg);
    if (Rval == OK) {
        Cfg->AvdeccCfg.MsgNum = AvdeccInitCfg.MsgNum;
        Cfg->AvdeccCfg.TaskInfo.Priority = AvdeccInitCfg.TaskInfo.Priority;
        Cfg->AvdeccCfg.TaskInfo.StackSize = AvdeccInitCfg.TaskInfo.StackSize;
        Cfg->AvdeccCfg.AcmpMaxConnectNum = AvdeccInitCfg.AcmpMaxConnectNum;
        Cfg->AvdeccCfg.AecpVuCmdNum = AvdeccInitCfg.AecpVuCmdNum;
        Cfg->AvdeccCfg.AecpMsgNum = AvdeccInitCfg.AecpMsgNum;
        Cfg->AvdeccCfg.AdpMsgNum = AvdeccInitCfg.AdpMsgNum;
    }
    Cfg->MaxTalkerNum = AvdeccInitCfg.MaxTalkerNum;
    Cfg->MaxListenerNum = AvdeccInitCfg.MaxListenerNum;
    Cfg->MaxControllerNum = AvdeccInitCfg.MaxControllerNum;
    Cfg->TaskInfo.Priority = AVB_SVC_DEFAULT_TASK_PRIORITY;
    Cfg->TaskInfo.StackSize = AVB_SVC_DEFAULT_TASK_STACK_SIZE;
    Cfg->Buffer = NULL;
    Rval = AmbaAvbSvc_GetRequiredBufferSize(Cfg, &Cfg->BufferSize);
    return Rval;
}

/**
 *  Initialize the AVB service module.
 *  @param [in] Cfg The configuration used to initialize the AVB service module
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaAvbSvc_Init(const AMBA_AVB_SVC_INIT_CFG_s *Cfg)
{
    static char MutexName[] = "AvbSvcMutex";
    static char MsgQueueName[] = "AvbSvcMsgQueue";
    static char TaskName[] = "AvbSvcTask";
    UINT32 Rval;
    UINT32 BufferSize;
    AmbaPrint_PrintStr5("%s ", __func__, NULL, NULL, NULL, NULL);
    Rval = AmbaAvbSvc_GetRequiredBufferSize(Cfg, &BufferSize);
    if (BufferSize == Cfg->BufferSize) {
        ULONG Addr;
        AmbaMisra_TypeCast(&Addr, &Cfg->Buffer);
        if ((Addr % AMBA_CACHE_LINE_SIZE) == 0U) {
            UINT8 *Buffer = Cfg->Buffer;
            if (AmbaWrap_memset(Buffer, 0, BufferSize)!= 0U) { }
            AvbSvcMgr.MaxTalkerNum = Cfg->MaxTalkerNum;
            AvbSvcMgr.MaxListenerNum = Cfg->MaxListenerNum;
            AvbSvcMgr.MaxControllerNum = Cfg->MaxControllerNum;
            AvbSvcMgr.MaxTotalEntityNum = AvbSvcMgr.MaxControllerNum;
            AvbSvcMgr.MaxTotalEntityNum += (AvbSvcMgr.MaxTalkerNum * 2U);
            AvbSvcMgr.MaxTotalEntityNum += (AvbSvcMgr.MaxListenerNum * 2U);
            AvbSvcMgr.MaxConnectNum = Cfg->AvdeccCfg.AcmpMaxConnectNum;
            if (AmbaWrap_memcpy(AvbSvcMgr.Mac, Cfg->Mac, 6U)!= 0U) { }
            AvbSvcMgr.EventCb = Cfg->EventCb;
            Rval = AmbaKAL_MutexCreate(&AvbSvcMgr.Mutex, MutexName);
            if (Rval == KAL_ERR_NONE) {
                UINT32 MaxMsgNum;
                MaxMsgNum = (AvbSvcMgr.MaxTotalEntityNum * Cfg->AvdeccCfg.AecpVuCmdNum);
                MaxMsgNum += (AvbSvcMgr.MaxTotalEntityNum * Cfg->AvdeccCfg.AecpMsgNum);
                MaxMsgNum += (AvbSvcMgr.MaxTotalEntityNum * Cfg->AvdeccCfg.AdpMsgNum);
                Rval = AmbaKAL_MsgQueueCreate(&AvbSvcMgr.MsgQueue, MsgQueueName,
                                             sizeof(AMBA_AVB_SVC_MSG_s), Buffer,
                                             (MaxMsgNum * sizeof(AMBA_AVB_SVC_MSG_s)));
                if (Rval == KAL_ERR_NONE) {
                    Addr += GetAlignedValU32((MaxMsgNum * sizeof(AMBA_AVB_SVC_MSG_s)), AMBA_CACHE_LINE_SIZE);
                    AmbaMisra_TypeCast(&Buffer, &Addr);
                    Rval = AmbaKAL_TaskCreate(&AvbSvcMgr.Task, TaskName, Cfg->TaskInfo.Priority,
                                             AmbaAvbSvc_TaskEntry, NULL, Buffer,
                                             Cfg->TaskInfo.StackSize,
                                             AMBA_KAL_DONT_START);
                    if (Rval == KAL_ERR_NONE) {
                        Addr += GetAlignedValU32(Cfg->TaskInfo.StackSize, AMBA_CACHE_LINE_SIZE);
                        Rval = AmbaKAL_TaskSetSmpAffinity(&AvbSvcMgr.Task, AVDECC_CORE);
                        if (Rval == KAL_ERR_NONE) {
                            Rval = AmbaKAL_TaskResume(&AvbSvcMgr.Task);
                            if (Rval == KAL_ERR_NONE) {
                                AMBA_ENET_CONFIG_s *EnetConfig;
                                AmbaMisra_TypeCast(&AvbSvcMgr.EntityInfo, &Addr);
                                Addr += GetAlignedValU32((AvbSvcMgr.MaxTotalEntityNum * sizeof(AMBA_AVB_SVC_ENTITY_INFO_s)), AMBA_CACHE_LINE_SIZE);
                                AmbaMisra_TypeCast(&AvbSvcMgr.LocalEntityInfo, &Addr);
                                Addr += GetAlignedValU32((AvbSvcMgr.MaxTotalEntityNum * sizeof(AMBA_AVB_SVC_LOCAL_ENTITY_INFO_s)), AMBA_CACHE_LINE_SIZE);
                                AmbaMisra_TypeCast(&AvbSvcMgr.FrameQueue, &Addr);
                                Addr += GetAlignedValU32((AvbSvcMgr.MaxListenerNum * sizeof(AMBA_AVB_SVC_FRAME_QUEUE_s)), AMBA_CACHE_LINE_SIZE);
                                AmbaMisra_TypeCast(&Buffer, &Addr);
                                (void) AmbaPrint_ModuleSetAllowList(AVB_MODULE_ID, 1U);
                                Rval = AmbaEnet_GetConfig(0U, &EnetConfig);
                                if (Rval == ETH_ERR_NONE) {
                                    AvbConfig[0U].Idx = 0U;
                                    if (AmbaWrap_memcpy(AvbConfig[0U].Mac, EnetConfig->Mac, 6U)!= 0U) { }
                                    AvbConfig[0U].PtpConfig.MstSlv = AVB_PTP_AUTO;
                                    AvbConfig[0U].pAvdeccCb = Avdecc_Process;
                                    Rval = AmbaAvbStack_SetConfig(0U, &AvbConfig[0U]);
                                    if (Rval == NET_ERR_NONE) {
                                        Rval = AmbaAvbStack_EnetInit(&AvbConfig[0U]);
                                        if (Rval == NET_ERR_NONE) {
                                            AVDECC_INIT_CFG_s AvdeccInitCfg = {0};
                                            AvdeccInitCfg.MaxTalkerNum = AvbSvcMgr.MaxTalkerNum;
                                            AvdeccInitCfg.MaxListenerNum = AvbSvcMgr.MaxListenerNum;
                                            AvdeccInitCfg.MaxControllerNum = AvbSvcMgr.MaxControllerNum;
                                            AvdeccInitCfg.MsgNum = Cfg->AvdeccCfg.MsgNum;
                                            AvdeccInitCfg.TaskInfo.Priority = Cfg->AvdeccCfg.TaskInfo.Priority;
                                            AvdeccInitCfg.TaskInfo.StackSize = Cfg->AvdeccCfg.TaskInfo.StackSize;
                                            AvdeccInitCfg.AcmpMaxConnectNum = Cfg->AvdeccCfg.AcmpMaxConnectNum;
                                            AvdeccInitCfg.AecpVuCmdNum = Cfg->AvdeccCfg.AecpVuCmdNum;
                                            AvdeccInitCfg.AecpMsgNum = Cfg->AvdeccCfg.AecpMsgNum;
                                            AvdeccInitCfg.AdpMsgNum = Cfg->AvdeccCfg.AdpMsgNum;
                                            AvdeccInitCfg.Valid_time = 2U;
                                            if (AmbaWrap_memcpy(AvdeccInitCfg.Mac, AvbSvcMgr.Mac, 6U)!= 0U) { }
                                            AvdeccInitCfg.DoTerminate = FALSE;
                                            AvdeccInitCfg.IsSupportVPN = FALSE;
                                            AvdeccInitCfg.EventCb = AmbaAvbSvc_EventCb;
                                            AvdeccInitCfg.Buffer = Buffer;
                                            Rval = Avdecc_GetRequiredBufferSize(&AvdeccInitCfg, &AvdeccInitCfg.BufferSize);
                                            if (Rval == OK) {
                                                Addr += GetAlignedValU32(AvdeccInitCfg.BufferSize, AMBA_CACHE_LINE_SIZE);
                                                AmbaMisra_TypeCast(&Buffer, &Addr);
                                                Rval = Avdecc_Init(&AvdeccInitCfg);
                                            }
                                        }
                                    } else {
                                        AmbaPrint_PrintStr5("%s, AmbaAvbStack_SetConfig() failed!", __func__, NULL, NULL, NULL, NULL);
                                    }
                                } else {
                                    AmbaPrint_PrintStr5("%s, AmbaEnet_GetConfig() failed!", __func__, NULL, NULL, NULL, NULL);
                                }
                            } else {
                                AmbaPrint_PrintStr5("%s, AmbaKAL_TaskResume() failed!", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                            AmbaPrint_PrintStr5("%s, AmbaKAL_TaskSetSmpAffinity() failed!", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_PrintStr5("%s, AmbaKAL_TaskCreate() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueCreate() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexCreate() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, Buffer address not alignd!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("%s, BufferSize != Cfg->BufferSize", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

