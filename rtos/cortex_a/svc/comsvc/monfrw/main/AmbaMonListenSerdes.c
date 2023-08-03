/**
 *  @file AmbaMonListenSerdes.c
 *
 *  Copyright (c) [2020] Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Amba Monitor Listen SerDes
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaVIN_Def.h"

#include "AmbaDSP_Capability.h"
#include "AmbaDSP_EventInfo.h"

#ifdef CONFIG_BUILD_MONFRW_SERDES
#include "AmbaWrap.h"
#include "AmbaSensor.h"
#include "AmbaFPD.h"
#endif
#include "AmbaMonFramework.h"
#include "AmbaMonDef.h"
#include "AmbaMonMain.h"
#include "AmbaMonMain_Internal.h"
#include "AmbaMonListenSerdes.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

typedef union /*_AMBA_MON_LISTEN_SERDES_MEM_u_*/ {
    AMBA_MON_MAIN_MEM_ADDR         Data;
    void                           *pVoid;
    AMBA_MON_MESSAGE_PORT_s        *pMsgPort;
    AMBA_MON_MESSAGE_HEADER_s      *pMsgHead;
    UINT8                          *pUint8;
#ifdef CONFIG_BUILD_MONFRW_SERDES
    UINT32                         *pUint32;
    AMBA_MON_VIN_ISR_INFO_s        *pVinIsrInfo;
    AMBA_MON_VIN_SERDES_STATE_s    *pVinSerdesState;
    AMBA_MON_VOUT_SERDES_STATE_s   *pVoutSerdesState;
#endif
} AMBA_MON_LISTEN_SERDES_MEM_u;

typedef struct /*_AMBA_MON_LISTEN_SERDES_MEM_s_*/ {
    AMBA_MON_LISTEN_SERDES_MEM_u    Ctx;
} AMBA_MON_LISTEN_SERDES_MEM_s;

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaMonListenSerdesTaskId;
static void AmbaMonListenSerdes_Task(UINT32 Param);
static void *AmbaMonListenSerdes_TaskEx(void *pParam);

static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonListenSerdes_Ready;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonListenSerdes_Go;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonListenSerdes_Ack;

static UINT32 AmbaMonListenSerdes_Cmd = (UINT32) LISTEN_SERDES_TASK_CMD_STOP;

#ifdef CONFIG_BUILD_MONFRW_SERDES
static AMBA_MON_MESSAGE_PORT_s VinSerdesStatePort;
static AMBA_MON_MESSAGE_PORT_s VoutSerdesStatePort;
static UINT32 LastSerdesState[AMBA_MON_NUM_VIN_CHANNEL] = {0};
static UINT32 LastSerdesDataState[AMBA_MON_NUM_VIN_CHANNEL] = {0};
static UINT32 LastVoutSerdesState[AMBA_MON_NUM_VOUT_CHANNEL] = {0};
#endif

