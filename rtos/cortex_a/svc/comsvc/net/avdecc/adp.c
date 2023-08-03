/**
 * @file adp.c
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

#define ADP_DBG (0)

#define ADP_TASK_PRIORITY_DEFAULT       (200U)
#define ADP_TASK_STACK_SIZE_DEFAULT     (0x2000U)

#define ADP_MSGQ_MAX_NUM_DEFAULT        (16U)

//#define ADP_SM_ADV          (0)
#define ADP_SM_ADVI         (1)
#define ADP_SM_DISCOVERY    (2)
#define ADP_SM_NUM          (3)

#define ADP_ADVE_STATE_INITIALIZE           (0U)
#define ADP_ADVE_STATE_ADVERTISE            (1U)
#define ADP_ADVE_STATE_WAITING              (2U)

#define ADP_ADVI_STATE_INITIALIZE           (0U)
#define ADP_ADVI_STATE_WAITING              (1U)
#define ADP_ADVI_STATE_DEPARTING            (2U)
#define ADP_ADVI_STATE_ADVERTISE            (3U)
#define ADP_ADVI_STATE_RECEIVED_DISCOVER    (4U)
#define ADP_ADVI_STATE_UPDATE_GM            (5U)
#define ADP_ADVI_STATE_LINK_STATE_CHANGE    (6U)

#define ADP_DISCOVERY_STATE_WAITING         (0U)
#define ADP_DISCOVERY_STATE_DISCOVER        (1U)
#define ADP_DISCOVERY_STATE_AVAILABLE       (2U)
#define ADP_DISCOVERY_STATE_DEPARTING       (3U)
#define ADP_DISCOVERY_STATE_TIMEOUT         (4U)

typedef struct ADP_PKT {
    ADP_PACKET_s AdpPkt;
    UINT32 Free;
    struct ADP_PKT *Next;
} ADP_PKT_BUFFER_s;

typedef struct {
    AMBA_KAL_MUTEX_t Mutex;
    AMBA_KAL_TASK_t Task;
    UINT8 *TaskStack;
    UINT32 MsgNum;
    AMBA_KAL_MSG_QUEUE_t *MsgQueue;
    AMBA_KAL_MSG_QUEUE_t *MsgQueuePtr;
    ADP_PACKET_s *MsgQBuffer;
    ADP_PACKET_s *MsgQBufferPtr;
    ADP_PKT_BUFFER_s *PktBuffer;
    ADP_PKT_BUFFER_s *PktBufferPtr;
    UINT32 MaxEntityNum;
    ADP_ADV_ENTITY_SM_s *AdvEntitySm;
    ADP_ADV_INTERFACE_SM_s *AdvInterfaceSm;
    ADP_DISCOVERY_SM_s *DiscoverySm;
} AVDECC_ADP_s;

static AVDECC_ADP_s Adp = {0};

void Adp_ShowEntityInfo(void)
{
    UINT32 i;
    for (i = 0; i < Adp.MaxEntityNum; i++) {
        if (Adp.DiscoverySm[i].EntityInfo.AvtpduHeader.Entity_id != 0U) {
            UINT32 j;
            for (j = 0; j < Adp.MaxEntityNum; j++) {
                const ADP_RCVDENTITYINFO_s *EntityInfo = &Adp.DiscoverySm[i].Entities[j].EntityInfo;
                if (EntityInfo->AdpduPkt.AvtpduHeader.Entity_id != 0U) {
                    UINT32 Tmp1, Tmp2;
                    AmbaPrint_PrintUInt5("------------------------------------------------", 0, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Control_data_len = %u ", EntityInfo->AdpduPkt.AvtpduHeader.Control_data_len, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Valid_time = %u ", EntityInfo->AdpduPkt.AvtpduHeader.Valid_time, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Message_type = %u ", EntityInfo->AdpduPkt.AvtpduHeader.Message_type, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Version = %u ", EntityInfo->AdpduPkt.AvtpduHeader.Version, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Sv = %u ", EntityInfo->AdpduPkt.AvtpduHeader.Sv, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Subtype = %u ", EntityInfo->AdpduPkt.AvtpduHeader.Subtype, 0, 0, 0, 0);
                    Tmp1 = (UINT32)((EntityInfo->AdpduPkt.AvtpduHeader.Entity_id & 0xFFFFFFFF00000000U) >> 32U);
                    Tmp2 = (UINT32)((EntityInfo->AdpduPkt.AvtpduHeader.Entity_id & 0x00000000FFFFFFFFU));
                    AmbaPrint_PrintUInt5("Entity_id = %X%X", Tmp1, Tmp2, 0U, 0U, 0U);
                    Tmp1 = (UINT32)((EntityInfo->AdpduPkt.Adpdu.Entity_model_id & 0xFFFFFFFF00000000U) >> 32U);
                    Tmp2 = (UINT32)((EntityInfo->AdpduPkt.Adpdu.Entity_model_id & 0x00000000FFFFFFFFU));
                    AmbaPrint_PrintUInt5("Entity_model_id = %X%X", Tmp1, Tmp2, 0U, 0U, 0U);
                    AmbaPrint_PrintUInt5("Entity_capabilities = %u ", EntityInfo->AdpduPkt.Adpdu.Entity_capabilities, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Talker_stream_sources = %u ", EntityInfo->AdpduPkt.Adpdu.Talker_stream_sources, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Talker_capabilities = %u ", EntityInfo->AdpduPkt.Adpdu.Talker_capabilities, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Listener_stream_sinks = %u ", EntityInfo->AdpduPkt.Adpdu.Listener_stream_sinks, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Listener_capabilities = %u ", EntityInfo->AdpduPkt.Adpdu.Listener_capabilities, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Controller_capabilities = %u ", EntityInfo->AdpduPkt.Adpdu.Controller_capabilities, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Available_index = %u ", EntityInfo->AdpduPkt.Adpdu.Available_index, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("------------------------------------------------", 0, 0, 0, 0, 0);
                }
            }
        }
    }
}

UINT32 Adp_GetEntityInfo(ADP_ENTITY_s **EntityInfo)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Adp.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        for (i = 0U; i < Adp.MaxEntityNum;i++) {
            if (Adp.DiscoverySm[i].EntityInfo.AvtpduHeader.Entity_id != 0U) {
                *EntityInfo = Adp.DiscoverySm[i].Entities;
                break;
            }
        }
        if (AmbaKAL_MutexGive(&Adp.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    }
    return Rval;
}

static void Adp_PacketBufferInit(void)
{
    UINT32 i;
    for (i = 0U; i < (Adp.MsgNum - 1U); i++) {
        Adp.PktBufferPtr[i].Next = &(Adp.PktBufferPtr[i + 1U]);
    }
    Adp.PktBufferPtr[Adp.MsgNum - 1U].Next = NULL;
}

static ADP_PACKET_s *Adp_AllocatePkt(void)
{
    ADP_PACKET_s *Pkt = NULL;
    if (Adp.PktBufferPtr != NULL) {
        ADP_PKT_BUFFER_s *Buffer = Adp.PktBufferPtr;
        Adp.PktBufferPtr = Adp.PktBufferPtr->Next;
        Buffer->Free = FALSE;
        Pkt = &Buffer->AdpPkt;
    }
    return Pkt;
}

static void Adp_FreePkt(const ADP_PACKET_s *Pkt)
{
    if (Pkt != NULL) {
        ADP_PKT_BUFFER_s *Buffer;
        AmbaMisra_TypeCast(&Buffer, &Pkt);
        if (Buffer->Free == FALSE) {
            Buffer->Free = TRUE;
            Buffer->Next = Adp.PktBufferPtr;
            Adp.PktBufferPtr = Buffer;
        } else {
            AmbaPrint_PrintStr5("%s, Pkt already free!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("%s, Pkt is NULL!", __func__, NULL, NULL, NULL, NULL);
    }
}

static UINT32 Adp_PrepareTxMsg(UINT8 MsgType, const ADP_PACKET_s *EntityInfo, ADP_PACKET_s *AdpPkt)
{
    UINT32 Rval = OK;
    if (AmbaWrap_memset(AdpPkt, 0, sizeof(ADP_PACKET_s))!= 0U) { }
    if (MsgType == ADP_MSG_ENTITY_DISCOVER) { /* AVTP Format */
        AdpPkt->AvtpduHeader.Message_type = ADP_MSG_ENTITY_DISCOVER;
    } else if (MsgType == ADP_MSG_ENTITY_AVAILABLE) {
        AdpPkt->AvtpduHeader.Message_type = ADP_MSG_ENTITY_AVAILABLE;
    } else if (MsgType == ADP_MSG_ENTITY_DEPARTING) {
        AdpPkt->AvtpduHeader.Message_type = ADP_MSG_ENTITY_DEPARTING ;
    } else {
        AmbaPrint_PrintStr5("%s, unsupport message type!", __func__, NULL, NULL, NULL, NULL);
        Rval = ERR_ARG;
    }
    if (Rval == OK) {
        const void *Vptr = &(AdpPkt->AvtpduHeader);
        UINT32 *Ptr;
        AdpPkt->AvtpduHeader.Control_data_len = ADP_CD_LEN;
        AdpPkt->AvtpduHeader.Valid_time = (UINT8)(EntityInfo->AvtpduHeader.Valid_time & 0x1FU);
        AdpPkt->AvtpduHeader.Version = 0;
        AdpPkt->AvtpduHeader.Sv = 0;
        AdpPkt->AvtpduHeader.Subtype = AVTPDU_SUBTYPE_ADP;
        AmbaMisra_TypeCast(&Ptr, &Vptr);
        *Ptr = AVDECC_HTONL(*Ptr);
        AdpPkt->AvtpduHeader.Entity_id = AVDECC_HTONLL(EntityInfo->AvtpduHeader.Entity_id);
        if (MsgType != ADP_MSG_ENTITY_DISCOVER) { /* AVDECC Discovery Protocol Data Unit (ADPDU) Format */
            AdpPkt->Adpdu.Entity_model_id = AVDECC_HTONLL(EntityInfo->Adpdu.Entity_model_id);
            AdpPkt->Adpdu.Entity_capabilities = AVDECC_HTONL(EntityInfo->Adpdu.Entity_capabilities);
            AdpPkt->Adpdu.Talker_capabilities = AVDECC_HTONS(EntityInfo->Adpdu.Talker_capabilities);
            AdpPkt->Adpdu.Talker_stream_sources = AVDECC_HTONS(EntityInfo->Adpdu.Talker_stream_sources);
            AdpPkt->Adpdu.Listener_capabilities = AVDECC_HTONS(EntityInfo->Adpdu.Listener_capabilities);
            AdpPkt->Adpdu.Listener_stream_sinks = AVDECC_HTONS(EntityInfo->Adpdu.Listener_stream_sinks);
            AdpPkt->Adpdu.Controller_capabilities = AVDECC_HTONL(EntityInfo->Adpdu.Controller_capabilities);
            AdpPkt->Adpdu.Available_index = AVDECC_HTONL(EntityInfo->Adpdu.Available_index);
            AdpPkt->Adpdu.Gptp_grandmaster_id = AVDECC_HTONLL(EntityInfo->Adpdu.Gptp_grandmaster_id);
            AdpPkt->Adpdu.Gptp_domain_number = EntityInfo->Adpdu.Gptp_domain_number;
            AdpPkt->Adpdu.Interface_index = AVDECC_HTONS(EntityInfo->Adpdu.Interface_index);
            AdpPkt->Adpdu.Identify_control_index = AVDECC_HTONS(EntityInfo->Adpdu.Identify_control_index);
            AdpPkt->Adpdu.Association_id = AVDECC_HTONLL(EntityInfo->Adpdu.Association_id);
        }
    }
    return Rval;
}

