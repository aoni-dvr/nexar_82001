/**
 *  @file SvcSysEntry.c
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
 *  @details Entry point of SVC
 *
 */

#include "AmbaTypes.h"
#include "AmbaSYS.h"
#include "AmbaGPIO.h"
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWdt.h"
#include "SvcClock.h"
#include "SvcUart.h"
#include "SvcPrint.h"
#include "SvcTaskList.h"
#include "SvcExcept.h"
#include "SvcInitTask.h"
#include "iCamTimeProfile.h"
#include "SvcTiming.h"

extern const AMBA_GPIO_DEFAULT_s GpioPinGrpConfig;

static void SvcSysInitPreOS(void)
{
#if defined(CONFIG_ICAM_TIMING_LOG)
    AmbaTime_A53Bld(A53_BLD_PREOS_CONFIG);
#endif

#if defined(CONFIG_ATT_MAP)
    /* init ATT */
    {
        extern void AmbaMemProt_Config(UINT32 EnableFlag);

        AmbaMemProt_Config(1U);
    }
#endif

    /* GPIO Initializations before OS running */
    if (GPIO_ERR_NONE != AmbaGPIO_LoadDefaultRegVals(&GpioPinGrpConfig)) {
        SvcLog_NG(SVC_LOG_SYS, "## Fail to initialize GPIO", 0U, 0U);
    }
}

static void SvcSysInitPostOS(void)
{
    extern void AmbaDiag_IntEnableProfiler(void);

    UINT32  Rval;

    /* init uart/print_task */
    Rval = SvcUart_Init();
    if (SVC_OK == Rval) {
        Rval = SvcPrint_Init(SVC_PRINT_TASK_PRI, SVC_PRINT_TASK_CPU_BITS);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_SYS, "uart/print_task isn't initialized", 0U, 0U);
        }
    }

    /* enable irq profiler */
    AmbaDiag_IntEnableProfiler();

    /* hook stack overflow handler */
    Rval = AmbaKAL_HookStkErrHandler(SvcExcept_StackOverflow);
    if (KAL_ERR_NONE != Rval) {
        SvcLog_NG(SVC_LOG_SYS, "hook stack overflow handler failed", 0U, 0U);
    }

#if defined(CONFIG_ICAM_TIMING_LOG)
    /* time profile initialization */
    iCamTimeProfile_Init();
    iCamTimeProfile_Set(SVC_TIME_POSTOS_CONFIG);
#endif

    /* create init task */
    SvcInitTask_Create(SVC_INIT_TASK_PRI, SVC_INIT_TASK_CPU_BITS);
}

static void SvcSysIdleFunc(void)
{
}

AMBA_SYS_USER_CALLBACKS_s AmbaSysUserCallbacks = {
    .UserEntryBefOS       = SvcSysInitPreOS,
    .UserEntryAftOS       = SvcSysInitPostOS,
    .UserIdleCallback     = SvcSysIdleFunc,
    .UserExceptionHandler = SvcExcept_Handler
};

