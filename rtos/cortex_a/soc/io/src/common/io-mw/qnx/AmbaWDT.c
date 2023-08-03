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
#include "hw/wdt.h"
#include "Generic.h"
#include <sys/resmgr.h>
#include <threads.h>

#include "AmbaTypes.h"
#include "AmbaWDT.h"

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
static AMBA_WDT_ISR_f AmbaWdtTimeOutFunc[AMBA_WDT_CH_NUM] = {0U};
static UINT32 AmbaWdtTimeOutFuncArg[AMBA_WDT_CH_NUM] = {0U};
static int WdtThreadID[AMBA_WDT_CH_NUM] = {0U};

#pragma GCC push_options
#pragma GCC target("general-regs-only")
static void *WDT_ISR(void* argv)
{
    struct sigevent event;
    int iid;
    UINT32 TimerID = *(UINT32 *)argv;
    UINT32 IntId = 84U;

    switch (TimerID) {
        case 0U:
            IntId = 84U;
            break;
        case 1U:
            IntId = 169U;
            break;
        case 2U:
            IntId = 170U;
            break;
        case 3U:
            IntId = 171U;
            break;
        case 4U:
            IntId = 172U;
            break;
        case 5U:
            IntId = 168U;
            break;
        default:
            IntId = 84U;
            break;
    }

    SIGEV_INTR_INIT(&event);
    iid = InterruptAttachEvent(IntId, &event, 0U);
    InterruptUnmask(IntId, iid);

    InterruptWait(0, NULL);
    if (AmbaWdtTimeOutFunc[TimerID] != NULL) {
        AmbaWdtTimeOutFunc[TimerID](AmbaWdtTimeOutFuncArg[TimerID]);
    }

    pthread_exit(NULL);
}
#pragma GCC pop_options
#endif

/**
 *  AmbaWDT_HookTimeOutHandler - The function registers an interrupt handler on watchdog timer expiry.
 *  @param[in] ExpireFunc WDT timeout handler
 *  @param[in] ExpireFuncArg Optional argument attached to WDT timeout handler
 *  @return error code
 */
