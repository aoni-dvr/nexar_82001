/**
*  @file SvcLogoDraw.c
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
*  @Svc Logo Draw
*
*/

#include "AmbaKAL.h"
#include "AmbaTypes.h"
#include "AmbaSYS.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaUtility.h"
#include "AmbaDSP_Liveview.h"
#include "SvcErrCode.h"
#include "SvcOsd.h"
#include "SvcGui.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcLogoDraw.h"

#define SVC_LOGO_DRAW       "LOGO_DRAW"

#define LOGO_AMBA_UPDATE   (0x01U)
#define SVC_GUI_LOGO_DRAW  (9U)

#define SVC_LOGO_AMBA_IDX   (0U)
#if defined(CONFIG_ICAM_REBEL_CUSTOMER_NN) && defined(CONFIG_ICAM_REBEL_CUSTOMER_NN_LOGO)
#define SVC_LOGO_REBEL_IDX  (1U)
#define SVC_LOGO_NUM        (2U)
#else
#define SVC_LOGO_NUM        (1U)
#endif

#if defined(CONFIG_ICAM_VOUTB_OSD_BUF_FHD)
#define SVC_AMBA_LOGO_W    (280U)
#define SVC_AMBA_LOGO_H    (180U)
#else
#define SVC_AMBA_LOGO_W    (140U)
#define SVC_AMBA_LOGO_H    (90U)
#endif
#define SVC_AMBA_LOGO_SIZE (SVC_AMBA_LOGO_W * SVC_AMBA_LOGO_H)

#if defined(CONFIG_ICAM_VOUTB_OSD_BUF_FHD)
#define SVC_OSD_BUF_MAX_WIDTH_VOUTB     (1920U)
#elif defined(CONFIG_ICAM_VOUTB_OSD_BUF_HD)
#define SVC_OSD_BUF_MAX_WIDTH_VOUTB     (1280U)
#else
#define SVC_OSD_BUF_MAX_WIDTH_VOUTB     (960U)
#endif

#if defined(CONFIG_ICAM_VOUTB_OSD_BUF_FHD)
    #define LOGO_AMBA_SIZE      (0x38000U)
    #define LOGO_BUF_SIZE       (LOGO_AMBA_SIZE)
#else
    #define LOGO_AMBA_SIZE      (0x10000U)

    #if defined(CONFIG_ICAM_REBEL_CUSTOMER_NN) && defined(CONFIG_ICAM_REBEL_CUSTOMER_NN_LOGO)
    #define LOGO_REBEL_SIZE     (0x10000U)
    #define LOGO_BUF_SIZE       (LOGO_AMBA_SIZE + LOGO_REBEL_SIZE)
    #else
    #define LOGO_BUF_SIZE       (LOGO_AMBA_SIZE)
    #endif
#endif

typedef struct {
    UINT32 StartX;
    UINT32 StartY;
    UINT32 EndX;
    UINT32 EndY;
} SVC_LODO_DRAW_AREA_s;

typedef struct {
    UINT32               Enable;
    char                 IconName[32];
    UINT32               Vout;
    SVC_LODO_DRAW_AREA_s IconArea;
    SVC_OSD_BMP_s        IconInfo;
} SVC_LODO_DRAW_s;

static SVC_LODO_DRAW_s LogoDrawInfo[SVC_LOGO_NUM] = {
#if defined(CONFIG_ICAM_PROJECT_EMIRROR)
    [0] = {1U, "icon_amba_logo.bmp", 0U,
            {1670U, 40U, 1920U, 480U},
            {{5U, 0U, 0U, SVC_AMBA_LOGO_W, SVC_AMBA_LOGO_H}, (SVC_AMBA_LOGO_SIZE) << 2, NULL}
    },
#elif defined(CONFIG_ICAM_PROJECT_ADAS_DVR)
    #ifdef CONFIG_ICAM_REBEL_USAGE
    [0] = {1U, "icon_amba_logo.bmp", 0U,
            {(SVC_OSD_BUF_MAX_WIDTH_VOUTB - SVC_AMBA_LOGO_W), 0U, SVC_OSD_BUF_MAX_WIDTH_VOUTB, SVC_AMBA_LOGO_H},
            {{5U, 0U, 0U, SVC_AMBA_LOGO_W, SVC_AMBA_LOGO_H}, (SVC_AMBA_LOGO_SIZE) << 2, NULL}
    },
        #if defined(CONFIG_ICAM_REBEL_CUSTOMER_NN) && defined(CONFIG_ICAM_REBEL_CUSTOMER_NN_LOGO)
        [1] = {1U, "icon_rebel_logo.bmp", 0U,
                {0U, 5U, 200U, 61U},
                {{5U, 0U, 0U, 200U, 56U}, (200U * 56U) << 2, NULL}
        },
        #endif
    #else
    [0] = {1U, "icon_amba_logo.bmp", 1U,
            {(SVC_OSD_BUF_MAX_WIDTH_VOUTB - SVC_AMBA_LOGO_W), 0U, SVC_OSD_BUF_MAX_WIDTH_VOUTB, SVC_AMBA_LOGO_H},
            {{5U, 0U, 0U, SVC_AMBA_LOGO_W, SVC_AMBA_LOGO_H}, (SVC_AMBA_LOGO_SIZE) << 2, NULL}
    },
    #endif
#else
    [0] = {1U, "icon_amba_logo.bmp", 1U,
            {(SVC_OSD_BUF_MAX_WIDTH_VOUTB - SVC_AMBA_LOGO_W), 0U, SVC_OSD_BUF_MAX_WIDTH_VOUTB, SVC_AMBA_LOGO_H},
            { {5U, 0U, 0U, SVC_AMBA_LOGO_W, SVC_AMBA_LOGO_H}, (SVC_AMBA_LOGO_SIZE) << 2, NULL}
    },
#endif
};

