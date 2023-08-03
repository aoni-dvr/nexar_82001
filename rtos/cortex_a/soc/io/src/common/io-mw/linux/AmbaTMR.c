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

#define _GNU_SOURCE
#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"
#include "AmbaTMR.h"
//#include "AmbaRTSL_TMR.h"
#include <signal.h>

/*
 * NOTE: total_us = tick * (1000000 / freq)
 */

// enable siganl version TMR; otherwise sleep version TMR
#define TIMER_SIGNAL_EXPERIMENTAL
// #define TIMER_SIGEV_THREAD

/* Reserved timer 19 for rtos schdeuler ticks, timer 18 for rtos profiling */
#define AMBA_TMR_AVAILS     0x3ffffU
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#define AMBA_NUM_AVAIL_TIMER AMBA_R52_TIMER_OFFSET
#else
#define AMBA_NUM_AVAIL_TIMER AMBA_NUM_TIMER
#endif

static AMBA_KAL_MUTEX_t AmbaTmrMutex;
static AMBA_KAL_EVENT_FLAG_t AmbaTmrEventFlag;

#if defined(TIMER_SIGNAL_EXPERIMENTAL)

static timer_t g_timer_id[AMBA_NUM_AVAIL_TIMER];
static UINT32 g_timer_freq[AMBA_NUM_AVAIL_TIMER];
static UINT32 g_timer_period_tick[AMBA_NUM_AVAIL_TIMER];
static struct itimerspec g_its[AMBA_NUM_AVAIL_TIMER];

#if defined(TIMER_SIGEV_THREAD)
static void TimerEntry_Sigev(__sigval_t sig)
{
    (void) sig;
    printf("[DBG] ExpireFunc\n");
}
#endif

static UINT32 TimerCreate_Sigev(UINT32 TimerID)
{
    UINT32 RetVal = TMR_ERR_NONE;
    struct sigevent sev;

#if !defined(TIMER_SIGEV_THREAD)
    sev.sigev_notify = SIGEV_NONE;
#else
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = TimerEntry_Sigev;
#endif
    /* detached thread, can't be joined */
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = &g_timer_id[TimerID];
    timer_create(CLOCK_REALTIME, &sev, &g_timer_id[TimerID]);

    return RetVal;
}

static UINT32 TimerDelete_Sigev(UINT32 TimerID)
{
    UINT32 RetVal = TMR_ERR_NONE;
    timer_delete(g_timer_id[TimerID]);
    return RetVal;
}

static UINT32 TimerConfig_Sigev(UINT32 TimerID, UINT32 TimerFreq, UINT32 NumPeriodicTick)
{
    UINT32 RetVal = TMR_ERR_NONE;
    g_timer_freq[TimerID] = TimerFreq;
    g_timer_period_tick[TimerID] = NumPeriodicTick;
    return RetVal;
}

static UINT32 TimerSetStart_Sigev(UINT32 TimerID, UINT32 NumRemainTick)
{
    UINT32 RetVal = TMR_ERR_NONE;
    ULONG total_us;

    // total_us = tick * (1000000 / freq)
    total_us = 1000000 / g_timer_freq[TimerID] * g_timer_period_tick[TimerID];
    g_its[TimerID].it_interval.tv_sec =  g_timer_period_tick[TimerID] / g_timer_freq[TimerID];
    g_its[TimerID].it_interval.tv_nsec = total_us * 1000 % 1000000000;

    total_us = 1000000 / g_timer_freq[TimerID] * NumRemainTick;
    g_its[TimerID].it_value.tv_sec = NumRemainTick / g_timer_freq[TimerID];
    g_its[TimerID].it_value.tv_nsec = total_us * 1000 % 1000000000;
    // printf("[DBG] %lu.%09lu\n",  g_its[TimerID].it_value.tv_sec,  g_its[TimerID].it_value.tv_nsec);
    // printf("[DBG] %lu.%09lu\n",  g_its[TimerID].it_interval.tv_sec,  g_its[TimerID].it_interval.tv_nsec);

    // set and start the timer
    timer_settime(g_timer_id[TimerID], 0, &g_its[TimerID], NULL);
    return RetVal;
}