UINT32 AmbaWDT_HookTimeOutHandler(AMBA_WDT_ISR_f ExpireFunc, UINT32 ExpireFuncArg)
{
    UINT32 RetVal = WDT_ERR_NONE;
    int fd, err;
    amba_wdt_t WdtCtrl;

    if ((ExpireFunc == NULL) && (ExpireFuncArg != 0U)) {
        RetVal = WDT_ERR_ARG;
    } else {
        fd = open("/dev/wdt", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/wdt manager. Use default clocks", __FUNCTION__);
        } else {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
            WdtCtrl.Channel = 0U;
#endif
            WdtCtrl.Handler = ExpireFunc;
            WdtCtrl.HandlerArgv = ExpireFuncArg;

            err = devctl(fd, DCMD_WDT_HOOKHAND, &WdtCtrl, sizeof(amba_wdt_t), NULL);
            if (err) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: GET devctl failed: %s", __FUNCTION__, strerror(errno));
                RetVal = WDT_ERR_UNEXPECTED;
            }

            close(fd);
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
            AmbaWdtTimeOutFunc[0U] = ExpireFunc;
            AmbaWdtTimeOutFuncArg[0U] = ExpireFuncArg;
#endif
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
    int fd, err;
    amba_wdt_t WdtCtrl;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    INT32 QnxRet = 0;
#endif

    fd = open("/dev/wdt", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/wdt manager. Use default clocks", __FUNCTION__);
    } else {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        if (AmbaWdtTimeOutFunc[0U] != NULL) {
            QnxRet = pthread_create(&WdtThreadID[0U], NULL, WDT_ISR, &QnxRet);
            if (QnxRet == 0) {
                pthread_setschedprio(WdtThreadID[0U], 128U);

                WdtCtrl.Channel = 0;
                WdtCtrl.CountDown = CountDown;
                WdtCtrl.ResetIrqPulseWidth = ResetIrqPulseWidth;
                err = devctl(fd, DCMD_WDT_START, &WdtCtrl, sizeof(amba_wdt_t), NULL);
                if (err) {
                    slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: GET devctl failed: %s", __FUNCTION__, strerror(errno));
                    pthread_cancel(WdtThreadID[0U]);
                    RetVal = WDT_ERR_UNEXPECTED;
                }
            } else {
                printf("%s %d, pthread_create failed, QnxRet(%d)\n", __FUNCTION__, __LINE__, QnxRet);
            }
        } else {
            WdtCtrl.Channel = 0;
            WdtCtrl.CountDown = CountDown;
            WdtCtrl.ResetIrqPulseWidth = ResetIrqPulseWidth;
            err = devctl(fd, DCMD_WDT_START, &WdtCtrl, sizeof(amba_wdt_t), NULL);
            if (err) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: GET devctl failed: %s", __FUNCTION__, strerror(errno));
                RetVal = WDT_ERR_UNEXPECTED;
            }
        }
#else
        WdtCtrl.CountDown = CountDown;
        WdtCtrl.ResetIrqPulseWidth = ResetIrqPulseWidth;

        err = devctl(fd, DCMD_WDT_START, &WdtCtrl, sizeof(amba_wdt_t), NULL);
        if (err) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: GET devctl failed: %s", __FUNCTION__, strerror(errno));
            RetVal = WDT_ERR_UNEXPECTED;
        }

#endif
        close(fd);
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
    int fd, err;
    amba_wdt_t WdtCtrl;

    fd = open("/dev/wdt", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/wdt manager. Use default clocks", __FUNCTION__);
    } else {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        WdtCtrl.Channel = AMBA_WDT_CH0;
#endif
        err = devctl(fd, DCMD_WDT_FEED, &WdtCtrl, sizeof(amba_wdt_t), NULL);
        if (err) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: GET devctl failed: %s", __FUNCTION__, strerror(errno));
            RetVal = WDT_ERR_UNEXPECTED;
        }

        close(fd);
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
    int fd, err;
    amba_wdt_t WdtCtrl;

    fd = open("/dev/wdt", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/wdt manager. Use default clocks", __FUNCTION__);
    } else {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        WdtCtrl.Channel = AMBA_WDT_CH0;
#endif
        err = devctl(fd, DCMD_WDT_STOP, &WdtCtrl, sizeof(amba_wdt_t), NULL);
        if (err) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: GET devctl failed: %s", __FUNCTION__, strerror(errno));
            RetVal = WDT_ERR_UNEXPECTED;
        }

        close(fd);
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
    int fd, err;
    amba_wdt_info_t WdtInfo;

    fd = open("/dev/wdt", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/wdt manager. Use default clocks", __FUNCTION__);
    } else {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        WdtInfo.Channel = AMBA_WDT_CH0;
#endif
        err = devctl(fd, DCMD_WDT_GET_INFO, &WdtInfo, sizeof(amba_wdt_info_t), NULL);

        if (err) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: GET devctl failed: %s", __FUNCTION__, strerror(errno));
            RetVal = WDT_ERR_UNEXPECTED;
        } else {
            /* Get WDT info */
            pWdtInfo->TimerExpired = WdtInfo.TimerExpired;
            pWdtInfo->ExpireAction = WdtInfo.ExpireAction;
        }
        close(fd);
    }

    return RetVal;
}


/**
 *  AmbaWDT_ClearStatus - Clear WDT status.
 *  @return error code
 */
