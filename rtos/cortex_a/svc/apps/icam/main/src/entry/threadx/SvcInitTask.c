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
#include "AmbaFPD.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaSensor.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaShell.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaHDMI_Def.h"
#include "AmbaUtility.h"
#include "AmbaSD.h"
#include "AmbaCache.h"
#if defined(CONFIG_ENABLE_AMBALINK)
#include "AmbaIPC.h"
#endif
#if defined(CONFIG_ICAM_SWPLL_CONTROL)
#include "AmbaFTCM.h"
#include "SvcSwPll.h"
#endif
#if defined(CONFIG_ICAM_FUSA_USED)
#include "AmbaFusa.h"
#endif
#include "AmbaShell_Commands.h"
#include "AmbaI2C.h"

#include "bsp.h"

#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcUtil.h"
#include "AmbaVfs.h"
#include "SvcLog.h"
#include "SvcTask.h"
#include "SvcTaskList.h"
#include "SvcShell.h"
#include "SvcBootDev.h"
#include "iCamSD.h"
#include "SvcUcode.h"
#include "SvcDSP.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcSysStat.h"
#include "SvcWdt.h"
#include "AmbaUtc.h"
#include "SvcVinSrc.h"
#include "SvcVoutSrc.h"
#include "SvcIK.h"
#include "SvcIKCfg.h"
#include "SvcLiveview.h"
#include "SvcDisplay.h"
#include "SvcIsoCfg.h"
#include "SvcSysEvent.h"
#include "SvcStgMonitor.h"
#include "SvcStgMgr.h"
#include "SvcCmdDSP.h"
#include "SvcCmdApp.h"
#include "SvcRecMain.h"
#include "SvcResCfg.h"
#include "SvcOsd.h"
#include "SvcGui.h"
#include "SvcRawCap.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "SvcUserPref.h"
#include "SvcUserPrefCustom.h"
#include "SvcInfoPack.h"
#include "SvcBuild.h"
#include "SvcInitTask.h"
#include "SvcGpioButton.h"
#include "SvcDisplayTask.h"
#include "SvcVoutCtrlTask.h"
#include "SvcFwUpdate.h"
#include "AmbaDCF.h"
#include "SvcUserPref.h"
#include "SvcCmdDiag.h"
#include "SvcCmdDSP.h"
#include "SvcCmdSys.h"
#include "SvcCmdPbk.h"
#include "SvcCmd.h"
#include "SvcShell.h"
#include "SvcControlTask.h"
#include "SvcFlowControl.h"
#include "SvcAppStat.h"
#include "SvcSafeStateMgr.h"
#include "SvcDramUtil.h"
#if defined(CONFIG_NETX_ENET) || defined(CONFIG_LWIP_ENET)
#include "AmbaShell_Enet.h"
#endif
#include "SvcPlat.h"
#include "iCamAudio.h"
#include "iCamBufPool.h"
#include "iCamTimeProfile.h"
#if defined(CONFIG_BUILD_CV)    /* CV related function */
#include "SvcCvMainTask.h"
#endif
#include "SvcAdvancedShmooTask.h"

#define SVC_LOG_INIT                "INIT"

#define INIT_TASK_STACK_SIZE        (0x8000U)

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

