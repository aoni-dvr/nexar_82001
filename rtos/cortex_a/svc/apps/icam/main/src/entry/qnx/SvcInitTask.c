/**
 *  @file SvcInitTask.c
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
 *  @details svc init task
 *
 */

#include ".svc_autogen"

#include "AmbaMisraFix.h"
#include "AmbaFS.h"
#include "AmbaVIN.h"
#include "AmbaSensor.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaUtility.h"
#include "AmbaSD.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"
#include "AmbaUtc.h"
#include "AmbaDCF.h"
#include "AmbaCache.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcTask.h"
#include "SvcTaskList.h"
#include "SvcBootDev.h"
#include "SvcSysStat.h"
#include "SvcClock.h"
#include "SvcSysEvent.h"
#include "SvcResCfg.h"
#include "SvcBuild.h"
#include "SvcFwUpdate.h"
#include "SvcUserPref.h"
#include "SvcControlTask.h"
#include "SvcFlowControl.h"
#include "SvcAppStat.h"
#include "SvcPlat.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "iCamBufPool.h"
#include "SvcInitTask.h"
#include "SvcSafeStateMgr.h"
#if defined(CONFIG_ENABLE_AMBALINK)
#include "AmbaIPC.h"
#endif
#if defined(CONFIG_BUILD_CV)    /* CV related function */
#include "SvcCvMainTask.h"
#endif
#define SVC_LOG_INIT                "INIT"

static void PrintModule_Enable(void)
{
    SVC_USER_PREF_s *pSvcUserPref;

    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
        UINT16 Module = (UINT16)(SSP_ERR_BASE >> 16U);
        UINT32 Rval;
        extern UINT32 AmbaDSP_MainApiDumpInit(UINT8 Enable, UINT32 Mask);

        SvcLog_OK(SVC_LOG_INIT, "ssp_log %u, dsp_log %u", pSvcUserPref->ShowSspLog, pSvcUserPref->ShowDspLog);
        SvcLog_OK(SVC_LOG_INIT, "sensor_log %u, bridge_log %u", pSvcUserPref->ShowSensorLog, pSvcUserPref->ShowBridgeLog);
        SvcLog_OK(SVC_LOG_INIT, "swpll_log %u, ik_log %u", pSvcUserPref->ShowSwpllLog, pSvcUserPref->ShowIKLog);
        SvcLog_OK(SVC_LOG_INIT, "ssp_api_log %u, yuv_log %u", pSvcUserPref->ShowSspApiLog, pSvcUserPref->ShowYuvLog);
        Rval = AmbaPrint_ModuleSetAllowList(Module, (UINT8)pSvcUserPref->ShowSspLog);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_INIT, "AmbaPrint_ModuleSetAllowList %u return %u", Module, Rval);
        }
        Rval = AmbaDSP_MainApiDumpInit((UINT8)pSvcUserPref->ShowSspApiLog, 0U);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_INIT, "AmbaDSP_MainApiDumpInit %u return %u", Module, Rval);
        }

        Module = (UINT16)(SENSOR_ERR_BASE >> 16U);
        Rval = AmbaPrint_ModuleSetAllowList(Module, (UINT8)pSvcUserPref->ShowSensorLog);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_INIT, "AmbaPrint_ModuleSetAllowList %u return %u", Module, Rval);
        }

        Module = (UINT16)(BRIDGE_ERR_BASE >> 16U);
        Rval = AmbaPrint_ModuleSetAllowList(Module, (UINT8)pSvcUserPref->ShowBridgeLog);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_INIT, "AmbaPrint_ModuleSetAllowList %u return %u", Module, Rval);
        }

        Module = (UINT16)(YUV_ERR_BASE >> 16U);
        Rval = AmbaPrint_ModuleSetAllowList(Module, (UINT8)pSvcUserPref->ShowYuvLog);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_INIT, "AmbaPrint_ModuleSetAllowList %u return %u", Module, Rval);
        }

#if defined(IK_ERR_BASE)
        Module = (UINT16)(IK_ERR_BASE >> 16U);
        Rval = AmbaPrint_ModuleSetAllowList(Module, (UINT8)pSvcUserPref->ShowIKLog);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_INIT, "AmbaPrint_ModuleSetAllowList %u return %u", Module, Rval);
        }
#endif

#if defined(CONFIG_ICAM_SWPLL_CONTROL)
        Module = (UINT16)(FTCM_ERR_BASE >> 16U);
        SvcSwPll_Debug((UINT8)pSvcUserPref->ShowSwpllLog);
        Rval = AmbaPrint_ModuleSetAllowList(Module, (UINT8)pSvcUserPref->ShowSwpllLog);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_INIT, "AmbaPrint_ModuleSetAllowList %u return %u", Module, Rval);
        }
