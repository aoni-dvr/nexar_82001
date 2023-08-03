/**
 *  @file AmbaMonListenAaa.c
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
 *  @details Amba Monitor Listen AAA
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaVIN_Def.h"

#include "AmbaDSP_Capability.h"
#include "AmbaDSP_EventInfo.h"

#include "AmbaDSP_Image3aStatistics.h"

#include "AmbaMonFramework.h"
#include "AmbaMonDef.h"
#include "AmbaMonMain.h"
#include "AmbaMonMain_Internal.h"

#include "AmbaMonListenAaa.h"

#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
#include "AmbaWrap.h"
#endif

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

typedef union /*_AMBA_MON_LISTEN_AAA_MEM_u_*/ {
    AMBA_MON_MAIN_MEM_ADDR       Data;
    AMBA_MON_MAIN_MEM_ADDR       *pAddr;
    //const void                   *pCvoid;
    void                         *pVoid;
    AMBA_MON_MESSAGE_PORT_s      *pMsgPort;
    AMBA_MON_MESSAGE_HEADER_s    *pMsgHead;
    UINT8                        *pUint8;
    //UINT32                       *pUint32;
    AMBA_IK_CFA_3A_DATA_s        *pCfaAaa;
    AMBA_IK_PG_3A_DATA_s         *pRgbAaa;
#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
    AMBA_MON_AAA_CFA_STATE_s     *pCfaState;
#endif
} AMBA_MON_LISTEN_AAA_MEM_u;

typedef struct /*_AMBA_MON_LISTEN_AAA_MEM_s_*/ {
    AMBA_MON_LISTEN_AAA_MEM_u    Ctx;
} AMBA_MON_LISTEN_AAA_MEM_s;

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaMonListenAaaTaskId;
static void AmbaMonListenAaa_Task(UINT32 Param);
static void *AmbaMonListenAaa_TaskEx(void *pParam);

static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonListenAaa_Ready;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonListenAaa_Go;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonListenAaa_Ack;

static UINT32 AmbaMonListenAaa_Cmd = (UINT32) LISTEN_AAA_TASK_CMD_STOP;

#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
#define AMBA_MON_LISTEN_AAA_FRAME_NUM 32U
static AMBA_MON_CFA_AWB_s GNU_SECTION_NOZEROINIT AmbaMonListenAaa_CfaAwb[AMBA_MON_LISTEN_AAA_FRAME_NUM][AMBA_MON_NUM_FOV_CHANNEL][AMBA_IK_3A_AWB_TILE_ROW_COUNT*AMBA_IK_3A_AWB_TILE_COL_COUNT];
static AMBA_MON_CFA_AWB_s GNU_SECTION_NOZEROINIT AmbaMonListenAaa_CfaAwbDSum[AMBA_MON_NUM_FOV_CHANNEL];

extern UINT32 AmbaMonListenAaa_DSumEnable;
UINT32 AmbaMonListenAaa_DSumEnable = 0U;
extern UINT32 AmbaMonListenAaa_DSumInterval;
UINT32 AmbaMonListenAaa_DSumInterval = 15U;
extern UINT32 AmbaMonListenAaa_DSumShift;
UINT32 AmbaMonListenAaa_DSumShift = 0U;
extern UINT32 AmbaMonListenAaa_DSumCnt[AMBA_MON_NUM_FOV_CHANNEL];
UINT32 AmbaMonListenAaa_DSumCnt[AMBA_MON_NUM_FOV_CHANNEL] = {0};
static UINT32 AmbaMonListenAAA_DSumZero[AMBA_MON_NUM_FOV_CHANNEL] = {0};

static AMBA_MON_MESSAGE_PORT_s VinCfaStatePort;
#endif

static UINT32 GNU_SECTION_NOZEROINIT  AmbaMonListenAaa_CfaRepeat[AMBA_MON_NUM_FOV_CHANNEL];
static UINT32 GNU_SECTION_NOZEROINIT  AmbaMonListenAaa_RgbRepeat[AMBA_MON_NUM_FOV_CHANNEL];