#else

typedef struct {
    UINT32 ExpireFuncArg;
    UINT32 StartInterval_us;
    UINT32 PeriodicInterval_us;
    UINT32 StartTotal_tick;
    UINT32 PeriodicTotal_tick;
    UINT32 TimerFreq;
    AMBA_KAL_MUTEX_t *pMutex;
    AMBA_TMR_ISR_f ExpireFunc;
} AMBA_TIMER_SETTING_s;

static AMBA_KAL_MUTEX_t g_mutex[AMBA_NUM_AVAIL_TIMER];
static AMBA_KAL_TIMER_t g_pTimer[AMBA_NUM_AVAIL_TIMER];
static AMBA_TIMER_SETTING_s g_TimerSetting[AMBA_NUM_AVAIL_TIMER];
static UINT32 g_ExpireTickCnt[AMBA_NUM_AVAIL_TIMER];
static struct timespec g_ReqStart[AMBA_NUM_AVAIL_TIMER];
static struct timespec g_ReqPeriodic[AMBA_NUM_AVAIL_TIMER];


static void test_expire_func(UINT32 arg1, UINT32 arg2)
{
    (void) arg1;
    (void) arg2;
    // printf("[DBG] %u:ExpireFunc\n", arg1);
}

static void compute_sub_interval_time(struct timespec *Req, const UINT32 Interval_usec, const UINT32 Total_tick)
{
    /*
     * Original Formula:
     * sub_interval_nsec = (1000 / Total_tick) * Interval_usec;
     * Req->tv_sec = sub_interval_nsec / 1000000000;
     * Req->tv_nsec = sub_interval_nsec % 1000000000;
     */
    Req->tv_sec = Interval_usec / Total_tick / 1000000;
    Req->tv_nsec = Interval_usec / Total_tick * 1000 % 1000000000;
}

static void *TimerEntry_Sleep(void* argv)
{
    UINT32 Idx = *(UINT32 *)argv;
    struct timespec Rem;

    /* wait for starting */
    if (pthread_mutex_trylock(g_TimerSetting[Idx].pMutex) == 0) {
        compute_sub_interval_time(&g_ReqStart[Idx], g_TimerSetting[Idx].StartInterval_us, g_TimerSetting[Idx].StartTotal_tick);
        pthread_mutex_unlock(g_TimerSetting[Idx].pMutex);
    }
    /* first timer */
    while (g_ExpireTickCnt[Idx] < g_TimerSetting[Idx].StartTotal_tick) {
        nanosleep(&g_ReqStart[Idx], &Rem);
        g_ExpireTickCnt[Idx]++;
    }
    if (pthread_mutex_trylock(g_TimerSetting[Idx].pMutex) == 0) {
        g_TimerSetting[Idx].ExpireFunc(g_TimerSetting[Idx].ExpireFuncArg, 0U);
        pthread_mutex_unlock(g_TimerSetting[Idx].pMutex);
    }

    /* interval timer */
    while (1) {
        if (0x0U != g_TimerSetting[Idx].PeriodicInterval_us) {
            g_ExpireTickCnt[Idx] = 0U;

            while (g_ExpireTickCnt[Idx] < g_TimerSetting[Idx].PeriodicTotal_tick) {
                nanosleep(&g_ReqPeriodic[Idx], &Rem);
                g_ExpireTickCnt[Idx]++;
            }

            if (pthread_mutex_trylock(g_TimerSetting[Idx].pMutex) == 0) {
                g_TimerSetting[Idx].ExpireFunc(g_TimerSetting[Idx].ExpireFuncArg, 0U);
                pthread_mutex_unlock(g_TimerSetting[Idx].pMutex);
            }

        } else {
            /*do not wait for AmbaKAL_TimerDelete*/
            pthread_exit(NULL);
        }
    }
}

