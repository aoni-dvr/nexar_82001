/**
 * @file avdecc.c
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

#define AVDECC_DBG  (0)     /**< Debug flag */

#define AVDECC_TASK_PRIORITY_DEFAULT    (200U)      /**< Default task priority */
#define AVDECC_TASK_STACK_SIZE_DEFAULT  (0x2000U)   /**< Default task stack size */

#define AVDECC_MAX_MSG_NUM_DEFAULT      (32U)       /**< Default max message number */

#define AVDECC_FRAME_SIZE (1600U)                   /**< Frame size of one avdecc packet */
#define AVDECC_PACKET_SIZE (AVDECC_FRAME_SIZE - ((sizeof(UINT32) * 6U) + (sizeof(UINT8) * 12U))) /**< L3 packet size. */

/**
 * Data type of Avdecc packet
 */
typedef struct {
    UINT32 Idx;         /**< Avdecc packet index */
    UINT32 Len;         /**< length of Avdecc packet */
    UINT32 Free;        /**< Indicator that the packet is free */
    UINT8 SrcMac[6U];   /**< Source MAC */
    UINT8 DstMac[6U];   /**< Destination MAC */
    UINT8 Pkt[AVDECC_PACKET_SIZE]; /**< Packer content */
} AVDECC_PACKET_s;

/**
 * Data type of Avdecc packet buffer
 */
typedef struct AVDECC_PACKET {
    AVDECC_PACKET_s AvdeccPkt;  /**< Avdecc packet */
    struct AVDECC_PACKET *Next; /**< Pointer to next Avdecc packet */
} AVDECC_PACKET_BUFFER_s;

/**
 * Data type of Avdecc information
 */
typedef struct {
    AMBA_KAL_MUTEX_t Mutex;                 /**< Mutex */
    UINT8 *Buffer;                          /**< Buffer */
    UINT32 BufferSize;                      /**< Buffer size */
    ULONG Addr;                            /**< Buffer address */
    UINT32 MsgNum;                          /**< Message number */
    AVDECC_PACKET_BUFFER_s *PktBuffer;      /**< Avdecc packet buffer */
    AVDECC_PACKET_BUFFER_s *PktPtr;         /**< Pointer to Avdecc packet buffer */
    AMBA_KAL_TASK_t Task;                   /**< Task */
    AMBA_KAL_MSG_QUEUE_t MsgQueue;          /**< Message queue */
    UINT32 MaxTalkerNum;                    /**< Max number of Talker */
    UINT32 MaxListenerNum;                  /**< Max number of Listener */
    UINT32 MaxControllerNum;                /**< Max number of Controller */
    UINT32 MaxTotalEntityNum;               /**< Max number of total entity */
    UINT32 TalkerNum;                       /**< Created Talker number */
    UINT32 ListenerNum;                     /**< Created Listener number */
    UINT32 ControllerNum;                   /**< Created Controller number */
    UINT32 Valid_time;                      /**< Valid_time in 2 sec, Time To Live, reannounceTimerTimeout 6.2.4.1.1(ADP) */
    UINT8 Mac[6U];                          /**< Mac */
    AMBA_AVB_AVTP_TALKER_s *AvtpTalker;     /**< Avtp configuration for Talker */
    AMBA_AVB_AVTP_LISTENER_s *AvtpListener; /**< Avtp configuration for Listener */
    AVDECC_ENTITY_INFO_s *EntityInfo;       /**< Entity information */
    UINT8 DoTerminate;                      /**< Boolean indicating that a request has been made to terminate. */
    UINT8 IsSupportVPN;                     /**< Boolean indicating VPN support or not. */
    UINT32 (*EventCb)(const UINT32 Event, UINT64 Info); /**< Event callback */
} AVDECC_INFO_s;

static AVDECC_INFO_s AvdeccMgr = {0}; /**< Avdecc manager */

#if AVDECC_DBG
/**
 *  Show entity information
 *  @param [in] EntityDesc The entity descript to show information
 *  @return void
 */