/**
 *  @private
 *  Amba monitor listen aaa create
 *  @param[in] Priority task priority id
 *  @param[in] CoreInclusion cpu core inclusion (bits)
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenAaa_Create(UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_MON_LISTEN_AAA_TASK_STACK_SIZE    0xA000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaMonListenAaaTaskStack[AMBA_MON_LISTEN_AAA_TASK_STACK_SIZE];
    static char AmbaMonListenAaaTaskName[] = "MonitorLsnAaa";

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonListenAaa_Ready, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonListenAaa_Go, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonListenAaa_Ack, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    if (RetCode == OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_MSG, "amba mon create listen aaa task");
        {
            AMBA_MON_LISTEN_AAA_MEM_s MemInfo;
            MemInfo.Ctx.Data = 0U;
            FuncRetCode = AmbaKAL_TaskCreate(&AmbaMonListenAaaTaskId,
                                             AmbaMonListenAaaTaskName,
                                             Priority,
                                             AmbaMonListenAaa_TaskEx,
                                             MemInfo.Ctx.pVoid,
                                             AmbaMonListenAaaTaskStack,
                                             AMBA_MON_LISTEN_AAA_TASK_STACK_SIZE,
                                             AMBA_KAL_DONT_START);
        }
        if (FuncRetCode == KAL_ERR_NONE) {
            FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaMonListenAaaTaskId, CoreInclusion);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_TaskResume(&AmbaMonListenAaaTaskId);
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
 *  Amba monitor listen aaa delete
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenAaa_Delete(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_TaskTerminate(&AmbaMonListenAaaTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_TaskDelete(&AmbaMonListenAaaTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonListenAaa_Ready);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonListenAaa_Go);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonListenAaa_Ack);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor listen aaa active
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenAaa_Active(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if (AmbaMonListenAaa_Cmd == (UINT32) LISTEN_AAA_TASK_CMD_STOP) {
        /* ready take */
        FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenAaa_Ready, AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* cmd start */
            AmbaMonListenAaa_Cmd = (UINT32) LISTEN_AAA_TASK_CMD_START;
            /* go release */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenAaa_Go);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* ack take */
                FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenAaa_Ack, AMBA_KAL_WAIT_FOREVER);
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
 *  Amba monitor listen aaa inactive
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenAaa_Inactive(void)
{
    if (AmbaMonListenAaa_Cmd == (UINT32) LISTEN_AAA_TASK_CMD_START) {
        AmbaMonListenAaa_Cmd = (UINT32) LISTEN_AAA_TASK_CMD_STOP;
    }

    return OK_UL;
}

/**
 *  @private
 *  Amba monitor listen aaa idle wait
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenAaa_IdleWait(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    /* ready take */
    FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenAaa_Ready, AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        /* ready give */
        FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenAaa_Ready);
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
#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
/**
 *  @private
 *  Amba monitor listen aaa change detection enable
 *  @param[in] Enable enable/disable
 *  @param[in] Interval time interval (frame)
 *  @param[in] Shift sum shift
 *  @note this function is intended for internal use only
 */
void AmbaMonListenAaa_CdEnable(UINT32 Enable, UINT32 Interval, UINT32 Shift)
{
    UINT32 FuncRetCode;

    if (Enable > 0UL) {
        /* enable */
        AmbaMonListenAaa_DSumInterval = Interval;
        AmbaMonListenAaa_DSumShift = Shift;
        FuncRetCode = AmbaWrap_memset(AmbaMonListenAaa_DSumCnt, 0, sizeof(AmbaMonListenAaa_DSumCnt));
        if (FuncRetCode != OK_UL) {
            /* */
        }
        AmbaMonListenAaa_DSumEnable = Enable;
    } else {
        /* disable */
        AmbaMonListenAaa_DSumEnable = Enable;
    }
}

