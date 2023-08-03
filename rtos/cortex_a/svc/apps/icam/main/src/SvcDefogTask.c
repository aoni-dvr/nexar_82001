/**
 *  @file SvcDefogTask.c
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
 *  @details svc display task
 *
 */

#include "AmbaTypes.h"
#include "AmbaVIN_Def.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaShell.h"
#include "AmbaSvcWrap.h"

#include "AmbaDSP_Capability.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_Liveview.h"

/* svc-framework */
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcCmd.h"

/* apps-shared */
#include "SvcImg.h"
#include "SvcSysStat.h"
#include "SvcResCfg.h"

/* apps-icam */
#include "SvcAppStat.h"
#include "SvcDefogTask.h"

#define SVC_LOG_DEFOG_TASK "DEFOG_TASK"

static UINT32 SvcDefogEnable = 0U;

static void DefogTask_MenuStatusCallback(UINT32 StatIdx, void *pInfo);
static void DefogTask_CmdInstall(void);
static void DefogTask_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void DefogTask_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc);

/**
 * Init Defog ctrl module
 * return 0-OK, 1-NG
 */
UINT32 SvcDefogTask_Init(void)
{
    UINT32 Rval = SVC_OK;
    UINT32 RetVal = SVC_OK;

    SVC_SYS_STAT_ID StatusID;

    Rval = SvcSysStat_Register(SVC_APP_STAT_MENU, DefogTask_MenuStatusCallback, &StatusID);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_DEFOG_TASK, "Sys Stat Register failed", 0U, 0U);
        RetVal = SVC_NG;
    }

    DefogTask_CmdInstall();

    return RetVal;
}

/**
 * Start Defog
 * return 0-OK, 1-NG
 */
UINT32 SvcDefogTask_Start(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 VinIDs[AMBA_DSP_MAX_VIN_NUM], VinID, VinNum;
    UINT32 SensorIdxs[16U], SensorIdx, SensorNum;

    UINT32 i, j;

    SvcLog_DBG(SVC_LOG_DEFOG_TASK, "Defog start", 0U, 0U);

    AmbaSvcWrap_MisraMemset(VinIDs, 0, sizeof(VinIDs));
    AmbaSvcWrap_MisraMemset(SensorIdxs, 0, sizeof(SensorIdxs));

    RetVal = SvcResCfg_GetVinIDs(VinIDs, &VinNum);
    if (SVC_OK == RetVal) {
        for (i = 0U; i < VinNum; i++) {
            VinID = VinIDs[i];
            RetVal = SvcResCfg_GetSensorIdxsInVinID(VinID, SensorIdxs, &SensorNum);
            for (j = 0U; j < SensorNum; j++) {
                SensorIdx = SensorIdxs[j];
                SvcImg_AlgoDefog(VinID, SensorIdx, 1U);
            }
        }
    }

    if (SVC_OK == RetVal) {
        SVC_APP_STAT_DEFOG_s DefogStatus = { .Status = SVC_APP_STAT_DEFOG_ENABLE };
        RetVal = SvcSysStat_Issue(SVC_APP_STAT_DEFOG, &DefogStatus);
    }

    SvcDefogEnable = 1U;

    SvcLog_DBG(SVC_LOG_DEFOG_TASK, "Defog start done", 0U, 0U);

    return RetVal;
}

/**
 * Stop Defog
 * return 0-OK, 1-NG
 */
