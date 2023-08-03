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
#ifdef __threadx
#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"
#include "AmbaTime.h"
#include "AmbaRTC.h"
#include "AmbaRTSL_RTC.h"
#endif
#include <fcntl.h>
#include <stdio.h>

#include "AmbaTypes.h"
#include "hw/ambarella_rtc.h"
#include "hw/time.h"
#include "AmbaRTC.h"

static AMBA_KAL_MUTEX_t AmbaRtcMutex;

static UINT32 AmbaRtcLastSetSystemTime;
static UINT32 AmbaRtcLastSetTickCount;

static AMBA_RTC_SET_DATE_TIME_CB_f AmbaRtcSetDateTimeFunc;
static AMBA_RTC_SET_TIME_STAMP_CB_f AmbaRtcSetTimeStampFunc;
static AMBA_RTC_GET_DATE_TIME_CB_f AmbaRtcGetDateTimeFunc;
static AMBA_RTC_GET_TIME_STAMP_CB_f AmbaRtcGetTimeStampFunc;

static void RTC_QnxSetSystemTime(UINT32 Seconds)
{
    int fd, err = 0;
    rtc_msg_t msg;

    fd = open("/dev/rtc", O_RDWR);
    if (fd == -1) {
        printf("Failed to open RTC \n");
    } else {
        msg.second = Seconds;

        err = devctl(fd, DCMD_RTC_SET, &msg, sizeof(rtc_msg_t), NULL);
        if (err) {
            printf("DCMD_RTC_SET error \n");
        }

        close(fd);
    }
}

/**
 *  RTC_SetSystemTime - Set system time
 *  @param[in] Seconds Number of seconds that have elapsed since an epoch of the beginning of 1970 GMT
 *  @return error code
 */
