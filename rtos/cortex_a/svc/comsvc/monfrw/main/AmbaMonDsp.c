/**
 *  @file AmbaMonDsp.c
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
 *  @details Amba Monitor DSP
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaVIN_Def.h"
#include "AmbaWrap.h"

#include "AmbaDSP_Capability.h"
#include "AmbaDSP_EventInfo.h"

#include "AmbaDSP_Image3aStatistics.h"

#include "AmbaMonFramework.h"
#include "AmbaMonDef.h"

#include "AmbaMonMain.h"
#include "AmbaMonMain_Internal.h"
#include "AmbaMonDsp.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

typedef union /*_AMBA_MON_MAIN_DSP_MEM_u_*/ {
    AMBA_MON_MAIN_MEM_ADDR            Data;
    AMBA_MON_MAIN_MEM_ADDR            *pAddr;
    const void                        *pCvoid;
    void                              *pVoid;
    UINT8                             *pUint8;
    AMBA_DSP_RAW_DATA_RDY_s           *pRawRdy;
    AMBA_DSP_YUV_DATA_RDY_EXTEND_s    *pYuvRdy;
    AMBA_DSP_VOUT_DATA_INFO_s         *pVoutStatus;
    AMBA_MON_DSP_FOV_LATENCY_s        *pFovLatency;
    AMBA_MON_DSP_VOUT_LATENCY_s       *pVoutLatency;
    AMBA_MON_MESSAGE_HEADER_s         *pMsgHead;
    //UINT32                            *pUint32;
    AMBA_IK_CFA_3A_DATA_s             *pCfaAaa;
    AMBA_IK_PG_3A_DATA_s              *pRgbAaa;
    AMBA_DSP_VIDEO_PATH_INFO_s        *pVideoPath;
} AMBA_MON_DSP_MEM_u;

typedef struct /*_AMBA_MON_MAIN_DSP_MEM_s_*/ {
    AMBA_MON_DSP_MEM_u    Ctx;
} AMBA_MON_DSP_MEM_s;

static UINT32 AmbaMonDsp_LastFovId = 0U;
static UINT32 AmbaMonDsp_IsDefRaw = 0U;

UINT32 AmbaMonDsp_RawSeqNum[AMBA_MON_NUM_VIN_CHANNEL];
UINT32 AmbaMonDsp_CfaRawSeqNum[AMBA_MON_NUM_FOV_CHANNEL];
UINT32 AmbaMonDsp_RgbRawSeqNum[AMBA_MON_NUM_FOV_CHANNEL];
UINT32 AmbaMonDsp_YuvSeqNum[AMBA_MON_NUM_FOV_CHANNEL];
UINT32 AmbaMonDsp_YuvAltSeqNum[AMBA_MON_NUM_FOV_CHANNEL];
UINT32 AmbaMonDsp_DispYuvSeqNum[AMBA_MON_NUM_DISP_CHANNEL];
 ULONG AmbaMonDsp_VoutBaseAddrY[AMBA_MON_NUM_VOUT_CHANNEL];

/**
 *  Amba monitor dsp raw put
 *  @param[in] pEvent pointer to the event data
 *  @param[in] TimeoutFlag timeout flag
 *  @param[in] IsDefRaw default raw indication
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonDsp_RawPut(const void *pEvent, UINT8 TimeoutFlag, UINT32 IsDefRaw)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 RawRdyBuf[256];
#else
    UINT8 RawRdyBuf[32];
#endif
    AMBA_MON_DSP_MEM_s MemInfo;
    const void *pMem;

    UINT32 VinId;
    UINT32 RawSeqNum;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    void *pMsgData;
#else
    const void *pMsgData;
#endif
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(AMBA_DSP_RAW_DATA_RDY_s);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* vin id get */
    MemInfo.Ctx.pCvoid = pEvent;
    VinId = (UINT32) MemInfo.Ctx.pRawRdy->VinId;
    RawSeqNum = (UINT32) (MemInfo.Ctx.pRawRdy->CapSequence & 0xFFFFFFFFULL);

    /* msg head get */
    MemInfo.Ctx.pUint8 = RawRdyBuf;
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
    if ((VinId < AMBA_MON_NUM_VIN_CHANNEL) && (TimeoutFlag == 0U)) {
        /* repeat check */
        pMsgHead->Ctx.Chunk.Repeat = (RawSeqNum == AmbaMonDsp_RawSeqNum[VinId]) ? 1U : 0U;
        /* raw cap seq update */
        AmbaMonDsp_RawSeqNum[VinId] = RawSeqNum;
    }
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
    if (VinId < AMBA_MON_NUM_VIN_CHANNEL) {
        MemInfo.Ctx.pUint8 = RawRdyBuf;
        if (IsDefRaw == 0U) {
            /* raw */
#ifndef AMBA_MON_MESSAGE_PUT2
            pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_DspSrcEn.Ctx.Bits.RawPut == 1U)) ? AmbaMonMessage_Put(&(DspRawMsgPort[VinId]), MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size) : NULL;
#else
            pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_DspSrcEn.Ctx.Bits.RawPut == 1U)) ? AmbaMonMessage_Put2(&(DspRawMsgPort[VinId]), MemInfo.Ctx.pVoid, HeaderSize, pMsgData, DataSize) : NULL;
#endif
            if (pMem == NULL) {
                /* */
            }
        } else {
            /* def raw */
#ifndef AMBA_MON_MESSAGE_PUT2
            pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_DspSrcEn.Ctx.Bits.RawPut == 1U)) ? AmbaMonMessage_Put(&(DspDefRawMsgPort[VinId]), MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size) : NULL;
#else
            pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_DspSrcEn.Ctx.Bits.RawPut == 1U)) ? AmbaMonMessage_Put2(&(DspDefRawMsgPort[VinId]), MemInfo.Ctx.pVoid, HeaderSize, pMsgData, DataSize) : NULL;
