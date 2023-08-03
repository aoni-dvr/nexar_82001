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
#include <fcntl.h>
#include <threads.h>

#include <hw/ambarella_clk.h>
#include "Amba_timer.h"

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaRTSL_TMR.h"
#include "AmbaCSL_TMR.h"

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#define AMBA_NUM_AVAIL_TIMER AMBA_R52_TIMER_OFFSET
#else
#define AMBA_NUM_AVAIL_TIMER AMBA_NUM_TIMER
#endif

typedef struct {
    UINT32  TimerFreq;
    UINT32  NumPeriodicTick;
    UINT32  NumRefClkTick;          /* Number of reference clock cycles to form a actual timer tick */
    UINT32  ExpireCount;
    UINT32  UserIntFuncArg;
    AMBA_TMR_ISR_f  UserIntFunc;    /* The table of line ISRs */
} AMBA_TMR_CTRL_s;

typedef struct {
    UINT32  IntID;                  /* Interrupt ID */
    pthread_t ThreadID;
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
UINT32 SourceClock = 0U;

/**
 *  AmbaRTSL_TmrInit - Initialize the timers
 */
void AmbaRTSL_TmrInit(void)
{
    UINT32 i;
    int fd, err;
    clk_freq_t clk_freq;

    fd = open("/dev/clock", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/clock manager. Use default clocks", __FUNCTION__);
    } else {
        clk_freq.id = AMBA_CLK_APB;


        err = devctl(fd, DCMD_CLOCK_GET_FREQ, &clk_freq, sizeof(clk_freq_t), NULL);
        if (err) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: GET devctl failed: %s", __FUNCTION__, strerror(errno));
        }

        if (clk_freq.freq == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: /dev/clock: Invalid frequency (-1)", __FUNCTION__);
        } else {
            /* Set real frequency */
            SourceClock = clk_freq.freq;
        }

        close(fd);
    }

    /* Program default values into timer registers */
    for (i = 0U; i < AMBA_NUM_AVAIL_TIMER; i++)  {
        AmbaCSL_TmrSetEnable(i, 0U);
        AmbaCSL_TmrSetConfig(i, AMBA_TMR_CLK_APB, AMBA_TMR_INT_ENABLE);

        /* Program default values into timer registers */
        AmbaCSL_TmrSetCurrentVal(i, 0xffffffffU);
        AmbaCSL_TmrSetReloadVal(i, 0x0U);
        AmbaCSL_TmrSetMatch0Val(i, 0x0U);
        AmbaCSL_TmrSetMatch1Val(i, 0x0U);

        (void)InterruptMask(AmbaTmrSysCtrl[i].IntID, -1);
    }
}

/**
 *  AmbaRTSL_TmrConfig - Configure a timer
 *  @param[in] TimerID Timer ID
 *  @param[in] TimerFreq Desired timer frequency
 *  @param[in] NumPeriodicTick Interval between two timer expiration (in number of ticks)
 *  @return error code
 */