#endif

        Module = (UINT16)(IMGPROC_ERR_BASE >> 16U);
        Rval = AmbaPrint_ModuleSetAllowList(Module, (UINT8)1);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_INIT, "AmbaPrint_ModuleSetAllowList %u return %u", Module, Rval);
        }

        Module = (UINT16)(CODEC_ERR_BASE >> 16U);
        Rval = AmbaPrint_ModuleSetAllowList(Module, (UINT8)1);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_INIT, "AmbaPrint_ModuleSetAllowList %u return %u", Module, Rval);
        }

        Module = (UINT16)(PLAYER_ERR_BASE >> 16U);
        Rval = AmbaPrint_ModuleSetAllowList(Module, (UINT8)1);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_INIT, "AmbaPrint_ModuleSetAllowList %u return %u", Module, Rval);
        }

        Module = (UINT16)(RECODER_ERR_BASE >> 16U);
        Rval = AmbaPrint_ModuleSetAllowList(Module, (UINT8)1);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_INIT, "AmbaPrint_ModuleSetAllowList %u return %u", Module, Rval);
        }

        Module = (UINT16)(AUDIO_ERR_BASE >> 16U);
        Rval = AmbaPrint_ModuleSetAllowList(Module, (UINT8)1);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_INIT, "AmbaPrint_ModuleSetAllowList %u return %u", Module, Rval);
        }

        Module = (UINT16)(FIFO_ERR_BASE >> 16U);
        Rval = AmbaPrint_ModuleSetAllowList(Module, (UINT8)1);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_INIT, "AmbaPrint_ModuleSetAllowList %u return %u", Module, Rval);
        }

        Module = (UINT16)(MUXER_ERR_BASE >> 16U);
        Rval = AmbaPrint_ModuleSetAllowList(Module, (UINT8)1);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_INIT, "AmbaPrint_ModuleSetAllowList %u return %u", Module, Rval);
        }

        Module = (UINT16)(CFS_ERR_BASE >> 16U);
        Rval = AmbaPrint_ModuleSetAllowList(Module, (UINT8)1);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_INIT, "AmbaPrint_ModuleSetAllowList %u return %u", Module, Rval);
        }

        Module = (UINT16)(DEMUXER_ERR_BASE >> 16U);
        Rval = AmbaPrint_ModuleSetAllowList(Module, (UINT8)1);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_INIT, "AmbaPrint_ModuleSetAllowList %u return %u", Module, Rval);
        }

        Module = (UINT16)(STREAM_ERR_BASE >> 16U);
        Rval = AmbaPrint_ModuleSetAllowList(Module, (UINT8)1);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_INIT, "AmbaPrint_ModuleSetAllowList %u return %u", Module, Rval);
        }
    } else {
        SvcLog_NG(SVC_LOG_INIT, "## OpMode_Enter() failed with get preference error", 0U, 0U);
    }
}

static void StgMountOp(char Drive, UINT32 MountOp)
{
    UINT32  i, Rval, Retry = 5U;

    if (0U < MountOp) {
        for (i = 0U; i < Retry; i++) {
            Rval = AmbaFS_Mount(Drive);
            if (Rval == OK) {
                break;
            }
            if (0U != AmbaKAL_TaskSleep(1000U)) {
                // misra-c
            }
            SvcLog_DBG(SVC_LOG_INIT, "## mount retry ...", 0U, 0U);
        }

        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_INIT, "## fail to mount storage, Rval(0x%X)", Rval, 0U);
        }
    } else {
        Rval = AmbaFS_UnMount(Drive);
        AmbaMisra_TouchUnused(&Rval);
    }
}

static UINT32 CacheMemCheck(void *pStartAddr, UINT32 Size)
{
    ULONG  StartAddr;

    AmbaMisra_TouchUnused(pStartAddr);

    AmbaMisra_TypeCast(&StartAddr, &pStartAddr);
    return SvcBuffer_CheckCached(StartAddr, Size);
}

