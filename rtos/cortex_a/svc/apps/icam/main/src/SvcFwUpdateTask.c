/**
 *  @file SvcFwUpdateTask.c
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
 *  @details svc firmware update task
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaFS.h"
#include "AmbaSYS.h"
#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VOUT_Def.h"
#include "AmbaFPD.h"
#include "AmbaSD.h"
#include "AmbaShell.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcTaskList.h"
#include "SvcTask.h"
#include "SvcNvm.h"
#include "SvcMem.h"
#include "SvcBufMap.h"
#include "SvcBuffer.h"
#include "SvcSysStat.h"
#include "SvcOsd.h"
#include "SvcGuiDiagBox.h"
#include "SvcUserPref.h"
#include "SvcResCfg.h"
#include "SvcFwUpdate.h"
#include "SvcMenuTask.h"
#include "SvcDataLoader.h"
#include "SvcFwUpdateTask.h"

#include "SvcAppStat.h"

#define SVC_FW_UPD_TSK_FLG_INIT      (0x1U)
#define SVC_FW_UPD_TSK_FLG_START     (0x2U)
#ifdef SVC_FW_UPDATE_DBG_MSG_ON
#define SVC_FW_UPD_TSK_FLA_DBG_MSG   (0x1000U)
#endif
#define SVC_FW_UPD_TSK_FLG_SD0_RDY   (0x1U)
#define SVC_FW_UPD_TSK_FLG_SD1_RDY   (0x2U)
#define SVC_FW_UPD_TSK_FLG_SYS_RDY   (0x4U)
#define SVC_FW_UPD_TSK_FLG_GUI_RDY   (0x10U)

#define SVC_FW_UPD_TSK_NAME          ("SvcFwUpdate")
#define SVC_FW_UPD_TSK_SCN_STK_SIZE  (0x4000U)
#define SVC_FW_UPD_TSK_SCN_INTERVAL  (1000U)
#define SVC_FW_UPD_TSK_SCN_PTN0      SVC_FW_UPDATA_DEF_FILE_NAME
#define SVC_FW_UPD_TSK_SCN_PTN1      "c:\\SvcDspFW.bin"
#define SVC_FW_UPD_TSK_SCN_PTN2      "c:\\SvcRomFW.bin"
#define SVC_FW_UPD_TSK_SCN_PTN_NUM   (0x3U)

#define SVC_FW_UPD_TSK_SCN_PTN_OFF   (0x0U)
#define SVC_FW_UPD_TSK_SCN_PTN_ON    (0x1U)
#define SVC_FW_UPD_TSK_SCN_PTN_UPD   (0x2U)
#define SVC_FW_UPD_TSK_SCN_PTN_DONE  (0x4U)

#define SVC_FW_UPD_TSK_OSD_WIDTH     (704U)
#define SVC_FW_UPD_TSK_OSD_HEIGHT    (283U)
#define SVC_FW_UPD_TSK_OSD_PTN       "icon_fwupdate.bmp"

#define SVC_FW_UPD_TSK_GUI_LEVEL     (21U)

#define SVC_LOG_FWU_TSK "FWU_TASK"
static void LOG_FWU_TSK_OK(const char *pStr, UINT32 Arg1, UINT32 Arg2);
static void LOG_FWU_TSK_NG(const char *pStr, UINT32 Arg1, UINT32 Arg2);
static void LOG_FWU_TSK_API(const char *pStr, UINT32 Arg1, UINT32 Arg2);

#define PRN_FWU_TSK_NG       SVC_WRAP_PRINT
#define PRN_FWU_TSK_API      SVC_WRAP_PRINT
#define PRN_FWU_TSK_ERR_HDLR SvcFwUpdateTask_ErrHdlr(__func__, __LINE__, PRetVal);

typedef struct {
    UINT32 Status;
    char   FilePath[64];
} SVC_FW_UPDATE_SCAN_PTN_s;

typedef struct {
    UINT32                   NumOfPattern;
    SVC_FW_UPDATE_SCAN_PTN_s *pScanPtns;
} SVC_FW_UPDATE_SCAN_CTRL_s;

typedef struct {
    char                      Name[32U];
    AMBA_KAL_EVENT_FLAG_t     EventFlag;
    SVC_FW_UPDATE_SCAN_CTRL_s ScanCtrl;
    SVC_TASK_CTRL_s           ScanTskCtrl;
    UINT8                     ScanTskStack[SVC_FW_UPD_TSK_SCN_STK_SIZE];
    UINT32                    ActiveVoutID;
} SVC_FW_UPDATE_TSK_CTRL_s;

typedef struct {
    UINT32 DiagBoxWidth;
    UINT32 DiagBoxHeight;
    UINT32 OsdBufWidth;
    UINT32 OsdBufHeight;
} SVC_FW_UPDATE_DIAGBOX_WIN_INFO_s;

static void   SvcFwUpdateTask_memset(void *ptr, INT32 v, SIZE_t n);
static UINT32 SvcFwUpdateTask_ScanProc(void);
static UINT32 SvcFwUpdateTask_ScanReset(void);
static UINT32 SvcFwUpdateTask_DiagBoxCb(UINT32 *pIsSuspend);
static UINT32 SvcFwUpdateTask_UpdateProc(void);
static UINT32 SvcFwUpdateTask_UpdatePreProc(SVC_FW_UPDATE_SCAN_PTN_s *pCurPtn);
static UINT32 SvcFwUpdateTask_UpdatePostProc(SVC_FW_UPDATE_SCAN_PTN_s *pCurPtn);
static UINT32 SvcFwUpdateTask_ReBootProc(void);
static void*  SvcFwUpdateTask_TaskEntry(void* EntryArg);
static void   SvcFwUpdateTask_SDStateCallback(UINT32 StatIdx, void *pInfo);
static void   SvcFwUpdateTask_BootStateCallback(UINT32 StatIdx, void *pInfo);
static UINT32 SvcFwUpdateTask_GuiLoad(void);
static UINT32 SvcFwUpdatetask_FwRename(SVC_FW_UPDATE_SCAN_PTN_s *pCurPtn);

static SVC_FW_UPDATE_SCAN_PTN_s SvcFwUpdTskScnPtns[SVC_FW_UPD_TSK_SCN_PTN_NUM] GNU_SECTION_NOZEROINIT;
static SVC_FW_UPDATE_TSK_CTRL_s SVC_FwUpdTskCtrl GNU_SECTION_NOZEROINIT;
#ifndef SVC_FW_UPDATE_DBG_MSG_ON
static UINT32 SVC_FwUpdTskCtrlFlag = 0;
#else
static UINT32 SVC_FwUpdTskCtrlFlag = SVC_FW_UPD_TSK_FLA_DBG_MSG;
#endif

static void LOG_FWU_TSK_OK(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
    SvcLog_OK(SVC_LOG_FWU_TSK, pStr, Arg1, Arg2);
}

static void LOG_FWU_TSK_NG(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
    SvcLog_NG(SVC_LOG_FWU_TSK, pStr, Arg1, Arg2);
}

static void LOG_FWU_TSK_API(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
    SvcLog_DBG(SVC_LOG_FWU_TSK, pStr, Arg1, Arg2);
}

static void SvcFwUpdateTask_memset(void *ptr, INT32 v, SIZE_t n)
{
    UINT32 ErrCode = AmbaWrap_memset(ptr, v, n);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static void SvcFwUpdateTask_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode)
{
    if (pCaller != NULL) {
        if (ErrCode != 0U) {
            PRN_FWU_TSK_NG "Catch ErrCode(0x%08x) @ %s, %d"
            SVC_PRN_ARG_S SVC_LOG_FWU_TSK
                SVC_PRN_ARG_UINT32 ErrCode  SVC_PRN_ARG_POST
                SVC_PRN_ARG_CSTR   pCaller  SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 CodeLine SVC_PRN_ARG_POST
            SVC_PRN_ARG_E
        }
    }
}

static UINT32 SvcFwUpdateTask_ScanProc(void)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_FwUpdTskCtrlFlag & SVC_FW_UPD_TSK_FLG_START) == 0U) {
        RetVal = SVC_NG;
    } else {
        UINT32 Idx;
        SVC_FW_UPDATE_SCAN_PTN_s *pCurPtn;
        AMBA_FS_FILE_INFO_s FileInfo;

        RetVal = SVC_NG;

        for (Idx = 0U; Idx < SVC_FwUpdTskCtrl.ScanCtrl.NumOfPattern; Idx ++) {

            pCurPtn = &(SVC_FwUpdTskCtrl.ScanCtrl.pScanPtns[Idx]);
            if (((pCurPtn->Status & SVC_FW_UPD_TSK_SCN_PTN_ON)  >  0U) &&
                ((pCurPtn->Status & SVC_FW_UPD_TSK_SCN_PTN_UPD) == 0U)) {

                SvcFwUpdateTask_memset(&FileInfo, 0, sizeof(FileInfo));
                if (0U == AmbaFS_GetFileInfo(pCurPtn->FilePath, &FileInfo)) {
                    if (FileInfo.Size > 0ULL) {
                        pCurPtn->Status &= ~(SVC_FW_UPD_TSK_SCN_PTN_DONE);
                        pCurPtn->Status |= SVC_FW_UPD_TSK_SCN_PTN_UPD;
                        RetVal = SVC_OK;
                    }
                }
            }
        }
    }

    return RetVal;
}

static UINT32 SvcFwUpdateTask_ScanReset(void)
{
    UINT32           RetVal = SVC_OK, Flag = SVC_FW_UPD_TSK_FLG_SD0_RDY, PRetVal;
    SVC_USER_PREF_s  *pUserPref;

    PRetVal = SvcUserPref_Get(&pUserPref); PRN_FWU_TSK_ERR_HDLR
    if (pUserPref->MainStgChan == AMBA_SD_CHANNEL1) {
        Flag = SVC_FW_UPD_TSK_FLG_SD1_RDY;
    }

    if ((SVC_FwUpdTskCtrlFlag & SVC_FW_UPD_TSK_FLG_START) == 0U) {
        RetVal = SVC_NG;
    } else {
        UINT32 Idx;
        SVC_FW_UPDATE_SCAN_PTN_s *pCurPtn;

        for (Idx = 0U; Idx < SVC_FwUpdTskCtrl.ScanCtrl.NumOfPattern; Idx ++) {
            pCurPtn = &(SVC_FwUpdTskCtrl.ScanCtrl.pScanPtns[Idx]);
            if ((pCurPtn->Status & SVC_FW_UPD_TSK_SCN_PTN_ON) > 0U) {
                pCurPtn->Status &= ~(SVC_FW_UPD_TSK_SCN_PTN_DONE);
                pCurPtn->Status &= ~(SVC_FW_UPD_TSK_SCN_PTN_UPD);
            }

        }

        PRetVal = AmbaKAL_EventFlagClear(&(SVC_FwUpdTskCtrl.EventFlag), Flag); PRN_FWU_TSK_ERR_HDLR
    }

    return RetVal;
}

static UINT32 SvcFwUpdateTask_DiagBoxCb(UINT32 *pIsSuspend)
{
    UINT32             RetVal = SVC_OK, MainStgCh = SVC_APP_STAT_STG_C_READY, PRetVal;
    SVC_APP_STAT_STG_s SdStatus = {0};
    SVC_USER_PREF_s    *pUserPref;

    PRetVal = SvcUserPref_Get(&pUserPref); PRN_FWU_TSK_ERR_HDLR
    if (pUserPref->MainStgChan == AMBA_SD_CHANNEL1) {
        MainStgCh = SVC_APP_STAT_STG_D_READY;
    }

    if (pIsSuspend != NULL) {
        RetVal = SvcSysStat_Get(SVC_APP_STAT_STG, &SdStatus);
        if (SVC_OK == RetVal) {
            if ((SdStatus.Status & MainStgCh) > 0U) {
                *pIsSuspend = 0U;
            } else {
                *pIsSuspend = 1U;
            }
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 SvcFwUpdateTask_UpdateProc(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SVC_FwUpdTskCtrlFlag & SVC_FW_UPD_TSK_FLG_START) == 0U) {
        RetVal = SVC_NG;
        LOG_FWU_TSK_NG("Failure to update firmware - start firmware update task first!", 0U, 0U);
    } else {
        UINT32 Idx;
        SVC_FW_UPDATE_SCAN_PTN_s *pCurPtn = &(SvcFwUpdTskScnPtns[0U]); AmbaMisra_TouchUnused(pCurPtn);

        for (Idx = 0U; Idx < SVC_FwUpdTskCtrl.ScanCtrl.NumOfPattern; Idx ++) {

            pCurPtn = &(SVC_FwUpdTskCtrl.ScanCtrl.pScanPtns[Idx]);
            if (((pCurPtn->Status & SVC_FW_UPD_TSK_SCN_PTN_UPD)  >  0U) &&
                ((pCurPtn->Status & SVC_FW_UPD_TSK_SCN_PTN_DONE) == 0U)) {
                RetVal = SvcFwUpdateTask_UpdatePreProc(pCurPtn);
                if (RetVal == SVC_OK) {
#ifdef CONFIG_LINUX
                    RetVal = SvcFwUpdatetask_FwRename(pCurPtn);
                    if (RetVal != 0U) {
                        PRN_FWU_TSK_NG "Rename filename fail"
                            SVC_PRN_ARG_S SVC_LOG_FWU_TSK
                            SVC_PRN_ARG_PROC SvcLog_NG
                            SVC_PRN_ARG_E
                    }
#endif
                    RetVal = SvcFwUpdate_Update(pCurPtn->FilePath, 0U);
                    if (RetVal == SVC_OK) {
                        pCurPtn->Status |= SVC_FW_UPD_TSK_SCN_PTN_DONE;
                        PRetVal = SvcFwUpdateTask_UpdatePostProc(pCurPtn); PRN_FWU_TSK_ERR_HDLR
                        pCurPtn->Status = SVC_FW_UPD_TSK_SCN_PTN_OFF;
                    } else {
                        PRN_FWU_TSK_NG "Failure to update firmware - update firmware fail %s"
                            SVC_PRN_ARG_S SVC_LOG_FWU_TSK
                            SVC_PRN_ARG_PROC SvcLog_NG
                            SVC_PRN_ARG_CSTR pCurPtn->FilePath SVC_PRN_ARG_POST
                            SVC_PRN_ARG_E
                    }
                }
            }

            if (RetVal != SVC_OK) {
                break;
            }

        }
    }

    return RetVal;
}

static UINT32 SvcFwUpdateTask_UpdatePreProc(SVC_FW_UPDATE_SCAN_PTN_s *pCurPtn)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    if (pCurPtn == NULL) {
        RetVal = SVC_NG;
        AmbaMisra_TouchUnused(pCurPtn);
    } else if (pResCfg == NULL) {
        RetVal = SVC_NG;
    } else {
        UINT32 RetState = 0U;
        SVC_DIAG_BOX_FORM_s DiagBox;
        SVC_FW_UPDATE_DIAGBOX_WIN_INFO_s SvcFwUpdWinInfo = {
            660U, 280U, 1920U, 1080U
        };
        UINT32 ActiveVoutID = SVC_FwUpdTskCtrl.ActiveVoutID;
        const AMBA_FPD_OBJECT_s *pCurVoutDriver;
        UINT32 ActualFlag = 0U;

        AmbaMisra_TypeCast(&(pCurVoutDriver), &(pResCfg->DispStrm[0].pDriver));
        if (pCurVoutDriver != NULL) {   // If MAXIM ZS LCD is exist, draw GUI to it.
            if (0 == SvcWrap_strcmp("MAXIM ZS095BH3001A8H3 BII", pCurVoutDriver->pName)) {
                if (pResCfg->DispStrm[0].VoutID < AMBA_DSP_MAX_VOUT_NUM) {
                    ActiveVoutID = pResCfg->DispStrm[0].VoutID;
                    SvcFwUpdWinInfo.DiagBoxWidth  = 480U;
                    SvcFwUpdWinInfo.DiagBoxHeight = 200U;
                    SvcFwUpdWinInfo.OsdBufWidth   = 1920U;
                    SvcFwUpdWinInfo.OsdBufHeight  = 480U;
                } else {
                    RetVal = SVC_NG;
                }
            }
        }

        if (RetVal == SVC_OK) {
            UINT32 OsdWidth = 0U, OsdHeight = 0U;

            if (0U == SvcOsd_GetOsdBufSize(ActiveVoutID, &OsdWidth, &OsdHeight)) {
                if ((OsdWidth > 0U) && (OsdHeight > 0U)) {
                    SvcFwUpdWinInfo.DiagBoxWidth  = ( OsdWidth * SvcFwUpdWinInfo.DiagBoxWidth ) / SvcFwUpdWinInfo.OsdBufWidth;
                    SvcFwUpdWinInfo.DiagBoxHeight = ( OsdHeight * SvcFwUpdWinInfo.DiagBoxHeight ) / SvcFwUpdWinInfo.OsdBufHeight;
                    SvcFwUpdWinInfo.OsdBufWidth   = OsdWidth;
                    SvcFwUpdWinInfo.OsdBufHeight  = OsdHeight;
                }
            }

            SvcFwUpdateTask_memset(&DiagBox, 0, sizeof(DiagBox));
            DiagBox.ActFlag |= SVC_DIAG_BOX_ENA_BTN_YES;
            DiagBox.ActFlag |= SVC_DIAG_BOX_ENA_BTN_NO;
            DiagBox.ActFlag |= SVC_DIAG_BOX_SEL_BTN_NO;
            DiagBox.VoutIdx    = ActiveVoutID;
            DiagBox.GuiLevel   = SVC_FW_UPD_TSK_GUI_LEVEL;
            DiagBox.FormWidth  = SvcFwUpdWinInfo.DiagBoxWidth;
            DiagBox.FormHeight = SvcFwUpdWinInfo.DiagBoxHeight;
            DiagBox.FormStartX = (SvcFwUpdWinInfo.OsdBufWidth  - DiagBox.FormWidth) >> 1U;
            DiagBox.FormStartY = (SvcFwUpdWinInfo.OsdBufHeight - DiagBox.FormHeight) >> 1U;
            DiagBox.FormExtraOsdWidth  = SVC_FW_UPD_TSK_OSD_WIDTH;
            DiagBox.FormExtraOsdHeight = SVC_FW_UPD_TSK_OSD_HEIGHT;
            SvcWrap_strcpy(DiagBox.FormExtraOsdPtn, sizeof(DiagBox.FormExtraOsdPtn), SVC_FW_UPD_TSK_OSD_PTN);

            if ((SVC_FwUpdTskCtrlFlag & SVC_FW_UPD_TSK_FLG_GUI_RDY) == 0U) {
                PRetVal = SvcDataLoader_LoadCallback(SvcFwUpdateTask_GuiLoad); PRN_FWU_TSK_ERR_HDLR
                PRetVal = AmbaKAL_EventFlagGet(&(SVC_FwUpdTskCtrl.EventFlag),
                                               SVC_FW_UPD_TSK_FLG_GUI_RDY,
                                               AMBA_KAL_FLAGS_ALL,
                                               AMBA_KAL_FLAGS_CLEAR_NONE,
                                               &ActualFlag,
                                               30000U); PRN_FWU_TSK_ERR_HDLR
            }

            if (SVC_OK == SvcDiagBox_Create(&DiagBox)) {

                LOG_FWU_TSK_API("Create diag box for VOUT %d", DiagBox.VoutIdx, 0U);

                if (SVC_OK != SvcDiagBox_Show(&RetState, AMBA_KAL_WAIT_FOREVER, SvcFwUpdateTask_DiagBoxCb)) {
                    RetVal = SVC_NG;
                }
                PRetVal = SvcDiagBox_Delete(); PRN_FWU_TSK_ERR_HDLR
            }

            if (RetState == 0U) {
                RetVal = SVC_NG;
            } else {
                if ((RetState & SVC_DIAG_BOX_CLI_BTN_YES) == 0U) {
                    RetVal = SVC_NG;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 SvcFwUpdateTask_UpdatePostProc(SVC_FW_UPDATE_SCAN_PTN_s *pCurPtn)
{
    UINT32 RetVal = SVC_OK;

    if (pCurPtn != NULL) {
        if (((pCurPtn->Status & SVC_FW_UPD_TSK_SCN_PTN_UPD) > 0U) &&
            ((pCurPtn->Status & SVC_FW_UPD_TSK_SCN_PTN_DONE) > 0U)) {
#ifndef CONFIG_LINUX
            RetVal = SvcFwUpdatetask_FwRename(pCurPtn);
#else
            AmbaMisra_TouchUnused(&RetVal);
            AmbaMisra_TouchUnused(pCurPtn);
#endif
        }
    }

    return RetVal;
}

static UINT32 SvcFwUpdateTask_ReBootProc(void)
{
    UINT32 PRetVal;
    INT32 Idx;

    for (Idx = 3; Idx > 0; Idx --) {

        PRN_FWU_TSK_API "Update firmware done. Reboot system ... Countdown %d sec!"
            SVC_PRN_ARG_S SVC_LOG_FWU_TSK
            SVC_PRN_ARG_INT32 Idx SVC_PRN_ARG_POST
            SVC_PRN_ARG_E
        PRetVal = AmbaKAL_TaskSleep(1000U); PRN_FWU_TSK_ERR_HDLR
    }

    PRetVal = AmbaSYS_Reboot(); PRN_FWU_TSK_ERR_HDLR

    return SVC_OK;
}

static void* SvcFwUpdateTask_TaskEntry(void* EntryArg)
{
    UINT32             RetVal = SVC_NG, PRetVal;
    UINT32             ActualFlag = 0U;
    UINT32             WaitFlag = SVC_FW_UPD_TSK_FLG_SD0_RDY, MainStgCh = SVC_APP_STAT_STG_C_READY;
    SVC_APP_STAT_STG_s SdStatus = {0};
    SVC_USER_PREF_s    *pUserPref;

    PRetVal = SvcUserPref_Get(&pUserPref); PRN_FWU_TSK_ERR_HDLR
    if (pUserPref->MainStgChan == AMBA_SD_CHANNEL1) {
        MainStgCh = SVC_APP_STAT_STG_D_READY;
        WaitFlag = SVC_FW_UPD_TSK_FLG_SD1_RDY;
    }

    // Wait dsp boot done to make sure vout osd is ready.
    PRetVal = AmbaKAL_EventFlagGet(&(SVC_FwUpdTskCtrl.EventFlag),
                                   SVC_FW_UPD_TSK_FLG_SYS_RDY,
                                   AMBA_KAL_FLAGS_ALL,
                                   AMBA_KAL_FLAGS_CLEAR_NONE,
                                   &ActualFlag,
                                   AMBA_KAL_WAIT_FOREVER); PRN_FWU_TSK_ERR_HDLR

    while (SVC_OK == AmbaKAL_EventFlagGet(&(SVC_FwUpdTskCtrl.EventFlag),
                                          WaitFlag,
                                          AMBA_KAL_FLAGS_ALL,
                                          AMBA_KAL_FLAGS_CLEAR_NONE,
                                          &ActualFlag,
                                          AMBA_KAL_WAIT_FOREVER)) {
        RetVal = SvcFwUpdateTask_ScanProc();
        if (RetVal == SVC_OK) {
            RetVal = SvcFwUpdateTask_UpdateProc();
        }

        if (RetVal != SVC_OK) {
            do {
                PRetVal = AmbaKAL_TaskSleep(SVC_FW_UPD_TSK_SCN_INTERVAL); PRN_FWU_TSK_ERR_HDLR
                PRetVal = SvcSysStat_Get(SVC_APP_STAT_STG, &SdStatus); PRN_FWU_TSK_ERR_HDLR
            } while ((SdStatus.Status & MainStgCh) > 0U);

            PRetVal = SvcFwUpdateTask_ScanReset(); PRN_FWU_TSK_ERR_HDLR
        } else {
            break;
        }
    }
    AmbaMisra_TouchUnused(EntryArg);

    if (RetVal == SVC_OK) {
        /* Reboot system */
        PRetVal = SvcFwUpdateTask_ReBootProc(); PRN_FWU_TSK_ERR_HDLR
    }

    return NULL;
}

