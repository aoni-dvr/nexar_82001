/**
 *  @file AmbaShell_AvbSvc.c
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
 *  @details AVB service functions
 *
 */
#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaPrint.h>
#include <AmbaMisraFix.h>
#include "AmbaUtility.h"
#include "AmbaShell.h"
#include "NetStack.h"
#include "AvbSvc.h"
#include "AvbStack.h"
#include "avdecc.h"
#include "avdecc_private.h"
#include "AmbaShell_AvbSvc.h"

#if defined(CONFIG_KAL_THREADX_SMP) && (CONFIG_KAL_THREADX_SMP_NUM_CORES > 1)
#define REF_DEFAULT_CORE   (0x00000001)
#else
#define REF_DEFAULT_CORE   AMBA_KAL_CPU_CORE_MASK
#endif
#define AVB_LISTENER_NUM                (3U)
#define AVB_LISTENER_BUFFER_SIZE        (5U * 1024U * 1024U)
#define AVB_LISTENER_FRAME_QUEUE_NUM    (1024U)
#define AVB_LISTENER_RX_TIMEOUT         (1000U)
#define AVB_LISTENER_TASK_STACK_SIZE    (64U * 1024U) //64K
#define AVB_LISTENER_TASK_PRIORITY      (100U)

static UINT64 g_Listener[AVB_LISTENER_NUM];

static UINT64 g_Controller;
static UINT8 g_AvbEncode[AVB_LISTENER_NUM] = {0};

static UINT32 AvbStr2Mac(const char *Str, UINT8 *Mac)
{
    UINT8 *ptr;
    UINT32 i, j;
    UINT32 Rval = OK;
    UINT8 Byte, HalfByte, c;
    const UINT8 LowCase = 'a' - 'A';

    ptr = Mac;
    i = 0U;
    do {
        j = 0U;
        Byte = 0;
        i++;

        /* We might get a semicolon here - not required. */
        if ((i != 0U) && (*Str == ':')) {
            Str++;
        }

        do {
            c = (UINT8)(*Str);
            //'0'->0x0
            if ((c - (UINT8)'0') <= (UINT8)9) {
                HalfByte = c - (UINT8)'0';

            }
            //'a'->0xa
            else if ((UINT8)((c | LowCase) - (UINT8)'a') <= 5U) {
                HalfByte = (c | LowCase) - (UINT8)'a';
                HalfByte += (UINT8)0xa;
            }
            else {
                if(c != (UINT8)':') {
                    Rval = ERR_ARG;
                }
                break;
            }
            ++Str;
            Byte <<= 4;
            Byte  += HalfByte;
        } while (++j < 2U);
        *ptr = Byte;
        ptr++;
    } while (i < 6U);
    return Rval;
}

static void *AvbListenerTaskEntry(void *EntryArg)
{
    UINT32 Rval, Info;
    UINT32 loop = 1U;
    AmbaMisra_TouchUnused(&EntryArg);
    /*Info = (UINT32)EntryArg;*/
    if (AmbaWrap_memcpy(&Info, &EntryArg, sizeof(Info))!= 0U) { }
    AmbaPrint_PrintStr5("%s, start", __func__, NULL, NULL, NULL, NULL);
    while (loop == 1U) {
        if (g_AvbEncode[Info] == 1U) {
            AMBA_AVB_SVC_FRAME_INFO_s *Frame;
            Rval = AmbaAvbSvc_PeekFrame(g_Listener[Info], &Frame, 0, AVB_LISTENER_RX_TIMEOUT);
            if (Rval == OK) {
                if (Frame->FrameType == AMBA_AVB_FRAME_TYPE_MJPEG) {
                    //TODO:
                } else if (Frame->FrameType == AMBA_AVB_FRAME_TYPE_H264) {
                    AmbaPrint_PrintUInt5("2 FrameType = %u, FrameTimeStamp = %u, AvtpTimeStamp = %u", Frame->FrameType, Frame->FrameTimeStamp, Frame->AvtpTimeStamp, 0, 0);
                } else if (Frame->FrameType == AMBA_AVB_FRAME_TYPE_JPEG2000) {
                    //TODO:
                } else if (Frame->FrameType == AMBA_AVB_FRAME_TYPE_ACF_CARINFO) {
                    //TODO:
                } else {
                    AmbaPrint_PrintStr5("Unsupport AVB frame type!", NULL, NULL, NULL, NULL, NULL);
                    Rval = ERR_ARG;
                }
                if (Rval == OK) {
                    Rval = AmbaAvbSvc_RemoveFrame(g_Listener[Info], 1U);
                }
            }
        }
        (void) AmbaKAL_TaskSleep(10);
    }
    return NULL;
}