/**
 *  @private
 *  Amba monitor listen serdes create
 *  @param[in] Priority task priority id
 *  @param[in] CoreInclusion cpu core inclusion (bits)
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenSerdes_Create(UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_MON_LISTEN_SERDES_TASK_STACK_SIZE    0xA000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaMonListenSerdesTaskStack[AMBA_MON_LISTEN_SERDES_TASK_STACK_SIZE];
    static char AmbaMonListenSerdesTaskName[] = "MonitorLsnSds";

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonListenSerdes_Ready, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonListenSerdes_Go, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonListenSerdes_Ack, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    if (RetCode == OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_MSG, "amba mon create listen serdes task");
        {
            AMBA_MON_LISTEN_SERDES_MEM_s MemInfo;
            MemInfo.Ctx.Data = 0U;
            FuncRetCode = AmbaKAL_TaskCreate(&AmbaMonListenSerdesTaskId,
                                             AmbaMonListenSerdesTaskName,
                                             Priority,
                                             AmbaMonListenSerdes_TaskEx,
                                             MemInfo.Ctx.pVoid,
                                             AmbaMonListenSerdesTaskStack,
                                             AMBA_MON_LISTEN_SERDES_TASK_STACK_SIZE,
                                             AMBA_KAL_DONT_START);
        }
        if (FuncRetCode == KAL_ERR_NONE) {
            FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaMonListenSerdesTaskId, CoreInclusion);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_TaskResume(&AmbaMonListenSerdesTaskId);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor listen serdes delete
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenSerdes_Delete(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_TaskTerminate(&AmbaMonListenSerdesTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_TaskDelete(&AmbaMonListenSerdesTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonListenSerdes_Ready);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonListenSerdes_Go);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonListenSerdes_Ack);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor listen serdes active
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenSerdes_Active(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if (AmbaMonListenSerdes_Cmd == (UINT32) LISTEN_SERDES_TASK_CMD_STOP) {
        /* ready take */
        FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenSerdes_Ready, AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* cmd start */
            AmbaMonListenSerdes_Cmd = (UINT32) LISTEN_SERDES_TASK_CMD_START;
            /* go release */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenSerdes_Go);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* ack take */
                FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenSerdes_Ack, AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* ack fail */
                    RetCode = NG_UL;
                }
            } else {
                /* go fail */
                RetCode = NG_UL;
            }
        } else {
            /* not ready */
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor listen serdes inactive
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenSerdes_Inactive(void)
{
    if (AmbaMonListenSerdes_Cmd == (UINT32) LISTEN_SERDES_TASK_CMD_START) {
        AmbaMonListenSerdes_Cmd = (UINT32) LISTEN_SERDES_TASK_CMD_STOP;
    }

    return OK_UL;
}

/**
 *  @private
 *  Amba monitor listen serdes idle wait
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenSerdes_IdleWait(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    /* ready take */
    FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenSerdes_Ready, AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        /* ready give */
        FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenSerdes_Ready);
        if (FuncRetCode != KAL_ERR_NONE) {
            /* ready give fail */
            RetCode = NG_UL;
        }
    } else {
        /* ready take fail */
        RetCode = NG_UL;
    }

    return RetCode;
}

#ifdef CONFIG_BUILD_MONFRW_SERDES
/**
 *  Amba monitor listen serdes state put
 *  @param[in] pPort pointer to the message port
 *  @param[in] pEvent pointer to the state data
 *  @param[in] DataSize data size
 *  @param[in] TimeoutFlag timeout flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenSerdes_StatePut(AMBA_MON_MESSAGE_PORT_s *pPort, const void *pEvent, UINT32 DataSize, UINT8 TimeoutFlag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 SerdesStateBuf[256];
#else
    UINT8 SerdesStateBuf[32];
#endif
    AMBA_MON_LISTEN_SERDES_MEM_s MemInfo;
    const void *pMem;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    void *pMsgData;
#else
    const void *pMsgData;
#endif
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* msg head get */
    MemInfo.Ctx.pUint8 = SerdesStateBuf;
    pMsgHead = MemInfo.Ctx.pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    /* msg data get */
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);
    pMsgData = MemInfo.Ctx.pVoid;
#endif
    /* msg header reset */
    pMsgHead->Ctx.Data[0] = 0ULL;
    pMsgHead->Ctx.Data[1] = 0ULL;
    pMsgHead->Ctx.Data[2] = 0ULL;

    /* msg header */
    pMsgHead->Ctx.Chunk.Id = (UINT8) AMBA_MON_MSG_ID_DATA;
    MsgLength &= 0xFFU;
    pMsgHead->Ctx.Chunk.Size = (UINT8) MsgLength;
    pMsgHead->Ctx.Chunk.Timeout = TimeoutFlag;
    pMsgHead->Ctx.Chunk.Timetick = Timetick;
#ifndef AMBA_MON_MESSAGE_PUT2
    /* msg data copy */
    FuncRetCode = AmbaWrap_memcpy(pMsgData, pEvent, DataSize);
    if (FuncRetCode != OK_UL) {
        /* */
    }
#else
    /* msg data get */
    pMsgData = pEvent;
#endif
    /* msg put */
    MemInfo.Ctx.pUint8 = SerdesStateBuf;