static void SvcFwUpdateTask_SDStateCallback(UINT32 StatIdx, void *pInfo)
{
    UINT32 PRetVal;
    UINT32             MainStgCh = SVC_APP_STAT_STG_C_READY, Flag = SVC_FW_UPD_TSK_FLG_SD0_RDY;
    SVC_APP_STAT_STG_s *pSdStatus = NULL;
    SVC_USER_PREF_s    *pUserPref;

    PRetVal = SvcUserPref_Get(&pUserPref); PRN_FWU_TSK_ERR_HDLR
    if (pUserPref->MainStgChan == AMBA_SD_CHANNEL1) {
        MainStgCh = SVC_APP_STAT_STG_D_READY;
        Flag = SVC_FW_UPD_TSK_FLG_SD1_RDY;
    }

    AmbaMisra_TouchUnused(pSdStatus);
    AmbaMisra_TouchUnused(pInfo);

    if (StatIdx == SVC_APP_STAT_STG) {
        AmbaMisra_TypeCast(&pSdStatus, &pInfo);
        if ((pSdStatus->Status & MainStgCh) > 0U) {
            if (SVC_OK != AmbaKAL_EventFlagSet(&(SVC_FwUpdTskCtrl.EventFlag), Flag)) {
                SvcLog_NG(SVC_LOG_FWU_TSK, "SVC_FwUpdTskEventFlag set failed", 0U, 0U);
            }
        }
    }
}

