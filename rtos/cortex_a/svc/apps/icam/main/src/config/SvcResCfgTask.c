/**
 *  @file SvcResCfgTask.c
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
 *  @details svc res config task
 *
 */

#include ".svc_autogen"

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaFPD.h"
#include "AmbaUtility.h"
#include "AmbaSYS.h"
#include "AmbaPrint.h"

/* ssp */
#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VOUT.h"

/* framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcPref.h"
/* app-shared */
#include "SvcResCfg.h"
#include "SvcSysStat.h"
#include "SvcButtonCtrl.h"
#include "SvcGui.h"
#include "SvcGuiCheckBox.h"
#include "SvcGuiTextView.h"
#include "SvcOsd.h"
#include "SvcFlowControl.h"
/* app-icam */
#include "SvcUserPref.h"
#include "SvcAppStat.h"
#include "SvcControlTask.h"

#include "SvcResCfgTask.h"

#ifdef SVC_ICAM_PROJECT_CFG
#include SVC_ICAM_PROJECT_CFG
#else
#include "Res_Default.c"
#endif

#ifdef CONFIG_ICAM_THMVIEW_IN_DPX
#ifdef SVC_ICAM_PROJECT_PBK_CFG
#include SVC_ICAM_PROJECT_PBK_CFG
#endif
#endif

#define SVC_LOG_RES_CFG_TASK "RESCFG_TASK"

#define SVC_RES_CFG_TASK_CATEGORY_MAX       (10U)
#define SVC_RES_CFG_TASK_CATEGORY_ID_MAX    (64U)

#define SVC_RES_CFG_TASK_FMT_CNT            (64U)

#define SVC_RES_CFG_TASK_GUI_NO             (0U)
#define SVC_RES_CFG_TASK_GUI_SHOW           (1U)
#define SVC_RES_CFG_TASK_GUI_UPDATE         (2U)

typedef struct {
    char  Name[32];
    UINT8 Idxs[SVC_RES_CFG_TASK_CATEGORY_ID_MAX];
    UINT8 Count;
} SVC_RES_CFG_TASK_CATEGORY_s;

typedef struct {
    UINT8 GuiToFmt[SVC_RES_CFG_TASK_FMT_CNT];
    UINT8 FmtToGui[SVC_RES_CFG_TASK_FMT_CNT];
} SVC_RES_CFG_TASK_IDX_MAP;

static UINT32 ResCfgTaskFormatId = 0xdeadbeafU;
static UINT32 ResCfgTaskGuiShow = 0U;
static UINT32 ResCfgTaskGuiVout = VOUT_IDX_A;
static SVC_GUI_CANVAS_s   ResCfgTaskCanvas = {0};
static SVC_GUI_CHECKBOX_s ResCfgTaskCheckBox = {
    .CheckBarColor    = 0x80e04000U,
    .SelectedBoxColor = 0x8000a0c0U,
    .BackgroundColor  = 0x80202040U,
    .FontColor        = 0xffffffffU,
    .FontSize         = 2U,
    .RowGap           = 1U,
    .RowNum           = 4U,
    .ListNum          = 0U,
    .pList            = { NULL },
    .CurIdx           = 0U,
    .CheckBits        = 0U,
};

static SVC_RES_CFG_TASK_CATEGORY_s ResCfgTaskCategory[SVC_RES_CFG_TASK_CATEGORY_MAX] GNU_SECTION_NOZEROINIT;
static UINT8                       ResCfgTaskCategoryCount = 0U;
static SVC_RES_CFG_TASK_IDX_MAP    ResCfgTaskIdxMap;
static SVC_RES_CFG_s               ResCfg[SVC_RES_CFG_TASK_FMT_CNT] GNU_SECTION_NOZEROINIT;

