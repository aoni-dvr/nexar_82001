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
// #include "AmbaRTSL_WDT.h"

#define WATCHDOG_IOCTL_BASE 'W'

#define WDIOC_GETSUPPORT    _IOR(WATCHDOG_IOCTL_BASE, 0, struct watchdog_info)
#define WDIOC_GETSTATUS     _IOR(WATCHDOG_IOCTL_BASE, 1, int)
#define WDIOC_GETBOOTSTATUS _IOR(WATCHDOG_IOCTL_BASE, 2, int)
#define WDIOC_GETTEMP       _IOR(WATCHDOG_IOCTL_BASE, 3, int)
#define WDIOC_SETOPTIONS    _IOR(WATCHDOG_IOCTL_BASE, 4, int)
#define WDIOC_KEEPALIVE     _IOR(WATCHDOG_IOCTL_BASE, 5, int)
#define WDIOC_SETTIMEOUT    _IOWR(WATCHDOG_IOCTL_BASE, 6, int)
#define WDIOC_GETTIMEOUT    _IOR(WATCHDOG_IOCTL_BASE, 7, int)
#define WDIOC_SETPRETIMEOUT _IOWR(WATCHDOG_IOCTL_BASE, 8, int)
#define WDIOC_GETPRETIMEOUT _IOR(WATCHDOG_IOCTL_BASE, 9, int)
#define WDIOC_GETTIMELEFT   _IOR(WATCHDOG_IOCTL_BASE, 10, int)

#define WDIOS_DISABLECARD   0x0001  /* Turn off the watchdog timer */
#define WDIOS_ENABLECARD    0x0002  /* Turn on the watchdog timer */

#define WD_SETTIMEOUT_MAX   (79)
#define WD_DEVICE_NAME      "/dev/watchdog"

#define WD_STOP_WDT_RUN     (0U)
#define WD_KEEP_WDT_RUN     (1U)

static INT32 fd = -1;
static UINT8 LinuxMW_trigger_wdt = WD_STOP_WDT_RUN;