static UINT32 AvbListenerCreate(UINT32 StreamId)
{
    static AMBA_KAL_TASK_t AvbListenerTask[AVB_LISTENER_NUM];
    static UINT8 AvbListenerTaskStack[AVB_LISTENER_NUM][AVB_LISTENER_TASK_STACK_SIZE];
    static char AvbListenerTaskName[32U] = "AvbListenerTask";
    void *EntryArg;
    UINT32 Rval;

    /*EntryArg = (void *)StreamId;*/
    EntryArg = NULL;
    if (AmbaWrap_memcpy(&EntryArg, &StreamId, sizeof(StreamId))!= 0U) { }

    Rval = AmbaKAL_TaskCreate(&AvbListenerTask[StreamId],
                                    AvbListenerTaskName,
                                    AVB_LISTENER_TASK_PRIORITY,
                                    AvbListenerTaskEntry,
                                    EntryArg,
                                    AvbListenerTaskStack[StreamId],
                                    AVB_LISTENER_TASK_STACK_SIZE,
                                    0U);
    if (Rval == KAL_ERR_NONE) {
        Rval = AmbaKAL_TaskSetSmpAffinity(&AvbListenerTask[StreamId], REF_DEFAULT_CORE);
        if (Rval == KAL_ERR_NONE) {
            Rval = AmbaKAL_TaskResume(&AvbListenerTask[StreamId]);
            if (Rval != KAL_ERR_NONE) {
                AmbaPrint_PrintStr5("%s, AmbaKAL_TaskResume(AvbListenerTask) failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_PrintStr5("%s, AmbaKAL_TaskSetSmpAffinity(AvbListenerTask) failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_TaskCreate(AvbListenerTask) failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 AvbEventCb(const UINT32 Event, const UINT64 Info)
{
    UINT32 Rval = OK;
    UINT32 Tmp1, Tmp2;
    Tmp1 = (UINT32)((Info & 0xFFFFFFFF00000000U) >> 32U);
    Tmp2 = (UINT32)((Info & 0x00000000FFFFFFFFU));
    AmbaPrint_PrintUInt5("Info = 0x%X-%X", Tmp1, Tmp2, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("Event = 0x%X", Event, 0U, 0U, 0U, 0U);
    if (Event == AMBA_AVB_EVENT_START_STREAMING) {
        UINT32 Tmp = (((Tmp1 & 0x00FFU) << 8U) | (Tmp2 >> 24U));
        g_AvbEncode[Tmp] = 1U;
    }
    if (Event == AMBA_AVB_EVENT_STOP_STREAMING) {
        UINT32 Tmp = (((Tmp1 & 0x00FFU) << 8U) | (Tmp2 >> 24U));
        g_AvbEncode[Tmp] = 0U;
    }
    return Rval;
}

static UINT32 AvbListenerStart(void)
{
    static char AvbListenerMac[32U] = "66:77:88:99:aa:bb";
    //static char AvbMultiCastMac[32U] = "91:E0:F0:00:FE:00";
    static AMBA_AVB_SVC_FRAME_INFO_s AvbListenerFrameQueue[AVB_LISTENER_NUM][AVB_LISTENER_FRAME_QUEUE_NUM];
    static UINT8 AvbListenerFrameBuffer[AVB_LISTENER_NUM][AVB_LISTENER_BUFFER_SIZE];
    UINT8 i = 0U;
    UINT32 Rval = OK;
    AMBA_AVB_SVC_INIT_CFG_s InitCfg = {0};

    AmbaPrint_PrintStr5("%s ", __FUNCTION__, NULL, NULL, NULL, NULL);

    (void) AvbStr2Mac(AvbListenerMac, InitCfg.Mac);
    InitCfg.EventCb = AvbEventCb;
    Rval = AmbaAvbSvc_Init(&InitCfg);

    if (Rval == OK) { /* Listener 1 */
        for (i = 0U; i < AVB_LISTENER_NUM; i++) {
            AMBA_AVB_SVC_LISTENER_CFG_s ListenerCfg;
            if (AmbaWrap_memset(&ListenerCfg, 0, sizeof(AMBA_AVB_SVC_LISTENER_CFG_s))!= 0U) { }
            ListenerCfg.FrameBuffer = AvbListenerFrameBuffer[i];
            ListenerCfg.FrameBufferSize = (UINT32)AVB_LISTENER_BUFFER_SIZE;
            ListenerCfg.FrameInfo = AvbListenerFrameQueue[i];
            ListenerCfg.FrameInfoNum = (UINT32)AVB_LISTENER_FRAME_QUEUE_NUM;
            //AvbStr2Mac(AvbMultiCastMac, ListenerCfg.MultiCastMac);
            Rval = AmbaAvbSvc_CreateListener(&ListenerCfg, &g_Listener[i]);
            if (Rval == OK) {
                Rval = AvbListenerCreate(i);
            } else {
                AmbaPrint_PrintStr5("%s, AmbaAvbSvc_CreateListener() failed!", __func__, NULL, NULL, NULL, NULL);
                break;
            }
        }
    }
    if (Rval == OK) {
        AMBA_AVB_SVC_CONTROLLER_CFG_s ControllerCfg = {0};
        Rval = AmbaAvbSvc_CreateController(&ControllerCfg, &g_Controller);
        if (Rval != OK) {
            AmbaPrint_PrintStr5("%s, AmbaAvbSvc_CreateController() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 Str2EntityId(const char *Str, UINT64 *EntityId)
{
    UINT8 Tmp[8U] = {0U};
    UINT32 i = 0U, j = 0U;
    UINT32 Rval = OK;
    UINT8 Character, Byte, HalfByte = 0U;
    const UINT8 LowCase = 'a' - 'A';
    UINT16 StreamId = 0U;

    while (i < 8U) {
        Byte = 0U;
        j = 0U;
        while (j < 2U) {
            Character = (UINT8)(*Str);
            if ((Character - 48U) <= 9U) { // '0' ~ '9'
                HalfByte = Character - 48U;
            }
            if ((UINT8)((Character | LowCase) - (UINT8)'a') <= 5U) { //'a'->0xa
                HalfByte = (Character | LowCase) - (UINT8)'a';
                HalfByte += (UINT8)0xa;
            }
            Byte <<= 4U;
            Byte += HalfByte;
            Str++;
            j++;
        };
        Tmp[i] = Byte;
        i++;
    };
    StreamId = (((UINT16)Tmp[6U] << 8U) | (UINT16)Tmp[7U]);
    *EntityId = Avdecc_GenEntityId(Tmp, StreamId);
    return Rval;
}

void AmbaShell_CommandAvbSvc(UINT32 ArgCount, char * const * ArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    AmbaMisra_TouchUnused(&ArgCount);
    if (AmbaUtility_StringCompare(ArgVector[1U], "init", AmbaUtility_StringLength("init")) == 0) {
        if (AmbaUtility_StringCompare(ArgVector[2U], "talker", AmbaUtility_StringLength("talker")) == 0) {

        } else if (AmbaUtility_StringCompare(ArgVector[2U], "listener", AmbaUtility_StringLength("listener")) == 0) {
            (void) AvbListenerStart();
        } else {
            //nothing
        }
    } else if (AmbaUtility_StringCompare(ArgVector[1U], "connect", AmbaUtility_StringLength("connect")) == 0) {
        UINT64 Controller = 0U;
        UINT64 DstEntityId = 0U;
        UINT64 SrcEntityId = 0U;
        (void) Str2EntityId(ArgVector[2U], &Controller);
        (void) Str2EntityId(ArgVector[3U], &DstEntityId);
        (void) Str2EntityId(ArgVector[4U], &SrcEntityId);
        (void) AmbaAvbSvc_Connect(Controller, DstEntityId, SrcEntityId);
    } else if (AmbaUtility_StringCompare(ArgVector[1U], "disconnect", AmbaUtility_StringLength("disconnect")) == 0) {
        UINT64 Controller = 0U;
        UINT64 DstEntityId = 0U;
        UINT64 SrcEntityId = 0U;
        (void) Str2EntityId(ArgVector[2U], &Controller);
        (void) Str2EntityId(ArgVector[3U], &DstEntityId);
        (void) Str2EntityId(ArgVector[4U], &SrcEntityId);
        (void) AmbaAvbSvc_Disconnect(Controller, DstEntityId, SrcEntityId);
    } else if (AmbaUtility_StringCompare(ArgVector[1U], "start", AmbaUtility_StringLength("start")) == 0) {
        UINT64 Controller = 0U;
        UINT64 EntityId = 0U;
        (void) Str2EntityId(ArgVector[2U], &Controller);
        (void) Str2EntityId(ArgVector[3U], &EntityId);
        (void) AmbaAvbSvc_StartStreaming(Controller, EntityId);
    } else if (AmbaUtility_StringCompare(ArgVector[1U], "stop", AmbaUtility_StringLength("stop")) == 0) {
        UINT64 Controller = 0U;
        UINT64 EntityId = 0U;
        (void) Str2EntityId(ArgVector[2U], &Controller);
        (void) Str2EntityId(ArgVector[3U], &EntityId);
        (void) AmbaAvbSvc_StopStreaming(Controller, EntityId);
    } else if (AmbaUtility_StringCompare(ArgVector[1U], "living", AmbaUtility_StringLength("living")) == 0) {
        //(void) Avdecc_ShowAvailableEntity();
        static AMBA_AVB_SVC_ENTITY_INFO_s AvbEntityInfo[AMBA_AVB_SVC_ENTITY_MAX_NUM] = {0};
        UINT32 EntityNum = 0U;
        UINT8 i = 0;
        (void) AmbaAvbSvc_GetEntityList(g_Controller, &EntityNum, AvbEntityInfo);
        for (i = 0; i < EntityNum; i++) {
            UINT32 Tmp1, Tmp2, Tmp3, Tmp4;
            Tmp1 = (UINT32)((AvbEntityInfo[i].EntityId & 0xFFFFFFFF00000000U) >> 32U);
            Tmp2 = (UINT32)((AvbEntityInfo[i].EntityId & 0x00000000FFFFFFFFU));
            Tmp3 = (UINT32)((AvbEntityInfo[i].RemoteEntityId & 0xFFFFFFFF00000000U) >> 32U);
            Tmp4 = (UINT32)((AvbEntityInfo[i].RemoteEntityId & 0x00000000FFFFFFFFU));
            AmbaPrint_PrintUInt5("%X-%X, %X-%X", Tmp1, Tmp2, Tmp3, Tmp4, 0);
            AmbaPrint_PrintUInt5("%u, %u", AvbEntityInfo[i].StreamType, AvbEntityInfo[i].Status, 0, 0, 0);
        }
    } else if (AmbaUtility_StringCompare(ArgVector[1U], "local", AmbaUtility_StringLength("local")) == 0) {
        UINT8 i = 0;
        UINT32 Rval = OK;
        AVDECC_ENTITY_INFO_s *EntityInfo;
        Rval = Avdecc_GetEntityList(&EntityInfo);
        if (Rval == OK) {
            for (i = 0U; i < AVDECC_RX_ENTITY_INFO_MAX_NUM; i++) {
                if (EntityInfo[i].EntityDesc.Entity_id != 0U) {
                    UINT32 Tmp1, Tmp2;
                    AmbaPrint_PrintUInt5("------------------------------------------------", 0, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Descriptor_type = %u ", EntityInfo[i].EntityDesc.Descriptor_type, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Descriptor_index = %u ", EntityInfo[i].EntityDesc.Descriptor_index, 0, 0, 0, 0);
                    Tmp1 = (UINT32)((EntityInfo[i].EntityDesc.Entity_id & 0xFFFFFFFF00000000U) >> 32U);
                    Tmp2 = (UINT32)((EntityInfo[i].EntityDesc.Entity_id & 0x00000000FFFFFFFFU));
                    AmbaPrint_PrintUInt5("Entity_id = %X%X", Tmp1, Tmp2, 0U, 0U, 0U);
                    Tmp1 = (UINT32)((EntityInfo[i].EntityDesc.Entity_model_id & 0xFFFFFFFF00000000U) >> 32U);
                    Tmp2 = (UINT32)((EntityInfo[i].EntityDesc.Entity_model_id & 0x00000000FFFFFFFFU));
                    AmbaPrint_PrintUInt5("Entity_model_id = %X%X", Tmp1, Tmp2, 0U, 0U, 0U);
                    AmbaPrint_PrintUInt5("Entity_capabilities = %u ", EntityInfo[i].EntityDesc.Entity_capabilities, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Talker_stream_sources = %u ", EntityInfo[i].EntityDesc.Talker_stream_sources, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Talker_capabilities = %u ", EntityInfo[i].EntityDesc.Talker_capabilities, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Listener_stream_sinks = %u ", EntityInfo[i].EntityDesc.Listener_stream_sinks, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Listener_capabilities = %u ", EntityInfo[i].EntityDesc.Listener_capabilities, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Controller_capabilities = %u ", EntityInfo[i].EntityDesc.Controller_capabilities, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("Available_index = %u ", EntityInfo[i].EntityDesc.Available_index, 0, 0, 0, 0);
                    AmbaPrint_PrintUInt5("------------------------------------------------", 0, 0, 0, 0, 0);
                }
            }
        }
    } else {
        //TBD
        PrintFunc("cmd not found\n");
    }
}

