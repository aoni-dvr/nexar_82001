/**
 *  @file SvcVinCtrlTask.c
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
 *  @details svc vin config task
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaShell.h"
#include "AmbaKAL.h"

#include "AmbaDSP.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_Capability.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_Event.h"
#include "AmbaHDMI_Def.h"

#include "AmbaSensor.h"
#include "AmbaFPD.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcIK.h"
#include "SvcLiveview.h"
#include "SvcDisplay.h"
#include "SvcEnc.h"
#include "SvcVinSrc.h"
#include "SvcVoutSrc.h"
#include "SvcInfoPack.h"
#include "SvcTaskList.h"
#include "SvcSysStat.h"
#include "SvcAppStat.h"
#include "SvcResCfg.h"

#include "SvcVinCtrlTask.h"

static void VinCtrlTask_SrcReady(void);

#define SVC_LOG_VIN_CTRL_TASK   "VIN_CTRL_TASK"

static AMBA_KAL_EVENT_FLAG_t SvcVinCtrlTaskEventFlag;
static SVC_VIN_SRC_INIT_s SrcInit = {0};

/**
 * Init all the vin configure resource
 * return 0-OK, 1-NG
 */
UINT32 SvcVinCtrlTask_Init(void)
{
    UINT32 RetVal;
    SVC_APP_STAT_VIN_s VinStatus = { .Status = SVC_APP_STAT_VIN_CONFIGING };
    static char VinCtrlTaskEventFlagName[16] = "VinCtrlTaskFlag";

    SvcLog_OK(SVC_LOG_VIN_CTRL_TASK, "SvcVinCtrlTask_Init() Start", 0U, 0U);

    RetVal = AmbaKAL_EventFlagCreate(&SvcVinCtrlTaskEventFlag, VinCtrlTaskEventFlagName);
    /* Create event flag for vin status */
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_VIN_CTRL_TASK, "Event flag create failed with %d", RetVal, 0U);
    }

    /* Clear event flag */
    if (SVC_OK == RetVal) {
        RetVal = AmbaKAL_EventFlagClear(&SvcVinCtrlTaskEventFlag, 0xFFFFFFFFU);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VIN_CTRL_TASK, "Event flag clear failed with %d", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
#ifdef CONFIG_ICAM_PARALLEL_VIN_CONFIG
        UINT32 Idx;
        UINT32 SysCoreNum = 1U;

        AmbaMisra_TouchUnused(&SysCoreNum);
#ifdef CONFIG_KAL_THREADX_SMP_NUM_CORES
        if (SysCoreNum != (UINT32)CONFIG_KAL_THREADX_SMP_NUM_CORES) {
            SysCoreNum = (UINT32)CONFIG_KAL_THREADX_SMP_NUM_CORES;
        }
#endif
#endif

        /* vin source init (sensor or yuv) */
        SvcLog_OK(SVC_LOG_VIN_CTRL_TASK, "SvcVinCtrlTask_Init() init sensor", 0U, 0U);

        SvcInfoPack_VinSrcInit(&SrcInit);
        SrcInit.pInitDoneCB = VinCtrlTask_SrcReady;
#ifdef CONFIG_ICAM_PARALLEL_VIN_CONFIG
        for (Idx = 0U; Idx < SrcInit.CfgNum; Idx ++) {
            SrcInit.CfgTaskInfo[Idx].State = 0U;
            SrcInit.CfgTaskInfo[Idx].TaskCpuBits = (UINT32)1U << (Idx % SysCoreNum); // just an example.
            SrcInit.CfgTaskInfo[Idx].TaskPriority = SVC_VIN_SRC_TASK_PRI;
        }
#endif
        SvcVinSrc_Init(&SrcInit, SVC_VIN_SRC_TASK_PRI, SVC_VIN_SRC_TASK_CPU_BITS);

        RetVal = SvcSysStat_Issue(SVC_APP_STAT_VIN, &VinStatus);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VIN_CTRL_TASK, "SvcSysStat_Issue VinStatus = %d failed", VinStatus.Status, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_VIN_CTRL_TASK, "SvcVinCtrlTask_Init() Done", 0U, 0U);
    }

    return RetVal;
}

/**
 * Configure all the vin needed resource, including init, hook, enable
 * return 0-OK, 1-NG
 */
