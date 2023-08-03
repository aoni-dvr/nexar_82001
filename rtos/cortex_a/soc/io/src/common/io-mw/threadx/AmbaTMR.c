/**
 *  @file AmbaTMR.c
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
 *  @details Interval Timer Device Driver
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"
#include "AmbaTMR.h"
#include "AmbaRTSL_TMR.h"

/* Reserved timer 19 for rtos schdeuler ticks, timer 18 for rtos profiling */
#define AMBA_TMR_AVAILS     0x3ffffU

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#define AMBA_NUM_AVAIL_TIMER AMBA_R52_TIMER_OFFSET
#else
#define AMBA_NUM_AVAIL_TIMER AMBA_NUM_TIMER
#endif

static AMBA_KAL_MUTEX_t AmbaTmrMutex;
static AMBA_KAL_EVENT_FLAG_t AmbaTmrEventFlag;

/**
 *  AmbaTMR_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaTMR_DrvEntry(void)
{
    static char AmbaTmrMutexName[16] = "AmbaTmrMutex";
    static char AmbaTmrEventFlagName[20] = "AmbaTmrEventFlag";
    UINT32 RetVal = TMR_ERR_NONE;

    /* Create Mutex */
    if (AmbaKAL_MutexCreate(&AmbaTmrMutex, AmbaTmrMutexName) != KAL_ERR_NONE) {
        RetVal = TMR_ERR_UNEXPECTED;
    } else {
        /* Create Event Flags */
        if (AmbaKAL_EventFlagCreate(&AmbaTmrEventFlag, AmbaTmrEventFlagName) != KAL_ERR_NONE) {
            RetVal = TMR_ERR_UNEXPECTED;
        } else {
            /* Signal the Event Flags of All Available Timers */
            if (AmbaKAL_EventFlagSet(&AmbaTmrEventFlag, AMBA_TMR_AVAILS) != KAL_ERR_NONE) {
                RetVal = TMR_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaTMR_FindAvail - Find an avaliable timer instance
 *  @param[out] pTimerID Timer ID
 *  @param[in] TimeOut A timeout value
 *  @return error code
 */
UINT32 AmbaTMR_FindAvail(UINT32 *pTimerID, UINT32 TimeOut)
{
    UINT32 KalRetVal, RetVal = TMR_ERR_NONE;
    UINT32 i, TimerAvailFlags;

    if (pTimerID == NULL) {
        RetVal = TMR_ERR_ARG;
    } else {
        KalRetVal = AmbaKAL_EventFlagGet(&AmbaTmrEventFlag, AMBA_TMR_AVAILS, 0x0U, 0x0U, &TimerAvailFlags, TimeOut);
        if (KalRetVal != KAL_ERR_NONE) {
            if (KalRetVal == KAL_ERR_TIMEOUT) {
                RetVal = TMR_ERR_TMO;
            } else {
                RetVal = TMR_ERR_UNEXPECTED;
            }
        }

        if (RetVal == TMR_ERR_NONE) {
            /* find one available */
            for (i = 0U; i < AMBA_NUM_AVAIL_TIMER; i++) {
                if ((TimerAvailFlags & 0x1U) != 0x0U) {
                    *pTimerID = i;
                    break;
                }

                TimerAvailFlags >>= 1U;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaTMR_Acquire - Acquire a timer
 *  @param[in] TimerID Timer ID
 *  @param[in] TimeOut A timeout value
 *  @return error code
 */
UINT32 AmbaTMR_Acquire(UINT32 TimerID, UINT32 TimeOut)
{
    UINT32 KalRetVal, RetVal = TMR_ERR_NONE;
    UINT32 ActualFlags;

    if (TimerID >= AMBA_NUM_AVAIL_TIMER) {
        RetVal = TMR_ERR_ARG;
    } else {
        /* acquire a timer by clear an event flag related to it */
        KalRetVal = AmbaKAL_EventFlagGet(&AmbaTmrEventFlag, ((UINT32)1U << TimerID), 0x0U, 0x1U, &ActualFlags, TimeOut);
        if (KalRetVal != KAL_ERR_NONE) {
            if (KalRetVal == KAL_ERR_TIMEOUT) {
                RetVal = TMR_ERR_BUSY;  /* It might be a race condition issue. The user needs to review the calling sequence. */
            } else {
                RetVal = TMR_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaTMR_Release - Release a timer
 *  @param[in] TimerID Timer ID
 *  @return error code
 */
UINT32 AmbaTMR_Release(UINT32 TimerID)
{
    UINT32 RetVal = TMR_ERR_NONE;
    AMBA_TMR_INFO_s TimerInfo;

    if (TimerID >= AMBA_NUM_AVAIL_TIMER) {
        RetVal = TMR_ERR_ARG;
    } else if (AmbaRTSL_TmrGetInfo(TimerID, &TimerInfo) != TMR_ERR_NONE) {
        RetVal = TMR_ERR_ARG;
    } else if (TimerInfo.State != 0U) {
        RetVal = TMR_ERR_BUSY;
    } else {
        /* release a timer by set an event flag related to it */
        if (AmbaKAL_EventFlagSet(&AmbaTmrEventFlag, ((UINT32)1U << TimerID)) != KAL_ERR_NONE) {
            RetVal = TMR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaTMR_HookTimeOutHandler - Hook timer expiration function
 *  @param[in] TimerID Timer ID
 *  @param[in] ExpireFunc Timer expiration function
 *  @param[in] ExpireFuncArg Optional argument attached to timer expiration function
 *  @return error code
 */
UINT32 AmbaTMR_HookTimeOutHandler(UINT32 TimerID, AMBA_TMR_ISR_f ExpireFunc, UINT32 ExpireFuncArg)
{
    UINT32 RetVal = TMR_ERR_NONE;
    AMBA_TMR_INFO_s TimerInfo;

    if (TimerID >= AMBA_NUM_AVAIL_TIMER) {
        RetVal = TMR_ERR_ARG;
    } else if (AmbaRTSL_TmrGetInfo(TimerID, &TimerInfo) != TMR_ERR_NONE) {
        RetVal = TMR_ERR_ARG;
    } else if (TimerInfo.State != 0U) {
        RetVal = TMR_ERR_BUSY;
    } else {
        if (AmbaKAL_MutexTake(&AmbaTmrMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = TMR_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_TmrHookTimeOutHandler(TimerID, ExpireFunc, ExpireFuncArg);

            if (AmbaKAL_MutexGive(&AmbaTmrMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = TMR_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaTMR_Config - Configure a timer
 *  @param[in] TimerID Timer ID
 *  @param[in] TimerFreq Desired timer frequency
 *  @param[in] NumPeriodicTick Interval between two timer expiration (in number of ticks)
 *  @return error code
 */
UINT32 AmbaTMR_Config(UINT32 TimerID, UINT32 TimerFreq, UINT32 NumPeriodicTick)
{
    UINT32 RetVal = TMR_ERR_NONE;
    AMBA_TMR_INFO_s TimerInfo;

    if ((TimerID >= AMBA_NUM_AVAIL_TIMER) || (TimerFreq == 0U)) {
        RetVal = TMR_ERR_ARG;
    } else if (AmbaRTSL_TmrGetInfo(TimerID, &TimerInfo) != TMR_ERR_NONE) {
        RetVal = TMR_ERR_ARG;
    } else if (TimerInfo.State != 0U) {
        RetVal = TMR_ERR_BUSY;
    } else {
        if (AmbaKAL_MutexTake(&AmbaTmrMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = TMR_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_TmrConfig(TimerID, TimerFreq, NumPeriodicTick);

            if (AmbaKAL_MutexGive(&AmbaTmrMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = TMR_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaTMR_Start - Start a timer
 *  @param[in] TimerID Timer ID
 *  @param[in] NumRemainTick Interval before the next timer expiration (in number of ticks)
 *  @return error code
 */
UINT32 AmbaTMR_Start(UINT32 TimerID, UINT32 NumRemainTick)
{
    UINT32 RetVal = TMR_ERR_NONE;

    if (TimerID >= AMBA_NUM_AVAIL_TIMER) {
        RetVal = TMR_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaTmrMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = TMR_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_TmrStart(TimerID, NumRemainTick);

            if (AmbaKAL_MutexGive(&AmbaTmrMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = TMR_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaTMR_Stop - Stop a timer
 *  @param[in] TimerID Timer ID
 *  @return error code
 */
UINT32 AmbaTMR_Stop(UINT32 TimerID)
{
    UINT32 RetVal = TMR_ERR_NONE;

    if (TimerID >= AMBA_NUM_AVAIL_TIMER) {
        RetVal = TMR_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaTmrMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = TMR_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_TmrStop(TimerID);

            if (AmbaKAL_MutexGive(&AmbaTmrMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = TMR_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaTMR_WaitTimeExpired - Wait until the next timer expiration
 *  @param[in] TimerID Timer ID
 *  @param[in] TimeOut A timeout value
 *  @return error code
 */
UINT32 AmbaTMR_WaitTimeExpired(UINT32 TimerID, UINT32 TimeOut)
{
    UINT32 KalRetVal, RetVal = TMR_ERR_NONE;
    AMBA_TMR_INFO_s TimerInfo;
    UINT32 ActualFlags;

    if (TimerID >= AMBA_NUM_AVAIL_TIMER) {
        RetVal = TMR_ERR_ARG;
    } else if (AmbaRTSL_TmrGetInfo(TimerID, &TimerInfo) != TMR_ERR_NONE) {
        RetVal = TMR_ERR_ARG;
    } else if ((TimerInfo.State == 0U) || (TimerInfo.PeriodicInterval != 0U)) {
        RetVal = TMR_ERR_NA;    /* Not applicable for cyclic timer */
    } else {
        KalRetVal = AmbaKAL_EventFlagGet(&AmbaTmrEventFlag, ((UINT32)1U << TimerID), 0x0U, 0x0U, &ActualFlags, TimeOut);
        if (KalRetVal != KAL_ERR_NONE) {
            if (KalRetVal == KAL_ERR_TIMEOUT) {
                RetVal = TMR_ERR_TMO;
            } else {
                RetVal = TMR_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaTMR_ShowTickCount - Show the current tick count value
 *  @param[in] TimerID Timer ID
 *  @param[out] pTimeLeft Number of the remaining ticks before expiration
 *  @return error code
 */
UINT32 AmbaTMR_ShowTickCount(UINT32 TimerID, UINT32 * pTimeLeft)
{
    UINT32 RetVal = TMR_ERR_NONE;

    if ((TimerID >= AMBA_NUM_AVAIL_TIMER) || (pTimeLeft == NULL)) {
        RetVal = TMR_ERR_ARG;
    } else {
        RetVal = AmbaRTSL_TmrShowTickCount(TimerID, pTimeLeft);
    }

    return RetVal;
}

/**
 *  AmbaTMR_GetInfo - Get timer information
 *  @param[in] TimerID Timer ID
 *  @param[out] pTimerInfo Timer information
 *  @return error code
 */
UINT32 AmbaTMR_GetInfo(UINT32 TimerID, AMBA_TMR_INFO_s * pTimerInfo)
{
    UINT32 RetVal = TMR_ERR_NONE;

    if ((TimerID >= AMBA_NUM_AVAIL_TIMER) || (pTimerInfo == NULL)) {
        RetVal = TMR_ERR_ARG;
    } else {
        RetVal = AmbaRTSL_TmrGetInfo(TimerID, pTimerInfo);
    }

    return RetVal;
}