/**
 *  Amba monitor listen aaa cfa difference
 *  @param[in] pCfaAaa pointer to the cfa aaa data
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenAaa_CfaDiff(const AMBA_IK_CFA_3A_DATA_s *pCfaAaa)
{
    static AMBA_MON_CFA_AWB_s GNU_SECTION_NOZEROINIT AmbaMonListenAaa_CfaAwbDiff[AMBA_MON_NUM_FOV_CHANNEL][AMBA_IK_3A_AWB_TILE_ROW_COUNT*AMBA_IK_3A_AWB_TILE_COL_COUNT];

    UINT32 i, j;

    UINT32 FovId;

    UINT32 AwbTileNumCol;
    UINT32 AwbTileNumRow;

    UINT32 TilePixelCnt;
    UINT32 ValidPixelCnt;

    UINT32 TileIdx;

    UINT32 LastIdx;
    UINT32 LastAvgValue;
    UINT32 DiffValue;

    UINT32 RawCapSeq;

    UINT32 R, G, B;

    /* tile r/g/b diff */
    if (pCfaAaa != NULL) {
        /* fov id get */
        FovId = pCfaAaa->Header.ChanIndex;
        /* raw cap seq get */
        RawCapSeq = pCfaAaa->Header.RawPicSeqNum;
#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV5) && !defined(CONFIG_SOC_CV52) && !defined(CONFIG_SOC_CV5X)
        /* awb tile info get */
        AwbTileNumCol =  pCfaAaa->Header.AwbTileNumCol;
        AwbTileNumRow =  pCfaAaa->Header.AwbTileNumRow;
        /* tile pixel count get */
        TilePixelCnt = ((UINT32) pCfaAaa->Header.AwbTileWidth) * ((UINT32) pCfaAaa->Header.AwbTileHeight);
#else
        /* awb tile info get */
        AwbTileNumCol =  pCfaAaa->Header.Awb.AwbTileNumCol;
        AwbTileNumRow =  pCfaAaa->Header.Awb.AwbTileNumRow;
        /* tile pixel count get */
        TilePixelCnt = ((UINT32) pCfaAaa->Header.Awb.AwbTileWidth) * ((UINT32) pCfaAaa->Header.Awb.AwbTileHeight);
#endif
        /* r/g/b diff */
        for (i = 0U; i < AwbTileNumRow; i++) {
            for (j = 0U; j < AwbTileNumCol; j++) {
                /* tile index get */
                TileIdx = (i*AwbTileNumCol)+j;
                /* valid pixel count get */
                ValidPixelCnt = TilePixelCnt - (((UINT32) pCfaAaa->Awb[TileIdx].CountMin) + ((UINT32) pCfaAaa->Awb[TileIdx].CountMax));
                /* last idx get */
                LastIdx = (RawCapSeq + AMBA_MON_LISTEN_AAA_FRAME_NUM - AmbaMonListenAaa_DSumInterval)%AMBA_MON_LISTEN_AAA_FRAME_NUM;
                /* r avg */
                LastAvgValue = AmbaMonListenAaa_CfaAwb[LastIdx][FovId][TileIdx].R;
                R = (((UINT32) pCfaAaa->Awb[TileIdx].SumR) << pCfaAaa->Header.AwbRgbShift)/ValidPixelCnt;
                AmbaMonListenAaa_CfaAwb[RawCapSeq%AMBA_MON_LISTEN_AAA_FRAME_NUM][FovId][TileIdx].R = R;
                /* r diff */
                if (R > LastAvgValue) {
                    DiffValue = R - LastAvgValue;
                } else {
                    DiffValue = LastAvgValue - R;
                }
                AmbaMonListenAaa_CfaAwbDiff[FovId][TileIdx].R = DiffValue;
                /* g avg */
                LastAvgValue = AmbaMonListenAaa_CfaAwb[LastIdx][FovId][TileIdx].G;
                G = (((UINT32) pCfaAaa->Awb[TileIdx].SumG) << pCfaAaa->Header.AwbRgbShift)/ValidPixelCnt;
                AmbaMonListenAaa_CfaAwb[RawCapSeq%AMBA_MON_LISTEN_AAA_FRAME_NUM][FovId][TileIdx].G = G;
                /* g diff */
                if (G > LastAvgValue) {
                    DiffValue = G - LastAvgValue;
                } else {
                    DiffValue = LastAvgValue - G;
                }
                AmbaMonListenAaa_CfaAwbDiff[FovId][TileIdx].G = DiffValue;
                /* b avg */
                LastAvgValue = AmbaMonListenAaa_CfaAwb[LastIdx][FovId][TileIdx].B;
                B = (((UINT32) pCfaAaa->Awb[TileIdx].SumB) << pCfaAaa->Header.AwbRgbShift)/ValidPixelCnt;
                AmbaMonListenAaa_CfaAwb[RawCapSeq%AMBA_MON_LISTEN_AAA_FRAME_NUM][FovId][TileIdx].B = B;
                /* b diff */
                if (B > LastAvgValue) {
                    DiffValue = B - LastAvgValue;
                } else {
                    DiffValue = LastAvgValue - B;
                }
                AmbaMonListenAaa_CfaAwbDiff[FovId][TileIdx].B = DiffValue;
            }
        }

        /* r/g/b diff sum */
        AmbaMonListenAaa_CfaAwbDSum[FovId].R = 0U;
        AmbaMonListenAaa_CfaAwbDSum[FovId].G = 0U;
        AmbaMonListenAaa_CfaAwbDSum[FovId].B = 0U;
        for (i = 0U; i < AwbTileNumRow; i++) {
            for (j = 0U; j < AwbTileNumCol; j++) {
                /* tile index get */
                TileIdx = (i*AwbTileNumCol)+j;
                /* diff sum */
                AmbaMonListenAaa_CfaAwbDSum[FovId].R += (AmbaMonListenAaa_CfaAwbDiff[FovId][TileIdx].R >> AmbaMonListenAaa_DSumShift);
                AmbaMonListenAaa_CfaAwbDSum[FovId].G += (AmbaMonListenAaa_CfaAwbDiff[FovId][TileIdx].G >> AmbaMonListenAaa_DSumShift);
                AmbaMonListenAaa_CfaAwbDSum[FovId].B += (AmbaMonListenAaa_CfaAwbDiff[FovId][TileIdx].B >> AmbaMonListenAaa_DSumShift);
            }
        }
    }
}