#ifndef AMBA_MON_MESSAGE_PUT2
    pMem = AmbaMonMessage_Put(pPort, MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size);
#else
    pMem = AmbaMonMessage_Put2(pPort, MemInfo.Ctx.pVoid, HeaderSize, pMsgData, DataSize);
#endif
    if (pMem == NULL) {
        RetCode = NG_UL;
    }

    return RetCode;
}
#endif
/**
 *  Amba monitor listen serdes message process
 *  @param[in] pListenMsg pointer to the message header
 *  @param[in] pListenEvent pointer to the listen serdes event information
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenSerdes_MsgProc(AMBA_MON_MESSAGE_HEADER_s *pListenMsg, const AMBA_MON_LISTEN_SERDES_EVENT_s *pListenEvent)
{
#ifdef CONFIG_BUILD_MONFRW_SERDES
    UINT32 FuncRetCode;
#endif
    AMBA_MON_LISTEN_SERDES_MEM_s MemInfo;
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);

    MemInfo.Ctx.pMsgHead = pListenMsg;
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);

    if (pListenEvent->Flag == pListenEvent->VinSof.Flag) {
        /* vin sof isr */
#ifdef CONFIG_BUILD_MONFRW_SERDES
        UINT32 VinId = MemInfo.Ctx.pVinIsrInfo->VinId;
        UINT32 SensorId = 0U;
        UINT32 SerdesId = MemInfo.Ctx.pVinIsrInfo->SerdesId;

        for (UINT32 i = 0U; i < AMBA_MON_NUM_VIN_SENSOR; i++) {
            if ((MemInfo.Ctx.pVinIsrInfo->SensorId & (((UINT32) 0x1U) << i)) > 0U) {
                SensorId |= AmbaMonMain_SensorIdTable[VinId][i];
            }
        }

        /* vin serdes query */
        if (VinId < AMBA_MON_NUM_VIN_CHANNEL) {
            AMBA_SENSOR_CHANNEL_s SensorChan = {0};
            AMBA_SENSOR_SERDES_STATUS_s SerDesStatus[AMBA_MON_NUM_VIN_SENSOR];
            UINT32 CurLinkState = 0U, CurDataState = 0U;

            SensorChan.VinID = VinId;
            SensorChan.SensorID = SensorId;

            FuncRetCode = AmbaWrap_memset(SerDesStatus, 0, sizeof(SerDesStatus));
            if (FuncRetCode != OK_UL) {
                /* */
            }
            FuncRetCode = AmbaSensor_GetSerdesStatus(&SensorChan, SerDesStatus);
            if (FuncRetCode == SENSOR_ERR_NONE) {

                for (UINT32 i = 0U; i < AMBA_MON_NUM_VIN_SENSOR; i++) {
                    if ((SensorId & (((UINT32) 0x1U) << ((i + 1U) << 2U))) > 0U) {
                        CurLinkState |= SerDesStatus[i].LinkLock << (i << 3U);
                        CurDataState |= SerDesStatus[i].VideoLock << (i << 3U);
                    }
                }

                /* serdes state change? */
                if ((CurLinkState != LastSerdesState[VinId]) || (CurDataState != LastSerdesDataState[VinId])) {
                    AMBA_MON_LISTEN_SERDES_MEM_s MemInfo2;
                    AMBA_MON_VIN_SERDES_STATE_s VinSerdesState;
                    /* serdes state get */
                    VinSerdesState.VinId = VinId;
                    VinSerdesState.SensorId = SensorId;
                    VinSerdesState.SerdesId = SerdesId;
                    VinSerdesState.LinkId = CurLinkState;
                    VinSerdesState.DataId = CurDataState;
                    /* serdes state put */
                    MemInfo2.Ctx.pVinSerdesState = &VinSerdesState;
                    FuncRetCode = AmbaMonListenSerdes_StatePut(&VinSerdesStatePort, MemInfo2.Ctx.pVoid, (UINT32) sizeof(AMBA_MON_VIN_SERDES_STATE_s), 0U);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* last serdes date update */
                    LastSerdesState[VinId] = CurLinkState;
                    LastSerdesDataState[VinId] = CurDataState;
                }
            }
        }