UINT32 SvcVinCtrlTask_Config(void)
{
    UINT32 RetVal = SVC_OK;
    SVC_APP_STAT_VIN_s VinStatus = {0};

    SvcLog_OK(SVC_LOG_VIN_CTRL_TASK, "SvcVinCtrlTask_Config() Start", 0U, 0U);

    RetVal = SvcSysStat_Get(SVC_APP_STAT_VIN, &VinStatus);
    if (SVC_OK == RetVal) {
        if (VinStatus.Status == SVC_APP_STAT_VIN_NOT_READY) {
            SvcLog_OK(SVC_LOG_VIN_CTRL_TASK, "SvcVinCtrlTask_Config() init sensor", 0U, 0U);
            /* vin source init (sensor or yuv) */
            SvcInfoPack_VinSrcInit(&SrcInit);
            SrcInit.pInitDoneCB = VinCtrlTask_SrcReady;
            SvcVinSrc_Init(&SrcInit, SVC_VIN_SRC_TASK_PRI, SVC_VIN_SRC_TASK_CPU_BITS);
        } else {
            SvcLog_OK(SVC_LOG_VIN_CTRL_TASK, "SvcVinCtrlTask_Config() skip", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_VIN_CTRL_TASK, "SvcVinCtrlTask_Config() get VinStatus err", 0U, 0U);
    }

    SvcLog_OK(SVC_LOG_VIN_CTRL_TASK, "SvcVinCtrlTask_Config() Done", 0U, 0U);

    return RetVal;
}

/**
 * Disable the vin/sensor resource configuration
 * return 0-OK, 1-NG
 */
UINT32 SvcVinCtrlTask_Destroy(void)
{
    UINT32 RetVal = SVC_OK;
    SVC_APP_STAT_VIN_s VinStatus = { .Status = SVC_APP_STAT_VIN_NOT_READY };

    SvcLog_OK(SVC_LOG_VIN_CTRL_TASK, "SvcVinCtrlTask_Destroy() Start", 0U, 0U);
    SvcVinSrc_DeInit(&SrcInit);

    RetVal = AmbaKAL_EventFlagClear(&SvcVinCtrlTaskEventFlag, 0xFFFFFFFFU);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_VIN_CTRL_TASK, "Event flag clear failed with %d", RetVal, 0U);
    }

    if (SVC_OK == RetVal) {
        RetVal = SvcSysStat_Issue(SVC_APP_STAT_VIN, &VinStatus);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VIN_CTRL_TASK, "Sys Status issue failed with %d", RetVal, 0U);
        }
    }

    SvcLog_OK(SVC_LOG_VIN_CTRL_TASK, "SvcVinCtrlTask_Destroy() Done", 0U, 0U);

    return RetVal;
}

/**
 * Wait vin src is ready or not
 * return 0-OK, 1-NG
 */
UINT32 SvcVinCtrlTask_WaitSrcReady(void)
{
    UINT32  RetVal, ActualFlags = {0}, WaitTimeOut;
    UINT32  i, VinNum, VinSrc = 0U, VinIDs[AMBA_DSP_MAX_VIN_NUM] = {0};

    WaitTimeOut = 10000U;
    if (SvcResCfg_GetVinIDs(VinIDs, &VinNum) == SVC_OK) {
        for (i = 0U; i < VinNum; i++) {
            if (SvcResCfg_GetVinSrc(VinIDs[i], &VinSrc) == SVC_OK) {
                if (VinSrc == SVC_VIN_SRC_YUV) {
                    WaitTimeOut = AMBA_KAL_WAIT_FOREVER;
                    break;
                }
            }
        }
    }

    /* Get event flag, maybe timeout */
    RetVal = AmbaKAL_EventFlagGet(&SvcVinCtrlTaskEventFlag,
                                  SVC_VIN_CTRL_SRC_INIT_DONE,
                                  AMBA_KAL_FLAGS_ALL,
                                  AMBA_KAL_FLAGS_CLEAR_NONE,
                                  &ActualFlags,
                                  WaitTimeOut);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_VIN_CTRL_TASK, "Event flag get failed with %d", RetVal, 0U);
    }

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_VIN_CTRL_TASK, "Vin src is ready", 0U, 0U);
    }

    return RetVal;
}

static void VinCtrlTask_SrcReady(void)
{
    SVC_APP_STAT_VIN_s VinStatus = { .Status = SVC_APP_STAT_VIN_READY };
    UINT32 RetVal = SVC_OK;

    RetVal = AmbaKAL_EventFlagSet(&SvcVinCtrlTaskEventFlag, SVC_VIN_CTRL_SRC_INIT_DONE);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_VIN_CTRL_TASK, "Event flag set failed with %d", RetVal, 0U);
    }

    RetVal = SvcSysStat_Issue(SVC_APP_STAT_VIN, &VinStatus);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_VIN_CTRL_TASK, "Sys Status issue failed with %d", RetVal, 0U);
    }
}