UINT32 SvcDefogTask_Stop(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 VinIDs[AMBA_DSP_MAX_VIN_NUM], VinID, VinNum;
    UINT32 SensorIdxs[16U], SensorIdx, SensorNum;

    UINT32 i, j;

    SvcLog_DBG(SVC_LOG_DEFOG_TASK, "Defog stop", 0U, 0U);

    AmbaSvcWrap_MisraMemset(VinIDs, 0, sizeof(VinIDs));
    AmbaSvcWrap_MisraMemset(SensorIdxs, 0, sizeof(SensorIdxs));

    RetVal = SvcResCfg_GetVinIDs(VinIDs, &VinNum);
    if (SVC_OK == RetVal) {
        for (i = 0U; i < VinNum; i++) {
            VinID = VinIDs[i];
            RetVal = SvcResCfg_GetSensorIdxsInVinID(VinID, SensorIdxs, &SensorNum);
            for (j = 0U; j < SensorNum; j++) {
                SensorIdx = SensorIdxs[j];
                SvcImg_AlgoDefog(VinID, SensorIdx, 0U);
            }
        }
    }

    if (SVC_OK == RetVal) {
        SVC_APP_STAT_DEFOG_s DefogStatus = { .Status = SVC_APP_STAT_DEFOG_DISABLE };
        RetVal = SvcSysStat_Issue(SVC_APP_STAT_DEFOG, &DefogStatus);
    }


    SvcDefogEnable = 0U;

    SvcLog_DBG(SVC_LOG_DEFOG_TASK, "Defog stop done", 0U, 0U);

    return RetVal;
}

static void DefogTask_MenuStatusCallback(UINT32 StatIdx, void *pInfo)
{
    UINT32 RetVal;

    SVC_APP_STAT_MENU_s *pStatus = NULL;

    AmbaMisra_TouchUnused(&StatIdx);    /* Misra-c fixed */
    AmbaMisra_TouchUnused(pInfo);       /* Misra-c fixed */
    AmbaMisra_TouchUnused(pStatus);     /* Misra-c fixed */

    AmbaMisra_TypeCast(&pStatus, &pInfo);

    if (pStatus->Type == SVC_APP_STAT_MENU_TYPE_EMR) {
        if (pStatus->FuncIdx == SVC_APP_STAT_MENU_EMR_DEFOG) {
            if (pStatus->Operation == SVC_APP_STAT_MENU_OP_ENTER) {
                /* Check status and start/stop */
                if (SvcDefogEnable == 0U) {
                    (void) SvcDefogTask_Start();
                } else {
                    (void) SvcDefogTask_Stop();
                }
            }
        }
    } else if (pStatus->Type == SVC_APP_STAT_MENU_TYPE_ADAS_DVR) {
        if (pStatus->FuncIdx == SVC_APP_STAT_MENU_ADAS_DEFOG) {
            if (pStatus->Operation == SVC_APP_STAT_MENU_OP_ENTER) {
                /* Check status and start/stop */
                if (SvcDefogEnable == 0U) {
                    (void) SvcDefogTask_Start();
                } else {
                    (void) SvcDefogTask_Stop();
                }
            }
        }
    } else {
        /* Do nothing */
    }

    AmbaMisra_TouchUnused(&RetVal);
}

static void DefogTask_CmdInstall(void)
{
    AMBA_SHELL_COMMAND_s  SvcDefogCmd;

    UINT32  RetVal;

    SvcDefogCmd.pName    = "svc_defog_task";
    SvcDefogCmd.MainFunc = DefogTask_CmdEntry;
    SvcDefogCmd.pNext    = NULL;

    RetVal = SvcCmd_CommandRegister(&SvcDefogCmd);
    if (SHELL_ERR_SUCCESS != RetVal) {
        SvcLog_NG(SVC_LOG_DEFOG_TASK, "## fail to install svc defog command", 0U, 0U);
    }
}

static void DefogTask_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = SVC_OK;

    if (1U < ArgCount) {
        if (0 == SvcWrap_strcmp("start", pArgVector[1U])) {
            RetVal = SvcDefogTask_Start();
        } else if (0 == SvcWrap_strcmp("stop", pArgVector[1U])) {
            RetVal = SvcDefogTask_Stop();
        } else {
            RetVal = SVC_NG;
        }
    } else {
        RetVal = SVC_NG;
    }

    if (SVC_OK != RetVal) {
        DefogTask_CmdUsage(PrintFunc);
    }
}

static void DefogTask_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("svc_defog_task commands:\n");
    PrintFunc(" =======================\n ");
    PrintFunc("             start    : Start defog\n");
    PrintFunc("             stop     : stop defog\n");
}
