/**
*  @file iCamSD.c
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
*  @details icam sd card functions
*
*/

#include ".svc_autogen"
#include "AmbaSD.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"
#include "AmbaGPIO.h"
#include "AmbaUtility.h"
#include "AmbaDSP_Liveview.h"

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcTask.h"
#include "SvcWrap.h"
#include "SvcTaskList.h"
#include "SvcUserPref.h"
#include "iCamSD.h"

#if defined(CONFIG_ENABLE_SPINOR_BOOT)
#define AmbaNVM_SysPartConfig  AmbaNORSPI_SysPartConfig
#define AmbaNVM_UserPartConfig AmbaNORSPI_UserPartConfig
#elif defined(CONFIG_ENABLE_NAND_BOOT) || defined(CONFIG_ENABLE_SPINAND_BOOT)
#define AmbaNVM_SysPartConfig  AmbaNAND_SysPartConfig
#define AmbaNVM_UserPartConfig AmbaNAND_UserPartConfig
#elif defined(CONFIG_ENABLE_EMMC_BOOT)
#include ".svc_autogen"
#define AmbaNVM_SysPartConfig  AmbaNAND_SysPartConfig
#define AmbaNVM_UserPartConfig AmbaNAND_UserPartConfig
#else
#pragma message ("[iCamSD.c] ERROR !! UNKNOWN BOOT DEVICE !!")
#endif

#define SVC_LOG_STOG "STOG"
#define SD_INIT_FREQ (300000U)

typedef struct {
    UINT32  ChanNo;
    UINT32  IsEmmcBoot;
    UINT32  DetailDelay;
    UINT32  MaxFreq;
    UINT32  TaskPriority;
    UINT32  TaskCpuBits;
} SVC_STG_SD_CFG_s;

static SVC_TASK_CTRL_s  g_SdTaskCtrl GNU_SECTION_NOZEROINIT;

typedef UINT32 (*SVC_SD_SYSTEM_TASK_CREATE_CB_f)(SD_TASK_ENTRY_f TaskEntry, const UINT32 Arg, AMBA_KAL_TASK_t **pTask);

static UINT32 SD_SystemTaskCreateCb(SD_TASK_ENTRY_f TaskEntry, const UINT32 Arg, AMBA_KAL_TASK_t **pTask)
{
    UINT32                 Rval;
    AMBA_KAL_TASK_ENTRY_f  pfnTk;

    AmbaMisra_TypeCast(&pfnTk, &TaskEntry);

    g_SdTaskCtrl.EntryFunc = pfnTk;
    g_SdTaskCtrl.EntryArg  = Arg;
    Rval = SvcTask_Create("SspSdTask", &g_SdTaskCtrl);
    if (Rval == SVC_OK) {
        *pTask = &(g_SdTaskCtrl.Task);
    } else {
        SvcLog_NG(SVC_LOG_STOG, "Init_task isn't created", 0U, 0U);
    }

    return Rval;
}