#endif
            if (pMem == NULL) {
                /* */
            }
        }
        /* debug msg */
        if (TimeoutFlag == 0U) {
            /* msg */
            if (AmbaMonMain_Info.Debug.Bits.Dsp > 0U) {
                if (IsDefRaw == 0U) {
                    /* raw */
                    AmbaMonMain_TimingMarkPut(VinId, "Raw_Rdy");
                } else {
                    /* def raw */
                    AmbaMonMain_TimingMarkPut(VinId, "D_Raw_Rdy");
                }
                /* debug info */
                {
                    char str[11];
                    str[0] = ' ';str[1] = ' ';

#ifndef AMBA_MON_MESSAGE_PUT2
                    MemInfo.Ctx.pVoid = pMsgData;
#else
                    MemInfo.Ctx.pCvoid = pMsgData;
#endif
                    str[0] = 'i';
                    svar_utoa(MemInfo.Ctx.pRawRdy->VinId, &(str[2]), 10U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPut(VinId, str);
                    str[0] = 'n';
                    svar_utoa((UINT32) (MemInfo.Ctx.pRawRdy->CapSequence & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPut(VinId, str);
                    str[0] = 'p';
                    svar_utoa((UINT32) (MemInfo.Ctx.pRawRdy->CapPts >> 32ULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPut(VinId, str);
                    str[0] = ' ';
                    svar_utoa((UINT32) (MemInfo.Ctx.pRawRdy->CapPts & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPut(VinId, str);
                }
            }
        } else {
            /* time out */
            if ((AmbaMonMain_Info.Debug.Bits.Dsp > 0U) &&
                (AmbaMonMain_Info.Debug.Bits.Timeout > 0U)) {
                if (IsDefRaw == 0U) {
                    /* raw */
                    AmbaMonMain_TimingMarkPut(VinId, "Raw_Rdy!");
                } else {
                    /* def raw: never enter without wdog */
                    AmbaMonMain_TimingMarkPut(VinId, "D_Raw_Rdy!");
                }
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor dsp raw ready
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonDsp_RawRdy(const void *pEvent)
{
    UINT32 FuncRetCode = OK;
    AMBA_MON_DSP_MEM_s MemInfo;

    MemInfo.Ctx.pCvoid = pEvent;
    if ((MemInfo.Ctx.pRawRdy->IsVirtChan & 0x4U) == 0U) {
        /* sensor raw */
        FuncRetCode = (AmbaMonMain_DspSrcEn.Ctx.Bits.RawSrc == 1U) ? AmbaMonDsp_RawPut(pEvent, 0U, 0U) : OK_UL;
        AmbaMonDsp_IsDefRaw = 0U;
    } else {
        /* default raw */
        FuncRetCode = (AmbaMonMain_DspSrcEn.Ctx.Bits.RawSrc == 1U) ? AmbaMonDsp_RawPut(pEvent, 0U, 1U) : OK_UL;
        AmbaMonDsp_IsDefRaw = 1U;
    }
    return FuncRetCode;
}

/**
 *  @private
 *  Amba monitor dsp raw timeout
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonDsp_RawTimeout(const void *pEvent)
{
    UINT32 FuncRetCode;
    FuncRetCode = AmbaMonDsp_RawPut(pEvent, 1U, 0U);
    return FuncRetCode;
}

/**
 *  Amba monitor dsp yuv put
 *  @param[in] pEvent pointer to the event data
 *  @param[in] TimeoutFlag timeout flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonDsp_YuvPut(const void *pEvent, UINT8 TimeoutFlag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 YuvRdyBuf[256];
#else
    UINT8 YuvRdyBuf[32];
#endif
    AMBA_MON_DSP_MEM_s MemInfo;
    const void *pMem;

    UINT32 FovId;
    UINT32 YuvSeqNum;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    void *pMsgData;
#else
    const void *pMsgData;
#endif
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(AMBA_DSP_YUV_DATA_RDY_EXTEND_s);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* fov id get */
    MemInfo.Ctx.pCvoid = pEvent;
    FovId = (UINT32) MemInfo.Ctx.pYuvRdy->ViewZoneId;
    YuvSeqNum = (UINT32) (MemInfo.Ctx.pYuvRdy->YuvSequence & 0xFFFFFFFFULL);

    /* msg head get */
    MemInfo.Ctx.pUint8 = YuvRdyBuf;
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
    if ((FovId < AMBA_MON_NUM_FOV_CHANNEL) && (TimeoutFlag == 0U)) {
        /* repeat check */
        pMsgHead->Ctx.Chunk.Repeat = (YuvSeqNum == AmbaMonDsp_YuvSeqNum[FovId]) ? 1U : 0U;
        /* raw cap seq update */
        AmbaMonDsp_YuvSeqNum[FovId] = YuvSeqNum;
    }
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
    if (FovId < AMBA_MON_NUM_FOV_CHANNEL) {
        MemInfo.Ctx.pUint8 = YuvRdyBuf;
#ifndef AMBA_MON_MESSAGE_PUT2
        pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_DspSrcEn.Ctx.Bits.YuvPut == 1U)) ? AmbaMonMessage_Put(&(DspYuvMsgPort[FovId]), MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size) : NULL;
#else
        pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_DspSrcEn.Ctx.Bits.YuvPut == 1U)) ? AmbaMonMessage_Put2(&(DspYuvMsgPort[FovId]), MemInfo.Ctx.pVoid, HeaderSize, pMsgData, DataSize) : NULL;
#endif
        if (pMem == NULL) {
            /* */
        }
        /* debug msg */
        if (TimeoutFlag == 0U) {
            if (AmbaMonMain_Info.Debug.Bits.Dsp > 0U) {
                AmbaMonMain_TimingMarkPutByFov(FovId, "Yuv_Rdy");
                {
                    char str[11];
                    str[0] = ' ';str[1] = ' ';

#ifndef AMBA_MON_MESSAGE_PUT2
                    MemInfo.Ctx.pVoid = pMsgData;
#else
                    MemInfo.Ctx.pCvoid = pMsgData;
#endif
                    str[0] = 'i';
                    svar_utoa(MemInfo.Ctx.pYuvRdy->ViewZoneId, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                    str[0] = 'v';
                    svar_utoa((UINT32) MemInfo.Ctx.pYuvRdy->VinId, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                    str[0] = 'n';
                    svar_utoa((UINT32) (MemInfo.Ctx.pYuvRdy->CapSequence & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                    str[0] = 'y';
                    svar_utoa((UINT32) (MemInfo.Ctx.pYuvRdy->YuvSequence & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                    str[0] = 'f';
                    svar_utoa((UINT32) MemInfo.Ctx.pYuvRdy->SourceViewBit, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                    str[0] = 'p';
                    svar_utoa((UINT32) (MemInfo.Ctx.pYuvRdy->YuvPts >> 32ULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                    str[0] = ' ';
                    svar_utoa((UINT32) (MemInfo.Ctx.pYuvRdy->YuvPts & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                    str[0] = 'b';
                    svar_utoa((UINT32) MemInfo.Ctx.pYuvRdy->Buffer.BaseAddrY, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                }
            }
        } else {
            /* timeout */
            if ((AmbaMonMain_Info.Debug.Bits.Dsp > 0U) &&
                (AmbaMonMain_Info.Debug.Bits.Timeout > 0U)) {
                AmbaMonMain_TimingMarkPutByFov(FovId, "Yuv_Rdy!");
            }
        }
    }

    return RetCode;
}

/**
 *  Amba monitor dsp yuv alt put
 *  @param[in] pEvent pointer to the event data
 *  @param[in] TimeoutFlag timeout flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonDsp_YuvAltPut(const void *pEvent, UINT8 TimeoutFlag, UINT32 IsDefRaw)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 YuvRdyBuf[256];
#else
    UINT8 YuvRdyBuf[32];
#endif
    AMBA_MON_DSP_MEM_s MemInfo;
    const void *pMem;

    UINT32 FovId;
    UINT32 YuvSeqNum;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    void *pMsgData;
#else
    const void *pMsgData;
#endif
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(AMBA_DSP_YUV_DATA_RDY_EXTEND_s);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* fov id get */
    MemInfo.Ctx.pCvoid = pEvent;
    FovId = (UINT32) MemInfo.Ctx.pYuvRdy->ViewZoneId;
    YuvSeqNum = (UINT32) (MemInfo.Ctx.pYuvRdy->YuvSequence & 0xFFFFFFFFULL);

    /* msg head get */
    MemInfo.Ctx.pUint8 = YuvRdyBuf;
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
    if ((FovId < AMBA_MON_NUM_FOV_CHANNEL) && (TimeoutFlag == 0U)) {
        /* repeat check */
        pMsgHead->Ctx.Chunk.Repeat = (YuvSeqNum == AmbaMonDsp_YuvAltSeqNum[FovId]) ? 1U : 0U;
        /* raw cap seq update */
        AmbaMonDsp_YuvAltSeqNum[FovId] = YuvSeqNum;
    }
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
    if (FovId < AMBA_MON_NUM_FOV_CHANNEL) {
        MemInfo.Ctx.pUint8 = YuvRdyBuf;
        if (IsDefRaw == 0U) {
            /* raw input */
#ifndef AMBA_MON_MESSAGE_PUT2
            pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_DspSrcEn.Ctx.Bits.YuvAltPut == 1U)) ? AmbaMonMessage_Put(&(DspYuvAltMsgPort[FovId]), MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size) : NULL;
#else
            pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_DspSrcEn.Ctx.Bits.YuvAltPut == 1U)) ? AmbaMonMessage_Put2(&(DspYuvAltMsgPort[FovId]), MemInfo.Ctx.pVoid, HeaderSize, pMsgData, DataSize) : NULL;
#endif
            if (pMem == NULL) {
                /* */
            }
        } else {
            /* def raw input */
#ifndef AMBA_MON_MESSAGE_PUT2
            pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_DspSrcEn.Ctx.Bits.YuvAltPut == 1U)) ? AmbaMonMessage_Put(&(DspDefYuvAltMsgPort[FovId]), MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size) : NULL;
#else
            pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_DspSrcEn.Ctx.Bits.YuvAltPut == 1U)) ? AmbaMonMessage_Put2(&(DspDefYuvAltMsgPort[FovId]), MemInfo.Ctx.pVoid, HeaderSize, pMsgData, DataSize) : NULL;
#endif
            if (pMem == NULL) {
                /* */
            }
        }
        /* debug msg */
        if (TimeoutFlag == 0U) {
            /* msg */
            if (AmbaMonMain_Info.Debug.Bits.Dsp > 0U) {
                if (IsDefRaw == 0U) {
                    /* raw input */
                    AmbaMonMain_TimingMarkPutByFov(FovId, "Yuv_Alt");
                } else {
                    /* def raw input */
                    AmbaMonMain_TimingMarkPutByFov(FovId, "D_Yuv_Alt");
                }
                {
                    char str[11];
                    str[0] = ' ';str[1] = ' ';

#ifndef AMBA_MON_MESSAGE_PUT2
                    MemInfo.Ctx.pVoid = pMsgData;
#else
                    MemInfo.Ctx.pCvoid = pMsgData;
#endif
                    str[0] = 'i';
                    svar_utoa(MemInfo.Ctx.pYuvRdy->ViewZoneId, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                    str[0] = 'v';
                    svar_utoa((UINT32) MemInfo.Ctx.pYuvRdy->VinId, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                    str[0] = 'n';
                    svar_utoa((UINT32) (MemInfo.Ctx.pYuvRdy->CapSequence & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                    str[0] = 'y';
                    svar_utoa((UINT32) (MemInfo.Ctx.pYuvRdy->YuvSequence & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                    str[0] = 'f';
                    svar_utoa((UINT32) MemInfo.Ctx.pYuvRdy->SourceViewBit, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                    str[0] = 'p';
                    svar_utoa((UINT32) (MemInfo.Ctx.pYuvRdy->YuvPts >> 32ULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                    str[0] = ' ';
                    svar_utoa((UINT32) (MemInfo.Ctx.pYuvRdy->YuvPts & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                    str[0] = 'b';
                    svar_utoa((UINT32) MemInfo.Ctx.pYuvRdy->Buffer.BaseAddrY, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                }
            }
        } else {
            /* timeout */
            if ((AmbaMonMain_Info.Debug.Bits.Dsp > 0U) &&
                (AmbaMonMain_Info.Debug.Bits.Timeout > 0U)) {
                if (IsDefRaw == 0U) {
                    /* raw input */
                    AmbaMonMain_TimingMarkPutByFov(FovId, "Yuv_Alt!");
                } else {
                    /* def raw input */
                    AmbaMonMain_TimingMarkPutByFov(FovId, "D_Yuv_Alt!");
                }
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor dsp yuv ready
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonDsp_YuvRdy(const void *pEvent)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_MON_DSP_MEM_s MemInfo;
    AMBA_DSP_YUV_DATA_RDY_EXTEND_s YuvRdy = {0};

    /* fov yuv data get */
    MemInfo.Ctx.pYuvRdy = &YuvRdy;
    FuncRetCode = AmbaWrap_memcpy(MemInfo.Ctx.pVoid, pEvent, sizeof(AMBA_DSP_YUV_DATA_RDY_EXTEND_s));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* fov view zone id check */
    if ((MemInfo.Ctx.pYuvRdy->ViewZoneId & 0x80000000U) > 0U) {
        MemInfo.Ctx.pYuvRdy->ViewZoneId &= 0x7FFFFFFFU;
        /* stream id check */
        if (MemInfo.Ctx.pYuvRdy->ViewZoneId >= AmbaMonMain_Info.Vout.Num) {
            /* fov id find */
            MemInfo.Ctx.pYuvRdy->ViewZoneId -= AmbaMonMain_Info.Vout.Num;
            if (AmbaMonMain_Info.Debug.Bits.Dsp > 0U) {
                char str[11];
                str[0] = ' ';str[1] = ' ';
                svar_utoa(MemInfo.Ctx.pYuvRdy->ViewZoneId, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                AmbaMonMain_TimingMarkPutByFov(MemInfo.Ctx.pYuvRdy->ViewZoneId, "VProc_Rdy");
                AmbaMonMain_TimingMarkPutByFov(MemInfo.Ctx.pYuvRdy->ViewZoneId, str);
            }
            /* fov yuv put */
            FuncRetCode = (AmbaMonMain_DspSrcEn.Ctx.Bits.YuvSrc == 1U) ? AmbaMonDsp_YuvPut(MemInfo.Ctx.pCvoid, 0U) : OK_UL;
            if (FuncRetCode != OK_UL) {
                RetCode = NG_UL;
            }
        }
    } else {
        /* bit[31] = 0 for safety's vproc message */
        if (AmbaMonMain_Info.Debug.Bits.Dsp > 0U) {
            char str[11];
            str[0] = ' ';str[1] = ' ';
            svar_utoa(MemInfo.Ctx.pYuvRdy->ViewZoneId, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
            AmbaMonMain_TimingMarkPutByFov(MemInfo.Ctx.pYuvRdy->ViewZoneId, "VProc_Rdy");
            AmbaMonMain_TimingMarkPutByFov(MemInfo.Ctx.pYuvRdy->ViewZoneId, str);
        }
        /* fov yuv (alt) put */
        if ((MemInfo.Ctx.pYuvRdy->IsVirtChan & 0x4U) == 0U) {
            if (AmbaMonDsp_IsDefRaw == 0U) {
                /* raw input */
                FuncRetCode = (AmbaMonMain_DspSrcEn.Ctx.Bits.YuvAltSrc == 1U) ? AmbaMonDsp_YuvAltPut(MemInfo.Ctx.pCvoid, 0U, 0U) : OK_UL;
            } else {
                /* def raw input */
                FuncRetCode = (AmbaMonMain_DspSrcEn.Ctx.Bits.YuvAltSrc == 1U) ? AmbaMonDsp_YuvAltPut(MemInfo.Ctx.pCvoid, 0U, 1U) : OK_UL;
            }
        } else {
            /* def raw input */
            FuncRetCode = (AmbaMonMain_DspSrcEn.Ctx.Bits.YuvAltSrc == 1U) ? AmbaMonDsp_YuvAltPut(MemInfo.Ctx.pCvoid, 0U, 1U) : OK_UL;
        }
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
        /* last fov id */
        AmbaMonDsp_LastFovId = MemInfo.Ctx.pYuvRdy->ViewZoneId;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor dsp yuv timeout
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonDsp_YuvTimeout(const void *pEvent)
{
    UINT32 FuncRetCode;
    FuncRetCode = AmbaMonDsp_YuvPut(pEvent, 1U);
    return FuncRetCode;
}

/**
 *  @private
 *  Amba monitor dsp yuv alt timeout
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonDsp_YuvAltTimeout(const void *pEvent)
{
    UINT32 FuncRetCode;
    FuncRetCode = AmbaMonDsp_YuvAltPut(pEvent, 1U, 0U);
    return FuncRetCode;
}

/**
 *  Amba monitor dsp display yuv put
 *  @param[in] pEvent pointer to the event data
 *  @param[in] TimeoutFlag timeout flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonDsp_DispYuvPut(const void *pEvent, UINT8 TimeoutFlag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 YuvRdyBuf[256];
#else
    UINT8 YuvRdyBuf[32];
#endif
    AMBA_MON_DSP_MEM_s MemInfo;
    const void *pMem;

    UINT32 DispId;
    UINT32 YuvSeqNum;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    void *pMsgData;
#else
    const void *pMsgData;
#endif
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(AMBA_DSP_YUV_DATA_RDY_EXTEND_s);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* fov id get */
    MemInfo.Ctx.pCvoid = pEvent;
    DispId = (UINT32) MemInfo.Ctx.pYuvRdy->ViewZoneId;
    YuvSeqNum = (UINT32) (MemInfo.Ctx.pYuvRdy->YuvSequence & 0xFFFFFFFFULL);

    /* msg head get */
    MemInfo.Ctx.pUint8 = YuvRdyBuf;
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
    if ((DispId < AMBA_MON_NUM_DISP_CHANNEL) && (TimeoutFlag == 0U)) {
        /* repeat check */
        pMsgHead->Ctx.Chunk.Repeat = (YuvSeqNum == AmbaMonDsp_DispYuvSeqNum[DispId]) ? 1U : 0U;
        /* raw cap seq update */
        AmbaMonDsp_DispYuvSeqNum[DispId] = YuvSeqNum;
    }
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
    if (DispId < AMBA_MON_NUM_DISP_CHANNEL) {
        MemInfo.Ctx.pUint8 = YuvRdyBuf;
#ifndef AMBA_MON_MESSAGE_PUT2
        pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_DspSrcEn.Ctx.Bits.DispPut == 1U)) ? AmbaMonMessage_Put(&(DspDispMsgPort[DispId]), MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size) : NULL;
#else
        pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_DspSrcEn.Ctx.Bits.DispPut == 1U)) ? AmbaMonMessage_Put2(&(DspDispMsgPort[DispId]), MemInfo.Ctx.pVoid, HeaderSize, pMsgData, DataSize) : NULL;
#endif
        if (pMem == NULL) {
            /* */
        }
        /* debug msg */
        if (TimeoutFlag == 0U) {
            /* msg */
            if (AmbaMonMain_Info.Debug.Bits.Dsp > 0U) {
                AmbaMonMain_TimingMarkPutByVout(DispId, "Disp_Rdy");
                {
                    char str[11];
                    str[0] = ' ';str[1] = ' ';

#ifndef AMBA_MON_MESSAGE_PUT2
                    MemInfo.Ctx.pVoid = pMsgData;
#else
                    MemInfo.Ctx.pCvoid = pMsgData;
#endif
                    str[0] = 'i';
                    svar_utoa(MemInfo.Ctx.pYuvRdy->ViewZoneId, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(DispId, str);
                    str[0] = 'v';
                    svar_utoa((UINT32) MemInfo.Ctx.pYuvRdy->VinId, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(DispId, str);
                    str[0] = 'n';
                    svar_utoa((UINT32) (MemInfo.Ctx.pYuvRdy->CapSequence & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(DispId, str);
                    str[0] = 'y';
                    svar_utoa((UINT32) (MemInfo.Ctx.pYuvRdy->YuvSequence & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(DispId, str);
                    str[0] = 'f';
                    svar_utoa((UINT32) MemInfo.Ctx.pYuvRdy->SourceViewBit, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(DispId, str);
                    str[0] = 'p';
                    svar_utoa((UINT32) (MemInfo.Ctx.pYuvRdy->YuvPts >> 32ULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(DispId, str);
                    str[0] = ' ';
                    svar_utoa((UINT32) (MemInfo.Ctx.pYuvRdy->YuvPts & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(DispId, str);
                    str[0] = 'b';
                    svar_utoa((UINT32) MemInfo.Ctx.pYuvRdy->Buffer.BaseAddrY, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(DispId, str);
                }
            }
        } else {
            /* timeout */
            if ((AmbaMonMain_Info.Debug.Bits.Dsp > 0U) &&
                (AmbaMonMain_Info.Debug.Bits.Timeout > 0U)) {
                AmbaMonMain_TimingMarkPutByVout(DispId, "Disp_Rdy!");
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor dsp display yuv ready
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonDsp_DispYuvRdy(const void *pEvent)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, VoutCnt;

    AMBA_MON_DSP_MEM_s MemInfo;
    AMBA_DSP_YUV_DATA_RDY_EXTEND_s YuvRdy = {0};

    /* disp yuv data get */
    MemInfo.Ctx.pYuvRdy = &YuvRdy;
    FuncRetCode = AmbaWrap_memcpy(MemInfo.Ctx.pVoid, pEvent, sizeof(AMBA_DSP_YUV_DATA_RDY_EXTEND_s));
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* disp view zone id check */
    if ((MemInfo.Ctx.pYuvRdy->ViewZoneId & 0x80000000U) > 0U) {
        MemInfo.Ctx.pYuvRdy->ViewZoneId &= 0x7FFFFFFFU;
        /* stream id check */
        if (MemInfo.Ctx.pYuvRdy->ViewZoneId < AmbaMonMain_Info.Vout.Num) {
            VoutCnt = 0U;
            /* disp id find */
            for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
                if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
                    if (MemInfo.Ctx.pYuvRdy->ViewZoneId == VoutCnt) {
                        break;
                    }
                    VoutCnt++;
                }
            }
            /* disp id */
            MemInfo.Ctx.pYuvRdy->ViewZoneId = i;
            /* temporary workaround for the fov bits mask of single fov */
            if (MemInfo.Ctx.pYuvRdy->SourceViewBit == 0U) {
                MemInfo.Ctx.pYuvRdy->SourceViewBit = (UINT16) ((((UINT32) 1U) << (AmbaMonDsp_LastFovId & 0x1FU)) & 0xFFFFU);
            }
            /* disp yuv put */
            FuncRetCode = (AmbaMonMain_DspSrcEn.Ctx.Bits.DispSrc == 1U) ? AmbaMonDsp_DispYuvPut(MemInfo.Ctx.pCvoid, 0U) : OK_UL;
            if (FuncRetCode != OK_UL) {
                RetCode = NG_UL;
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor dsp display yuv timeout
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonDsp_DispYuvTimeout(const void *pEvent)
{
    UINT32 FuncRetCode;
    FuncRetCode = AmbaMonDsp_DispYuvPut(pEvent, 1);
    return FuncRetCode;
}

/**
 *  Amba monitor dsp vout yuv put
 *  @param[in] pEvent pointer to the event data
 *  @param[in] TimeoutFlag timeout flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonDsp_VoutYuvPut(const void *pEvent, UINT8 TimeoutFlag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 VoutRdyBuf[256];
#else
    UINT8 VoutRdyBuf[32];
#endif
    AMBA_MON_DSP_MEM_s MemInfo;
    const void *pMem;

    UINT32 VoutId;
     ULONG BaseAddrY;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    void *pMsgData;
#else
    const void *pMsgData;
#endif
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(AMBA_DSP_VOUT_DATA_INFO_s);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* fov id get */
    MemInfo.Ctx.pCvoid = pEvent;
    VoutId = (UINT32) MemInfo.Ctx.pVoutStatus->VoutIdx;
    BaseAddrY = MemInfo.Ctx.pVoutStatus->YuvBuf.BaseAddrY;

    /* msg head get */
    MemInfo.Ctx.pUint8 = VoutRdyBuf;
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
    if ((VoutId < AMBA_MON_NUM_VOUT_CHANNEL) && (TimeoutFlag == 0U)) {
        /* repeat check */
        pMsgHead->Ctx.Chunk.Repeat = (BaseAddrY == AmbaMonDsp_VoutBaseAddrY[VoutId]) ? 1U : 0U;
        /* raw cap seq update */
        AmbaMonDsp_VoutBaseAddrY[VoutId] = BaseAddrY;
    }
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
    if (VoutId < AMBA_MON_NUM_VOUT_CHANNEL) {
        MemInfo.Ctx.pUint8 = VoutRdyBuf;
#ifndef AMBA_MON_MESSAGE_PUT2
        pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_DspSrcEn.Ctx.Bits.VoutPut == 1U)) ? AmbaMonMessage_Put(&(DspVoutMsgPort[VoutId]), MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size) : NULL;
#else
        pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_DspSrcEn.Ctx.Bits.VoutPut == 1U)) ? AmbaMonMessage_Put2(&(DspVoutMsgPort[VoutId]), MemInfo.Ctx.pVoid, HeaderSize, pMsgData, DataSize) : NULL;
#endif
        if (pMem == NULL) {
            /* */
        }
        /* debug msg */
        if (TimeoutFlag == 0U) {
            /* msg */
            if (AmbaMonMain_Info.Debug.Bits.DspVout > 0U) {
                AmbaMonMain_TimingMarkPutByVout(VoutId, "Vout_Rdy");
                {
                    char str[11];
                    str[0] = ' ';str[1] = ' ';

#ifndef AMBA_MON_MESSAGE_PUT2
                    MemInfo.Ctx.pVoid = pMsgData;
#else
                    MemInfo.Ctx.pCvoid = pMsgData;
#endif
                    str[0] = 'i';
                    svar_utoa(MemInfo.Ctx.pVoutStatus->VoutIdx, &(str[2]), 10U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                    str[0] = 'p';
                    svar_utoa((UINT32) (MemInfo.Ctx.pVoutStatus->DispStartTime >> 32ULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                    str[0] = ' ';
                    svar_utoa((UINT32) (MemInfo.Ctx.pVoutStatus->DispStartTime & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                    str[0] = 'b';
                    svar_utoa((UINT32) MemInfo.Ctx.pVoutStatus->YuvBuf.BaseAddrY, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                }
            }
        } else {
            /* timeout */
            if ((AmbaMonMain_Info.Debug.Bits.DspVout > 0U) &&
                (AmbaMonMain_Info.Debug.Bits.Timeout > 0U)) {
                AmbaMonMain_TimingMarkPutByVout(VoutId, "Vout_Rdy!");
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor dsp vout yuv ready
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonDsp_VoutYuvRdy(const void *pEvent)
{
    UINT32 FuncRetCode;
    FuncRetCode = (AmbaMonMain_DspSrcEn.Ctx.Bits.VoutSrc == 1U) ? AmbaMonDsp_VoutYuvPut(pEvent, 0) : OK_UL;
    return FuncRetCode;
}

/**
 *  @private
 *  Amba monitor dsp vout yuv timeout
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonDsp_VoutYuvTimeout(const void *pEvent)
{
    UINT32 FuncRetCode;
    FuncRetCode = AmbaMonDsp_VoutYuvPut(pEvent, 1);
    return FuncRetCode;
}

/**
 *  Amba monitor dsp cfa aaa put
 *  @param[in] pEvent pointer to the event data
 *  @param[in] TimeoutFlag timeout flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonDsp_CfaAaaPut(const void *pEvent, UINT8 TimeoutFlag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
    UINT8 CfaRdyBuf[256];

    AMBA_MON_DSP_MEM_s MemInfo;
    const void *pMem;

    UINT32 FovId;
    UINT32 RawSeqNum;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
    void *pMsgData;

    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(void *);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* fov id get */
    MemInfo.Ctx.pCvoid = pEvent;
    FovId = (UINT32) MemInfo.Ctx.pCfaAaa->Header.ChanIndex;
    RawSeqNum = MemInfo.Ctx.pCfaAaa->Header.RawPicSeqNum;

    /* msg head get */
    MemInfo.Ctx.pUint8 = CfaRdyBuf;
    pMsgHead = MemInfo.Ctx.pMsgHead;

    /* msg data get */
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);
    pMsgData = MemInfo.Ctx.pVoid;

    /* msg header reset */
    pMsgHead->Ctx.Data[0] = 0ULL;
    pMsgHead->Ctx.Data[1] = 0ULL;
    pMsgHead->Ctx.Data[2] = 0ULL;

    /* msg header */
    pMsgHead->Ctx.Chunk.Id = (UINT8) AMBA_MON_MSG_ID_DATA;
    pMsgHead->Ctx.Chunk.Size = (UINT8) MsgLength;
    if ((FovId < AMBA_MON_NUM_FOV_CHANNEL) && (TimeoutFlag == 0U)) {
        /* repeat check */
        pMsgHead->Ctx.Chunk.Repeat = (RawSeqNum == AmbaMonDsp_CfaRawSeqNum[FovId]) ? 1U : 0U;
        /* raw cap seq update */
        AmbaMonDsp_CfaRawSeqNum[FovId] = RawSeqNum;
    }
    pMsgHead->Ctx.Chunk.Timeout = TimeoutFlag;
    pMsgHead->Ctx.Chunk.Timetick = Timetick;

    /* msg data copy */
    FuncRetCode = AmbaWrap_memcpy(pMsgData, &pEvent, DataSize);
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* msg put */
    if (FovId < AMBA_MON_NUM_FOV_CHANNEL) {
        MemInfo.Ctx.pUint8 = CfaRdyBuf;
        pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_DspSrcEn.Ctx.Bits.CfaPut == 1U)) ? AmbaMonMessage_Put(&(DspCfaAaaMsgPort[FovId]), MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size) : NULL;
        if (pMem == NULL) {
            /* */
        }
        /* debug msg */
        if (TimeoutFlag == 0U) {
            /* msg */
            if (AmbaMonMain_Info.Debug.Bits.Dsp > 0U) {
                AmbaMonMain_TimingMarkPutByFov(FovId, "Cfa_Rdy");
                {
                    char str[11];
                    str[0] = ' ';str[1] = ' ';

                    MemInfo.Ctx.pVoid = pMsgData;
                    MemInfo.Ctx.Data = *(MemInfo.Ctx.pAddr);

                    str[0] = 'i';
                    svar_utoa(MemInfo.Ctx.pCfaAaa->Header.ChanIndex, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                    str[0] = 'n';
                    svar_utoa(MemInfo.Ctx.pRgbAaa->Header.RawPicSeqNum, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                    str[0] = ' ';
                    svar_utoa((UINT32) MemInfo.Ctx.Data, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                }
            }
        } else {
            /* timeout */
            if ((AmbaMonMain_Info.Debug.Bits.Dsp > 0U) &&
                (AmbaMonMain_Info.Debug.Bits.Timeout > 0U)) {
                AmbaMonMain_TimingMarkPutByFov(FovId, "Cfa_Rdy!");
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor dsp cfa aaa ready
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonDsp_CfaAaaRdy(const void *pEvent)
{
    UINT32 FuncRetCode;
    AMBA_MON_DSP_MEM_s MemInfo;

    MemInfo.Ctx.pCvoid = pEvent;
    if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << MemInfo.Ctx.pCfaAaa->Header.ChanIndex)) > 0U) {
        FuncRetCode = (AmbaMonMain_DspSrcEn.Ctx.Bits.CfaSrc == 1U) ? AmbaMonDsp_CfaAaaPut(pEvent, 0) : OK_UL;
    } else {
        FuncRetCode = 0U;
    }

    return FuncRetCode;
}

/**
 *  @private
 *  Amba monitor dsp cfa aaa timeout
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonDsp_CfaAaaTimeout(const void *pEvent)
{
    UINT32 FuncRetCode;
    FuncRetCode = AmbaMonDsp_CfaAaaPut(pEvent, 1);
    return FuncRetCode;
}

/**
 *  Amba monitor dsp rgb aaa put
 *  @param[in] pEvent pointer to the event data
 *  @param[in] TimeoutFlag timeout flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonDsp_RgbAaaPut(const void *pEvent, UINT8 TimeoutFlag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
    UINT8 RgbRdyBuf[256];

    AMBA_MON_DSP_MEM_s MemInfo;
    const void *pMem;

    UINT32 FovId;
    UINT32 RawSeqNum;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
    void *pMsgData;

    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(void *);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* fov id get */
    MemInfo.Ctx.pCvoid = pEvent;
    FovId = (UINT32) MemInfo.Ctx.pRgbAaa->Header.ChanIndex;
    RawSeqNum = MemInfo.Ctx.pCfaAaa->Header.RawPicSeqNum;

    /* msg head get */
    MemInfo.Ctx.pUint8 = RgbRdyBuf;
    pMsgHead = MemInfo.Ctx.pMsgHead;

    /* msg data get */
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);
    pMsgData = MemInfo.Ctx.pVoid;

    /* msg header reset */
    pMsgHead->Ctx.Data[0] = 0ULL;
    pMsgHead->Ctx.Data[1] = 0ULL;
    pMsgHead->Ctx.Data[2] = 0ULL;

    /* msg header */
    pMsgHead->Ctx.Chunk.Id = (UINT8) AMBA_MON_MSG_ID_DATA;
    pMsgHead->Ctx.Chunk.Size = (UINT8) MsgLength;
    if ((FovId < AMBA_MON_NUM_FOV_CHANNEL) && (TimeoutFlag == 0U)) {
        /* repeat check */
        pMsgHead->Ctx.Chunk.Repeat = (RawSeqNum == AmbaMonDsp_RgbRawSeqNum[FovId]) ? 1U : 0U;
        /* raw cap seq update */
        AmbaMonDsp_RgbRawSeqNum[FovId] = RawSeqNum;
    }
    pMsgHead->Ctx.Chunk.Timeout = TimeoutFlag;
    pMsgHead->Ctx.Chunk.Timetick = Timetick;

    /* msg data copy */
    FuncRetCode = AmbaWrap_memcpy(pMsgData, &pEvent, DataSize);
    if (FuncRetCode != OK_UL) {
        /* */
    }

    /* msg put */
    if (FovId < AMBA_MON_NUM_FOV_CHANNEL) {
        MemInfo.Ctx.pUint8 = RgbRdyBuf;
        pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_DspSrcEn.Ctx.Bits.RgbPut == 1U)) ? AmbaMonMessage_Put(&(DspRgbAaaMsgPort[FovId]), MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size) : NULL;
        if (pMem == NULL) {
            /* */
        }
        /* debug msg */
        if (TimeoutFlag == 0U) {
            /* msg */
            if (AmbaMonMain_Info.Debug.Bits.Dsp > 0U) {
                AmbaMonMain_TimingMarkPutByFov(FovId, "Rgb_Rdy");
                {
                    char str[11];
                    str[0] = ' ';str[1] = ' ';

                    MemInfo.Ctx.pVoid = pMsgData;
                    MemInfo.Ctx.Data = *(MemInfo.Ctx.pAddr);

                    str[0] = 'i';
                    svar_utoa(MemInfo.Ctx.pRgbAaa->Header.ChanIndex, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                    str[0] = 'n';
                    svar_utoa(MemInfo.Ctx.pRgbAaa->Header.RawPicSeqNum, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                    str[0] = ' ';
                    svar_utoa((UINT32) MemInfo.Ctx.Data, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                }
            }
        } else {
            /* timeout */
            if ((AmbaMonMain_Info.Debug.Bits.Dsp > 0U) &&
                (AmbaMonMain_Info.Debug.Bits.Timeout > 0U)) {
                AmbaMonMain_TimingMarkPutByFov(FovId, "Rgb_Rdy!");
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor dsp rgb aaa ready
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonDsp_RgbAaaRdy(const void *pEvent)
{
    UINT32 FuncRetCode;
    AMBA_MON_DSP_MEM_s MemInfo;

    MemInfo.Ctx.pCvoid = pEvent;
    if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << MemInfo.Ctx.pRgbAaa->Header.ChanIndex)) > 0U) {
        FuncRetCode = (AmbaMonMain_DspSrcEn.Ctx.Bits.RgbSrc == 1U) ? AmbaMonDsp_RgbAaaPut(pEvent, 0) : OK_UL;
    } else {
        FuncRetCode = OK_UL;
    }
    return FuncRetCode;
}

/**
 *  @private
 *  Amba monitor dsp rgb aaa timeout
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonDsp_RgbAaaTimeout(const void *pEvent)
{
    UINT32 FuncRetCode;
    FuncRetCode = AmbaMonDsp_RgbAaaPut(pEvent, 1);
    return FuncRetCode;
}

/**
 *  Amba monitor dsp fov latency put
 *  @param[in] pEvent pointer to the event data
 *  @param[in] TimeoutFlag timeout flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonDsp_FovLatencyPut(const void *pEvent, UINT8 TimeoutFlag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 FovLatencyBuf[256];
#else
    UINT8 FovLatencyBuf[32];
#endif
    AMBA_MON_DSP_MEM_s MemInfo;
    const void *pMem;

    UINT32 FovId;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    void *pMsgData;
#else
    const void *pMsgData;
#endif
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(AMBA_MON_DSP_FOV_LATENCY_s);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* fov id get */
    MemInfo.Ctx.pCvoid = pEvent;
    FovId = (UINT32) MemInfo.Ctx.pFovLatency->FovId;

    /* msg head get */
    MemInfo.Ctx.pUint8 = FovLatencyBuf;
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
    if (FovId < AMBA_MON_NUM_FOV_CHANNEL) {
        MemInfo.Ctx.pUint8 = FovLatencyBuf;
#ifndef AMBA_MON_MESSAGE_PUT2
        pMem = AmbaMonMessage_Put(&(DspFovLatencyMsgPort[FovId]), MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size);
#else
        pMem = AmbaMonMessage_Put2(&(DspFovLatencyMsgPort[FovId]), MemInfo.Ctx.pVoid, HeaderSize, pMsgData, DataSize);
#endif
        if (pMem == NULL) {
            /* */
        }
        /* debug msg */
        if (TimeoutFlag == 0U) {
            /* msg */
            if (AmbaMonMain_Info.Debug.Bits.Dsp > 0U) {
                AmbaMonMain_TimingMarkPutByFov(FovId, "FovL");
                {
                    char str[11];
                    str[0] = ' ';str[1] = ' ';

#ifndef AMBA_MON_MESSAGE_PUT2
                    MemInfo.Ctx.pVoid = pMsgData;
#else
                    MemInfo.Ctx.pCvoid = pMsgData;
#endif
                    str[0] = 'i';
                    svar_utoa(MemInfo.Ctx.pFovLatency->FovId, &(str[2]), 10U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                    str[0] = 'l';
                    svar_utoa(MemInfo.Ctx.pFovLatency->Latency, &(str[2]), 10U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByFov(FovId, str);
                }
            }
        } else {
            /* timeout */
            if ((AmbaMonMain_Info.Debug.Bits.Dsp > 0U) &&
                (AmbaMonMain_Info.Debug.Bits.Timeout > 0U)) {
                AmbaMonMain_TimingMarkPutByFov(FovId, "FovL!");
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor dsp fov delay
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonDsp_FovDelay(const void *pEvent)
{
    UINT32 FuncRetCode;
    FuncRetCode = AmbaMonDsp_FovLatencyPut(pEvent, 0);
    return FuncRetCode;
}

/**
 *  @private
 *  Amba monitor dsp fov delay timeout
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonDsp_FovDelayTimeout(const void *pEvent)
{
    UINT32 FuncRetCode;
    FuncRetCode = AmbaMonDsp_FovLatencyPut(pEvent, 1);
    return FuncRetCode;
}

/**
 *  Amba monitor dsp vout latency put
 *  @param[in] pEvent pointer to the event data
 *  @param[in] TimeoutFlag timeout flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonDsp_VoutLatencyPut(const void *pEvent, UINT8 TimeoutFlag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 VoutLatencyBuf[256];
#else
    UINT8 VoutLatencyBuf[32];
#endif
    AMBA_MON_DSP_MEM_s MemInfo;
    const void *pMem;

    UINT32 VoutId;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    void *pMsgData;
#else
    const void *pMsgData;
#endif
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(AMBA_MON_DSP_VOUT_LATENCY_s);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* fov id get */
    MemInfo.Ctx.pCvoid = pEvent;
    VoutId = (UINT32) MemInfo.Ctx.pVoutLatency->VoutId;

    /* msg head get */
    MemInfo.Ctx.pUint8 = VoutLatencyBuf;
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
    if (VoutId < AMBA_MON_NUM_VOUT_CHANNEL) {
        MemInfo.Ctx.pUint8 = VoutLatencyBuf;
#ifndef AMBA_MON_MESSAGE_PUT2
        pMem = AmbaMonMessage_Put(&(DspVoutLatencyMsgPort[VoutId]), MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size);
#else
        pMem = AmbaMonMessage_Put2(&(DspVoutLatencyMsgPort[VoutId]), MemInfo.Ctx.pVoid, HeaderSize, pMsgData, DataSize);
#endif
        if (pMem == NULL) {
            /* */
        }
        /* debug msg */
        if (TimeoutFlag == 0U) {
            /* msg */
            if (AmbaMonMain_Info.Debug.Bits.DspVout > 0U) {
                AmbaMonMain_TimingMarkPutByVout(VoutId, "Vout_FovL");
                {
                    UINT32 i;

                    char str[11];
                    str[0] = ' ';str[1] = ' ';

#ifndef AMBA_MON_MESSAGE_PUT2
                    MemInfo.Ctx.pVoid = pMsgData;
#else
                    MemInfo.Ctx.pCvoid = pMsgData;
#endif
                    str[0] = 'i';
                    svar_utoa(MemInfo.Ctx.pVoutLatency->VoutId, &(str[2]), 10U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                    str[0] = 'p';
                    svar_utoa((UINT32) (MemInfo.Ctx.pVoutLatency->Pts >> 32ULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                    str[0] = ' ';
                    svar_utoa((UINT32) (MemInfo.Ctx.pVoutLatency->Pts & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                    str[0] = 'n';
                    svar_utoa((UINT32) (MemInfo.Ctx.pVoutLatency->DispSeqNum & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                    str[0] = 'd';
                    svar_utoa(MemInfo.Ctx.pVoutLatency->DispLatency, &(str[2]), 10U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                    str[0] = 'f';
                    svar_utoa(MemInfo.Ctx.pVoutLatency->FovSelectBits, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                    str[0] = 'l';
                    for (i = 0U; (i < AMBA_MON_NUM_FOV_CHANNEL) && ((MemInfo.Ctx.pVoutLatency->FovSelectBits >> i) > 0U); i++) {
                        if ((MemInfo.Ctx.pVoutLatency->FovSelectBits & (((UINT32) 1U) << i)) > 0U) {
                            svar_utoa(MemInfo.Ctx.pVoutLatency->Latency[i], &(str[2]), 10U, 8U, (UINT32) SVAR_LEADING_SPACE);
                            AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                        }
                    }
                }
            }
        } else {
            /* timeout */
            if ((AmbaMonMain_Info.Debug.Bits.DspVout > 0U) &&
                (AmbaMonMain_Info.Debug.Bits.Timeout > 0U)) {
                AmbaMonMain_TimingMarkPutByVout(VoutId, "Vout_FovL!");
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor dsp vout delay
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonDsp_VoutDelay(const void *pEvent)
{
    UINT32 FuncRetCode;
    FuncRetCode = AmbaMonDsp_VoutLatencyPut(pEvent, 0);
    return FuncRetCode;
}

/**
 *  @private
 *  Amba monitor dsp vout delay timeout
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonDsp_VoutDelayTimeout(const void *pEvent)
{
    UINT32 FuncRetCode;
    FuncRetCode = AmbaMonDsp_VoutLatencyPut(pEvent, 1);
    return FuncRetCode;
}

/**
 *  Amba monitor dsp vin timeout put
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonDsp_VinTimeoutPut(const void *pEvent)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 VideoPathBuf[256];
#else
    UINT8 VideoPathBuf[32];
#endif
    AMBA_MON_DSP_MEM_s MemInfo;
    const void *pMem;

    UINT32 VinId;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    void *pMsgData;
#else
    const void *pMsgData;
#endif
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(AMBA_DSP_VIDEO_PATH_INFO_s);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* vin id get */
    MemInfo.Ctx.pCvoid = pEvent;
    VinId = (UINT32) MemInfo.Ctx.pVideoPath->Data;

    /* msg head get */
    MemInfo.Ctx.pUint8 = VideoPathBuf;
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
    if (VinId < AMBA_MON_NUM_VIN_CHANNEL) {
        MemInfo.Ctx.pUint8 = VideoPathBuf;
        /* def raw */
#ifndef AMBA_MON_MESSAGE_PUT2
        pMem = AmbaMonMessage_Put(&(DspVinTimeoutMsgPort[VinId]), MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size);
#else
        pMem = AmbaMonMessage_Put2(&(DspVinTimeoutMsgPort[VinId]), MemInfo.Ctx.pVoid, HeaderSize, pMsgData, DataSize);
#endif
        if (pMem == NULL) {
            /* */
        }
        /* debug msg */
        /* msg */
        if (AmbaMonMain_Info.Debug.Bits.Dsp > 0U) {
            /* vin timeout */
            AmbaMonMain_TimingMarkPut(VinId, "Vin_Timeout");
            /* debug info */
            {
                char str[11];
                str[0] = ' ';str[1] = ' ';

#ifndef AMBA_MON_MESSAGE_PUT2
                MemInfo.Ctx.pVoid = pMsgData;
#else
                MemInfo.Ctx.pCvoid = pMsgData;
#endif
                str[0] = 'i';
                svar_utoa(MemInfo.Ctx.pVideoPath->Data, &(str[2]), 10U, 8U, (UINT32) SVAR_LEADING_SPACE);
                AmbaMonMain_TimingMarkPut(VinId, str);
                str[0] = 's';
                svar_utoa((UINT32) MemInfo.Ctx.pVideoPath->Status, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                AmbaMonMain_TimingMarkPut(VinId, str);
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor dsp vin timeout
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonDsp_VinTimeout(const void *pEvent)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_MON_DSP_MEM_s MemInfo;

    MemInfo.Ctx.pCvoid = pEvent;
    if (MemInfo.Ctx.pVideoPath->Status == VID_PATH_STATUS_VIN_TIMEOUT) {
        FuncRetCode = AmbaMonDsp_VinTimeoutPut(pEvent);
        if (FuncRetCode == NG_UL) {
            RetCode = NG_UL;
        }
    }

    return RetCode;
}
