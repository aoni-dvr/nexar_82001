/**
 *  @file SvcStgMonitorTask.c
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
 *  @details Svc Storage Monitor Control Task
 *
 */

#include "AmbaTypes.h"
#include "AmbaDSP_Capability.h"
#include "AmbaWrap.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"

/* app-shared */
#include "SvcSysStat.h"
#include "SvcStgMonitor.h"

/* app */
#include "SvcAppStat.h"
#include "SvcTaskList.h"
#include "SvcUserPref.h"

#include "SvcStgMonitorTask.h"
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#include "app_helper.h"
#endif

#define SVC_LOG_STG_MONITOR_TASK    "STG_MONITOR_TASK"

static void StgMonitorCB(char Drive, UINT32 IsInsert);

/**
* Initialize the Storage Monitor Task
* @return ErrorCode
*/
UINT32 SvcStgMonitorTask_Init(void)
{
    UINT32 RetVal = SVC_OK;
    SVC_STG_MONI_INIT_INFO_s InitInfo;
    SVC_USER_PREF_s          *pSvcUserPref;

    AmbaSvcWrap_MisraMemset(&InitInfo, 0, sizeof(SVC_STG_MONI_INIT_INFO_s));
    InitInfo.StgMonitPriority       = SVC_SD_MONI_TASK_PRI;
    InitInfo.StgMonitCpuBits        = SVC_SD_MONI_TASK_CPU_BITS;
    InitInfo.EnableStgMonitCallBack = 1U;
    InitInfo.StgMonitCallBack       = StgMonitorCB;
    InitInfo.EnableStgMgr           = 1U;
    InitInfo.StgMgrPriority         = SVC_STOR_MGR_TASK_PRI;
    InitInfo.StgMgrCpuBits          = SVC_STOR_MGR_TASK_CPU_BITS;
    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
        InitInfo.UsbClass = pSvcUserPref->UsbClass;
        InitInfo.MainStgDrive = pSvcUserPref->MainStgDrive[0];
    } else {
        SvcLog_NG(SVC_LOG_STG_MONITOR_TASK, "SvcUserPref_Get() error", 0U, 0U);
    }

    RetVal = SvcStgMonitor_Init(&InitInfo);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_STG_MONITOR_TASK, "## fail to init svc storage monitor", 0U, 0U);
    }

    return RetVal;
}

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#define tolower(c)          ((c) + 0x20 * (((c) >= 'A') && ((c) <= 'Z')))
static void StgMonitorCB(char Drive, UINT32 IsInsert)
{
    SVC_APP_STAT_STG_s SdStatus = {0};
    UINT32 Rval = SVC_OK;

    Rval = SvcSysStat_Get(SVC_APP_STAT_STG, &SdStatus);
    if (tolower(Drive) == tolower(SD_SLOT[0])) {
        if (IsInsert == 2U) {
            SdStatus.Status = SVC_APP_STAT_SD_IDLE;
            SvcLog_OK(SVC_LOG_STG_MONITOR_TASK, "The drive %c idle!", Drive, 0U);
        } else if (IsInsert == 1U) {
            SdStatus.Status = SVC_APP_STAT_SD_READY;
            SvcLog_OK(SVC_LOG_STG_MONITOR_TASK, "The drive %c ready!", Drive, 0U);
        } else {
            SdStatus.Status = SVC_APP_STAT_SD_NONE;
            SvcLog_OK(SVC_LOG_STG_MONITOR_TASK, "The drive %c removed!", Drive, 0U);
        }
        Rval = SvcSysStat_Issue(SVC_APP_STAT_STG, &SdStatus);
    } else {
        SvcLog_NG(SVC_LOG_STG_MONITOR_TASK, "The drive %c is not supported!", Drive, 0U);
    }

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STG_MONITOR_TASK, "## fail to set flag", 0U, 0U);
    }
}
#else
static void StgMonitorCB(char Drive, UINT32 IsInsert)
{
    SVC_APP_STAT_STG_s SdStatus = {0};
    UINT32             Rval = SVC_OK;

    Rval = SvcSysStat_Get(SVC_APP_STAT_STG, &SdStatus);

    if ((Drive == 'c') || (Drive == 'C')) {
        if (IsInsert == 0U) {
            SdStatus.Status &= (UINT8)(~SVC_APP_STAT_STG_C_READY);
        } else {
            SdStatus.Status |= SVC_APP_STAT_STG_C_READY;
        }
        Rval = SvcSysStat_Issue(SVC_APP_STAT_STG, &SdStatus);
    } else if ((Drive == 'd') || (Drive == 'D')) {
        if (IsInsert == 0U) {
            SdStatus.Status &= (UINT8)(~SVC_APP_STAT_STG_D_READY);
        } else {
            SdStatus.Status |= SVC_APP_STAT_STG_D_READY;
        }
        Rval = SvcSysStat_Issue(SVC_APP_STAT_STG, &SdStatus);
    } else if ((Drive == 'i') || (Drive == 'I')) {
        if (IsInsert == 0U) {
            SdStatus.Status &= (UINT8)(~SVC_APP_STAT_STG_I_READY);
        } else {
            SdStatus.Status |= SVC_APP_STAT_STG_I_READY;
        }
        Rval = SvcSysStat_Issue(SVC_APP_STAT_STG, &SdStatus);
    } else {
        SvcLog_NG(SVC_LOG_STG_MONITOR_TASK, "The drive is not supported!", 0U, 0U);
    }

    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_STG_MONITOR_TASK, "## fail to set flag", 0U, 0U);
    }
}
#endif

