/**
 *  @file AmbaMonVout.c
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
 *  @details Amba Monitor VOUT
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaVIN_Def.h"

#include "AmbaDSP.h"

#include "AmbaMonFramework.h"
#include "AmbaMonDef.h"

#include "AmbaMonMain.h"
#include "AmbaMonMain_Internal.h"
#include "AmbaMonVout.h"

#ifndef AMBA_MON_MESSAGE_PUT2
#include "AmbaWrap.h"
#endif

#define OK_UL  ((UINT32) 0U)
//#define NG_UL  ((UINT32) 1U)

typedef union /*_AMBA_MON_MAIN_VOUT_MEM_u_*/ {
    const void                   *pCvoid;
    void                         *pVoid;
    UINT8                        *pUint8;
    AMBA_MON_MESSAGE_HEADER_s    *pMsgHead;
    AMBA_MON_VOUT_ISR_RDY_s      *pVoutIsrRdy;
} AMBA_MON_MAIN_VOUT_MEM_u;

typedef struct /*_AMBA_MON_MAIN_VOUT_MEM_s_*/ {
    AMBA_MON_MAIN_VOUT_MEM_u    Ctx;
} AMBA_MON_MAIN_VOUT_MEM_s;

/**
 *  Amba momitor vout isr put
 *  @param[in] pEvent pointer to the event data
 *  @param[in] TimeoutFlag timeout flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonVout_IsrPut(const void *pEvent, UINT8 TimeoutFlag)
{
    static const char * const SafetMainVout_Name[2][AMBA_MON_NUM_VOUT_CHANNEL] = {
        {
            "Vout0_Isr",
#ifdef AMBA_DSP_VOUT1_INT
            "Vout1_Isr",
#endif
#ifdef AMBA_DSP_VOUT2_INT
            "Vout2_Isr"
#endif
        },
        {
            "Vout0_Isr!",
#ifdef AMBA_DSP_VOUT1_INT
            "Vout1_Isr!",
#endif
#ifdef AMBA_DSP_VOUT2_INT
            "Vout2_Isr!"
#endif
        }
    };

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 VoutRdyBuf[256];
#else
    UINT8 VoutRdyBuf[32];
#endif
    AMBA_MON_MAIN_VOUT_MEM_s MemInfo;
    const void *pMem;

    UINT32 VoutId;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    void *pMsgData;
#else
    const void *pMsgData;
#endif
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(AMBA_MON_VOUT_ISR_RDY_s);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* vout id get */
    MemInfo.Ctx.pCvoid = pEvent;
    VoutId = MemInfo.Ctx.pVoutIsrRdy->VoutId;

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
        pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_VoutSrcEn.Ctx.Bits.IsrPut == 1U)) ? AmbaMonMessage_Put(&(VoutIsrMsgPort[VoutId]), MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size) : NULL;
#else
        pMem = ((TimeoutFlag > 0U) || (AmbaMonMain_VoutSrcEn.Ctx.Bits.IsrPut == 1U)) ? AmbaMonMessage_Put2(&(VoutIsrMsgPort[VoutId]), MemInfo.Ctx.pVoid, HeaderSize, pMsgData, DataSize) : NULL;
#endif
        if (pMem == NULL) {
            /* */
        }
        /* debug msg */
        if (TimeoutFlag < 2U) {
            /* msg/timeout */
            if (((AmbaMonMain_Info.Debug.Bits.Vout > 0U) && (TimeoutFlag == 0U)) || ((AmbaMonMain_Info.Debug.Bits.Timeout > 0U) && (TimeoutFlag == 1U))) {
                AmbaMonMain_TimingMarkPutByVout(VoutId, SafetMainVout_Name[TimeoutFlag][VoutId]);
                {
                    char str[11];
                    str[0] = ' ';str[1] = ' ';

#ifndef AMBA_MON_MESSAGE_PUT2
                    MemInfo.Ctx.pVoid = pMsgData;
#else
                    MemInfo.Ctx.pCvoid = pMsgData;
#endif
                    svar_utoa((UINT32) (MemInfo.Ctx.pVoutIsrRdy->Dts >> 32ULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                    svar_utoa((UINT32) (MemInfo.Ctx.pVoutIsrRdy->Dts & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                    svar_utoa((UINT32) (MemInfo.Ctx.pVoutIsrRdy->Pts >> 32ULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                    svar_utoa((UINT32) (MemInfo.Ctx.pVoutIsrRdy->Pts & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                    AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                }
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba momitor vout isr ready
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonVout_IsrRdy(const void *pEvent)
{
    UINT32 FuncRetCode;
    FuncRetCode = (AmbaMonMain_VoutSrcEn.Ctx.Bits.IsrSrc == 1U) ? AmbaMonVout_IsrPut(pEvent, 0) : OK_UL;
    return FuncRetCode;
}

/**
 *  @private
 *  Amba momitor vout isr timeout
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonVout_IsrTimeout(const void *pEvent)
{
    UINT32 FuncRetCode;
    FuncRetCode = AmbaMonVout_IsrPut(pEvent, 1);
    return FuncRetCode;
}