#define RES_CFG_TASK_GUI_CHECKBOX_W (600U)
#define RES_CFG_TASK_GUI_CHECKBOX_H (100U)
#define RES_CFG_TASK_GUI_TEXTBOX1_H (15U)
#define RES_CFG_TASK_GUI_TEXTBOX2_H (15U)
#define RES_CFG_TASK_GUI_W          (RES_CFG_TASK_GUI_CHECKBOX_W)
#define RES_CFG_TASK_GUI_H          (RES_CFG_TASK_GUI_CHECKBOX_H + RES_CFG_TASK_GUI_TEXTBOX1_H + RES_CFG_TASK_GUI_TEXTBOX2_H)

static SVC_BUTTON_CTRL_ID ResCfgTaskButtonCtrl_ID = 0U;

/* Menu status callback */
static void ResCfgTask_MenuStatusCallback(UINT32 StatIdx, void *pInfo);
/* Gui Drawing callback */
static void ResCfgTask_DrawEntry(UINT32 VoutIdx, UINT32 Level);
static void ResCfgTask_DrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);
/* Button Ctrl callback */
static UINT32 ResCfgTask_Minus(void);
static UINT32 ResCfgTask_Plus(void);
static UINT32 ResCfgTask_Move(void);

static UINT32 CheckFormatId(UINT32 FormatId)
{
    UINT32 RetVal = SVC_OK;
    UINT32 MaxFormatId = ((UINT32) sizeof(g_ResCfg) / (UINT32) sizeof(SVC_RES_CFG_s)) - 1U;

#ifdef CONFIG_ICAM_THMVIEW_IN_DPX
    MaxFormatId += ((UINT32) sizeof(g_ResPbkCfg) / (UINT32) sizeof(SVC_RES_CFG_s));
#endif

    if (FormatId > MaxFormatId) {
        RetVal = SVC_NG;
    }

    return RetVal;
}

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
void SvcResCfgTask_LoadSetting(void)
{
    extern void app_init_res_cfg(SVC_RES_CFG_s *resCfg);
    app_init_res_cfg((SVC_RES_CFG_s *)g_ResCfg);
}
#endif

/**
 * Resolution configuration initialization
 * @return ErrorCode
 */
UINT32 SvcResCfgTask_Init(void)
{
    UINT32             RetVal;
    UINT32             FormatId;
    SVC_USER_PREF_s    *pSvcUserPref;
    UINT32             FormatCount;
    UINT32             i, j;
    UINT32             CmpResult;

    /* Callback for ButtonControl */
    static SVC_BUTTON_CTRL_LIST_s ResCfgTaskButtonCtrl[3] = {
        { "button_1", ResCfgTask_Minus },
        { "button_2", ResCfgTask_Plus  },
        { "button_3", ResCfgTask_Move  },
    };
    static UINT8 ResCfgTaskFormatIdInCategory = 0U;

    AmbaMisra_TouchUnused(&ResCfgTaskFormatIdInCategory);   /* Misra-c fixed */

    SvcLog_OK(SVC_LOG_RES_CFG_TASK, "@@ SvcResCfgTask_Init() start", 0U, 0U);

    RetVal = AmbaWrap_memset(ResCfgTaskCategory, 0, sizeof(ResCfgTaskCategory));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_RES_CFG_TASK, "ResCfgTaskCategory Memset failed", 0U, 0U);
    }
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    g_RefCfgPreInit();
    SvcResCfgTask_LoadSetting();
#endif

    RetVal = AmbaWrap_memset(ResCfg, 0, sizeof(ResCfg));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_RES_CFG_TASK, "ResCfg Memset failed", 0U, 0U);
    }

    RetVal = AmbaWrap_memcpy(&(ResCfg[0]), g_ResCfg, sizeof(g_ResCfg));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_RES_CFG_TASK, "g_ResCfg Memcpy failed", 0U, 0U);
    }

    /* Get total format count */
    FormatCount = (UINT32) sizeof(g_ResCfg) / (UINT32) sizeof(SVC_RES_CFG_s);

#ifdef CONFIG_ICAM_THMVIEW_IN_DPX
    RetVal = AmbaWrap_memcpy(&(ResCfg[FormatCount]), g_ResPbkCfg, sizeof(g_ResPbkCfg));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_RES_CFG_TASK, "g_ResPbkCfg Memcpy failed", 0U, 0U);
    }
    FormatCount += (UINT32) sizeof(g_ResPbkCfg) / (UINT32) sizeof(SVC_RES_CFG_s);