static void Adp_TxMsg(UINT8 MsgType, const ADP_PACKET_s *EntityInfo)
{
    UINT32 Rval;
    UINT16 Size = (UINT16)sizeof(ADP_PACKET_s);
    UINT8 AdpMulticastMac[6U] = ADP_MULTICAST_MAC;
    ADP_PACKET_s AdpPkt;
    const ADP_PACKET_s *Ptr;
    const UINT8 *Buffer;
    Ptr = &AdpPkt;
    AmbaMisra_TypeCast(&Buffer, &Ptr);
    Rval = Adp_PrepareTxMsg(MsgType, EntityInfo, &AdpPkt);
    if (Rval == OK) {
        Rval = Avdecc_NetTx(0U, Buffer, Size, AdpMulticastMac);
        if (Rval != ETH_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaEnet_Tx() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
}

/* 6.2.4.2.1 sendAvailable() */
static void Adp_AdvSendAvailable(void)
{
    UINT8 i;
    for (i = 0; i < Adp.MaxEntityNum; i++) {
        Adp.AdvInterfaceSm[i].DoAdvertise = TRUE;
    }
}

/* 6.2.4 Advertise Entity State Machine */
static void Adp_AdvEntitySm(UINT8 EntityInfoId)
{
    ADP_ADV_ENTITY_SM_s *StateMachine = &Adp.AdvEntitySm[EntityInfoId];
    switch (StateMachine->State) {
        case ADP_ADVE_STATE_INITIALIZE:
            StateMachine->EntityInfo.Adpdu.Available_index = 0U;
            StateMachine->State = ADP_ADVE_STATE_ADVERTISE; /* UCT to ADP_ADVE_STATE_ADVERTISE */
            break;
        case ADP_ADVE_STATE_ADVERTISE:
            /* Do Adp_AdvSendAvailable(); */
            Adp_AdvSendAvailable();
            /* reannounceTimerTimeout = MAX(1, entityInfo.valid_time / 4), valid_time in 2sec. */
            StateMachine->ReannounceTimerTimeout = StateMachine->EntityInfo.AvtpduHeader.Valid_time;
            if (StateMachine->ReannounceTimerTimeout < 4U) {
                StateMachine->ReannounceTimerTimeout = 1U;
            } else {
                StateMachine->ReannounceTimerTimeout /= 4U;
            }
            StateMachine->ReannounceTimerTimeout *= 2U;
            (void)AmbaKAL_GetSysTickCount(&StateMachine->CurrentTime);
            StateMachine->ReannounceTimerTimeout = (StateMachine->ReannounceTimerTimeout * 1000U) + StateMachine->CurrentTime;
            StateMachine->NeedsAdvertise = FALSE;
            StateMachine->State = ADP_ADVE_STATE_WAITING; /* UCT to ADP_ADVE_STATE_WAITING */
            break;
        case ADP_ADVE_STATE_WAITING:
            //TODO: rcvdDiscover = FALSE;
            StateMachine->EntityInfo.Adpdu.Available_index++;
            (void)AmbaKAL_GetSysTickCount(&StateMachine->CurrentTime);
            if ((StateMachine->NeedsAdvertise == TRUE) ||
                (StateMachine->CurrentTime > StateMachine->ReannounceTimerTimeout)) {
                StateMachine->NeedsAdvertise = FALSE;
                StateMachine->State = ADP_ADVE_STATE_ADVERTISE;
            }
            if (StateMachine->DoTerminate == TRUE) {
                StateMachine->DoTerminate = FALSE;
                StateMachine->State = ADP_ADVE_STATE_ADVERTISE;
            }
            break;
        default:
            /* TODO */
            break;
    }
}

/* 6.2.5.2.1 txEntityAvailable() */
static void Adp_AdvITxEntityAvailable(UINT8 EntityNo)
{
    ADP_PACKET_s *EntityInfo = &Adp.AdvInterfaceSm[EntityNo].EntityInfo;
    Adp_TxMsg(ADP_MSG_ENTITY_AVAILABLE, EntityInfo);
    EntityInfo->Adpdu.Available_index += 1U; /* 6.2.1.16 incremented after transmitting an ENTITY_AVAILABLE message */
}

/* 6.2.5.2.2 txEntityDeparting() */
static void Adp_AdvITxEntityDeparting(UINT8 EntityNo)
{
    const ADP_PACKET_s *EntityInfo = &Adp.AdvInterfaceSm[EntityNo].EntityInfo;
    Adp_TxMsg(ADP_MSG_ENTITY_DEPARTING, EntityInfo);
}

/* 6.2.5 Advertise Interface State Machine */
static void Adp_AdvInterfaceSm(UINT8 EntityInfoId)
{
    ADP_ADV_ENTITY_SM_s *AdvStateMachine = &Adp.AdvEntitySm[EntityInfoId];
    ADP_ADV_INTERFACE_SM_s *StateMachine = &Adp.AdvInterfaceSm[EntityInfoId];
    AMBA_ENET_CONFIG_s *EnetConfig = NULL;
    switch (StateMachine->State) {
        case ADP_ADVI_STATE_INITIALIZE:
            StateMachine->LastLinkIsUp = FALSE;
            //StateMachine->AdvertisedGrandmasterID = CurrentGrandmasterID; // TODO:
            StateMachine->State = ADP_ADVI_STATE_WAITING; /* UCT to ADP_ADVI_STATE_WAITING */
            break;
        case ADP_ADVI_STATE_WAITING:
            StateMachine->RcvdDiscover = FALSE;
            StateMachine->EntityInfo.Adpdu.Available_index += 1U;
            (void) AmbaEnet_GetConfig(0U, &EnetConfig);
            if (EnetConfig->LinkSpeed != 0U) {
                StateMachine->LinkIsUp = TRUE;
            } else {
                StateMachine->LinkIsUp = FALSE;
            }
            if (StateMachine->DoTerminate == TRUE) {
                StateMachine->DoTerminate = FALSE;
                StateMachine->State = ADP_ADVI_STATE_DEPARTING;
            } else if (StateMachine->DoAdvertise == TRUE) {
                StateMachine->DoAdvertise = FALSE;
                StateMachine->State = ADP_ADVI_STATE_ADVERTISE;
            } else if (StateMachine->RcvdDiscover == TRUE) {
                StateMachine->RcvdDiscover = FALSE;
                StateMachine->State = ADP_ADVI_STATE_RECEIVED_DISCOVER;
            } else if (StateMachine->LastLinkIsUp != StateMachine->LinkIsUp) {
                StateMachine->State = ADP_ADVI_STATE_LINK_STATE_CHANGE;
#if 0 // TODO:
            } else if (currentGrandmasterID != AvdeccAdp.AdpAdvInterfaceSm.AdvertisedGrandmasterID) {
                AvdeccAdp.AdpAdvInterfaceSm.State = ADP_ADVI_STATE_UPDATE_GM;
#endif
            } else {
                // TODO:
            }
            break;
        case ADP_ADVI_STATE_DEPARTING:
            Adp_AdvITxEntityDeparting(EntityInfoId);
            //TODO: task suspend
            break;
        case ADP_ADVI_STATE_ADVERTISE:
            Adp_AdvITxEntityAvailable(EntityInfoId);
            StateMachine->State = ADP_ADVI_STATE_WAITING; /* UCT to ADP_ADVI_STATE_WAITING */
            break;
        case ADP_ADVI_STATE_RECEIVED_DISCOVER:
            if ((StateMachine->EntityId == 0U) ||
                (StateMachine->EntityId == StateMachine->EntityInfo.AvtpduHeader.Entity_id)) {
                AdvStateMachine->NeedsAdvertise = TRUE;
            }
            StateMachine->State = ADP_ADVI_STATE_WAITING; /* UCT to ADP_ADVI_STATE_WAITING */
            break;
        case ADP_ADVI_STATE_UPDATE_GM:
            // TODO: AvdeccAdp.AdpAdvInterfaceSm.advertisedGrandmasterID = currentGrandmasterID;
            AdvStateMachine->NeedsAdvertise = TRUE;
            StateMachine->State = ADP_ADVI_STATE_WAITING; /* UCT to ADP_ADVI_STATE_WAITING */
            break;
        case ADP_ADVI_STATE_LINK_STATE_CHANGE:
            StateMachine->LastLinkIsUp = StateMachine->LinkIsUp;
            if (StateMachine->LinkIsUp == TRUE) {
                AdvStateMachine->NeedsAdvertise = TRUE;
            } else {
                /* Timeout will remove listed entities */
                /* TODO: should notification?! */
            }
            StateMachine->State = ADP_ADVI_STATE_WAITING; /* UCT to ADP_ADVI_STATE_WAITING */
            break;
        default:
            /* TODO */
            break;
    }
}

/* 6.2.6.2.1 performDiscover */
#if 0
static void Adp_PerformDiscover(void)
{
    UINT8 i = 0U;
    for (i = 0U; i < AVDECC_ENTITY_CONTROLLER_MAX_NUM; i++) {
        AvdeccAdp.AdpDiscoverySm[i].DoDiscover = TRUE;
        AvdeccAdp.AdpDiscoverySm[i].DiscoverID = 0U;
    }
}
#endif

/* 6.2.6.3.1 txDiscover(entityID) */
static void Adp_DiscoveryTxDiscover(UINT8 EntityInfoId, UINT64 EntityId)
{
    ADP_PACKET_s *EntityInfo = &Adp.DiscoverySm[EntityInfoId].EntityInfo;
    EntityInfo->AvtpduHeader.Entity_id = EntityId;
    // EntityId: 0 to look for all devices Or specific one.
    Adp_TxMsg(ADP_MSG_ENTITY_DISCOVER, EntityInfo);
    // TBD
}

/* 6.2.6.3.2 haveEntity(entityID) */
static UINT32 Adp_DiscoveryHaveEntity(const ADP_ENTITY_s *EntityInfo, UINT64 EntityId)
{
    UINT8 i;
    UINT8 Have = 0U;
    for (i = 0U; i < (Adp.MaxEntityNum * 2U); i++) {
        if ((EntityInfo[i].EntityInfo.AdpduPkt.AvtpduHeader.Entity_id != 0U) &&
            (EntityInfo[i].EntityInfo.AdpduPkt.AvtpduHeader.Entity_id == EntityId)) {
            Have = 1U;
        }
    }
    return Have;
}

/* 6.2.6.3.3 updateEntity(entityInfo) */
static UINT32 Adp_DiscoveryUpdateEntity(const ADP_PACKET_s *AdpPkt, UINT32 Idx, ADP_ENTITY_s *Entities)
{
    UINT8 i;
    UINT32 Rval = OK;
    for (i = 0U; i < (Adp.MaxEntityNum * 2U); i++) {
        if (Entities[i].EntityInfo.AdpduPkt.AvtpduHeader.Entity_id == AdpPkt->AvtpduHeader.Entity_id) {
            if (AmbaWrap_memcpy(&Entities[i].EntityInfo.AdpduPkt, AdpPkt, sizeof(ADP_PACKET_s))!= 0U) { }
            Entities[i].EntityInfo.Port = (UINT8)(Idx & 0xFFU);
            (void)AmbaKAL_GetSysTickCount(&Entities[i].Timer);
        }
    }
    return Rval;
}

/* 6.2.6.3.4 addEntity(entityInfo) */
static UINT32 Adp_DiscoveryAddEntity(const ADP_PACKET_s *AdpPkt, UINT32 Idx, ADP_ENTITY_s *Entities)
{
    UINT8 i;
    UINT32 Rval = OK;
    for (i = 0U; i < (Adp.MaxEntityNum * 2U); i++) {
        if (Entities[i].EntityInfo.AdpduPkt.AvtpduHeader.Entity_id == 0U) {
            if (AmbaWrap_memcpy(&Entities[i].EntityInfo.AdpduPkt, AdpPkt, sizeof(ADP_PACKET_s))!= 0U) { }
            Entities[i].EntityInfo.Port = (UINT8)(Idx & 0xFFU);
            (void)AmbaKAL_GetSysTickCount(&Entities[i].Timer);
            Rval = Avdecc_SendEvent(AVDECC_EVENT_ENTITY_AVAILABLE, Entities[i].EntityInfo.AdpduPkt.AvtpduHeader.Entity_id);
            break;
        }
    }
#if ADP_DBG
    Adp_ShowEntityInfo();
#endif
    return Rval;
}

/* 6.2.6.3.5 removeEntity(eui64) */
static UINT32 Adp_DiscoveryRemoveEntity(ADP_ENTITY_s *Entities, const UINT64 EntityId)
{
    UINT8 i;
    UINT32 Rval = OK;
    for (i = 0U; i < (Adp.MaxEntityNum * 2U); i++) {
        if (Entities[i].EntityInfo.AdpduPkt.AvtpduHeader.Entity_id == EntityId) {
            Rval = Avdecc_SendEvent(AVDECC_EVENT_ENTITY_DEPARTING, Entities[i].EntityInfo.AdpduPkt.AvtpduHeader.Entity_id);
            if (AmbaWrap_memset(&Entities[i], 0, sizeof(ADP_ENTITY_s))!= 0U) { }
            break;
        }
    }
    return Rval;
}

static UINT32 Adp_CheckEntitiesTimeout(ADP_ENTITY_s *Entities)
{
    UINT8 i;
    UINT32 Timeout = 0U;
    for (i = 0U; i < (Adp.MaxEntityNum * 2U); i++) {
        if (Entities[i].EntityInfo.AdpduPkt.AvtpduHeader.Entity_id != 0U) {
            UINT32 CurrectTime;
            (void) AmbaKAL_GetSysTickCount(&CurrectTime);
            if ((CurrectTime - Entities[i].Timer) > (UINT32)((UINT32)Entities[i].EntityInfo.AdpduPkt.AvtpduHeader.Valid_time * 2U * 1000U)) {
                Entities[i].Timeout = 1U;
                Timeout++;
            }
        }
    }
    return Timeout;
}

/* 6.2.6 Discovery State machine */
static void Adp_DiscoverySm(UINT8 EntityInfoId)
{
    ADP_DISCOVERY_SM_s *StateMachine = &Adp.DiscoverySm[EntityInfoId];
    if (Adp_CheckEntitiesTimeout(StateMachine->Entities) > 0U) {
        StateMachine->State = ADP_DISCOVERY_STATE_TIMEOUT;
    }
    switch (StateMachine->State) {
    case ADP_DISCOVERY_STATE_WAITING:
        if (StateMachine->DoTerminate == TRUE) {
            // TODO: task suspend
            StateMachine->DoTerminate = FALSE;
        } else if (StateMachine->DoDiscover == TRUE) {
            StateMachine->DoDiscover = FALSE;
            StateMachine->State = ADP_DISCOVERY_STATE_DISCOVER;
        } else if (StateMachine->RcvdEntityInfo.RcvdAvailable == TRUE) {
            StateMachine->RcvdEntityInfo.RcvdAvailable = FALSE;
            StateMachine->State = ADP_DISCOVERY_STATE_AVAILABLE;
        } else if (StateMachine->RcvdEntityInfo.RcvdDeparting == TRUE) {
            StateMachine->State = ADP_DISCOVERY_STATE_DEPARTING;
            StateMachine->RcvdEntityInfo.RcvdDeparting = FALSE;
        } else {
            /* TODO */
        }
        break;
    case ADP_DISCOVERY_STATE_DISCOVER:
        Adp_DiscoveryTxDiscover(EntityInfoId, (UINT64)0U);
        StateMachine->State = ADP_DISCOVERY_STATE_WAITING; /* UCT to ADP_DISCOVERY_STATE_WAITING */
        break;
    case ADP_DISCOVERY_STATE_AVAILABLE:
        if (Adp_DiscoveryHaveEntity(StateMachine->Entities, StateMachine->RcvdEntityInfo.AdpduPkt.AvtpduHeader.Entity_id) == 1U) {
            if (Adp_DiscoveryUpdateEntity(&StateMachine->RcvdEntityInfo.AdpduPkt, 0U, StateMachine->Entities) == OK) {
                StateMachine->RcvdEntityInfo.RcvdAvailable = FALSE;
            } else {
                AmbaPrint_PrintStr5("%s, Adp_DiscoveryUpdateEntity() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            if (Adp_DiscoveryAddEntity(&StateMachine->RcvdEntityInfo.AdpduPkt, 0U, StateMachine->Entities) == OK) {
                StateMachine->RcvdEntityInfo.RcvdAvailable = FALSE;
            } else {
                AmbaPrint_PrintStr5("%s, Adp_DiscoveryAddEntity() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        }
        StateMachine->State = ADP_DISCOVERY_STATE_WAITING; /* UCT to ADP_DISCOVERY_STATE_WAITING */
        break;
    case ADP_DISCOVERY_STATE_DEPARTING:
        if (Adp_DiscoveryHaveEntity(StateMachine->Entities, StateMachine->RcvdEntityInfo.AdpduPkt.AvtpduHeader.Entity_id) == 1U) {
            if (Adp_DiscoveryRemoveEntity(StateMachine->Entities,
                                          StateMachine->RcvdEntityInfo.AdpduPkt.AvtpduHeader.Entity_id) == OK) {
                StateMachine->RcvdEntityInfo.RcvdDeparting = FALSE;
            } else {
                AmbaPrint_PrintStr5("%s, Adp_DiscoveryRemoveEntity(DEPARTING) failed!", __func__, NULL, NULL, NULL, NULL);
            }
        }
        StateMachine->State = ADP_DISCOVERY_STATE_WAITING; /* UCT to ADP_DISCOVERY_STATE_WAITING */
        break;
    case ADP_DISCOVERY_STATE_TIMEOUT:
        {
            UINT8 i;
            for (i = 0U; i < (Adp.MaxEntityNum * 2U); i++) {
                if (StateMachine->Entities[i].Timeout != 0U) {
                    if (Adp_DiscoveryRemoveEntity(StateMachine->Entities,
                                                  StateMachine->Entities[i].EntityInfo.AdpduPkt.AvtpduHeader.Entity_id) != OK) {
                        AmbaPrint_PrintStr5("%s, Adp_DiscoveryRemoveEntity(TIMEOUT) failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            }
        }
        StateMachine->State = ADP_DISCOVERY_STATE_WAITING; /* UCT to ADP_DISCOVERY_STATE_WAITING */
        break;
    default:
        /* TODO */
        break;
    }
}

/* htonl, htons, ntohl, ntohs - convert values between host and network byte order */
static void Adp_Ntoh(ADP_PACKET_s *AdpPkt)
{
    UINT32 *Ptr;
    const void *Vptr = &(AdpPkt->AvtpduHeader);
    AmbaMisra_TypeCast(&Ptr, &Vptr);
    *Ptr = AVDECC_NTOHL(*Ptr);
    AdpPkt->AvtpduHeader.Entity_id = AVDECC_NTOHLL(AdpPkt->AvtpduHeader.Entity_id);
    AdpPkt->Adpdu.Entity_model_id = AVDECC_NTOHLL(AdpPkt->Adpdu.Entity_model_id); /* AVDECC Discovery Protocol Data Unit (ADPDU) Format */
    AdpPkt->Adpdu.Entity_capabilities = AVDECC_NTOHL(AdpPkt->Adpdu.Entity_capabilities);
    AdpPkt->Adpdu.Talker_stream_sources = AVDECC_NTOHS(AdpPkt->Adpdu.Talker_stream_sources);
    AdpPkt->Adpdu.Talker_capabilities = AVDECC_NTOHS(AdpPkt->Adpdu.Talker_capabilities);
    AdpPkt->Adpdu.Listener_stream_sinks = AVDECC_NTOHS(AdpPkt->Adpdu.Listener_stream_sinks);
    AdpPkt->Adpdu.Listener_capabilities = AVDECC_NTOHS(AdpPkt->Adpdu.Listener_capabilities);
    AdpPkt->Adpdu.Controller_capabilities = AVDECC_NTOHL(AdpPkt->Adpdu.Controller_capabilities);
    AdpPkt->Adpdu.Available_index = AVDECC_NTOHL(AdpPkt->Adpdu.Available_index);
    AdpPkt->Adpdu.Gptp_grandmaster_id = AVDECC_NTOHLL(AdpPkt->Adpdu.Gptp_grandmaster_id);
    AdpPkt->Adpdu.Identify_control_index = AVDECC_NTOHS(AdpPkt->Adpdu.Identify_control_index);
    AdpPkt->Adpdu.Interface_index = AVDECC_NTOHS(AdpPkt->Adpdu.Interface_index);
    AdpPkt->Adpdu.Association_id = AVDECC_NTOHLL(AdpPkt->Adpdu.Association_id);
#if ADP_DBG
    {
        UINT32 Tmp1, Tmp2;
        AmbaPrint_PrintUInt5("Control_data_len: %d", AdpPkt->AvtpduHeader.Control_data_len, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Valid_time: %d", AdpPkt->AvtpduHeader.Valid_time, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Message_type: %d", AdpPkt->AvtpduHeader.Message_type, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Version: %d", AdpPkt->AvtpduHeader.Version, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Sv: %d", AdpPkt->AvtpduHeader.Sv, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Subtype: %d", AdpPkt->AvtpduHeader.Subtype, 0U, 0U, 0U, 0U);
        Tmp1 = ((AdpPkt->AvtpduHeader.Entity_id & 0xFFFFFFFF00000000U) >> 32U);
        Tmp2 = ((AdpPkt->AvtpduHeader.Entity_id & 0x00000000FFFFFFFFU));
        AmbaPrint_PrintUInt5("Entity_id: %X%X", Tmp1, Tmp2, 0U, 0U, 0U);
        Tmp1 = ((AdpPkt->AvtpduHeader.Entity_model_id & 0xFFFFFFFF00000000U) >> 32U);
        Tmp2 = ((AdpPkt->AvtpduHeader.Entity_model_id & 0x00000000FFFFFFFFU));
        AmbaPrint_PrintUInt5("Entity_model_id: %X%X", Tmp1, Tmp2, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Entity_capabilities: %lX", AdpPkt->Adpdu.Entity_capabilities, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Talker_stream_sources: %X", AdpPkt->Adpdu.Talker_stream_sources, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Talker_capabilities: %X", AdpPkt->Adpdu.Talker_capabilities, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Listener_stream_sinks: %X", AdpPkt->Adpdu.Listener_stream_sinks, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Listener_capabilities: %X", AdpPkt->Adpdu.Listener_capabilities, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Controller_capabilities: %lX", AdpPkt->Adpdu.Controller_capabilities, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Available_index: %lX", AdpPkt->Adpdu.Available_index, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Gptp_grandmaster_id: %llX", AdpPkt->Adpdu.Gptp_grandmaster_id, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("gptp_domain_no: %X", AdpPkt->Adpdu.Gptp_domain_number, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Identify_control_index: %X", AdpPkt->Adpdu.Identify_control_index, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Interface_index: %X", AdpPkt->Adpdu.Interface_index, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Association_id: %llX", AdpPkt->Adpdu.Association_id, 0U, 0U, 0U, 0U);
    }
#endif
}

UINT32 Adp_Process(ADP_PACKET_s *AdpPkt)
{
    UINT32 Rval = OK;
    Adp_Ntoh(AdpPkt);
    switch (AdpPkt->AvtpduHeader.Message_type) { /* Table 6.1Message_type field */
    case ADP_MSG_ENTITY_AVAILABLE :
    case ADP_MSG_ENTITY_DEPARTING :
        {
            Rval = AmbaKAL_MutexTake(&Adp.Mutex, AMBA_KAL_WAIT_FOREVER);
            if (Rval == KAL_ERR_NONE) {
                UINT8 i;
                for (i = 0U; i < Adp.MaxEntityNum; i++) {
                    if ((Adp.DiscoverySm[i].EntityInfo.AvtpduHeader.Entity_id != 0U) &&
                        (Adp.DiscoverySm[i].EntityInfo.Adpdu.Controller_capabilities == ADP_CTRL_CAP_IMPLEMENTED)) { /* Only Controller */
                        ADP_PACKET_s *Pkt;
                        Pkt = Adp_AllocatePkt();
                        if (Pkt != NULL) {
                            if (AmbaWrap_memcpy(Pkt, AdpPkt, sizeof(ADP_PACKET_s))!= 0U) { }
                            Rval = AmbaKAL_MsgQueueSend(&Adp.MsgQueue[((i * (UINT8)ADP_SM_NUM) + (UINT8)ADP_SM_DISCOVERY)], &Pkt, AMBA_KAL_NO_WAIT); // Trigger Entities to handle events.
                            if (Rval != KAL_ERR_NONE) {
                                AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueSend() failed!", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                            AmbaPrint_PrintStr5("%s, Adp_AllocatePkt() failed!", __func__, NULL, NULL, NULL, NULL);
                            Rval = FALSE; // TODO:
                        }
                    }
                }
                if (AmbaKAL_MutexGive(&Adp.Mutex) != KAL_ERR_NONE) {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
                    Rval = ERR_ARG;
                }
            } else {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
            }
        }
        break;
    case ADP_MSG_ENTITY_DISCOVER :
        {
            Rval = AmbaKAL_MutexTake(&Adp.Mutex, AMBA_KAL_WAIT_FOREVER);
            if (Rval == KAL_ERR_NONE) {
                UINT8 i;
                for (i = 0U; i < Adp.MaxEntityNum; i++) {
                    if (Adp.AdvInterfaceSm[i].EntityInfo.AvtpduHeader.Entity_id != 0U) {
                        ADP_PACKET_s *Pkt;
                        Pkt = Adp_AllocatePkt();
                        if (Pkt != NULL) {
                            if (AmbaWrap_memcpy(Pkt, AdpPkt, sizeof(ADP_PACKET_s))!= 0U) { }
                            Rval = AmbaKAL_MsgQueueSend(&Adp.MsgQueue[((i * (UINT8)ADP_SM_NUM) + (UINT8)ADP_SM_ADVI)], &Pkt, AMBA_KAL_NO_WAIT); // Trigger Entities to handle events.
                            if (Rval != KAL_ERR_NONE) {
                                AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueSend() failed!", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                            AmbaPrint_PrintStr5("%s, Adp_AllocatePkt() failed!", __func__, NULL, NULL, NULL, NULL);
                            Rval = FALSE; // TODO:
                        }
                    }
                }
                if (AmbaKAL_MutexGive(&Adp.Mutex) != KAL_ERR_NONE) {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
                    Rval = ERR_ARG;
                }
            } else {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
            }
        }
        break;
    default:
        /* TODO */
        break;
    }
    return Rval;
}

static UINT32 Adp_TaskEntryAdv(UINT8 EntityInfoId)
{
    UINT32 Rval = OK;
    if (Adp.AdvEntitySm[EntityInfoId].EntityInfo.AvtpduHeader.Entity_id != 0U) {
        Adp_AdvEntitySm(EntityInfoId);
    }
    return Rval;
}

static UINT32 Adp_TaskEntryAdvI(UINT8 EntityInfoId, UINT32 *Busy)
{
    UINT32 Rval = OK;
    if (Adp.AdvInterfaceSm[EntityInfoId].EntityInfo.AvtpduHeader.Entity_id != 0U) {
        if (Adp.AdvInterfaceSm[EntityInfoId].State == ADP_ADVI_STATE_WAITING) {
            ULONG PktAddr = 0U;
            const ADP_PACKET_s *Pkt;
            Rval = AmbaKAL_MsgQueueReceive(&Adp.MsgQueue[((EntityInfoId * (UINT8)ADP_SM_NUM) + (UINT8)ADP_SM_ADVI)], &PktAddr, AMBA_KAL_NO_WAIT);
            if (Rval == KAL_ERR_NONE) {
                AmbaMisra_TypeCast(&Pkt, &PktAddr);
                Adp.AdvInterfaceSm[EntityInfoId].RcvdDiscover = TRUE;
                Adp.AdvInterfaceSm[EntityInfoId].EntityId = Pkt->AvtpduHeader.Entity_id;
                Adp_FreePkt(Pkt);
            } else {
                if (Rval == KAL_ERR_TIMEOUT) {
                    Rval = OK;
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueReceive() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
        Adp_AdvInterfaceSm(EntityInfoId);
        if (Adp.AdvInterfaceSm[EntityInfoId].State > ADP_ADVI_STATE_WAITING) {
            *Busy += 1U;
        }
    }
    return Rval;
}

static UINT32 Adp_TaskEntryDiscovery(UINT8 EntityInfoId, UINT32 *Busy)
{
    UINT32 Rval = OK;
    if (Adp.DiscoverySm[EntityInfoId].EntityInfo.AvtpduHeader.Entity_id != 0U) {
        if (Adp.DiscoverySm[EntityInfoId].State == ADP_DISCOVERY_STATE_WAITING) {
            ULONG PktAddr = 0U;
            const ADP_PACKET_s *Pkt;
            Rval = AmbaKAL_MsgQueueReceive(&Adp.MsgQueue[((EntityInfoId * (UINT8)ADP_SM_NUM) + (UINT8)ADP_SM_DISCOVERY)], &PktAddr, AMBA_KAL_NO_WAIT);
            if (Rval == KAL_ERR_NONE) {
                AmbaMisra_TypeCast(&Pkt, &PktAddr);
                if (AmbaWrap_memcpy(&Adp.DiscoverySm[EntityInfoId].RcvdEntityInfo.AdpduPkt, Pkt, sizeof(ADP_PACKET_s))!= 0U) { }
                if (Pkt->AvtpduHeader.Message_type == ADP_MSG_ENTITY_AVAILABLE) {
                    Adp.DiscoverySm[EntityInfoId].RcvdEntityInfo.RcvdAvailable = TRUE;
                }
                if (Pkt->AvtpduHeader.Message_type == ADP_MSG_ENTITY_DEPARTING) {
                    Adp.DiscoverySm[EntityInfoId].RcvdEntityInfo.RcvdDeparting = TRUE;
                }
                Adp_FreePkt(Pkt);
            } else {
                if (Rval == KAL_ERR_TIMEOUT) {
                    Rval = OK;
                } else {
                    AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueReceive() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
        Adp_DiscoverySm(EntityInfoId);
        if (Adp.DiscoverySm[EntityInfoId].State > ADP_DISCOVERY_STATE_WAITING) {
            *Busy += 1U;
        }
    }
    return Rval;
}

static void *Adp_TaskEntry(void *Info)
{
    UINT8 i;
    (void)Info;
    AmbaPrint_PrintStr5("%s, start", __func__, NULL, NULL, NULL, NULL);
    for (i = 0U; i < Adp.MaxEntityNum; i++) {
        Adp.AdvEntitySm[i].State = ADP_ADVE_STATE_INITIALIZE;
        Adp.AdvInterfaceSm[i].State = ADP_ADVI_STATE_INITIALIZE;
        //AvdeccAdp.AdpAdvInterfaceSm[i].advertisedGrandmasterID = currentGrandmasterID;
        Adp.AdvEntitySm[i].DoTerminate = Avdecc_DoTerminate();
        Adp.AdvInterfaceSm[i].DoTerminate = Avdecc_DoTerminate();
    }
    for (i = 0U; i < Adp.MaxEntityNum; i++) {
        Adp.DiscoverySm[i].State = ADP_DISCOVERY_STATE_WAITING;
        Adp.DiscoverySm[i].DoTerminate = Avdecc_DoTerminate();
    }
    for (;;) {
        UINT32 Busy = 0U;
        if (AmbaKAL_MutexTake(&Adp.Mutex, AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
            for (i = 0U; i < Adp.MaxEntityNum; i++) {
                if (Adp_TaskEntryAdv(i) == OK) { /* 6.2.4 Advertise Entity State Machine */
                    if (Adp_TaskEntryAdvI(i, &Busy) == OK) { /* 6.2.5 Advertise Interface State Machine */
                        if (Adp_TaskEntryDiscovery(i, &Busy) != OK) { /* 6.2.6 Discovery State machine */
                            AmbaPrint_PrintStr5("%s, Adp_TaskEntryDiscovery() fail!", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_PrintStr5("%s, Adp_TaskEntryAdvI() fail!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_PrintStr5("%s, Adp_TaskEntryAdv() fail!", __func__, NULL, NULL, NULL, NULL);
                }
            }
            if (AmbaKAL_MutexGive(&Adp.Mutex) != KAL_ERR_NONE) {
                AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() failed!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Busy == 0U) {
            (void) AmbaKAL_TaskSleep(10U);
        }
    }
}

static UINT32 Adp_SetEntityInfo(ADP_PACKET_s *AdpPkt, const AVDECC_ENTITY_INFO_s *EntityInfo)
{
    UINT32 Rval = OK;
    UINT32 ValidTime = 0U;
    (void) Avdecc_GetValidTime(&ValidTime);
    AdpPkt->AvtpduHeader.Valid_time = (UINT8)ValidTime;
    AdpPkt->AvtpduHeader.Entity_id = EntityInfo->EntityDesc.Entity_id;
    AdpPkt->Adpdu.Entity_model_id = EntityInfo->EntityDesc.Entity_model_id;
    AdpPkt->Adpdu.Entity_capabilities = EntityInfo->EntityDesc.Entity_capabilities;
    AdpPkt->Adpdu.Talker_stream_sources = EntityInfo->EntityDesc.Talker_stream_sources;
    AdpPkt->Adpdu.Talker_capabilities = EntityInfo->EntityDesc.Talker_capabilities;
    AdpPkt->Adpdu.Listener_stream_sinks = EntityInfo->EntityDesc.Listener_stream_sinks;
    AdpPkt->Adpdu.Listener_capabilities = EntityInfo->EntityDesc.Listener_capabilities;
    AdpPkt->Adpdu.Controller_capabilities = EntityInfo->EntityDesc.Controller_capabilities;
    AdpPkt->Adpdu.Available_index = EntityInfo->EntityDesc.Available_index;
    AdpPkt->Adpdu.Association_id = EntityInfo->EntityDesc.Association_id;
    return Rval;
}

UINT32 Adp_AddEntity(const AVDECC_ENTITY_INFO_s *EntityInfo)
{
    static char MsgQueueName[ADP_SM_NUM][32U] = {"AdpAdvMsgQueue", "AdpAdvIMsgQueue", "AdpDisMsgQueue"};
    UINT32 Rval = AmbaKAL_MutexTake(&Adp.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT32 i;
        for (i = 0U; i < Adp.MaxEntityNum; i++) {
            if (Adp.AdvEntitySm[i].EntityInfo.AvtpduHeader.Entity_id == 0U) {
                UINT32 j;
                AMBA_KAL_MSG_QUEUE_t *MsgQ;
                ULONG MsgQAddr;
                UINT8 *MsgQBuffer;
                ULONG MsgQBufferAddr;
                (void) Adp_SetEntityInfo(&Adp.AdvEntitySm[i].EntityInfo, EntityInfo);
                (void) Adp_SetEntityInfo(&Adp.AdvInterfaceSm[i].EntityInfo, EntityInfo);
                if (EntityInfo->EntityDesc.Controller_capabilities == ADP_CTRL_CAP_IMPLEMENTED) {
                    (void) Adp_SetEntityInfo(&Adp.DiscoverySm[i].EntityInfo, EntityInfo);
                }
                AmbaMisra_TypeCast(&MsgQ, &Adp.MsgQueuePtr);
                AmbaMisra_TypeCast(&MsgQAddr, &MsgQ);
                AmbaMisra_TypeCast(&MsgQBuffer, &Adp.MsgQBufferPtr);
                AmbaMisra_TypeCast(&MsgQBufferAddr, &MsgQBuffer);
                for (j = (i * (UINT32)ADP_SM_NUM); j < ((i * (UINT32)ADP_SM_NUM) + (UINT32)ADP_SM_NUM); j++) {
                    Rval = AmbaKAL_MsgQueueCreate(MsgQ, MsgQueueName[(j % (UINT8)ADP_SM_NUM)],
                                                 sizeof(ADP_PACKET_s *), MsgQBuffer,
                                                 (Adp.MsgNum * sizeof(ADP_PACKET_s *)));
                    if (Rval == KAL_ERR_NONE) {
                        MsgQAddr += sizeof(AMBA_KAL_MSG_QUEUE_t);
                        AmbaMisra_TypeCast(&MsgQ, &MsgQAddr);
                        MsgQBufferAddr += (Adp.MsgNum * sizeof(ADP_PACKET_s *));
                        AmbaMisra_TypeCast(&MsgQBuffer, &MsgQBufferAddr);
                    } else {
                        AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueCreate() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                AmbaMisra_TypeCast(&Adp.MsgQueuePtr, &MsgQAddr);
                AmbaMisra_TypeCast(&Adp.MsgQBufferPtr, &MsgQBufferAddr);
                break;
            }
        }
        if (AmbaKAL_MutexGive(&Adp.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

UINT32 Adp_RemoveEntity(const AVDECC_ENTITY_INFO_s *EntityInfo)
{
    UINT32 Rval = AmbaKAL_MutexTake(&Adp.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT32 i;
        for (i = 0U; i < Adp.MaxEntityNum; i++) {
            if (Adp.AdvEntitySm[i].EntityInfo.AvtpduHeader.Entity_id == EntityInfo->EntityDesc.Entity_id) {
                UINT32 j;
                if (AmbaWrap_memset(&Adp.AdvEntitySm[i].EntityInfo, 0, sizeof(ADP_PACKET_s))!= 0U) { }
                if (AmbaWrap_memset(&Adp.AdvInterfaceSm[i].EntityInfo, 0, sizeof(ADP_PACKET_s))!= 0U) { }
                if (EntityInfo->EntityDesc.Controller_capabilities == ADP_CTRL_CAP_IMPLEMENTED) {
                    if (AmbaWrap_memset(&Adp.DiscoverySm[i].EntityInfo, 0, sizeof(ADP_PACKET_s))!= 0U) { }
                }
                for (j = (i * (UINT32)ADP_SM_NUM); j < ((i * (UINT32)ADP_SM_NUM) + (UINT32)ADP_SM_NUM); j++) {
                    Rval = AmbaKAL_MsgQueueDelete(&Adp.MsgQueue[j]);
                }
                break;
            }
        }
        if (AmbaKAL_MutexGive(&Adp.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

UINT32 Adp_GetRequiredBufferSize(const AVDECC_ADP_INIT_CFG_s *Cfg, UINT32 *Size)
{
    UINT32 Rval = OK;
    UINT32 BufferSize = 0U;
    BufferSize += (GetAlignedValU32(Cfg->TaskInfo.StackSize, (UINT8)AMBA_CACHE_LINE_SIZE));    /* Task stack */
    BufferSize += (GetAlignedValU32((Cfg->MaxEntityNum * (UINT8)ADP_SM_NUM * sizeof(AMBA_KAL_MSG_QUEUE_t)), (UINT8)AMBA_CACHE_LINE_SIZE)); /* MsgQ */
    BufferSize += (GetAlignedValU32((Cfg->MaxEntityNum * (UINT8)ADP_SM_NUM * Cfg->MsgNum * sizeof(ADP_PACKET_s *)), (UINT8)AMBA_CACHE_LINE_SIZE)); /* MsgQ buffer */
    BufferSize += (GetAlignedValU32((Cfg->MsgNum * sizeof(ADP_PKT_BUFFER_s)), (UINT8)AMBA_CACHE_LINE_SIZE)); /* Pkt buffer */
    BufferSize += (GetAlignedValU32((Cfg->MaxEntityNum * sizeof(ADP_ADV_ENTITY_SM_s)), (UINT8)AMBA_CACHE_LINE_SIZE)); /* Adv StateMachine */
    BufferSize += (GetAlignedValU32((Cfg->MaxEntityNum * sizeof(ADP_ADV_INTERFACE_SM_s)), (UINT8)AMBA_CACHE_LINE_SIZE)); /* AdvI StateMachine */
    BufferSize += (GetAlignedValU32((Cfg->MaxEntityNum * sizeof(ADP_DISCOVERY_SM_s)), (UINT8)AMBA_CACHE_LINE_SIZE)); /* Discovery StateMachine */
    BufferSize += (GetAlignedValU32((Cfg->MaxEntityNum * (Cfg->MaxEntityNum * 2U * sizeof(ADP_ENTITY_s))), (UINT8)AMBA_CACHE_LINE_SIZE)); /* Entities of Discovery StateMachine */
    BufferSize = GetAlignedValU32(BufferSize, AMBA_CACHE_LINE_SIZE);
    *Size = BufferSize;
    return Rval;
}

UINT32 Adp_GetDefaultInitCfg(AVDECC_ADP_INIT_CFG_s *Cfg)
{
    UINT32 Rval;
    if (AmbaWrap_memset(Cfg, 0, sizeof(AVDECC_ADP_INIT_CFG_s))!= 0U) { }
    Cfg->MaxEntityNum = AVDECC_RX_ENTITY_INFO_MAX_NUM_DEFAULT;
    Cfg->MsgNum = ADP_MSGQ_MAX_NUM_DEFAULT;
    Cfg->TaskInfo.Priority = ADP_TASK_PRIORITY_DEFAULT;
    Cfg->TaskInfo.StackSize = ADP_TASK_STACK_SIZE_DEFAULT;
    Rval = Adp_GetRequiredBufferSize(Cfg, &Cfg->BufferSize);
    return Rval;
}

static UINT32 Adp_InitImpl(const AVDECC_ADP_INIT_CFG_s *Cfg)
{
    static char TaskName[] = {"AvdeccAdp"};
    UINT32 Rval;
    Rval = AmbaKAL_TaskCreate(&Adp.Task,
                                TaskName,
                                Cfg->TaskInfo.Priority,
                                Adp_TaskEntry,
                                NULL,
                                Adp.TaskStack,
                                Cfg->TaskInfo.StackSize,
                                AMBA_KAL_DONT_START);
    if (Rval == KAL_ERR_NONE) {
        Rval = AmbaKAL_TaskSetSmpAffinity(&Adp.Task, AVDECC_CORE);
        if (Rval == KAL_ERR_NONE) {
            Rval = AmbaKAL_TaskResume(&Adp.Task);
            if (Rval != KAL_ERR_NONE) {
                AmbaPrint_PrintStr5("%s, AmbaKAL_TaskResume() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_TaskSetSmpAffinity() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_TaskCreate() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

UINT32 Adp_Init(const AVDECC_ADP_INIT_CFG_s *Cfg)
{
    static char MutexName[] = {"AdpMutex"};
    UINT32 Rval;
    UINT32 BufferSize;
    Rval = Adp_GetRequiredBufferSize(Cfg, &BufferSize);
    if (Rval == OK) {
        if (BufferSize == Cfg->BufferSize) {
            ULONG Addr;
            AmbaMisra_TypeCast(&Addr, &Cfg->Buffer);
            if ((Addr % AMBA_CACHE_LINE_SIZE) == 0U) {
                UINT8 *Buffer = Cfg->Buffer;
                UINT32 i;
                Adp.MsgNum = Cfg->MsgNum;
                Adp.MaxEntityNum = Cfg->MaxEntityNum;
                Adp.TaskStack = Buffer; /* Task Stack */
                Addr += (GetAlignedValU32(Cfg->TaskInfo.StackSize, (UINT8)AMBA_CACHE_LINE_SIZE));
                AmbaMisra_TypeCast(&Adp.MsgQueue, &Addr); /* MsgQ */
                Adp.MsgQueuePtr = Adp.MsgQueue;
                Addr += (GetAlignedValU32((Adp.MaxEntityNum * (UINT8)ADP_SM_NUM * sizeof(AMBA_KAL_MSG_QUEUE_t)), (UINT8)AMBA_CACHE_LINE_SIZE));
                AmbaMisra_TypeCast(&Adp.MsgQBuffer, &Addr); /* MsgQ Buffer */
                Adp.MsgQBufferPtr = Adp.MsgQBuffer;
                Addr += (GetAlignedValU32((Adp.MaxEntityNum * (UINT8)ADP_SM_NUM * Adp.MsgNum * sizeof(ADP_PACKET_s *)), (UINT8)AMBA_CACHE_LINE_SIZE));
                AmbaMisra_TypeCast(&Adp.PktBuffer, &Addr); /* Pkt Buffer */
                Adp.PktBufferPtr = Adp.PktBuffer;
                Addr += (GetAlignedValU32((Adp.MsgNum * sizeof(ADP_PKT_BUFFER_s)), (UINT8)AMBA_CACHE_LINE_SIZE));
                AmbaMisra_TypeCast(&Adp.AdvEntitySm, &Addr); /* Adv StateMachine */
                Addr += (GetAlignedValU32((Adp.MaxEntityNum * sizeof(ADP_ADV_ENTITY_SM_s)), (UINT8)AMBA_CACHE_LINE_SIZE));
                AmbaMisra_TypeCast(&Adp.AdvInterfaceSm, &Addr); /* AdvI StateMachine */
                Addr += (GetAlignedValU32((Adp.MaxEntityNum * sizeof(ADP_ADV_INTERFACE_SM_s)), (UINT8)AMBA_CACHE_LINE_SIZE));
                AmbaMisra_TypeCast(&Adp.DiscoverySm, &Addr); /* Discovery StateMachine */
                Addr += (GetAlignedValU32((Adp.MaxEntityNum * sizeof(ADP_DISCOVERY_SM_s)), (UINT8)AMBA_CACHE_LINE_SIZE));
                Rval = AmbaKAL_MutexCreate(&Adp.Mutex, MutexName);
                for (i = 0U; i < Adp.MaxEntityNum; i++) { /* Entities of Discovery StateMachine */
                    AmbaMisra_TypeCast(&Adp.DiscoverySm[i].Entities, &Addr);
                    Addr += (Adp.MaxEntityNum * 2U * sizeof(ADP_ENTITY_s));
                }
                if (Rval == KAL_ERR_NONE) {
                    Rval = Adp_InitImpl(Cfg);
                    if (Rval == OK) {
                        Adp_PacketBufferInit();
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
    }
    return Rval;
}

