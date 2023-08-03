/**
 *  @file AmbaINT.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"
#include "AmbaINT.h"
#include "AmbaRTSL_GIC.h"

static AMBA_KAL_MUTEX_t AmbaIntMutex;

/**
 *  AmbaINT_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaINT_DrvEntry(void)
{
    static char AmbaIntMutexName[16] = "AmbaIntMutex";
    UINT32 RetVal = INT_ERR_NONE;

    /* Create Mutex */
    if (AmbaKAL_MutexCreate(&AmbaIntMutex, AmbaIntMutexName) != KAL_ERR_NONE) {
        RetVal = INT_ERR_UNEXPECTED;
    }

    return RetVal;
}

/**
 *  AmbaINT_GlobalMaskEnable - Disable interrupts from GIC to processors
 *  @note It doesn't change the enable state of each interrupt
 *  @return error code
 */
UINT32 AmbaINT_GlobalMaskEnable(void)
{
    UINT32 RetVal = INT_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaIntMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = INT_ERR_MUTEX;
    } else {
        AmbaRTSL_GicIntGlobalDisable();

        if (AmbaKAL_MutexGive(&AmbaIntMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = INT_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaINT_GlobalMaskDisable - Enable interrupts from GIC to processors
 *  @return error code
 */
UINT32 AmbaINT_GlobalMaskDisable(void)
{
    UINT32 RetVal = INT_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaIntMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = INT_ERR_MUTEX;
    } else {
        AmbaRTSL_GicIntGlobalEnable();

        if (AmbaKAL_MutexGive(&AmbaIntMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = INT_ERR_UNEXPECTED;
        }
    }

    return RetVal;
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

    if ( (Irq >= (UINT32)AMBA_NUM_INTERRUPT) || (pIntConfig == NULL)) {
        RetVal = INT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaIntMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = INT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_GicIntConfig(Irq, pIntConfig, IntFunc, IntFuncArg);

            if (AmbaKAL_MutexGive(&AmbaIntMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = INT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaINT_Enable - Enable Interrupt
 *  @param[in] Irq Interrupt ID
 *  @return error code
 */
UINT32 AmbaINT_Enable(UINT32 Irq)
{
    return AmbaRTSL_GicIntEnable(Irq);
}

/**
 *  AmbaINT_Disable - Disable Interrupt
 *  @param[in] Irq Interrupt ID
 *  @return error code
 */
UINT32 AmbaINT_Disable(UINT32 Irq)
{
    return AmbaRTSL_GicIntDisable(Irq);
}

/**
 *  AmbaINT_SendSGI - send software generated interrupt
 *  @param[in] Irq SGI interrupt ID
 *  @param[in] SgiType CPU targets of SGI
 *  @param[in] CpuTargets CPU target list
 *  @return error code
 */
UINT32 AmbaINT_SendSGI(UINT32 Irq, UINT32 SgiType, UINT32 CpuTargets)
{
    return AmbaRTSL_GicSendSGI(Irq, INT_TYPE_FIQ, SgiType, CpuTargets);
}

/**
 *  AmbaINT_GetInfo - Get interrupt Info
 *  @param[in] Irq Interrupt ID
 *  @param[out] pGlobalIntEnable Interrupt state
 *  @param[out] pIrqInfo Interrupt info
 *  @return error code
 */
UINT32 AmbaINT_GetInfo(UINT32 Irq, UINT32 * pGlobalIntEnable, AMBA_INT_INFO_s * pIrqInfo)
{
    UINT32 RetVal = INT_ERR_NONE;

    if ((Irq >= (UINT32)AMBA_NUM_INTERRUPT) || (pIrqInfo == NULL)) {
        RetVal = INT_ERR_ARG;
    } else {
        RetVal = AmbaRTSL_GicGetIntInfo(Irq, pIrqInfo);

        if (pGlobalIntEnable != NULL) {
            *pGlobalIntEnable = AmbaRTSL_GicIntGetGlobalState();
        }
    }

    return RetVal;
}