static void SvcFwUpdateTask_BootStateCallback(UINT32 StatIdx, void *pInfo)
{
    SVC_APP_STAT_STG_s       *pSdStatus = NULL;
    SVC_APP_STAT_DSP_BOOT_s  DspStatus;

    AmbaMisra_TouchUnused(pSdStatus);
    AmbaMisra_TouchUnused(pInfo);

    if (StatIdx == SVC_APP_STAT_DSP_BOOT) {
        if (SvcSysStat_Get(SVC_APP_STAT_DSP_BOOT, &DspStatus) == SVC_OK) {
            if (DspStatus.Status == SVC_APP_STAT_DSP_BOOT_DONE) {
               if (SVC_OK != AmbaKAL_EventFlagSet(&(SVC_FwUpdTskCtrl.EventFlag),
                                                  SVC_FW_UPD_TSK_FLG_SYS_RDY)) {
                   SvcLog_NG(SVC_LOG_FWU_TSK, "SVC_FwUpdTskEventFlag set failed", 0U, 0U);
               }
           }
        }
    }
}

/**
 * Initial firmware update task
 *
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcFwUpdateTask_Init(void)
{
    UINT32           RetVal = SVC_OK, PRetVal;
    SVC_USER_PREF_s  *pUserPref;

    if ((SVC_FwUpdTskCtrlFlag & SVC_FW_UPD_TSK_FLG_INIT) > 0U) {
        LOG_FWU_TSK_API("The firmware update task module has been initialized!", 0U, 0U);
    } else {
        ULONG  BufBase = 0U;
        UINT32 BufSize = 0U;

        SvcFwUpdateTask_memset(SvcFwUpdTskScnPtns, 0, sizeof(SvcFwUpdTskScnPtns));
        SvcFwUpdTskScnPtns[0U] = (SVC_FW_UPDATE_SCAN_PTN_s) { SVC_FW_UPD_TSK_SCN_PTN_ON, SVC_FW_UPD_TSK_SCN_PTN0 };
        SvcFwUpdTskScnPtns[1U] = (SVC_FW_UPDATE_SCAN_PTN_s) { SVC_FW_UPD_TSK_SCN_PTN_ON, SVC_FW_UPD_TSK_SCN_PTN1 };
        SvcFwUpdTskScnPtns[2U] = (SVC_FW_UPDATE_SCAN_PTN_s) { SVC_FW_UPD_TSK_SCN_PTN_ON, SVC_FW_UPD_TSK_SCN_PTN2 };

        PRetVal = SvcUserPref_Get(&pUserPref); PRN_FWU_TSK_ERR_HDLR
        SvcFwUpdTskScnPtns[0U].FilePath[0] = pUserPref->MainStgDrive[0];
        SvcFwUpdTskScnPtns[1U].FilePath[0] = pUserPref->MainStgDrive[0];
        SvcFwUpdTskScnPtns[2U].FilePath[0] = pUserPref->MainStgDrive[0];

        // Reset the firmware update task module setting
        SvcFwUpdateTask_memset(&SVC_FwUpdTskCtrl, 0, sizeof(SVC_FwUpdTskCtrl));
        SVC_FwUpdTskCtrl.ActiveVoutID = VOUT_IDX_B;

        // Configure scan control
        SVC_FwUpdTskCtrl.ScanCtrl.NumOfPattern = SVC_FW_UPD_TSK_SCN_PTN_NUM;
        SVC_FwUpdTskCtrl.ScanCtrl.pScanPtns    = SvcFwUpdTskScnPtns;
        AmbaUtility_StringAppend(SVC_FwUpdTskCtrl.Name, (UINT32)sizeof(SVC_FwUpdTskCtrl.Name), "SVC_FwUpdTskEventFlag");

        // Get the fw update module memory
        RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_TRANSIENT_BUF, &BufBase, &BufSize);
        if (RetVal != SVC_OK) {
            LOG_FWU_TSK_NG("Failure to initial firmware update task - get the memory base/size fail", 0U, 0U);
        } else {
            if (BufBase == 0U) {
                RetVal = SVC_NG;
                LOG_FWU_TSK_NG("Failure to initial firmware update task - invalid memory address", 0U, 0U);
            }

            if (BufSize == 0U) {
                RetVal = SVC_NG;
                LOG_FWU_TSK_NG("Failure to initial firmware update task - invalid memory size", 0U, 0U);
            }
        }

        // Create the SVC firmware update module
        if (RetVal == SVC_OK) {
            RetVal = SvcFwUpdate_Create(BufBase, BufSize);
            if (RetVal != SVC_OK) {
                LOG_FWU_TSK_NG("Failure to initial firmware update task - create firmware update module fail!", 0U, 0U);
            }
        }

        if (RetVal == SVC_OK) {
            RetVal = AmbaKAL_EventFlagCreate(&(SVC_FwUpdTskCtrl.EventFlag), SVC_FwUpdTskCtrl.Name);
            if (RetVal != SVC_OK) {
                LOG_FWU_TSK_API("Failure to initial firmware update task - create event flag fail!", 0U, 0U);
            } else {
                PRetVal = AmbaKAL_EventFlagClear(&(SVC_FwUpdTskCtrl.EventFlag), 0xFFFFFFFFU); PRN_FWU_TSK_ERR_HDLR
            }
        }

        if (RetVal == SVC_OK) {
            UINT32 CtrlID;
            RetVal = SvcSysStat_Register(SVC_APP_STAT_STG, SvcFwUpdateTask_SDStateCallback, &CtrlID);
            if (RetVal != SVC_OK) {
                LOG_FWU_TSK_NG("Failure to initial firmware update task - register sd card callback fail!", 0U, 0U);
            }
            RetVal = SvcSysStat_Register(SVC_APP_STAT_DSP_BOOT, SvcFwUpdateTask_BootStateCallback, &CtrlID);
            if (RetVal != SVC_OK) {
                LOG_FWU_TSK_NG("Failure to initial firmware update task - register sd card callback fail!", 0U, 0U);
            }
        }

        if (RetVal == SVC_OK) {
            SVC_FwUpdTskCtrlFlag |= SVC_FW_UPD_TSK_FLG_INIT;
            LOG_FWU_TSK_OK("Successful to initial firmware update task", 0U, 0U);
        }
    }

    return RetVal;
}

/**
 * Start firmware update task
 *
 * @return ErrCode SVC_OK(0)/SVC_NG(1)
 */
