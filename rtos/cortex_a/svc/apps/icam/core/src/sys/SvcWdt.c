/**
 *  @file SvcWdt.c
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
*  @details svc watching dog
*
*/

#include "AmbaPrint.h"
#include "AmbaGPIO.h"
#include "AmbaMisraFix.h"

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcTask.h"
#include "SvcWdt.h"
#include "SvcNvm.h"
#include ".svc_autogen"

#define WDT_OK(a, b, c) SvcLog_OK("WDT", a, b, c)
#if defined(CONFIG_SVC_ENABLE_WDT)
#define WDT_NG(a, b, c) SvcLog_NG("WDT", a, b, c)
#include "AmbaRTSL_PWC.h"
#include "AmbaRTSL_SpiNOR.h"
#include "AmbaRTSL_SD.h"
#include "AmbaSpiNOR_Ctrl.h"

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
/* CV2FS has more than 1 watchdog, please select the WDT ID here */
#define SVC_WDT_ID            (0U)
#define WdtStart(a, b)        AmbaWDT_StartByChId(SVC_WDT_ID, a, b)
#define WdtStop()             AmbaWDT_StopByChId(SVC_WDT_ID)
#define WdtFeed()             AmbaWDT_FeedByChId(SVC_WDT_ID)
#define WdtHookHdlr(a, b)     AmbaWDT_HookHandler(SVC_WDT_ID, a, b)
#define WdtGetInfo(a)         AmbaWDT_GetInfoByChId(SVC_WDT_ID, a)
#else
#define WdtStart(a, b)        AmbaWDT_Start(a, b)
#define WdtStop()             AmbaWDT_Stop()
#define WdtFeed()             AmbaWDT_Feed()
#define WdtHookHdlr(a, b)     AmbaWDT_HookTimeOutHandler(a, b)
#define WdtGetInfo(a)         AmbaWDT_GetInfo(a)
#endif
#define WDT_TASK_PRI          (40U)
#define WDT_TASK_CPU_BITS     (0x01U)
#define WDT_STACK_SIZE        (0x1000UL)
#define WDT_MAGIC             (0xEEU)

static AMBA_USER_PARTITION_TABLE_s UserPTB = {0U};
static SVC_TASK_CTRL_s WdtTask;
UINT32 SvcWdt_FeedTest     = 1U;
UINT32 SvcWdt_FeedPrint    = 0U;
UINT32 SvcWdt_FeedInterval = WDT_TIMEOUT - WDT_TIMEOUT_TOLERANCE;

UINT32 SvcWdt_SetFlag(UINT32 ID, UINT32 Ena)
{
    UINT32 Rval;

    /* WDT enable flag was saved in User PTB 0 table, reserved[0], in UINT8 format. */
    Rval = SvcNvm_ReadUserPartitionTable(&UserPTB, 0U, 1000U);
    if (SVC_OK == Rval) {
        if (0U == Ena) {
            UserPTB.Reserved[ID] = 0U;
        } else {
            UserPTB.Reserved[ID] = WDT_MAGIC;
        }
        Rval = SvcNvm_WriteUserPartitionTable(&UserPTB, 0U, 5000U);
        if (SVC_OK != Rval) {
            WDT_NG("AmbaNVM_WriteUserPartitionTable failed", 0U, 0U);
        }
    } else {
        WDT_NG("AmbaNVM_ReadUserPartitionTable failed", 0U, 0U);
    }

    return Rval;
}

UINT32 SvcWdt_GetFlag(UINT32 ID, UINT32* Ena)
{
    UINT32 Rval;

    /* WDT enable flag was saved in User PTB 0 table, reserved[0], in UINT8 format. */
    Rval = SvcNvm_ReadUserPartitionTable(&UserPTB, 0U, 1000U);
    if (SVC_OK == Rval) {
        #if 0 /* debug */
        UINT32 i;
        UINT32 num;
        for (i = 0; i < 10; i++) {
            num = (0U | UserPTB.Reserved[i]);
            WDT_NG("UserPTB.Reserved[%u] %u", i, num);
        }
        #endif
        if (WDT_MAGIC == UserPTB.Reserved[ID]) {
            *Ena = 1U;
        } else {
            *Ena = 0U;
        }

    } else {
        WDT_NG("AmbaNVM_ReadUserPartitionTable failed", 0U, 0U);
    }

    return Rval;
}

