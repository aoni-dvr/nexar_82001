/**
 *  @file SvcMenuTask.c
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
 *  @details svc Menu control task file
 *
 */

#include ".svc_autogen"

#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaFPD.h"
#include "AmbaFPD_HDMI.h"
#include "AmbaKAL.h"

/* ssp */
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VOUT.h"

/* framework */
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcMem.h"
/* app-shared */
#include "SvcResCfg.h"
#include "SvcOsd.h"
#include "SvcGui.h"
#include "SvcButtonCtrl.h"
#include "SvcSysStat.h"
#include "SvcBuffer.h"
/* app-icam */
#include "SvcAppStat.h"
#include "SvcBufMap.h"
#include "SvcMenuTask.h"
#include "SvcDataLoader.h"

#define SVC_MENU_TASK_ICON_EMR_LIVEVIEW         (0U)
#define SVC_MENU_TASK_ICON_EMR_RECORD           (1U)
#define SVC_MENU_TASK_ICON_EMR_PAN              (2U)
#define SVC_MENU_TASK_ICON_EMR_TILT             (3U)
#define SVC_MENU_TASK_ICON_EMR_ROTATE           (4U)
#define SVC_MENU_TASK_ICON_EMR_DIM              (5U)
#define SVC_MENU_TASK_ICON_EMR_BSD              (6U)
#define SVC_MENU_TASK_ICON_EMR_FORMAT           (7U)
#define SVC_MENU_TASK_ICON_EMR_EMR2_0           (8U)
#define SVC_MENU_TASK_ICON_EMR_OD               (9U)
#define SVC_MENU_TASK_ICON_EMR_DEFOG            (10U)
#define SVC_MENU_TASK_ICON_EMR_DIRT_DETECT      (11U)
#define SVC_MENU_TASK_ICON_EMR_LAYOUT           (12U)
#define SVC_MENU_TASK_ICON_EMR_NUM              (13U)

#define SVC_MENU_TASK_ICON_ADAS_DVR_LIVEVIEW    (0U)
#define SVC_MENU_TASK_ICON_ADAS_DVR_RECORD      (1U)
#define SVC_MENU_TASK_ICON_ADAS_DVR_PLAYBACK    (2U)
#define SVC_MENU_TASK_ICON_ADAS_DVR_FCWS        (3U)
#define SVC_MENU_TASK_ICON_ADAS_DVR_FCMD        (4U)
#define SVC_MENU_TASK_ICON_ADAS_DVR_LDWS        (5U)
#define SVC_MENU_TASK_ICON_ADAS_DVR_DMS         (6U)
#define SVC_MENU_TASK_ICON_ADAS_DVR_WIFI        (7U)
#define SVC_MENU_TASK_ICON_ADAS_DVR_OD          (8U)
#define SVC_MENU_TASK_ICON_ADAS_DVR_DEFOG       (9U)
#define SVC_MENU_TASK_ICON_ADAS_DVR_DIRT_DETECT (10U)
#define SVC_MENU_TASK_ICON_ADAS_DVR_NUM         (11U)

#define SVC_MENU_TASK_ICON_DMS_LIVEVIEW         (0U)
#define SVC_MENU_TASK_ICON_DMS_RECORD           (1U)
#define SVC_MENU_TASK_ICON_DMS_PLAYBACK         (2U)
#define SVC_MENU_TASK_ICON_DMS_DMS              (3U)
#define SVC_MENU_TASK_ICON_DMS_NUM              (4U)

#define SVC_MENU_TASK_ICON_PLAYBACK             (0U)
#define SVC_MENU_TASK_ICON_PLAYBACK_PREV        (1U)
#define SVC_MENU_TASK_ICON_PLAYBACK_NEXT        (2U)
#define SVC_MENU_TASK_ICON_PLAYBACK_PLAY        (3U)
#define SVC_MENU_TASK_ICON_PLAYBACK_STOP        (4U)
#define SVC_MENU_TASK_ICON_PLAYBACK_BACKWARD    (5U)
#define SVC_MENU_TASK_ICON_PLAYBACK_FORWARD     (6U)
#define SVC_MENU_TASK_ICON_PLAYBACK_NUM         (7U)

#define SVC_MENU_TASK_ICON_SUR_LIVEVIEW    (0U)
#define SVC_MENU_TASK_ICON_SUR_RECORD      (1U)
#define SVC_MENU_TASK_ICON_SUR_PLAYBACK    (2U)
#define SVC_MENU_TASK_ICON_SUR_PSD         (3U)
#define SVC_MENU_TASK_ICON_SUR_OWS         (4U)
#define SVC_MENU_TASK_ICON_SUR_RCTA        (5U)
#define SVC_MENU_TASK_ICON_SUR_NUM         (6U)

#define SVC_MENU_TASK_GUI_VALID             (0x01U)
#define SVC_MENU_TASK_GUI_UPDATE            (0x02U)

#if defined(CONFIG_ICAM_VOUTB_OSD_BUF_FHD)
#define SVC_MENU_TASK_ICON_W                    (120U)
#define SVC_MENU_TASK_ICON_H                    (120U)
#else
#define SVC_MENU_TASK_ICON_W                    (60U)
#define SVC_MENU_TASK_ICON_H                    (60U)
#endif

#define SVC_MENU_TASK_ICON_NUM                  (13U)

typedef struct {
    UINT32 StartX;
    UINT32 StartY;
    UINT32 EndX;
    UINT32 EndY;
} SVC_MENU_AREA_s;

typedef struct /* _SVC_MENU_TASK_ICON_s_ */ {
    SVC_MENU_AREA_s MenuArea;
    char            IconName[32];       /* Icon name is used to read icon from FLASH */
    char            SelIconName[32];    /* Icon name is used to read icon from FLASH */
    SVC_OSD_BMP_s     IconInfo; 
    SVC_OSD_BMP_s     SelIconInfo;
    //UINT8           *pIcon;
    //UINT8           *pSelIcon;
    UINT32          IsPressed;
} SVC_MENU_TASK_ICON_s;

typedef struct /* _SVC_MENU_TASK_MENU_CTRL_s_ */ {
    UINT8 MenuType;
    UINT8 FuncMenuCount;
    UINT8 FuncMenuOn;
    UINT8 FuncMenuIdx;
    UINT32 MenuValid;
} SVC_MENU_TASK_MENU_CTRL_s;

