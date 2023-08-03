/**
*  @file SvcStgMonitor.c
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
*  @details SD Card Monitor
*
*/

#ifdef CONFIG_ICAM_USB_USED
#include "AmbaUSBH_API.h"
#endif
#include "SvcErrCode.h"
#include "AmbaKAL.h"
#include "AmbaFS.h"
#include "AmbaSD.h"
#include "AmbaMisraFix.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaDCF.h"

#include "SvcTask.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#ifdef CONFIG_BUILD_COMMON_SERVICE_DCF
#include "AmbaDCF.h"
#endif
#include "SvcStgMgr.h"
#include "SvcShell.h"
#ifdef CONFIG_ICAM_USB_USED
#include "AmbaSvcUsb.h"
#endif
#include "SvcPlat.h"
#include "SvcStgMonitor.h"
#include "SvcSafeStateMgr.h"
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#include "AmbaSD_Ctrl.h"
#include "app_helper.h"
#define tolower(c)          ((c) + 0x20 * (((c) >= 'A') && ((c) <= 'Z')))
#endif

#define SVC_STG_MONITOR             "STGMON"
#define SVC_STG_MONITOR_STACK_SIZE  (0x8000U)
#define SVC_STG_MONITOR_TIMEOUT     (10000U)

static UINT32 StgDeviceInitStatus = 0U; /* Init done(1U) / Not Init yet(0U) */

static SVC_STG_MONI_DRIVE_INFO_s DriveInfo[SVC_STG_DRIVE_NUM] = {
    {.Drive = 'C', .DriveID = (UINT8)AMBA_DCF_DRIVE_0, .DriveType = SVC_STG_MONI_TYPE_SD, .IsExist = 0U},
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
    {.Drive = 'D', .DriveID = (UINT8)AMBA_DCF_DRIVE_1, .DriveType = SVC_STG_MONI_TYPE_SD, .IsExist = 0U},
    {.Drive = 'I', .DriveID = (UINT8)AMBA_DCF_DRIVE_2, .DriveType = SVC_STG_MONI_TYPE_USB, .IsExist = 0U}
#endif
};

static void* StgMonitor_TaskEntry(void* EntryArg);
static UINT32 DeletFirstFile(char Drive, UINT64 *pFileSize);

static STG_MONI_NOTIFY_CB  NotifyCallBackFunc = NULL;
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
#ifdef CONFIG_ICAM_USB_USED
static UINT32 USBHostCheck GNU_SECTION_NOZEROINIT;
static UINT32 SvcHostEventCallback(UINT32 Code, UINT32 SubCode)
{
    AmbaMisra_TouchUnused(&SubCode);
    switch (Code) {
    case USBH_EVENT_DEVICE_NO_RESPONSE:
        SvcLog_OK(SVC_STG_MONITOR, "[USBH][SVC] Device No Respond", 0, 0);
        break;
    case USBH_EVENT_DEVICE_NOT_SUPPORT:
        SvcLog_OK(SVC_STG_MONITOR, "[USBH][SVC] Device Not Support", 0, 0);
        break;
    case USBH_EVENT_DEVICE_OVER_CURRENT:
        SvcLog_OK(SVC_STG_MONITOR, "[USBH][SVC] Device Over Current", 0, 0);
        break;
    case USBH_EVENT_HUB_NOT_SUPPORT:
        SvcLog_OK(SVC_STG_MONITOR, "[USBH][SVC] Hub Not Support", 0, 0);
        break;
    case USBH_EVENT_DEVICE_INSERTED:
        USBHostCheck = 1U;
        SvcLog_OK(SVC_STG_MONITOR, "[USBH][SVC] USB MSC Host is inserted", 0U, 0U);
        break;
    case USBH_EVENT_DEVICE_REMOVED:
        USBHostCheck = 1U;
        SvcLog_OK(SVC_STG_MONITOR, "[USBH][SVC] Device Removed", 0U, 0U);
        break;
    default:
        SvcLog_OK(SVC_STG_MONITOR, "[USBH][SVC] Unknown Event: 0x%X", Code, 0);
        break;
    };

    return SVC_OK;
}
#endif