#endif
    } else if (pListenEvent->Flag == pListenEvent->VoutIsr.Flag) {
        /* vout isr */
#ifdef CONFIG_BUILD_MONFRW_SERDES
        UINT32 VoutId = *(MemInfo.Ctx.pUint32);
        if (VoutId < AMBA_MON_NUM_VOUT_CHANNEL) {
            UINT32 VoutSerdesStatus = 0U;

            FuncRetCode = AmbaFPD_GetSerdesLinkStatus(VoutId, &VoutSerdesStatus);
            if (FuncRetCode == 0U) {
                /* serdes state change? */
                if (VoutSerdesStatus != LastVoutSerdesState[VoutId]) {
                    AMBA_MON_LISTEN_SERDES_MEM_s MemInfo2;
                    AMBA_MON_VOUT_SERDES_STATE_s VoutSerdesState;
                    /* serdes state get */
                    VoutSerdesState.VoutId = VoutId;
                    VoutSerdesState.SerdesId = 0x1U;
                    VoutSerdesState.LinkId = VoutSerdesStatus;
                    /* serdes state put */
                    MemInfo2.Ctx.pVoutSerdesState = &VoutSerdesState;
                    FuncRetCode = AmbaMonListenSerdes_StatePut(&VoutSerdesStatePort, MemInfo2.Ctx.pVoid, (UINT32) sizeof(AMBA_MON_VOUT_SERDES_STATE_s), 0U);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* last serdes date update */
                    LastVoutSerdesState[VoutId] = VoutSerdesStatus;
                }
            }
        }
#endif
    } else {
        /* */
    }
}

/**
 *  Amba monitor listen serdes timeout process
 *  @param[in] pListenMsg pointer to the message header
 *  @param[in] pListenEvent pointer to the listen serdes event information
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenSerdes_TimeoutProc(AMBA_MON_MESSAGE_HEADER_s *pListenMsg, const AMBA_MON_LISTEN_SERDES_EVENT_s *pListenEvent)
{
#ifdef CONFIG_BUILD_MONFRW_SERDES
    UINT32 FuncRetCode;
#endif
    AMBA_MON_LISTEN_SERDES_MEM_s MemInfo;
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);

    MemInfo.Ctx.pMsgHead = pListenMsg;
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);

    if (pListenEvent->Flag == pListenEvent->VinSof.Flag) {
        /* vin sof timeout */
#ifdef CONFIG_BUILD_MONFRW_SERDES
        UINT32 VinId = MemInfo.Ctx.pVinIsrInfo->VinId;
        UINT32 SensorId = 0U;
        UINT32 SerdesId = MemInfo.Ctx.pVinIsrInfo->SerdesId;

        for (UINT32 i = 0U; i < AMBA_MON_NUM_VIN_SENSOR; i++) {
            if ((MemInfo.Ctx.pVinIsrInfo->SensorId & (((UINT32) 0x1U) << i)) > 0U) {
                SensorId |= AmbaMonMain_SensorIdTable[VinId][i];
            }
        }

        /* vin serdes query */
        if (VinId < AMBA_MON_NUM_VIN_CHANNEL) {
            AMBA_SENSOR_CHANNEL_s SensorChan = {0};
            AMBA_SENSOR_SERDES_STATUS_s SerDesStatus[AMBA_MON_NUM_VIN_SENSOR];
            UINT32 CurLinkState = 0U, CurDataState = 0U;

            SensorChan.VinID = VinId;
            SensorChan.SensorID = SensorId;

            FuncRetCode = AmbaWrap_memset(SerDesStatus, 0, sizeof(SerDesStatus));
            if (FuncRetCode != OK_UL) {
                /* */
            }
            FuncRetCode = AmbaSensor_GetSerdesStatus(&SensorChan, SerDesStatus);
            if (FuncRetCode == SENSOR_ERR_NONE) {

                for (UINT32 i = 0U; i < AMBA_MON_NUM_VIN_SENSOR; i++) {
                    if ((SensorId & (((UINT32) 0x1U) << ((i + 1U) << 2U))) > 0U) {
                        CurLinkState |= SerDesStatus[i].LinkLock << (i << 3U);
                        CurDataState |= SerDesStatus[i].VideoLock << (i << 3U);
                    }
                }

                /* serdes state change? */
                if ((CurLinkState != LastSerdesState[VinId]) || (CurDataState != LastSerdesDataState[VinId])) {
                    AMBA_MON_LISTEN_SERDES_MEM_s MemInfo2;
                    AMBA_MON_VIN_SERDES_STATE_s VinSerdesState;
                    /* serdes state get */
                    VinSerdesState.VinId = VinId;
                    VinSerdesState.SensorId = SensorId;
                    VinSerdesState.SerdesId = SerdesId;
                    VinSerdesState.LinkId = CurLinkState;
                    VinSerdesState.DataId = CurDataState;
                    /* serdes state put */
                    MemInfo2.Ctx.pVinSerdesState = &VinSerdesState;
                    FuncRetCode = AmbaMonListenSerdes_StatePut(&VinSerdesStatePort, MemInfo2.Ctx.pVoid, (UINT32) sizeof(AMBA_MON_VIN_SERDES_STATE_s), 1U);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* last serdes date update */
                    LastSerdesState[VinId] = CurLinkState;
                    LastSerdesDataState[VinId] = CurDataState;
                }
            }
        }