UINT32 SvcFwUpdateTask_Start(void)
{
    UINT32 RetVal = SVC_OK;

    if ((SVC_FwUpdTskCtrlFlag & SVC_FW_UPD_TSK_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_FWU_TSK_NG("Failure to start firmware update task - initial firmware update task first!", 0U, 0U);
    } else if ((SVC_FwUpdTskCtrlFlag & SVC_FW_UPD_TSK_FLG_START) > 0U) {
        LOG_FWU_TSK_API("The firmware update task has been started!", 0U, 0U);
    } else {
        // Create scan task setting
        SvcFwUpdateTask_memset(SVC_FwUpdTskCtrl.ScanTskStack, 0, sizeof(SVC_FwUpdTskCtrl.ScanTskStack));

        SVC_FwUpdTskCtrl.ScanTskCtrl.Priority   = (UINT32)SVC_FW_UPDATE_TASK_PRI;
        SVC_FwUpdTskCtrl.ScanTskCtrl.EntryFunc  = SvcFwUpdateTask_TaskEntry;
        SVC_FwUpdTskCtrl.ScanTskCtrl.pStackBase = SVC_FwUpdTskCtrl.ScanTskStack;
        SVC_FwUpdTskCtrl.ScanTskCtrl.StackSize  = SVC_FW_UPD_TSK_SCN_STK_SIZE;
        SVC_FwUpdTskCtrl.ScanTskCtrl.CpuBits    = SVC_FW_UPDATE_TASK_CPU_BITS;
        SVC_FwUpdTskCtrl.ScanTskCtrl.EntryArg   = 0U;

        RetVal = SvcTask_Create(SVC_FW_UPD_TSK_NAME, &(SVC_FwUpdTskCtrl.ScanTskCtrl));
        if (SVC_OK != RetVal) {
            LOG_FWU_TSK_NG("Failure to start firmware update task - create scan task fail", 0U, 0U);
        } else {
            SVC_FwUpdTskCtrlFlag |= SVC_FW_UPD_TSK_FLG_START;
            LOG_FWU_TSK_OK("Successful to start firmware update task", 0U, 0U);
        }
    }

    return RetVal;
}

void SvcFwUpdateTask_GuiType(UINT32 Type)
{
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

#ifdef SVC_MENU_TASK_H
    switch (Type) {
    case SVC_MENU_TASK_TYPE_LV_EMR :
        SVC_FwUpdTskCtrl.ActiveVoutID = VOUT_IDX_A;
        break;
    case SVC_MENU_TASK_TYPE_LV_ADAS_DVR :
#if (AMBA_DSP_MAX_VOUT_NUM > 1U)
        SVC_FwUpdTskCtrl.ActiveVoutID = VOUT_IDX_B;
#else
        SVC_FwUpdTskCtrl.ActiveVoutID = VOUT_IDX_A;
#endif
        break;
    case SVC_MENU_TASK_TYPE_LV_DMS :
#if (AMBA_DSP_MAX_VOUT_NUM > 1U)
        SVC_FwUpdTskCtrl.ActiveVoutID = VOUT_IDX_B;
#else
        SVC_FwUpdTskCtrl.ActiveVoutID = VOUT_IDX_A;
#endif
        break;
    case SVC_MENU_TASK_TYPE_PLAYBACK :
#if (AMBA_DSP_MAX_VOUT_NUM > 1U)
        SVC_FwUpdTskCtrl.ActiveVoutID = VOUT_IDX_B;
#else
        SVC_FwUpdTskCtrl.ActiveVoutID = VOUT_IDX_A;
#endif
        break;
    case SVC_MENU_TASK_TYPE_SURROUND :
#if (AMBA_DSP_MAX_VOUT_NUM > 1U)
        SVC_FwUpdTskCtrl.ActiveVoutID = VOUT_IDX_B;
#else
        SVC_FwUpdTskCtrl.ActiveVoutID = VOUT_IDX_A;
#endif
        break;
    default :
        SVC_FwUpdTskCtrl.ActiveVoutID = VOUT_IDX_B;
        break;
    }
#else
    SVC_FwUpdTskCtrl.ActiveVoutID = VOUT_IDX_B;
    AmbaMisra_TouchUnused(&Type);
#endif

    if (pResCfg != NULL) {
        extern AMBA_FPD_OBJECT_s AmbaFPD_HDMI_Obj GNU_WEAK_SYMBOL;
        const AMBA_FPD_OBJECT_s *pFpdObj = &AmbaFPD_HDMI_Obj;

        AmbaMisra_TouchUnused(&pFpdObj);

        if (pFpdObj != NULL) {
            UINT32 DispIdx;

            for (DispIdx = 0; DispIdx < pResCfg->DispNum; DispIdx++) {
                if ((pResCfg->DispBits & (0x1UL << DispIdx)) > 0U) {
                    if (pResCfg->DispStrm[DispIdx].VoutID == VOUT_IDX_B) {
                        if (pResCfg->DispStrm[DispIdx].pDriver == pFpdObj) {
                            SVC_FwUpdTskCtrl.ActiveVoutID = VOUT_IDX_B;
                        }
                    }
                }
            }
        }
    }
}

static UINT32 SvcFwUpdateTask_GuiLoad(void)
{
    if ((SVC_FwUpdTskCtrlFlag & SVC_FW_UPD_TSK_FLG_GUI_RDY) == 0U) {
        UINT32 PRetVal;
        ULONG  BufBase = 0U;
        UINT32 BufSize = 0U;
        PRetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_TRANSIENT_BUF, &BufBase, &BufSize); PRN_FWU_TSK_ERR_HDLR
        if (PRetVal == 0U) {
            UINT8 *pBuf = NULL, *pExtraData = NULL;
            UINT32 ExtraDataSize = SVC_FW_UPD_TSK_OSD_WIDTH * SVC_FW_UPD_TSK_OSD_HEIGHT;
            UINT32 FileSize = 0U;

            AmbaMisra_TypeCast(&pBuf, &BufBase);
            SvcFwUpdateTask_memset(pBuf, 0, BufSize);

            if (pBuf == NULL) {
                LOG_FWU_TSK_NG("Fail to load fwupdate gui - invalid working buffer", 0U, 0U);
            } else if (0U != SvcNvm_GetRomFileSize(AMBA_USER_PARTITION_SYS_DATA, SVC_FW_UPD_TSK_OSD_PTN, &FileSize)) {
                LOG_FWU_TSK_NG("Fail to load fwupdate gui - get NVM file size fail!", 0U, 0U);
            } else if (FileSize == 0U) {
                LOG_FWU_TSK_NG("Fail to load fwupdate gui - NVM file size should not zero!", 0U, 0U);
            } else if((FileSize << 1U) > BufSize) {
                LOG_FWU_TSK_NG("Fail to load fwupdate gui - working buf size(0x%X) < request size(0x%X)", BufSize, FileSize << 1U);
            } else {
                ExtraDataSize = FileSize;
    #ifdef CONFIG_ICAM_32BITS_OSD_USED
                ExtraDataSize -= 54U;
    #else
                ExtraDataSize -= 1078U;
    #endif
                pExtraData = pBuf;
                pBuf = &(pBuf[ExtraDataSize]);
                BufSize -= ExtraDataSize;

                PRetVal = SvcNvm_ReadRomFileCrc(AMBA_USER_PARTITION_SYS_DATA, SVC_FW_UPD_TSK_OSD_PTN, 0U, FileSize, pBuf, 10000);
                if (PRetVal != 0U) {
                    LOG_FWU_TSK_NG("Fail to load fwupdate gui - read NVM file fail! ErrCode(0x%08X)", PRetVal, 0U);
                } else {
                    UINT8 *pData, *pDst;
                    UINT32 DataOfs = 0U;
                    INT32  Width = 0, Height = 0, RowIdx;

                    /*
                     * -- Bitmap header --
                     * 2-byte : Type        offset 0
                     * 4-byte : Size        offset 2
                     * 4-byte : reserved    offset 6
                     * 4-byte : offset      offset 10
                     */
                    PRetVal = AmbaWrap_memcpy(&DataOfs, &pBuf[10], sizeof(UINT32)); PRN_FWU_TSK_ERR_HDLR
                    PRetVal = AmbaWrap_memcpy(&Width,   &pBuf[18], sizeof(INT32));  PRN_FWU_TSK_ERR_HDLR
                    PRetVal = AmbaWrap_memcpy(&Height,  &pBuf[22], sizeof(INT32));  PRN_FWU_TSK_ERR_HDLR

                    if (DataOfs == 0U) {
                        LOG_FWU_TSK_NG("Fail to load fwupdate gui - data offset should not zero", 0U, 0U);
                    } else if ((Width == 0) || (Height == 0)) {
                        LOG_FWU_TSK_NG("Fail to load fwupdate gui - invalid window %dx%d", (UINT32)Width, (UINT32)Height);
                    } else {
                        pData = &(pBuf[DataOfs]);

#ifdef CONFIG_ICAM_32BITS_OSD_USED
                        Width *= 4;
#endif
                        for (RowIdx = 0; RowIdx < Height; RowIdx ++) {
                            pDst  = &(pExtraData[((Height - 1) - RowIdx) * Width]);

                            PRetVal = AmbaWrap_memcpy(pDst, pData, (UINT32)Width); PRN_FWU_TSK_ERR_HDLR

                            pData = &(pData[Width]);
                        }

                        LOG_FWU_TSK_API("Load fwupdate GUI done", 0U, 0U);

                        AmbaMisra_TouchUnused(pData);
                    }
                }
            }

            SvcDiagBox_OsdLoad(pBuf, BufSize, pExtraData, ExtraDataSize);

            LOG_FWU_TSK_API("Load Diagbox GUI done", 0U, 0U);
        }

        PRetVal = AmbaKAL_EventFlagSet(&(SVC_FwUpdTskCtrl.EventFlag), SVC_FW_UPD_TSK_FLG_GUI_RDY); PRN_FWU_TSK_ERR_HDLR

        SVC_FwUpdTskCtrlFlag |= SVC_FW_UPD_TSK_FLG_GUI_RDY;
    }

    return 0U;
}