static void SvcScriptAutoRun(char Drive, UINT32 IsStgInstall)
{
    static UINT32 IsSvcScriptAutoRun = 0U;

    /* SVC Auto run if svc_auto_run.ash file exist */
    if ((1U == IsStgInstall) && (0U == IsSvcScriptAutoRun)) {
        UINT32               RetVal;
        char                 ScriptName[32U];
        const char           Name[30U] = {":\\svc_auto_run.ash"};
        AMBA_FS_FILE_INFO_s  Fstat;

        /* Setup script file name*/
        AmbaSvcWrap_MisraMemset(ScriptName, 0, sizeof(ScriptName));
        ScriptName[0] = Drive;
        ScriptName[1] = '\0';
        AmbaUtility_StringAppend(ScriptName, (UINT32)sizeof(Name), Name);

        RetVal = AmbaFS_GetFileInfo(ScriptName, &Fstat);
        if (AMBA_FS_ERR_NONE == RetVal) {
            RetVal = SvcShell_ExecScript(ScriptName);
            if (RetVal == SVC_OK) {
                IsSvcScriptAutoRun = 1U;
            }
        } else {
            //SvcLog_DBG(SVC_STG_MONITOR, "No svc_auto_run.ash", 0U, 0U);
        }
    } else if ((0U == IsStgInstall) && (1U == IsSvcScriptAutoRun)) {
        /* Reset IsSvcScriptAutoRun flag */
        IsSvcScriptAutoRun = 0U;
    } else {
        //Do nothing
    }
}
#endif

