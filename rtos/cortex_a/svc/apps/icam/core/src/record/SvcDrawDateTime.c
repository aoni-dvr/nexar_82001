/**
*  @file SvcDrawDateTime.c
*
*  @copyright Copyright (c) 2018 Ambarella, Inc.
*
*  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
*  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
*  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
*  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
*  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
*  return this Software to Ambarella, Inc.
*
*  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
*  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
*  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*  @details Definitions and Constants for record blend
*
*/
#include "AmbaRTSL_PLL.h"
#include "AmbaKAL.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaCache.h"
#include "AmbaUtility.h"
/* framework */
#include "SvcUtil.h"
#include "SvcErrCode.h"
#include "SvcLog.h"
/* share */
#include "SvcRecMain.h"
#include "SvcBlendCustomFont_X1.h"
#include "SvcBlendCustomFont_X2.h"
#include "SvcBlendCustomFont_X3.h"
#include "SvcRecBlend.h"
#include "SvcDrawDateTime.h"
#include "AmbaRTC.h"
#include "../../../cardv/platform.h"
#include "../../../cardv/app_helper.h"
#include "../../../cardv/peripheral_task/ir_task.h"
#include "../../../cardv/cardv_modules/modules/gnss/gnss_parser.h"
#include "../../../cardv/cardv_modules/modules/dqa_test_script/dqa_test_script.h"

#define SVC_LOG_DRAW_DATETIME  "DST"

#define AMBA_BLEND_CHAR_WIDTH     ((UINT32)32U)
#define AMBA_BLEND_CHAR_HEIGHT    ((UINT32)48U)
#define AMBA_BLEND_CHAR_SIZE      (AMBA_BLEND_CHAR_WIDTH * AMBA_BLEND_CHAR_HEIGHT)
#define DATETIME_DIGIT_NUM   ((UINT32)30U)

static UINT8           DateTimeBuf_Y[CONFIG_ICAM_MAX_REC_STRM][AMBA_BLEND_CHAR_HEIGHT * AMBA_BLEND_CHAR_WIDTH * DATETIME_DIGIT_NUM] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static UINT8           DateTimeBuf_UV[CONFIG_ICAM_MAX_REC_STRM][(AMBA_BLEND_CHAR_HEIGHT * AMBA_BLEND_CHAR_WIDTH * DATETIME_DIGIT_NUM) >> 1U] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static UINT8           DateTimeBuf_AY[CONFIG_ICAM_MAX_REC_STRM][AMBA_BLEND_CHAR_HEIGHT * AMBA_BLEND_CHAR_WIDTH * DATETIME_DIGIT_NUM] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static UINT8           DateTimeBuf_AUV[CONFIG_ICAM_MAX_REC_STRM][(AMBA_BLEND_CHAR_HEIGHT * AMBA_BLEND_CHAR_WIDTH * DATETIME_DIGIT_NUM) >> 1U] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static UINT32          StreamWidthCache[CONFIG_ICAM_MAX_REC_STRM];

