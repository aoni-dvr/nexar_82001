/**
 *  @file AmbaTime.h
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
 *  @details Definitions & Constants for time spec converter.
 *
 */

#ifndef AMBA_TIME_H
#define AMBA_TIME_H

#ifndef AMBA_RTC_DEF_H
#include "AmbaRTC_Def.h"
#endif

/*
 * Defined in AmbaTime.c
 */
void AmbaTime_Init(void);

UINT32 AmbaTime_TaiDateTime2TimeStamp(const AMBA_RTC_DATE_TIME_s * pDateTime, UINT32 * pTimeStamp);
UINT32 AmbaTime_UtcDateTime2TimeStamp(const AMBA_RTC_DATE_TIME_s * pDateTime, UINT32 * pTimeStamp);
UINT32 AmbaTime_TaiTimeStamp2DateTime(UINT32 TimeStamp, AMBA_RTC_DATE_TIME_s * pDateTime);
UINT32 AmbaTime_UtcTimeStamp2DateTime(UINT32 TimeStamp, AMBA_RTC_DATE_TIME_s * pDateTime);

#endif /* AMBA_TIME_H */
