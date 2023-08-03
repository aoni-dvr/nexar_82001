/**
 *  @file AmbaINT.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Interrupt Controller driver Middleware APIs
 *
 */

#include <errno.h>
#include <sys/neutrino.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <fcntl.h>
#include <pthread.h>

#include "AmbaTypes.h"
#include "AmbaINT.h"

#define AMBA_INT_PRIORITY   128U

typedef struct {
    UINT32 Id;
    UINT32 IntFuncArg;
    AMBA_INT_ISR_f IntFunc;
} AMBA_INT_SETTING_s;

static void *INT_ISR(void* argv)
{
    struct sigevent event;
    int iid;
    AMBA_INT_SETTING_s *pIntSetting = (AMBA_INT_SETTING_s *)argv;

    UINT32 IntID = pIntSetting->Id;
    UINT32 IntFuncArg = pIntSetting->IntFuncArg;
    AMBA_INT_ISR_f IntFunc = pIntSetting->IntFunc;

    if (IntFunc != NULL) {
        SIGEV_INTR_INIT(&event);
        iid = InterruptAttachEvent(IntID, &event, 0U);
        InterruptMask(IntID, -1);

        while(1) {
            InterruptWait(0, NULL);
            IntFunc(IntID, IntFuncArg);
            InterruptUnmask(IntID, iid);
        }
    }

    return NULL;
}

/**
 *  AmbaINT_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 */
UINT32 AmbaINT_DrvEntry(void)
{
    return INT_ERR_NONE;
}

/**
 *  AmbaINT_GlobalMaskEnable - Disable interrupts from GIC to processors
 *  @note It doesn't change the enable state of each interrupt
 *  @return error code
 */
UINT32 AmbaINT_GlobalMaskEnable(void)
{
    return INT_ERR_NONE;
}

/**
 *  AmbaINT_GlobalMaskDisable - Enable interrupts from GIC to processors
 *  @return error code
 */
UINT32 AmbaINT_GlobalMaskDisable(void)
{
    return INT_ERR_NONE;
}

/**
 *  AmbaINT_Config - register an interrupt handler for the specified IRQ
 *  @param[in] Irq interrupt request number
 *  @param[in] pIntConfig pointer to the configurations of this interrupt
 *  @param[in] IntFunc function to be called when interrupt occurs. NULL is only valid for edge-triggered interrupt.
 *  @param[in] IntFuncArg Optional argument attached to the interrupt handler
 *  @return error code
 */
UINT32 AmbaINT_Config(UINT32 Irq, const AMBA_INT_CONFIG_s *pIntConfig, AMBA_INT_ISR_f IntFunc, UINT32 IntFuncArg)
{
    UINT32 RetVal = INT_ERR_NONE;
    AMBA_INT_SETTING_s IntSetting = {0U};
    pthread_t Task;

    (void)pIntConfig;

    IntSetting.Id = Irq;
    IntSetting.IntFunc = IntFunc;
    IntSetting.IntFuncArg = IntFuncArg;

    pthread_create(&Task, NULL, INT_ISR, &IntSetting);

    pthread_setschedprio(Task, AMBA_INT_PRIORITY);

    return RetVal;
}

/**
 *  AmbaINT_Enable - Enable Interrupt
 *  @param[in] Irq Interrupt ID
 *  @return error code
 */
UINT32 AmbaINT_Enable(UINT32 Irq)
{
    UINT32 RetVal = INT_ERR_NONE;

    InterruptUnmask(Irq, -1);

    return RetVal;
}

/**
 *  AmbaINT_Disable - Disable Interrupt
 *  @param[in] Irq Interrupt ID
 *  @return error code
 */
UINT32 AmbaINT_Disable(UINT32 Irq)
{
    return InterruptMask(Irq, -1);
}

/**
 *  AmbaINT_SendSGI - send software generated interrupt
 *  @return error code
 */
UINT32 AmbaINT_SendSGI(UINT32 Irq, UINT32 SgiType, UINT32 CpuTargets)
{
    (void)Irq;
    (void)SgiType;
    (void)CpuTargets;

    return INT_ERR_NONE;
}

/**
 *  AmbaINT_GetInfo - Get interrupt Info
 *  @return error code
 */
UINT32 AmbaINT_GetInfo(UINT32 Irq, UINT32 * pGlobalIntEnable, AMBA_INT_INFO_s * pIrqInfo)
{
    (void)Irq;
    (void)pGlobalIntEnable;
    (void)pIrqInfo;

    return INT_ERR_NONE;
}