/**
 *  Amba monitor listen aaa stat put
 *  @param[in] pPort pointer to the message port
 *  @param[in] pEvent pointer to the event data
 *  @param[in] DataSize data size
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenAaa_StatePut(AMBA_MON_MESSAGE_PORT_s *pPort, const void *pEvent, UINT32 DataSize)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 AaaStateBuf[256];
#else
    UINT8 AaaStateBuf[32];
#endif
    AMBA_MON_LISTEN_AAA_MEM_s MemInfo;
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
    MemInfo.Ctx.pUint8 = AaaStateBuf;
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
    pMsgHead->Ctx.Chunk.Size = (UINT8) MsgLength;
    pMsgHead->Ctx.Chunk.Timeout = 0U;
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
    MemInfo.Ctx.pUint8 = AaaStateBuf;
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
 *  Amba monitor listen aaa message process
 *  @param[in] pListenMsg pointer to the message header
 *  @param[in] pListenEvent pointer to the listen aaa event information
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenAaa_MsgProc(AMBA_MON_MESSAGE_HEADER_s *pListenMsg, const AMBA_MON_LISTEN_AAA_EVENT_s *pListenEvent)
{
#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
    UINT32 FuncRetCode;
#endif
    UINT32 FovId;
    UINT32 RawCapSeq;

    const AMBA_IK_CFA_3A_DATA_s *pCfaAaa;

    AMBA_MON_LISTEN_AAA_MEM_s MemInfo;
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);

    MemInfo.Ctx.pMsgHead = pListenMsg;
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);

    if (pListenEvent->Flag == pListenEvent->CfaAaa.Flag) {
        /* cfa aaa get */
        MemInfo.Ctx.Data = *(MemInfo.Ctx.pAddr);
        pCfaAaa = MemInfo.Ctx.pCfaAaa;
        /* fov id get */
        FovId = pCfaAaa->Header.ChanIndex;
        /* raw cap seq get */
        RawCapSeq = pCfaAaa->Header.RawPicSeqNum;
