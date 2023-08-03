/**
*  @file SvcDrawLogo.c
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
#include "SvcRecBlend.h"
#include "SvcDrawLogo.h"
#include "AmbaRTC.h"
#include "../../../cardv/platform.h"
#include "../../../cardv/app_helper.h"
#include "AmbaNVM_Partition.h"
#include "SvcOsd.h"

#define SVC_LOG_DRAW_LOGO  "DST"

#define AMBA_BLEND_LOGO_WIDTH (448)
#define AMBA_BLEND_LOGO_HEIGHT (128)

static UINT8           LogoBuf_Y[CONFIG_ICAM_MAX_REC_STRM][AMBA_BLEND_LOGO_WIDTH * AMBA_BLEND_LOGO_HEIGHT] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static UINT8           LogoBuf_UV[CONFIG_ICAM_MAX_REC_STRM][(AMBA_BLEND_LOGO_WIDTH * AMBA_BLEND_LOGO_HEIGHT) >> 1U] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static UINT8           LogoBuf_AY[CONFIG_ICAM_MAX_REC_STRM][AMBA_BLEND_LOGO_WIDTH * AMBA_BLEND_LOGO_HEIGHT] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static UINT8           LogoBuf_AUV[CONFIG_ICAM_MAX_REC_STRM][(AMBA_BLEND_LOGO_WIDTH * AMBA_BLEND_LOGO_HEIGHT) >> 1U] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

static UINT32 RecBlendBehavior;
static int logo_enable __attribute__((aligned(4096))) = 0;
    
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::SvcDrawLogo_Init
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
void SvcDrawLogo_Init(void)
{
    UINT32 Err = 0, StreamId = 0;
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    SvcUserPref_Get(&pSvcUserPref);
    logo_enable = (pSvcUserPref->WaterMark & WATER_MARK_LOGO) ? 1 : 0;
    for (StreamId = 0; StreamId < CONFIG_ICAM_MAX_REC_STRM; StreamId++) {
        if (logo_enable) {
            int logo_index = 0;
            if (StreamId == 0) {
                if (pSvcUserPref->InternalCamera.MainStream.width > 1920) {
                    logo_index = 3;
                } else if (pSvcUserPref->InternalCamera.MainStream.width > 1280) {
                    logo_index = 2;
                } else {
                    logo_index = 1;
                }
            } else if (StreamId == 1) {
                if (pSvcUserPref->InternalCamera.SecStream.width > 1920) {
                    logo_index = 3;
                } else if (pSvcUserPref->InternalCamera.SecStream.width > 1280) {
                    logo_index = 2;
                } else {
                    logo_index = 1;
                }
            } else if (StreamId == 2) {
                if (pSvcUserPref->ExternalCamera.MainStream.width > 1920) {
                    logo_index = 3;
                } else if (pSvcUserPref->ExternalCamera.MainStream.width > 1280) {
                    logo_index = 2;
                } else {
                    logo_index = 1;
                }
            } else if (StreamId == 3) {
                if (pSvcUserPref->ExternalCamera.SecStream.width > 1920) {
                    logo_index = 3;
                } else if (pSvcUserPref->ExternalCamera.SecStream.width > 1280) {
                    logo_index = 2;
                } else {
                    logo_index = 1;
                }
            }

            if (logo_index == 3) {
                Err = AmbaNVM_ReadRomFile(NVM_TYPE, AMBA_NVM_ROM_REGION_SYS_DATA, "logo_nexar_x3.yuv", 0U, AMBA_BLEND_LOGO_WIDTH * AMBA_BLEND_LOGO_HEIGHT, &LogoBuf_Y[StreamId][0], 5000);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_DRAW_LOGO, "AmbaNVM_ReadRomFile Y failed!(%u)", Err, 0U);
                }
                Err = AmbaNVM_ReadRomFile(NVM_TYPE, AMBA_NVM_ROM_REGION_SYS_DATA, "logo_nexar_x3.yuv", AMBA_BLEND_LOGO_WIDTH * AMBA_BLEND_LOGO_HEIGHT, AMBA_BLEND_LOGO_WIDTH * AMBA_BLEND_LOGO_HEIGHT >> 1U, &LogoBuf_UV[StreamId][0], 5000);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_DRAW_LOGO, "AmbaNVM_ReadRomFile UV failed!(%u)", Err, 0U);
                }
                Err = AmbaNVM_ReadRomFile(NVM_TYPE, AMBA_NVM_ROM_REGION_SYS_DATA, "logo_nexar_x3.bin", 0U, AMBA_BLEND_LOGO_WIDTH * AMBA_BLEND_LOGO_HEIGHT, &LogoBuf_AY[StreamId][0], 5000);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_DRAW_LOGO, "AmbaNVM_ReadRomFile AY failed!(%u)", Err, 0U);
                }
                Err = AmbaNVM_ReadRomFile(NVM_TYPE, AMBA_NVM_ROM_REGION_SYS_DATA, "logo_nexar_x3.bin", AMBA_BLEND_LOGO_WIDTH * AMBA_BLEND_LOGO_HEIGHT, AMBA_BLEND_LOGO_WIDTH * AMBA_BLEND_LOGO_HEIGHT >> 1U, &LogoBuf_AUV[StreamId][0], 5000);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_DRAW_LOGO, "AmbaNVM_ReadRomFile AUV failed!(%u)", Err, 0U);
                }
            } else if (logo_index <= 2) {
                Err = AmbaNVM_ReadRomFile(NVM_TYPE, AMBA_NVM_ROM_REGION_SYS_DATA, "logo_nexar_x2.yuv", 0U, AMBA_BLEND_LOGO_WIDTH * AMBA_BLEND_LOGO_HEIGHT, &LogoBuf_Y[StreamId][0], 5000);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_DRAW_LOGO, "AmbaNVM_ReadRomFile Y failed!(%u)", Err, 0U);
                }
                Err = AmbaNVM_ReadRomFile(NVM_TYPE, AMBA_NVM_ROM_REGION_SYS_DATA, "logo_nexar_x2.yuv", AMBA_BLEND_LOGO_WIDTH * AMBA_BLEND_LOGO_HEIGHT, AMBA_BLEND_LOGO_WIDTH * AMBA_BLEND_LOGO_HEIGHT >> 1U, &LogoBuf_UV[StreamId][0], 5000);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_DRAW_LOGO, "AmbaNVM_ReadRomFile UV failed!(%u)", Err, 0U);
                }
                Err = AmbaNVM_ReadRomFile(NVM_TYPE, AMBA_NVM_ROM_REGION_SYS_DATA, "logo_nexar_x2.bin", 0U, AMBA_BLEND_LOGO_WIDTH * AMBA_BLEND_LOGO_HEIGHT, &LogoBuf_AY[StreamId][0], 5000);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_DRAW_LOGO, "AmbaNVM_ReadRomFile AY failed!(%u)", Err, 0U);
                }
                Err = AmbaNVM_ReadRomFile(NVM_TYPE, AMBA_NVM_ROM_REGION_SYS_DATA, "logo_nexar_x2.bin", AMBA_BLEND_LOGO_WIDTH * AMBA_BLEND_LOGO_HEIGHT, AMBA_BLEND_LOGO_WIDTH * AMBA_BLEND_LOGO_HEIGHT >> 1U, &LogoBuf_AUV[StreamId][0], 5000);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_DRAW_LOGO, "AmbaNVM_ReadRomFile AUV failed!(%u)", Err, 0U);
                }
            } else {
                Err = AmbaWrap_memset(&(LogoBuf_AY[StreamId][0U]), 255U, sizeof(LogoBuf_AY[StreamId]));
                Err = AmbaWrap_memset(&(LogoBuf_AUV[StreamId][0U]), 255U, sizeof(LogoBuf_AUV[StreamId]));
            }
        } else {
            Err = AmbaWrap_memset(&(LogoBuf_AY[StreamId][0U]), 255U, sizeof(LogoBuf_AY[StreamId]));
            Err = AmbaWrap_memset(&(LogoBuf_AUV[StreamId][0U]), 255U, sizeof(LogoBuf_AUV[StreamId]));
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::SvcDrawLogo_InfoGet
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
void SvcDrawLogo_InfoGet(UINT32 StreamId, SVC_DRAW_LOGO_INFO *pInfo, const UINT32 RecBlend)
{
    const UINT8 *pBuf;

    /* RecBlend: Alpha blending = 0U; CLUT = 1U */
    RecBlendBehavior = RecBlend;
    if (0U == RecBlendBehavior) { //Alpha blending
        pInfo->Logo.Width         = (UINT16)AMBA_BLEND_LOGO_WIDTH;
        pInfo->Logo.Height        = (UINT16)AMBA_BLEND_LOGO_HEIGHT;
        pInfo->Logo.Pitch         = (UINT16)AMBA_BLEND_LOGO_WIDTH;
        pBuf = &(LogoBuf_Y[StreamId][0]);
        AmbaMisra_TypeCast32(&(pInfo->Logo.BaseAddrY), &pBuf);
        pBuf = &(LogoBuf_UV[StreamId][0]);
        AmbaMisra_TypeCast32(&(pInfo->Logo.BaseAddrUV), &pBuf);
        pBuf = &(LogoBuf_AY[StreamId][0]);
        AmbaMisra_TypeCast32(&(pInfo->Logo.BaseAddrAY), &pBuf);
        pBuf = &(LogoBuf_AUV[StreamId][0]);
        AmbaMisra_TypeCast32(&(pInfo->Logo.BaseAddrAUV), &pBuf);
    }
}