static UINT32 SvcFwUpdatetask_FwRename(SVC_FW_UPDATE_SCAN_PTN_s *pCurPtn)
{
    UINT32 RetVal = SVC_NG, PRetVal;

    if (pCurPtn != NULL) {
        char *pStr, NewFileName[64];
        AMBA_FS_FILE_INFO_s FileInfo;

        SvcFwUpdateTask_memset(NewFileName, 0, sizeof(NewFileName));
        SvcWrap_strcpy(NewFileName, sizeof(NewFileName), pCurPtn->FilePath);
        pStr = SvcWrap_strrchr(NewFileName, 46);

        if (pStr != NULL) {
            pStr[1] = 'n';
            pStr[2] = 'e';
            pStr[3] = 'w';
            pStr[4] = '\0';

            SvcFwUpdateTask_memset(&FileInfo, 0, sizeof(FileInfo));
            PRetVal = AmbaFS_GetFileInfo(NewFileName, &FileInfo);
            if (PRetVal != 0U) {
                FileInfo.Size = 0ULL;
            }
            if (FileInfo.Size > 0ULL) {
                if (0U != AmbaFS_Remove(NewFileName)) {
                    PRN_FWU_TSK_NG "Failure to remove older firmware file %s"
                        SVC_PRN_ARG_S SVC_LOG_FWU_TSK
                        SVC_PRN_ARG_CSTR NewFileName SVC_PRN_ARG_POST
                        SVC_PRN_ARG_E
                }
            }

            if (0U != AmbaFS_Rename(pCurPtn->FilePath, NewFileName)) {
                PRN_FWU_TSK_NG "Failure to rename firmware file from %s to %s"
                    SVC_PRN_ARG_S SVC_LOG_FWU_TSK
                    SVC_PRN_ARG_CSTR pCurPtn->FilePath SVC_PRN_ARG_POST
                    SVC_PRN_ARG_CSTR NewFileName       SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
            } else {
                SvcWrap_strcpy(pCurPtn->FilePath, sizeof(pCurPtn->FilePath), NewFileName);
                RetVal = SVC_OK;
            }
        }
    }

    AmbaMisra_TouchUnused(pCurPtn);

    return RetVal;
}