#endif
    } else if (pListenEvent->Flag == pListenEvent->VoutIsr.Flag) {
        /* vout isr timeout */
#ifdef CONFIG_BUILD_MONFRW_SERDES
        UINT32 VoutId = *(MemInfo.Ctx.pUint32);
        if (VoutId < AMBA_MON_NUM_VOUT_CHANNEL) {
            UINT32 VoutSerdesStatus = 0U;

            FuncRetCode = AmbaFPD_GetSerdesLinkStatus(VoutId, &VoutSerdesStatus);
            if (FuncRetCode == 0U) {
                /* serdes state change? */
                if (VoutSerdesStatus != LastVoutSerdesState[VoutId]) {
                    AMBA_MON_LISTEN_SERDES_MEM_s MemInfo2;
                    AMBA_MON_VOUT_SERDES_STATE_s VoutSerdesState;
                    /* serdes state get */
                    VoutSerdesState.VoutId = VoutId;
                    VoutSerdesState.SerdesId = 0x1U;
                    VoutSerdesState.LinkId = VoutSerdesStatus;
                    /* serdes state put */
                    MemInfo2.Ctx.pVoutSerdesState = &VoutSerdesState;
                    FuncRetCode = AmbaMonListenSerdes_StatePut(&VoutSerdesStatePort, MemInfo2.Ctx.pVoid, (UINT32) sizeof(AMBA_MON_VOUT_SERDES_STATE_s), 1U);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* last serdes date update */
                    LastVoutSerdesState[VoutId] = VoutSerdesStatus;
                }
            }
        }
#endif
    } else {
        /* */
    }
}

/**
 *  Amba monitor listen serdes graph delete
 *  @param[in] pListenPort pointer to message port
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenSerdes_GraphDelete(AMBA_MON_MESSAGE_PORT_s *pListenPort)
{
    UINT32 FuncRetCode;

    /* listen port in timeline */
    FuncRetCode = AmbaMonMessage_Close(pListenPort);
    if (FuncRetCode != OK_UL) {
        /* listen port fail */
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: serdes listen port clse", FuncRetCode, 16U);
    }
#ifdef CONFIG_BUILD_MONFRW_SERDES
    /* serdes state port close (vin) */
    FuncRetCode = AmbaMonMessage_Close(&VinSerdesStatePort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state serdes link port close", FuncRetCode, 16U);
    }

    /* serdes state port close (vout) */
    FuncRetCode = AmbaMonMessage_Close(&VoutSerdesStatePort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vout state serdes link port close", FuncRetCode, 16U);
    }
#endif
}