static UINT32 init_fd(const UINT8 WdtActionAfterOpen)
{
    UINT32 RetVal = WDT_ERR_NONE;
    INT32 Flags, Ret;

    if (fd == -1) {
        fd = open(WD_DEVICE_NAME, O_WRONLY);
    }
    if (fd == -1) {
        if (errno == ENOENT) {
            printf("Watchdog device (%s) not found.\n", WD_DEVICE_NAME);
        } else if (errno == EACCES) {
            printf("Run watchdog as root.\n");
        } else {
            printf("Watchdog device open failed %s\n",
                   strerror(errno));
        }
        RetVal = WDT_ERR_UNEXPECTED;
    } else {
        if (WdtActionAfterOpen == WD_STOP_WDT_RUN) {
            Flags = WDIOS_DISABLECARD;
            Ret = ioctl(fd, WDIOC_SETOPTIONS, &Flags);
            if (Ret != 0) {
                printf("WDIOS_DISABLECARD error '%s'\n", strerror(errno));
                RetVal = WDT_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaWDT_DrvEntry - WDT device driver initializations
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaWDT_DrvEntry(void)
{
    UINT32 RetVal = WDT_ERR_NONE;
    if (system("modprobe ambarella-wdt 2>/dev/null") < 0) {
        printf("## [NG] AmbaWDT_DrvEntry: modprobe ambarella-wdt 2>/dev/null FAILED !!");
    } else {
        printf("## [OK] AmbaWDT_DrvEntry: modprobe ambarella-wdt 2>/dev/null SUCCESS !!");
    }

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
    (void) ExpireFunc;
    (void) ExpireFuncArg;
    return RetVal;
}

/**
 *  AmbaWDT_Start - Start the WDT.
 *  @param[in] CountDown WDT timeout value (millisecond)
 *  @param[in] ResetIrqPulseWidth Pulse width on WDT timeout
 *  @return error code
 */
UINT32 AmbaWDT_Start(UINT32 CountDown, UINT32 ResetIrqPulseWidth)
{
    UINT32 RetVal = WDT_ERR_NONE;
    INT32 Ret, Flags;
    int Dummy;

    printf("## AmbaWDT_Start: CountDown %d, ResetIrqPulseWidth %d\n", CountDown, ResetIrqPulseWidth);
    (void) ResetIrqPulseWidth; // no implement for pulse width

    // open device
    RetVal = init_fd(WD_STOP_WDT_RUN);
    if (RetVal == WDT_ERR_NONE) {
        // Set WD timeout (the uint is one second); maximum is 79 seconds
        Flags = CountDown / 1000U;
        if (Flags >= WD_SETTIMEOUT_MAX) {
            Flags = WD_SETTIMEOUT_MAX;
        } else {
            Flags += !!(CountDown % 1000U);
        }
        printf("[DBG] set timeout=%u sec.\n", Flags);
        Ret = ioctl(fd, WDIOC_SETTIMEOUT, &Flags);
        if (Ret != 0) {
            printf("WDIOC_SETTIMEOUT error '%s'\n", strerror(errno));
            RetVal = WDT_ERR_UNEXPECTED;
        } else {
            printf("Watchdog timeout set to %u seconds.\n", Flags);
        }

        // Enable WD
        Flags = WDIOS_ENABLECARD;
        Ret = ioctl(fd, WDIOC_SETOPTIONS, &Flags);
        if (Ret != 0) {
            printf("WDIOS_ENABLECARD error '%s'\n", strerror(errno));
            RetVal = WDT_ERR_UNEXPECTED;
        }

        // Kick WD (must do this kick; or wdt reset will start immediately)
        Ret = ioctl(fd, WDIOC_KEEPALIVE, &Dummy);
        if (Ret != 0) {
            RetVal = WDT_ERR_UNEXPECTED;
        }

        LinuxMW_trigger_wdt = WD_KEEP_WDT_RUN;
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
    INT32 Ret, Dummy;
    RetVal = init_fd(WD_KEEP_WDT_RUN);
    if (RetVal == WDT_ERR_NONE) {
        Ret = ioctl(fd, WDIOC_KEEPALIVE, &Dummy);
        if (Ret != 0) {
            printf("WDIOC_KEEPALIVE error '%s'\n", strerror(errno));
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
    INT32 Flags, Ret;
    if (fd > 0) {
        Flags = WDIOS_DISABLECARD;
        Ret = ioctl(fd, WDIOC_SETOPTIONS, &Flags);
        if (Ret != 0) {
            RetVal = WDT_ERR_UNEXPECTED;
        }
        LinuxMW_trigger_wdt = 0U;
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
    INT32 Ret, Flags;
    FILE *ifp;
    char buf[16]  = "\0";
    char cmd[] = "find /sys/firmware/devicetree/base/ -type d -name wdt@* | xargs ls | grep interrupts";

    if (pWdtInfo == NULL) {
        RetVal = WDT_ERR_ARG;
    } else {
        RetVal = init_fd(LinuxMW_trigger_wdt);
        if (RetVal == WDT_ERR_NONE) {
            Ret = ioctl(fd, WDIOC_GETBOOTSTATUS, &Flags);
            if (Ret != 0) {
                printf("WDIOC_GETBOOTSTATUS error '%s'\n", strerror(errno));
                RetVal = WDT_ERR_UNEXPECTED;
            } else {
                pWdtInfo->TimerExpired = Flags;
            }
        }

        #if 0
        /* get time left */
        if (RetVal == WDT_ERR_NONE) {
            Ret = ioctl(fd, WDIOC_GETTIMELEFT, &Flags);
            if (Ret != 0) {
                printf("WDIOC_GETTIMELEFT error '%s'\n", strerror(errno));
                RetVal = WDT_ERR_UNEXPECTED;
            } else {
                printf("WDIOC_GETTIMELEFT = %d\n", Flags);
            }
        }
        #endif

        /* The default ExpireAction is reset. */
        /* Get IRQ ExpireAction from the device tree by checking if there is "interrupts" */
        //TODO: read param
        pWdtInfo->ExpireAction = AMBA_WDT_ACT_SYS_RESET;
        if ((ifp = popen(cmd, "r")) != NULL) {
            if (fgets(buf, sizeof(buf), ifp) != NULL) {
                if (strncmp(buf, "interrupts", 10) == 0) {
                    pWdtInfo->ExpireAction = WDT_ACT_IRQ;
                }
            }
            pclose(ifp);
        }
    }

    return RetVal;
}

/**
 *  AmbaWDT_ClearStatus - Clear WDT status.
 *  @return error code
 */
UINT32 AmbaWDT_ClearStatus(void)
{
    printf("%s not implemented!\n", __func__);
    return 1;
}

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
    (void) ChId;
    (void) ExpireFunc;
    (void) ExpireFuncArg;
    printf("%s not implemented!\n", __func__);
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
    UINT32 RetVal;
    (void) ChId;
    RetVal = AmbaWDT_Start(CountDown, ResetIrqPulseWidth);
    return RetVal;
}

/**
 *  AmbaWDT_FeedByChId - Feed the specified WDT.
 *  @param[in] ChId WDT channel id
 *  @return error code
 */
UINT32 AmbaWDT_FeedByChId(UINT32 ChId)
{
    UINT32 RetVal;
    (void) ChId;
    RetVal = AmbaWDT_Feed();
    return RetVal;
}

/**
 *  AmbaWDT_StopByChId - Stop the specified WDT.
 *  @param[in] ChId WDT channel id
 *  @return error code
 */
UINT32 AmbaWDT_StopByChId(UINT32 ChId)
{
    UINT32 RetVal;
    (void) ChId;
    RetVal = AmbaWDT_Stop();
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
    UINT32 RetVal;
    (void) ChId;
    if (pWdtInfo == NULL) {
        RetVal = WDT_ERR_ARG;
    } else {
        RetVal = AmbaWDT_GetInfo(pWdtInfo);
    }
    return RetVal;
}