static UINT32 RecBlendBehavior;

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::SvcDrawDateTime_Init
 *
 *  @Description:: init stopwatch, draw 000:00:00
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
void SvcDrawDateTime_Init(void)
{
    UINT32 Err = 0;

    Err = AmbaWrap_memset(&(DateTimeBuf_Y[0U][0U]), 255, sizeof(DateTimeBuf_Y));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_DRAW_DATETIME, "AmbaWrap_memset failed!(%u)", Err, 0U);
    }

    Err = AmbaWrap_memset(&(DateTimeBuf_UV[0U][0U]), 128, sizeof(DateTimeBuf_UV));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_DRAW_DATETIME, "AmbaWrap_memset failed!(%u)", Err, 0U);
    }

    Err = AmbaWrap_memset(&(DateTimeBuf_AY[0U][0U]), 255, sizeof(DateTimeBuf_AY));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_DRAW_DATETIME, "AmbaWrap_memset failed!(%u)", Err, 0U);
    }

    Err = AmbaWrap_memset(&(DateTimeBuf_AUV[0U][0U]), 255, sizeof(DateTimeBuf_AUV));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_DRAW_DATETIME, "AmbaWrap_memset failed!(%u)", Err, 0U);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::SvcDrawDateTime_InfoGet
 *
 *  @Description:: get buf info  
 *
 *  @Input      ::
 *      StreamId: stream index
 *      RecBlend: Alpha blending = 0U; CLUT = 1U
 * 
 *  @Output     ::
 *         pInfo: pointer to get the buffer info
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
void SvcDrawDateTime_InfoGet(UINT32 StreamId, UINT32 StreamWidth, UINT32 StreamHeight, SVC_DRAW_DATETIME_INFO *pInfo, const UINT32 RecBlend)
{
    const UINT8 *pBuf;

    (void)StreamHeight;
    StreamWidthCache[StreamId] = StreamWidth;
    /* RecBlend: Alpha blending = 0U; CLUT = 1U */
    RecBlendBehavior = RecBlend;
    if (0U == RecBlendBehavior) { //Alpha blending
        pInfo->DateTime.Width         = (UINT16)(AMBA_BLEND_CHAR_WIDTH * DATETIME_DIGIT_NUM);
        pInfo->DateTime.Height        = (UINT16)AMBA_BLEND_CHAR_HEIGHT;
        pInfo->DateTime.Pitch         = (UINT16)(AMBA_BLEND_CHAR_WIDTH * DATETIME_DIGIT_NUM);
        if (StreamWidth > 1920) {
            pInfo->DateTime.RealWidth     = (UINT16)(AMBA_BLEND_REAL_CHAR_WIDTH_X3 * DATETIME_DIGIT_NUM);
        } else if (StreamWidth > 1280) {
            pInfo->DateTime.RealWidth     = (UINT16)(AMBA_BLEND_REAL_CHAR_WIDTH_X2 * DATETIME_DIGIT_NUM);
        } else {
            pInfo->DateTime.RealWidth     = (UINT16)(AMBA_BLEND_REAL_CHAR_WIDTH_X1 * DATETIME_DIGIT_NUM);
        }
        pBuf = &(DateTimeBuf_Y[StreamId][0]);
        AmbaMisra_TypeCast32(&(pInfo->DateTime.BaseAddrY), &pBuf);
        pBuf = &(DateTimeBuf_UV[StreamId][0]);
        AmbaMisra_TypeCast32(&(pInfo->DateTime.BaseAddrUV), &pBuf);
        pBuf = &(DateTimeBuf_AY[StreamId][0]);
        AmbaMisra_TypeCast32(&(pInfo->DateTime.BaseAddrAY), &pBuf);
        pBuf = &(DateTimeBuf_AUV[StreamId][0]);
        AmbaMisra_TypeCast32(&(pInfo->DateTime.BaseAddrAUV), &pBuf);
    }
}

static int GetCharFontIndex(char c)
{
    char *fonts = "01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz~`@#$%^&*()-_+={}[]|\\:;'<,>.? /";
    unsigned int i = 0;

    for (i = 0; i < strlen(fonts); i++) {
        if (c == fonts[i]) {
            return i;
        }
    }
    return 0;
}