static UINT32 RTC_SetSystemTime(UINT32 Seconds)
{
    UINT32 RetVal = RTC_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaRtcMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = RTC_ERR_MUTEX;
    } else {
        RTC_QnxSetSystemTime(Seconds);
        AmbaRtcLastSetSystemTime = Seconds;
        if (AmbaKAL_GetSysTickCount(&AmbaRtcLastSetTickCount) != KAL_ERR_NONE) {
            RetVal = RTC_ERR_UNEXPECTED;
        }

        if (AmbaKAL_MutexGive(&AmbaRtcMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = RTC_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

static UINT32 RTC_QnxGetSystemTime(void)
{
    int fd, err = 0;
    rtc_msg_t msg;

    fd = open("/dev/rtc", O_RDWR);
    if (fd == -1) {
        msg.second = 0xFFFFFFFF;
        printf("Failed to open RTC \n");
    } else {

        err = devctl(fd, DCMD_RTC_GET, &msg, sizeof(rtc_msg_t), NULL);
        if (err) {
            printf("DCMD_RTC_SET error \n");
        }

        close(fd);
    }

    return msg.second;
}

/**
 *  RTC_GetSystemTime - Get system time
 *  @param[out] pSysTime Number of seconds that have elapsed since an epoch of the beginning of 1970 GMT
 *  @return error code
 */
static UINT32 RTC_GetSystemTime(UINT32 *pSysTime)
{
    UINT32 SysTime, SysTickCount = 0U;
    UINT32 RetVal = RTC_ERR_NONE;

    if (pSysTime == NULL) {
        RetVal = RTC_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaRtcMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = RTC_ERR_MUTEX;
        } else {
            /* check if system time changed within 1 second after apply the new system time */
            SysTime = RTC_QnxGetSystemTime();

            if (AmbaKAL_GetSysTickCount(&SysTickCount) != KAL_ERR_NONE) {
                RetVal = RTC_ERR_UNEXPECTED;
            } else {
                if (((SysTime - AmbaRtcLastSetSystemTime) == 1U) &&
                    ((SysTickCount - AmbaRtcLastSetTickCount) < 1000U)) {
                    /* set back to the original system time */
                    RTC_QnxSetSystemTime(AmbaRtcLastSetSystemTime);
                    *pSysTime = AmbaRtcLastSetSystemTime;
                } else {
                    *pSysTime = SysTime;
                }
            }

            if (AmbaKAL_MutexGive(&AmbaRtcMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = RTC_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaRTC_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaRTC_DrvEntry(void)
{
    UINT32 RetVal = RTC_ERR_NONE;

    if (AmbaKAL_MutexCreate(&AmbaRtcMutex, NULL) != KAL_ERR_NONE) {
        RetVal = RTC_ERR_UNEXPECTED;
    } else {
        AmbaTime_Init();

        AmbaRtcSetDateTimeFunc = NULL;
        AmbaRtcSetTimeStampFunc = RTC_SetSystemTime;
        AmbaRtcGetDateTimeFunc = NULL;
        AmbaRtcGetTimeStampFunc = RTC_GetSystemTime;

        AmbaRtcLastSetSystemTime = 0U;
        if (AmbaKAL_GetSysTickCount(&AmbaRtcLastSetTickCount) != KAL_ERR_NONE) {
            RetVal = RTC_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaRTC_Config - Hook hardware functions
 *  @param[in] SetDateTimeFunc User-defined function to set data/time
 *  @param[in] SetTimeStampFunc User-defined function to set unix time
 *  @param[in] GetDateTimeFunc User-defined function to get data/time
 *  @param[in] GetTimeStampFunc User-defined function to get unix time
 *  @return error code
 */
UINT32 AmbaRTC_Config(const AMBA_RTC_SET_DATE_TIME_CB_f SetDateTimeFunc, const AMBA_RTC_SET_TIME_STAMP_CB_f SetTimeStampFunc, AMBA_RTC_GET_DATE_TIME_CB_f GetDateTimeFunc, AMBA_RTC_GET_TIME_STAMP_CB_f GetTimeStampFunc)
{
    AmbaTime_Init();

    if ((SetDateTimeFunc == NULL) && (SetTimeStampFunc == NULL)) {
        AmbaRtcSetDateTimeFunc = NULL;
        AmbaRtcSetTimeStampFunc = RTC_SetSystemTime;
    } else if (SetDateTimeFunc == NULL) {
        AmbaRtcSetDateTimeFunc = NULL;
        AmbaRtcSetTimeStampFunc = SetTimeStampFunc;
    } else {
        AmbaRtcSetDateTimeFunc = SetDateTimeFunc;
        AmbaRtcSetTimeStampFunc = NULL;
    }

    if ((GetDateTimeFunc == NULL) && (GetTimeStampFunc == NULL)) {
        AmbaRtcGetDateTimeFunc = NULL;
        AmbaRtcGetTimeStampFunc = RTC_GetSystemTime;
    } else if (GetDateTimeFunc == NULL) {
        AmbaRtcGetDateTimeFunc = NULL;
        AmbaRtcGetTimeStampFunc = GetTimeStampFunc;
    } else {
        AmbaRtcGetDateTimeFunc = GetDateTimeFunc;
        AmbaRtcGetTimeStampFunc = NULL;
    }

    return RTC_ERR_NONE;
}

/**
 *  AmbaRTC_SetSysTime - Set system time
 *  @param[in] Seconds Number of seconds that have elapsed since an epoch
 *  @return error code
 */
UINT32 AmbaRTC_SetSysTime(const AMBA_RTC_DATE_TIME_s * pCurrTime)
{
    UINT32 RetVal = RTC_ERR_NONE;
    UINT32 TimeStamp;

    if (AmbaRtcSetDateTimeFunc != NULL) {
        RetVal = AmbaRtcSetDateTimeFunc(pCurrTime);
    } else {
        if (AmbaTime_TaiDateTime2TimeStamp(pCurrTime, &TimeStamp) != OK) {
            RetVal = RTC_ERR_ARG;
        } else {
            if (AmbaRtcSetTimeStampFunc != NULL) {
                RetVal = AmbaRtcSetTimeStampFunc(TimeStamp);
            } else {
                /* Shall never happen!! */
                RetVal = RTC_ERR_USR;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaRTC_GetSysTime - Get system time
 *  @param[in] pCurrTime Number of seconds that have elapsed since an epoch
 *  @return error code
 */
UINT32 AmbaRTC_GetSysTime(AMBA_RTC_DATE_TIME_s * pCurrTime)
{
    UINT32 RetVal = RTC_ERR_NONE;
    UINT32 TimeStamp;

    if (AmbaRtcGetDateTimeFunc != NULL) {
        RetVal = AmbaRtcGetDateTimeFunc(pCurrTime);
    } else {
        if (AmbaRtcGetTimeStampFunc != NULL) {
            RetVal = AmbaRtcGetTimeStampFunc(&TimeStamp);
            if (AmbaTime_TaiTimeStamp2DateTime(TimeStamp, pCurrTime) != OK) {
                RetVal = RTC_ERR_ARG;
            }
        } else {
            /* Shall never happen!! */
            RetVal = RTC_ERR_USR;
        }
    }

    return RetVal;
}