#endif

    RetVal = SvcUserPref_Get(&pSvcUserPref);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_RES_CFG_TASK, "SvcUserPref_Get failed!!", 0U, 0U);
        FormatId = 0U;
    } else {
        if (SVC_OK != CheckFormatId(pSvcUserPref->FormatId)) {
            pSvcUserPref->FormatId = 0U;
            SvcLog_NG(SVC_LOG_RES_CFG_TASK, "SvcResCfgTask_Config() failed. FormatId err", 0U, 0U);
        }
        SvcLog_OK(SVC_LOG_RES_CFG_TASK, "current format_id %u", pSvcUserPref->FormatId, 0U);
        FormatId = pSvcUserPref->FormatId;
    }

    ResCfgTaskFormatId = FormatId;

    for (i = 0U; i < FormatCount; i++) {
        for (j = 0U; j < ResCfgTaskCategoryCount; j++) {
            if (SVC_OK == AmbaWrap_memcmp(ResCfg[i].GroupName, ResCfgTaskCategory[j].Name, sizeof(char) * 32U, &CmpResult)) {
                /* Two string are the same */
                if (CmpResult == 0U) {
                    UINT8 WrIdx = ResCfgTaskCategory[j].Count;
                    if (WrIdx < SVC_RES_CFG_TASK_CATEGORY_ID_MAX) {
                        ResCfgTaskCategory[j].Idxs[WrIdx] = (UINT8) i;  /* i is Format_id */
                        ResCfgTaskCategory[j].Count++;
                    } else {
                        SvcLog_NG(SVC_LOG_RES_CFG_TASK, "Please increase SVC_RES_CFG_TASK_CATEGORY_ID_MAX (%d)", SVC_RES_CFG_TASK_CATEGORY_ID_MAX, 0U);
                    }
                    break;
                }
            } else {
                SvcLog_NG(SVC_LOG_RES_CFG_TASK, "String compare error", 0U, 0U);
            }
        }
        if (j == SVC_RES_CFG_TASK_CATEGORY_MAX) {
            SvcLog_NG(SVC_LOG_RES_CFG_TASK, "Please increase SVC_RES_CFG_TASK_CATEGORY_MAX (%d)", SVC_RES_CFG_TASK_CATEGORY_MAX, 0U);
            RetVal = SVC_NG;
        } else {
            /* Cannot find any one category match, then create a new category */
            if (j == ResCfgTaskCategoryCount) {
                if (SVC_OK == AmbaWrap_memcpy(&(ResCfgTaskCategory[j].Name), ResCfg[i].GroupName, sizeof(ResCfg[i].GroupName))) {
                    UINT8 WrIdx = ResCfgTaskCategory[j].Count;
                    ResCfgTaskCategory[j].Idxs[WrIdx] = (UINT8) i;
                    ResCfgTaskCategory[j].Count++;
                    ResCfgTaskCategoryCount++;
                }
            }
        }
    }

    if (SVC_OK == RetVal) {
        for (i = 0U; i < ResCfgTaskCategoryCount; i++) {
            if (SVC_OK == AmbaWrap_memcmp(ResCfg[ResCfgTaskFormatId].GroupName, ResCfgTaskCategory[i].Name, sizeof(char) * 32U, &CmpResult)) {
                if (CmpResult == 0U) {
                    ResCfgTaskFormatIdInCategory = (UINT8) i;
                    break;
                }
            }
        }
    }
    (void) SvcResCfg_ConfigMax(&ResCfg[ResCfgTaskFormatId], 1U);

    if (SVC_OK == RetVal) {
        RetVal = SvcResCfg_Init(&ResCfg[ResCfgTaskFormatId]);

        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_RES_CFG_TASK, "SvcResCfgTask_Init() failed with %d", 0U, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        #if (defined(CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined(CONFIG_BSP_CV2XFSDK_OPTION_B)) && defined(CONFIG_FPD_MAXIM_ZS095BH3001A8H3_BII) && defined(CONFIG_BUILD_AMBA_ADAS)
        UINT8  ResCfgTaskDefinedFormatId[] = { 2U, 3U, 4U, 8U };
        UINT32 GuiIdx = 0U;
        UINT32 MaxDefinedFmtCnt = sizeof(ResCfgTaskDefinedFormatId) / sizeof(UINT8);
        FormatId = 0U;

        for (GuiIdx = 0U; GuiIdx < MaxDefinedFmtCnt; GuiIdx++) {
            FormatId = ResCfgTaskDefinedFormatId[GuiIdx];
            if (SVC_OK == CheckFormatId(FormatId)) {
                const char *pFormatName = ResCfg[FormatId].FormatName;

                ResCfgTaskIdxMap.GuiToFmt[GuiIdx]   = (UINT8) FormatId;
                ResCfgTaskIdxMap.FmtToGui[FormatId] = (UINT8) GuiIdx;

                ResCfgTaskCheckBox.pList[GuiIdx] = pFormatName;
                ResCfgTaskCheckBox.ListNum = ResCfgTaskCheckBox.ListNum + 1U;
            }
        }
        #else
        UINT32 GuiIdx = 0U;
        FormatId = 0U;
        while (SVC_OK == RetVal) {
            /* Fill in the check box information */
            if (SVC_OK == CheckFormatId(FormatId)) {
                const char *pFormatName = ResCfg[FormatId].FormatName;

                ResCfgTaskIdxMap.GuiToFmt[GuiIdx]   = (UINT8) FormatId;
                ResCfgTaskIdxMap.FmtToGui[FormatId] = (UINT8) GuiIdx;

                ResCfgTaskCheckBox.pList[FormatId] = pFormatName;
                ResCfgTaskCheckBox.ListNum = ResCfgTaskCheckBox.ListNum + 1U;

                FormatId = FormatId + 1U;
            } else {
                RetVal = SVC_NG;
            }
        }
        RetVal = SVC_OK;
        #endif
    }


    if (SVC_OK == RetVal) {
        SVC_SYS_STAT_ID StatusID;
        RetVal = SvcSysStat_Register(SVC_APP_STAT_MENU, ResCfgTask_MenuStatusCallback, &StatusID);
    }

    if (SVC_OK == RetVal) {
        RetVal = SvcButtonCtrl_Register(SVC_BUTTON_TYPE_GPIO, ResCfgTaskButtonCtrl, 3U, &ResCfgTaskButtonCtrl_ID);
    }

    SvcLog_OK(SVC_LOG_RES_CFG_TASK, "@@ SvcResCfgTask_Init() done", 0U, 0U);

    return RetVal;
}