static void* StgMonitor_TaskEntry(void* EntryArg)
{
    UINT32                 i, Rval;
    AMBA_SD_CARD_STATUS_s  CardStatus;
#ifdef CONFIG_BUILD_COMMON_SERVICE_DCF
    UINT32                 LastValidDirNum;
#endif

    AmbaMisra_TouchUnused(EntryArg);

    SvcLog_DBG(SVC_STG_MONITOR, "In StgMonitor_TaskEntry", 0U, 0U);

    /* Monitor sd status 1 time/sec */
    while (AmbaKAL_TaskSleep(1000U) == SVC_OK) {
        for (i = 0U; i < SVC_STG_DRIVE_NUM; i++) {
            if (DriveInfo[i].DriveType == SVC_STG_MONI_TYPE_SD) {
                /* SD check */
                Rval = AmbaSD_GetCardStatus(DriveInfo[i].DriveID, &CardStatus);
                if ((Rval == SVC_OK) && (DriveInfo[i].IsExist == 0U)) {
                    if (CardStatus.CardIsInit != 0U) {
                        SvcLog_DBG(SVC_STG_MONITOR, "SD%u is inserted", DriveInfo[i].DriveID, 0U);

                        /* Init and get/detect storage device */
                        StgDeviceInitStatus = 1U;

                        /* mount drive if needed                */
                        /* ThreadX: mount is done at driver     */
                        /* QNX: mount op is done by application */
                        if (g_pPlatCbEntry != NULL) {
                            if (g_pPlatCbEntry->pCbMountOp != NULL) {
                                g_pPlatCbEntry->pCbMountOp(DriveInfo[i].Drive, 1U);
                            }
                        }
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                        if (tolower(DriveInfo[i].Drive) == tolower(SD_SLOT[0])) {
                            char DrivePath[32] = {0};
                            AMBA_FS_DRIVE_INFO_t TmpDriveInfo;
                            if (AmbaFS_GetDriveInfo(DriveInfo[i].Drive, &TmpDriveInfo) == 0U) {
                                UINT64 DriveMB = (UINT64)TmpDriveInfo.TotalClusters * (UINT64)TmpDriveInfo.SectorsPerCluster * (UINT64)TmpDriveInfo.BytesPerSector >> 20;
                                UINT64 TotalMB = (UINT64)AmbaSD_GetTotalSectors(SD_CHANNEL) * (UINT64)TmpDriveInfo.BytesPerSector >> 20;
                                SvcLog_DBG(SVC_STG_MONITOR, "SD Total:%lldMB, Drive:%lldMB", TotalMB, DriveMB);
                                if (DriveMB < (98 * TotalMB / 100)) {
                                    SvcLog_NG(SVC_STG_MONITOR, "small "SD_SLOT" disk. format it", 0U, 0U);
                                    AmbaFS_Format(DriveInfo[i].Drive);
                                }
                            }
                            DrivePath[0] = DriveInfo[i].Drive;
                            DrivePath[1] = ':';
                            DrivePath[2] = '\\';
                            DrivePath[3] = '\0';
                            if (AmbaFS_ChangeDir(DrivePath) != AMBA_FS_ERR_NONE) {
                                SvcLog_NG(SVC_STG_MONITOR, "can not access "SD_SLOT" disk, may invalid format. format it", 0U, 0U);
                                AmbaFS_Format(DriveInfo[i].Drive);
                            }
                        }
#endif
                        SvcStgMgr_TrigUpdFreeSize(DriveInfo[i].Drive, SVC_STG_MONITOR_TIMEOUT);
                        if (NotifyCallBackFunc != NULL) {
                            NotifyCallBackFunc(DriveInfo[i].Drive, 1U);
                        }
#ifdef CONFIG_BUILD_COMMON_SERVICE_DCF
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                        if (tolower(DriveInfo[i].Drive) == tolower(REC_DEST_DISK[0]))
#else
                        if (1)
#endif
                        {
                            if (SVC_OK != AmbaDCF_ConfigDrive(DriveInfo[i].DriveID, &(DriveInfo[i].Drive))) {
                                SvcLog_NG(SVC_STG_MONITOR, "AmbaDCF_ConfigDrive failed!", 0U, 0U);
                            }

                            if (SVC_OK != AmbaDCF_Init(DriveInfo[i].DriveID, AMBA_DCF_MOV_FMT_MP4)) {
                                SvcLog_NG(SVC_STG_MONITOR, "AmbaDCF_Init failed!", 0U, 0U);
                            }

                            if (SVC_OK != AmbaDCF_ScanRootStep1(DriveInfo[i].DriveID, &LastValidDirNum)) {
                                SvcLog_NG(SVC_STG_MONITOR, "AmbaDCF_ScanRootStep1 failed!", 0U, 0U);
                            }
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
                            if (SVC_OK != AmbaDCF_ScanRootStep2(DriveInfo[i].DriveID, LastValidDirNum)) {
                                SvcLog_NG(SVC_STG_MONITOR, "AmbaDCF_ScanRootStep2 failed!", 0U, 0U);
                            }
#endif
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                            AmbaDCF_SetReady(1);
#endif
                        }
#endif
                        if (NotifyCallBackFunc != NULL) {
                            NotifyCallBackFunc(DriveInfo[i].Drive, 2U);
                        }
                        DriveInfo[i].IsExist = 1U;
                        /* SVC auto run function when Storage install */
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
                        SvcScriptAutoRun(DriveInfo[i].Drive, 1U);
#endif

                        {
                            SVC_PWR_MONITOR_CFG_s Cfg;

                            AmbaSvcWrap_MisraMemset(&Cfg, 0, sizeof(SVC_PWR_MONITOR_CFG_s));
                            Cfg.ModuleID = (DriveInfo[i].DriveID == 0U)? (UINT32)AMBA_PSM_SD: (UINT32)AMBA_PSM_SDIO;
                            Cfg.Enable = AMBA_PSM_ENABLE;
                            Cfg.UpperBound = AMBA_PSM_PERCENT_7;
                            Cfg.LowerBound = AMBA_PSM_PERCENT_7;
                            Rval = SvcSafeStateMgr_SetPwrMonitor(&Cfg);
                            if (Rval != OK) {
                                SvcLog_NG(SVC_STG_MONITOR, "SvcSafeStateMgr_SetPwrMonitor return 0x%x", Rval, 0U);
                            }
                        }
                    }
                } else if ((Rval != SVC_OK) && (DriveInfo[i].IsExist == 1U)) {
                    SvcLog_DBG(SVC_STG_MONITOR, "SD%u is ejected", i, 0U);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                    if (tolower(DriveInfo[i].Drive) == tolower(REC_DEST_DISK[0])) {
                        AmbaDCF_SetReady(0);
                    }
#endif
                    /* unmount drive if needed                  */
                    /* ThreadX: unmount is done at driver       */
                    /* QNX: unmount op is done by application   */
                    if (g_pPlatCbEntry != NULL) {
                        if (g_pPlatCbEntry->pCbMountOp != NULL) {
                            g_pPlatCbEntry->pCbMountOp(DriveInfo[i].Drive, 0U);
                        }
                    }

                    if (NotifyCallBackFunc != NULL) {
                        NotifyCallBackFunc(DriveInfo[i].Drive, 0U);
                    }
                    SvcStgMgr_TrigUpdFreeSize(DriveInfo[i].Drive, SVC_STG_MONITOR_TIMEOUT);

#ifdef CONFIG_BUILD_COMMON_SERVICE_DCF
                    if (SVC_OK != AmbaDCF_Destroy(DriveInfo[i].DriveID)) {
                        SvcLog_NG(SVC_STG_MONITOR, "AmbaDCF_Destroy failed!", 0U, 0U);
                    }
#endif
                    DriveInfo[i].IsExist = 0U;
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
                    /* Storage uninstall */
                    SvcScriptAutoRun(DriveInfo[i].Drive, 0U);
#endif


                    {
                        SVC_PWR_MONITOR_CFG_s Cfg;

                        AmbaSvcWrap_MisraMemset(&Cfg, 0, sizeof(SVC_PWR_MONITOR_CFG_s));
                        Cfg.ModuleID = (DriveInfo[i].DriveID == 0U)? (UINT32)AMBA_PSM_SD: (UINT32)AMBA_PSM_SDIO;
                        Cfg.Enable = AMBA_PSM_DISABLE;
                        Rval = SvcSafeStateMgr_SetPwrMonitor(&Cfg);
                        if (Rval != OK) {
                            SvcLog_NG(SVC_STG_MONITOR, "SvcSafeStateMgr_SetPwrMonitor return 0x%x", Rval, 0U);
                        }
                    }
                } else {
                    /* do nothing */
                }
            } else {
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
#ifdef CONFIG_ICAM_USB_USED
                //SVC_STG_MONI_TYPE_USB
                if (USBHostCheck == 1U) {
                    USBHostCheck = 0U;
                    if (DriveInfo[i].IsExist == 0U) {
#ifdef CONFIG_BUILD_COMMON_SERVICE_DCF
                        if (SVC_OK != AmbaDCF_ConfigDrive(DriveInfo[i].DriveID, &(DriveInfo[i].Drive))) {
                            SvcLog_NG(SVC_STG_MONITOR, "AmbaDCF_ConfigDrive failed!", 0U, 0U);
                        }

                        if (SVC_OK != AmbaDCF_Init(DriveInfo[i].DriveID, AMBA_DCF_MOV_FMT_MP4)) {
                            SvcLog_NG(SVC_STG_MONITOR, "AmbaDCF_Init failed!", 0U, 0U);
                        }

                        if (SVC_OK != AmbaDCF_ScanRootStep1(DriveInfo[i].DriveID, &LastValidDirNum)) {
                            SvcLog_NG(SVC_STG_MONITOR, "AmbaDCF_ScanRootStep1 failed!", 0U, 0U);
                        }

                        if (SVC_OK != AmbaDCF_ScanRootStep2(DriveInfo[i].DriveID, LastValidDirNum)) {
                            SvcLog_NG(SVC_STG_MONITOR, "AmbaDCF_ScanRootStep2 failed!", 0U, 0U);
                        }
#endif

                        SvcStgMgr_TrigUpdFreeSize(DriveInfo[i].Drive, SVC_STG_MONITOR_TIMEOUT);

                        DriveInfo[i].IsExist = 1U;

                        if (NotifyCallBackFunc != NULL) {
                            NotifyCallBackFunc(DriveInfo[i].Drive, 1U);
                        }

                        /* SVC auto run function when Storage install */
                        SvcScriptAutoRun(DriveInfo[i].Drive, 1U);
                    } else if (DriveInfo[i].IsExist == 1U) {
                        SvcStgMgr_TrigUpdFreeSize(DriveInfo[i].Drive, SVC_STG_MONITOR_TIMEOUT);

#ifdef CONFIG_BUILD_COMMON_SERVICE_DCF
                        if (SVC_OK != AmbaDCF_Destroy(DriveInfo[i].DriveID)) {
                            SvcLog_NG(SVC_STG_MONITOR, "AmbaDCF_Destroy failed!", 0U, 0U);
                        }
#endif

                        DriveInfo[i].IsExist = 0U;

                        if (NotifyCallBackFunc != NULL) {
                            NotifyCallBackFunc(DriveInfo[i].Drive, 0U);
                        }

                        /* Storage uninstall */
                        SvcScriptAutoRun(DriveInfo[i].Drive, 0U);
                    } else {
                        /* do nothing */
                    }
                }
#endif
#endif
            }
        }
    }

    SvcLog_NG(SVC_STG_MONITOR, "StgMonitor task terminate", 0U, 0U);
    return NULL;
}

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
static UINT32 DeletFirstFile(char Drive, UINT64 *pFileSize)
{
    UINT32                     Rval = SVC_OK;
    SVC_STG_MONI_DRIVE_INFO_s  DriveInfos;
#ifdef CONFIG_BUILD_COMMON_SERVICE_DCF
    AMBA_FS_FILE_INFO_s        Info;
    char                       FileName[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    AMBA_DCF_FILE_TYPE_e       Type = AMBA_DCF_FILE_TYPE_VIDEO;
#endif

    if (SVC_OK != SvcStgMonitor_GetDriveInfo(Drive, &DriveInfos)) {
        SvcLog_NG(SVC_STG_MONITOR, "SvcStgMonitor_GetDriveInfo failed!", 0U, 0U);
        Rval = SVC_NG;
    } else {
#ifdef CONFIG_BUILD_COMMON_SERVICE_DCF
        UINT32 Index = 1U;
        while (1) {
            if (SVC_OK != AmbaDCF_GetFileName(DriveInfos.DriveID, 0U, Type, FileName, Index++)) {
                SvcLog_NG(SVC_STG_MONITOR, "AmbaDCF_GetFileName Index[%d]failed!", Index, 0U);
                Rval = SVC_NG;
                break;
            } else {
                if (AMBA_FS_ERR_NONE != AmbaFS_GetFileInfo(FileName, &Info)) {
                    AmbaPrint_PrintStr5("["SVC_STG_MONITOR"]Get file info failed: %s", FileName, NULL, NULL, NULL, NULL);
                    continue;
                } else {
                    if (Info.Attr & AMBA_FS_ATTR_RDONLY) {
                        AmbaPrint_PrintStr5("["SVC_STG_MONITOR"]File locked: %s", FileName, NULL, NULL, NULL, NULL);
                        continue;
                    }
                    AmbaPrint_PrintStr5("["SVC_STG_MONITOR"]Delete file: %s", FileName, NULL, NULL, NULL, NULL);
                    if (SVC_OK != AmbaDCF_DeleteFile(FileName)) {
                        SvcLog_NG(SVC_STG_MONITOR, "AmbaDCF_DeleteFile failed!", 0U, 0U);
                        continue;
                    }
                    *pFileSize = Info.Size;
                    break;
                }
            }
        }
        if (KAL_ERR_NONE != AmbaKAL_TaskSleep(100)) {
            SvcLog_NG(SVC_STG_MONITOR, "AmbaKAL_TaskSleep failed!", 0U, 0U);
        }
#else
        AmbaMisra_TouchUnused(pFileSize);
#endif
    }
    return Rval;
}
#else
static UINT32 DeletFirstFile(char Drive, UINT64 *pFileSize)
{
    UINT32                     Rval = SVC_OK;
    SVC_STG_MONI_DRIVE_INFO_s  DriveInfos;
#ifdef CONFIG_BUILD_COMMON_SERVICE_DCF
    AMBA_FS_FILE_INFO_s        Info;
    char                       FileName[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    AMBA_DCF_FILE_TYPE_e       Type = AMBA_DCF_FILE_TYPE_VIDEO;
#endif

    if (SVC_OK != SvcStgMonitor_GetDriveInfo(Drive, &DriveInfos)) {
        SvcLog_NG(SVC_STG_MONITOR, "SvcStgMonitor_GetDriveInfo failed!", 0U, 0U);
        Rval = SVC_NG;
    } else {
#ifdef CONFIG_BUILD_COMMON_SERVICE_DCF
        if (SVC_OK != AmbaDCF_GetFileName(DriveInfos.DriveID, 0U, Type, FileName, 1U)) {
            //SvcLog_NG(SVC_STG_MONITOR, "AmbaDCF_GetFileName failed!", 0U, 0U);
            Rval = SVC_NG;
        } else {
            if (AMBA_FS_ERR_NONE != AmbaFS_GetFileInfo(FileName, &Info)) {
                AmbaPrint_PrintStr5("Get file info failed: %s", FileName, NULL, NULL, NULL, NULL);
                *pFileSize = 0ULL;
            } else {
                *pFileSize = Info.Size;
                AmbaPrint_PrintStr5("Delete file: %s", FileName, NULL, NULL, NULL, NULL);
                if (SVC_OK != AmbaDCF_DeleteFile(FileName)) {
                    SvcLog_NG(SVC_STG_MONITOR, "AmbaDCF_DeleteFile failed!", 0U, 0U);
                    *pFileSize = 0ULL;
                    Rval = SVC_NG;
                }
            }
        }
        if (KAL_ERR_NONE != AmbaKAL_TaskSleep(100)) {
            SvcLog_NG(SVC_STG_MONITOR, "AmbaKAL_TaskSleep failed!", 0U, 0U);
        }
#else
        AmbaMisra_TouchUnused(pFileSize);
#endif
    }
    return Rval;
}
#endif


/**
* initialization of storage monitor module
* @param [in] pInitInfo info block of storage monitor module
* @return 0-OK, 1-NG
*/
UINT32 SvcStgMonitor_Init(const SVC_STG_MONI_INIT_INFO_s *pInitInfo)
{
    UINT32                  Rval;
    static UINT8            StgMonStack[SVC_STG_MONITOR_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static SVC_TASK_CTRL_s  TaskCtrl GNU_SECTION_NOZEROINIT;
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
#ifdef CONFIG_ICAM_USB_USED
    static AMBA_SVC_USBH_s  HostMSC GNU_SECTION_NOZEROINIT;
    UINT32                  Err;

    /* Initialize */
    USBHostCheck = 0U;
#endif
#endif
    /* Not init yet */
    StgDeviceInitStatus = 0U;

    /* Create storage manager task */
    if (pInitInfo->EnableStgMgr != 0U) {
        if (SVC_OK != SvcStgMgr_Init(pInitInfo->StgMgrPriority, pInitInfo->StgMgrCpuBits, DeletFirstFile)) {
            SvcLog_NG(SVC_STG_MONITOR, "SvcStgMgr_Init failed!", 0U, 0U);
        }
    }

    TaskCtrl.Priority    = pInitInfo->StgMonitPriority;
    TaskCtrl.EntryFunc   = StgMonitor_TaskEntry;
    TaskCtrl.EntryArg    = 0U;
    TaskCtrl.pStackBase  = StgMonStack;
    TaskCtrl.StackSize   = SVC_STG_MONITOR_STACK_SIZE;
    TaskCtrl.CpuBits     = pInitInfo->StgMonitCpuBits;

    if (pInitInfo->EnableStgMonitCallBack != 0U) {
        if (pInitInfo->StgMonitCallBack != NULL) {
            NotifyCallBackFunc = pInitInfo->StgMonitCallBack;
        } else {
            SvcLog_NG(SVC_STG_MONITOR, "Set StgMonitCallBack failed!", 0U, 0U);
        }
    }

    Rval =  SvcTask_Create("StgMonitorTask", &TaskCtrl);

    if (Rval == SVC_OK) {
        SvcLog_OK(SVC_STG_MONITOR, "StgMonitorTask created!", 0U, 0U);
    } else {
        SvcLog_NG(SVC_STG_MONITOR, "fail to create StgMonitorTask!", 0U, 0U);
    }
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
#ifdef CONFIG_ICAM_USB_USED
    if (pInitInfo->UsbClass == SVC_STG_USBD_CLASS_MSC) {
        Err = AmbaSvcUsb_DeviceSysInit();
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_STG_MONITOR, "AmbaSvcUsb_DeviceSysInit failed %u", Err, 0U);
        }
        AmbaSvcUsb_DeviceClassSet(AMBA_USBD_CLASS_MSC);
        AmbaSvcUsb_DeviceClassMscLoad(pInitInfo->MainStgDrive);
        AmbaSvcUsb_DeviceClassStart();
    } else if (pInitInfo->UsbClass == SVC_STG_USBD_CLASS_MTP) {
        Err = AmbaSvcUsb_DeviceSysInit();
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_STG_MONITOR, "AmbaSvcUsb_DeviceSysInit failed %u", Err, 0U);
        }
        AmbaSvcUsb_DeviceClassSet(AMBA_USBD_CLASS_MTP);
        Err = AmbaSvcUsb_DeviceClassMtpInit(pInitInfo->MainStgDrive);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_STG_MONITOR, "AmbaSvcUsb_DeviceClassMtpInit failed %u", Err, 0U);
        }
        AmbaSvcUsb_DeviceClassStart();
    } else if (pInitInfo->UsbClass == SVC_STG_USBH_CLASS_MSC) {
        /* Hookup USB Host */
        HostMSC.SelectHcd = AMBA_USB_HOST_HCD;
        HostMSC.OCPolarity = 0;
        HostMSC.Phy0Owner  = 1;
        HostMSC.pfnCallback = SvcHostEventCallback;
        AmbaSvcUsb_HostClassStart(&HostMSC);
    } else {
        /* do nothong */
    }
    SvcLog_OK(SVC_STG_MONITOR, "hook usb class %d", pInitInfo->UsbClass, 0U);
#endif
#endif
    return Rval;
}

/**
* drive info get of storage
* @param [in] Drive drive of storage
* @param [out] pDriveInfo info block of drive
* @return 0-OK, 1-NG
*/
UINT32 SvcStgMonitor_GetDriveInfo(char Drive, SVC_STG_MONI_DRIVE_INFO_s *pDriveInfo)
{
    UINT8 i, DriveLCase, Rval = SVC_NG;
    for (i = 0U; i < SVC_STG_DRIVE_NUM; i++) {
        DriveLCase = (UINT8)DriveInfo[i].Drive + (UINT8)32U;
        if ((DriveInfo[i].Drive == Drive) || (DriveLCase == (UINT8)Drive)) {
            *pDriveInfo = DriveInfo[i];
            Rval = SVC_OK;
            break;
        }
    }

    return Rval;
}

/**
* Get Storage Init Status
* @return Init done(1U) / Not Init done(0U)
*/
UINT32 SvcStgMonitor_StgInitStatus(void)
{
    return StgDeviceInitStatus;
}
