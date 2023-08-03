/**
*  @file AmbaUtc.c
*
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
*
*  @details Amba UTC related APIs
*
*/

#include "AmbaRTC_Def.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaCodecCom.h"
#include "AmbaUtc.h"

#define SEC_PER_MIN     (60U)
#define SEC_PER_HOUR    (3600U)
#define SEC_PER_DAY     (86400U)

#define YEAR_LIMIT      (68U)
#define SYS_YEAR_BASE   (1970U)    /* Unix time base: 1970/01/01 00:00:00 */

/* This table need to be updated every year, latest update date 2019/03/13 */
static UINT8 LeapSec[(YEAR_LIMIT + 1U) * 2U] = {
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 1970 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 1971 */
    1U,  /* Jun 30 */    1U,  /* Dec 31 */    /* 1972 */
    0U,  /* Jun 30 */    1U,  /* Dec 31 */    /* 1973 */
    0U,  /* Jun 30 */    1U,  /* Dec 31 */    /* 1974 */
    0U,  /* Jun 30 */    1U,  /* Dec 31 */    /* 1975 */
    0U,  /* Jun 30 */    1U,  /* Dec 31 */    /* 1976 */
    0U,  /* Jun 30 */    1U,  /* Dec 31 */    /* 1977 */
    0U,  /* Jun 30 */    1U,  /* Dec 31 */    /* 1978 */
    0U,  /* Jun 30 */    1U,  /* Dec 31 */    /* 1979 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 1980 */
    1U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 1981 */
    1U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 1982 */
    1U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 1983 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 1984 */
    1U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 1985 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 1986 */
    0U,  /* Jun 30 */    1U,  /* Dec 31 */    /* 1987 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 1988 */
    0U,  /* Jun 30 */    1U,  /* Dec 31 */    /* 1989 */
    0U,  /* Jun 30 */    1U,  /* Dec 31 */    /* 1990 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 1991 */
    1U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 1992 */
    1U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 1993 */
    1U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 1994 */
    0U,  /* Jun 30 */    1U,  /* Dec 31 */    /* 1995 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 1996 */
    1U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 1997 */
    0U,  /* Jun 30 */    1U,  /* Dec 31 */    /* 1998 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 1999 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2000 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2001 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2002 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2003 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2004 */
    0U,  /* Jun 30 */    1U,  /* Dec 31 */    /* 2005 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2006 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2007 */
    0U,  /* Jun 30 */    1U,  /* Dec 31 */    /* 2008 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2009 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2010 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2011 */
    1U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2012 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2013 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2014 */
    1U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2015 */
    0U,  /* Jun 30 */    1U,  /* Dec 31 */    /* 2016 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2017 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2018 */
    0U,  /* Jun 30 */    1U,  /* Dec 31 */    /* 2019 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2020 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2021 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2022 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2023 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2024 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2025 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2026 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2027 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2028 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2029 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2030 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2031 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2032 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2033 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2034 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2035 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2036 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2037 */
    0U,  /* Jun 30 */    0U,  /* Dec 31 */    /* 2038 */
};

static UINT32 UtcTimeOffset[(YEAR_LIMIT + 1U) * 2U] GNU_SECTION_NOZEROINIT;   /* UTC time offset(secs) per half year*/

static UINT32 IsLeapYear(UINT32 Year)
{
    UINT32 Rval = 0U;

    if ((Year % 4U) == 0U) {
        Rval = 1U;
        if ((Year % 100U) == 0U) {
            Rval = 0U;
            if ((Year % 400U) == 0U) {
                Rval = 1U;
            }
        }
    }

    return Rval;
}

/**
* AmbaUtcInit
*/
void AmbaUtcInit(void)
{
    UINT32 DayFirstHalfYear  = 31U + 28U + 31U + 30U + 31U + 30U;    /* if it's not leap year */
    UINT32 DaySecondHalfYear = 31U + 31U + 30U + 31U + 30U + 31U;
    UINT32 Year = SYS_YEAR_BASE, i, Err;

    Err = AmbaWrap_memset(UtcTimeOffset, 0, sizeof(UtcTimeOffset));
    if (Err != CODEC_OK) {
        /* nothing */
    }


    /* Calculate seconds per half year */
    for (i = 0U; i < (YEAR_LIMIT * 2U); i += 2U) {
        if (i == 0U) {
            UtcTimeOffset[i]  = (SEC_PER_DAY * (DayFirstHalfYear + IsLeapYear(Year))) + LeapSec[i];
        } else {
            UtcTimeOffset[i]  = UtcTimeOffset[i - 1U] + (SEC_PER_DAY * (DayFirstHalfYear + IsLeapYear(Year))) + LeapSec[i];
        }

        UtcTimeOffset[i + 1U] = UtcTimeOffset[i] + (SEC_PER_DAY * DaySecondHalfYear) + LeapSec[i + 1U];
        Year++;
    }

}

/**
* get total senconds
* @param [in]  pDateTime date
* @param [out]  pTotalSec TotalSec
* @return ErrorCode
*/
UINT32 AmbaUtcGetTotalSec(const AMBA_RTC_DATE_TIME_s *pDateTime, UINT32 *pTotalSec)
{
    static UINT32 DaysOfMonth[12U] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    UINT32 Rval = CODEC_OK, YearOffset, i, Seconds, MonthMinusOne;

    AmbaMisra_TouchUnused(LeapSec);

    if (0U < IsLeapYear(pDateTime->Year)) {
        DaysOfMonth[1U] = 29U;
    } else {
        DaysOfMonth[1U] = 28U;
    }

    MonthMinusOne = pDateTime->Month - 1U;

    if (MonthMinusOne < 12U) {
        if ((pDateTime->Year  < SYS_YEAR_BASE) ||
            (pDateTime->Year  > (SYS_YEAR_BASE + YEAR_LIMIT)) ||
            (pDateTime->Day   > DaysOfMonth[MonthMinusOne])) {
            Rval = CODEC_ERROR_GENERAL_ERROR;
        }
    } else {
        Rval = CODEC_ERROR_GENERAL_ERROR;
    }

    if (Rval == CODEC_OK) {
        /* Year offset */
        YearOffset = pDateTime->Year - SYS_YEAR_BASE;

        if (pDateTime->Month > 6U) {
            /* Second half-year */
            i = 6U;
            Seconds = UtcTimeOffset[(YearOffset << 1UL)];
        } else {
            /* First half-year */
            i = 0U;
            if (YearOffset > 0U) {
                Seconds = UtcTimeOffset[(YearOffset << 1UL) - 1U];
            } else {
                Seconds = 0U;
            }
        }

        while (i < (pDateTime->Month - 1U)) {
            Seconds += SEC_PER_DAY * DaysOfMonth[i];
            i ++;
        }

        Seconds += SEC_PER_DAY  * (pDateTime->Day - 1U);
        Seconds += SEC_PER_HOUR * pDateTime->Hour;
        Seconds += SEC_PER_MIN  * pDateTime->Minute;
        Seconds += pDateTime->Second;

        *pTotalSec = Seconds;
    }

    return Rval;
}
