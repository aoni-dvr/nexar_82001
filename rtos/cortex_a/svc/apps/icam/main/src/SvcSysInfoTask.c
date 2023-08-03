/**
 *  @file SvcSysInfoTask.c
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
 *  @details svc system information task
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_VOUT.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcDisplay.h"
#include "SvcTask.h"
#include "SvcResCfg.h"
#include "SvcOsd.h"
#include "SvcGui.h"
#include "SvcDramUtil.h"

#ifdef CONFIG_BUILD_CV
#include "SvcCvFlow.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvAppDef.h"
#endif

#include "SvcSysInfoTask.h"

#define SVC_LOG_SYSINFO_TASK  "SYSINFO"

#define SVC_SYSINFO_GUI_LEVEL               (63U)
#define SVC_SYSINFO_TASK_STACK_SIZE         (0x2000)

#define SVC_DRAM_PFTIME_MS                  (2000U)

#if defined(SVC_DRAMUTIL_CMD_USED)
#if defined(CONFIG_SOC_CV5)
    #if defined(CONFIG_BSP_CV5BUB_OPTION_B_V100)
    #define SVC_DRAM_TYPE                   (SVC_DRAMUTIL_LP4_32)
    #else
    #define SVC_DRAM_TYPE                   (SVC_DRAMUTIL_LP5_32)
    #endif
    #if defined(CONFIG_ENABLE_ONE_DDRC_HOST)
    #define SVC_DRAM_PIECE                  (1U)
    #else
    #define SVC_DRAM_PIECE                  (2U)
    #endif
#elif defined(CONFIG_SOC_CV52)
    #define SVC_DRAM_TYPE                   (SVC_DRAMUTIL_LP4_32)
    #define SVC_DRAM_PIECE                  (1U)
#elif defined(CONFIG_SOC_CV2)
    #define SVC_DRAM_TYPE                   (SVC_DRAMUTIL_LP4_32)
    #define SVC_DRAM_PIECE                  (2U)
#elif defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
    #define SVC_DRAM_TYPE                   (SVC_DRAMUTIL_LP4_16)
    #define SVC_DRAM_PIECE                  (1U)
#else
    #define SVC_DRAM_TYPE                   (SVC_DRAMUTIL_LP4_32)
    #define SVC_DRAM_PIECE                  (1U)
#endif
#endif

typedef struct {
    UINT32 DispCvRate   :1;                 /* Disp rate or time */
    UINT32 Reserved     :31;
    SVC_TASK_CTRL_s  DramPfTask;
} SVC_SYS_INFO_CTRL_s;

typedef struct {
    DOUBLE DramUtil;
    UINT32 CvFrameTime[SVC_RES_CV_FLOW_NUM];
    UINT32 CvProcTime[SVC_RES_CV_FLOW_NUM];
} SVC_SYS_INFO_s;

static UINT32 SysInfoTaskInit = 0U;
static UINT32 SysInfoEnableBits = 0U;
static SVC_SYS_INFO_CTRL_s SysInfoCtrl;
static SVC_SYS_INFO_s      SysInfo;

typedef struct {
    UINT32 X1;
    UINT32 Y1;
    UINT8  FontSize;
    UINT32 Color;
} SVC_SYS_INFO_DRAW_CFG_s;

static UINT32 SysInfoDispInit = 0U;
static UINT32 SysInfoDispBits = 0U;
#if defined (CONFIG_SOC_CV28)
static UINT32 DispVoutIdx = 0U;
#else
static UINT32 DispVoutIdx = 1U;
#endif
static UINT32 OsdTabIdx = 0U;

#define SVC_SYSINFO_OSD_TAB_NUM            (13U)
static SVC_SYS_INFO_DRAW_CFG_s SysInfoDrawCfg[SVC_SYSINFO_OSD_TAB_NUM] = {
    {    10U,    10U,    3U,    37U },
    {    10U,    40U,    3U,    37U },
    {    10U,    70U,    3U,    37U },
    {    10U,   110U,    3U,    37U },
    {    10U,   140U,    3U,    37U },
    {    10U,   170U,    3U,    37U },
    {    10U,   200U,    3U,    37U },
    {    10U,   230U,    3U,    37U },
    {    10U,   260U,    3U,    37U },
    {    10U,   290U,    3U,    37U },
    {    10U,   320U,    3U,    37U },
    {    10U,   350U,    3U,    37U },
    {    10U,   380U,    3U,    37U },
};