typedef struct {
    UINT8 Vout;
    UINT8 MenuOn;
    UINT8 MenuIconCount;
    SVC_MENU_AREA_s MenuArea;
    SVC_MENU_TASK_ICON_s FuncMenuIcon[SVC_MENU_TASK_ICON_NUM];
} SVC_MENU_CFG_s;

typedef struct {
    UINT8  *pBuf;
    UINT32 Size;
} SVC_MENU_BUFFER_s;

static SVC_MENU_CFG_s MenuCfg[SVC_MENU_TASK_TYPE_NUM] = {

    [SVC_MENU_TASK_TYPE_LV_EMR] = {   /* For liveview EMR */
        .Vout = VOUT_IDX_A,
        .MenuOn = 1U,
        .MenuIconCount = SVC_MENU_TASK_ICON_EMR_NUM,
        .MenuArea = { 0U } /* TBD */,
        .FuncMenuIcon = {
            [SVC_MENU_TASK_ICON_EMR_LIVEVIEW]    = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_liveview_0.bmp",       "icon_liveview_1.bmp",       { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_EMR_RECORD]      = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_record_0.bmp",         "icon_record_1.bmp",         { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_EMR_PAN]         = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_pan_0.bmp",            "icon_pan_1.bmp",            { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_EMR_TILT]        = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_tilt_0.bmp",           "icon_tilt_1.bmp",           { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_EMR_ROTATE]      = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_rotate_0.bmp",         "icon_rotate_1.bmp",         { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_EMR_DIM]         = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_dim_0.bmp",            "icon_dim_1.bmp",            { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_EMR_BSD]         = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_bsd_0.bmp",            "icon_bsd_1.bmp",            { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_EMR_FORMAT]      = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_format_0.bmp",         "icon_format_1.bmp",         { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_EMR_EMR2_0]      = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_emr2_0.bmp",           "icon_emr2_1.bmp",           { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_EMR_OD]          = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_od_0.bmp",             "icon_od_1.bmp",             { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_EMR_DEFOG]       = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_defog_0.bmp",          "icon_defog_1.bmp",          { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_EMR_DIRT_DETECT] = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_dirt_detection_0.bmp", "icon_dirt_detection_1.bmp", { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_EMR_LAYOUT]      = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_layout_0.bmp",         "icon_layout_1.bmp",         { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
        },
    },

    [SVC_MENU_TASK_TYPE_LV_ADAS_DVR] = {   /* For liveview ADAS */
        #if (AMBA_DSP_MAX_VOUT_NUM > 1U)
        .Vout = VOUT_IDX_B,
        #else
        .Vout = VOUT_IDX_A,
        #endif
        .MenuOn = 1U,
        .MenuIconCount = SVC_MENU_TASK_ICON_ADAS_DVR_NUM,
        .MenuArea = { 0U } /* TBD */,
        .FuncMenuIcon = {
            [SVC_MENU_TASK_ICON_ADAS_DVR_LIVEVIEW]    = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_liveview_0.bmp",       "icon_liveview_1.bmp",         { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL}, 0 },
            [SVC_MENU_TASK_ICON_ADAS_DVR_RECORD]      = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_record_0.bmp",         "icon_record_1.bmp",           { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL}, 0 },
            [SVC_MENU_TASK_ICON_ADAS_DVR_PLAYBACK]    = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_playback_0.bmp",       "icon_playback_1.bmp",         { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL}, 0 },
            [SVC_MENU_TASK_ICON_ADAS_DVR_FCWS]        = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_fcws_0.bmp",           "icon_fcws_1.bmp",             { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL}, 0 },
            [SVC_MENU_TASK_ICON_ADAS_DVR_FCMD]        = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_fcmd_0.bmp",           "icon_fcmd_1.bmp",             { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL}, 0 },
            [SVC_MENU_TASK_ICON_ADAS_DVR_LDWS]        = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_ldws_0.bmp",           "icon_ldws_1.bmp",             { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL}, 0 },
            [SVC_MENU_TASK_ICON_ADAS_DVR_DMS]         = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_dms_0.bmp",            "icon_dms_1.bmp",              { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL}, 0 },
            [SVC_MENU_TASK_ICON_ADAS_DVR_WIFI]        = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_wifi_0.bmp",           "icon_wifi_1.bmp",             { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL}, 0 },
            [SVC_MENU_TASK_ICON_ADAS_DVR_OD]          = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_od_0.bmp",             "icon_od_1.bmp",               { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL}, 0 },
            [SVC_MENU_TASK_ICON_ADAS_DVR_DEFOG]       = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_defog_0.bmp",          "icon_defog_1.bmp",            { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL}, 0 },
            [SVC_MENU_TASK_ICON_ADAS_DVR_DIRT_DETECT] = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_dirt_detection_0.bmp", "icon_dirt_detection_1.bmp",   { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL}, 0 },
            [11U /* Reserved and misra-c */]          = { { 0U, 0U, 0U, 0U } /* TBD */, "",                          "",                            { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL}, 0 },
            [12U /* Reserved and misra-c */]          = { { 0U, 0U, 0U, 0U } /* TBD */, "",                          "",                            { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL}, 0 },
        },
    },

    [SVC_MENU_TASK_TYPE_LV_DMS] = {   /* For liveview DMS */
        #if (AMBA_DSP_MAX_VOUT_NUM > 1U)
        .Vout = VOUT_IDX_B,
        #else
        .Vout = VOUT_IDX_A,
        #endif
        .MenuOn = 0U,
        .MenuIconCount = SVC_MENU_TASK_ICON_DMS_NUM,
        .MenuArea = { 0U } /* TBD */,
        .FuncMenuIcon = {
            [SVC_MENU_TASK_ICON_DMS_LIVEVIEW] = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_liveview_0.bmp",       "icon_liveview_1.bmp",       { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL },  { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_DMS_RECORD]   = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_record_0.bmp",         "icon_record_1.bmp",         { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL },  { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_DMS_PLAYBACK] = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_playback_0.bmp",       "icon_playback_1.bmp",       { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL },  { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_DMS_DMS]      = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_dms_0.bmp",            "icon_dms_1.bmp",            { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL },  { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [4U  /* Reserved and misra-c */]  = { { 0U, 0U, 0U, 0U } /* TBD */, "",                          "",                          { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL },  { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [5U  /* Reserved and misra-c */]  = { { 0U, 0U, 0U, 0U } /* TBD */, "",                          "",                          { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL },  { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [6U  /* Reserved and misra-c */]  = { { 0U, 0U, 0U, 0U } /* TBD */, "",                          "",                          { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL },  { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [7U  /* Reserved and misra-c */]  = { { 0U, 0U, 0U, 0U } /* TBD */, "",                          "",                          { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL },  { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [8U  /* Reserved and misra-c */]  = { { 0U, 0U, 0U, 0U } /* TBD */, "",                          "",                          { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL },  { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [9U  /* Reserved and misra-c */]  = { { 0U, 0U, 0U, 0U } /* TBD */, "",                          "",                          { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL },  { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [10U /* Reserved and misra-c */]  = { { 0U, 0U, 0U, 0U } /* TBD */, "",                          "",                          { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL },  { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [11U /* Reserved and misra-c */]  = { { 0U, 0U, 0U, 0U } /* TBD */, "",                          "",                          { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL },  { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [12U /* Reserved and misra-c */]  = { { 0U, 0U, 0U, 0U } /* TBD */, "",                          "",                          { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL },  { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            
        },
    },

    [SVC_MENU_TASK_TYPE_SURROUND] = {   /* For liveview SUR*/
        #if (AMBA_DSP_MAX_VOUT_NUM > 1U)
        .Vout = VOUT_IDX_B,
        #else
        .Vout = VOUT_IDX_A,
        #endif
        .MenuOn = 1U,
        .MenuIconCount = SVC_MENU_TASK_ICON_SUR_NUM,
        .MenuArea = { 0U } /* TBD */,
        .FuncMenuIcon = {
            [SVC_MENU_TASK_ICON_SUR_LIVEVIEW]      = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_liveview_0.bmp",       "icon_liveview_1.bmp",       { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_SUR_RECORD]        = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_record_0.bmp",         "icon_record_0.bmp",         { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_SUR_PLAYBACK]      = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_playback_0.bmp",       "icon_playback_0.bmp",       { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_SUR_PSD]           = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_psd_0.bmp",            "icon_psd_0.bmp",            { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_SUR_OWS]           = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_ows_0.bmp",            "icon_ows_0.bmp",            { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_SUR_RCTA]          = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_rcta_0.bmp",           "icon_rcta_0.bmp",           { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [6U  /* Reserved and misra-c */]       = { { 0U, 0U, 0U, 0U } /* TBD */, "",                          "",                          { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [7U  /* Reserved and misra-c */]       = { { 0U, 0U, 0U, 0U } /* TBD */, "",                          "",                          { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [8U  /* Reserved and misra-c */]       = { { 0U, 0U, 0U, 0U } /* TBD */, "",                          "",                          { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [9U  /* Reserved and misra-c */]       = { { 0U, 0U, 0U, 0U } /* TBD */, "",                          "",                          { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [10U /* Reserved and misra-c */]       = { { 0U, 0U, 0U, 0U } /* TBD */, "",                          "",                          { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [11U /* Reserved and misra-c */]       = { { 0U, 0U, 0U, 0U } /* TBD */, "",                          "",                          { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [12U /* Reserved and misra-c */]       = { { 0U, 0U, 0U, 0U } /* TBD */, "",                          "",                          { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
        },
    },

    [SVC_MENU_TASK_TYPE_PLAYBACK] = {   /* For playback */
        #if (AMBA_DSP_MAX_VOUT_NUM > 1U)
        .Vout = VOUT_IDX_B,
        #else
        .Vout = VOUT_IDX_A,
        #endif
        .MenuOn = 1U,
        .MenuIconCount = SVC_MENU_TASK_ICON_PLAYBACK_NUM,
        .MenuArea = { 0U } /* TBD */,
        .FuncMenuIcon = {
            [SVC_MENU_TASK_ICON_PLAYBACK]           = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_playback_0.bmp", "icon_playback_1.bmp", { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_PLAYBACK_PREV]      = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_prev_0.bmp",     "icon_prev_1.bmp",     { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_PLAYBACK_NEXT]      = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_next_0.bmp",     "icon_next_1.bmp",     { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_PLAYBACK_PLAY]      = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_play_0.bmp",     "icon_play_1.bmp",     { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_PLAYBACK_STOP]      = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_stop_0.bmp",     "icon_stop_1.bmp",     { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_PLAYBACK_BACKWARD]  = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_backward_0.bmp", "icon_backward_1.bmp", { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [SVC_MENU_TASK_ICON_PLAYBACK_FORWARD]   = { { 0U, 0U, 0U, 0U } /* TBD */, "icon_forward_0.bmp",  "icon_forward_1.bmp",  { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [7U  /* Reserved and misra-c */]        = { { 0U, 0U, 0U, 0U } /* TBD */, "",                    "",                    { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [8U  /* Reserved and misra-c */]        = { { 0U, 0U, 0U, 0U } /* TBD */, "",                    "",                    { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [9U  /* Reserved and misra-c */]        = { { 0U, 0U, 0U, 0U } /* TBD */, "",                    "",                    { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [10U /* Reserved and misra-c */]        = { { 0U, 0U, 0U, 0U } /* TBD */, "",                    "",                    { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [11U /* Reserved and misra-c */]        = { { 0U, 0U, 0U, 0U } /* TBD */, "",                    "",                    { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
            [12U /* Reserved and misra-c */]        = { { 0U, 0U, 0U, 0U } /* TBD */, "",                    "",                    { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, { {5U,0U,0U,SVC_MENU_TASK_ICON_W,SVC_MENU_TASK_ICON_H}, (SVC_MENU_TASK_ICON_W*SVC_MENU_TASK_ICON_H)<<2, NULL }, 0 },
        },
    },
};

/* internal function */
static void   MenuTask_FuncMenu(UINT32 OnOff);
static void   MenuTask_FuncMenuMove(void);

/* Callback for SvcGui */
static void   MenuTask_Draw(UINT32 VoutIdx, UINT32 Level);
static void   MenuTask_DrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);
/* Callback for SvcButtonCtrl */
static UINT32 MenuTask_Minus(void); /* Operation when button1 (-) is pressed */
static UINT32 MenuTask_Plus(void);  /* Operation when button2 (+) is pressed */
static UINT32 MenuTask_Move(void);  /* Operation when button3 (!) is pressed */
static UINT32 MenuTask_Enter(void); /* Operation when button4 (o) is pressed */

static void   MenuTask_StatusCallback(UINT32 StatIdx, void *pInfo);

#define SVC_LOG_MENU_TASK       "MENU_TASK"

#define MENU_GUI_LEVEL      (16U)

static inline void MENU_NG( const char *pFormat, UINT32 Arg1, UINT32 Arg2) { SvcLog_NG( SVC_LOG_MENU_TASK, pFormat, Arg1, Arg2); }
static inline void MENU_DBG(const char *pFormat, UINT32 Arg1, UINT32 Arg2) { SvcLog_DBG(SVC_LOG_MENU_TASK, pFormat, Arg1, Arg2); }

static SVC_MENU_TASK_ICON_s FuncIcon[SVC_MENU_TASK_ICON_NUM];
static SVC_MENU_TASK_MENU_CTRL_s MenuController = {0};
static UINT32 VoutOfMenu = VOUT_IDX_A;
static SVC_MENU_BUFFER_s MenuBuf GNU_SECTION_NOZEROINIT;

#define SVC_MENU_TASK_INIT_DONE     (0x1U)
#define SVC_MENU_TASK_LOAD_DONE     (0x2U)
static AMBA_KAL_EVENT_FLAG_t MenuFlag GNU_SECTION_NOZEROINIT;

/**
 * Init Menu Task, creating FuncMenu
 * return 0-OK, 1-NG
 */
UINT32 SvcMenuTask_Init(void)
{
    static UINT8 SvcMenuInit = 0U;
    static char SvcMenuFlagName[] = "SvcMenuTask";
    UINT32 RetVal = SVC_OK;

    if (SvcMenuInit == 0U) {
        ULONG  MemBase;
        UINT32 MemSize = 0U;

        SvcMenuInit = 1U;

        RetVal = AmbaWrap_memset(&MenuFlag, 0, sizeof(MenuFlag));
        if (SVC_OK != RetVal) {
            MENU_NG("SvcMenuTask_Init failed. AmbaWrap_memset err %d AMBA_KAL_EVENT_FLAG_t", RetVal, 0U);
        }
        RetVal = AmbaKAL_EventFlagCreate(&MenuFlag, SvcMenuFlagName);
        if (RetVal != 0U) {
            MENU_NG("SvcMenuTask_Init failed. create flag fail. err %d", RetVal, 0U);
        } else {
            RetVal = AmbaKAL_EventFlagClear(&MenuFlag, 0xFFFFFFFFU);
            if (RetVal != 0U) {
                MENU_NG("SvcMenuTask_Init failed. clear flag fail. err %d", RetVal, 0U);
            }
        }

        if (SVC_OK == RetVal) {
            RetVal = AmbaWrap_memset(&MenuController, 0, sizeof(SVC_MENU_TASK_MENU_CTRL_s));
            if (SVC_OK != RetVal) {
                MENU_NG("SvcMenuTask_Init failed. AmbaWrap_memset err %d SVC_MENU_TASK_MENU_CTRL_s", RetVal, 0U);
            }

            RetVal = AmbaWrap_memset(&MenuBuf, 0, sizeof(SVC_MENU_BUFFER_s));
            if (SVC_OK != RetVal) {
                MENU_NG("SvcMenuTask_Init failed. AmbaWrap_memset err %d SVC_MENU_BUFFER_s", RetVal, 0U);
            }
        }

        if (SVC_OK == RetVal) {
            RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_MENU_BMP, &MemBase, &MemSize);
            if (SVC_OK == RetVal) {
                MenuBuf.Size = MemSize;
                AmbaMisra_TypeCast(&MenuBuf.pBuf, &MemBase);
                if (NULL == MenuBuf.pBuf) {
                    MENU_NG("SvcMenuTask_Init failed. MenuBuf.pBuf == NULL", 0U, 0U);
                    RetVal = SVC_NG;
                }
            }
        }

        if (SVC_OK == RetVal) {
            RetVal = AmbaKAL_EventFlagSet(&MenuFlag, SVC_MENU_TASK_INIT_DONE);
            if (RetVal != 0U) {
                MENU_DBG("SvcMenuTask_Init failed. set 'init done' flag fail", 0U, 0U);
            }

            MenuController.FuncMenuCount = 0U;
        }
    }

    return RetVal;
}

/**
 * Choose the menu type to be shown
 * @param [in] 0 for Liveview mode, 1 for playback mode
 * return 0-OK, 1-NG
 */
UINT32 SvcMenuTask_Config(UINT32 MenuType)
{
    UINT32 RetVal;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 ActualFlags = 0U;

    /* force to re-init */
    RetVal = SvcMenuTask_Init();
    if (SVC_OK != RetVal) {
        MENU_NG("SvcMenuTask_Config() failed. SvcMenuTask_Init err %d", RetVal, 0U);
    } else {
        RetVal = AmbaKAL_EventFlagGet(&MenuFlag, SVC_MENU_TASK_INIT_DONE, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, 10000U);
        if ((RetVal != 0U) || (ActualFlags == 0U)) {
            MENU_NG("SvcMenuTask_Config failed. wait init timeout! err: %d", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        static SVC_BUTTON_CTRL_ID MenuButtonCtrl_ID = 0U;
        /* Callback for ButtonControl */
        static SVC_BUTTON_CTRL_LIST_s ButtonCtrl[4] = {
            { "button_1", MenuTask_Minus },
            { "button_2", MenuTask_Plus  },
            { "button_3", MenuTask_Move  },
            { "button_4", MenuTask_Enter },
        };

        RetVal = SvcButtonCtrl_Register(SVC_BUTTON_TYPE_GPIO, ButtonCtrl, 4, &MenuButtonCtrl_ID);
        if (SVC_OK == RetVal) {
            RetVal = SvcButtonCtrl_Request(&MenuButtonCtrl_ID);
            if (SVC_OK != RetVal) {
                MENU_NG("SvcButtonCtrl_Request failed", 0U, 0U);
            }
        } else {
            MENU_NG("SvcButtonCtrl_Register failed", 0U, 0U);
        }
    }

    if ((SVC_OK == RetVal) && (MenuType < SVC_MENU_TASK_TYPE_NUM)) {
        UINT32 Offset = 0U, AddrShift = 0U;

        MenuController.MenuType = (UINT8) MenuType;

        for (UINT32 Idx = 0U; Idx < MenuCfg[MenuType].MenuIconCount; Idx++) {
            /* Assign buffer address */
            MenuCfg[MenuType].FuncMenuIcon[Idx].IconInfo.data = &MenuBuf.pBuf[Offset];

            /* Update offset */
            AddrShift = (UINT32) sizeof(UINT8) * SVC_MENU_TASK_ICON_W * SVC_MENU_TASK_ICON_H;
            AddrShift = AddrShift << SVC_OSD_PIXEL_SIZE_SFT;
            Offset = Offset + AddrShift;

            /* Assign buffer address */
            MenuCfg[MenuType].FuncMenuIcon[Idx].SelIconInfo.data = &MenuBuf.pBuf[Offset];

            /* Update offset */
            AddrShift = (UINT32) sizeof(UINT8) * SVC_MENU_TASK_ICON_W * SVC_MENU_TASK_ICON_H;
            AddrShift = AddrShift << SVC_OSD_PIXEL_SIZE_SFT;
            Offset = Offset + AddrShift;
        }

        /* Determine which vout shows menu */
        VoutOfMenu = MenuCfg[MenuType].Vout;

        for (i = 0; i < pResCfg->DispNum; i++) {
            if ((pResCfg->DispBits & ((UINT32) 1U << i)) > 0U) {
                #if defined(CONFIG_ICAM_HDMI_EXIST)
                if (pResCfg->DispStrm[i].VoutID == VOUT_IDX_B) {
                    if (pResCfg->DispStrm[i].pDriver == &AmbaFPD_HDMI_Obj) {
                        VoutOfMenu = VOUT_IDX_B;
                    }
                }
                #endif
            }
        }

        /* Menu on by default */
        MenuController.FuncMenuOn = MenuCfg[MenuType].MenuOn;

        RetVal = AmbaWrap_memcpy(&FuncIcon, MenuCfg[MenuType].FuncMenuIcon, sizeof(FuncIcon));
        /* Determine the dimension of menu and icon */
        if (SVC_OK == RetVal) {
            UINT32 VoutOsdWidth, VoutOsdHeight;
            UINT32 StartX, StartY, Width, Height;

            RetVal = SvcOsd_GetOsdBufSize(VoutOfMenu, &VoutOsdWidth, &VoutOsdHeight);
            if (SVC_OK == RetVal) {
                if (MenuType == SVC_MENU_TASK_TYPE_LV_ADAS_DVR) {
                    StartX = 0U;
                } else {
                    StartX = 60U;
                }
                StartY = VoutOsdHeight - SVC_MENU_TASK_ICON_H;
                Width  = SVC_MENU_TASK_ICON_W * SVC_MENU_TASK_ICON_NUM;
                Height = SVC_MENU_TASK_ICON_H;

                for (i = 0; i < SVC_MENU_TASK_ICON_NUM; i++) {
                    FuncIcon[i].MenuArea.StartX = StartX + (i * SVC_MENU_TASK_ICON_W);
                    FuncIcon[i].MenuArea.StartY = StartY;
                    FuncIcon[i].MenuArea.EndX = FuncIcon[i].MenuArea.StartX + SVC_MENU_TASK_ICON_W;
                    FuncIcon[i].MenuArea.EndY = StartY + SVC_MENU_TASK_ICON_H;
                }
                AmbaMisra_TouchUnused(&Width);
                AmbaMisra_TouchUnused(&Height);
            }
        }

        /* Unregister first */
        SvcGui_Unregister(VoutOfMenu, MENU_GUI_LEVEL);

        /* Register necessary callback */
        {
            SVC_SYS_STAT_ID StatusID = 0U;
            RetVal = SvcSysStat_Register(SVC_APP_STAT_BSD, MenuTask_StatusCallback, &StatusID);
            if (SVC_OK != RetVal) {
                MENU_NG("Register Bsd Status failed", 0U, 0U);
            }

            RetVal = SvcSysStat_Register(SVC_APP_STAT_OD, MenuTask_StatusCallback, &StatusID);
            if (SVC_OK != RetVal) {
                MENU_NG("Register OD Status failed", 0U, 0U);
            }

            RetVal = SvcSysStat_Register(SVC_APP_STAT_DIRT_DETECT, MenuTask_StatusCallback, &StatusID);
            if (SVC_OK != RetVal) {
                MENU_NG("Register DirtDetect Status failed", 0U, 0U);
            }

            RetVal = SvcSysStat_Register(SVC_APP_STAT_DEFOG, MenuTask_StatusCallback, &StatusID);
            if (SVC_OK != RetVal) {
                MENU_NG("Register Defog status failed", 0U, 0U);
            }

            RetVal = SvcSysStat_Register(SVC_APP_STAT_PSD, MenuTask_StatusCallback, &StatusID);
            if (SVC_OK != RetVal) {
                MENU_NG("Register PSD status failed", 0U, 0U);
            }

            RetVal = SvcSysStat_Register(SVC_APP_STAT_OWS, MenuTask_StatusCallback, &StatusID);
            if (SVC_OK != RetVal) {
                MENU_NG("Register OWS status failed", 0U, 0U);
            }

            RetVal = SvcSysStat_Register(SVC_APP_STAT_RCTA, MenuTask_StatusCallback, &StatusID);
            if (SVC_OK != RetVal) {
                MENU_NG("Register RCTA status failed", 0U, 0U);
            }

            RetVal = SvcSysStat_Register(SVC_APP_STAT_DISP, MenuTask_StatusCallback, &StatusID);
            if (SVC_OK != RetVal) {
                MENU_NG("Register DISP status failed", 0U, 0U);
            }

            {
                SVC_APP_STAT_EMR_VER_s Version;
                RetVal = SvcSysStat_Get(SVC_APP_STAT_EMR_VER, &Version);
                if (SVC_OK == RetVal) {
                    if (Version.Status == SVC_APP_STAT_EMR_VER_2_0) {
                        FuncIcon[SVC_MENU_TASK_ICON_EMR_EMR2_0].IsPressed = 1U;
                    }
                }
            }
        }

    } else {
        MenuController.MenuValid &= ~(SVC_MENU_TASK_GUI_VALID);
    }

    return RetVal;
}

/**
 * Trigger and wait menu task loading bmp done
 * return N
 */


static UINT32 MenuTask_Load(void)
{
    UINT32 RetVal = SVC_OK, i;

    for (i = 0; i < MenuCfg[MenuController.MenuType].MenuIconCount; i++) {
        UINT32 BufSize = (FuncIcon[i].MenuArea.EndX - FuncIcon[i].MenuArea.StartX) * (FuncIcon[i].MenuArea.EndY - FuncIcon[i].MenuArea.StartY);
        if ((BufSize == 0U) || (FuncIcon[i].IconInfo.data == NULL) || (FuncIcon[i].SelIconInfo.data == NULL)) {
            MENU_DBG("SvcMenuTask_Config() FumcMenu Bmp will not load", 0U, 0U);
            RetVal = SVC_OK;
        } else {
            MenuController.FuncMenuCount = MenuController.FuncMenuCount + 1U;
            RetVal = SvcOsd_LoadBmp(FuncIcon[i].IconInfo.data, (BufSize << SVC_OSD_PIXEL_SIZE_SFT), FuncIcon[i].IconName);
            if (SVC_OK != RetVal) {
                MENU_DBG("SvcMenuTask_Config failed. FuncMenu Bmp (%d) load err %d", i, RetVal);
            } else {
                RetVal = SvcOsd_LoadBmp(FuncIcon[i].SelIconInfo.data, (BufSize << SVC_OSD_PIXEL_SIZE_SFT), FuncIcon[i].SelIconName);
                if (SVC_OK != RetVal) {
                    MENU_DBG("SvcMenuTask_Config failed. FuncMenu Sel Bmp (%d) load err %d", i, RetVal);
                }
            }
        }

        if (SVC_OK != RetVal) {
            break;
        }
    }

    if (0U != AmbaKAL_EventFlagSet(&MenuFlag, SVC_MENU_TASK_LOAD_DONE)) {
        MENU_DBG("SvcMenuTask_Config failed. set 'loading done' flag fail", 0U, 0U);
    }

    if (SVC_OK == RetVal) {
        MenuController.MenuValid |= SVC_MENU_TASK_GUI_VALID;
        SvcGui_Register(VoutOfMenu, MENU_GUI_LEVEL, "Menu", MenuTask_Draw, MenuTask_DrawUpdate);
    } else {
        MenuController.MenuValid &= ~(SVC_MENU_TASK_GUI_VALID);
    }

    return RetVal;
}

UINT32 SvcMenuTask_Load(void)
{
    #define SVC_MENU_LOAD_TMO   (20000U)

    UINT32 RetVal;
    
    RetVal = SvcDataLoader_LoadCallback(MenuTask_Load);
    if (SVC_OK == RetVal) {
        UINT32 ActualFlags = 0U;

        RetVal = AmbaKAL_EventFlagGet(&MenuFlag,
                                      SVC_MENU_TASK_LOAD_DONE,
                                      AMBA_KAL_FLAGS_ALL,
                                      AMBA_KAL_FLAGS_CLEAR_NONE,
                                      &ActualFlags,
                                      SVC_MENU_LOAD_TMO);
        if ((RetVal != 0U) || (ActualFlags == 0U)) {
            MENU_NG("SvcMenuTask_Load failed. wait loading timeout! err: %d", RetVal, 0U);
        }
    } else {
        MENU_NG("SvcMenuTask_Load failed. SvcDataLoader_LoadCallback failed! err: %d", RetVal, 0U);
    }
    return RetVal;
}

/**
 * Providing the working size that menu moduled needed
 * @param [out] the working buffer size
 * return 0-OK, 1-NG
 */
UINT32 SvcMenuTask_MemSizeQuery(UINT32 *pWorkSize)
{
    *pWorkSize = ((UINT32) sizeof(UINT8) * SVC_MENU_TASK_ICON_NUM * SVC_MENU_TASK_ICON_W * SVC_MENU_TASK_ICON_H * 2U) << SVC_OSD_PIXEL_SIZE_SFT;
    return SVC_OK;
}

/**
 * Contorl the function menu to be on or off
 * @param [in] 0 for off, 1 for on
 * return none
 */
static void MenuTask_FuncMenu(UINT32 OnOff)
{
    if (OnOff == 0U) {
        MenuController.FuncMenuOn = 0U;
    } else {
        MenuController.FuncMenuOn = 1U;
    }
}

static void MenuTask_FuncMenuMove(void)
{
    MenuController.FuncMenuIdx = (MenuController.FuncMenuIdx + 1U) % MenuController.FuncMenuCount;

    if (MenuController.FuncMenuIdx == 0U) {
        MenuController.FuncMenuOn = 0U;
    }
}

static void MenuTask_Draw(UINT32 VoutIdx, UINT32 Level)
{
    UINT32 RetVal = SVC_OK;
    UINT32 FuncMenuOn  = MenuController.FuncMenuOn;
    UINT32 FuncMenuIdx = MenuController.FuncMenuIdx;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    char FovShow[8] = "Camera1\0";

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if ((MenuController.MenuValid & SVC_MENU_TASK_GUI_VALID) > 0U) {
        if (FuncMenuOn > 0U) {
            /* Draw func menu icon */
            for (i = 0U; i < MenuController.FuncMenuCount; i++) {
                if ((FuncIcon[i].IconInfo.data == NULL) || (FuncIcon[i].SelIconInfo.data == NULL)) {
                    continue;
                }
                if (i != FuncMenuIdx) {

                    //RetVal = SvcOsd_DrawBmp(VoutOfMenu, FuncIcon[i].MenuArea.StartX, FuncIcon[i].MenuArea.StartY, FuncIcon[i].MenuArea.EndX, FuncIcon[i].MenuArea.EndY, FuncIcon[i].pIcon);
                    RetVal = SvcOsd_DrawBmp(VoutOfMenu, FuncIcon[i].MenuArea.StartX, FuncIcon[i].MenuArea.StartY, FuncIcon[i].MenuArea.EndX, FuncIcon[i].MenuArea.EndY, &FuncIcon[i].IconInfo);

                } else {
           
                    //RetVal = SvcOsd_DrawBmp(VoutOfMenu, FuncIcon[i].MenuArea.StartX, FuncIcon[i].MenuArea.StartY, FuncIcon[i].MenuArea.EndX, FuncIcon[i].MenuArea.EndY, FuncIcon[i].pSelIcon);
                    RetVal = SvcOsd_DrawBmp(VoutOfMenu, FuncIcon[i].MenuArea.StartX, FuncIcon[i].MenuArea.StartY, FuncIcon[i].MenuArea.EndX, FuncIcon[i].MenuArea.EndY, &FuncIcon[i].SelIconInfo);

                }

                if (FuncIcon[i].IsPressed == 1U) {
#ifdef CONFIG_ICAM_32BITS_OSD_USED
                    RetVal = SvcOsd_DrawSolidRect(VoutOfMenu, FuncIcon[i].MenuArea.StartX, FuncIcon[i].MenuArea.EndY - 5U, FuncIcon[i].MenuArea.EndX, FuncIcon[i].MenuArea.EndY ,0x80e04000U);
#else
                    RetVal = SvcOsd_DrawSolidRect(VoutOfMenu, FuncIcon[i].MenuArea.StartX, FuncIcon[i].MenuArea.EndY - 5U, FuncIcon[i].MenuArea.EndX, FuncIcon[i].MenuArea.EndY ,23U);
#endif
                }

                if (SVC_OK != RetVal) {
                    MENU_NG("Draw func menu icon err %d", RetVal, 0U);
                }
            }
        }
        if (pResCfg->DispAltNum > 0U){
            FovShow[6] = (pResCfg->DispAltIdx +1U) + '0';
            RetVal = SvcOsd_DrawString(VoutOfMenu, 40U, 10U, 3U, 0xffffffffU, FovShow);
            if (RetVal != SVC_OK){
                MENU_NG("Fail to draw Camera Number on Vout [%u]", VoutOfMenu, 0U);
            }
        }

    }
}

static void MenuTask_DrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if ((MenuController.MenuValid & SVC_MENU_TASK_GUI_UPDATE) > 0U) {
        MenuController.MenuValid &= ~(SVC_MENU_TASK_GUI_UPDATE);
        *pUpdate = 1U;
    } else {
        *pUpdate = 0U;
    }
}

static UINT32 MenuTask_Minus(void)
{
    UINT32 RetVal = SVC_OK;
    SVC_APP_STAT_MENU_s MenuStatus = {0};

    MenuStatus.Vout      = (UINT8) VoutOfMenu;
    /* Menu type is mapping to definition in SvcAppStat.h */
    MenuStatus.Type      = MenuController.MenuType;
    MenuStatus.Operation = SVC_APP_STAT_MENU_OP_MINUS;
    /* Menu FuncIdx is mapping to definition in SvcAppStat.h */
    MenuStatus.FuncIdx   = MenuController.FuncMenuIdx;

    RetVal = SvcSysStat_Issue(SVC_APP_STAT_MENU, &MenuStatus);
    if (SVC_OK != RetVal) {
        MENU_NG("Sys Status issue failed", 0U, 0U);
    }

    return SVC_OK;
}

static UINT32 MenuTask_Plus(void)
{
    UINT32 RetVal = SVC_OK;
    SVC_APP_STAT_MENU_s MenuStatus = {0};

    MenuStatus.Vout      = (UINT8) VoutOfMenu;
    /* Menu type is mapping to definition in SvcAppStat.h */
    MenuStatus.Type      = MenuController.MenuType;
    MenuStatus.Operation = SVC_APP_STAT_MENU_OP_PLUS;
    /* Menu FuncIdx is mapping to definition in SvcAppStat.h */
    MenuStatus.FuncIdx   = MenuController.FuncMenuIdx;

    RetVal = SvcSysStat_Issue(SVC_APP_STAT_MENU, &MenuStatus);
    if (SVC_OK != RetVal) {
        MENU_NG("Sys Status issue failed", 0U, 0U);
    }

    return SVC_OK;
}

static UINT32 MenuTask_Move(void)
{
    if (MenuController.FuncMenuOn == 0U) {
        MenuTask_FuncMenu(1U);
    } else {
        MenuTask_FuncMenuMove();
    }

    MenuController.MenuValid |= SVC_MENU_TASK_GUI_UPDATE;

    return SVC_OK;
}

static UINT32 MenuTask_Enter(void)
{
    UINT32 RetVal = SVC_OK;
    SVC_APP_STAT_MENU_s MenuStatus = {0};

    MenuStatus.Vout      = (UINT8) VoutOfMenu;
    /* Menu type is mapping to definition in SvcAppStat.h */
    MenuStatus.Type      = MenuController.MenuType;
    MenuStatus.Operation = SVC_APP_STAT_MENU_OP_ENTER;
    /* Menu FuncIdx is mapping to definition in SvcAppStat.h */
    MenuStatus.FuncIdx   = MenuController.FuncMenuIdx;

    RetVal = SvcSysStat_Issue(SVC_APP_STAT_MENU, &MenuStatus);
    if (SVC_OK != RetVal) {
        MENU_NG("Sys Status issue failed", 0U, 0U);
    }

    return SVC_OK;
}

static void MenuTask_StatusCallback(UINT32 StatIdx, void *pInfo)
{
    SVC_APP_STAT_BSD_s *pBsdStatus = NULL;
    SVC_APP_STAT_OD_s  *pOdStatus = NULL;
    SVC_APP_STAT_DIRT_DETECT_s *pDirtDetectStatus = NULL;
    SVC_APP_STAT_PSD_s *pPsdStatus = NULL;
    SVC_APP_STAT_OWS_s *pOwsStatus = NULL;
    SVC_APP_STAT_RCTA_s *pRctaStatus = NULL;
    SVC_APP_STAT_DEFOG_s *pDefogStatus = NULL;
    SVC_APP_STAT_DISP_s  *pDispStatus = NULL;
    UINT32 MenuUpdate = 1U;

    AmbaMisra_TouchUnused(pInfo);
    AmbaMisra_TouchUnused(pBsdStatus);
    AmbaMisra_TouchUnused(pOdStatus);
    AmbaMisra_TouchUnused(pDirtDetectStatus);
    AmbaMisra_TouchUnused(pPsdStatus);
    AmbaMisra_TouchUnused(pOwsStatus);
    AmbaMisra_TouchUnused(pRctaStatus);
    AmbaMisra_TouchUnused(pDefogStatus);
    AmbaMisra_TouchUnused(pDispStatus);

    if (MenuController.MenuType == SVC_MENU_TASK_TYPE_LV_EMR) {
        switch (StatIdx) {
            case SVC_APP_STAT_BSD:
                AmbaMisra_TypeCast(&pBsdStatus, &pInfo);
                if (pBsdStatus->Status == SVC_APP_STAT_BSD_ENABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_EMR_BSD].IsPressed = 1U;
                } else if (pBsdStatus->Status == SVC_APP_STAT_BSD_DISABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_EMR_BSD].IsPressed = 0U;
                } else {
                    MENU_DBG("Unknwon Bsd Status", 0U, 0U);
                }
            break;
            case SVC_APP_STAT_OD:
                AmbaMisra_TypeCast(&pOdStatus, &pInfo);
                if (pOdStatus->Status == SVC_APP_STAT_OD_ENABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_EMR_OD].IsPressed = 1U;
                } else if (pOdStatus->Status == SVC_APP_STAT_OD_DISABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_EMR_OD].IsPressed = 0U;
                } else {
                    MENU_DBG("Unknwon OD Status", 0U, 0U);
                }
            break;
            case SVC_APP_STAT_DIRT_DETECT:
                AmbaMisra_TypeCast(&pDirtDetectStatus, &pInfo);
                if (pDirtDetectStatus->Status == SVC_APP_STAT_DIRT_DETECT_ENABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_EMR_DIRT_DETECT].IsPressed = 1U;
                } else if (pDirtDetectStatus->Status == SVC_APP_STAT_DIRT_DETECT_DISABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_EMR_DIRT_DETECT].IsPressed = 0U;
                } else {
                    MENU_DBG("Unknwon DirtDetect Status", 0U, 0U);
                }
            break;
            case SVC_APP_STAT_DEFOG:
                AmbaMisra_TypeCast(&pDefogStatus, &pInfo);
                if (pDefogStatus->Status == SVC_APP_STAT_DEFOG_ENABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_EMR_DEFOG].IsPressed = 1U;
                } else if (pDefogStatus->Status == SVC_APP_STAT_DEFOG_DISABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_EMR_DEFOG].IsPressed = 0U;
                } else {
                    MENU_DBG("Unknwon Defog Status", 0U, 0U);
                }
            break;
            default:
                MenuUpdate = 0U;
            break;
        }
    } else if (MenuController.MenuType == SVC_MENU_TASK_TYPE_LV_ADAS_DVR) {
        switch (StatIdx) {
            case SVC_APP_STAT_OD:
                AmbaMisra_TypeCast(&pOdStatus, &pInfo);
                if (pOdStatus->Status == SVC_APP_STAT_OD_ENABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_ADAS_DVR_OD].IsPressed = 1U;
                } else if (pOdStatus->Status == SVC_APP_STAT_OD_DISABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_ADAS_DVR_OD].IsPressed = 0U;
                } else {
                    MENU_DBG("Unknwon OD Status", 0U, 0U);
                }
            break;
            case SVC_APP_STAT_DIRT_DETECT:
                AmbaMisra_TypeCast(&pDirtDetectStatus, &pInfo);
                if (pDirtDetectStatus->Status == SVC_APP_STAT_DIRT_DETECT_ENABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_ADAS_DVR_DIRT_DETECT].IsPressed = 1U;
                } else if (pDirtDetectStatus->Status == SVC_APP_STAT_DIRT_DETECT_DISABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_ADAS_DVR_DIRT_DETECT].IsPressed = 0U;
                } else {
                    MENU_DBG("Unknwon DirtDetect Status", 0U, 0U);
                }
            break;
            case SVC_APP_STAT_DEFOG:
                AmbaMisra_TypeCast(&pDefogStatus, &pInfo);
                if (pDefogStatus->Status == SVC_APP_STAT_DEFOG_ENABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_ADAS_DVR_DEFOG].IsPressed = 1U;
                } else if (pDefogStatus->Status == SVC_APP_STAT_DEFOG_DISABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_ADAS_DVR_DEFOG].IsPressed = 0U;
                } else {
                    MENU_DBG("Unknwon Defog Status", 0U, 0U);
                }
            break;
            case SVC_APP_STAT_DISP:
                AmbaMisra_TypeCast(&pDispStatus, &pInfo);
                if (pDispStatus->Status == SVC_APP_STAT_DISP_CHG) {
                    MenuController.MenuValid |= SVC_MENU_TASK_GUI_UPDATE;
                    MENU_DBG("Display status changed. Menu will re-draw.", 0U, 0U);
                } else {
                    /* Do nothing now */
                }
            break;                
            default:
                MenuUpdate = 0U;
            break;
        }
    } else if (MenuController.MenuType == SVC_MENU_TASK_TYPE_SURROUND) {
        switch (StatIdx) {
            case SVC_APP_STAT_PSD:
                AmbaMisra_TypeCast(&pPsdStatus, &pInfo);
                if (pPsdStatus->Status == SVC_APP_STAT_PSD_ENABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_SUR_PSD].IsPressed = 1U;
                } else if (pPsdStatus->Status == SVC_APP_STAT_PSD_DISABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_SUR_PSD].IsPressed = 0U;
                } else {
                    MENU_DBG("Unknwon PSD Status", 0U, 0U);
                }
            break;
            case SVC_APP_STAT_OWS:
                AmbaMisra_TypeCast(&pOwsStatus, &pInfo);
                if (pOwsStatus->Status == SVC_APP_STAT_OWS_ENABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_SUR_OWS].IsPressed = 1U;
                } else if (pOwsStatus->Status == SVC_APP_STAT_OWS_DISABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_SUR_OWS].IsPressed = 0U;
                } else {
                    MENU_DBG("Unknwon OWS Status", 0U, 0U);
                }
            break;
            case SVC_APP_STAT_RCTA:
                AmbaMisra_TypeCast(&pRctaStatus, &pInfo);
                if (pRctaStatus->Status == SVC_APP_STAT_RCTA_ENABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_SUR_RCTA].IsPressed = 1U;
                } else if (pRctaStatus->Status == SVC_APP_STAT_RCTA_DISABLE) {
                    FuncIcon[SVC_MENU_TASK_ICON_SUR_RCTA].IsPressed = 0U;
                } else {
                    MENU_DBG("Unknwon RCTA Status", 0U, 0U);
                }
            break;
            default:
                MenuUpdate = 0U;
            break;
        }
    } else if (MenuController.MenuType == SVC_MENU_TASK_TYPE_LV_DMS) {
        /* Do nothing now */
//        switch (StatIdx) {
//            default:
//                MENU_NG("Unknown status received.", 0U, 0U);
//            break;
//        }
    } else if (MenuController.MenuType == SVC_MENU_TASK_TYPE_PLAYBACK) {
        /* Do nothing now */
    } else {
        /* Do nothing */
    }

    if (MenuUpdate == 1U){
        MenuController.MenuValid |= SVC_MENU_TASK_GUI_UPDATE;
    }
}