static UINT32 ShellInit(UINT32 CliPriority,
                        UINT32 CliCpuBits,
                        UINT32 ExecPriority,
                        UINT32 ExecCpuBits)
{
    #define SHELL_CLI_STACK_SIZE        (0x1000U)
    #define SHELL_EXEC_STACK_SIZE       (0x320000U)

    extern void  AmbaSensorUT_TestCmd(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
#ifdef CONFIG_ENABLE_AMBALINK
    extern void AmbaShell_CommandAmbaLink(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc);
#endif
    extern void AmbaEEPRomUT_TestCmd(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);

    static UINT8 CliTaskStack[SHELL_CLI_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 ExecTaskStack[SHELL_EXEC_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    static const AMBA_SHELL_COMMAND_s BasicCmds[] = {
        {.pName = "help",    .MainFunc = AmbaShell_CommandHelp,          .pNext = NULL},
        {.pName = "cat",     .MainFunc = AmbaShell_CommandCat,           .pNext = NULL},
        {.pName = "cd",      .MainFunc = AmbaShell_CommandChangeDir,     .pNext = NULL},
        {.pName = "cp",      .MainFunc = AmbaShell_CommandCopy,          .pNext = NULL},
        {.pName = "format",  .MainFunc = AmbaShell_CommandFormat,        .pNext = NULL},
        {.pName = "ls",      .MainFunc = AmbaShell_CommandListDir,       .pNext = NULL},
        {.pName = "mkdir",   .MainFunc = AmbaShell_CommandMakeDir,       .pNext = NULL},
        {.pName = "rmdir",   .MainFunc = AmbaShell_CommandRemoveDir,     .pNext = NULL},
        {.pName = "mv",      .MainFunc = AmbaShell_CommandMove,          .pNext = NULL},
        {.pName = "rm",      .MainFunc = AmbaShell_CommandRemove,        .pNext = NULL},
        {.pName = "vol",     .MainFunc = AmbaShell_CommandVolumn,        .pNext = NULL},
        {.pName = "readl",   .MainFunc = AmbaShell_CommandReadLong,      .pNext = NULL},
        {.pName = "writel",  .MainFunc = AmbaShell_CommandWriteLong,     .pNext = NULL},
        {.pName = "ps",      .MainFunc = AmbaShell_CommandProcessStatus, .pNext = NULL},
        {.pName = "savebin", .MainFunc = AmbaShell_CommandSaveBinary,    .pNext = NULL},
        {.pName = "loadbin", .MainFunc = AmbaShell_CommandLoadBinary,    .pNext = NULL},
        {.pName = "dmesg",   .MainFunc = SvcCmd_DisplayMsg,              .pNext = NULL},
        {.pName = "sleep",   .MainFunc = AmbaShell_CommandSleep,         .pNext = NULL},
        {.pName = "reboot",  .MainFunc = AmbaShell_CommandReboot,        .pNext = NULL},
        {.pName = "sdshmoo", .MainFunc = AmbaShell_SdShmoo,              .pNext = NULL},
        {.pName = "thruput", .MainFunc = AmbaShell_CommandFsThruput,     .pNext = NULL},
        {.pName = "date",    .MainFunc = AmbaShell_CommandDate,          .pNext = NULL},
        {.pName = "sysinfo", .MainFunc = AmbaShell_CommandSysInfo,       .pNext = NULL},

        {.pName = "sensor",  .MainFunc = AmbaSensorUT_TestCmd,           .pNext = NULL},
#if defined(CONFIG_ICAM_EEPROM_USED)
        {.pName = "eeprom",  .MainFunc = AmbaEEPRomUT_TestCmd,           .pNext = NULL},
#endif
        {.pName = "gpio",    .MainFunc = SvcCmd_Gpio,                    .pNext = NULL},

#ifdef CONFIG_ENABLE_AMBALINK
        {.pName = "ambalink", .MainFunc = AmbaShell_CommandAmbaLink,     .pNext = NULL},
#endif
#ifdef CONFIG_ICAM_USB_USED
        { .pName = "usb",   .MainFunc = AmbaShell_CommandUsb,            .pNext = NULL },
#endif
#if defined(CONFIG_NETX_ENET) || defined(CONFIG_LWIP_ENET)
        { .pName = "svc_net",.MainFunc = AmbaShell_CommandEnet,          .pNext = NULL },
#endif
        { .pName = "atf", .MainFunc = AmbaShell_CommandAtf,              .pNext = NULL },
#if defined(CONFIG_ICAM_FUSA_USED)
        { .pName = "fusa", .MainFunc = AmbaShell_CommandFuSa,            .pNext = NULL },
#endif
    };

    UINT32               i, Rval, CmdNum;
    AMBA_SHELL_CONFIG_s  Config = {0};

    Config.CliTaskPriority    = CliPriority;
    Config.CliTaskSmpCoreSet  = CliCpuBits;
    Config.pCliTaskStack      = CliTaskStack;
    Config.CliTaskStackSize   = SHELL_CLI_STACK_SIZE;
    Config.ExecTaskPriority   = ExecPriority;
    Config.ExecTaskSmpCoreSet = ExecCpuBits;
    Config.pExecTaskStack     = ExecTaskStack;
    Config.ExecTaskStackSize  = SHELL_EXEC_STACK_SIZE;
    Config.GetCharFunc        = AmbaUserShell_Read;
    Config.PutCharFunc        = AmbaUserShell_Write;

    Rval = AmbaShell_Init(&Config);
    if (SHELL_ERR_SUCCESS == Rval) {
        CmdNum = ((UINT32)sizeof(BasicCmds)) / ((UINT32)sizeof(AMBA_SHELL_COMMAND_s));
        for (i = 0; i < CmdNum; i++) {
            Rval = SvcCmd_CommandRegister(&BasicCmds[i]);
            if (SHELL_ERR_SUCCESS != Rval) {
                SvcLog_NG(SVC_LOG_INIT, "## fail to register cmd", 0U, 0U);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_INIT, "## fail to init shell", 0U, 0U);
    }

    /* install svc dsp command */
    SvcCmdDSP_Install();
    /* install svc sys command */
    SvcCmdSys_Install();

#ifdef CONFIG_ICAM_PLAYBACK_USED
    SvcCmdPbk_Install();
#endif

#if defined(CONFIG_ICAM_USE_DIAG_COMMANDS)
    /* diag commands */
    SvcCmdDiag_Install();
#endif

    return Rval;
}

static UINT32 CacheMemCheck(void *pStartAddr, UINT32 Size)
{
    ULONG  StartAddr;

    AmbaMisra_TouchUnused(pStartAddr);

    AmbaMisra_TypeCast(&StartAddr, &pStartAddr);
    return SvcBuffer_CheckCached(StartAddr, Size);
}

static UINT32 CFS_CacheDataClean(void *StartAddr, UINT32 Size)
{
    ULONG Vir;
    ULONG AlignAddr, AlignSize;
    AmbaMisra_TypeCast(&Vir, &StartAddr);

    AlignAddr = Vir & AMBA_CACHE_LINE_MASK;
    AlignSize = (((Vir - AlignAddr) + (ULONG)Size) + (AMBA_CACHE_LINE_SIZE - 1UL)) & AMBA_CACHE_LINE_MASK;

    return AmbaCache_DataClean(AlignAddr, AlignSize);
}

static UINT32 CFS_CacheDataInvalidate(void *StartAddr, UINT32 Size)
{
    ULONG Vir;
    ULONG AlignAddr, AlignSize;
    AmbaMisra_TypeCast(&Vir, &StartAddr);

    AlignAddr = Vir & AMBA_CACHE_LINE_MASK;
    AlignSize = (((Vir - AlignAddr) + (ULONG)Size) + (AMBA_CACHE_LINE_SIZE - 1UL)) & AMBA_CACHE_LINE_MASK;

    return AmbaCache_DataInvalidate(AlignAddr, AlignSize);

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

static void* InitTaskEntry(void* EntryArg)
{
    static SVC_PLAT_CB_s  PlatEntry = {
        /* sd card mount/umount */
        .pCbMountOp = NULL,
    #if defined(CONFIG_ICAM_AUDIO_USED)
        /* audio driver control */
        .pCbADrvInit = iCamAudio_DrvInit,
        .pCbAinCtrl  = iCamAudio_AinCtrl,
        .pCbAoutCtrl = iCamAudio_AoutCtrl,
        .pCbADrvCtrl = iCamAudio_DrvCtrl,
    #endif
        /* cache memory check */
        .pCbCacheChk = CacheMemCheck,
        .pCbCacheCln = CFS_CacheDataClean,
        .pCbCacheInv = CFS_CacheDataInvalidate,
        /* buffer pool init */
        .pCbBufPoolInit = iCamBufPool_Init,
        .pCbBufPoolCvInit = iCamBufPool_CV_Init,
        /* cache oprtation */
        .pAmbaCacheClean      = AmbaCache_DataClean,
        .pAmbaCacheInvalidate = AmbaCache_DataInvalidate,
        .pAmbaCacheFlush      = AmbaCache_DataFlush,
    #if defined(CONFIG_ICAM_TIMING_LOG)
        /* time profile */
        .pCbTimeProfleSet = iCamTimeProfile_Set,
        .pCbTimeProfleGet = iCamTimeProfile_Get,
    #endif
    };

    static SVC_FLOW_CONTROL_CMD_FUNC ControlTask_ProjectEntry = NULL;
    UINT32           Rval;

    /* Do dram activity */
    DramActivity();

    /* Show build information */
    SvcBuild_ShowInfo();

    /* init platform dependent cb */
    SvcPlat_InitCB(&PlatEntry);

    /* System event init */
    SvcSysEvent_Init();

    /* ambalink init */
    {
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
        {
            SVC_SAFE_STATE_CALLBACK_s Callback = {NULL};
            #if defined(CONFIG_BUILD_CV)
            Callback.pEnableVpuBist  = SvcCvMainTask_EnableVpuBist;
            #else
            Callback.pEnableVpuBist  = NULL;
            #endif

            #if defined (CONFIG_ICAM_PROJECT_SHMOO)
            Callback.pChkDdrShm = SvcAdvancedShmooTask_StatusCheck;
            #else
            Callback.pChkDdrShm = NULL;
            #endif
            SvcSafeStateMgr_RegisterCallback(&Callback);
        }

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
#else
        #if defined(CONFIG_AMBALINK_BOOT_OS)
        Rval = AmbaPrint_ModuleSetAllowList(AMBALINK_MODULE_ID, 1U);
        if (SVC_OK == Rval) {
            Rval = AmbaIPC_Init();
            if (Rval == SVC_OK) {
                SvcLog_OK(SVC_LOG_INIT, "AmbaIPC_Init done", 0U, 0U);
            } else {
                SvcLog_NG(SVC_LOG_INIT, "AmbaIPC_Init failed with 0x%x", Rval, 0U);
            }
        }
        #endif
#endif
    }

    SvcWdt_WdtStop();

    SvcClock_Init(CONFIG_ICAM_CLK_LINE);
    SvcClock_Dump();

    SvcWdt_AppWdtStart();
    SvcWdt_AppWdtTaskCreate();

    {
        AMBA_RTC_DATE_TIME_s Calendar;

        (void)AmbaRTC_GetSysTime(&Calendar);
        if (Calendar.Year < 2023U) {
            Calendar.Year = 2023U;
            Calendar.Month = 5U;
            Calendar.Day = 1U;
            (void)AmbaRTC_SetSysTime(&Calendar);
        }
    }

    AmbaMisra_TouchUnused(EntryArg);

#if defined(CONFIG_FWPROG_R52SYS_ENABLE) && defined(CONFIG_BUILD_CV)
    Rval = SvcSafeStateMgr_EnableVpBist(); /* This will enable FEX feature. So it needs to be after SvcClock_Init. */
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_INIT, "SvcSafeStateMgr_EnableVpBist failed with 0x%x", Rval, 0U);
    }
#endif

#ifdef CONFIG_BUILD_COMAMBA_CODEC_COMMON
    AmbaUtcInit();
#endif

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    {
        extern int app_pre_init(void);
        app_pre_init();
    }
#endif

    /* init file system */
    Rval = AmbaFS_Init();
    if (AMBA_FS_ERR_NONE != Rval) {
        SvcLog_NG(SVC_LOG_INIT, "## fs isn't initialized", 0U, 0U);
    }

    /* init boot device */
#if defined(CONFIG_ENABLE_EMMC_BOOT) || defined(CONFIG_MUTI_BOOT_DEVICE)
    Rval = SvcBootDev_Init(iCamSD_EMMCBoot);
#else
    Rval = SvcBootDev_Init(NULL);
#endif
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_INIT, "## boot_device isn't initialized", 0U, 0U);
    } else {
        SvcWdt_BldWdtClearExpireFlag();
    }

    /* Get svc user preference */
    Rval = SvcUserPref_Init();
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_INIT, "## fail to get svc preference", 0U, 0U);
    }

    Rval = SvcUserPrefCustom_Init();
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_INIT, "## fail to get svc preference", 0U, 0U);
    }

    /* init storage */