UINT32 AmbaRTSL_TmrConfig(UINT32 TimerID, UINT32 TimerFreq, UINT32 NumPeriodicTick)
{
    UINT32 RetVal = TMR_ERR_NONE;
    AMBA_TMR_CTRL_s *pTimer = &AmbaTmrCtrl[TimerID];
    UINT32 RefFreq;

    if ((TimerID >= AMBA_NUM_TIMER) || (TimerFreq == 0U)) {
        RetVal = TMR_ERR_ARG;
    } else {
        pTimer->TimerFreq = TimerFreq;
        pTimer->NumPeriodicTick = NumPeriodicTick;

        if (NumPeriodicTick == 0U) {
            /* NOTE: For cyclic timer, interrupt could be ignored because we can have continuous timestamp values by its tick count. */
            /* NOTE: For non-cyclic timer, interrupt is necessary because its tick count is only meaningful before it reaches zero. */
            (void)InterruptUnmask(AmbaTmrSysCtrl[TimerID].IntID, -1);
            AmbaCSL_TmrSetReloadVal(TimerID, 0U);
        } else {
            /* Assume the reference clock is from gclk_apb */
            /* TODO: external clock source */
            RefFreq = SourceClock;

            pTimer->NumRefClkTick = RefFreq / TimerFreq;
            if (NumPeriodicTick > (0xffffffffU / pTimer->NumRefClkTick)) {
                /* hardware counter width is 32-bit */
                RetVal = TMR_ERR_NA;
            } else {
                /* NOTE: If RefFreq is not a multiple of TimerFreq, the accuracy of the timer will be low. */
                AmbaCSL_TmrSetReloadVal(TimerID, (NumPeriodicTick * pTimer->NumRefClkTick) - 1U);
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_TmrStart - Start a timer
 *  @param[in] TimerID Timer ID
 *  @param[in] NumRemainTick Interval before the next timer expiration (in number of ticks)
 *  @return error code
 */
UINT32 AmbaRTSL_TmrStart(UINT32 TimerID, UINT32 NumRemainTick)
{
    UINT32 RetVal = TMR_ERR_NONE;
    AMBA_TMR_CTRL_s *pTimer;

    if (TimerID >= AMBA_NUM_TIMER) {
        RetVal = TMR_ERR_ARG;
    } else {
        pTimer = &AmbaTmrCtrl[TimerID];
        pTimer->ExpireCount = 0U;

        if (NumRemainTick > (0xffffffffU / pTimer->NumRefClkTick)) {
            /* hardware counter width is 32-bit */
            RetVal = TMR_ERR_NA;
        } else {
            /* NOTE: If RefFreq is not a multiple of TimerFreq, the accuracy of the timer will be low. */
            AmbaCSL_TmrSetCurrentVal(TimerID, (NumRemainTick * pTimer->NumRefClkTick));
            AmbaCSL_TmrSetEnable(TimerID, 1U);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_TmrStop - Stop a timer
 *  @param[in] TimerID Timer ID
 *  @return error code
 */
UINT32 AmbaRTSL_TmrStop(UINT32 TimerID)
{
    UINT32 RetVal = TMR_ERR_NONE;

    if (TimerID >= AMBA_NUM_TIMER) {
        RetVal = TMR_ERR_ARG;
    } else {
        (void)InterruptMask(AmbaTmrSysCtrl[TimerID].IntID, -1);
        AmbaCSL_TmrSetEnable(TimerID, 0U);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_TmrShowTickCount - Show the current tick count value
 *  @param[in] TimerID Timer ID
 *  @param[out] pTimeLeft Number of the remaining ticks before expiration
 *  @return error code
 */
UINT32 AmbaRTSL_TmrShowTickCount(UINT32 TimerID, UINT32 * pTimeLeft)
{
    UINT32 RetVal = TMR_ERR_NONE;
    const AMBA_TMR_CTRL_s *pTimer;

    if ((TimerID >= AMBA_NUM_TIMER) || (pTimeLeft == NULL)) {
        RetVal = TMR_ERR_ARG;
    } else {
        pTimer = &AmbaTmrCtrl[TimerID];

        /* Don't do rounding otherwise it might cause external clock ticking inversion */
        *pTimeLeft = AmbaCSL_TmrGetCurrentVal(TimerID) / pTimer->NumRefClkTick;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_TmrGetInfo - Get timer information
 *  @param[in] TimerID Timer ID
 *  @param[out] pTimerInfo Timer information
 *  @return error code
 */
UINT32 AmbaRTSL_TmrGetInfo(UINT32 TimerID, AMBA_TMR_INFO_s * pTimerInfo)
{
    UINT32 RetVal = TMR_ERR_NONE;
    const AMBA_TMR_CTRL_s *pTimer;

    if ((TimerID >= AMBA_NUM_TIMER) || (pTimerInfo == NULL)) {
        RetVal = TMR_ERR_ARG;
    } else {
        pTimer = &AmbaTmrCtrl[TimerID];

        /* Assume the reference clock is from gclk_apb */
        /* TODO: external clock source */
        pTimerInfo->SysFreq = SourceClock;
        pTimerInfo->TimerFreq = pTimer->TimerFreq;
        pTimerInfo->PeriodicInterval = pTimer->NumPeriodicTick;
        pTimerInfo->ExpireCount = pTimer->ExpireCount;
        pTimerInfo->State = AmbaCSL_TmrGetEnable(TimerID);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_TmrCoreFreqChg - Reconfigure timer based on the new clock frequency
 */
void AmbaRTSL_TmrCoreFreqChg(void)
{
    return;
}

/**
 *  AmbaRTSL_TmrGetOsTimerId - Get timer id reserved for RTOS
 *  @param[out] pTimerID Timer ID
 *  @return error code
 */
UINT32 AmbaRTSL_TmrGetOsTimerId(UINT32 *pTimerID)
{
    UINT32 RetVal = TMR_ERR_NONE;

    *pTimerID = AMBA_NUM_AVAIL_TIMER;

    return RetVal;
}