/**
 * Resolution configuration for specified format index
 * @param [in] FormatId format index
 * @return ErrorCode
 */
UINT32 SvcResCfgTask_Config(UINT32 FormatId)
{
    UINT32 RetVal;

    SvcLog_OK(SVC_LOG_RES_CFG_TASK, "@@ SvcResCfgTask_Config() start", 0U, 0U);

    RetVal = CheckFormatId(FormatId);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_RES_CFG_TASK, "SvcResCfgTask_Config() failed. FormatId err", 0U, 0U);
    } else {
        ResCfgTaskFormatId = FormatId;
        (void) SvcResCfg_ConfigMax(&ResCfg[ResCfgTaskFormatId], 1U);
        RetVal = SvcResCfg_Config(&ResCfg[FormatId]);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_RES_CFG_TASK, "SvcResCfgTask_Config() failed with %d", RetVal, 0U);
        }
    }

    SvcLog_OK(SVC_LOG_RES_CFG_TASK, "@@ SvcResCfg_Config() done", 0U, 0U);

    return RetVal;
}

/**
 * Format index switching function
 * @param [in] FormatId format index
 * @return ErrorCode
 */
UINT32 SvcResCfgTask_Switch(UINT32 FormatId)
{
    UINT32 RetVal;
    UINT32 CmpResult = 0U;

    RetVal = CheckFormatId(FormatId);
    if (SVC_OK == RetVal) {
        const char EmptyStr[32] = "";
        if (SVC_OK == AmbaWrap_memcmp(ResCfg[ResCfgTaskFormatId].GroupName, EmptyStr, sizeof(char) * 32U, &CmpResult)) {
            if (CmpResult == 0U) {
                /* Reboot */
                if (SVC_OK != AmbaSYS_Reboot()) {
                    SvcLog_NG(SVC_LOG_RES_CFG_TASK, "AmbaSYS_Reboot() failed", 0U, 0U);
                }
            } else {
                if (SVC_OK == AmbaWrap_memcmp(ResCfg[ResCfgTaskFormatId].GroupName, ResCfg[FormatId].GroupName, sizeof(char) * 32U, &CmpResult)) {
                    if (CmpResult == 0U) {
                        /* Switch mode directly */
                        (void) SvcFlowControl_Exec("liveview_switch");
                    } else {
                        /* Reboot */
                        if (SVC_OK != AmbaSYS_Reboot()) {
                            SvcLog_NG(SVC_LOG_RES_CFG_TASK, "AmbaSYS_Reboot() failed", 0U, 0U);
                        }
                    }
                }
            }
        }
    }

    return RetVal;
}