static UINT32 iCamSD_Config(const SVC_STG_SD_CFG_s *pSdUsrCfg)
{
    #define SD_TASK_STACK_SIZE      (3072U)

    extern void                     AmbaUserSD_PowerCtrl(UINT32 SdChanNo, UINT32 VddPower);
    extern AMBA_PARTITION_CONFIG_s  AmbaNVM_SysPartConfig[AMBA_NUM_SYS_PARTITION];
    extern AMBA_PARTITION_CONFIG_s  AmbaNVM_UserPartConfig[AMBA_NUM_USER_PARTITION];

    static UINT32  g_SspSdTaskCreated = 0U;
    static UINT8   SspSdTaskStack[SD_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    UINT32                      Rval = SVC_NG;
    AMBA_SD_CONFIG_s            SdConfig;

    SD_SYSTEM_TASK_CREATE_CB_f      TaskCreateCb = NULL;
    SVC_SD_SYSTEM_TASK_CREATE_CB_f  SvcTaskCreateCb = SD_SystemTaskCreateCb;

    if (pSdUsrCfg->ChanNo < AMBA_NUM_SD_CHANNEL) {
        AmbaSvcWrap_MisraMemset(&SdConfig, 0, sizeof(AMBA_SD_CONFIG_s));

        AmbaMisra_TypeCast(&TaskCreateCb, &SvcTaskCreateCb);
        SdConfig.SystemTaskCreateCb = TaskCreateCb;
        /* task config */
        if (g_SspSdTaskCreated == 0U) {
            AmbaSvcWrap_MisraMemset(&g_SdTaskCtrl, 0, sizeof(g_SdTaskCtrl));
            g_SdTaskCtrl.Priority   = pSdUsrCfg->TaskPriority;
            g_SdTaskCtrl.CpuBits    = pSdUsrCfg->TaskCpuBits;
            g_SdTaskCtrl.pStackBase = SspSdTaskStack;
            g_SdTaskCtrl.StackSize  = SD_TASK_STACK_SIZE;
            g_SspSdTaskCreated = 1U;
        }

        /* driver config */
        if (0U < pSdUsrCfg->IsEmmcBoot) {
            SdConfig.PowerCtrl       = NULL;
            SdConfig.pSysPartConfig  = (AMBA_PARTITION_CONFIG_s *)AmbaNVM_SysPartConfig;
            SdConfig.pUserPartConfig = (AMBA_PARTITION_CONFIG_s *)AmbaNVM_UserPartConfig;
        } else {

            if (pSdUsrCfg->ChanNo == AMBA_SD_CHANNEL0) {
                #if defined(CONFIG_MUTI_BOOT_DEVICE)
                extern AMBA_PARTITION_CONFIG_s  AmbaNORSPI_SysPartConfig[AMBA_NUM_SYS_PARTITION];
                extern AMBA_PARTITION_CONFIG_s  AmbaNORSPI_UserPartConfig[AMBA_NUM_USER_PARTITION];

                SdConfig.PowerCtrl       = NULL;
                SdConfig.pSysPartConfig  = AmbaNORSPI_SysPartConfig;
                SdConfig.pUserPartConfig = AmbaNORSPI_UserPartConfig;
                
                #else
                SdConfig.PowerCtrl       = AmbaUserSD_PowerCtrl;
                SdConfig.pSysPartConfig  = NULL;
                SdConfig.pUserPartConfig = NULL;
                #endif

            } else {    // AMBA_SD_CHANNEL1
                SdConfig.PowerCtrl       = AmbaUserSD_PowerCtrl;
                SdConfig.pSysPartConfig  = NULL;
                SdConfig.pUserPartConfig = NULL;
            }
        }
        SdConfig.PhyCtrl                 = NULL;
        SdConfig.SdSetting.ClockDrive    = AMBA_SD_DRIVE_STRENGTH_12MA;
        SdConfig.SdSetting.DataDrive     = AMBA_SD_DRIVE_STRENGTH_12MA;
        SdConfig.SdSetting.CMDDrive      = AMBA_SD_DRIVE_STRENGTH_12MA;
        SdConfig.SdSetting.CDDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
        SdConfig.SdSetting.WPDrive       = AMBA_SD_DRIVE_STRENGTH_12MA;
        SdConfig.SdSetting.DetailDelay   = pSdUsrCfg->DetailDelay;
        SdConfig.SdSetting.InitFrequency = SD_INIT_FREQ;
        SdConfig.SdSetting.MaxFrequency  = pSdUsrCfg->MaxFreq;

        #ifdef CONFIG_MUTI_BOOT_DEVICE
        (void)AmbaGPIO_SetFuncGPO(SVCAQ_EMMC_HS_SEL, AMBA_GPIO_LEVEL_LOW);
        (void)AmbaGPIO_SetFuncGPO(SVCAQ_EMMC_RESET, AMBA_GPIO_LEVEL_HIGH);
        #endif

        Rval = AmbaSD_Config(pSdUsrCfg->ChanNo, &SdConfig);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STOG, "## fail to init storage, err = %u", Rval, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_STOG, "## no entry to config sd", 0U, 0U);
    }

    return Rval;
}


