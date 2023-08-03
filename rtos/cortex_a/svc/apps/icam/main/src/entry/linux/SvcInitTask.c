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
#include "AmbaPIO.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcTask.h"
#include "SvcTaskList.h"
#include "SvcBootDev.h"
#include "SvcSysStat.h"
#include "SvcWdt.h"
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
#include "iCamTimeProfile.h"
#include "SvcDramUtil.h"
#include "SvcInitTask.h"
#if defined(CONFIG_ICAM_SWPLL_CONTROL)
#include "AmbaFTCM.h"
#include "SvcSwPll.h"
#endif

#define SVC_LOG_INIT                "INIT"

static void PrintModule_Enable(void)
{
    SVC_USER_PREF_s *pSvcUserPref;

    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
        UINT16 Module = (UINT16)(SSP_ERR_BASE >> 16U);
        UINT32 Rval;
        extern void AmbaDSP_MainApiDumpInit(UINT8 Enable, UINT32 Mask);

        SvcLog_OK(SVC_LOG_INIT, "ssp_log %u, dsp_log %u", pSvcUserPref->ShowSspLog, pSvcUserPref->ShowDspLog);
        SvcLog_OK(SVC_LOG_INIT, "sensor_log %u, bridge_log %u", pSvcUserPref->ShowSensorLog, pSvcUserPref->ShowBridgeLog);
        SvcLog_OK(SVC_LOG_INIT, "swpll_log %u, ik_log %u", pSvcUserPref->ShowSwpllLog, pSvcUserPref->ShowIKLog);
        SvcLog_OK(SVC_LOG_INIT, "ssp_api_log %u, yuv_log %u", pSvcUserPref->ShowSspApiLog, pSvcUserPref->ShowYuvLog);
        Rval = AmbaPrint_ModuleSetAllowList(Module, (UINT8)pSvcUserPref->ShowSspLog);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_INIT, "AmbaPrint_ModuleSetAllowList %u return %u", Module, Rval);
        }
        AmbaDSP_MainApiDumpInit((UINT8)pSvcUserPref->ShowSspApiLog, 0U);

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

#if 0
static void StgMountOp(char Drive, UINT32 MountOp)
{
    /* system will mount directly when it detect sd cards, we don't need to mount it again */
    AmbaMisra_TouchUnused(&Drive);
    AmbaMisra_TouchUnused(&MountOp);
}
#endif

static UINT32 CacheMemCheck(void *pStartAddr, UINT32 Size)
{
    ULONG  StartAddr;

    AmbaMisra_TouchUnused(pStartAddr);

    AmbaMisra_TypeCast(&StartAddr, &pStartAddr);
    return SvcBuffer_CheckCached(StartAddr, Size);
}

static UINT32 Cache_DataClean(ULONG VirtAddr, ULONG Size)
{
    UINT32      PoolType, Rval = SVC_OK;
    AMBA_PIO_s  IoBuf = {0};

    PoolType = SvcBuffer_GetMemPoolType(VirtAddr, (UINT32)Size);

    switch (PoolType) {
        case SVC_MEM_TYPE_DSP:
            Rval = AmbaDSP_MainCacheBufOp(DSP_CACHE_OP_CLEAN, VirtAddr, (UINT32)Size);
            break;
        case SVC_MEM_TYPE_IO:
            if (OK == SvcBuffer_Vir2Phys(VirtAddr, &(IoBuf.PhyAddr))) {
                IoBuf.VirAddr   = VirtAddr;
                IoBuf.Size      = (UINT32)Size;
                IoBuf.Cacheable = 1U;

                Rval = AmbaPIO_CacheClean(&IoBuf);
            }
            break;
        default:
            /* nothing */
            break;
    }

    return Rval;
}

static UINT32 Cache_DataInvalidate(ULONG VirtAddr, ULONG Size)
{
    UINT32      PoolType, Rval = SVC_OK;
    AMBA_PIO_s  IoBuf = {0};

    PoolType = SvcBuffer_GetMemPoolType(VirtAddr, (UINT32)Size);

    switch (PoolType) {
        case SVC_MEM_TYPE_DSP:
            Rval = AmbaDSP_MainCacheBufOp(DSP_CACHE_OP_INVLD, VirtAddr, (UINT32)Size);
            break;
        case SVC_MEM_TYPE_IO:
            if (OK == SvcBuffer_Vir2Phys(VirtAddr, &(IoBuf.PhyAddr))) {
                IoBuf.VirAddr   = VirtAddr;
                IoBuf.Size      = (UINT32)Size;
                IoBuf.Cacheable = 1U;

                Rval = AmbaPIO_CacheInvalid(&IoBuf);
            }
            break;
        default:
            /* nothing */
            break;
    }

    return Rval;
}

