/**
 *  @file AmbaMonVin.c
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
 *  @details Amba Monior VIN
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaVIN_Def.h"

#include "AmbaDSP_Capability.h"
#include "AmbaDSP_EventInfo.h"

#include "AmbaMonFramework.h"
#include "AmbaMonDef.h"

#include "AmbaMonMain.h"
#include "AmbaMonMain_Internal.h"
#include "AmbaMonVin.h"
#include "AmbaMonVin_Platform.h"

#ifndef AMBA_MON_MESSAGE_PUT2
#include "AmbaWrap.h"
#endif

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

typedef union /*_AMBA_MON_MAIN_VIN_MEM_u_*/ {
    const void                    *pCvoid;
    void                          *pVoid;
    UINT8                         *pUint8;
    AMBA_MON_MESSAGE_HEADER_s     *pMsgHead;
    AMBA_MON_VIN_ISR_RDY_s        *pVinIsrRdy;
    AMBA_MON_VIN_ISR_INFO_s       *pVinIsrInfo;
} AMBA_MON_MAIN_VIN_MEM_u;

typedef struct /*_AMBA_MON_MAIN_VIN_MEM_s_*/ {
    AMBA_MON_MAIN_VIN_MEM_u    Ctx;
} AMBA_MON_MAIN_VIN_MEM_s;

/**
 *  Amba momitor vin sof put
 *  @param[in] pEvent pointer to the event data
 *  @param[in] TimeoutFlag timeout flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonVin_SofPut(const void *pEvent, UINT8 TimeoutFlag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 SofRdyBuf[256];
#else
    UINT8 SofRdyBuf[32];
#endif
    AMBA_MON_MAIN_VIN_MEM_s MemInfo;
    const void *pMem;

    UINT32 VinId;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    void *pMsgData;
#else
    const void *pMsgData;
#endif
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(AMBA_MON_VIN_ISR_INFO_s);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* vin id get */
    MemInfo.Ctx.pCvoid = pEvent;
    VinId = MemInfo.Ctx.pVinIsrInfo->VinId;

    /* msg head get */
    MemInfo.Ctx.pUint8 = SofRdyBuf;
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
    if (VinId < AMBA_MON_NUM_VIN_CHANNEL) {
        MemInfo.Ctx.pUint8 = SofRdyBuf;
#ifndef AMBA_MON_MESSAGE_PUT2
        pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_VinSrcEn.Ctx.Bits.SofPut == 1U)) ? AmbaMonMessage_Put(&(VinSofMsgPort[VinId]), MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size) : NULL;
#else
        pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_VinSrcEn.Ctx.Bits.SofPut == 1U)) ? AmbaMonMessage_Put2(&(VinSofMsgPort[VinId]), MemInfo.Ctx.pVoid, HeaderSize, pMsgData, DataSize) : NULL;