/**
 * Format index printing function
 */
void SvcResCfgTask_Dump(void)
{
    UINT32 FormatId = 0U;
    UINT32 i, j;

    AmbaPrint_PrintUInt5("FormatId list:", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("------------------------", 0U, 0U, 0U, 0U, 0U);

    for (i = 0; i < ResCfgTaskCategoryCount; i++) {
        AmbaPrint_PrintUInt5("********************************", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("Group %d:", i, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5(ResCfgTaskCategory[i].Name, 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("********************************", 0U, 0U, 0U, 0U, 0U);

        for (j = 0; j < ResCfgTaskCategory[i].Count; j++) {
            FormatId = ResCfgTaskCategory[i].Idxs[j];
            /* Print FormatId name */
            if (FormatId == ResCfgTaskFormatId) {
                AmbaPrint_PrintUInt5("%2d", FormatId, 0U, 0U, 0U, 0U);
                AmbaPrint_PrintStr5("\033""[38;2;255;255;128m""\033""[48;2;0;0;0m%s\033""[0m",
                                        ResCfg[FormatId].FormatName, NULL, NULL, NULL, NULL);
            } else {
                AmbaPrint_PrintUInt5("%2d", FormatId, 0U, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5(ResCfg[FormatId].FormatName, 0U, 0U, 0U, 0U, 0U);
            }
        }
    }

    AmbaPrint_PrintUInt5("------------------------", 0U, 0U, 0U, 0U, 0U);
}

/**
 * Resolution configuration getting function
 * @param [out] pResCfgArr pointer to format configuration array
 * @param [out] pResCfgNum pointer to number of format index
 */
void SvcResCfgTask_GetCfgArr(const SVC_RES_CFG_s **pResCfgArr, UINT32 *pResCfgNum)
{
    *pResCfgArr = ResCfg;
    *pResCfgNum = (UINT32) sizeof(g_ResCfg) / (UINT32) sizeof(SVC_RES_CFG_s);

#ifdef CONFIG_ICAM_THMVIEW_IN_DPX
    *pResCfgNum += ((UINT32) sizeof(g_ResPbkCfg) / (UINT32) sizeof(SVC_RES_CFG_s));
#endif

}

/**
 * playback mode id getting function
 * @param [out] pFormatId pointer to playback mode id
 * @return ErrorCode
 */
UINT32 SvcResCfgTask_GetPbkModeId(UINT32 *pFormatId)
{
    UINT32 RetVal = SVC_NG;

    AmbaMisra_TouchUnused(&RetVal);
    AmbaMisra_TouchUnused(pFormatId);

#ifdef CONFIG_ICAM_THMVIEW_IN_DPX
    *pFormatId  = ((UINT32) sizeof(g_ResCfg) / (UINT32) sizeof(SVC_RES_CFG_s));
    RetVal      = SVC_OK;
#endif

    return RetVal;
}

static void ResCfgTask_MenuStatusCallback(UINT32 StatIdx, void *pInfo)
{
    UINT32 RetVal = SVC_OK;
    SVC_APP_STAT_MENU_s *pStatus = NULL;

    AmbaMisra_TouchUnused(pInfo);   /* Misra-c fixed */
    AmbaMisra_TouchUnused(pStatus); /* Misra-c fixed */

    AmbaMisra_TypeCast(&pStatus, &pInfo);

    if (StatIdx == SVC_APP_STAT_MENU) {
        if ((pStatus->Type == SVC_APP_STAT_MENU_TYPE_EMR) && (pStatus->FuncIdx == SVC_APP_STAT_MENU_EMR_FORMAT)) {
            UINT32 OsdWidth, OsdHeight;

            ResCfgTaskGuiVout = (UINT32) pStatus->Vout;
            RetVal = SvcOsd_GetOsdBufSize(ResCfgTaskGuiVout, &OsdWidth, &OsdHeight);
            if (SVC_OK == RetVal) {
                ResCfgTaskCanvas.StartX = (OsdWidth - (RES_CFG_TASK_GUI_W)) >> (UINT32) 1U;
                ResCfgTaskCanvas.StartY = (OsdHeight - (RES_CFG_TASK_GUI_H)) >> (UINT32) 1U;
                ResCfgTaskCanvas.Width  = RES_CFG_TASK_GUI_W;
                ResCfgTaskCanvas.Height = RES_CFG_TASK_GUI_H;
            } else {
                SvcLog_NG(SVC_LOG_RES_CFG_TASK, "Get Osd Buf failed", 0U, 0U);
            }
            if (ResCfgTaskGuiShow == SVC_RES_CFG_TASK_GUI_NO) {
                ResCfgTaskGuiShow = SVC_RES_CFG_TASK_GUI_SHOW | SVC_RES_CFG_TASK_GUI_UPDATE;
                ResCfgTaskCheckBox.CurIdx = ResCfgTaskIdxMap.FmtToGui[ResCfgTaskFormatId];
                ResCfgTaskCheckBox.CheckBits = ((UINT32) 1U << ResCfgTaskCheckBox.CurIdx);
                RetVal = SvcButtonCtrl_Request(&ResCfgTaskButtonCtrl_ID);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_RES_CFG_TASK, "SvcButtonCtrl_Request(%d) failed", ResCfgTaskButtonCtrl_ID, 0U);
                }
                SvcGui_Register(ResCfgTaskGuiVout, 17U, "ResMenu", ResCfgTask_DrawEntry, ResCfgTask_DrawUpdate);
            } else {
                ResCfgTaskGuiShow = SVC_RES_CFG_TASK_GUI_NO;
                RetVal = SvcButtonCtrl_Release(&ResCfgTaskButtonCtrl_ID);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_RES_CFG_TASK, "SvcButtonCtrl_Release(%d) failed", ResCfgTaskButtonCtrl_ID, 0U);
                }
                SvcGui_Unregister(ResCfgTaskGuiVout, 17U);
            }
        }
    }
}

static void ResCfgTask_DrawEntry(UINT32 VoutIdx, UINT32 Level)
{
    SVC_GUI_CANVAS_s CheckBoxCanvas = { .StartX = ResCfgTaskCanvas.StartX, .StartY = ResCfgTaskCanvas.StartY, .Width = RES_CFG_TASK_GUI_W, .Height = RES_CFG_TASK_GUI_CHECKBOX_H };
    SVC_GUI_CANVAS_s TextViewCanvas = { .StartX = ResCfgTaskCanvas.StartX, .StartY = ResCfgTaskCanvas.StartY + RES_CFG_TASK_GUI_CHECKBOX_H, .Width = RES_CFG_TASK_GUI_W, .Height = RES_CFG_TASK_GUI_TEXTBOX1_H };
    SVC_GUI_TEXTVIEW_s TextView = {
        .BackgroundColor = 0x80202040U,
        .FontColor = 0xffffffffU,
        .FontSize = 1U,
        .LineGap = 1U,
        .pText = "1/2: Choose the formatId. 3: Move 4: Leave",
    };
    SVC_GUI_CANVAS_s TextViewCanvas1 = { .StartX = ResCfgTaskCanvas.StartX, .StartY = ResCfgTaskCanvas.StartY + RES_CFG_TASK_GUI_CHECKBOX_H + RES_CFG_TASK_GUI_TEXTBOX1_H, .Width = RES_CFG_TASK_GUI_W, .Height = RES_CFG_TASK_GUI_TEXTBOX2_H };
    SVC_GUI_TEXTVIEW_s TextView1 = {
        .BackgroundColor = 0x80202040U,
        .FontColor = 0xffffffffU,
        .FontSize = 1U,
        .LineGap = 1U,
        .pText = "-/+: Choose the formatId. !: Move @: Leave",
    };

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if ((ResCfgTaskGuiShow & SVC_RES_CFG_TASK_GUI_SHOW) > 0U) {
        SvcGuiCheckBox_Draw(ResCfgTaskGuiVout, &CheckBoxCanvas, &ResCfgTaskCheckBox);
        SvcGuiTextView_Draw(ResCfgTaskGuiVout, &TextViewCanvas, &TextView);
        SvcGuiTextView_Draw(ResCfgTaskGuiVout, &TextViewCanvas1, &TextView1);
    }
}

static void ResCfgTask_DrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if ((ResCfgTaskGuiShow & SVC_RES_CFG_TASK_GUI_UPDATE) > 0U) {
        ResCfgTaskGuiShow &= ~(SVC_RES_CFG_TASK_GUI_UPDATE);
        *pUpdate = 1U;
    } else {
        *pUpdate = 0U;
    }
}