#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
        /* sum diff */
        if (FovId < AMBA_MON_NUM_FOV_CHANNEL) {
            if (AmbaMonListenAaa_DSumEnable > 0U) {
                /* cfa diff (evaluated only) */
                AmbaMonListenAaa_CfaDiff(pCfaAaa);
                if (AmbaMonListenAaa_DSumCnt[FovId] >= AMBA_MON_LISTEN_AAA_FRAME_NUM) {
                    /* zer check */
                    if ((AmbaMonListenAaa_CfaAwbDSum[FovId].R == 0U) &&
                        (AmbaMonListenAaa_CfaAwbDSum[FovId].G == 0U) &&
                        (AmbaMonListenAaa_CfaAwbDSum[FovId].B == 0U)) {
                        /* zero */
                        if (AmbaMonListenAAA_DSumZero[FovId] == 0U) {
                            AMBA_MON_LISTEN_AAA_MEM_s MemInfo2;
                            AMBA_MON_AAA_CFA_STATE_s CfaState;
                            /* fov id */
                            CfaState.FovId = FovId;
                            /* state clr */
                            CfaState.StatusId.Data = 0U;
                            /* cd zero flag */
                            CfaState.StatusId.Bits.CdZero = (UINT8) 1U;
                            /* zero status put */
                            MemInfo2.Ctx.pCfaState = &CfaState;
                            FuncRetCode = AmbaMonListenAaa_StatePut(&VinCfaStatePort, MemInfo2.Ctx.pVoid, (UINT32) sizeof(AMBA_MON_AAA_CFA_STATE_s));
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                            /* flag update */
                            AmbaMonListenAAA_DSumZero[FovId] = 1U;
                        }
                    } else {
                        /* non-zero */
                        if (AmbaMonListenAAA_DSumZero[FovId] > 0UL) {
                            AMBA_MON_LISTEN_AAA_MEM_s MemInfo2;
                            AMBA_MON_AAA_CFA_STATE_s CfaState;
                            /* fov id */
                            CfaState.FovId = FovId;
                            /* state clr */
                            CfaState.StatusId.Data = 0U;
                            /* cd zero flag */
                            CfaState.StatusId.Bits.CdZero = (UINT8) 0U;
                            /* zero status put */
                            MemInfo2.Ctx.pCfaState = &CfaState;
                            FuncRetCode = AmbaMonListenAaa_StatePut(&VinCfaStatePort, MemInfo2.Ctx.pVoid, (UINT32) sizeof(AMBA_MON_AAA_CFA_STATE_s));
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                            /* flag update */
                            AmbaMonListenAAA_DSumZero[FovId] = 0U;
                        }
                    }
                } else {
                    AmbaMonListenAaa_DSumCnt[FovId]++;
                }
            }
        }