#endif
        if (pMem == NULL) {
            RetCode = NG_UL;
        }
        /* debug msg */
        if (TimeoutFlag < 2U) {
            /* msg/timeout */
            if (((AmbaMonMain_Info.Debug.Bits.Vin > 0U) && (TimeoutFlag == 0U)) || ((AmbaMonMain_Info.Debug.Bits.Timeout > 0U) && (TimeoutFlag == 1U))) {
                AmbaMonMain_TimingMarkPut(VinId, AmbaMonVin_SofName[TimeoutFlag][VinId]);
                {
                    char str[11];
                    str[0] = ' ';str[1] = ' ';

#ifndef AMBA_MON_MESSAGE_PUT2
                    MemInfo.Ctx.pVoid = pMsgData;
#else
                    MemInfo.Ctx.pCvoid = pMsgData;
#endif
                    svar_utoa((UINT32) (MemInfo.Ctx.pVinIsrInfo->Dts >> 32ULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPut(VinId, str);
                    svar_utoa((UINT32) (MemInfo.Ctx.pVinIsrInfo->Dts & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPut(VinId, str);
                    svar_utoa((UINT32) (MemInfo.Ctx.pVinIsrInfo->Pts >> 32ULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPut(VinId, str);
                    svar_utoa((UINT32) (MemInfo.Ctx.pVinIsrInfo->Pts & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPut(VinId, str);
                }
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba momitor vin sof ready
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonVin_SofRdy(const void *pEvent)
{
    UINT32 FuncRetCode;
    AMBA_MON_VIN_ISR_INFO_s VinIsrInfo;
    AMBA_MON_MAIN_VIN_MEM_s MemInfo;

    /* vin isr msg */
    MemInfo.Ctx.pCvoid = pEvent;
    VinIsrInfo.VinId = MemInfo.Ctx.pVinIsrRdy->VinId;
    VinIsrInfo.SensorId = AmbaMonMain_Info.Sensor[VinIsrInfo.VinId].SelectBits;
    VinIsrInfo.SerdesId = AmbaMonMain_Info.Serdes[VinIsrInfo.VinId].SelectBits;
    VinIsrInfo.Dts = MemInfo.Ctx.pVinIsrRdy->Dts;
    VinIsrInfo.Pts = MemInfo.Ctx.pVinIsrRdy->Pts;

    /* vin isr msg put */
    MemInfo.Ctx.pVinIsrInfo = &VinIsrInfo;
    FuncRetCode = (AmbaMonMain_VinSrcEn.Ctx.Bits.SofSrc == 1U) ? AmbaMonVin_SofPut(MemInfo.Ctx.pCvoid, 0) : OK_UL;
    if (FuncRetCode != OK_UL) {
        AmbaMonPrint(S_PRINT_FLAG_DBG, "sof rdy put fail...", MemInfo.Ctx.pVinIsrInfo->VinId);
    }

    return FuncRetCode;
}

/**
 *  @private
 *  Amba momitor vin sof timeout
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonVin_SofTimeout(const void *pEvent)
{
    UINT32 FuncRetCode;
    AMBA_MON_VIN_ISR_INFO_s VinIsrInfo;
    AMBA_MON_MAIN_VIN_MEM_s MemInfo;

    /* vin isr timeout msg */
    MemInfo.Ctx.pCvoid = pEvent;
    VinIsrInfo.VinId = MemInfo.Ctx.pVinIsrRdy->VinId;
    VinIsrInfo.SensorId = AmbaMonMain_Info.Sensor[VinIsrInfo.VinId].SelectBits;
    VinIsrInfo.SerdesId = AmbaMonMain_Info.Serdes[VinIsrInfo.VinId].SelectBits;
    VinIsrInfo.Dts = MemInfo.Ctx.pVinIsrRdy->Dts;
    VinIsrInfo.Pts = MemInfo.Ctx.pVinIsrRdy->Pts;

    /* vin isr timeout msg put */
    MemInfo.Ctx.pVinIsrInfo = &VinIsrInfo;
    FuncRetCode = AmbaMonVin_SofPut(MemInfo.Ctx.pCvoid, 1);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrint(S_PRINT_FLAG_DBG, "sof timeout put fail...", MemInfo.Ctx.pVinIsrInfo->VinId);
    }

    return FuncRetCode;
}

/**
 *  Amba momitor vin eof put
 *  @param[in] pEvent pointer to the event data
 *  @param[in] TimeoutFlag timeout flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonVin_EofPut(const void *pEvent, UINT8 TimeoutFlag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 EofRdyBuf[256];
#else
    UINT8 EofRdyBuf[32];
#endif
    AMBA_MON_MAIN_VIN_MEM_s MemInfo;
    const void *pMem;

    UINT32 VinId;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    void *pMsgData;
#else
    const void *pMsgData;
#endif
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(AMBA_MON_VIN_ISR_INFO_s);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* vin id get */
    MemInfo.Ctx.pCvoid = pEvent;
    VinId = MemInfo.Ctx.pVinIsrInfo->VinId;

    /* msg head get */
    MemInfo.Ctx.pUint8 = EofRdyBuf;
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
    if (VinId < AMBA_MON_NUM_VIN_CHANNEL) {
        MemInfo.Ctx.pUint8 = EofRdyBuf;
#ifndef AMBA_MON_MESSAGE_PUT2
        pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_VinSrcEn.Ctx.Bits.EofPut == 1U)) ? AmbaMonMessage_Put(&(VinEofMsgPort[VinId]), MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size) : NULL;
#else
        pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_VinSrcEn.Ctx.Bits.EofPut == 1U)) ? AmbaMonMessage_Put2(&(VinEofMsgPort[VinId]), MemInfo.Ctx.pVoid, HeaderSize, pMsgData, DataSize) : NULL;
#endif
        if (pMem == NULL) {
            RetCode = NG_UL;
        }
        /* debug msg */
        if (TimeoutFlag < 2U) {
            /* msg/timeout */
            if (((AmbaMonMain_Info.Debug.Bits.Vin > 0U) && (TimeoutFlag == 0U)) || ((AmbaMonMain_Info.Debug.Bits.Timeout > 0U) && (TimeoutFlag == 1U))) {
                AmbaMonMain_TimingMarkPut(VinId, AmbaMonVin_EofName[TimeoutFlag][VinId]);
                {
                    char str[11];
                    str[0] = ' ';str[1] = ' ';

#ifndef AMBA_MON_MESSAGE_PUT2
                    MemInfo.Ctx.pVoid = pMsgData;
#else
                    MemInfo.Ctx.pCvoid = pMsgData;
#endif
                    svar_utoa((UINT32) (MemInfo.Ctx.pVinIsrInfo->Dts >> 32ULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPut(VinId, str);
                    svar_utoa((UINT32) (MemInfo.Ctx.pVinIsrInfo->Dts & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPut(VinId, str);
                    svar_utoa((UINT32) (MemInfo.Ctx.pVinIsrInfo->Pts >> 32ULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPut(VinId, str);
                    svar_utoa((UINT32) (MemInfo.Ctx.pVinIsrInfo->Pts & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPut(VinId, str);
                }
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba momitor vin eof ready
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonVin_EofRdy(const void *pEvent)
{
    UINT32 FuncRetCode;
    AMBA_MON_VIN_ISR_INFO_s VinIsrInfo;
    AMBA_MON_MAIN_VIN_MEM_s MemInfo;

    /* vin isr timeout msg */
    MemInfo.Ctx.pCvoid = pEvent;
    VinIsrInfo.VinId = MemInfo.Ctx.pVinIsrRdy->VinId;
    VinIsrInfo.SensorId = AmbaMonMain_Info.Sensor[VinIsrInfo.VinId].SelectBits;
    VinIsrInfo.SerdesId = AmbaMonMain_Info.Serdes[VinIsrInfo.VinId].SelectBits;
    VinIsrInfo.Dts = MemInfo.Ctx.pVinIsrRdy->Dts;
    VinIsrInfo.Pts = MemInfo.Ctx.pVinIsrRdy->Pts;

    /* vin isr timeout msg put */
    MemInfo.Ctx.pVinIsrInfo = &VinIsrInfo;
    FuncRetCode = (AmbaMonMain_VinSrcEn.Ctx.Bits.EofSrc == 1U) ? AmbaMonVin_EofPut(MemInfo.Ctx.pCvoid, 0) : OK_UL;
    if (FuncRetCode != OK_UL) {
        AmbaMonPrint(S_PRINT_FLAG_DBG, "eof rdy put fail...", MemInfo.Ctx.pVinIsrInfo->VinId);
    }

    return FuncRetCode;
}

/**
 *  @private
 *  Amba momitor vin eof timeout
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonVin_EofTimeout(const void *pEvent)
{
    UINT32 FuncRetCode;
    AMBA_MON_VIN_ISR_INFO_s VinIsrInfo;
    AMBA_MON_MAIN_VIN_MEM_s MemInfo;

    /* vin isr timeout msg */
    MemInfo.Ctx.pCvoid = pEvent;
    VinIsrInfo.VinId = MemInfo.Ctx.pVinIsrRdy->VinId;
    VinIsrInfo.SensorId = AmbaMonMain_Info.Sensor[VinIsrInfo.VinId].SelectBits;
    VinIsrInfo.SerdesId = AmbaMonMain_Info.Serdes[VinIsrInfo.VinId].SelectBits;
    VinIsrInfo.Dts = MemInfo.Ctx.pVinIsrRdy->Dts;
    VinIsrInfo.Pts = MemInfo.Ctx.pVinIsrRdy->Pts;

    /* vin isr timeout msg put */
    MemInfo.Ctx.pVinIsrInfo = &VinIsrInfo;
    FuncRetCode = AmbaMonVin_EofPut(MemInfo.Ctx.pCvoid, 1);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrint(S_PRINT_FLAG_DBG, "eof timeout put fail...", MemInfo.Ctx.pVinIsrInfo->VinId);
    }

    return FuncRetCode;
}

