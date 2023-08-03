/**
 *  @file AmbaWDT.c
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
 *  @details WatchDog Timer APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"
#include "AmbaWDT.h"
#include "AmbaRTSL_WDT.h"

static AMBA_KAL_MUTEX_t AmbaWdtMutex;

/**
 *  AmbaWDT_DrvEntry - WDT device driver initializations
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaWDT_DrvEntry(void)
{
    UINT32 RetVal = WDT_ERR_NONE;

    /* Create Mutexes */
    if (AmbaKAL_MutexCreate(&AmbaWdtMutex, NULL) != KAL_ERR_NONE) {
        RetVal = WDT_ERR_UNEXPECTED;
    }
    AmbaRTSL_Wdt_Init();

    return RetVal;
}

/**
 *  AmbaWDT_HookTimeOutHandler - The function registers an interrupt handler on watchdog timer expiry.
 *  @param[in] ExpireFunc WDT timeout handler
 *  @param[in] ExpireFuncArg Optional argument attached to WDT timeout handler
 *  @return error code
 */
UINT32 AmbaWDT_HookTimeOutHandler(AMBA_WDT_ISR_f ExpireFunc, UINT32 ExpireFuncArg)
{
    UINT32 RetVal = WDT_ERR_NONE;

    if ((ExpireFunc == NULL) && (ExpireFuncArg != 0U)) {
        RetVal = WDT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaWdtMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = WDT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_WdtHookTimeOutHandler(ExpireFunc, ExpireFuncArg);

            if (AmbaKAL_MutexGive(&AmbaWdtMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = WDT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaWDT_Start - Start the WDT.
 *  @param[in] CountDown WDT timeout value
 *  @param[in] ResetIrqPulseWidth Pulse width on WDT timeout
 *  @return error code
 */
UINT32 AmbaWDT_Start(UINT32 CountDown, UINT32 ResetIrqPulseWidth)
{
    UINT32 RetVal = WDT_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaWdtMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = WDT_ERR_MUTEX;
    } else {
        RetVal = AmbaRTSL_WdtStart(CountDown, ResetIrqPulseWidth);

        if (AmbaKAL_MutexGive(&AmbaWdtMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = WDT_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaWDT_Feed - Feed the WDT.
 *  @return error code
 */
UINT32 AmbaWDT_Feed(void)
{
    UINT32 RetVal = WDT_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaWdtMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = WDT_ERR_MUTEX;
    } else {
        RetVal = AmbaRTSL_WdtFeed();

        if (AmbaKAL_MutexGive(&AmbaWdtMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = WDT_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaWDT_Stop - Stop the WDT.
 *  @return error code
 */
UINT32 AmbaWDT_Stop(void)
{
    UINT32 RetVal = WDT_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaWdtMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = WDT_ERR_MUTEX;
    } else {
        RetVal = AmbaRTSL_WdtStop();

        if (AmbaKAL_MutexGive(&AmbaWdtMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = WDT_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaWDT_GetInfo - The function returns the WDT configuration and status.
 *  @param[out] pWdtInfo WDT configuration and status
 *  @return error code
 */
UINT32 AmbaWDT_GetInfo(AMBA_WDT_INFO_s * pWdtInfo)
{
    UINT32 RetVal = WDT_ERR_NONE;

    if (pWdtInfo == NULL) {
        RetVal = WDT_ERR_ARG;
    } else {
        pWdtInfo->TimerExpired = AmbaRTSL_WdtGetTimeOutStatus();
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        pWdtInfo->ExpireAction = AmbaRTSL_WdtGetTimeOutAction(0U);
#else
        pWdtInfo->ExpireAction = AmbaRTSL_WdtGetTimeOutAction();
#endif
    }

    return RetVal;
}

/**
 *  AmbaWDT_ClearStatus - Clear WDT status.
 *  @return error code
 */
UINT32 AmbaWDT_ClearStatus(void)
{
    AmbaRTSL_WdtClearTimeOutStatus();

    return WDT_ERR_NONE;
}

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
/**
 *  AmbaWDT_HookHandler - The function registers an interrupt handler on watchdog timer expiry.
 *  @param[in] ChId WDT channel id
 *  @param[in] ExpireFunc WDT timeout handler
 *  @param[in] ExpireFuncArg Optional argument attached to WDT timeout handler
 *  @return error code
 */
UINT32 AmbaWDT_HookHandler(UINT32 ChId, AMBA_WDT_ISR_f ExpireFunc, UINT32 ExpireFuncArg)
{
    UINT32 RetVal = WDT_ERR_NONE;

    if ((ExpireFunc == NULL) && (ExpireFuncArg != 0U)) {
        RetVal = WDT_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaWdtMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = WDT_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_WdtHookHandler(ChId, ExpireFunc, ExpireFuncArg);

            if (AmbaKAL_MutexGive(&AmbaWdtMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = WDT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaWDT_StartByChId - Start the specified WDT.
 *  @param[in] ChId WDT channel id
 *  @param[in] CountDown WDT timeout value
 *  @param[in] ResetIrqPulseWidth Pulse width on WDT timeout
 *  @return error code
 */
UINT32 AmbaWDT_StartByChId(UINT32 ChId, UINT32 CountDown, UINT32 ResetIrqPulseWidth)
{
    UINT32 RetVal = WDT_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaWdtMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = WDT_ERR_MUTEX;
    } else {
        RetVal = AmbaRTSL_WdtStartByChId(ChId, CountDown, ResetIrqPulseWidth);

        if (AmbaKAL_MutexGive(&AmbaWdtMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = WDT_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaWDT_FeedByChId - Feed the specified WDT.
 *  @param[in] ChId WDT channel id
 *  @return error code
 */
UINT32 AmbaWDT_FeedByChId(UINT32 ChId)
{
    UINT32 RetVal = WDT_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaWdtMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = WDT_ERR_MUTEX;
    } else {
        RetVal = AmbaRTSL_WdtFeedByChId(ChId);

        if (AmbaKAL_MutexGive(&AmbaWdtMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = WDT_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaWDT_StopByChId - Stop the specified WDT.
 *  @param[in] ChId WDT channel id
 *  @return error code
 */
UINT32 AmbaWDT_StopByChId(UINT32 ChId)
{
    UINT32 RetVal = WDT_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaWdtMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = WDT_ERR_MUTEX;
    } else {
        RetVal = AmbaRTSL_WdtStopByChId(ChId);

        if (AmbaKAL_MutexGive(&AmbaWdtMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = WDT_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaWDT_GetInfoByChId - The function returns the specified WDT configuration and status.
 *  @param[in] ChId WDT channel id
 *  @param[out] pWdtInfo WDT configuration and status
 *  @return error code
 */
UINT32 AmbaWDT_GetInfoByChId(UINT32 ChId, AMBA_WDT_INFO_s * pWdtInfo)
{
    UINT32 RetVal = WDT_ERR_NONE;

    if (pWdtInfo == NULL) {
        RetVal = WDT_ERR_ARG;
    } else {
        pWdtInfo->TimerExpired = AmbaRTSL_WdtGetTimeOutStatus();
        pWdtInfo->ExpireAction = AmbaRTSL_WdtGetTimeOutAction(ChId);
    }

    return RetVal;
}

UINT32 AmbaWDT_SetPattern(void)
{
    UINT32 RetVal;

    RetVal = AmbaRTSL_WdtSetPattern();

    return RetVal;
}
#endif