static UINT32 ResCfgTask_Minus(void)
{
    UINT32          PrefBufSize;
    ULONG           PrefBufAddr;
    SVC_USER_PREF_s *pSvcUserPref;

    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
        SvcLog_NG(SVC_LOG_RES_CFG_TASK, "SvcUserPref_Get() failed!!", 0U, 0U);
    } else {
        pSvcUserPref->FormatId = ResCfgTaskIdxMap.GuiToFmt[ResCfgTaskCheckBox.CurIdx];

        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);

        if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
            SvcLog_NG(SVC_LOG_RES_CFG_TASK, "SvcPref_Save failed!!", 0U, 0U);
        }

        if (SVC_OK != AmbaSYS_Reboot()) {
            SvcLog_NG(SVC_LOG_RES_CFG_TASK, "AmbaSYS_Reboot() failed", 0U, 0U);
        }
    }

    return SVC_OK;
}

static UINT32 ResCfgTask_Plus(void)
{
    UINT32          PrefBufSize;
    ULONG           PrefBufAddr;
    SVC_USER_PREF_s *pSvcUserPref;

    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
        SvcLog_NG(SVC_LOG_RES_CFG_TASK, "SvcUserPref_Get() failed!!", 0U, 0U);
    } else {
        pSvcUserPref->FormatId = ResCfgTaskIdxMap.GuiToFmt[ResCfgTaskCheckBox.CurIdx];

        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);

        if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
            SvcLog_NG(SVC_LOG_RES_CFG_TASK, "SvcPref_Save failed!!", 0U, 0U);
        }

        if (SVC_OK != AmbaSYS_Reboot()) {
            SvcLog_NG(SVC_LOG_RES_CFG_TASK, "AmbaSYS_Reboot() failed", 0U, 0U);
        }
    }

    return SVC_OK;
}

static UINT32 ResCfgTask_Move(void)
{
    ResCfgTaskCheckBox.CurIdx = (ResCfgTaskCheckBox.CurIdx + 1U) % ResCfgTaskCheckBox.ListNum;
    ResCfgTaskGuiShow |= SVC_RES_CFG_TASK_GUI_UPDATE;
    return SVC_OK;
}