#if !defined(CONFIG_LINUX) && !defined(CONFIG_GPIO_PIN_WDT_EXT)
static void WdtCallback(UINT32 UserArg)
{
    UINT32 Rval = SVC_OK;
    
    /* Restore some hardware settings to make boot code is able to be reloaded successfully. */
    /* If a hardware register could be reset by soc soft-reset, it needs not to be programmed here. */
    {
        #if defined(CONFIG_ENABLE_SPINOR_BOOT)
        AmbaSPINOR_RestoreRebootClkSetting();
        #endif

        #if defined(CONFIG_ENABLE_EMMC_BOOT)
        AmbaRTSL_SDPhyDisable(0U);
        Rval |= AmbaRTSL_PllSetSd0Clk(24000000U);
        AmbaRTSL_SdClockEnable( 0U, 1U);
        /* Disable the interrupt for avoiding the other emmc command operation */
        AmbaRTSL_GicIntGlobalDisable();
        Rval |= AmbaRTSL_SdSendCMD0(0U, 0xF0F0F0F0U);
        #endif
    }

    /* Trigger soc soft-reset */
    AmbaRTSL_PwcReboot();

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(&UserArg);
}
#endif

/**
* watchdog task entry
* @param [in] none
* @return none
*/
static void* WdtTaskEntry(void * EntryArg)
{
    UINT32 Rval = SVC_OK;

    WDT_OK("WdtTaskEntry start !!", 0U, 0U);

    while (SVC_OK == Rval) {
        if (0U != SvcWdt_FeedTest) {
            Rval = WdtFeed();
            if (SVC_OK != Rval) {
                WDT_NG("AmbaWDT_Feed dog feed failed !!", 0U, 0U);
            } else {
                if (0U != SvcWdt_FeedPrint) {
                    WDT_OK("AmbaWDT_Feed OK !!", 0U, 0U);    
                }
            }
        }

        /* feed the dog before it timeout */
        Rval = AmbaKAL_TaskSleep(SvcWdt_FeedInterval);
        if (SVC_OK != Rval) {
            WDT_NG("AmbaKAL_TaskSleep failed !!", 0U, 0U);
        } else {
            if (0U != SvcWdt_FeedPrint) {
                WDT_OK("AmbaKAL_TaskSleep %u !!", SvcWdt_FeedInterval, 0U);
            }
        }
    }
    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TouchUnused(&WdtTask);

    return NULL;
}

void SvcWdt_WdtStop(void)
{
    UINT32 Rval = WdtStop();
    if (SVC_OK != Rval) {
        WDT_NG("AmbaWDT_Stop failed !!", 0U, 0U);
    } else {
        WDT_OK("AmbaWDT_Stop OK !!", 0U, 0U);
    }
}

void SvcWdt_BldWdtClearExpireFlag(void)
{
    /* set watchdog expire flag as 0 for system boot success */
    UINT32 Rval = SvcWdt_SetFlag(SVC_WDT_EXPIRE, 0U);
    if (SVC_OK != Rval) {
        WDT_NG("SetWdt set expire as 0 failed, Rval 0x%X", Rval, 0U);
    } else {
        WDT_OK("SetWdt set expire as 0 OK", 0U, 0U);
    }
}

