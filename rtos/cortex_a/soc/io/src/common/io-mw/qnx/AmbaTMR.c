/*
 * Copyright (c) 2020 Ambarella International LP
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
 */
#include "Generic.h"
#include "hw/timer.h"
#include <sys/resmgr.h>
#include <threads.h>

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"

#include "AmbaTMR.h"

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#define AMBA_NUM_AVAIL_TIMER AMBA_R52_TIMER_OFFSET
#else
#define AMBA_NUM_AVAIL_TIMER AMBA_NUM_TIMER
#endif

typedef struct {
    UINT32  ExpireCount;
    UINT32  UserIntFuncArg;
    AMBA_TMR_ISR_f  UserIntFunc;    /* The table of line ISRs */
} AMBA_TMR_CTRL_s;

typedef struct {
    UINT32  IntID;                  /* Interrupt ID */
    int ThreadID;
} AMBA_TMR_SYS_CTRL_s;

static AMBA_TMR_SYS_CTRL_s AmbaTmrSysCtrl[AMBA_NUM_TIMER] = {
    { .IntID = 54U, .ThreadID = -1U},
    { .IntID = 55U, .ThreadID = -1U},
    { .IntID = 56U, .ThreadID = -1U},
    { .IntID = 57U, .ThreadID = -1U},
    { .IntID = 58U, .ThreadID = -1U},
    { .IntID = 59U, .ThreadID = -1U},
    { .IntID = 60U, .ThreadID = -1U},
    { .IntID = 61U, .ThreadID = -1U},
    { .IntID = 62U, .ThreadID = -1U},
    { .IntID = 63U, .ThreadID = -1U},
    { .IntID = 64U, .ThreadID = -1U},
    { .IntID = 65U, .ThreadID = -1U},
    { .IntID = 66U, .ThreadID = -1U},
    { .IntID = 67U, .ThreadID = -1U},
    { .IntID = 68U, .ThreadID = -1U},
    { .IntID = 69U, .ThreadID = -1U},
    { .IntID = 70U, .ThreadID = -1U},
    { .IntID = 71U, .ThreadID = -1U},
    { .IntID = 72U, .ThreadID = -1U},
    { .IntID = 73U, .ThreadID = -1U},
};

static AMBA_TMR_CTRL_s AmbaTmrCtrl[AMBA_NUM_TIMER];