void Avdecc_ShowEntityInfo(AEM_ENTITY_DESCRIPTOR_s *EntityDesc)
{
    UINT32 Tmp1, Tmp2;
    AmbaPrint_PrintUInt5("------------------------------------------------", 0, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("Descriptor_type = %u ", EntityDesc->Descriptor_type, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("Descriptor_index = %u ", EntityDesc->Descriptor_index, 0, 0, 0, 0);
    Tmp1 = ((EntityDesc->Entity_id & 0xFFFFFFFF00000000U) >> 32U);
    Tmp2 = ((EntityDesc->Entity_id & 0x00000000FFFFFFFFU));
    AmbaPrint_PrintUInt5("Entity_id = %X%X", Tmp1, Tmp2, 0U, 0U, 0U);
    Tmp1 = ((EntityDesc->Entity_model_id & 0xFFFFFFFF00000000U) >> 32U);
    Tmp2 = ((EntityDesc->Entity_model_id & 0x00000000FFFFFFFFU));
    AmbaPrint_PrintUInt5("Entity_model_id = %X%X", Tmp1, Tmp2, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("Entity_capabilities = %u ", EntityDesc->Entity_capabilities, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("Talker_stream_sources = %u ", EntityDesc->Talker_stream_sources, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("Talker_capabilities = %u ", EntityDesc->Talker_capabilities, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("Listener_stream_sinks = %u ", EntityDesc->Listener_stream_sinks, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("Listener_capabilities = %u ", EntityDesc->Listener_capabilities, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("Controller_capabilities = %u ", EntityDesc->Controller_capabilities, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("Available_index = %u ", EntityDesc->Available_index, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("------------------------------------------------", 0, 0, 0, 0, 0);
}
#endif

/**
 *  Cast rx hash
 *  @param [in] EthIdx The index to cast the hash to the ethernet controller
 *  @return 0 - OK, others - Error
 */
static UINT32 Avdecc_AddMcastRxHash(const UINT32 EthIdx)
{
    /* Table B.1 - AVDECC Multicast MAC Addresses */
    static const UINT8 AvdeccAdpAcmpDstMac[6U] = {0x91U, 0xE0U, 0xF0U, 0x01U, 0x00U, 0x00U}; /* ADP and ACMP multicast destination MAC address */
    static const UINT8 AvdeccIdNotifyDstMac[6U] = {0x91U, 0xE0U, 0xF0U, 0x01U, 0x00U, 0x01U}; /* ID Notifications */
    UINT32 Rval = AmbaEnet_McastRxHash(EthIdx, AvdeccAdpAcmpDstMac);
    if (Rval == OK) {
        Rval = AmbaEnet_McastRxHash(EthIdx, AvdeccIdNotifyDstMac);
        if (Rval == OK) {
            Rval = AmbaEnet_McastRxHash(EthIdx, AvdeccMgr.Mac);
            if (Rval != OK) {
                AmbaPrint_PrintStr5("%s, AmbaEnet_McastRxHash(AvdeccMgr.Mac) failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaEnet_McastRxHash(AvdeccIdNotifyDstMac) failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaEnet_McastRxHash(AvdeccAdpAcmpDstMac) failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Avdecc packet buffer initial
 *  @param [in] Buffer The buffer use to initial
 *  @return 0 - OK, others - Error
 */
static UINT32 Avdecc_PacketBufferInit(const UINT8 *Buffer)
{
    UINT32 i;
    UINT32 Rval = OK;
    AmbaMisra_TypeCast(&AvdeccMgr.PktBuffer, &Buffer);
    AvdeccMgr.PktPtr = AvdeccMgr.PktBuffer;
    for (i = 0U; i < (AvdeccMgr.MsgNum - 1U); i++) {
        AvdeccMgr.PktPtr[i].Next = &(AvdeccMgr.PktPtr[i + 1U]);
    }
    AvdeccMgr.PktPtr[AvdeccMgr.MsgNum - 1U].Next = NULL;
    return Rval;
}

/**
 *  Allocate a Avdecc packet
 *  @return a pointer of Avdecc packet
 */
static AVDECC_PACKET_s *Avdecc_AllocatePkt(void)
{
    AVDECC_PACKET_s *Pkt = NULL;
    UINT32 Rval = AmbaKAL_MutexTake(&AvdeccMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        if (AvdeccMgr.PktPtr != NULL) {
            AVDECC_PACKET_BUFFER_s *Buffer = AvdeccMgr.PktPtr;
            AvdeccMgr.PktPtr = AvdeccMgr.PktPtr->Next;
            Buffer->AvdeccPkt.Free = FALSE;
            Pkt = &(Buffer->AvdeccPkt);
        }
        if (AmbaKAL_MutexGive(&AvdeccMgr.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Pkt;
}

/**
 *  Release a Avdecc packet
 *  @param [in] Pkt The pointer use to release Avdecc packet
 *  @return 0 - OK, others - Error
 */
static void Avdecc_FreePkt(AVDECC_PACKET_s *Pkt)
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvdeccMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        if (Pkt != NULL) {
            if (Pkt->Free == FALSE) {
                AVDECC_PACKET_BUFFER_s *Buffer;
                Pkt->Free = TRUE;
                AmbaMisra_TypeCast(&Buffer, &Pkt);
                Buffer->Next = AvdeccMgr.PktPtr;
                AvdeccMgr.PktPtr = Buffer;
            } else {
                AmbaPrint_PrintStr5("%s, Pkt already free!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, Pkt is NULL!", __func__, NULL, NULL, NULL, NULL);
        }
        if (AmbaKAL_MutexGive(&AvdeccMgr.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
}

/**
 *  Avdecc packet process
 *  @param [in] Idx     The index of Avdecc packet
 *  @param [in] DstMac  The destination MAC of Avdecc packet
 *  @param [in] SrcMac  The source MAC of Avdecc packet
 *  @param [in] L3Frame The frame of networking level 3
 *  @param [in] L3Len   The frame size of networking level 3
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_Process(const UINT32 Idx, const UINT8 DstMac[6U], const UINT8 SrcMac[6U],
                      const void *L3Frame, UINT16 L3Len)
{
    UINT32 Rval = OK;
    AVDECC_PACKET_s *Pkt;
    Pkt = Avdecc_AllocatePkt();
    if (Pkt != NULL) {
        Pkt->Idx = Idx;
        Pkt->Len = L3Len;
        if (AmbaWrap_memcpy(Pkt->DstMac, DstMac, 6U)!= 0U) { }
        if (AmbaWrap_memcpy(Pkt->SrcMac, SrcMac, 6U)!= 0U) { }
        if (AmbaWrap_memcpy(Pkt->Pkt, L3Frame, L3Len)!= 0U) { }
        Rval = AmbaKAL_MsgQueueSend(&AvdeccMgr.MsgQueue, &Pkt, AMBA_KAL_NO_WAIT); // Trigger Entities to handle events.
        if (Rval != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueSend() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("%s, Avdecc_AllocatePkt() failed!", __func__, NULL, NULL, NULL, NULL);
        Rval = FALSE; // TODO:
    }
    return Rval;
}

/**
 *  Avdecc task entry.
 *  @param [in] Info The argument attached to entry function
 *  @return 0 - OK, others - Error
 */
static void *Avdecc_TaskEntry(void *Info)
{
    UINT32 Rval;
    ULONG AvdeccPktAddr = 0U;
    AVDECC_PACKET_s *AvdeccPkt;
    (void)Info;
    for (;;) {
        Rval = AmbaKAL_MsgQueueReceive(&AvdeccMgr.MsgQueue, &AvdeccPktAddr, 10U);
        if (Rval == KAL_ERR_NONE) {
            UINT8 SubType;
            const UINT8 *Ptr;
            AmbaMisra_TypeCast(&AvdeccPkt, &AvdeccPktAddr);
            if (AmbaWrap_memcpy(&SubType, AvdeccPkt->Pkt, sizeof(UINT8))!= 0U) { } /* Note: for IEEE 1722.1 - 2013, here is 1-bit 'cd' field */
            Ptr = AvdeccPkt->Pkt;
            switch (SubType) {
            case AVTPDU_SUBTYPE_ADP:
                //AmbaPrint_PrintStr5("AVTPDU_SUBTYPE_ADP", NULL, NULL, NULL, NULL, NULL);
                {
                    ADP_PACKET_s *AdpPkt = NULL;
                    AmbaMisra_TypeCast(&AdpPkt, &Ptr);
                    Rval = Adp_Process(AdpPkt);
                    if (Rval != OK) {
                        AmbaPrint_PrintStr5("%s, Adp_Process() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                break;
            case AVTPDU_SUBTYPE_AECP:
                //AmbaPrint_PrintStr5("AVTPDU_SUBTYPE_AECP", NULL, NULL, NULL, NULL, NULL);
                {
                    AECP_PACKET_s *AecpPkt = NULL;
                    AmbaMisra_TypeCast(&AecpPkt, &Ptr);
                    Rval = Aecp_Process(AecpPkt);
                    if (Rval != OK) {
                        AmbaPrint_PrintStr5("%s, Aecp_Process() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                break;
            case AVTPDU_SUBTYPE_ACMP:
                //AmbaPrint_PrintStr5("AVTPDU_SUBTYPE_ACMP", NULL, NULL, NULL, NULL, NULL);
                {
                    ACMP_PACKET_s *AcmpPkt = NULL;
                    AmbaMisra_TypeCast(&AcmpPkt, &Ptr);
                    Rval = Acmp_Process(AcmpPkt);
                    if (Rval != OK) {
                        AmbaPrint_PrintStr5("%s, Acmp_Process() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                break;
            default:
                AmbaPrint_PrintUInt5("Unknow SubType = %u", SubType, 0, 0, 0, 0);
                break;
            }
            Avdecc_FreePkt(AvdeccPkt);
        }
    }
}

/**
 *  Get the required buffer size for initializing the Avdecc module.
 *  @param [in]  Cfg  The configuration of the Avdecc module
 *  @param [out] Size The required buffer size
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_GetRequiredBufferSize(const AVDECC_INIT_CFG_s *Cfg, UINT32 *Size)
{
    UINT32 Rval;
    UINT32 MaxProbEntityNum;
    UINT32 BufferSize = 0U;
    AVDECC_ACMP_INIT_CFG_s AcmpInitCfg = {0};
    AVDECC_TALKER_INIT_CFG_s TalkerInitCfg = {0};
    AVDECC_LISTENER_INIT_CFG_s ListenerInitCfg = {0};
    AVDECC_CONTROLLER_INIT_CFG_s ControllerInitCfg = {0};
    AVDECC_AECP_INIT_CFG_s AecpInitCfg = {0};
    AVDECC_ADP_INIT_CFG_s AdpInitCfg = {0};
    /* Avdecc */
    BufferSize += GetAlignedValU32((Cfg->MsgNum * sizeof(AVDECC_PACKET_BUFFER_s)), AMBA_CACHE_LINE_SIZE);  /* Avdecc Pkt buffer */
    BufferSize += GetAlignedValU32((Cfg->MsgNum * sizeof(AVDECC_PACKET_s *)), AMBA_CACHE_LINE_SIZE);    /* Avdecc MsgQ */
    BufferSize += GetAlignedValU32(Cfg->TaskInfo.StackSize, AMBA_CACHE_LINE_SIZE);   /* Avdecc Task */
    BufferSize += GetAlignedValU32(Cfg->MaxTalkerNum * sizeof(AMBA_AVB_AVTP_TALKER_s), AMBA_CACHE_LINE_SIZE); /* Avtp Talker */
    BufferSize += GetAlignedValU32(Cfg->MaxListenerNum * sizeof(AMBA_AVB_AVTP_LISTENER_s), AMBA_CACHE_LINE_SIZE); /* Avtp Listener */
    MaxProbEntityNum = (Cfg->MaxTalkerNum * 2U) + (Cfg->MaxListenerNum * 2U) + Cfg->MaxControllerNum;
    BufferSize += GetAlignedValU32(MaxProbEntityNum * sizeof(AVDECC_ENTITY_INFO_s), AMBA_CACHE_LINE_SIZE);   /* EntityInfo */
    /* Acmp */
    AcmpInitCfg.InflightCmdNum = ACMP_INFLIGHT_CMD_DEFAULT_NUM;
    Rval = Acmp_GetRequiredBufferSize(&AcmpInitCfg, &AcmpInitCfg.BufferSize);
    BufferSize += AcmpInitCfg.BufferSize;
    /* Acmp, Talker */
    if (Rval == OK) {
        TalkerInitCfg.MaxEntityNum = Cfg->MaxTalkerNum;
        TalkerInitCfg.MsgNum = ACMP_MSG_MAX_NUM_DEFAULT;
        TalkerInitCfg.MaxConnectNum = Cfg->AcmpMaxConnectNum;
        TalkerInitCfg.TaskInfo.StackSize = Cfg->TaskInfo.StackSize;
        Rval = Talker_GetRequiredBufferSize(&TalkerInitCfg, &TalkerInitCfg.BufferSize);
        BufferSize += TalkerInitCfg.BufferSize;
    }
    /* Acmp, Listener */
    if (Rval == OK) {
        ListenerInitCfg.MaxEntityNum = Cfg->MaxListenerNum;
        ListenerInitCfg.MsgNum = ACMP_MSG_MAX_NUM_DEFAULT;
        ListenerInitCfg.TaskInfo.StackSize = Cfg->TaskInfo.StackSize;
        Rval = Listener_GetRequiredBufferSize(&ListenerInitCfg, &ListenerInitCfg.BufferSize);
        BufferSize += ListenerInitCfg.BufferSize;
    }
    /* Acmp, Controller */
    if (Rval == OK) {
        ControllerInitCfg.MaxEntityNum = Cfg->MaxControllerNum;
        ControllerInitCfg.MsgNum = ACMP_MSG_MAX_NUM_DEFAULT;
        ControllerInitCfg.TaskInfo.StackSize = Cfg->TaskInfo.StackSize;
        Rval = Ctrller_GetRequiredBufferSize(&ControllerInitCfg, &ControllerInitCfg.BufferSize);
        BufferSize += ControllerInitCfg.BufferSize;
    }
    /* Aecp */
    if (Rval == OK) {
        AecpInitCfg.MaxEntityNum = Cfg->MaxTalkerNum + Cfg->MaxListenerNum + Cfg->MaxControllerNum;
        AecpInitCfg.InflightCmdNum = AECP_MAX_INFLIGHT_CMD_NUM_DEFAULT;
        AecpInitCfg.VuCmdNum = Cfg->AecpVuCmdNum;
        AecpInitCfg.TaskInfo.StackSize = Cfg->TaskInfo.StackSize;
        AecpInitCfg.MsgNum = Cfg->AecpMsgNum;
        Rval = Aecp_GetRequiredBufferSize(&AecpInitCfg, &AecpInitCfg.BufferSize);
        BufferSize += AecpInitCfg.BufferSize;
    }
    /* Adp */
    if (Rval == OK) {
        AdpInitCfg.MaxEntityNum = Cfg->MaxTalkerNum + Cfg->MaxListenerNum + Cfg->MaxControllerNum;
        AdpInitCfg.MsgNum = Cfg->AdpMsgNum;
        AdpInitCfg.TaskInfo.StackSize = Cfg->TaskInfo.StackSize;
        Rval = Adp_GetRequiredBufferSize(&AdpInitCfg, &AdpInitCfg.BufferSize);
        BufferSize += AdpInitCfg.BufferSize;
    }
    *Size = BufferSize;
    return Rval;
}

/**
 *  Get the default configuration for initializing the Avdecc module.
 *  @param [out] Cfg The returned configuration of the Avdecc module
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_GetDefaultInitCfg(AVDECC_INIT_CFG_s *Cfg)
{
    UINT32 Rval;
    AVDECC_ACMP_INIT_CFG_s AcmpInitCfg = {0};
    if (AmbaWrap_memset(Cfg, 0, sizeof(AVDECC_INIT_CFG_s))!= 0U) { }
    /* Avdecc */
    Cfg->MaxTalkerNum = AVDECC_ENTITY_TALKER_MAX_NUM_DEFAULT;
    Cfg->MaxListenerNum = AVDECC_ENTITY_LISTENER_MAX_NUM_DEFAULT;
    Cfg->MaxControllerNum = AVDECC_ENTITY_CONTROLLER_MAX_NUM_DEFAULT;
    Cfg->MsgNum = AVDECC_MAX_MSG_NUM_DEFAULT;
    Cfg->TaskInfo.Priority = AVDECC_TASK_PRIORITY_DEFAULT;
    Cfg->TaskInfo.StackSize = AVDECC_TASK_STACK_SIZE_DEFAULT;
    /* Acmp */
    Rval = Acmp_GetDefaultInitCfg(&AcmpInitCfg);
    if (Rval == OK) {
        AVDECC_TALKER_INIT_CFG_s TalkerInitCfg = {0};
        Rval = Talker_GetDefaultInitCfg(&TalkerInitCfg);
        if (Rval == OK) {
            Cfg->AcmpInflightCmdNum = AcmpInitCfg.InflightCmdNum;
            Cfg->AcmpMaxConnectNum = TalkerInitCfg.MaxConnectNum;
        }
    }
    /* Aecp */
    if (Rval == OK) {
        AVDECC_AECP_INIT_CFG_s AecpInitCfg = {0};
        Rval = Aecp_GetDefaultInitCfg(&AecpInitCfg);
        if (Rval == OK) {
            Cfg->AecpInflightCmdNum = AecpInitCfg.InflightCmdNum;
            Cfg->AecpVuCmdNum = AecpInitCfg.VuCmdNum;
            Cfg->AecpMsgNum = AecpInitCfg.MsgNum;
        }
    }
    /* Adp */
    if (Rval == OK) {
        AVDECC_ADP_INIT_CFG_s AdpInitCfg = {0};
        Rval = Adp_GetDefaultInitCfg(&AdpInitCfg);
        if (Rval == OK) {
            Cfg->AdpMsgNum = AdpInitCfg.MsgNum;
        }
    }
    Rval = Avdecc_GetRequiredBufferSize(Cfg, &Cfg->BufferSize);
    return Rval;
}

/**
 *  Implement initialize the Avdecc module.
 *  @param [in] Cfg The configuration used to initialize the Avdecc module
 *  @return 0 - OK, others - Error
 */
static UINT32 Avdecc_InitImpl(const AVDECC_INIT_CFG_s *Cfg)
{
    static char MsgQueueName[] = "AvdeccMsgQueue";
    static char TaskName[] = "AvdeccTask";
    UINT32 Rval;
    AvdeccMgr.MsgNum = Cfg->MsgNum;
    Rval = Avdecc_PacketBufferInit(AvdeccMgr.Buffer);
    if (Rval == OK) {
        AvdeccMgr.Addr += GetAlignedValU32((AvdeccMgr.MsgNum * sizeof(AVDECC_PACKET_BUFFER_s)), AMBA_CACHE_LINE_SIZE);
        AmbaMisra_TypeCast(&AvdeccMgr.Buffer, &AvdeccMgr.Addr);
        Rval = AmbaKAL_MsgQueueCreate(&AvdeccMgr.MsgQueue, MsgQueueName,
                                     sizeof(AVDECC_PACKET_s *), AvdeccMgr.Buffer,
                                     (Cfg->MsgNum * sizeof(AVDECC_PACKET_s *)));
        if (Rval == KAL_ERR_NONE) {
            AvdeccMgr.Addr += GetAlignedValU32((Cfg->MsgNum * sizeof(AVDECC_PACKET_s *)), AMBA_CACHE_LINE_SIZE);
            AmbaMisra_TypeCast(&AvdeccMgr.Buffer, &AvdeccMgr.Addr);
            Rval = AmbaKAL_TaskCreate(&AvdeccMgr.Task, TaskName, Cfg->TaskInfo.Priority,
                                     Avdecc_TaskEntry, NULL, AvdeccMgr.Buffer,
                                     Cfg->TaskInfo.StackSize,
                                     AMBA_KAL_DONT_START);
            if (Rval == KAL_ERR_NONE) {
                AvdeccMgr.Addr += GetAlignedValU32(Cfg->TaskInfo.StackSize, AMBA_CACHE_LINE_SIZE);
                AmbaMisra_TypeCast(&AvdeccMgr.Buffer, &AvdeccMgr.Addr);
                Rval = AmbaKAL_TaskSetSmpAffinity(&AvdeccMgr.Task, AVDECC_CORE);
                if (Rval == KAL_ERR_NONE) {
                    Rval = AmbaKAL_TaskResume(&AvdeccMgr.Task);
                    if (Rval != KAL_ERR_NONE) {
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
    }
    return Rval;
}

/**
 *  Initialize the Acmp of Avdecc module.
 *  @param [in] Cfg The configuration used to initialize the Acmp of Avdecc module
 *  @return 0 - OK, others - Error
 */
static UINT32 Avdecc_InitAcmp(const AVDECC_INIT_CFG_s *Cfg)
{
    UINT32 Rval;
    AVDECC_ACMP_INIT_CFG_s AcmpInitCfg = {0};
    AmbaMisra_TouchUnused(Cfg);
    AcmpInitCfg.InflightCmdNum = ACMP_INFLIGHT_CMD_DEFAULT_NUM;
    Rval = Acmp_GetRequiredBufferSize(&AcmpInitCfg, &AcmpInitCfg.BufferSize);
    if (Rval == OK) {
        AcmpInitCfg.Buffer = AvdeccMgr.Buffer;
        AvdeccMgr.Addr += AcmpInitCfg.BufferSize;
        AmbaMisra_TypeCast(&AvdeccMgr.Buffer, &AvdeccMgr.Addr);
        Rval = Acmp_Init(&AcmpInitCfg);
    }
    return Rval;
}

/**
 *  Initialize the Talker of Acmp module.
 *  @param [in] Cfg The configuration used to initialize the Talker of Acmp module
 *  @return 0 - OK, others - Error
 */
static UINT32 Avdecc_InitAcmpTalker(const AVDECC_INIT_CFG_s *Cfg)
{
    UINT32 Rval;
    AVDECC_TALKER_INIT_CFG_s TalkerInitCfg = {0};
    TalkerInitCfg.MaxEntityNum = AvdeccMgr.MaxTalkerNum;
    TalkerInitCfg.MsgNum = ACMP_MSG_MAX_NUM_DEFAULT;
    TalkerInitCfg.MaxConnectNum = ACMP_TALKER_MAX_STREAM_INFO_DEFAULT_NUM;
    TalkerInitCfg.TaskInfo.Priority = Cfg->TaskInfo.Priority + 1U;
    TalkerInitCfg.TaskInfo.StackSize = Cfg->TaskInfo.StackSize;
    Rval = Talker_GetRequiredBufferSize(&TalkerInitCfg, &TalkerInitCfg.BufferSize);
    if (Rval == OK) {
        TalkerInitCfg.Buffer = AvdeccMgr.Buffer;
        AvdeccMgr.Addr += TalkerInitCfg.BufferSize;
        AmbaMisra_TypeCast(&AvdeccMgr.Buffer, &AvdeccMgr.Addr);
        Rval = Talker_Init(&TalkerInitCfg);
    }
    return Rval;
}

/**
 *  Initialize the Listener of Acmp module.
 *  @param [in] Cfg The configuration used to initialize the Listener of Acmp module
 *  @return 0 - OK, others - Error
 */
static UINT32 Avdecc_InitAcmpListener(const AVDECC_INIT_CFG_s *Cfg)
{
    UINT32 Rval;
    AVDECC_LISTENER_INIT_CFG_s ListenerInitCfg = {0};
    ListenerInitCfg.MaxEntityNum = AvdeccMgr.MaxListenerNum;
    ListenerInitCfg.MsgNum = ACMP_MSG_MAX_NUM_DEFAULT;
    ListenerInitCfg.TaskInfo.Priority = Cfg->TaskInfo.Priority + 1U;
    ListenerInitCfg.TaskInfo.StackSize = Cfg->TaskInfo.StackSize;
    Rval = Listener_GetRequiredBufferSize(&ListenerInitCfg, &ListenerInitCfg.BufferSize);
    if (Rval == OK) {
        ListenerInitCfg.Buffer = AvdeccMgr.Buffer;
        AvdeccMgr.Addr += ListenerInitCfg.BufferSize;
        AmbaMisra_TypeCast(&AvdeccMgr.Buffer, &AvdeccMgr.Addr);
        Rval = Listener_Init(&ListenerInitCfg);
    }
    return Rval;
}

/**
 *  Initialize the Controller of Acmp module.
 *  @param [in] Cfg The configuration used to initialize the Controller of Acmp module
 *  @return 0 - OK, others - Error
 */
static UINT32 Avdecc_InitAcmpController(const AVDECC_INIT_CFG_s *Cfg)
{
    UINT32 Rval;
    AVDECC_CONTROLLER_INIT_CFG_s ControllerInitCfg = {0};
    ControllerInitCfg.MaxEntityNum = AvdeccMgr.MaxControllerNum;
    ControllerInitCfg.MsgNum = ACMP_MSG_MAX_NUM_DEFAULT;
    ControllerInitCfg.TaskInfo.Priority = Cfg->TaskInfo.Priority + 1U;
    ControllerInitCfg.TaskInfo.StackSize = Cfg->TaskInfo.StackSize;
    Rval = Ctrller_GetRequiredBufferSize(&ControllerInitCfg, &ControllerInitCfg.BufferSize);
    if (Rval == OK) {
        ControllerInitCfg.Buffer = AvdeccMgr.Buffer;
        AvdeccMgr.Addr += ControllerInitCfg.BufferSize;
        AmbaMisra_TypeCast(&AvdeccMgr.Buffer, &AvdeccMgr.Addr);
        Rval = Ctrller_Init(&ControllerInitCfg);
    }
    return Rval;
}

/**
 *  Initialize the Aecp of Avdecc module.
 *  @param [in] Cfg The configuration used to initialize the Aecp of Avdecc module
 *  @return 0 - OK, others - Error
 */
static UINT32 Avdecc_InitAecp(const AVDECC_INIT_CFG_s *Cfg)
{
    UINT32 Rval;
    AVDECC_AECP_INIT_CFG_s AecpInitCfg = {0};
    AecpInitCfg.MaxEntityNum = AvdeccMgr.MaxTotalEntityNum;
    AecpInitCfg.InflightCmdNum = AECP_MAX_INFLIGHT_CMD_NUM_DEFAULT;
    AecpInitCfg.VuCmdNum = Cfg->AecpVuCmdNum;
    AecpInitCfg.TaskInfo.Priority = Cfg->TaskInfo.Priority + 2U;
    AecpInitCfg.TaskInfo.StackSize = Cfg->TaskInfo.StackSize;
    AecpInitCfg.MsgNum = Cfg->AecpMsgNum;
    Rval = Aecp_GetRequiredBufferSize(&AecpInitCfg, &AecpInitCfg.BufferSize);
    if (Rval == OK) {
        AecpInitCfg.Buffer = AvdeccMgr.Buffer;
        AvdeccMgr.Addr += AecpInitCfg.BufferSize;
        AmbaMisra_TypeCast(&AvdeccMgr.Buffer, &AvdeccMgr.Addr);
        Rval = Aecp_Init(&AecpInitCfg);
    }
    return Rval;
}

/**
 *  Initialize the Adp of Avdecc module.
 *  @param [in] Cfg The configuration used to initialize the Adp of Avdecc module
 *  @return 0 - OK, others - Error
 */
static UINT32 Avdecc_InitAdp(const AVDECC_INIT_CFG_s *Cfg)
{
    UINT32 Rval;
    AVDECC_ADP_INIT_CFG_s AdpInitCfg = {0};
    AdpInitCfg.MaxEntityNum = AvdeccMgr.MaxTotalEntityNum;
    AdpInitCfg.MsgNum = Cfg->AdpMsgNum;
    AdpInitCfg.TaskInfo.Priority = Cfg->TaskInfo.Priority;
    AdpInitCfg.TaskInfo.StackSize = Cfg->TaskInfo.StackSize;
    Rval = Adp_GetRequiredBufferSize(&AdpInitCfg, &AdpInitCfg.BufferSize);
    if (Rval == OK) {
        AdpInitCfg.Buffer = AvdeccMgr.Buffer;
        AvdeccMgr.Addr += AdpInitCfg.BufferSize;
        AmbaMisra_TypeCast(&AvdeccMgr.Buffer, &AvdeccMgr.Addr);
        Rval = Adp_Init(&AdpInitCfg);
    }
    return Rval;
}

/**
 *  Initialize the AVDECC module.
 *  @param [in] Cfg The configuration used to initialize the AVDECC module
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_Init(const AVDECC_INIT_CFG_s *Cfg)
{
    static char MutexName[] = "AvdeccMutex";
    UINT32 Rval;
    UINT32 BufferSize;
    Rval = Avdecc_GetRequiredBufferSize(Cfg, &BufferSize);
    if (Rval == OK) {
        if (BufferSize == Cfg->BufferSize) {
            AmbaMisra_TypeCast(&AvdeccMgr.Addr, &Cfg->Buffer);
            if ((AvdeccMgr.Addr % AMBA_CACHE_LINE_SIZE) == 0U) {
                Rval = AmbaKAL_MutexCreate(&AvdeccMgr.Mutex, MutexName);
                if (Rval == KAL_ERR_NONE) { /* Avdecc */
                    AvdeccMgr.Buffer = Cfg->Buffer;
                    if (AmbaWrap_memset(AvdeccMgr.Buffer, 0, BufferSize)!= 0U) { }
                    AvdeccMgr.BufferSize = Cfg->BufferSize;
                    AvdeccMgr.MaxTalkerNum = Cfg->MaxTalkerNum;
                    AvdeccMgr.MaxListenerNum = Cfg->MaxListenerNum;
                    AvdeccMgr.MaxControllerNum = Cfg->MaxControllerNum;
                    AvdeccMgr.MaxTotalEntityNum = AvdeccMgr.MaxTalkerNum +
                                                  AvdeccMgr.MaxListenerNum +
                                                  AvdeccMgr.MaxControllerNum;
                    Rval = Avdecc_InitImpl(Cfg);
                    if (Rval == OK) {
                        UINT32 MaxProbEntityNum;
                        if (AvdeccMgr.MaxTalkerNum > 0U) {
                            AmbaMisra_TypeCast(&AvdeccMgr.AvtpTalker, &AvdeccMgr.Buffer);
                            AvdeccMgr.Addr += GetAlignedValU32(AvdeccMgr.MaxTalkerNum * sizeof(AMBA_AVB_AVTP_TALKER_s), AMBA_CACHE_LINE_SIZE);   /* EntityInfo */
                            AmbaMisra_TypeCast(&AvdeccMgr.Buffer, &AvdeccMgr.Addr);
                        }
                        if (AvdeccMgr.MaxListenerNum > 0U) {
                            AmbaMisra_TypeCast(&AvdeccMgr.AvtpListener, &AvdeccMgr.Buffer);
                            AvdeccMgr.Addr += GetAlignedValU32(AvdeccMgr.MaxListenerNum * sizeof(AMBA_AVB_AVTP_LISTENER_s), AMBA_CACHE_LINE_SIZE);   /* EntityInfo */
                            AmbaMisra_TypeCast(&AvdeccMgr.Buffer, &AvdeccMgr.Addr);
                        }
                        AmbaMisra_TypeCast(&AvdeccMgr.EntityInfo, &AvdeccMgr.Buffer);
                        MaxProbEntityNum = (AvdeccMgr.MaxTalkerNum * 2U) + (AvdeccMgr.MaxListenerNum * 2U) + AvdeccMgr.MaxControllerNum;
                        AvdeccMgr.Addr += GetAlignedValU32(MaxProbEntityNum * sizeof(AVDECC_ENTITY_INFO_s), AMBA_CACHE_LINE_SIZE);   /* EntityInfo */
                        AmbaMisra_TypeCast(&AvdeccMgr.Buffer, &AvdeccMgr.Addr);
                        AvdeccMgr.Valid_time = Cfg->Valid_time;
                        if (AmbaWrap_memcpy(AvdeccMgr.Mac, Cfg->Mac, 6U)!= 0U) { }
                        AvdeccMgr.DoTerminate = Cfg->DoTerminate;
                        AvdeccMgr.IsSupportVPN = Cfg->IsSupportVPN;
                        AvdeccMgr.EventCb = Cfg->EventCb;
                        Rval = Avdecc_AddMcastRxHash(0U); /* MultiCast */
                    } else {
                        AmbaPrint_PrintStr5("%s, Avdecc_InitImpl() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                    if (Rval == OK) {
                        Rval = Avdecc_InitAcmp(Cfg); /* ACMP (Chapter 8.2 of IEEE Std 1722.1-2013) */
                        if (Rval == OK) {
                            if (AvdeccMgr.MaxTalkerNum > 0U) { /* ACMP, Talker */
                                Rval = Avdecc_InitAcmpTalker(Cfg);
                            }
                            if (AvdeccMgr.MaxListenerNum > 0U) { /* ACMP, Listener */
                                Rval = Avdecc_InitAcmpListener(Cfg);
                            }
                            if (AvdeccMgr.MaxControllerNum > 0U) { /* ACMP, Controller */
                                Rval = Avdecc_InitAcmpController(Cfg);
                            }
                        } else {
                            AmbaPrint_PrintStr5("%s, Avdecc_InitAcmp() failed!", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                    if (Rval == OK) { /* AECP (Chapter 9.2 of IEEE Std 1722.1-2013)*/
                        Rval = Avdecc_InitAecp(Cfg);
                        if (Rval != OK) {
                            AmbaPrint_PrintStr5("%s, Avdecc_InitAecp() failed!", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                    if (Rval == OK) { /* ADP, start it at last step (Chapter 6.2 of IEEE Std 1722.1-2013) */
                        Rval = Avdecc_InitAdp(Cfg);
                        if (Rval != OK) {
                            AmbaPrint_PrintStr5("%s, Avdecc_InitAdp() failed!", __func__, NULL, NULL, NULL, NULL);
                        }
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

/**
 *  Add entity
 *  @param [in] Role The role which want to add.
 *  @param [in] Cfg  The configuration for add the entity
 *  @return 0 - OK, others - Error
 */
static UINT32 Avdecc_AddEntity(UINT8 Role, const AVDECC_CFG_s *Cfg)
{
    UINT8 i;
    UINT8 Exist = 0U;
    UINT32 Rval = OK;
    for (i = 0U; i < AvdeccMgr.MaxTotalEntityNum; i++) {
        if ((AvdeccMgr.EntityInfo[i].EntityDesc.Entity_id != 0U) &&
            (AvdeccMgr.EntityInfo[i].EntityDesc.Entity_id == Cfg->EntityDescriptor.Entity_id)) {
            Exist = 1U;
            AmbaPrint_PrintStr5("%s, The entity already exist!", __func__, NULL, NULL, NULL, NULL);
            break;
        }
    }
    if (Exist == 0U) {
        for (i = 0U; i < AvdeccMgr.MaxTotalEntityNum; i++) {
            if (AvdeccMgr.EntityInfo[i].EntityDesc.Entity_id == 0U) {
                if (AmbaWrap_memcpy(&AvdeccMgr.EntityInfo[i].EntityDesc, &Cfg->EntityDescriptor, sizeof(AEM_ENTITY_DESCRIPTOR_s))!= 0U) { }
                if (Role == AVDECC_CONTROLLER) { /* AEM, ACMP */
                    AvdeccMgr.EntityInfo[i].Acmpdu.Controller_entity_id = AvdeccMgr.EntityInfo[i].EntityDesc.Entity_id;
                    Rval = Ctrller_AddEntity(&AvdeccMgr.EntityInfo[i]);
                    if (Rval != OK) {
                        AmbaPrint_PrintStr5("%s, Ctrller_AddEntity() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else if (Role == AVDECC_TALKER) {
                    AvdeccMgr.EntityInfo[i].Acmpdu.Talker_entity_id = AvdeccMgr.EntityInfo[i].EntityDesc.Entity_id;
                    AvdeccMgr.EntityInfo[i].Acmpdu.Talker_unique_id = Cfg->UniqueId;
                    AvdeccMgr.EntityInfo[i].VuCmdCb = Cfg->VuCmdCb;
                    Rval = Talker_AddEntity(&AvdeccMgr.EntityInfo[i]);
                    if (Rval != OK) {
                        AmbaPrint_PrintStr5("%s, Talker_AddEntity() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else if (Role == AVDECC_LISTENER) {
                    AvdeccMgr.EntityInfo[i].Acmpdu.Listener_entity_id = AvdeccMgr.EntityInfo[i].EntityDesc.Entity_id;
                    AvdeccMgr.EntityInfo[i].Acmpdu.Listener_unique_id = Cfg->UniqueId;
                    AvdeccMgr.EntityInfo[i].VuCmdCb = Cfg->VuCmdCb;
                    Rval = Listener_AddEntity(&AvdeccMgr.EntityInfo[i]);
                    if (Rval != OK) {
                        AmbaPrint_PrintStr5("%s, Listener_AddEntity() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    //nothing
                }
                if (Rval == OK) {
                    Rval = Adp_AddEntity(&AvdeccMgr.EntityInfo[i]); /* ADP */
                    if (Rval == OK) {
                        Rval = Aecp_AddEntity(&AvdeccMgr.EntityInfo[i]); /* AECP */
                        if (Rval != OK) {
                            AmbaPrint_PrintStr5("%s, Aecp_AddEntity() failed!", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_PrintStr5("%s, Adp_AddEntity() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                break;
            }
        }
    } else {
        Rval = ERR_ARG;
    }
#if AVDECC_DBG
    for (i = 0U; i < AvdeccMgr.MaxTotalEntityNum; i++) {
        Avdecc_ShowEntityInfo(&AvdeccMgr.EntityInfo[i].EntityDesc);
    }
#endif
    return Rval;
}

/**
 *  Remove entity
 *  @param [in] Role     The role which want to remove.
 *  @param [in] EntityId The configuration for remove the entity
 *  @return 0 - OK, others - Error
 */
static UINT32 Avdecc_RemoveEntity(UINT8 Role, UINT64 EntityId)
{
    UINT8 i;
    UINT32 Rval = OK;
    for (i = 0U; i < AvdeccMgr.MaxTotalEntityNum; i++) {
        if ((AvdeccMgr.EntityInfo[i].EntityDesc.Entity_id != 0U) &&
            (AvdeccMgr.EntityInfo[i].EntityDesc.Entity_id == EntityId)) {
            if (Role == AVDECC_CONTROLLER) { /* AEM, ACMP */
                Rval = Ctrller_RemoveEntity(&AvdeccMgr.EntityInfo[i]);
                if (Rval != OK) {
                    AmbaPrint_PrintStr5("%s, Ctrller_AddEntity() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else if (Role == AVDECC_TALKER) {
                Rval = Talker_RemoveEntity(&AvdeccMgr.EntityInfo[i]);
                if (Rval != OK) {
                    AmbaPrint_PrintStr5("%s, Talker_AddEntity() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else if (Role == AVDECC_LISTENER) {
                Rval = Listener_RemoveEntity(&AvdeccMgr.EntityInfo[i]);
                if (Rval != OK) {
                    AmbaPrint_PrintStr5("%s, Listener_AddEntity() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                //nothing
            }
            if (Rval == OK) {
                Rval = Adp_RemoveEntity(&AvdeccMgr.EntityInfo[i]); /* ADP */
                if (Rval == OK) {
                    Rval = Aecp_RemoveEntity(&AvdeccMgr.EntityInfo[i]); /* AECP */
                    if (Rval == OK) {
                        if (AmbaWrap_memset(&AvdeccMgr.EntityInfo[i], 0, sizeof(AVDECC_ENTITY_INFO_s))!= 0U) { }
                    } else {
                        AmbaPrint_PrintStr5("%s, Aecp_AddEntity() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_PrintStr5("%s, Adp_AddEntity() failed!", __func__, NULL, NULL, NULL, NULL);
                }
            }
            break;
        }
    }
    return Rval;
}

/**
 *  Create the Talker of AVDECC module.
 *  @param [in] Cfg The configuration used to create the Talker of AVDECC module
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_CreateTalker(const AVDECC_CFG_s *Cfg)
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvdeccMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        if (AvdeccMgr.TalkerNum < AvdeccMgr.MaxTalkerNum) {
            UINT8 i;
            for(i = 0U; i < AvdeccMgr.MaxTalkerNum; i++) {
                if (AvdeccMgr.AvtpTalker[i].pAvbConfig == NULL) {
                    if (AmbaWrap_memcpy(AvdeccMgr.AvtpTalker[(i % AvdeccMgr.MaxTalkerNum)].StreamID.Mac, AvdeccMgr.Mac, 6U)!= 0U) { }
                    AvdeccMgr.AvtpTalker[(i % AvdeccMgr.MaxTalkerNum)].StreamID.UniqueID = Cfg->UniqueId;
                    Rval = AmbaAvbStack_GetConfig(0U, &AvdeccMgr.AvtpTalker[(i % AvdeccMgr.MaxTalkerNum)].pAvbConfig);
                    if (Rval == NET_ERR_NONE) {
                        Rval = Avdecc_AddEntity(AVDECC_TALKER, Cfg);
                        if (Rval == OK) {
                            AvdeccMgr.TalkerNum++;
                            AmbaPrint_PrintStr5("%s, success", __func__, NULL, NULL, NULL, NULL);
                        } else {
                            AmbaPrint_PrintStr5("%s, failed!", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                    break;
                }
            }
        } else {
            AmbaPrint_PrintStr5("%s, TalkerNum > MaxTalkerNum", __func__, NULL, NULL, NULL, NULL);
        }
        if (AmbaKAL_MutexGive(&AvdeccMgr.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Delete the Talker of AVDECC module.
 *  @param [in] EntityId The EntityId used to delete the Talker of AVDECC module
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_DeleteTalker(UINT64 EntityId)
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvdeccMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        UINT32 Searched = 0U;
        for(i = 0U; i < AvdeccMgr.MaxTalkerNum; i++) {
            if (Avdecc_GenEntityId(AvdeccMgr.Mac, AvdeccMgr.AvtpTalker[i].StreamID.UniqueID) == EntityId) {
               Searched = 1U;
               break;
            }
        }
        if (Searched == 1U) {
            Rval = Avdecc_RemoveEntity(AVDECC_TALKER, EntityId);
            if (Rval == OK) {
                if (AmbaWrap_memset(&AvdeccMgr.AvtpTalker[(i % AvdeccMgr.MaxTalkerNum)], 0, sizeof(AMBA_AVB_AVTP_TALKER_s))!= 0U) { }
                AvdeccMgr.TalkerNum--;
                AmbaPrint_PrintStr5("%s, success", __func__, NULL, NULL, NULL, NULL);
            } else {
                AmbaPrint_PrintStr5("%s, failed!", __func__, NULL, NULL, NULL, NULL);
            }
        }
        if (AmbaKAL_MutexGive(&AvdeccMgr.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Create the Listener of AVDECC module.
 *  @param [in] Cfg The configuration used to create the Listener of AVDECC module
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_CreateListener(const AVDECC_CFG_s *Cfg)
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvdeccMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        if (AvdeccMgr.ListenerNum < AvdeccMgr.MaxListenerNum) {
            UINT8 i;
            for(i = 0U; i < AvdeccMgr.MaxListenerNum; i++) {
                if (AvdeccMgr.AvtpListener[i].pAvbConfig == NULL) {
                    if (AmbaWrap_memcpy(AvdeccMgr.AvtpListener[(i % AvdeccMgr.MaxListenerNum)].StreamID.Mac, AvdeccMgr.Mac, 6U)!= 0U) { }
                    AvdeccMgr.AvtpListener[(i % AvdeccMgr.MaxListenerNum)].StreamID.UniqueID = Cfg->UniqueId;
                    Rval = AmbaAvbStack_GetConfig(0U, &AvdeccMgr.AvtpListener[(i % AvdeccMgr.MaxListenerNum)].pAvbConfig);
                    if (Rval == NET_ERR_NONE) {
                        AvdeccMgr.AvtpListener[(i % AvdeccMgr.MaxListenerNum)].pBuf = Cfg->FrameBuffer;
                        AvdeccMgr.AvtpListener[(i % AvdeccMgr.MaxListenerNum)].BufSize = Cfg->FrameBufferSize;
                        AmbaMisra_TypeCast(&AvdeccMgr.AvtpListener[(i % AvdeccMgr.MaxListenerNum)].pFrameQueue, &Cfg->FrameInfo);
                        AvdeccMgr.AvtpListener[(i % AvdeccMgr.MaxListenerNum)].FrameQueueNum = Cfg->FrameInfoNum;
                        Rval = Avdecc_AddEntity(AVDECC_LISTENER, Cfg);
                        if (Rval == OK) {
                            AvdeccMgr.ListenerNum++;
                            AmbaPrint_PrintStr5("%s, success", __func__, NULL, NULL, NULL, NULL);
                        } else {
                            AmbaPrint_PrintStr5("%s, failed!", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                    break;
                }
            }
        } else {
            AmbaPrint_PrintStr5("%s, ListenerNum > MaxListenerNum", __func__, NULL, NULL, NULL, NULL);
        }
        if (AmbaKAL_MutexGive(&AvdeccMgr.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Delete the Listener of AVDECC module.
 *  @param [in] EntityId The EntityId used to delete the Listener of AVDECC module
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_DeleteListener(UINT64 EntityId)
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvdeccMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        UINT32 Searched = 0U;
        for(i = 0U; i < AvdeccMgr.MaxListenerNum; i++) {
            if (Avdecc_GenEntityId(AvdeccMgr.Mac, AvdeccMgr.AvtpListener[i].StreamID.UniqueID) == EntityId) {
               Searched = 1U;
               break;
            }
        }
        if (Searched == 1U) {
            Rval = Avdecc_RemoveEntity(AVDECC_LISTENER, EntityId);
            if (Rval == OK) {
                if (AmbaWrap_memset(&AvdeccMgr.AvtpListener[(i % AvdeccMgr.MaxListenerNum)], 0, sizeof(AMBA_AVB_AVTP_LISTENER_s))!= 0U) { }
                AvdeccMgr.ListenerNum--;
                AmbaPrint_PrintStr5("%s, success", __func__, NULL, NULL, NULL, NULL);
            } else {
                AmbaPrint_PrintStr5("%s, failed!", __func__, NULL, NULL, NULL, NULL);
            }
        }
        if (AmbaKAL_MutexGive(&AvdeccMgr.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Create the Controller of AVDECC module.
 *  @param [in] Cfg The configuration used to create the Controller of AVDECC module
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_CreateController(const AVDECC_CFG_s *Cfg)
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvdeccMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        if (AvdeccMgr.ControllerNum < AvdeccMgr.MaxControllerNum) {
            Rval = Avdecc_AddEntity(AVDECC_CONTROLLER, Cfg);
            if (Rval == OK) {
                AvdeccMgr.ControllerNum++;
                AmbaPrint_PrintStr5("%s, success", __func__, NULL, NULL, NULL, NULL);
            } else {
                AmbaPrint_PrintStr5("%s, failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, ControllerNum > MaxControllerNum", __func__, NULL, NULL, NULL, NULL);
        }
        if (AmbaKAL_MutexGive(&AvdeccMgr.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Send Adp command
 *  (6.2 AVDECC Discovery Protocol (ADP)(controller only))
 *  @param [in] Cmd The Adp command
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_SendAdpCmd(UINT32 Cmd)
{
    UINT32 Rval = OK;
    AmbaMisra_TouchUnused(&Cmd);
    return Rval;
}

/**
 *  Send Acmp command
 *  (8.2 AVDECC Connection Management Protocol (ACMP))
 *  @param [in] Cmd    The Acmp command
 *  @param [in] DstMac The Destination MAC
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_SendAcmpCmd(const ACMP_COMMAND_PARAMS_s *Cmd, const UINT8 DstMac[6U])
{
    UINT32 Rval;
    ACMP_PACKET_s AcmpPkt = {0};
    AcmpPkt.AvtpduHeader.Message_type = Cmd->Message_type;
    AcmpPkt.Acmpdu.Talker_entity_id = Cmd->Talker_entity_id;
    AcmpPkt.Acmpdu.Listener_entity_id = Cmd->Listener_entity_id;
    AcmpPkt.Acmpdu.Talker_unique_id = Cmd->Talker_unique_id;
    AcmpPkt.Acmpdu.Listener_unique_id = Cmd->Listener_unique_id;
    if (AmbaWrap_memcpy(AcmpPkt.Acmpdu.Stream_dest_mac, DstMac, 6U)!= 0U) { }
    AcmpPkt.Acmpdu.Connection_count = Cmd->Connection_count;
    AcmpPkt.Acmpdu.Flags = Cmd->Flags;
    AcmpPkt.Acmpdu.Stream_vlan_id = Cmd->Stream_vlan_id;
    Rval = Ctrller_SendAcmpPacket(&AcmpPkt);
    return Rval;
}

/**
 *  Send Aecp command
 *  (9.2 AVDECC Enumeration and Control Protocol (AECP))
 *  @param [in] Cmd The Aecp command
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_SendAecpCmd(const AEM_CMD_RESP_s *Cmd)
{
    return Aecp_SendAecpCmd(Cmd);
}

/**
 *  Aecp command process done
 *  @param [in] EntityId The entity id use to notify Aecp command done
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_ProcessAecpCmdDone(UINT64 EntityId)
{
    return Aecp_ProcessCmdDone(EntityId);
}

/**
 *  Send Aecp vendor unique command
 *  @param [in] Cmd The Aecp vu command
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_SendAecpVuCmd(const AECP_VU_AECPDU_s *Cmd)
{
    return Aecp_SendAecpVuCmd(Cmd);
}

/**
 *  Get entity list
 *  @param [out] EntityInfo The list of entity
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_GetEntityList(AVDECC_ENTITY_INFO_s **EntityInfo) /* Get EntityList(controller only) */
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvdeccMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        *EntityInfo = AvdeccMgr.EntityInfo;
        if (AmbaKAL_MutexGive(&AvdeccMgr.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Get valid time
 *  @param [out] Valid_time The valid time of Avdecc
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_GetValidTime(UINT32 *Valid_time)
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvdeccMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        *Valid_time = AvdeccMgr.Valid_time;
        if (AmbaKAL_MutexGive(&AvdeccMgr.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Get MAC
 *  @param [out] Mac The MAC of Avdecc
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_GetMac(const UINT8 *Mac)
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvdeccMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        if (AmbaWrap_memcpy(&Mac, AvdeccMgr.Mac, 6U)!= 0U) { }
        if (AmbaKAL_MutexGive(&AvdeccMgr.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Get entity number
 *  @param [in] Role The role use to get entity number
 *  @param [out] Num The entity number
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_GetEntityNum(UINT32 Role, UINT32 *Num)
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvdeccMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        if (Role == AVDECC_CONTROLLER) {
            *Num = AvdeccMgr.ControllerNum;
        } else if (Role == AVDECC_TALKER) {
            *Num = AvdeccMgr.TalkerNum;
        } else if (Role == AVDECC_LISTENER) {
            *Num = AvdeccMgr.ListenerNum;
        } else {
            *Num = 0U;
        }
        if (AmbaKAL_MutexGive(&AvdeccMgr.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Loop back Tx
 *  @param [in] Data The data to transmit
 *  @param [in] Size The size of data
 *  @return 0 - OK, others - Error
 */
static UINT32 Avdecc_LoopbackTx(const void *Data, UINT32 Size)
{
    UINT32 Rval = OK;
    AVDECC_PACKET_s *Pkt;
    Pkt = Avdecc_AllocatePkt();
    if (Pkt != NULL) {
        Pkt->Idx = 0U;
        Pkt->Len = Size;
        if (AmbaWrap_memcpy(Pkt->Pkt, Data, Size)!= 0U) { }
        Rval = AmbaKAL_MsgQueueSend(&AvdeccMgr.MsgQueue, &Pkt, AMBA_KAL_NO_WAIT); // Trigger Entities to handle events.
        if (Rval != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MsgQueueSend() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("%s, Avdecc_AllocatePkt() failed!", __func__, NULL, NULL, NULL, NULL);
        Rval = FALSE; // TODO:
    }
    return Rval;
}

/**
 *  Avdecc net transmit
 *  @param [in] Idx    The index of ethernet
 *  @param [in] Data   The data to transmit
 *  @param [in] Size   The size of data
 *  @param [in] DstMac The destination MAC for transmit data
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_NetTx(UINT32 Idx, const void *Data, UINT32 Size, const UINT8 *DstMac)
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvdeccMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        Rval = Avdecc_LoopbackTx(Data, Size);
        if (Rval == OK) {
            void *EnetBuffer;
            UINT8 *U8EnetBuffer;
            UINT16 TxSize = 0U;
            UINT8 *PktBuffer;
            (void)AmbaEnet_GetTxBuf(Idx, &EnetBuffer);
            AmbaMisra_TypeCast(&U8EnetBuffer, &EnetBuffer);
            if (Avdecc_IsVlan() == TRUE) {
                PktBuffer = &U8EnetBuffer[sizeof(ETH_VLAN_HDR_s)];
                TxSize = (UINT16)sizeof(ETH_VLAN_HDR_s);
            } else {
                PktBuffer = &U8EnetBuffer[sizeof(ETH_HDR_s)];
                TxSize = (UINT16)sizeof(ETH_HDR_s);
            }
            if (AmbaWrap_memcpy(PktBuffer, Data, Size)!= 0U) { }
            TxSize += (UINT16)Size;
            Avdecc_PrepareEth(U8EnetBuffer, Avdecc_IsVlan(), DstMac, AvdeccMgr.Mac);
            Rval = AmbaEnet_Tx(Idx, TxSize);
            if (Rval != ETH_ERR_NONE) {
                AmbaPrint_PrintStr5("%s, AmbaEnet_Tx() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, Avdecc_LoopbackTx() failed!", __func__, NULL, NULL, NULL, NULL);
        }
        if (AmbaKAL_MutexGive(&AvdeccMgr.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Avdecc send event
 *  @param [in] Event The event to send
 *  @param [in] Info  The information for the event
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_SendEvent(UINT32 Event, UINT64 Info)
{
    return AvdeccMgr.EventCb(Event, Info);
}

/**
 *  Get entity type
 *  @param [in]  EntityId   The entity id use to get type
 *  @param [out] EntityType The type for the entity
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_GetEntityType(UINT64 EntityId, UINT32 *EntityType)
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvdeccMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        for(i = 0U; i < AvdeccMgr.MaxTotalEntityNum; i++) {
            if (AvdeccMgr.EntityInfo[i].EntityDesc.Entity_id == EntityId) {
                if ((AvdeccMgr.EntityInfo[i].EntityDesc.Talker_capabilities & ADP_TALKER_CAP_IMPLEMENTED) ==
                    ADP_TALKER_CAP_IMPLEMENTED) {
                    *EntityType = AVDECC_TALKER;
                } else if ((AvdeccMgr.EntityInfo[i].EntityDesc.Listener_capabilities & ADP_LISTENER_CAP_IMPLEMENTED) ==
                    ADP_LISTENER_CAP_IMPLEMENTED) {
                    *EntityType = AVDECC_LISTENER;
                } else if ((AvdeccMgr.EntityInfo[i].EntityDesc.Controller_capabilities & ADP_CTRL_CAP_IMPLEMENTED) ==
                    ADP_CTRL_CAP_IMPLEMENTED) {
                    *EntityType = AVDECC_CONTROLLER;
                } else {
                    AmbaPrint_PrintStr5("%s, unkonw entity type", __func__, NULL, NULL, NULL, NULL);
                    *EntityType = AVDECC_UNKNOW;
                }
                break;
            }
        }
        if (AmbaKAL_MutexGive(&AvdeccMgr.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Get avtp configuration of Talker
 *  @param [in]  EntityId The entity id use to get configuration
 *  @param [out] Talker   The configuration of the Talker
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_GetTalkerConfig(UINT64 EntityId, AMBA_AVB_AVTP_TALKER_s **Talker)
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvdeccMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        for (i = 0U; i < AvdeccMgr.MaxTalkerNum; i++) {
            UINT64 TalkerEntityId = Avdecc_GenEntityId(AvdeccMgr.Mac, i);
            if (EntityId == TalkerEntityId) {
                *Talker = &AvdeccMgr.AvtpTalker[i];
                break;
            }
        }
        if (AmbaKAL_MutexGive(&AvdeccMgr.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Get avtp configuration of Listener
 *  @param [in]  EntityId The entity id use to get configuration
 *  @param [out] Listener The configuration of the Listener
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_GetListenerConfig(UINT64 EntityId, AMBA_AVB_AVTP_LISTENER_s **Listener)
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvdeccMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT8 i;
        for (i = 0U; i < AvdeccMgr.MaxListenerNum; i++) {
            UINT64 ListenerEntityId = Avdecc_GenEntityId(AvdeccMgr.Mac, i);
            if (EntityId == ListenerEntityId) {
                *Listener = &AvdeccMgr.AvtpListener[i];
                break;
            }
        }
        if (AmbaKAL_MutexGive(&AvdeccMgr.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Get Avdecc connecting information
 *  @param [in]  EntityId The entity id use to get connecting information
 *  @param [out] Connect  The connect status
 *  @param [out] Info     The connecting information
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_GetConnectInfo(UINT64 EntityId, UINT32 *Connect, UINT8 **Info)
{
    UINT32 Rval = AmbaKAL_MutexTake(&AvdeccMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == KAL_ERR_NONE) {
        UINT32 EntityType;
        Rval = Avdecc_GetEntityType(EntityId, &EntityType);
        if (Rval == OK) {
            UINT32 State = 0U;
            UINT8 *ConnectInfo = NULL;
            if (EntityType == AVDECC_TALKER) {
                Rval = Talker_GetConnectInfo(EntityId, &State, &ConnectInfo);
            } else if (EntityType == AVDECC_LISTENER) {
                Rval = Listener_GetConnectInfo(EntityId, &State, &ConnectInfo);
            } else {
                Rval = OK;
            }
            *Connect = State;
            *Info = ConnectInfo;
        }
        if (AmbaKAL_MutexGive(&AvdeccMgr.Mutex) != KAL_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Show available entity information
 *  @return 0 - OK, others - Error
 */
UINT32 Avdecc_ShowAvailableEntity(void)
{
    UINT32 Rval = OK;
    Adp_ShowEntityInfo();
    return Rval;
}

