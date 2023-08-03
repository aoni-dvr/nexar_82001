/**
 *  @file SvcOsdTask.c
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
 *  @details svc osd task
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"

/* ssp */
#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#if defined(CONFIG_SVC_ENABLE_STEREO_CLUT)
#include "AmbaSTU_IF.h"
#endif

/* svc-framework */
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcMem.h"
#include "SvcWrap.h"

/* app-shared */
#include "SvcResCfg.h"
#include "SvcOsd.h"
#include "SvcBuffer.h"

/* app-icam */
#include "SvcBufMap.h"
#include "SvcOsdTask.h"

#define SVC_LOG_OSD_TASK "SVC_OSD_TASK"

static UINT32 SvcOsdTask_GetClut(UINT8 **pClut, UINT32 *pClutSize)
{
    UINT32 RetVal = SVC_OK;

    /* Internal CLUT */
    *pClut = NULL;
    *pClutSize = 0U;

#if defined(CONFIG_SVC_ENABLE_STEREO_CLUT)
    {
        AMBA_STU_OSD_PALETTE_ORDER_s ColorPaletteOrder;
        static AMBA_STU_8BITS_COLOR_PALETTE_s ColorPalette GNU_ALIGNED_CACHESAFE;

        ColorPaletteOrder.Color[0] = AMBA_STU_OSD_BLUE;
        ColorPaletteOrder.Color[1] = AMBA_STU_OSD_GREEN;
        ColorPaletteOrder.Color[2] = AMBA_STU_OSD_RED;
        ColorPaletteOrder.Color[3] = AMBA_STU_OSD_ALPHA;
        RetVal = AmbaSTU_GetDefaultColorPalette(&ColorPaletteOrder, &ColorPalette);
        if (SVC_OK == RetVal) {
            /* Set to transparent for Index 0 and 255(if HW cannot detect the disparity) */
            ColorPalette.ColorLUT[(4 * 1) - 1]   = 0U;
            ColorPalette.ColorLUT[(4 * 256) - 1] = 0U;
            *pClut     = ColorPalette.ColorLUT;
            *pClutSize = sizeof(ColorPalette.ColorLUT);
        } else {
            SvcLog_NG(SVC_LOG_OSD_TASK, "Get stereo CLUT failed", 0U, 0U);
        }
    }
#endif

    return RetVal;
}

/**
 * Init OSD. Prepare the resource needed for OSD.
 * return 0-OK, 1-NG
 */
UINT32 SvcOsdTask_Init(void)
{
    UINT32 RetVal;
    UINT32 i, Bit = 0x1U;
    UINT32 VoutID;
    UINT32 Size = 0U;
    ULONG  Base = 0U;
    UINT32 WorkSize;
    SVC_OSD_CONFIG_s Config;

    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    const SVC_DISP_STRM_s *pDisp;

    SvcLog_DBG(SVC_LOG_OSD_TASK, "SvcOsdTask_Init start", 0, 0);
    RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_OSD_BUF, &Base, &Size);

    if (SVC_OK == RetVal) {

        if (SVC_OK == SvcOsd_WorkMemSizeQuery(&WorkSize)) {
            UINT8 *pOsdWorkMem = NULL;

            AmbaMisra_TypeCast(&pOsdWorkMem, &Base);
            if (0U == SvcOsd_SetWorkBuf(pOsdWorkMem)) {
                Base = Base + WorkSize;
            }
        }

        for (i = 0; i < pCfg->DispNum; i++) {
            if ((pCfg->DispBits & (UINT32)(Bit << i)) == 0U) {
                continue;
            }

            pDisp = &(pCfg->DispStrm[i]);
            VoutID = pDisp->VoutID;
            Config.OsdSize.Width   = pDisp->StrmCfg.MaxWin.Width;
            Config.OsdSize.Height  = pDisp->StrmCfg.MaxWin.Height;
            Config.VoutSize.Width  = pDisp->StrmCfg.MaxWin.Width;
            Config.VoutSize.Height = pDisp->StrmCfg.MaxWin.Height;
            Config.VoutInterlace   = pDisp->FrameRate.Interlace;

            Config.MemType = SVC_OSD_MEM_TYPE_CACHE;
            if (VoutID == 0U) {
                AmbaMisra_TypeCast(&Config.pOsdMem, &Base);
            } else {
                if (SVC_OK == SvcOsd_MemSizeQuery(0U, &WorkSize)) {
                    Base = Base + WorkSize;
                }
                AmbaMisra_TypeCast(&Config.pOsdMem, &Base);
            }

            RetVal = SvcOsdTask_GetClut(&Config.pClut, &Config.ClutSize);
            RetVal = SvcOsd_Init(VoutID, &Config);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_OSD_TASK, "SvcOsd_Init(%d) failed %d", i, RetVal);
            }
        }
    }
    SvcLog_DBG(SVC_LOG_OSD_TASK, "SvcOsdTask_Init done", 0, 0);

    return SVC_OK;
}

/**
 * Query OSD mem buffer size, based on the maximum osd buffer size
 * @param [out] Worksize of the osd module
 * return 0-OK, 1-NG
 */
UINT32 SvcOsdTask_MemSizeQuery(UINT32 *pWorkSize)
{
    UINT32 RetVal = SVC_OK;
    UINT32 WorkSize = 0U;

    if (pWorkSize == NULL) {
        SvcLog_NG(SVC_LOG_OSD_TASK, "SvcOsdTask_MemSizeQuery failed. Input should not be NULL", 0U, 0U);
        RetVal = SVC_NG;
    } else {
        *pWorkSize = 0U;
        for (UINT32 Chan = 0U; Chan < SVC_OSD_CHAN_NUM; Chan++) {
            if (SVC_OK == SvcOsd_MemSizeQuery(Chan, &WorkSize)) {
                *pWorkSize += WorkSize;
            }
        }

        if (SVC_OK == SvcOsd_WorkMemSizeQuery(&WorkSize)) {
            *pWorkSize += WorkSize;
        }

        RetVal = SVC_OK;
    }

    return RetVal;
}