/**
 *  AmbaTMR_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaTMR_DrvEntry(void)
{
    UINT32 RetVal = TMR_ERR_NONE;

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
    UINT32 RetVal = TMR_ERR_NONE;
    amba_timer_t Timer = {0U};
    int fd, err;

    (void)TimeOut;
    if (pTimerID != NULL) {
        fd = open("/dev/amba_tmr", O_RDWR);
        if (fd == -1) {
            fprintf(stderr,"%s: Can't open /dev/amba_tmr manager.", __FUNCTION__);
        } else {
            err = devctl(fd, DCMD_TIMER_GETAVAIL, &Timer, sizeof(amba_timer_t), NULL);
            if (err) {
                fprintf(stderr,"%s: failed: %s", __FUNCTION__, strerror(errno));
            } else {
                *pTimerID = Timer.TimerId;
            }
            close(fd);
        }
    } else {
        RetVal = TMR_ERR_ARG;
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
    amba_timer_t Timer = {0U};
    int fd, err;

    (void)TimeOut;
    if (TimerID < AMBA_NUM_AVAIL_TIMER) {
        fd = open("/dev/amba_tmr", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't open /dev/amba_tmr manager.", __FUNCTION__);
        } else {
            Timer.TimerId = TimerID;
            err = devctl(fd, DCMD_TIMER_LOCK, &Timer, sizeof(amba_timer_t), NULL);
            if (err) {
                fprintf(stderr,"%s: failed: %s", __FUNCTION__, strerror(errno));
            }
            close(fd);

            if (Timer.TimerId != TimerID) {
                RetVal = TMR_ERR_BUSY;
            }
        }
    } else {
        RetVal = TMR_ERR_ARG;
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
    amba_timer_t Timer = {0U};
    int fd, err;

    if (TimerID < AMBA_NUM_AVAIL_TIMER) {
        fd = open("/dev/amba_tmr", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't open /dev/amba_tmr manager.", __FUNCTION__);
        } else {
            Timer.TimerId = TimerID;
            err = devctl(fd, DCMD_TIMER_RELEASE, &Timer, sizeof(amba_timer_t), NULL);
            if (err) {
                fprintf(stderr,"%s: failed: %s", __FUNCTION__, strerror(errno));
            }
            close(fd);
        }
    } else {
        RetVal = TMR_ERR_ARG;
    }

    return RetVal;
}

static void *TMR_ISR(void* argv)
{
    struct sigevent event;
    int iid;
    UINT32 TimerID = *(UINT32 *)argv;
    UINT32 IntID = AmbaTmrSysCtrl[TimerID].IntID;
    AMBA_TMR_CTRL_s *pTimer = &AmbaTmrCtrl[TimerID];

    pTimer->ExpireCount = 0;

    SIGEV_INTR_INIT(&event);
    iid = InterruptAttachEvent(IntID, &event, 0U);
    if (InterruptUnmask(IntID,iid) == 0) {
        while(1) {
            InterruptWait(0, NULL);

            if (TimerID < AMBA_NUM_TIMER) {
                pTimer->ExpireCount++;

                if (pTimer->UserIntFunc != NULL) {
                    pTimer->UserIntFunc(TimerID, pTimer->UserIntFuncArg);
                }
            } else {
                (void)IntID;
            }

            InterruptUnmask(IntID, iid);
        }
    }

    return NULL;
}

static UINT32 TmrHookTimeOutHandler(UINT64 TimerID, AMBA_TMR_ISR_f ExpireFunc, UINT32 ExpireFuncArg)
{
    UINT32 RetVal = TMR_ERR_NONE;
    AMBA_TMR_CTRL_s *pTimer = &AmbaTmrCtrl[TimerID];
    INT32 QnxRet = 0;

    if (TimerID >= AMBA_NUM_TIMER) {
        RetVal = TMR_ERR_ARG;
    } else {
        pTimer->UserIntFunc = ExpireFunc;
        pTimer->UserIntFuncArg = ExpireFuncArg;
        if (ExpireFunc != NULL) {
            QnxRet = pthread_create(&AmbaTmrSysCtrl[TimerID].ThreadID, NULL, TMR_ISR, &TimerID);
            if (QnxRet == 0) {
                pthread_setschedprio(AmbaTmrSysCtrl[TimerID].ThreadID, 128U);
            } else {
                printf("%s %d, pthread_create failed, QnxRet(%d)\n", __FUNCTION__, __LINE__, QnxRet);
            }
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

    if (TimerID < AMBA_NUM_AVAIL_TIMER) {
        TmrHookTimeOutHandler(TimerID, ExpireFunc, ExpireFuncArg);
    } else {
        RetVal = TMR_ERR_ARG;
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
    amba_timer_t Timer = {0U};
    timer_info_t TimerInfo = {0U};
    int fd, err;

    if (TimerID < AMBA_NUM_AVAIL_TIMER) {
        fd = open("/dev/amba_tmr", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't open /dev/amba_tmr.\n", __FUNCTION__);
        } else {
            TimerInfo.TimerId = TimerID;
            err = devctl(fd, DCMD_TIMER_GETINFO, &TimerInfo, sizeof(timer_info_t), NULL);
            if (err) {
                fprintf(stderr,"%s: failed: %s\n", __FUNCTION__, strerror(errno));
            } else {
                if (TimerInfo.State == 0U) {
                    Timer.TimerId = TimerID;
                    Timer.TimerFreq = TimerFreq;
                    Timer.NumPeriodicTick = NumPeriodicTick;
                    err = devctl(fd, DCMD_TIMER_CONFIG, &Timer, sizeof(amba_timer_t), NULL);
                    if (err) {
                        fprintf(stderr,"%s: failed: %s\n", __FUNCTION__, strerror(errno));
                    }
                } else {
                    RetVal = TMR_ERR_BUSY;
                }
            }
            close(fd);
        }
    } else {
        RetVal = TMR_ERR_ARG;
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
    amba_timer_t Timer = {0U};
    int fd, err;

    if (TimerID < AMBA_NUM_AVAIL_TIMER) {
        fd = open("/dev/amba_tmr", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/amba_tmr manager.", __FUNCTION__);
        } else {
            Timer.TimerId = TimerID;
            Timer.NumRemainTick = NumRemainTick;
            err = devctl(fd, DCMD_TIMER_START, &Timer, sizeof(amba_timer_t), NULL);
            if (err) {
                fprintf(stderr, "%s: failed: %s", __FUNCTION__, strerror(errno));
            }
            close(fd);
        }
    } else {
        RetVal = TMR_ERR_ARG;
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
    amba_timer_t Timer = {0U};
    int fd, err;

    if (TimerID < AMBA_NUM_AVAIL_TIMER) {
        (void)InterruptMask(AmbaTmrSysCtrl[TimerID].IntID, -1);
        if (AmbaTmrSysCtrl[TimerID].ThreadID != -1) {
            pthread_cancel(AmbaTmrSysCtrl[TimerID].ThreadID);
        }

        fd = open("/dev/amba_tmr", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/amba_tmr manager.", __FUNCTION__);
        } else {
            Timer.TimerId = TimerID;
            err = devctl(fd, DCMD_TIMER_STOP, &Timer, sizeof(amba_timer_t), NULL);
            if (err) {
                fprintf(stderr,"%s: failed: %s", __FUNCTION__, strerror(errno));
            }
            close(fd);
        }
    } else {
        RetVal = TMR_ERR_ARG;
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

    (void)TimerID;
    (void)TimeOut;

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
    amba_timer_t Timer = {0U};
    int fd, err;

    if (TimerID < AMBA_NUM_AVAIL_TIMER) {
        fd = open("/dev/amba_tmr", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/amba_tmr manager.", __FUNCTION__);
        } else {
            Timer.TimerId = TimerID;
            err = devctl(fd, DCMD_TIMER_SHOW, &Timer, sizeof(amba_timer_t), NULL);
            if (err) {
                fprintf(stderr,"%s: failed: %s", __FUNCTION__, strerror(errno));
            } else {
                *pTimeLeft = Timer.NumRemainTick;
            }
            close(fd);
        }
    } else {
        RetVal = TMR_ERR_ARG;
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
    timer_info_t TimerInfo = {0U};
    int fd, err;

    if (TimerID < AMBA_NUM_AVAIL_TIMER) {
        fd = open("/dev/amba_tmr", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/amba_tmr manager.", __FUNCTION__);
        } else {
            TimerInfo.TimerId = TimerID;
            err = devctl(fd, DCMD_TIMER_GETINFO, &TimerInfo, sizeof(timer_info_t), NULL);
            if (err) {
                fprintf(stderr,"%s: failed: %s", __FUNCTION__, strerror(errno));
            } else {
                pTimerInfo->ExpireCount = TimerInfo.ExpireCount;
                pTimerInfo->SysFreq = TimerInfo.SysFreq;
                pTimerInfo->TimerFreq = TimerInfo.TimerFreq;
                pTimerInfo->PeriodicInterval = TimerInfo.PeriodicInterval;
                pTimerInfo->State = TimerInfo.State;
            }
            close(fd);
        }
    } else {
        RetVal = TMR_ERR_ARG;
    }

    return RetVal;
}