static UINT32 TimerCreate_Sleep(AMBA_KAL_TIMER_t *pTimer, char * pTimerName, AMBA_TMR_ISR_f ExpireFunc,
                           UINT32 ExpireFuncArg, UINT32 StartInterval_us, UINT32 PeriodicInterval_us, UINT32 Idx)
{
    int Ret = KAL_ERR_NONE;

    /* create mutex */
    pthread_mutex_init(&pTimer->mutex, NULL);

    /* set TimerSetting */
    g_TimerSetting[Idx].pMutex = &pTimer->mutex;
    g_TimerSetting[Idx].ExpireFunc = ExpireFunc;
    g_TimerSetting[Idx].ExpireFuncArg = ExpireFuncArg;
    g_TimerSetting[Idx].PeriodicInterval_us = PeriodicInterval_us;
    g_TimerSetting[Idx].StartInterval_us = StartInterval_us;
    g_TimerSetting[Idx].TimerFreq = 1000000; //default 1MHz
    g_TimerSetting[Idx].PeriodicTotal_tick = PeriodicInterval_us * g_TimerSetting[Idx].TimerFreq / 1000000;
    g_TimerSetting[Idx].StartTotal_tick = StartInterval_us * g_TimerSetting[Idx].TimerFreq / 1000000;

    /* init g_ExpireTickCnt */
    g_ExpireTickCnt[Idx] = 0U;

    /* do not run ExpireFunc */
    pthread_mutex_lock(g_TimerSetting[Idx].pMutex);

    /* create pthread */
    Ret = pthread_create(&pTimer->th, NULL, TimerEntry_Sleep, &Idx);

    /* wait 1ms for KAL_TimerEntry to copy TimerSetting */
    usleep(1000);

    if (Ret == OK) {
        if (pTimerName != NULL) {
            pthread_setname_np(pTimer->th, (const char *)pTimerName);
        }
    } else {
        printf("AmbaKAL_TimerCreate failed, error = %d\n", Ret);
        Ret = KAL_ERR_POSIX;
    }
    return Ret;
}

static UINT32 TimerDelete_Sleep(AMBA_KAL_TIMER_t *pTimer)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pTimer == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        /* ignore err, one-shot timer already exit */
        (void) pthread_cancel(pTimer->th);
    }

    return RetVal;
}

static UINT32 TimerConfig_Sleep(UINT32 TimerID, UINT32 TimerFreq, UINT32 NumPeriodicTick)
{
    UINT32 RetVal = TMR_ERR_NONE;
    g_TimerSetting[TimerID].TimerFreq = TimerFreq;
    g_TimerSetting[TimerID].PeriodicTotal_tick = NumPeriodicTick;
    g_TimerSetting[TimerID].PeriodicInterval_us = NumPeriodicTick * (1000000 / TimerFreq);
    compute_sub_interval_time(&g_ReqPeriodic[TimerID], g_TimerSetting[TimerID].PeriodicInterval_us, g_TimerSetting[TimerID].PeriodicTotal_tick);
    return RetVal;
}
#endif //TIMER_SIGNAL_EXPERIMENTAL

/**
 *  AmbaTMR_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaTMR_DrvEntry(void)
{
    UINT32 RetVal = TMR_ERR_NONE;
    static char AmbaTmrMutexName[16] = "AmbaTmrMutex";
    static char AmbaTmrEventFlagName[20] = "AmbaTmrEventFlag";
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
 *  AmbaTMR_FindAvail - Find an available timer instance
 *  @param[out] pTimerID Timer ID
 *  @param[in] TimeOut A timeout value
 *  @return error code
 */