/**
 *  Amba monitor listen serdes graph create
 *  @param[in] pListenPort pointer to message port
 *  @param[in] pListenEvent pointer to listen event
 *  @param[in] pEventFlag pointer to accumulated event flag
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenSerdes_GraphCreate(AMBA_MON_MESSAGE_PORT_s *pListenPort, AMBA_MON_LISTEN_SERDES_EVENT_s *pListenEvent, UINT64 *pEventFlag)
{
    UINT32 FuncRetCode;

    AMBA_MON_LISTEN_SERDES_MEM_s MemInfo;

    UINT32 SerdesEventId;
    UINT64 SerdesEventFlag;

    /* listen port in timeline */
    FuncRetCode = AmbaMonMessage_Open(pListenPort, "timeline", NULL);
    if (FuncRetCode == OK_UL) {
        /* liste port success */
        MemInfo.Ctx.pMsgPort = pListenPort;
        AmbaMonPrintEx(S_PRINT_FLAG_DBG, "serdes listen port", (UINT32) MemInfo.Ctx.Data, 16U);
        /* vin sof event flag get */
        pListenEvent->VinSof.pName = pVinSofName;
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->VinSof.pName, &SerdesEventId, &SerdesEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= SerdesEventFlag;
            pListenEvent->VinSof.Flag = SerdesEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->VinSof.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (SerdesEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) SerdesEventFlag, 16U);
        } else {
            pListenEvent->VinSof.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->VinSof.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* vout isr event flag get */
        pListenEvent->VoutIsr.pName = pVoutName;
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->VoutIsr.pName, &SerdesEventId, &SerdesEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= SerdesEventFlag;
            pListenEvent->VoutIsr.Flag = SerdesEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->VoutIsr.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (SerdesEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) SerdesEventFlag, 16U);
        } else {
            pListenEvent->VoutIsr.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->VoutIsr.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* echo event flag get */
        pListenEvent->Echo.pName = "echo";
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->Echo.pName, &SerdesEventId, &SerdesEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= SerdesEventFlag;
            pListenEvent->Echo.Flag = SerdesEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->Echo.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (SerdesEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) SerdesEventFlag, 16U);
        } else {
            pListenEvent->Echo.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->Echo.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
    } else {
        /* listen port fail */
        MemInfo.Ctx.pMsgPort = pListenPort;
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: serdes listen port", (UINT32) MemInfo.Ctx.Data, 16U);
    }
#ifdef CONFIG_BUILD_MONFRW_SERDES
    /* serdes state port open (vin) */
    FuncRetCode = AmbaMonMessage_Open(&VinSerdesStatePort, "vin_state", "serdes_link");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state serdes link port open", FuncRetCode, 16U);
    }

    /* serdes state port open (vout) */
    FuncRetCode = AmbaMonMessage_Open(&VoutSerdesStatePort, "vout_state", "serdes_link");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vout state serdes link port open", FuncRetCode, 16U);
    }
#endif
}

/**
 *  Amba monitor listen serdes task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaMonListenSerdes_TaskEx(void *pParam)
{
    AMBA_MON_LISTEN_SERDES_MEM_s MemInfo;
    MemInfo.Ctx.pVoid = pParam;
    AmbaMonListenSerdes_Task((UINT32) MemInfo.Ctx.Data);
    return NULL;
}

/**
 *  Amba monitor listen serdes task
 *  @param[in] Param (unused)
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenSerdes_Task(UINT32 Param)
{
    static UINT32 AmbaMonListenSerdes_DummyFlag = 1U;
    static AMBA_MON_MESSAGE_PORT_s AmbaMonListenSerdes_ListenPort = {0};

     UINT8 FirstActive = 1U;
    UINT32 FuncRetCode;

    UINT32 MsgRetCode;
    UINT64 ActualFlags;

    AMBA_MON_LISTEN_SERDES_MEM_s MemInfo;

    UINT64 EventFlag = 0ULL;

    AMBA_MON_MESSAGE_PORT_s *pListenPort;
    AMBA_MON_LISTEN_SERDES_EVENT_s ListenEvent = {0};

    AMBA_MON_MESSAGE_HEADER_s *pListenMsg;
    void *pMsg;

    (void) Param;
    pListenPort = &(AmbaMonListenSerdes_ListenPort);

    AmbaMonListenSerdes_Cmd = (UINT32) LISTEN_SERDES_TASK_CMD_STOP;

    while (AmbaMonListenSerdes_DummyFlag > 0U) {
        if (AmbaMonListenSerdes_Cmd == (UINT32) LISTEN_SERDES_TASK_CMD_STOP) {
            if (FirstActive == 1U) {
                FirstActive = 0U;
            } else {
                /* graph delete */
                AmbaMonListenSerdes_GraphDelete(pListenPort);
            }
            /* ready give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenSerdes_Ready);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen serdes task idle");
            }
            /* go take */
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenSerdes_Go, AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen serdes task go");
            }
            /* even flag reset */
            EventFlag = 0ULL;
            /* graph create */
            AmbaMonListenSerdes_GraphCreate(pListenPort, &ListenEvent, &EventFlag);
            /* port reset */
            FuncRetCode = AmbaMonMessage_RSeek(pListenPort, 0U, (UINT32) R_SEEK_PASS_BEGIN);
            if(FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen serdes port seek");
            }
            /* state reset */