#if defined(CONFIG_ENABLE_EMMC_BOOT) || defined(CONFIG_MUTI_BOOT_DEVICE)
#else
    iCamSD_Init();
#endif

    /* init svc system status */
    Rval = SvcSysStat_Init(SVC_APP_STAT_NUM);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_INIT, "## fail to init svc status", 0U, 0U);
    }

    /* init shell */
    Rval = ShellInit(SVC_SHELL_CLI_TASK_PRI,
                     SVC_SHELL_CLI_TASK_CPU_BITS,
                     SVC_SHELL_EXEC_TASK_PRI,
                     SVC_SHELL_EXEC_TASK_CPU_BITS);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_INIT, "shell isn't initialized", 0U, 0U);
    }

    /* Enable differnt AmbaPrint Module for debug if necessary */
    PrintModule_Enable();

    /* init DCF format */
#ifdef CONFIG_BUILD_COMMON_SERVICE_DCF
    {
        SVC_USER_PREF_s        *pSvcUserPref;
        if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
            SvcLog_NG("InitTaskEntry", "SvcUserPref_Get failed", 0U, 0U);
        } else {
            AmbaDCF_ConfigFormat(pSvcUserPref->DcfFormatId);
        }
    }
#endif

    /* install firmware update command */
    SvcFwUpdate_CmdInstall();

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

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);
        if (pSvcUserPref->OperationMode != 0U) {
            extern int app_init_check(void);
            while (app_init_check() == 0) {
                AmbaKAL_TaskSleep(100);
            }
        }
    }