UINT32 AmbaTMR_FindAvail(UINT32 *pTimerID, UINT32 TimeOut)
{
    UINT32 RetVal = TMR_ERR_NONE;

    UINT32 KalRetVal;
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
    UINT32 RetVal = TMR_ERR_NONE;

    UINT32 KalRetVal;
    UINT32 ActualFlags;
    if (TimerID >= AMBA_NUM_AVAIL_TIMER) {
        RetVal = TMR_ERR_ARG;
    } else {
        /* create a timer */
#if defined(TIMER_SIGNAL_EXPERIMENTAL)
        TimerCreate_Sigev(TimerID);
#else
        g_pTimer[TimerID].mutex = g_mutex[TimerID];
        RetVal = TimerCreate_Sleep(&g_pTimer[TimerID], "TEST_TIMER", test_expire_func, TimerID,
                                     1000000U, 1000000U, TimerID);
#endif
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
    if (TimerID >= AMBA_NUM_AVAIL_TIMER) {
        RetVal = TMR_ERR_ARG;
    } else {
        /* delete the timer */
#if defined(TIMER_SIGNAL_EXPERIMENTAL)
        TimerDelete_Sigev(TimerID);
#else
        TimerDelete_Sleep(&g_pTimer[TimerID]);
#endif

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

    if (TimerID >= AMBA_NUM_AVAIL_TIMER) {
        RetVal = TMR_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaTmrMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = TMR_ERR_MUTEX;
        } else {
#if defined(TIMER_SIGNAL_EXPERIMENTAL)
            (void)  ExpireFunc;
            (void)  ExpireFuncArg;
#else
            g_TimerSetting[TimerID].ExpireFunc = ExpireFunc;
            g_TimerSetting[TimerID].ExpireFuncArg = ExpireFuncArg;
#endif
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

     if ((TimerID >= AMBA_NUM_AVAIL_TIMER) || (TimerFreq == 0U)) {
        RetVal = TMR_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaTmrMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = TMR_ERR_MUTEX;
        } else {
#if defined(TIMER_SIGNAL_EXPERIMENTAL)
            RetVal = TimerConfig_Sigev(TimerID, TimerFreq, NumPeriodicTick);
#else
            RetVal = TimerConfig_Sleep(TimerID, TimerFreq, NumPeriodicTick);
#endif
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

#if defined(TIMER_SIGNAL_EXPERIMENTAL)
            TimerSetStart_Sigev(TimerID, NumRemainTick);
#else
            if (NumRemainTick > 0U) {
                g_TimerSetting[TimerID].StartTotal_tick = NumRemainTick;
                g_TimerSetting[TimerID].StartInterval_us = NumRemainTick * (1000000 / g_TimerSetting[TimerID].TimerFreq);
            }
            /* run ExpireFunc */
            (void)pthread_mutex_unlock(&g_pTimer[TimerID].mutex);
#endif

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
            /* do not run ExpireFunc */

#if defined(TIMER_SIGNAL_EXPERIMENTAL)
            // timer_settime(2): Arm (start) or disarm (stop) a timer.
            //    If new_value->it_value specifies a zero value
            //    (i.e., both subfields are zero), then the timer is disarmed.
            g_its[TimerID].it_value.tv_sec = 0;
            g_its[TimerID].it_value.tv_nsec = 0;
            timer_settime(g_timer_id[TimerID], 0, &g_its[TimerID], NULL);
#else
            RetVal = pthread_mutex_lock(&g_pTimer[TimerID].mutex);
#endif

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
    UINT32 RetVal = TMR_ERR_NONE;
    (void) TimerID;
    (void) TimeOut;
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
#if defined(TIMER_SIGNAL_EXPERIMENTAL)
        struct itimerspec its;
        UINT32 multiplier = g_timer_freq[TimerID];
        UINT32 divider = 1000000000U;

        timer_gettime(g_timer_id[TimerID], &its);

        // Formula: total_us = 1000000 / g_timer_freq[TimerID] * g_timer_period_tick[TimerID];
        if ((its.it_value.tv_sec) != 0 || (its.it_value.tv_nsec != 0)) {
            /*
             * Original Formula:
               *pTimeLeft = its.it_value.tv_sec * g_timer_freq[TimerID] + its.it_value.tv_nsec / 1000000000 * g_timer_freq[TimerID];
             *
             * For avoiding losting time accuracy (us), only divide 10 each loop instead of dividing 1000000000 at a time.
             */
            while (multiplier >= 10U && divider >= 10U) {
                multiplier /= 10U;
                divider /= 10U;
            }
            *pTimeLeft = its.it_value.tv_sec * g_timer_freq[TimerID] + its.it_value.tv_nsec * multiplier / divider;
        } else {
            *pTimeLeft = 0U;
            RetVal = TMR_ERR_UNEXPECTED;
        }
#else
        *pTimeLeft = g_ExpireTickCnt[TimerID];
#endif
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

    (void) TimerID;
    (void) pTimerInfo;

    return RetVal;
}