static UINT32 SvcLogoGuiUpdate = 0U;

static void LogoDrawCB(UINT32 VoutIdx, UINT32 Level)
{
    UINT32 RetVal = SVC_OK;

    AmbaMisra_TouchUnused(&Level);//pass vcast
    AmbaMisra_TouchUnused(&VoutIdx);//pass vcast

    for (UINT8 i = 0U; i<SVC_LOGO_NUM; i++) {
        if (LogoDrawInfo[i].Enable == 1U) {
            RetVal = SvcOsd_DrawBmp(LogoDrawInfo[i].Vout,
                                    LogoDrawInfo[i].IconArea.StartX,
                                    LogoDrawInfo[i].IconArea.StartY,
                                    LogoDrawInfo[i].IconArea.EndX,
                                    LogoDrawInfo[i].IconArea.EndY,
                                    &LogoDrawInfo[i].IconInfo);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOGO_DRAW, "SvcOsd_DrawBmp() Draw Logo error idx = %d", i, 0U);
            }
        }
    }

}

static void MsgDrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if ((SvcLogoGuiUpdate & LOGO_AMBA_UPDATE) > 0U) {
        SvcLogoGuiUpdate &= ~(LOGO_AMBA_UPDATE);
        *pUpdate = 1U;
    }
}


/**
 *  Logo draw init
 *  @param[in] VoutIdx Vout index
 */
void SvcLogoDraw_Init(UINT32 VoutIdx)
{
    static UINT32 InitFlag = 0U;

    UINT8   *pBufBase;
    ULONG   MemBase, BufBase;
    UINT32  RetVal = SVC_OK, MemSize = 0U;

    if (InitFlag == 0U) {
        RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_GUI_LOGO, &MemBase, &MemSize);
        if ((RetVal == SVC_OK) && (LOGO_BUF_SIZE <= MemSize)) {
            BufBase = MemBase;
            AmbaMisra_TypeCast(&pBufBase, &BufBase);

            LogoDrawInfo[SVC_LOGO_AMBA_IDX].IconInfo.data = pBufBase;
            #if defined(CONFIG_ICAM_REBEL_CUSTOMER_NN) && defined(CONFIG_ICAM_REBEL_CUSTOMER_NN_LOGO)
            BufBase += LOGO_AMBA_SIZE;
            AmbaMisra_TypeCast(&pBufBase, &BufBase);
            LogoDrawInfo[SVC_LOGO_REBEL_IDX].IconInfo.data = pBufBase;
            #endif

            for (UINT8 i = 0U; i < SVC_LOGO_NUM; i++) {
                if (0U < LogoDrawInfo[i].Enable) {
                    RetVal = SvcOsd_LoadBmp(LogoDrawInfo[i].IconInfo.data,
                                            LogoDrawInfo[i].IconInfo.data_size,
                                            LogoDrawInfo[i].IconName);
                    if (RetVal != SVC_OK) {
                        SvcLog_NG(SVC_LOGO_DRAW, "SvcOsd_LoadBmp() error idx = %d", i, 0U);
                    }
                }
            }

            SvcGui_Register(VoutIdx, SVC_GUI_LOGO_DRAW, "Logo", LogoDrawCB, MsgDrawUpdate);
            InitFlag = 1U;
        } else {
            SvcLog_NG(SVC_LOGO_DRAW, "fail to get logo buffer", 0U, 0U);
        }
    }

#ifdef SVC_OSD_BUF_MAX_WIDTH_VOUTB
    SvcLog_OK(SVC_LOGO_DRAW, "SVC_OSD_BUF_MAX_WIDTH_VOUTB = %d", SVC_OSD_BUF_MAX_WIDTH_VOUTB, 0U);
#endif
}

/**
 *  Query of logo memory size
 *  @param[out] pWorkSize memory size needed
 */
void SvcLogoDraw_MemSizeQuery(UINT32 *pWorkSize)
{
    *pWorkSize = LOGO_BUF_SIZE;
}

/**
 *  Update Amba Logo
 */
void SvcLogoDraw_Update(void)
{
    SvcLogoGuiUpdate |= LOGO_AMBA_UPDATE;
}