static UINT32 Cache_DataFlush(ULONG VirtAddr, ULONG Size)
{
    UINT32      PoolType, Rval = SVC_OK;
    AMBA_PIO_s  IoBuf = {0};

    PoolType = SvcBuffer_GetMemPoolType(VirtAddr, (UINT32)Size);

    switch (PoolType) {
        case SVC_MEM_TYPE_DSP:
            Rval  = AmbaDSP_MainCacheBufOp(DSP_CACHE_OP_CLEAN, VirtAddr, (UINT32)Size);
            Rval |= AmbaDSP_MainCacheBufOp(DSP_CACHE_OP_INVLD, VirtAddr, (UINT32)Size);
            break;
        case SVC_MEM_TYPE_IO:
            if (OK == SvcBuffer_Vir2Phys(VirtAddr, &(IoBuf.PhyAddr))) {
                IoBuf.VirAddr   = VirtAddr;
                IoBuf.Size      = (UINT32)Size;
                IoBuf.Cacheable = 1U;

                Rval  = AmbaPIO_CacheClean(&IoBuf);
                Rval |= AmbaPIO_CacheInvalid(&IoBuf);
            }
            break;
        default:
            /* nothing */
            break;
    }

    return Rval;
}

static void DramActivity(void)
{
#if defined(CONFIG_BUILD_COMSVC_DRAMSHMOO) && defined(CONFIG_DRAM_TYPE_LPDDR5)

    #define SVC_DRAM_ADJ_DURATION   (10000U)    /* adjust every 10 sec */

    SvcDramUtil_Adjust(SVC_DRAM_ADJ_TASK_PRI, SVC_DRAM_ADJ_TASK_CPU_BITS, SVC_DRAM_ADJ_DURATION);

    SvcLog_DBG(SVC_LOG_INIT, "## dram activity is done", 0U, 0U);
#else
    SvcLog_DBG(SVC_LOG_INIT, "## no dram activity", 0U, 0U);
#endif
}

static UINT32 CFS_Vir2Phys(void *VirtAddr, void **PhysAddr)
{
    UINT32    Rval;
    ULONG     Vir;
    ULONG     *pPhy;

    AmbaMisra_TypeCast(&Vir, &VirtAddr);
    AmbaMisra_TypeCast(&pPhy, &PhysAddr);

    Rval = SvcBuffer_Vir2Phys(Vir, pPhy);

    return Rval;
}

static UINT32 CFS_CacheDataClean(void *StartAddr, UINT32 Size)
{
    ULONG     Vir;

    AmbaMisra_TypeCast(&Vir, &StartAddr);

    return Cache_DataClean(Vir, (ULONG)Size);
}

static UINT32 CFS_CacheDataInvalidate(void *StartAddr, UINT32 Size)
{
    ULONG     Vir;

    AmbaMisra_TypeCast(&Vir, &StartAddr);

    return Cache_DataInvalidate(Vir, (ULONG)Size);

}


/**
* create of init task
* @return none
*/
void SvcInitTask_Create(void)
{
    static SVC_PLAT_CB_s  PlatEntry = {
        /* sd card mount/umount */
        //.pCbMountOp = StgMountOp,
        .pCbMountOp = NULL,
        /* buffer pool init */
        .pCbBufPoolInit = iCamBufPool_Init,
        /* cache oprtation */
        .pAmbaCacheClean      = Cache_DataClean,
        .pAmbaCacheInvalidate = Cache_DataInvalidate,
        .pAmbaCacheFlush      = Cache_DataFlush,
        /* cfs operation */
        .pCbCacheChk = CacheMemCheck,
        .pCbCacheCln = CFS_CacheDataClean,
        .pCbCacheInv = CFS_CacheDataInvalidate,
        .pCbVirToPhy = CFS_Vir2Phys,
    #if defined(CONFIG_ICAM_TIMING_LOG)
        /* time profile */
        .pCbTimeProfleSet = iCamTimeProfile_Set,
        .pCbTimeProfleGet = iCamTimeProfile_Get,
    #endif
    };

    static SVC_FLOW_CONTROL_CMD_FUNC ControlTask_ProjectEntry = NULL;

    UINT32  Rval;

    /* Do dram activity */
    DramActivity();

    /* Show build information */
    SvcBuild_ShowInfo();

    /* init platform dependent cb */
    SvcPlat_InitCB(&PlatEntry);

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
    } else {
        SvcWdt_BldWdtClearExpireFlag();
    }

#if defined(CONFIG_ENABLE_EMMC_BOOT)
    Rval = AmbaFS_Mount('c');
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_INIT, "## AmbaFS_Mount failed, return 0x%x", Rval, 0U);
    }
#endif

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