/**
* initialization of storage module
* @return none
*/
void iCamSD_Init(void)
{
    #define ORDER_MAIN_STG      (0U)
    #define ORDER_AUX_STG       (1U)

    SVC_STG_SD_CFG_s  StgCfg;
    SVC_USER_PREF_s   *pUserPref;
    UINT32            i, StgCount = 1U;

    AmbaMisra_TouchUnused(&StgCount);
#if defined(CONFIG_ICAM_AUX_STG)
    StgCount++;
#endif

    if (SvcUserPref_Get == NULL || SvcUserPref_Get(&pUserPref) == SVC_OK) {
        for (i = 0U; i < StgCount; i++) {
            AmbaSvcWrap_MisraMemset(&StgCfg, 0, sizeof(SVC_STG_SD_CFG_s));
            switch (i) {
            case ORDER_AUX_STG:
            #if defined(CONFIG_ICAM_AUX_STG)
                StgCfg.ChanNo = (UINT32)CONFIG_ICAM_AUX_STG_CHA;
            #else
                /* do nothing */
            #endif
                break;
            case ORDER_MAIN_STG:
            default:
                if (SvcUserPref_Get == NULL) {
                    StgCfg.ChanNo = CONFIG_ICAM_MAIN_STG_CHA;
                } else {
                    StgCfg.ChanNo = pUserPref->MainStgChan;
                }
                break;
            }

            StgCfg.TaskPriority = SVC_STG_MONI_TASK_PRI;
            StgCfg.TaskCpuBits  = SVC_STG_MONI_TASK_CPU_BITS;
            if (StgCfg.ChanNo == AMBA_SD_CHANNEL0) {
                /* configure sd0 as main storage */
                StgCfg.DetailDelay = SVCAG_SD0_DELAY;
                StgCfg.MaxFreq     = SVCAG_SD0_CLOCK;
            } else {
                #if defined(SVCAG_SD1_DELAY)
                /* configure sd1 as main storage */
                StgCfg.DetailDelay = SVCAG_SD1_DELAY;
                StgCfg.MaxFreq     = SVCAG_SD1_CLOCK;
                #endif
            }

            if (iCamSD_Config(&StgCfg) != SVC_OK) {
                SvcLog_NG(SVC_LOG_STOG, "## storage(ch%u) isn't initialized", StgCfg.ChanNo, 0U);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_STOG, "## fail to SvcUserPref_Get", 0U, 0U);
    }
}

/**
* initialization of emmc module
* @return 0-OK, 1-NG
*/
UINT32 iCamSD_EMMCBoot(void)
{
#if defined(CONFIG_ENABLE_EMMC_BOOT) || defined(CONFIG_MUTI_BOOT_DEVICE)
    SVC_STG_SD_CFG_s  SdConfig;

    AmbaSvcWrap_MisraMemset(&SdConfig, 0, sizeof(SdConfig));
    SdConfig.ChanNo       = AMBA_SD_CHANNEL0;
    SdConfig.IsEmmcBoot   = 1U;
    #if defined(SVC_BOARD_BUB)
    SdConfig.MaxFreq      = 12000000U;
    #else
    SdConfig.MaxFreq      = 100000000U;
    #endif
    SdConfig.TaskPriority = SVC_STG_MONI_TASK_PRI;
    SdConfig.TaskCpuBits  = SVC_STG_MONI_TASK_CPU_BITS;

    (void)AmbaGPIO_SetFuncGPO(SVCAQ_EMMC_HS_SEL, AMBA_GPIO_LEVEL_LOW);
    (void)AmbaGPIO_SetFuncGPO(SVCAQ_EMMC_RESET, AMBA_GPIO_LEVEL_HIGH);

    return iCamSD_Config(&SdConfig);
#else
    return SVC_OK;
#endif
}