UINT32 AmbaWDT_ClearStatus(void)
{
    UINT32 RetVal = WDT_ERR_NONE;
    int fd, err;
    amba_wdt_t WdtCtrl;

    fd = open("/dev/wdt", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/wdt manager. Use default clocks", __FUNCTION__);
    } else {
        err = devctl(fd, DCMD_WDT_CLEAN, &WdtCtrl, sizeof(amba_wdt_t), NULL);
        if (err) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: GET devctl failed: %s", __FUNCTION__, strerror(errno));
            RetVal = WDT_ERR_UNEXPECTED;
        }

        close(fd);
    }

    return RetVal;
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
    int fd, err;
    amba_wdt_t WdtCtrl;

    if ((ExpireFunc == NULL) && (ExpireFuncArg != 0U)) {
        RetVal = WDT_ERR_ARG;
    } else {
        fd = open("/dev/wdt", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/wdt manager. Use default clocks", __FUNCTION__);
        } else {
            WdtCtrl.Channel = ChId;
            WdtCtrl.Handler = ExpireFunc;
            WdtCtrl.HandlerArgv = ExpireFuncArg;

            err = devctl(fd, DCMD_WDT_HOOKHAND, &WdtCtrl, sizeof(amba_wdt_t), NULL);
            if (err) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: GET devctl failed: %s", __FUNCTION__, strerror(errno));
                RetVal = WDT_ERR_UNEXPECTED;
            }

            close(fd);

            AmbaWdtTimeOutFunc[ChId] = ExpireFunc;
            AmbaWdtTimeOutFuncArg[ChId] = ExpireFuncArg;
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
    int fd, err;
    amba_wdt_t WdtCtrl;
    INT32 QnxRet = ChId;

    fd = open("/dev/wdt", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/wdt manager. Use default clocks", __FUNCTION__);
    } else {
        WdtCtrl.Channel = ChId;
        WdtCtrl.CountDown = CountDown;
        WdtCtrl.ResetIrqPulseWidth = ResetIrqPulseWidth;
        if (AmbaWdtTimeOutFunc[ChId] != NULL) {
            QnxRet = pthread_create(&WdtThreadID[ChId], NULL, WDT_ISR, &QnxRet);
            if (QnxRet == 0) {
                pthread_setschedprio(WdtThreadID[ChId], 128U);

                err = devctl(fd, DCMD_WDT_START, &WdtCtrl, sizeof(amba_wdt_t), NULL);
                if (err) {
                    slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: GET devctl failed: %s", __FUNCTION__, strerror(errno));
                    pthread_cancel(WdtThreadID[ChId]);
                    RetVal = WDT_ERR_UNEXPECTED;
                }
            } else {
                printf("%s %d, pthread_create failed, QnxRet(%d)\n", __FUNCTION__, __LINE__, QnxRet);
            }
        } else {
            err = devctl(fd, DCMD_WDT_START, &WdtCtrl, sizeof(amba_wdt_t), NULL);
            if (err) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: GET devctl failed: %s", __FUNCTION__, strerror(errno));
                RetVal = WDT_ERR_UNEXPECTED;
            }
        }

        close(fd);
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
    int fd, err;
    amba_wdt_t WdtCtrl;

    fd = open("/dev/wdt", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/wdt manager. Use default clocks", __FUNCTION__);
    } else {
        WdtCtrl.Channel = ChId;
        err = devctl(fd, DCMD_WDT_FEED, &WdtCtrl, sizeof(amba_wdt_t), NULL);
        if (err) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: GET devctl failed: %s", __FUNCTION__, strerror(errno));
            RetVal = WDT_ERR_UNEXPECTED;
        }

        close(fd);
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
    int fd, err;
    amba_wdt_t WdtCtrl;

    fd = open("/dev/wdt", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/wdt manager. Use default clocks", __FUNCTION__);
    } else {
        WdtCtrl.Channel = ChId;
        err = devctl(fd, DCMD_WDT_STOP, &WdtCtrl, sizeof(amba_wdt_t), NULL);
        if (err) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: GET devctl failed: %s", __FUNCTION__, strerror(errno));
            RetVal = WDT_ERR_UNEXPECTED;
        }

        close(fd);
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
    int fd, err;
    amba_wdt_info_t WdtInfo;

    fd = open("/dev/wdt", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/wdt manager. Use default clocks", __FUNCTION__);
    } else {
        WdtInfo.Channel = ChId;
        err = devctl(fd, DCMD_WDT_GET_INFO, &WdtInfo, sizeof(amba_wdt_info_t), NULL);

        if (err) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: GET devctl failed: %s", __FUNCTION__, strerror(errno));
            RetVal = WDT_ERR_UNEXPECTED;
        } else {
            /* Get WDT info */
            pWdtInfo->TimerExpired = WdtInfo.TimerExpired;
            pWdtInfo->ExpireAction = WdtInfo.ExpireAction;
        }
        close(fd);
    }

    return RetVal;
}

UINT32 AmbaWDT_SetPattern(void)
{
    UINT32 RetVal = WDT_ERR_NONE;
    int fd, err;
    amba_wdt_t WdtCtrl;

    fd = open("/dev/wdt", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/wdt manager. Use default clocks", __FUNCTION__);
    } else {
        err = devctl(fd, DCMD_WDT_SETPATTERN, &WdtCtrl, sizeof(amba_wdt_t), NULL);
        if (err) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: GET devctl failed: %s", __FUNCTION__, strerror(errno));
            RetVal = WDT_ERR_UNEXPECTED;
        }

        close(fd);
    }

    return RetVal;
}
#endif