//ffmpeg -i logo.png -s 448x128 -pix_fmt nv12 logo.yuv
void SvcDrawLogo_BufUpdate(UINT16 StreamId, UINT16 BlendAreaIdx, UINT32 *pAction)
{
    if (0U == RecBlendBehavior) { //Alpha blending
        UINT32       Addr;
        const UINT8  *pBuf;
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        int enable = 0;

        SvcUserPref_Get(&pSvcUserPref);
        enable = (pSvcUserPref->WaterMark & WATER_MARK_LOGO) ? 1 : 0;
        if (enable != logo_enable) {
            SvcDrawLogo_Init();
        }
        *pAction = SVC_REC_BLEND_ACTION_UPDATE_BUF;

        pBuf = &(LogoBuf_AY[StreamId][0U]);
        AmbaMisra_TypeCast32(&Addr, &pBuf);
        (void)AmbaCache_DataFlush(Addr, AMBA_BLEND_LOGO_WIDTH * AMBA_BLEND_LOGO_HEIGHT);

        pBuf = &(LogoBuf_AUV[StreamId][0U]);
        AmbaMisra_TypeCast32(&Addr, &pBuf);
        (void)AmbaCache_DataFlush(Addr, AMBA_BLEND_LOGO_WIDTH * AMBA_BLEND_LOGO_HEIGHT >> 1);
    }
}