#endif
        /* debug msg */
        if (AmbaMonMain_Info.Debug.Bits.Dsp > 0U) {
            AmbaMonMain_TimingMarkPutByFov(FovId, "Cfa_Listen");
            {
                char str[11];
                str[0] = ' ';str[1] = ' ';

                str[0] = 'i';
                svar_utoa(FovId, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                AmbaMonMain_TimingMarkPutByFov(FovId, str);
                str[0] = 'n';
                svar_utoa(RawCapSeq, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                AmbaMonMain_TimingMarkPutByFov(FovId, str);
                str[0] = ' ';
                svar_utoa((UINT32) MemInfo.Ctx.Data, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                AmbaMonMain_TimingMarkPutByFov(FovId, str);
#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
                /* debug msg (evaluated only) */
                if (AmbaMonMain_Info.Debug.Bits.Cd > 0U) {
                    if (FovId < AMBA_MON_NUM_FOV_CHANNEL) {
                        if (AmbaMonListenAaa_DSumEnable > 0U) {
                            if (AmbaMonListenAaa_DSumCnt[FovId] >= AMBA_MON_LISTEN_AAA_FRAME_NUM) {
                                str[0] = 'r';
                                svar_utoa(AmbaMonListenAaa_CfaAwbDSum[FovId].R, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                                AmbaMonMain_TimingMarkPutByFov(FovId, str);
                                str[0] = 'g';
                                svar_utoa(AmbaMonListenAaa_CfaAwbDSum[FovId].G, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                                AmbaMonMain_TimingMarkPutByFov(FovId, str);
                                str[0] = 'b';
                                svar_utoa(AmbaMonListenAaa_CfaAwbDSum[FovId].B, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                                AmbaMonMain_TimingMarkPutByFov(FovId, str);
                            }
                        }
                    }
                }
#endif
            }
        }
        /* dsp cfa repeat check */
        if ((FovId < AMBA_MON_NUM_FOV_CHANNEL) && (pListenMsg->Ctx.Chunk.Repeat > 0U)) {
            AmbaMonListenAaa_CfaRepeat[FovId]++;
            AmbaMonPrintEx2(S_PRINT_FLAG_REPEAT, "dsp cfa repeat", FovId, 10U, AmbaMonListenAaa_CfaRepeat[FovId], 16U);
        }
    } else if (pListenEvent->Flag == pListenEvent->RgbAaa.Flag) {
        /* rgb aaa get */
        MemInfo.Ctx.Data = *(MemInfo.Ctx.pAddr);
        /* fov id get */
        FovId = MemInfo.Ctx.pRgbAaa->Header.ChanIndex;
        /* raw cap seq get */
        RawCapSeq = MemInfo.Ctx.pRgbAaa->Header.RawPicSeqNum;
        /* tile r/g/b diff */
        if (FovId < AMBA_MON_NUM_FOV_CHANNEL) {
            /* */
        }
        /* debug msg */
        if (AmbaMonMain_Info.Debug.Bits.Dsp > 0U) {
            AmbaMonMain_TimingMarkPutByFov(FovId, "Rgb_Listen");
            {
                char str[11];
                str[0] = ' ';str[1] = ' ';

                str[0] = 'i';
                svar_utoa(FovId, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                AmbaMonMain_TimingMarkPutByFov(FovId, str);
                str[0] = 'n';
                svar_utoa(RawCapSeq, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                AmbaMonMain_TimingMarkPutByFov(FovId, str);
                str[0] = ' ';
                svar_utoa((UINT32) MemInfo.Ctx.Data, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                AmbaMonMain_TimingMarkPutByFov(FovId, str);
            }
        }
        /* dsp rgb repeat check */
        if ((FovId < AMBA_MON_NUM_FOV_CHANNEL) && (pListenMsg->Ctx.Chunk.Repeat > 0U)) {
            AmbaMonListenAaa_RgbRepeat[FovId]++;
            AmbaMonPrintEx2(S_PRINT_FLAG_REPEAT, "dsp rgb repeat", FovId, 10U, AmbaMonListenAaa_RgbRepeat[FovId], 16U);
        }
    } else {
        /* */
    }
}

/**
 *  Amba monitor listen aaa timeout process
 *  @param[in] pListenMsg pointer to the message header
 *  @param[in] pListenEvent pointer to the listen aaa event information
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenAaa_TimeoutProc(AMBA_MON_MESSAGE_HEADER_s *pListenMsg, const AMBA_MON_LISTEN_AAA_EVENT_s *pListenEvent)
{
    AMBA_MON_LISTEN_AAA_MEM_s MemInfo;
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);

    MemInfo.Ctx.pMsgHead = pListenMsg;
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);

    if (pListenEvent->Flag == pListenEvent->CfaAaa.Flag) {
        /* */
    } else if (pListenEvent->Flag == pListenEvent->RgbAaa.Flag) {
        /* */
    } else {
        /* */
    }
}

/**
 *  Amba monitor listen aaa graph delete
 *  @param[in] pListenPort pointer to message port
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenAaa_GraphDelete(AMBA_MON_MESSAGE_PORT_s *pListenPort)
{
    UINT32 FuncRetCode;

    /* listen port in timeline */
    FuncRetCode = AmbaMonMessage_Close(pListenPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: aaa listen port close", FuncRetCode, 16U);
    }
#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
    /* cfa state port close (vin) */
    FuncRetCode = AmbaMonMessage_Close(&VinCfaStatePort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state aaa cfa port close", FuncRetCode, 16U);
    }
#endif
}

/**
 *  Amba monitor listen aaa graph create
 *  @param[in] pListenPort pointer to message port
 *  @param[in] pListenEvent pointer to listen event
 *  @param[in] pEventFlag pointer to accumulated event flag
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenAaa_GraphCreate(AMBA_MON_MESSAGE_PORT_s *pListenPort, AMBA_MON_LISTEN_AAA_EVENT_s *pListenEvent, UINT64 *pEventFlag)
{
    UINT32 FuncRetCode;

    AMBA_MON_LISTEN_AAA_MEM_s MemInfo;

    UINT32 AaaEventId;
    UINT64 AaaEventFlag;

    /* listen port in timeline */
    FuncRetCode = AmbaMonMessage_Open(pListenPort, "timeline", NULL);
    if (FuncRetCode == OK_UL) {
        /* listen port success */
        MemInfo.Ctx.pMsgPort = pListenPort;
        AmbaMonPrintEx(S_PRINT_FLAG_DBG, "aaa listen port", (UINT32) MemInfo.Ctx.Data, 16U);
        /* cfa aaa event flag get */
        pListenEvent->CfaAaa.pName = pDspCfaAaaName;
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->CfaAaa.pName, &AaaEventId, &AaaEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= AaaEventFlag;
            pListenEvent->CfaAaa.Flag = AaaEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->CfaAaa.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (AaaEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) AaaEventFlag, 16U);
        } else {
            pListenEvent->CfaAaa.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->CfaAaa.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* rgb aaa raw event flag get */
        pListenEvent->RgbAaa.pName = pDspRgbAaaName;
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->RgbAaa.pName, &AaaEventId, &AaaEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= AaaEventFlag;
            pListenEvent->RgbAaa.Flag = AaaEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->RgbAaa.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (AaaEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) AaaEventFlag, 16U);
        } else {
            pListenEvent->RgbAaa.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->RgbAaa.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* echo event flag get */
        pListenEvent->Echo.pName = "echo";
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->Echo.pName, &AaaEventId, &AaaEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= AaaEventFlag;
            pListenEvent->Echo.Flag = AaaEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->Echo.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (AaaEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) AaaEventFlag, 16U);
        } else {
            pListenEvent->Echo.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->Echo.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
    } else {
        /* listen port fail */
        MemInfo.Ctx.pMsgPort = pListenPort;
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: aaa listen port", (UINT32) MemInfo.Ctx.Data, 16U);
    }
#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
    /* cfa state port open (vin) */
    FuncRetCode = AmbaMonMessage_Open(&VinCfaStatePort, "vin_state", "aaa_cfa");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state aaa cfa port open", FuncRetCode, 16U);
    }
