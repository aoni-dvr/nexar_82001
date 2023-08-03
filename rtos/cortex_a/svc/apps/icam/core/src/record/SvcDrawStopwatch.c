/**
*  @file SvcDrawStopwatch.c
*
 * Copyright (c) [2020] Ambarella International LP
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
*  @details Definitions and Constants for record blend
*
*/

#include "AmbaKAL.h"
#include "AmbaDef.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaCache.h"
#include "AmbaUtility.h"
#include "AmbaSYS.h"
/* framework */
#include "SvcWrap.h"
#include "SvcUtil.h"
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcPlat.h"
/* share */
#include "SvcRecMain.h"
#include "SvcBlendFont.h"
#include "SvcRecBlend.h"
#include "SvcDrawStopwatch.h"

#define SVC_LOG_DRAW_STOP_WATCH  "DST"

#define HOUR_DIGIT_NUM   ((UINT32)3U)
#define MIN_DIGIT_NUM    ((UINT32)3U)
#define SEC_DIGIT_NUM    ((UINT32)3U)
#define CLUT_SIZE        ((UINT32)256)
#define CLUT_ENTRY_SIZE  ((UINT32)4U * CLUT_SIZE)  //4 Bytes * 256 colors

#define BLEND_ALIGN_SIZE ((UINT32)64U)
#define HOUR_PITCH       ((((AMBA_BLEND_CHAR_WIDTH * HOUR_DIGIT_NUM) + (BLEND_ALIGN_SIZE - 1U)) / BLEND_ALIGN_SIZE) * BLEND_ALIGN_SIZE)
#define MIN_PITCH        ((((AMBA_BLEND_CHAR_WIDTH * HOUR_DIGIT_NUM) + (BLEND_ALIGN_SIZE - 1U)) / BLEND_ALIGN_SIZE) * BLEND_ALIGN_SIZE)
#define SEC_PITCH        ((((AMBA_BLEND_CHAR_WIDTH * HOUR_DIGIT_NUM) + (BLEND_ALIGN_SIZE - 1U)) / BLEND_ALIGN_SIZE) * BLEND_ALIGN_SIZE)

typedef struct {
    UINT64  Hour;
    UINT64  Min;
    UINT64  Sec;
}STOPWATCH_INFO;

typedef UINT8 HOUR_Y_t[HOUR_PITCH * AMBA_BLEND_CHAR_HEIGHT];
typedef UINT8 HOUR_UV_t[(HOUR_PITCH * AMBA_BLEND_CHAR_HEIGHT) >> 1U];
typedef UINT8 MIN_Y_t[MIN_PITCH * AMBA_BLEND_CHAR_HEIGHT];
typedef UINT8 MIN_UV_t[(MIN_PITCH * AMBA_BLEND_CHAR_HEIGHT) >> 1U];
typedef UINT8 SEC_Y_t[SEC_PITCH * AMBA_BLEND_CHAR_HEIGHT];
typedef UINT8 SEC_UV_t[(SEC_PITCH * AMBA_BLEND_CHAR_HEIGHT) >> 1U];

static HOUR_Y_t   *HourBuf_Y    = NULL;
static HOUR_UV_t  *HourBuf_UV   = NULL;
static HOUR_Y_t   *HourBuf_AY   = NULL;
static HOUR_UV_t  *HourBuf_AUV  = NULL;
static MIN_Y_t    *MinBuf_Y     = NULL;
static MIN_UV_t   *MinBuf_UV    = NULL;
static MIN_Y_t    *MinBuf_AY    = NULL;
static MIN_UV_t   *MinBuf_AUV   = NULL;
static SEC_Y_t    *SecBuf_Y     = NULL;
static SEC_UV_t   *SecBuf_UV    = NULL;
static SEC_Y_t    *SecBuf_AY    = NULL;
static SEC_UV_t   *SecBuf_AUV   = NULL;
static UINT8      *CLUT_AlphaY  = NULL;
static UINT8      *CLUT_AlphaUV = NULL;

static STOPWATCH_INFO  Stopwatch[CONFIG_ICAM_MAX_REC_STRM] GNU_SECTION_NOZEROINIT;


static ULONG  SecCLUT_YAddr[CONFIG_ICAM_MAX_REC_STRM]  GNU_SECTION_NOZEROINIT;
static ULONG  MinCLUT_YAddr[CONFIG_ICAM_MAX_REC_STRM]  GNU_SECTION_NOZEROINIT;
static ULONG  HourCLUT_YAddr[CONFIG_ICAM_MAX_REC_STRM] GNU_SECTION_NOZEROINIT;

static ULONG  CLUT_AlphaYAddr;
static ULONG  CLUT_AlphaUVAddr;
static UINT32 RecBlendBehavior;

static void BufferAlloc(ULONG BufBase, UINT32 BufSize)
{
    UINT32  YSize, Temp;
    ULONG   Addr = BufBase;

    if (HourBuf_Y == NULL) {
        YSize = HOUR_PITCH * AMBA_BLEND_CHAR_HEIGHT;
        /* hour_y */
        AmbaMisra_TypeCast(&HourBuf_Y, &Addr);
        Temp = YSize;
        Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
        Addr += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
        /* hour_uv */
        AmbaMisra_TypeCast(&HourBuf_UV, &Addr);
        Temp = YSize >> 1U;
        Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
        Addr += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
        /* hour_ay */
        AmbaMisra_TypeCast(&HourBuf_AY, &Addr);
        Temp = YSize;
        Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
        Addr += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
        /* hour_auv */
        AmbaMisra_TypeCast(&HourBuf_AUV, &Addr);
        Temp = YSize >> 1U;
        Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
        Addr += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);

        /* min_y */
        AmbaMisra_TypeCast(&MinBuf_Y, &Addr);
        Temp = YSize;
        Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
        Addr += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
        /* min_uv */
        AmbaMisra_TypeCast(&MinBuf_UV, &Addr);
        Temp = YSize >> 1U;
        Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
        Addr += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
        /* min_ay */
        AmbaMisra_TypeCast(&MinBuf_AY, &Addr);
        Temp = YSize;
        Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
        Addr += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
        /* min_auv */
        AmbaMisra_TypeCast(&MinBuf_AUV, &Addr);
        Temp = YSize >> 1U;
        Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
        Addr += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);

        /* sec_y */
        AmbaMisra_TypeCast(&SecBuf_Y, &Addr);
        Temp = YSize;
        Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
        Addr += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
        /* sec_uv */
        AmbaMisra_TypeCast(&SecBuf_UV, &Addr);
        Temp = YSize >> 1U;
        Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
        Addr += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
        /* sec_ay */
        AmbaMisra_TypeCast(&SecBuf_AY, &Addr);
        Temp = YSize;
        Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
        Addr += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
        /* sec_auv */
        AmbaMisra_TypeCast(&SecBuf_AUV, &Addr);
        Temp = YSize >> 1U;
        Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
        Addr += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);

        /* clut_alpha_y, clut_alpha_uv */
        AmbaMisra_TypeCast(&CLUT_AlphaY, &Addr);
        Temp = CLUT_ENTRY_SIZE;
        Addr += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
        AmbaMisra_TypeCast(&CLUT_AlphaUV, &Addr);
        Temp = (CLUT_ENTRY_SIZE >> 1U);
        Addr += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);

        if ((BufBase + BufSize) < Addr) {
            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "!!! buffer is too small", 0U, 0U);
        }
    }
}