static void   SysInfoTask_GUIUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);
static void   SysInfoTask_GUIDraw(UINT32 VoutIdx, UINT32 Level);
static UINT32 SysInfoTask_DrawTabString(UINT32 VoutIdx, UINT32 TabIdx, const char *pString);
static void*  DramProf_TaskEntry(void* EntryArg);
static void   SysInfoTask_DrawDramUtil(UINT32 VoutIdx);
#ifdef CONFIG_BUILD_CV
static void   SysInfoTask_GetCvFrameRate(void);
static void   SysInfoTask_DrawCvFrameRate(UINT32 VoutIdx);
#endif

/**
 *  Start System information Task.
 *  @param [in] EnableBits Indicate which information to be enabled
 *  @return error code
 */
UINT32 SvcSysInfoTask_Start(UINT32 EnableBits)
{
    SVC_TASK_CTRL_s *pTaskCtrl;
    static UINT8 DramProfTaskStack[SVC_SYSINFO_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    UINT32 RetVal = SVC_OK;

    if (SysInfoTaskInit == 0U) {
        SysInfoTaskInit = 1U;
        AmbaSvcWrap_MisraMemset(&SysInfoCtrl, 0, sizeof(SysInfoCtrl));
        AmbaSvcWrap_MisraMemset(&SysInfo, 0, sizeof(SysInfo));
    }

    if ((EnableBits & SVC_SYS_INFO_DRAM_UTIL) > 0U) {
        pTaskCtrl = &SysInfoCtrl.DramPfTask;
        AmbaSvcWrap_MisraMemset(pTaskCtrl, 0, sizeof(SVC_TASK_CTRL_s));
        pTaskCtrl->Priority   = 120U;
        pTaskCtrl->EntryFunc  = DramProf_TaskEntry;
        pTaskCtrl->EntryArg   = 0U;
        pTaskCtrl->pStackBase = &DramProfTaskStack[0];
        pTaskCtrl->StackSize  = SVC_SYSINFO_TASK_STACK_SIZE;
        pTaskCtrl->CpuBits    = 0x01U;
        RetVal = SvcTask_Create("DramProfTask", pTaskCtrl);
        if (SVC_OK == RetVal) {
            SysInfoEnableBits |= SVC_SYS_INFO_DRAM_UTIL;
        } else {
            SvcLog_NG(SVC_LOG_SYSINFO_TASK, "fail to SvcTask_Create", 0U, 0U);
        }
    }

    if ((EnableBits & SVC_SYS_INFO_CV_FRAME_RATE) > 0U) {
        SysInfoEnableBits |= SVC_SYS_INFO_CV_FRAME_RATE;
    }

    return RetVal;
}

/**
 *  Init System information Display Task.
 *  @param [in] EnableBits Indicate which information to be displayed
 *  @return error code
 */
UINT32 SvcSysInfoTask_DispEnable(UINT32 EnableBits)
{
    UINT32 RetVal = SVC_OK;

    if (SysInfoDispInit == 0U) {
        SysInfoDispInit = 1U;
        SvcGui_Register(DispVoutIdx,
                        SVC_SYSINFO_GUI_LEVEL,
                        SVC_LOG_SYSINFO_TASK,
                        SysInfoTask_GUIDraw,
                        SysInfoTask_GUIUpdate);
    }

    SysInfoDispBits = EnableBits;

    AmbaMisra_TouchUnused(&SysInfoDrawCfg);

    return RetVal;
}

static void SvcSysInfoTask_DispStop(void)
{
    if (SysInfoDispInit > 0U) {
        SvcGui_Unregister(DispVoutIdx, SVC_SYSINFO_GUI_LEVEL);
        SysInfoDispBits = 0U;
        SysInfoDispInit = 0U;
    }
}

/**
 *  Stop System information Task.
 *  @return error code
 */
UINT32 SvcSysInfoTask_Stop(void)
{
    UINT32 RetVal = SVC_OK;

    if (SysInfoTaskInit > 0U) {
        SvcSysInfoTask_DispStop();

        if ((SysInfoEnableBits & SVC_SYS_INFO_DRAM_UTIL) > 0U) {
            RetVal = SvcTask_Destroy(&SysInfoCtrl.DramPfTask);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_SYSINFO_TASK, "fail to destroy DramPfTask", 0U, 0U);
            }
        }

        SysInfoEnableBits = 0U;
        SysInfoTaskInit = 0U;
    } else {
        SvcLog_NG(SVC_LOG_SYSINFO_TASK, "SvcSysInfoTask is not enabled", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

static void SysInfoTask_GUIUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    if (SysInfoDispInit > 0U) {
        if ((VoutIdx == DispVoutIdx) && (SysInfoDispBits > 0U)) {
            *pUpdate = 1U;
        }
    }

    AmbaMisra_TouchUnused(&Level);
}


static void SysInfoTask_GUIDraw(UINT32 VoutIdx, UINT32 Level)
{
    if (SysInfoDispInit > 0U) {
        if (VoutIdx == DispVoutIdx) {
            /* Reset the index */
            OsdTabIdx = 0U;

            if ((SysInfoDispBits & SVC_SYS_INFO_DRAM_UTIL) > 0U) {
                SysInfoTask_DrawDramUtil(DispVoutIdx);
            }

#ifdef CONFIG_BUILD_CV
            if ((SysInfoDispBits & SVC_SYS_INFO_CV_FRAME_RATE) > 0U) {
                SysInfoTask_GetCvFrameRate();
                SysInfoTask_DrawCvFrameRate(DispVoutIdx);
            }
#endif
        }
    }

    AmbaMisra_TouchUnused(&Level);
}

static UINT32 SysInfoTask_DrawTabString(UINT32 VoutIdx, UINT32 TabIdx, const char *pString)
{
    UINT32 RetVal;
    const SVC_SYS_INFO_DRAW_CFG_s *pDrawCfg;
    UINT32 Color;

    if (TabIdx < SVC_SYSINFO_OSD_TAB_NUM) {
        pDrawCfg = &SysInfoDrawCfg[TabIdx];
        SvcOsd_ColorIdxToColorSetting(VoutIdx, pDrawCfg->Color, &Color);
        RetVal = SvcOsd_DrawString(VoutIdx, pDrawCfg->X1, pDrawCfg->Y1, pDrawCfg->FontSize, Color, pString);
    } else {
        SvcLog_NG(SVC_LOG_SYSINFO_TASK, "SysInfoTask_DrawTabString invalid TabIdx(%d)", TabIdx, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/* Task to profile DRAM utilization periodically */
static void* DramProf_TaskEntry(void* EntryArg)
{
    UINT32  RetVal = SVC_OK;
#if defined(SVC_DRAMUTIL_CMD_USED)
    SVC_DRAMUTIL_PAR_s  Param;
    SVC_DRAMUTIL_RES_s  Result;

    Param.MSec        = SVC_DRAM_PFTIME_MS;
    Param.DramType    = SVC_DRAM_TYPE;
    Param.DramPiece   = SVC_DRAM_PIECE;
    Param.ProfileMode = SVC_DRAMUTIL_PFMODE_REAL;
#endif

    AmbaMisra_TouchUnused(EntryArg);
    SvcLog_DBG(SVC_LOG_SYSINFO_TASK, "DramProf_TaskEntry start", 0U, 0U);

    while (SVC_OK == RetVal) {
        if ((SysInfoEnableBits & SVC_SYS_INFO_DRAM_UTIL) > 0U) {
        #if defined(SVC_DRAMUTIL_CMD_USED)
            SvcDramUtil_Profile(&Param, &Result);
            SysInfo.DramUtil = Result.TotalUtil;
        #else
            if (AmbaKAL_TaskSleep(SVC_DRAM_PFTIME_MS) != OK) {
                SvcLog_NG(SVC_LOG_SYSINFO_TASK, "!!!! fail to task sleep !!!!\n", 0U, 0U);
            }
        #endif
        }

        AmbaMisra_TouchUnused(&RetVal);
    }

    SvcLog_DBG(SVC_LOG_SYSINFO_TASK, "DramProf_TaskEntry exit", 0U, 0U);

    return NULL;
}

static void SysInfoTask_DrawDramUtil(UINT32 VoutIdx)
{
    UINT32 RetVal = SVC_OK;
    char TmpStr[16];
    char OsdStr[32];
    const char *ArgS[1U];

    TmpStr[0] = '\0';
    ArgS[0U] = TmpStr;
    if (0U == AmbaUtility_DoubleToStr(TmpStr, (UINT32)sizeof(TmpStr), SysInfo.DramUtil, 2U)) { /* none */ }
    if (0U == AmbaUtility_StringPrintStr(OsdStr, (UINT32)sizeof(TmpStr), "DRAM: %s %%", 1U, ArgS)) { /* none */ }

    RetVal = SysInfoTask_DrawTabString(VoutIdx, OsdTabIdx, OsdStr);
    OsdTabIdx++;
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_SYSINFO_TASK, "SysInfoTask_DrawDramUtil failed", 0U, 0U);
    }
}

#ifdef CONFIG_BUILD_CV
static void SysInfoTask_GetCvFrameRate(void)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    SVC_CV_FRAME_TIME_INFO_s FTInfo = {0};

    for (UINT32 CvfChan = 0U; CvfChan < CvFlowNum; CvfChan++) {
        if ((CvFlowBits & ((UINT32) 1U << CvfChan)) > 0U) {
            if ((pResCfg->CvFlow[CvfChan].CvFlowType == SVC_CV_FLOW_REF_OD) ||
                (pResCfg->CvFlow[CvfChan].CvFlowType == SVC_CV_FLOW_AMBA_OD) ||
                (pResCfg->CvFlow[CvfChan].CvFlowType == SVC_CV_FLOW_REF_SEG) ||
                (pResCfg->CvFlow[CvfChan].CvFlowType == SVC_CV_FLOW_AMBA_SEG)) {
                RetVal = SvcCvFlow_Control(CvfChan, SVC_CV_CTRL_GET_OUTPUT_FRAME_TIME, &FTInfo);
                if (RetVal == SVC_OK) {
                    SysInfo.CvFrameTime[CvfChan] = FTInfo.FrameTime;
                    SysInfo.CvProcTime[CvfChan] = FTInfo.ProcTimeFD;
                } else {
                    SvcLog_DBG(SVC_LOG_SYSINFO_TASK, "No frame rate info", 0U, 0U);
                }
            }
        }
    }
}

static void SysInfoTask_DrawCvFrameRate(UINT32 VoutIdx)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    char OsdStr[32];

    for (UINT32 CvfChan = 0U; CvfChan < CvFlowNum; CvfChan++) {
        if ((CvFlowBits & ((UINT32) 1U << CvfChan)) > 0U) {
            if ((pResCfg->CvFlow[CvfChan].CvFlowType == SVC_CV_FLOW_REF_OD) ||
                (pResCfg->CvFlow[CvfChan].CvFlowType == SVC_CV_FLOW_AMBA_OD) ||
                (pResCfg->CvFlow[CvfChan].CvFlowType == SVC_CV_FLOW_REF_SEG) ||
                (pResCfg->CvFlow[CvfChan].CvFlowType == SVC_CV_FLOW_AMBA_SEG)) {

                if (SysInfoCtrl.DispCvRate > 0U) {
                    /* Frame rate */
                    DOUBLE FrameRate;

                    if (SysInfo.CvFrameTime[CvfChan] != 0U) {
                        FrameRate = (DOUBLE)1000000 / (DOUBLE)SysInfo.CvFrameTime[CvfChan];
                    } else {
                        FrameRate = 0.0;
                    }

                    OsdStr[0] = '\0';
                    if ((pResCfg->CvFlow[CvfChan].CvFlowType == SVC_CV_FLOW_REF_OD) ||
                        (pResCfg->CvFlow[CvfChan].CvFlowType == SVC_CV_FLOW_AMBA_OD)) {
                        if (0U == AmbaUtility_StringPrintUInt32(OsdStr, (UINT32)sizeof(OsdStr), "OD%u FPS:", 1U, &CvfChan)) { /* none */ }
                    } else {
                        if (0U == AmbaUtility_StringPrintUInt32(OsdStr, (UINT32)sizeof(OsdStr), "SG%u FPS:", 1U, &CvfChan)) { /* none */ }
                    }
                    AmbaUtility_StringAppendDouble(OsdStr, (UINT32)sizeof(OsdStr), FrameRate, 2U);
                } else {
                    /* Proc time */
                    UINT32 ProcTime = SysInfo.CvProcTime[CvfChan] / 1000U;

                    OsdStr[0] = '\0';
                    if ((pResCfg->CvFlow[CvfChan].CvFlowType == SVC_CV_FLOW_REF_OD) ||
                        (pResCfg->CvFlow[CvfChan].CvFlowType == SVC_CV_FLOW_AMBA_OD)) {
                        if (0U == AmbaUtility_StringPrintUInt32(OsdStr, (UINT32)sizeof(OsdStr), "OD%u t:", 1U, &CvfChan)) { /* none */ }
                    } else {
                        if (0U == AmbaUtility_StringPrintUInt32(OsdStr, (UINT32)sizeof(OsdStr), "SG%u t:", 1U, &CvfChan)) { /* none */ }
                    }
                    AmbaUtility_StringAppendUInt32(OsdStr, (UINT32)sizeof(OsdStr), ProcTime, 10U);
                    AmbaUtility_StringAppend(OsdStr, (UINT32)sizeof(OsdStr), " ms");
                }

                RetVal = SysInfoTask_DrawTabString(VoutIdx, OsdTabIdx, OsdStr);
                OsdTabIdx++;
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_SYSINFO_TASK, "SysInfoTask_DrawCvFrameRate failed", 0U, 0U);
                }
            }
        }
    }
}
#endif