#endif
}

/**
 *  Amba monitor listen aaa task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaMonListenAaa_TaskEx(void *pParam)
{
    AMBA_MON_LISTEN_AAA_MEM_s MemInfo;
    MemInfo.Ctx.pVoid = pParam;
    AmbaMonListenAaa_Task((UINT32) MemInfo.Ctx.Data);
    return NULL;
}

/**
 *  Amba monitor listen aaa task
 *  @param[in] Param (unused)
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenAaa_Task(UINT32 Param)
{
    static UINT32 AmbaMonListenAaa_DummyFlag = 1U;
    static AMBA_MON_MESSAGE_PORT_s AmbaMonListenAaa_ListenPort = {0};

     UINT8 FirstActive = 1U;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 MsgRetCode;
    UINT64 ActualFlags;

    AMBA_MON_LISTEN_AAA_MEM_s MemInfo;

    UINT64 EventFlag = 0ULL;

    AMBA_MON_MESSAGE_PORT_s *pListenPort;
    AMBA_MON_LISTEN_AAA_EVENT_s ListenEvent = {0};

    AMBA_MON_MESSAGE_HEADER_s *pListenMsg;
    void *pMsg;

    (void) Param;
    pListenPort = &(AmbaMonListenAaa_ListenPort);

    AmbaMonListenAaa_Cmd = (UINT32) LISTEN_AAA_TASK_CMD_STOP;

    while (AmbaMonListenAaa_DummyFlag > 0U) {
        if (AmbaMonListenAaa_Cmd == (UINT32) LISTEN_AAA_TASK_CMD_STOP) {
            if (FirstActive == 1U) {
                FirstActive = 0U;
            } else {
                /* graph delete */
                AmbaMonListenAaa_GraphDelete(pListenPort);
            }
            /* ready give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenAaa_Ready);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen aaa task idle");
            }
            /* go take */
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenAaa_Go, AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen aaa task go");
            }
            /* even flag reset */
            EventFlag = 0ULL;
            /* graph create */
            AmbaMonListenAaa_GraphCreate(pListenPort, &ListenEvent, &EventFlag);
            /* port reset */
            FuncRetCode = AmbaMonMessage_RSeek(pListenPort, 0U, (UINT32) R_SEEK_PASS_BEGIN);
            if(FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen aaa port seek");
            }
            /* msg repeat reset */
            for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
                AmbaMonListenAaa_CfaRepeat[i] = 0U;
                AmbaMonListenAaa_RgbRepeat[i] = 0U;
            }