/**
* watchdog task init
* @param [in] none
* @return none
*/
void SvcWdt_AppWdtTaskCreate(void)
{
    UINT32 Rval = SVC_OK;
    static UINT8 WdtStack[WDT_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    /* Create data loader task */
    WdtTask.Priority   = WDT_TASK_PRI;
    WdtTask.CpuBits    = WDT_TASK_CPU_BITS;
    WdtTask.StackSize  = WDT_STACK_SIZE;
    WdtTask.EntryFunc  = WdtTaskEntry;
    WdtTask.pStackBase = WdtStack;
    WdtTask.EntryArg   = 0U;
    Rval = SvcTask_Create("SvcWdtTask", &WdtTask);
    if (SVC_OK != Rval) {
        WDT_NG("### WdtInitTask: SvcTask_Create failed, Rval 0x%X", Rval, 0U);
    } else {
        WDT_OK("### WdtInitTask: SvcTask_Create OK", 0U, 0U);
    }
}

void SvcWdt_AppWdtStart(void)
{
    UINT32 Rval = SVC_OK;

    #if !defined(CONFIG_LINUX) && !defined(CONFIG_GPIO_PIN_WDT_EXT)
    /* Hooking hanlder will make watchdog work under IRQ callback mode,
       if you using Linux OS, which has only reset mode,
       or watchdog work under external pin mode,
       dont hook handler here. */
    Rval |= WdtHookHdlr(WdtCallback, 0U);
    #endif

    Rval |= WdtStart(WDT_TIMEOUT, 10U);
    if (SVC_OK != Rval) {
        WDT_NG("AmbaWDT_Start failed !! try to stop watchdog", 0U, 0U);
        Rval = WdtStop();
        if (SVC_OK != Rval) {
            WDT_NG("AmbaWDT_Stop failed !!", 0U, 0U);
        }
    } else {
        AMBA_WDT_INFO_s Info;
        (void) WdtGetInfo(&Info);
        AmbaPrint_PrintUInt5("[WDT|OK]: watchdog start OK, timeout %u ms, tolerance %u, expire action %u,", WDT_TIMEOUT, WDT_TIMEOUT_TOLERANCE, Info.ExpireAction, 0U, 0U);
        
        #if defined(CONFIG_GPIO_PIN_WDT_EXT)
        {
        AMBA_GPIO_INFO_s PinInfo = {0U};
        (void) AmbaGPIO_GetInfo((UINT32)CONFIG_GPIO_PIN_WDT_EXT, &PinInfo);
        AmbaPrint_PrintUInt5("[WDT|OK]: watchdog ext pin (%u) gpio type is (%u),", ((UINT32)CONFIG_GPIO_PIN_WDT_EXT & 0x00000FFFU), PinInfo.PinFunc, 0U, 0U, 0U);
        }
        #endif
    }
}

UINT32 SvcWdt_Info(void)
{
    AMBA_WDT_INFO_s Info = {0U};
    UINT32 Rval = WdtGetInfo(&Info);
    if (SVC_OK != Rval) {
        WDT_NG("AmbaWDT_GetInfo failed, Rval 0x%X", Rval, 0U);
    }
    WDT_OK("", 0U, 0U);
    WDT_OK("### Wdt info: ExpireAction               %u", Info.ExpireAction, 0U);
    WDT_OK("###         : TimerExpired               %u", Info.TimerExpired, 0U);
    WDT_OK("###         : default expire time   (ms) %u", WDT_TIMEOUT, 0U);
    WDT_OK("###         : default feed interval (ms) (%u - %u)", WDT_TIMEOUT, WDT_TIMEOUT_TOLERANCE);
    WDT_OK("###         : current feed interval (ms) %u", SvcWdt_FeedInterval, 0U);
    #if defined(CONFIG_GPIO_PIN_WDT_EXT)
    {
    AMBA_GPIO_INFO_s PinInfo = {0U};
    Rval = AmbaGPIO_GetInfo((UINT32)CONFIG_GPIO_PIN_WDT_EXT, &PinInfo);
    if (SVC_OK != Rval) {
        WDT_NG("AmbaGPIO_GetInfo failed, Rval 0x%X", Rval, 0U);
    }
    WDT_OK("###         : watchdog ext pin (%u) gpio type is (%u)", ((UINT32)CONFIG_GPIO_PIN_WDT_EXT & 0x00000FFFU), PinInfo.PinFunc);
    }
    #endif
    return SVC_OK;
}

#else

/* print module */
#define WDT_MODULE_ID    ((UINT16)(SENSOR_ERR_BASE >> 16U))
#define WDT_DBG(a, b, c) AmbaPrint_ModulePrintUInt5(WDT_MODULE_ID, a, b, c, 0U, 0U, 0U)

/* ***************************************************** *
 *               watchdog is not enabled.                *
 * ***************************************************** */
void SvcWdt_AppWdtStart(void)                 { WDT_OK( "### SvcWdt is not enabled", 0U, 0U); }
void SvcWdt_WdtStop(void)                     { WDT_DBG("### SvcWdt is not enabled", 0U, 0U); }
void SvcWdt_AppWdtTaskCreate(void)            { WDT_DBG("### SvcWdt is not enabled", 0U, 0U); }
void SvcWdt_BldWdtClearExpireFlag(void)       { WDT_DBG("### SvcWdt is not enabled", 0U, 0U); }
UINT32 SvcWdt_Info(void)                      { WDT_DBG("### SvcWdt is not enabled", 0U, 0U);                                                          return SVC_OK; }
UINT32 SvcWdt_GetFlag(UINT32 ID, UINT32* Ena) { WDT_DBG("### SvcWdt is not enabled", 0U, 0U); AmbaMisra_TouchUnused(&ID); AmbaMisra_TouchUnused( Ena); return SVC_OK; }
UINT32 SvcWdt_SetFlag(UINT32 ID, UINT32  Ena) { WDT_DBG("### SvcWdt is not enabled", 0U, 0U); AmbaMisra_TouchUnused(&ID); AmbaMisra_TouchUnused(&Ena); return SVC_OK; }

#endif
