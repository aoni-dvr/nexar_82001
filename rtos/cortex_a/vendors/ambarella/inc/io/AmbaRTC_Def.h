/**
 *  @file AmbaRTC_Def.h
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
 *  @details Definitions & Constants for RTC (Real-Time Clock) Controller Middleware APIs
 *
 */

#ifndef AMBA_RTC_DEF_H
#define AMBA_RTC_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#define RTC_ERR_0000            (RTC_ERR_BASE)              /* Invalid argument */
#define RTC_ERR_0001            (RTC_ERR_BASE + 0X1U)       /* Hardware error */
#define RTC_ERR_0002            (RTC_ERR_BASE + 0X2U)       /* User function error */
#define RTC_ERR_0003            (RTC_ERR_BASE + 0X3U)       /* Mutex error */
#define RTC_ERR_00FF            (RTC_ERR_BASE + 0XFFU)      /* Unexpected error */

/* RTC error values */
#define RTC_ERR_NONE            (OK)
#define RTC_ERR_ARG             RTC_ERR_0000
#define RTC_ERR_HW              RTC_ERR_0001
#define RTC_ERR_USR             RTC_ERR_0002
#define RTC_ERR_MUTEX           RTC_ERR_0003
#define RTC_ERR_UNEXPECTED      RTC_ERR_00FF

typedef struct {
    UINT32 Year;
    UINT32 Month;
    UINT32 Day;
    UINT32 WeekDay;
    UINT32 Hour;
    UINT32 Minute;
    UINT32 Second;
} AMBA_RTC_DATE_TIME_s;

typedef UINT32 (*AMBA_RTC_SET_DATE_TIME_CB_f)(const AMBA_RTC_DATE_TIME_s * pDateTime);
typedef UINT32 (*AMBA_RTC_SET_TIME_STAMP_CB_f)(UINT32 TimeStamp);
typedef UINT32 (*AMBA_RTC_GET_DATE_TIME_CB_f)(AMBA_RTC_DATE_TIME_s * pDateTime);
typedef UINT32 (*AMBA_RTC_GET_TIME_STAMP_CB_f)(UINT32 * pTimeStamp);

#endif /* AMBA_RTC_DEF_H */