#ifdef CONFIG_BUILD_MONFRW_SERDES
            for (UINT32 i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
                LastSerdesState[i] = 0xFFFFFFFFU;
                LastSerdesDataState[i] = 0xFFFFFFFFU;
            }
#endif
            /* ack give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenSerdes_Ack);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen serdes task ack");
            }
        }

        ActualFlags = AmbaMonMessage_Wait(pListenPort, EventFlag, (UINT32) MON_EVENT_FLAG_OR_CLR, (UINT32) AMBA_KAL_WAIT_FOREVER);
        if ((ActualFlags & EventFlag) > 0ULL) {
            do {
                MsgRetCode = AmbaMonMessage_Get(pListenPort, &pMsg);
                if (MsgRetCode == OK_UL) {
                    /* msg get */
                    MemInfo.Ctx.pVoid = pMsg;
                    /* event flag get */
                    ListenEvent.Flag = MemInfo.Ctx.pMsgHead->Ctx.Event.Flag;
                    /* link check */
                    if (MemInfo.Ctx.pMsgHead->Ctx.Com.Id == (UINT8) AMBA_MON_MSG_ID_LINK) {
                        MemInfo.Ctx.Data = (AMBA_MON_MAIN_MEM_ADDR) MemInfo.Ctx.pMsgHead->Ctx.Link.Pointer;
                    }
                    /* chunk itself */
                    pListenMsg = MemInfo.Ctx.pMsgHead;
                    /* msg parse */
                    if (pListenMsg->Ctx.Chunk.Timeout == 0U) {
                        AmbaMonListenSerdes_MsgProc(pListenMsg, &ListenEvent);
                    } else {
                        AmbaMonListenSerdes_TimeoutProc(pListenMsg, &ListenEvent);
                    }
                } else if (MsgRetCode == MON_RING_RESET) {
                    /* port reset */
                    FuncRetCode = AmbaMonMessage_RSeek(pListenPort, 0U, (UINT32) R_SEEK_PASS_LAST);
                    if(FuncRetCode == OK_UL) {
                        MsgRetCode = OK_UL;
                    } else {
                        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen serdes port seek (reset)");
                    }
                    /* state reset */
#ifdef CONFIG_BUILD_MONFRW_SERDES
                    for (UINT32 i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
                        LastSerdesState[i] = 0xFFFFFFFFU;
                        LastSerdesDataState[i] = 0xFFFFFFFFU;
                    }
#endif
                } else if (MsgRetCode == MON_RING_OVERFLOW) {
                    /* port reset */
                    FuncRetCode = AmbaMonMessage_RSeek(pListenPort, 0U, (UINT32) R_SEEK_PASS_BEGIN);
                    if(FuncRetCode == OK_UL) {
                        MsgRetCode = OK_UL;
                    } else {
                        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen serdes port seek (overflow)");
                    }
                } else {
                    /* */
                }
            } while (MsgRetCode == OK_UL);
        }
    }
}