void SvcDrawDateTime_BufUpdate(UINT16 StreamId, UINT16 BlendAreaIdx, UINT32 *pAction)
{
    UINT32 Width = StreamWidthCache[StreamId];
    UINT32 RealCharWidth = 0, RealCharHeight = 0;

    if (Width > 1920) {
        RealCharWidth = AMBA_BLEND_REAL_CHAR_WIDTH_X3;
        RealCharHeight = AMBA_BLEND_REAL_CHAR_HEIGHT_X3;
    } else if (Width > 1280) {
        RealCharWidth = AMBA_BLEND_REAL_CHAR_WIDTH_X2;
        RealCharHeight = AMBA_BLEND_REAL_CHAR_HEIGHT_X2;
    } else {
        RealCharWidth = AMBA_BLEND_REAL_CHAR_WIDTH_X1;
        RealCharHeight = AMBA_BLEND_REAL_CHAR_HEIGHT_X1;
    }

    if (0U == RecBlendBehavior) { //Alpha blending
        UINT32       Addr;
        const UINT8  *pBuf;
        char buf[64] = {0};
        unsigned int i = 0;
        int num = 0;
        UINT32 XOffset, Y, Err;
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        char tmp[64] = {0};

        memset(buf, 0, sizeof(buf));
        *pAction = SVC_REC_BLEND_ACTION_UPDATE_BUF;
 
        SvcUserPref_Get(&pSvcUserPref);
        if (pSvcUserPref->WaterMark & WATER_MARK_DATETIME) {
            memset(tmp, 0, sizeof(tmp));
            if (pSvcUserPref->Use24HMode) {
                linux_time_s linux_time;
                app_helper.get_linux_time(&linux_time);
                snprintf(tmp, sizeof(tmp) - 1, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", linux_time.local_time.year, linux_time.local_time.month, linux_time.local_time.day,
                                                                                 linux_time.local_time.hour, linux_time.local_time.minute, linux_time.local_time.second);
            } else {
                linux_time_s linux_time;
                int pm = 0;
                app_helper.get_linux_time(&linux_time);
                time_convert_12h(&(linux_time.local_time), &pm);
                if (pm) {
                    snprintf(tmp, sizeof(tmp) - 1, "%.4d-%.2d-%.2d %.2d:%.2d:%.2dpm", linux_time.local_time.year, linux_time.local_time.month, linux_time.local_time.day,
                                                                                     linux_time.local_time.hour, linux_time.local_time.minute, linux_time.local_time.second);
                } else {
                    snprintf(tmp, sizeof(tmp) - 1, "%.4d-%.2d-%.2d %.2d:%.2d:%.2dam", linux_time.local_time.year, linux_time.local_time.month, linux_time.local_time.day,
                                                                                     linux_time.local_time.hour, linux_time.local_time.minute, linux_time.local_time.second);
                }
            }
            strcat(buf, tmp);
        }

        if (dqa_test_script.ir_debug && app_helper.external_camera_enabled) {
            memset(tmp, 0, sizeof(tmp));
            snprintf(tmp, sizeof(tmp) - 1, " %d", ir_task_get_brightness());            
            strcat(buf, tmp);
        } else {
            if (pSvcUserPref->WaterMark & WATER_MARK_SPEED) {
                gnss_data_s gnss_data;
                gnss_parser_get_data(&gnss_data);
                memset(tmp, 0, sizeof(tmp));
                if (pSvcUserPref->SpeedUnit == SPEED_UNIT_KMH) {
                    snprintf(tmp, sizeof(tmp) - 1, " %03dkph", (int)(gnss_data.m_nSpeed));
                } else {
                    snprintf(tmp, sizeof(tmp) - 1, " %03dmph", (int)(gnss_data.m_nSpeed * 0.6213712));
                }
                strcat(buf, tmp);
            }
        }
        for (i = 0; i < DATETIME_DIGIT_NUM; i++) {
            if (i < strlen(buf)) {
                num = GetCharFontIndex(buf[i]);
            } else {
                num = GetCharFontIndex(' ');
            }
            XOffset = RealCharWidth * i;
            for (Y = (AMBA_BLEND_CHAR_HEIGHT - RealCharHeight); Y < AMBA_BLEND_CHAR_HEIGHT; Y++) {
                if (Width > 1920) {
                    Err = AmbaWrap_memcpy(&(DateTimeBuf_AY[StreamId][(Y * AMBA_BLEND_CHAR_WIDTH * DATETIME_DIGIT_NUM) + XOffset]), &(AmbaBlendChar_Alpha_X3[num][(Y - (AMBA_BLEND_CHAR_HEIGHT - RealCharHeight)) * RealCharWidth]), RealCharWidth);
                } else if (Width > 1280) {
                    Err = AmbaWrap_memcpy(&(DateTimeBuf_AY[StreamId][(Y * AMBA_BLEND_CHAR_WIDTH * DATETIME_DIGIT_NUM) + XOffset]), &(AmbaBlendChar_Alpha_X2[num][(Y - (AMBA_BLEND_CHAR_HEIGHT - RealCharHeight)) * RealCharWidth]), RealCharWidth);
                } else {
                    Err = AmbaWrap_memcpy(&(DateTimeBuf_AY[StreamId][(Y * AMBA_BLEND_CHAR_WIDTH * DATETIME_DIGIT_NUM) + XOffset]), &(AmbaBlendChar_Alpha_X1[num][(Y - (AMBA_BLEND_CHAR_HEIGHT - RealCharHeight)) * RealCharWidth]), RealCharWidth);
                }
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_DRAW_DATETIME, "AmbaWrap_memcpy failed!(%u)", Err, 0U);
                }
            }
        }
        pBuf = &(DateTimeBuf_AY[StreamId][0U]);
        AmbaMisra_TypeCast32(&Addr, &pBuf);
        (void)AmbaCache_DataFlush(Addr, AMBA_BLEND_CHAR_HEIGHT * AMBA_BLEND_CHAR_WIDTH * DATETIME_DIGIT_NUM);
    }
}