#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
            /* avg buf reset */
            FuncRetCode = AmbaWrap_memset(AmbaMonListenAaa_CfaAwb, 0, sizeof(AmbaMonListenAaa_CfaAwb));
            if (FuncRetCode != OK_UL) {
                /* */
            }
            /* diff sum reset */
            for (UINT32 i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
                AmbaMonListenAaa_CfaAwbDSum[i].R = 0xFFFFFFFFU;
                AmbaMonListenAaa_CfaAwbDSum[i].G = 0xFFFFFFFFU;
                AmbaMonListenAaa_CfaAwbDSum[i].B = 0xFFFFFFFFU;
            }
            /* diff sum zero flag reset */
            FuncRetCode = AmbaWrap_memset(AmbaMonListenAAA_DSumZero, 0, sizeof(AmbaMonListenAAA_DSumZero));
            if (FuncRetCode != OK_UL) {
                /* */
            }
            /* diff sum cnt reset */
            FuncRetCode = AmbaWrap_memset(AmbaMonListenAaa_DSumCnt, 0, sizeof(AmbaMonListenAaa_DSumCnt));
            if (FuncRetCode != OK_UL) {
                /* */
            }
#endif
            /* ack give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenAaa_Ack);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen aaa task ack");
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
                        AmbaMonListenAaa_MsgProc(pListenMsg, &ListenEvent);
                    } else {
                        AmbaMonListenAaa_TimeoutProc(pListenMsg, &ListenEvent);
                    }
                } else if (MsgRetCode == MON_RING_RESET) {
                    /* port reset */
                    FuncRetCode = AmbaMonMessage_RSeek(pListenPort, 0U, (UINT32) R_SEEK_PASS_LAST);
                    if(FuncRetCode == OK_UL) {
                        MsgRetCode = OK_UL;
                    } else {
                        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen aaa port seek (reset)");
                    }
                    /* msg repeat reset */
                    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
                        AmbaMonListenAaa_CfaRepeat[i] = 0U;
                        AmbaMonListenAaa_RgbRepeat[i] = 0U;
                    }
#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
                    /* avg buf reset */
                    FuncRetCode = AmbaWrap_memset(AmbaMonListenAaa_CfaAwb, 0, sizeof(AmbaMonListenAaa_CfaAwb));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* diff sum reset */
                    for (UINT32 i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
                        AmbaMonListenAaa_CfaAwbDSum[i].R = 0xFFFFFFFFU;
                        AmbaMonListenAaa_CfaAwbDSum[i].G = 0xFFFFFFFFU;
                        AmbaMonListenAaa_CfaAwbDSum[i].B = 0xFFFFFFFFU;
                    }
                    /* diff sum zero flag reset */
                    FuncRetCode = AmbaWrap_memset(AmbaMonListenAAA_DSumZero, 0, sizeof(AmbaMonListenAAA_DSumZero));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    /* diff sum cnt reset */
                    FuncRetCode = AmbaWrap_memset(AmbaMonListenAaa_DSumCnt, 0, sizeof(AmbaMonListenAaa_DSumCnt));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
#endif
                } else {
                    /* */
                }
            } while (MsgRetCode == OK_UL);
        }
    }
}