/**
* create of init task
* @return none
*/
void SvcInitTask_Create(void)
{
    static SVC_PLAT_CB_s  PlatEntry = {
        /* sd card mount/umount */
        .pCbMountOp = StgMountOp,
        /* cache memory check */
        .pCbCacheChk = CacheMemCheck,
        /* buffer pool init */
        .pCbBufPoolInit = iCamBufPool_Init,
        /* cache oprtation */
        .pAmbaCacheClean      = AmbaCache_DataClean,
        .pAmbaCacheInvalidate = AmbaCache_DataInvalidate,
        .pAmbaCacheFlush      = AmbaCache_DataFlush,
    };

    static SVC_FLOW_CONTROL_CMD_FUNC ControlTask_ProjectEntry = NULL;

    UINT32  Rval;

    /* Show build information */
    SvcBuild_ShowInfo();

    /* init platform dependent cb */
    SvcPlat_InitCB(&PlatEntry);

#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
#if defined(CONFIG_BUILD_CV)
        {
            SVC_SAFE_STATE_CALLBACK_s Callback;

            Callback.pEnableVpuBist  = SvcCvMainTask_EnableVpuBist;
            SvcSafeStateMgr_RegisterCallback(&Callback);
        }
#endif
        Rval = AmbaPrint_ModuleSetAllowList(AMBALINK_MODULE_ID, 1U);
        if (SVC_OK == Rval) {
            SVC_SAFE_STATE_MGR_TSK_CFG_s Cfg;
            Cfg.Priority = SVC_SAFE_STATE_MGR_TASK_PRI;
            Cfg.CpuBits = SVC_SAFE_STATE_MGR_TASK_CPU_BITS;
            Rval = SvcSafeStateMgr_Init(&Cfg);
            if (SVC_OK == Rval) {
                Rval = SvcSafeStateMgr_WaitSysReady();
                AmbaMisra_TouchUnused(&Rval);
            }
        }
#endif
    /* RTC time checking */
    {
        AMBA_RTC_DATE_TIME_s  Calendar;

        Rval = AmbaRTC_GetSysTime(&Calendar);
        if (Rval == OK) {
            if (Calendar.Year < 2000U) {
                Calendar.Year = 2018U;
                Calendar.Month = 10U;
                Calendar.Day = 2U;
                Rval = AmbaRTC_SetSysTime(&Calendar);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_INIT, "## fail to AmbaRTC_SetSysTime, Rval = 0x%X", Rval, 0U);
                }
            }
        } else {
            SvcLog_NG(SVC_LOG_INIT, "## fail to AmbaRTC_GetSysTime, Rval = 0x%X", Rval, 0U);
        }
    }

    SvcClock_Init(CONFIG_ICAM_CLK_LINE);

#ifdef CONFIG_BUILD_COMAMBA_CODEC_COMMON
    AmbaUtcInit();
#endif

    /* init file system */
    Rval = AmbaFS_Init();
    if (AMBA_FS_ERR_NONE != Rval) {
        SvcLog_NG(SVC_LOG_INIT, "## fs isn't initialized", 0U, 0U);
    }

    /* init boot device */
    Rval = SvcBootDev_Init(NULL);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_INIT, "## boot_device isn't initialized", 0U, 0U);
    }

    /* Get svc user preference */
    Rval = SvcUserPref_Init();
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_INIT, "## fail to get svc preference", 0U, 0U);
    }

    /* init svc system status */
    Rval = SvcSysStat_Init(SVC_APP_STAT_NUM);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_INIT, "## fail to init svc status", 0U, 0U);
    }

    /* System event init */
    SvcSysEvent_Init();

    /* Enable differnt AmbaPrint Module for debug if necessary */
    PrintModule_Enable();

    /* dump clocks */
    SvcClock_Dump();

    /* init DCF format */
#if defined(CONFIG_BUILD_COMMON_SERVICE_DCF)
    {
        SVC_USER_PREF_s        *pSvcUserPref;
        if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
            SvcLog_NG("InitTaskEntry", "SvcUserPref_Get failed", 0U, 0U);
        } else {
            AmbaDCF_ConfigFormat(pSvcUserPref->DcfFormatId);
        }
    }
#endif

    /* Flow control mechanism init */
    if (SVC_OK == Rval) {
        Rval = SvcFlowControl_Init(SVC_CONTROL_TASK_PRI, SVC_CONTROL_TASK_CPU_BITS);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_INIT, "## SvcFlowControl_Init() failed with %d", Rval, 0U);
        }
    }

    /* Install SVC flow control command  */
    if (SVC_OK == Rval) {
        Rval = SvcControlTask_CmdInstall();
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_INIT, "## SvcControlTask_CmdInstall() failed with %d", Rval, 0U);
        }
    }

    /* Control task "init" operation executed */
    if (SVC_OK == Rval) {
        Rval = SvcFlowControl_Exec("init");
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_INIT, "## SvcFlowControl_Exec(\"init\") failed with %d", Rval, 0U);
        }
    }

    if (SVC_OK == Rval) {
        ControlTask_ProjectEntry = SvcControlTask_ProjectEntry;
        if (ControlTask_ProjectEntry != NULL) {
            Rval = ControlTask_ProjectEntry();
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_INIT, "## ControlTask_ProjectEntry() failed with %d", Rval, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_INIT, "## ControlTask_ProjectEntry() is not implemented", Rval, 0U);
        }
    }

    SvcLog_OK(SVC_LOG_INIT, "init_task is done", 0U, 0U);
}