/**
* init stopwatch, draw 000:00:00
*/
void SvcDrawStopwatch_Init(void)
{
    UINT8     *pBuf;
    UINT32    Stream, Y, XOffset, Number, Err, i;
    ULONG     Addr;
    HOUR_Y_t  *pHrY = NULL;
    MIN_Y_t   *pMinY = NULL;
    SEC_Y_t   *pSecY = NULL;
    HOUR_UV_t *pHrUV = NULL;
    MIN_UV_t  *pMinUV = NULL;
    SEC_UV_t  *pSecUV = NULL;

    AmbaMisra_TouchUnused(pHrY);
    AmbaMisra_TouchUnused(pMinY);
    AmbaMisra_TouchUnused(pSecY);
    AmbaMisra_TouchUnused(pHrUV);
    AmbaMisra_TouchUnused(pMinUV);
    AmbaMisra_TouchUnused(pSecUV);

    /* draw 000:00:00 */

    /* hour */
    for (Stream = 0U; Stream < (UINT32)CONFIG_ICAM_MAX_REC_STRM; Stream++) {
        pHrY = &(HourBuf_AY[Stream]);
        AmbaMisra_TypeCast(&pBuf, &pHrY);

        for (Number = 0U; Number < HOUR_DIGIT_NUM; Number++) {
            XOffset = Number * AMBA_BLEND_CHAR_WIDTH;
            for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                Err = AmbaWrap_memcpy(&(pBuf[(Y * HOUR_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_Y[0U][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                }
            }
        }


        pHrUV = &(HourBuf_AUV[Stream]);
        AmbaMisra_TypeCast(&pBuf, &pHrUV);

        for (Number = 0U; Number < HOUR_DIGIT_NUM; Number++) {
            XOffset = Number * AMBA_BLEND_CHAR_WIDTH;
            for (Y = 0U; Y < AMBA_BLEND_CHAR_UV_HEIGHT; Y++) {
                Err = AmbaWrap_memcpy(&(pBuf[(Y * HOUR_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_UV[0U][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                }
            }
        }
    }

    /* we only control alpha value to show digit */
    Err = AmbaWrap_memset(HourBuf_Y, (INT32)AMBA_BLEND_FONT_COLOR_Y, sizeof(HOUR_Y_t) * (UINT32)CONFIG_ICAM_MAX_REC_STRM);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memset failed!(%u)", Err, 0U);
    }

    AmbaMisra_TypeCast(&Addr, &HourBuf_Y);
    Err = SvcPlat_CacheClean(Addr, sizeof(HOUR_Y_t) * (UINT32)CONFIG_ICAM_MAX_REC_STRM);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "SvcPlat_CacheClean failed!(%u)", Err, 0U);
    }

    AmbaMisra_TypeCast(&pBuf, &HourBuf_UV);
    for (i = 0U; i < ((UINT32)sizeof(HOUR_UV_t) * (UINT32)CONFIG_ICAM_MAX_REC_STRM); i++) {
        if ((i % 2UL) == 0U) {
            pBuf[i] = AMBA_BLEND_FONT_COLOR_U;
        } else {
            pBuf[i] = AMBA_BLEND_FONT_COLOR_V;
        }
    }

    AmbaMisra_TypeCast(&Addr, &HourBuf_UV);
    Err = SvcPlat_CacheClean(Addr, sizeof(HOUR_UV_t) * (UINT32)CONFIG_ICAM_MAX_REC_STRM);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "SvcPlat_CacheClean failed!(%u)", Err, 0U);
    }

    /* min */
    for (Stream = 0U; Stream < (UINT32)CONFIG_ICAM_MAX_REC_STRM; Stream++) {
        pMinY = &(MinBuf_AY[Stream]);
        AmbaMisra_TypeCast(&pBuf, &pMinY);

        for (Number = 0U; Number < MIN_DIGIT_NUM; Number++) {
            XOffset = Number * AMBA_BLEND_CHAR_WIDTH;
            for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                if (Number == 0U) {
                    // Err = AmbaWrap_memcpy(&(pBuf[(Y * MIN_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_Y[10U][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                    Err = AmbaWrap_memset(&(pBuf[(Y * MIN_PITCH) + XOffset]), 255, AMBA_BLEND_CHAR_WIDTH);
                } else {
                    Err = AmbaWrap_memcpy(&(pBuf[(Y * MIN_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_Y[0U][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                }
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                }
            }
        }

        pMinUV = &(MinBuf_AUV[Stream]);
        AmbaMisra_TypeCast(&pBuf, &pMinUV);

        for (Number = 0U; Number < MIN_DIGIT_NUM; Number++) {
            XOffset = Number * AMBA_BLEND_CHAR_WIDTH;
            for (Y = 0U; Y < AMBA_BLEND_CHAR_UV_HEIGHT; Y++) {
                if (Number == 0U) {
                    // Err = AmbaWrap_memcpy(&(pBuf[(Y * MIN_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_UV[10U][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                    Err = AmbaWrap_memset(&(pBuf[(Y * MIN_PITCH) + XOffset]), 255, AMBA_BLEND_CHAR_WIDTH);
                } else {
                    Err = AmbaWrap_memcpy(&(pBuf[(Y * MIN_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_UV[0U][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                }
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                }
            }
        }
    }

    Err = AmbaWrap_memset(MinBuf_Y, (INT32)AMBA_BLEND_FONT_COLOR_Y, sizeof(MIN_Y_t) * (UINT32)CONFIG_ICAM_MAX_REC_STRM);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memset failed!(%u)", Err, 0U);
    }

    AmbaMisra_TypeCast(&Addr, &MinBuf_Y);
    Err = SvcPlat_CacheClean(Addr, sizeof(MIN_Y_t) * (UINT32)CONFIG_ICAM_MAX_REC_STRM);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "SvcPlat_CacheClean failed!(%u)", Err, 0U);
    }

    AmbaMisra_TypeCast(&pBuf, &MinBuf_UV);
    for (i = 0U; i < ((UINT32)sizeof(MIN_UV_t) * (UINT32)CONFIG_ICAM_MAX_REC_STRM); i++) {
        if ((i % 2UL) == 0U) {
            pBuf[i] = AMBA_BLEND_FONT_COLOR_U;
        } else {
            pBuf[i] = AMBA_BLEND_FONT_COLOR_V;
        }
    }

    AmbaMisra_TypeCast(&Addr, &MinBuf_UV);
    Err = SvcPlat_CacheClean(Addr, sizeof(MIN_UV_t) * (UINT32)CONFIG_ICAM_MAX_REC_STRM);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "SvcPlat_CacheClean failed!(%u)", Err, 0U);
    }

    /* sec */
    for (Stream = 0U; Stream < (UINT32)CONFIG_ICAM_MAX_REC_STRM; Stream++) {
        pSecY = &(SecBuf_AY[Stream]);
        AmbaMisra_TypeCast(&pBuf, &pSecY);

        for (Number = 0U; Number < SEC_DIGIT_NUM; Number++) {
            XOffset = Number * AMBA_BLEND_CHAR_WIDTH;
            for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                if (Number == 0U) {
                    // Err = AmbaWrap_memcpy(&(pBuf[(Y * SEC_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_Y[10U][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                    Err = AmbaWrap_memset(&(pBuf[(Y * SEC_PITCH) + XOffset]), 255, AMBA_BLEND_CHAR_WIDTH);
                } else {
                    Err = AmbaWrap_memcpy(&(pBuf[(Y * SEC_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_Y[0U][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                }
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                }
            }
        }

        pSecUV = &(SecBuf_AUV[Stream]);
        AmbaMisra_TypeCast(&pBuf, &pSecUV);

        for (Number = 0U; Number < SEC_DIGIT_NUM; Number++) {
            XOffset = Number * AMBA_BLEND_CHAR_WIDTH;
            for (Y = 0U; Y < AMBA_BLEND_CHAR_UV_HEIGHT; Y++) {
                if (Number == 0U) {
                    // Err = AmbaWrap_memcpy(&(pBuf[(Y * SEC_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_UV[10U][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                    Err = AmbaWrap_memset(&(pBuf[(Y * SEC_PITCH) + XOffset]), 255, AMBA_BLEND_CHAR_WIDTH);
                } else {
                    Err = AmbaWrap_memcpy(&(pBuf[(Y * SEC_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_UV[0U][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                }
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                }
            }
        }
    }

    Err = AmbaWrap_memset(SecBuf_Y, (INT32)AMBA_BLEND_FONT_COLOR_Y, sizeof(SEC_Y_t) * (UINT32)CONFIG_ICAM_MAX_REC_STRM);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memset failed!(%u)", Err, 0U);
    }

    AmbaMisra_TypeCast(&Addr, &SecBuf_Y);
    Err = SvcPlat_CacheClean(Addr, sizeof(SEC_Y_t) * (UINT32)CONFIG_ICAM_MAX_REC_STRM);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "SvcPlat_CacheClean failed!(%u)", Err, 0U);
    }

    AmbaMisra_TypeCast(&pBuf, &SecBuf_UV);
    for (i = 0U; i < ((UINT32)sizeof(SEC_UV_t) * (UINT32)CONFIG_ICAM_MAX_REC_STRM); i++) {
        if ((i % 2UL) == 0U) {
            pBuf[i] = AMBA_BLEND_FONT_COLOR_U;
        } else {
            pBuf[i] = AMBA_BLEND_FONT_COLOR_V;
        }
    }

    AmbaMisra_TypeCast(&Addr, &SecBuf_UV);
    Err = SvcPlat_CacheClean(Addr, sizeof(SEC_UV_t) * (UINT32)CONFIG_ICAM_MAX_REC_STRM);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "SvcPlat_CacheClean failed!(%u)", Err, 0U);
    }

    /* reset time  */
    Err = AmbaWrap_memset(&(Stopwatch[0U]), 0xFF, sizeof(Stopwatch));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memset failed!(%u)", Err, 0U);
    }

    /* CLUT init */
    if (RecBlendBehavior == 1U) { //CLUT
        UINT8  *pU8Ptr = NULL;
        UINT32 *pU32Val;
        void   *pVoid;
        ULONG  BaseAddr;

        AmbaMisra_TouchUnused(pU8Ptr);

        pU8Ptr = CLUT_AlphaUV;
        AmbaMisra_TypeCast(&CLUT_AlphaUVAddr, &pU8Ptr);
        pU8Ptr = CLUT_AlphaY;
        AmbaMisra_TypeCast(&CLUT_AlphaYAddr, &pU8Ptr);
        AmbaMisra_TypeCast(&pU32Val, &CLUT_AlphaYAddr);
        Err = AmbaWrap_memset(pU32Val, 0, CLUT_ENTRY_SIZE);// Init to transparent
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memset failed!(%u)", Err, 0U);
        }
        pU32Val[0U]   = 0xFFFF8080U;  // white
        pU32Val[1U]   = 0xFFFF8080U;  // white
        pU32Val[2U]   = 0xFF008080U;  // black
        pU32Val[3U]   = 0xFF4C54FFU;  // red
        pU32Val[4U]   = 0xFF952B15U;  // green
        pU32Val[5U]   = 0xFF1DFF6BU;  // blue
        pU32Val[255U] = 0x00000000U;  // transparent

        /* Init sec, min and hour block to '0' or ':' character */
        for (Stream = 0U; Stream < (UINT32)CONFIG_ICAM_MAX_REC_STRM; Stream++) {
            /* Sec */
            pSecY = &(SecBuf_Y[Stream]);
            AmbaMisra_TypeCast(&pU8Ptr, &pSecY);

            AmbaMisra_TypeCast(&SecCLUT_YAddr[Stream], &pU8Ptr);
            for (Number = 0U; Number < SEC_DIGIT_NUM; Number++) {
                XOffset = Number * AMBA_BLEND_CHAR_WIDTH;
                AmbaMisra_TypeCast(&BaseAddr, &SecCLUT_YAddr[Stream]);
                for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                    BaseAddr += XOffset;
                    AmbaMisra_TypeCast(&pVoid, &BaseAddr);
                    if (Number == 0U) { // ':' character
                        Err = AmbaWrap_memcpy(pVoid, &(AmbaBlendChar_Alpha_Y[10U][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                    } else { // '0' character
                        Err = AmbaWrap_memcpy(pVoid, &(AmbaBlendChar_Alpha_Y[0U][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                    }
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "[SEC] AmbaWrap_memcpy failed!(%u)", Err, 0U);
                    }
                    BaseAddr += ((ULONG)SEC_DIGIT_NUM - (ULONG)Number) * (ULONG)(AMBA_BLEND_CHAR_WIDTH);
                }
            }

            /* Min */
            pMinY = &(MinBuf_Y[Stream]);
            AmbaMisra_TypeCast(&pU8Ptr, &pMinY);

            AmbaMisra_TypeCast(&MinCLUT_YAddr[Stream], &pU8Ptr);
            for (Number = 0U; Number < MIN_DIGIT_NUM; Number++) {
                XOffset = Number * AMBA_BLEND_CHAR_WIDTH;
                AmbaMisra_TypeCast(&BaseAddr, &MinCLUT_YAddr[Stream]);
                for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                    BaseAddr += XOffset;
                    AmbaMisra_TypeCast(&pVoid, &BaseAddr);
                    if (Number == 0U) { // ':' character
                        Err = AmbaWrap_memcpy(pVoid, &(AmbaBlendChar_Alpha_Y[10U][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                    } else {// '0' character
                        Err = AmbaWrap_memcpy(pVoid, &(AmbaBlendChar_Alpha_Y[0U][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                    }
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "[Min] AmbaWrap_memcpy failed!(%u)", Err, 0U);
                    }
                    BaseAddr += ((ULONG)MIN_DIGIT_NUM - (ULONG)Number) * (ULONG)(AMBA_BLEND_CHAR_WIDTH);
                }
            }

            /* HOUR */
            pHrY = &(HourBuf_Y[Stream]);
            AmbaMisra_TypeCast(&pU8Ptr, &pHrY);

            AmbaMisra_TypeCast(&HourCLUT_YAddr[Stream], &pU8Ptr);
            for (Number = 0U; Number < HOUR_DIGIT_NUM; Number++) {
                XOffset = Number * AMBA_BLEND_CHAR_WIDTH;
                AmbaMisra_TypeCast(&BaseAddr, &HourCLUT_YAddr[Stream]);
                for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                    BaseAddr += XOffset;
                    AmbaMisra_TypeCast(&pVoid, &BaseAddr);
                    /* '0' character */
                    Err = AmbaWrap_memcpy(pVoid, &(AmbaBlendChar_Alpha_Y[0U][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "[Hour 0U] AmbaWrap_memcpy failed!(%u)", Err, 0U);
                    }
                    BaseAddr += ((ULONG)HOUR_DIGIT_NUM - (ULONG)Number) * (ULONG)(AMBA_BLEND_CHAR_WIDTH);
                }
            }
        }
    } else {
        //Do nothing
    }
}

/**
* get DrawStopwatch buf info
* @param [in] StreamId stream index
* @param [in] pInfo Alpha blending = 0U; CLUT = 1U
* @param [out]RecBlend  pointer to get the buffer info
*/
void SvcDrawStopwatch_InfoGet(UINT32 StreamId, SVC_DRAW_STOPWATCH_INFO *pInfo, const UINT32 RecBlend)
{
    static ULONG SecCLUT_UVAddr[CONFIG_ICAM_MAX_REC_STRM];
    static ULONG MinCLUT_UVAddr[CONFIG_ICAM_MAX_REC_STRM];
    static ULONG HourCLUT_UVAddr[CONFIG_ICAM_MAX_REC_STRM];

    HOUR_Y_t   *pHrY = NULL;
    HOUR_UV_t  *pHrUV = NULL;
    MIN_Y_t    *pMinY = NULL;
    MIN_UV_t   *pMinUV = NULL;
    SEC_Y_t    *pSecY = NULL;
    SEC_UV_t   *pSecUV = NULL;

    AmbaMisra_TouchUnused(pHrY);
    AmbaMisra_TouchUnused(pMinY);
    AmbaMisra_TouchUnused(pSecY);
    AmbaMisra_TouchUnused(pHrUV);
    AmbaMisra_TouchUnused(pMinUV);
    AmbaMisra_TouchUnused(pSecUV);

    BufferAlloc(pInfo->BufBase, pInfo->BufSize);

    /* RecBlend: Alpha blending = 0U; CLUT = 1U */
    RecBlendBehavior = RecBlend;
    if (0U == RecBlendBehavior) { //Alpha blending
        /* hour */
        pInfo->Hour.Width         = (UINT16)(AMBA_BLEND_CHAR_WIDTH * HOUR_DIGIT_NUM);
        pInfo->Hour.Height        = (UINT16)AMBA_BLEND_CHAR_HEIGHT;
        pInfo->Hour.Pitch         = (UINT16)(HOUR_PITCH);
        pHrY = &(HourBuf_Y[StreamId]);
        AmbaMisra_TypeCast(&(pInfo->Hour.BaseAddrY), &pHrY);
        pHrUV = &(HourBuf_UV[StreamId]);
        AmbaMisra_TypeCast(&(pInfo->Hour.BaseAddrUV), &pHrUV);
        pHrY = &(HourBuf_AY[StreamId]);
        AmbaMisra_TypeCast(&(pInfo->Hour.BaseAddrAY), &pHrY);
        pHrUV = &(HourBuf_AUV[StreamId]);
        AmbaMisra_TypeCast(&(pInfo->Hour.BaseAddrAUV), &pHrUV);
        /* min */
        pInfo->Min.Width         = (UINT16)(AMBA_BLEND_CHAR_WIDTH * MIN_DIGIT_NUM);
        pInfo->Min.Height        = (UINT16)AMBA_BLEND_CHAR_HEIGHT;
        pInfo->Min.Pitch         = (UINT16)(MIN_PITCH);
        pMinY = &(MinBuf_Y[StreamId]);
        AmbaMisra_TypeCast(&(pInfo->Min.BaseAddrY), &pMinY);
        pMinUV = &(MinBuf_UV[StreamId]);
        AmbaMisra_TypeCast(&(pInfo->Min.BaseAddrUV), &pMinUV);
        pMinY = &(MinBuf_AY[StreamId]);
        AmbaMisra_TypeCast(&(pInfo->Min.BaseAddrAY), &pMinY);
        pMinUV = &(MinBuf_AUV[StreamId]);
        AmbaMisra_TypeCast(&(pInfo->Min.BaseAddrAUV), &pMinUV);
        /* sec */
        pInfo->Sec.Width         = (UINT16)(AMBA_BLEND_CHAR_WIDTH * SEC_DIGIT_NUM);
        pInfo->Sec.Height        = (UINT16)AMBA_BLEND_CHAR_HEIGHT;
        pInfo->Sec.Pitch         = (UINT16)(SEC_PITCH);
        pSecY = &(SecBuf_Y[StreamId]);
        AmbaMisra_TypeCast(&(pInfo->Sec.BaseAddrY), &pSecY);
        pSecUV = &(SecBuf_UV[StreamId]);
        AmbaMisra_TypeCast(&(pInfo->Sec.BaseAddrUV), &pSecUV);
        pSecY = &(SecBuf_AY[StreamId]);
        AmbaMisra_TypeCast(&(pInfo->Sec.BaseAddrAY), &pSecY);
        pSecUV = &(SecBuf_AUV[StreamId]);
        AmbaMisra_TypeCast(&(pInfo->Sec.BaseAddrAUV), &pSecUV);

    } else if (1U == RecBlendBehavior) { //CLUT
        /* sec */
        pSecY = &(SecBuf_Y[StreamId]);
        AmbaMisra_TypeCast(&SecCLUT_YAddr[StreamId], &pSecY);
        pSecUV = &(SecBuf_UV[StreamId]);
        AmbaMisra_TypeCast(&SecCLUT_UVAddr[StreamId], &pSecUV);
        pInfo->Sec.Width         = (UINT16)(AMBA_BLEND_CHAR_WIDTH * SEC_DIGIT_NUM);
        pInfo->Sec.Height        = (UINT16)(AMBA_BLEND_CHAR_HEIGHT);
        pInfo->Sec.Pitch         = (UINT16)(AMBA_BLEND_CHAR_WIDTH * SEC_DIGIT_NUM);
        AmbaMisra_TypeCast(&(pInfo->Sec.BaseAddrY), &SecCLUT_YAddr[StreamId]);
        AmbaMisra_TypeCast(&(pInfo->Sec.BaseAddrUV), &SecCLUT_UVAddr[StreamId]);
        AmbaMisra_TypeCast(&(pInfo->Sec.BaseAddrAY), &CLUT_AlphaYAddr);
        AmbaMisra_TypeCast(&(pInfo->Sec.BaseAddrAUV), &CLUT_AlphaUVAddr);

        /* Min */
        pMinY = &(MinBuf_Y[StreamId]);
        AmbaMisra_TypeCast(&MinCLUT_YAddr[StreamId], &pMinY);
        pMinUV = &(MinBuf_UV[StreamId]);
        AmbaMisra_TypeCast(&MinCLUT_UVAddr[StreamId], &pMinUV);
        pInfo->Min.Width         = (UINT16)(AMBA_BLEND_CHAR_WIDTH * MIN_DIGIT_NUM);
        pInfo->Min.Height        = (UINT16)(AMBA_BLEND_CHAR_HEIGHT);
        pInfo->Min.Pitch         = (UINT16)(AMBA_BLEND_CHAR_WIDTH * MIN_DIGIT_NUM);
        AmbaMisra_TypeCast(&(pInfo->Min.BaseAddrY), &MinCLUT_YAddr[StreamId]);
        AmbaMisra_TypeCast(&(pInfo->Min.BaseAddrUV), &MinCLUT_UVAddr[StreamId]);
        AmbaMisra_TypeCast(&(pInfo->Min.BaseAddrAY), &CLUT_AlphaYAddr);
        AmbaMisra_TypeCast(&(pInfo->Min.BaseAddrAUV), &CLUT_AlphaUVAddr);

        /* Hour */
        pHrY = &(HourBuf_Y[StreamId]);
        AmbaMisra_TypeCast(&HourCLUT_YAddr[StreamId], &pHrY);
        pHrUV = &(HourBuf_UV[StreamId]);
        AmbaMisra_TypeCast(&HourCLUT_UVAddr[StreamId], &pHrUV);
        pInfo->Hour.Width         = (UINT16)(AMBA_BLEND_CHAR_WIDTH * HOUR_DIGIT_NUM);
        pInfo->Hour.Height        = (UINT16)(AMBA_BLEND_CHAR_HEIGHT);
        pInfo->Hour.Pitch         = (UINT16)(AMBA_BLEND_CHAR_WIDTH * HOUR_DIGIT_NUM);
        AmbaMisra_TypeCast(&(pInfo->Hour.BaseAddrY), &HourCLUT_YAddr[StreamId]);
        AmbaMisra_TypeCast(&(pInfo->Hour.BaseAddrUV), &HourCLUT_UVAddr[StreamId]);
        AmbaMisra_TypeCast(&(pInfo->Hour.BaseAddrAY), &CLUT_AlphaYAddr);
        AmbaMisra_TypeCast(&(pInfo->Hour.BaseAddrAUV), &CLUT_AlphaUVAddr);
    } else {
        //Do nothing
    }
}

/**
* callback function for rec blend task, to update stopwatch time
* @param [in] StreamId stream index
* @param [in] BlendAreaIdx blend area index
* @param [out]pAction set SVC_REC_BLEND_ACTION_UPDATE_BUF, if it needs to update buffer
*/
void SvcDrawStopwatch_BufUpdate(UINT16 StreamId, UINT16 BlendAreaIdx, UINT32 *pAction)
{
    ULONG              Addr;
    UINT8              *pBuf;
    UINT32             XOffset, Y, Number, Err, ClkFreq = 0U;
    UINT64             HwTimeScale, Time, Sec, Min, Hour;
    STOPWATCH_INFO     *pStopwatch = &(Stopwatch[StreamId]);
    AMBA_RSC_STATIS_s  Statis = {0U};
    HOUR_Y_t           *pHrAY = NULL;
    MIN_Y_t            *pMinAY = NULL;
    SEC_Y_t            *pSecAY = NULL;
    HOUR_UV_t          *pHrAUV = NULL;
    MIN_UV_t           *pMinAUV = NULL;
    SEC_UV_t           *pSecAUV = NULL;

    AmbaMisra_TouchUnused(pHrAY);
    AmbaMisra_TouchUnused(pMinAY);
    AmbaMisra_TouchUnused(pSecAY);
    AmbaMisra_TouchUnused(pHrAUV);
    AmbaMisra_TouchUnused(pMinAUV);
    AmbaMisra_TouchUnused(pSecAUV);

    Err = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_AUD_0, &ClkFreq);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaSYS_GetClkFreq failed!(%u)", Err, 0U);
    }
    if (ClkFreq == 0U) {
        ClkFreq = 1U;
    }
    HwTimeScale = ClkFreq;
    /* RecBlendBehavior: Alpha blending = 0U; CLUT = 1U */
    if (0U == RecBlendBehavior) { //Alpha blending
        SvcRecMain_Control(SVC_RCM_GET_VID_STATIS, 1U, &StreamId, &Statis);

        Time = Statis.RxLtHwTime / HwTimeScale;

        Sec  = Time % 60U;
        Min  = ((Time - Sec) / 60U) % 60U;
        Hour = (((Time - Sec) - (Min * 60U)) / 3600U);

        *pAction = 0U;

        /* sec */
        switch (BlendAreaIdx) {
        case SVC_DRAW_STOPWATCH_SEC_IDX:
            if (Sec != pStopwatch->Sec) {
                UINT64 Digit0, Digit1;

                Digit1 = Sec % 10U;
                Digit0 = (Sec - Digit1) / 10U;

                pSecAY  = &(SecBuf_AY[StreamId]);
                pSecAUV = &(SecBuf_AUV[StreamId]);

                if ((Digit1) != (pStopwatch->Sec % 10U)) {
                    Number = 2U;

                    AmbaMisra_TypeCast(&pBuf, &pSecAY);
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                        Err = AmbaWrap_memcpy(&(pBuf[(Y * SEC_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_Y[Digit1][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                    }

                    AmbaMisra_TypeCast(&pBuf, &pSecAUV);
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_UV_HEIGHT; Y++) {
                        Err = AmbaWrap_memcpy(&(pBuf[(Y * SEC_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_UV[Digit1][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                    }
                    *pAction = SVC_REC_BLEND_ACTION_UPDATE_BUF;
                }

                if ((Digit0) != (pStopwatch->Sec / 10U)) {
                    Number = 1U;

                    AmbaMisra_TypeCast(&pBuf, &pSecAY);
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                        Err = AmbaWrap_memcpy(&(pBuf[(Y * SEC_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_Y[Digit0][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                    }

                    AmbaMisra_TypeCast(&pBuf, &pSecAUV);
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_UV_HEIGHT; Y++) {
                        Err = AmbaWrap_memcpy(&(pBuf[(Y * SEC_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_UV[Digit0][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                    }
                    *pAction = SVC_REC_BLEND_ACTION_UPDATE_BUF;
                }

                if (*pAction == SVC_REC_BLEND_ACTION_UPDATE_BUF) {
                    AmbaMisra_TypeCast(&Addr, &pSecAY);
                    Err = SvcPlat_CacheClean(Addr, AMBA_BLEND_CHAR_HEIGHT * SEC_PITCH);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "SvcPlat_CacheClean failed!(%u)", Err, 0U);
                    }

                    AmbaMisra_TypeCast(&Addr, &pSecAUV);
                    Err = SvcPlat_CacheClean(Addr, AMBA_BLEND_CHAR_UV_HEIGHT * SEC_PITCH);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "SvcPlat_CacheClean failed!(%u)", Err, 0U);
                    }
                }

                pStopwatch->Sec = Sec;
            }
            break;
        case SVC_DRAW_STOPWATCH_MIN_IDX:
            if (Min != pStopwatch->Min) {
                UINT64 Digit0, Digit1;

                Digit1 = Min % 10U;
                Digit0 = (Min - Digit1) / 10U;

                pMinAY  = &(MinBuf_AY[StreamId]);
                pMinAUV = &(MinBuf_AUV[StreamId]);

                if ((Digit1) != (pStopwatch->Min % 10U)) {
                    Number = 2U;

                    AmbaMisra_TypeCast(&pBuf, &pMinAY);
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                        Err = AmbaWrap_memcpy(&(pBuf[(Y * MIN_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_Y[Digit1][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                    }

                    AmbaMisra_TypeCast(&pBuf, &pMinAUV);
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_UV_HEIGHT; Y++) {
                        Err = AmbaWrap_memcpy(&(pBuf[(Y * MIN_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_UV[Digit1][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                    }
                    *pAction = SVC_REC_BLEND_ACTION_UPDATE_BUF;
                }

                if ((Digit0) != (pStopwatch->Min / 10U)) {
                    Number = 1U;

                    AmbaMisra_TypeCast(&pBuf, &pMinAY);
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                        Err = AmbaWrap_memcpy(&(pBuf[(Y * MIN_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_Y[Digit0][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                    }

                    AmbaMisra_TypeCast(&pBuf, &pMinAUV);
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_UV_HEIGHT; Y++) {
                        Err = AmbaWrap_memcpy(&(pBuf[(Y * MIN_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_UV[Digit0][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                    }
                    *pAction = SVC_REC_BLEND_ACTION_UPDATE_BUF;
                }

                if (*pAction == SVC_REC_BLEND_ACTION_UPDATE_BUF) {
                    AmbaMisra_TypeCast(&Addr, &pMinAY);
                    Err = SvcPlat_CacheClean(Addr, AMBA_BLEND_CHAR_HEIGHT * MIN_PITCH);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "SvcPlat_CacheClean failed!(%u)", Err, 0U);
                    }

                    AmbaMisra_TypeCast(&Addr, &pMinAUV);
                    Err = SvcPlat_CacheClean(Addr, AMBA_BLEND_CHAR_UV_HEIGHT * MIN_PITCH);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "SvcPlat_CacheClean failed!(%u)", Err, 0U);
                    }
                }

                pStopwatch->Min = Min;
            }
            break;
        case SVC_DRAW_STOPWATCH_HOUR_IDX:
            if (Hour != pStopwatch->Hour) {
                UINT64 Digit0, Digit1, Digit2;

                Digit2 = Hour % 10U;
                Digit1 = ((Hour - Digit2) / 10U) % 10U;
                Digit0 = (((Hour - Digit2) - (Digit1 * 10U)) / 100U) % 10U;

                pHrAY  = &(HourBuf_AY[StreamId]);
                pHrAUV = &(HourBuf_AUV[StreamId]);

                if ((Digit2) != (pStopwatch->Hour % 10U)) {
                    Number = 2U;
                    AmbaMisra_TypeCast(&pBuf, &pHrAY);
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                        Err = AmbaWrap_memcpy(&(pBuf[(Y * HOUR_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_Y[Digit2][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                    }

                    AmbaMisra_TypeCast(&pBuf, &pHrAUV);
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_UV_HEIGHT; Y++) {
                        Err = AmbaWrap_memcpy(&(pBuf[(Y * HOUR_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_UV[Digit2][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                    }
                    *pAction = SVC_REC_BLEND_ACTION_UPDATE_BUF;
                }

                if ((Digit1) != ((pStopwatch->Hour / 10U) % 10U)) {
                    Number = 1U;

                    AmbaMisra_TypeCast(&pBuf, &pHrAY);
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                        Err = AmbaWrap_memcpy(&(pBuf[(Y * HOUR_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_Y[Digit1][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                    }

                    AmbaMisra_TypeCast(&pBuf, &pHrAUV);
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_UV_HEIGHT; Y++) {
                        Err = AmbaWrap_memcpy(&(pBuf[(Y * HOUR_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_UV[Digit1][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                    }
                    *pAction = SVC_REC_BLEND_ACTION_UPDATE_BUF;
                }

                if ((Digit0) != ((pStopwatch->Hour / 100U) % 10U)) {
                    Number = 0U;

                    AmbaMisra_TypeCast(&pBuf, &pHrAY);
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                        Err = AmbaWrap_memcpy(&(pBuf[(Y * HOUR_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_Y[Digit0][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                    }

                    AmbaMisra_TypeCast(&pBuf, &pHrAUV);
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_UV_HEIGHT; Y++) {
                        Err = AmbaWrap_memcpy(&(pBuf[(Y * HOUR_PITCH) + XOffset]), &(AmbaBlendChar_Alpha_UV[Digit0][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                    }
                    *pAction = SVC_REC_BLEND_ACTION_UPDATE_BUF;
                }

                if (*pAction == SVC_REC_BLEND_ACTION_UPDATE_BUF) {
                    AmbaMisra_TypeCast(&Addr, &pHrAY);
                    Err = SvcPlat_CacheClean(Addr, AMBA_BLEND_CHAR_HEIGHT * HOUR_PITCH);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "SvcPlat_CacheClean failed!(%u)", Err, 0U);
                    }

                    AmbaMisra_TypeCast(&Addr, &pHrAUV);
                    Err = SvcPlat_CacheClean(Addr, AMBA_BLEND_CHAR_UV_HEIGHT * HOUR_PITCH);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "SvcPlat_CacheClean failed!(%u)", Err, 0U);
                    }
                }

                pStopwatch->Hour = Hour;
            }
            break;
        default:
            /* do nothing */
            break;
        }
    } else if (1U == RecBlendBehavior) { //CLUT
        void   *pVoid;
        ULONG  BaseAddr;

        SvcRecMain_Control(SVC_RCM_GET_VID_STATIS, 1U, &StreamId, &Statis);

        Time = Statis.RxLtHwTime / HwTimeScale;

        Sec  = Time % 60U;
        Min  = ((Time - Sec) / 60U) % 60U;
        Hour = (((Time - Sec) - (Min * 60U)) / 3600U);

        *pAction = 0U;

        /* sec */
        switch (BlendAreaIdx) {
        case SVC_DRAW_STOPWATCH_SEC_IDX:
            if (Sec != pStopwatch->Sec) {
                UINT64 Digit0, Digit1;

                Digit1 = Sec % 10U;
                Digit0 = (Sec - Digit1) / 10U;

                if ((Digit1) != (pStopwatch->Sec % 10U)) {
                    Number = 2U;
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    AmbaMisra_TypeCast(&BaseAddr, &SecCLUT_YAddr[StreamId]);
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                        BaseAddr += XOffset;
                        AmbaMisra_TypeCast(&pVoid, &BaseAddr);
                        Err = AmbaWrap_memcpy(pVoid, &(AmbaBlendChar_Alpha_Y[Digit1][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "[SEC Digit1] AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                        BaseAddr += ((ULONG)SEC_DIGIT_NUM - (ULONG)Number) * (ULONG)(AMBA_BLEND_CHAR_WIDTH);
                    }
                    *pAction = SVC_REC_BLEND_ACTION_UPDATE_BUF;
                }

                if ((Digit0) != (pStopwatch->Sec / 10U)) {
                    Number = 1U;
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    AmbaMisra_TypeCast(&BaseAddr, &SecCLUT_YAddr[StreamId]);
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                        BaseAddr += XOffset;
                        AmbaMisra_TypeCast(&pVoid, &BaseAddr);
                        Err = AmbaWrap_memcpy(pVoid, &(AmbaBlendChar_Alpha_Y[Digit0][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "[SEC Digit0] AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                        BaseAddr += ((ULONG)SEC_DIGIT_NUM - (ULONG)Number) * (ULONG)(AMBA_BLEND_CHAR_WIDTH);
                    }
                    *pAction = SVC_REC_BLEND_ACTION_UPDATE_BUF;
                }

                if (*pAction == SVC_REC_BLEND_ACTION_UPDATE_BUF) {
                    Err = SvcPlat_CacheClean(SecCLUT_YAddr[StreamId], AMBA_BLEND_CHAR_WIDTH * SEC_DIGIT_NUM * AMBA_BLEND_CHAR_HEIGHT);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "SvcPlat_CacheClean failed!(%u)", Err, 0U);
                    }
                }

                pStopwatch->Sec = Sec;
            }
            break;
        case SVC_DRAW_STOPWATCH_MIN_IDX:
            if (Min != pStopwatch->Min) {
                UINT64 Digit0, Digit1;

                Digit1 = Min % 10U;
                Digit0 = (Min - Digit1) / 10U;

                if ((Digit1) != (pStopwatch->Min % 10U)) {
                    Number = 2U;
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    AmbaMisra_TypeCast(&BaseAddr, &MinCLUT_YAddr[StreamId]);
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                        BaseAddr += XOffset;
                        AmbaMisra_TypeCast(&pVoid, &BaseAddr);
                        Err = AmbaWrap_memcpy(pVoid, &(AmbaBlendChar_Alpha_Y[Digit1][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "[Min Digit1] AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                        BaseAddr += ((ULONG)MIN_DIGIT_NUM - (ULONG)Number) * (ULONG)(AMBA_BLEND_CHAR_WIDTH);
                    }
                    *pAction = SVC_REC_BLEND_ACTION_UPDATE_BUF;
                }

                if ((Digit0) != (pStopwatch->Min / 10U)) {
                    Number = 1U;
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    AmbaMisra_TypeCast(&BaseAddr, &MinCLUT_YAddr[StreamId]);
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                        BaseAddr += XOffset;
                        AmbaMisra_TypeCast(&pVoid, &BaseAddr);
                        Err = AmbaWrap_memcpy(pVoid, &(AmbaBlendChar_Alpha_Y[Digit0][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "[Min Digit0] AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                        BaseAddr += ((ULONG)MIN_DIGIT_NUM - (ULONG)Number) * (ULONG)(AMBA_BLEND_CHAR_WIDTH);
                    }
                    *pAction = SVC_REC_BLEND_ACTION_UPDATE_BUF;
                }

                if (*pAction == SVC_REC_BLEND_ACTION_UPDATE_BUF) {
                    Err = SvcPlat_CacheClean(MinCLUT_YAddr[StreamId], AMBA_BLEND_CHAR_WIDTH * SEC_DIGIT_NUM * AMBA_BLEND_CHAR_HEIGHT);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "SvcPlat_CacheClean failed!(%u)", Err, 0U);
                    }
                }

                pStopwatch->Min = Min;
            }
            break;
        case SVC_DRAW_STOPWATCH_HOUR_IDX:
            if (Hour != pStopwatch->Hour) {
                UINT64 Digit0, Digit1, Digit2;

                Digit2 = Hour % 10U;
                Digit1 = ((Hour - Digit2) / 10U) % 10U;
                Digit0 = (((Hour - Digit2) - (Digit1 * 10U)) / 100U) % 10U;

                if ((Digit2) != (pStopwatch->Hour % 10U)) {
                    Number = 2U;
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    AmbaMisra_TypeCast(&BaseAddr, &HourCLUT_YAddr[StreamId]);
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                        BaseAddr += XOffset;
                        AmbaMisra_TypeCast(&pVoid, &BaseAddr);
                        Err = AmbaWrap_memcpy(pVoid, &(AmbaBlendChar_Alpha_Y[Digit2][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "[Hour Digit2] AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                        BaseAddr += ((ULONG)HOUR_DIGIT_NUM - (ULONG)Number) * (ULONG)(AMBA_BLEND_CHAR_WIDTH);
                    }
                    *pAction = SVC_REC_BLEND_ACTION_UPDATE_BUF;
                }

                if ((Digit1) != ((pStopwatch->Hour / 10U) % 10U)) {
                    Number = 1U;
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    AmbaMisra_TypeCast(&BaseAddr, &HourCLUT_YAddr[StreamId]);
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                        BaseAddr += XOffset;
                        AmbaMisra_TypeCast(&pVoid, &BaseAddr);
                        Err = AmbaWrap_memcpy(pVoid, &(AmbaBlendChar_Alpha_Y[Digit1][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "[Hour Digit1] AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                        BaseAddr += ((ULONG)HOUR_DIGIT_NUM - (ULONG)Number) * (ULONG)AMBA_BLEND_CHAR_WIDTH;
                    }
                    *pAction = SVC_REC_BLEND_ACTION_UPDATE_BUF;
                }

                if ((Digit0) != ((pStopwatch->Hour / 100U) % 10U)) {
                    Number = 0U;
                    XOffset = AMBA_BLEND_CHAR_WIDTH * Number;
                    AmbaMisra_TypeCast(&BaseAddr, &HourCLUT_YAddr[StreamId]);
                    for (Y = 0U; Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                        BaseAddr += XOffset;
                        AmbaMisra_TypeCast(&pVoid, &BaseAddr);
                        Err = AmbaWrap_memcpy(pVoid, &(AmbaBlendChar_Alpha_Y[Digit0][Y * AMBA_BLEND_CHAR_WIDTH]), AMBA_BLEND_CHAR_WIDTH);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "[Hour Digit0] AmbaWrap_memcpy failed!(%u)", Err, 0U);
                        }
                        BaseAddr += ((ULONG)HOUR_DIGIT_NUM - (ULONG)Number) * (ULONG)(AMBA_BLEND_CHAR_WIDTH);
                    }
                    *pAction = SVC_REC_BLEND_ACTION_UPDATE_BUF;
                }

                if (*pAction == SVC_REC_BLEND_ACTION_UPDATE_BUF) {
                    Err = SvcPlat_CacheClean(HourCLUT_YAddr[StreamId], AMBA_BLEND_CHAR_WIDTH * SEC_DIGIT_NUM * AMBA_BLEND_CHAR_HEIGHT);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "SvcPlat_CacheClean failed!(%u)", Err, 0U);
                    }
                }

                pStopwatch->Hour = Hour;
            }
            break;
        default:
            /* do nothing */
            break;
        }
    } else {
        //Do nothing
    }
}

void SvcDrawStopwatch_EvalMemSize(UINT32 *pNeedSize)
{
    UINT32  YSize, Temp, TotalSize = 0U;

    *pNeedSize = 0U;

    YSize = HOUR_PITCH * AMBA_BLEND_CHAR_HEIGHT;
    /* hour_y */
    Temp = YSize;
    Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
    TotalSize += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
    /* hour_uv */
    Temp = YSize >> 1U;
    Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
    TotalSize += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
    /* hour_ay */
    Temp = YSize;
    Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
    TotalSize += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
    /* hour_auv */
    Temp = YSize >> 1U;
    Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
    TotalSize += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);

    /* min_y */
    Temp = YSize;
    Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
    TotalSize += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
    /* min_uv */
    Temp = YSize >> 1U;
    Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
    TotalSize += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
    /* min_ay */
    Temp = YSize;
    Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
    TotalSize += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
    /* min_auv */
    Temp = YSize >> 1U;
    Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
    TotalSize += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);

    /* sec_y */
    Temp = YSize;
    Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
    TotalSize += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
    /* sec_uv */
    Temp = YSize >> 1U;
    Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
    TotalSize += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
    /* sec_ay */
    Temp = YSize;
    Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
    TotalSize += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
    /* sec_auv */
    Temp = YSize >> 1U;
    Temp *= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
    TotalSize += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);

    /* clut_alpha_y, clut_alpha_uv */
    Temp = CLUT_ENTRY_SIZE;
    TotalSize += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);
    Temp = (CLUT_ENTRY_SIZE >> 1U);
    TotalSize += GetAlignedValU32(Temp, (UINT32)AMBA_CACHE_LINE_SIZE);

    *pNeedSize = TotalSize;
}

void SvcDrawStopwatch_TimerReset(UINT32 StreamBits)
{
    UINT32 i, Err, Bit = 1U;

    for (i = 0U; i < (UINT32)CONFIG_ICAM_MAX_REC_STRM; i++) {
        if (((Bit << i) & StreamBits) > 0U) {
            Err = AmbaWrap_memset(&(Stopwatch[i]), 0xFF, sizeof(STOPWATCH_INFO));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_DRAW_STOP_WATCH, "AmbaWrap_memset failed!(%u)", Err, 0U);
            }
        }
    }
}