#endif

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
#if 0
    {
    extern UINT32 AmbaRTSL_PllSetFreq(UINT32 PllIdx, UINT32 Frequency);
    AmbaRTSL_PllSetFreq(AMBA_PLL_HDMI, 0);// close HDMI
    AmbaRTSL_PllSetFreq(AMBA_PLL_VIDEO2, 0);// close lcd
    AmbaRTSL_PllSetFreq(AMBA_PLL_ENET, 0);// close ENET
    }
#endif

    SvcLog_OK(SVC_LOG_INIT, "init_task is done", 0U, 0U);

    return NULL;
}

/**
* create of init task
* @param [in] task priority
* @param [in] task cpu bits
* @return none
*/
void SvcInitTask_Create(UINT32 Priority, UINT32 CpuBits)
{
    static SVC_TASK_CTRL_s InitTaskCtrl GNU_SECTION_NOZEROINIT;
    static UINT8 InitTaskStack[INIT_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    UINT32  Rval;

    InitTaskCtrl.Priority   = Priority;
    InitTaskCtrl.EntryFunc  = InitTaskEntry;
    InitTaskCtrl.EntryArg   = 0;
    InitTaskCtrl.pStackBase = InitTaskStack;
    InitTaskCtrl.StackSize  = INIT_TASK_STACK_SIZE;
    InitTaskCtrl.CpuBits    = CpuBits;

    Rval = SvcTask_Create("SvcInitTask", &InitTaskCtrl);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_INIT, "Init_task isn't created", 0U, 0U);
    }
}
