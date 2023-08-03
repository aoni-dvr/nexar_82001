/**
 *  @file AmbaTime.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Convert Unix timestamp to Readable Date/Time and vice versa.
 *
 */

#include "hw/time.h"

#define SECONDS_PER_MINUTE      60U
#define MINUTES_PER_HOUR        60U
#define HOURS_PER_DAY           24U
#define MONTHS_PER_YEAR         12U

#define SECONDS_PER_HOUR        (SECONDS_PER_MINUTE * MINUTES_PER_HOUR)
#define SECONDS_PER_DAY         (SECONDS_PER_HOUR * HOURS_PER_DAY)      /* No leap second */

#define SYSTEM_TIME_NUM_YEARS   136U    /* limitation of 32-bit system time */

/* An epoch of the beginning of 1970 GMT
 * NOTE: the beginning of 1970 GMT (Thursday)
 *       = 1970-01-01 00:00:00 UTC
 *       = 1970-01-01 00:00:10 TAI
 */
static const AMBA_RTC_DATE_TIME_s AmbaTaiEpoch = {
    .Year       = 1970U,
    .Month      = 1U,               /* January */
    .Day        = 1U,               /* 1th */
    .WeekDay    = 4U,               /* Thursday */
    .Hour       = 0U,
    .Minute     = 0U,
    .Second     = 10U,
};
static const AMBA_RTC_DATE_TIME_s AmbaUtcEpoch = {
    .Year       = 1970U,
    .Month      = 1U,               /* January */
    .Day        = 1U,               /* 1th */
    .WeekDay    = 4U,               /* Thursday */
    .Hour       = 0U,
    .Minute     = 0U,
    .Second     = 0U,
};

/* Time stamp lookup table for the end of Jun 30 and Dec 31 of each year since the Epoch */
static UINT32 AmbaTaiTimeStampTable[SYSTEM_TIME_NUM_YEARS * 2U] GNU_SECTION_NOZEROINIT;
static UINT32 AmbaUtcTimeStampTable[SYSTEM_TIME_NUM_YEARS * 2U] GNU_SECTION_NOZEROINIT;
static UINT32 AmbaCalendarDaysOfMonth[MONTHS_PER_YEAR] = {31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};

/**
 *  GetLeapYear - Check if the year contains one additional day (leap day)
 *  @param[in] Year A calendar year
 *  @return Common Year(0)/Leap Year(1)
 */
static UINT32 GetLeapYear(UINT32 Year)
{
    UINT32 RetVal;

    if ((Year & 0x3U) != 0x0U) {
        RetVal = 0U;
    } else {
        Year %= 400U;
        if ((Year == 100U) || (Year == 200U) || (Year == 300U)) {
            RetVal = 0U;
        } else {
            RetVal = 1U;
        }
    }

    return RetVal;
}

/**
 *  GetLeapSecond - Check if leap second is required
 *  @param[in] Year A calendar year
 *  @param[in] HalfYear first half year (0) or second half year (1)
 *  @return No Leap Second(0)/Leap Second(1)
 */
static UINT32 GetLeapSecond(UINT32 Year, UINT32 HalfYear)
{
    /* IERS decides to apply a leap second insertion every six months */
    static const UINT8 AmbaUtcLeapSecondIERS[SYSTEM_TIME_NUM_YEARS * 2U] = {
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 1970 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 1971 */
        1U, /* Jun 30 */    1U, /* Dec 31 */    /* 1972 */
        0U, /* Jun 30 */    1U, /* Dec 31 */    /* 1973 */
        0U, /* Jun 30 */    1U, /* Dec 31 */    /* 1974 */
        0U, /* Jun 30 */    1U, /* Dec 31 */    /* 1975 */
        0U, /* Jun 30 */    1U, /* Dec 31 */    /* 1976 */
        0U, /* Jun 30 */    1U, /* Dec 31 */    /* 1977 */
        0U, /* Jun 30 */    1U, /* Dec 31 */    /* 1978 */
        0U, /* Jun 30 */    1U, /* Dec 31 */    /* 1979 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 1980 */
        1U, /* Jun 30 */    0U, /* Dec 31 */    /* 1981 */
        1U, /* Jun 30 */    0U, /* Dec 31 */    /* 1982 */
        1U, /* Jun 30 */    0U, /* Dec 31 */    /* 1983 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 1984 */
        1U, /* Jun 30 */    0U, /* Dec 31 */    /* 1985 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 1986 */
        0U, /* Jun 30 */    1U, /* Dec 31 */    /* 1987 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 1988 */
        0U, /* Jun 30 */    1U, /* Dec 31 */    /* 1989 */
        0U, /* Jun 30 */    1U, /* Dec 31 */    /* 1990 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 1991 */
        1U, /* Jun 30 */    0U, /* Dec 31 */    /* 1992 */
        1U, /* Jun 30 */    0U, /* Dec 31 */    /* 1993 */
        1U, /* Jun 30 */    0U, /* Dec 31 */    /* 1994 */
        0U, /* Jun 30 */    1U, /* Dec 31 */    /* 1995 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 1996 */
        1U, /* Jun 30 */    0U, /* Dec 31 */    /* 1997 */
        0U, /* Jun 30 */    1U, /* Dec 31 */    /* 1998 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 1999 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2000 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2001 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2002 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2003 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2004 */
        0U, /* Jun 30 */    1U, /* Dec 31 */    /* 2005 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2006 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2007 */
        0U, /* Jun 30 */    1U, /* Dec 31 */    /* 2008 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2009 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2010 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2011 */
        1U, /* Jun 30 */    0U, /* Dec 31 */    /* 2012 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2013 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2014 */
        1U, /* Jun 30 */    0U, /* Dec 31 */    /* 2015 */
        0U, /* Jun 30 */    1U, /* Dec 31 */    /* 2016 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2017 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2019 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2019 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2020 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2021 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2022 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2023 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2024 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2025 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2026 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2027 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2028 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2029 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2030 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2031 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2032 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2033 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2034 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2035 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2036 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2037 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2038 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2039 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2040 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2041 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2042 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2043 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2044 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2045 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2046 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2047 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2048 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2049 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2050 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2051 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2052 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2053 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2054 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2055 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2056 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2057 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2058 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2059 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2060 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2061 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2062 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2063 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2064 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2065 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2066 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2067 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2068 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2069 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2070 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2071 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2072 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2073 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2074 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2075 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2076 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2077 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2078 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2079 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2080 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2081 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2082 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2083 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2084 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2085 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2086 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2087 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2088 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2089 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2090 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2091 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2092 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2093 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2094 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2095 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2096 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2097 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2098 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2099 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2100 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2101 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2102 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2103 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2104 */
        0U, /* Jun 30 */    0U, /* Dec 31 */    /* 2105 */
    };

    UINT32 RetVal;

    if ((Year < 1970U) || (Year >= 2106U) || (HalfYear >= 2U)) {
        RetVal = 0U;
    } else {
        RetVal = AmbaUtcLeapSecondIERS[((Year - 1970U) << 1U) + HalfYear];
    }

    return RetVal;
}

/**
 *  MakeDate - Convert a day number of year to date
 *  @param[in] Days Day number of the half-year
 *  @param[out] pCalendar pointer to a formatted system time
 */
static void MakeDate(UINT32 Days, AMBA_RTC_DATE_TIME_s *pCalendar)
{
    UINT32 i;

    AmbaCalendarDaysOfMonth[1U] = (GetLeapYear(pCalendar->Year) != 0U) ? 29UL : 28UL;    /* Feb. 29 if leap year */

    for (i = (pCalendar->Month - 1U); i < ((pCalendar->Month - 1U) + 6U); i ++) {
        if (Days < AmbaCalendarDaysOfMonth[i]) {
            pCalendar->Month = i + 1U;
            pCalendar->Day += Days;
            break;
        }
        Days -= AmbaCalendarDaysOfMonth[i];
    }

    if (i == ((pCalendar->Month - 1U) + 6U)) {
        /* Date containing leap second */
        pCalendar->Month += 5U;
        pCalendar->Day = AmbaCalendarDaysOfMonth[pCalendar->Month - 1U];
        pCalendar->Hour = HOURS_PER_DAY - 1U;
        pCalendar->Minute = MINUTES_PER_HOUR - 1U;
        pCalendar->Second = SECONDS_PER_MINUTE; /* Leap second */
    }
}

/**
 *  MakeTime - Convert a second number of day to Hour, Minute, and Second
 *  @param[in] Seconds Second number of the day
 *  @param[out] pCalendar pointer to a formatted system time
 */
static void MakeTime(UINT32 Seconds, AMBA_RTC_DATE_TIME_s *pCalendar)
{
    if (pCalendar->Second < SECONDS_PER_MINUTE) {
        /* Common second */
        pCalendar->Hour += Seconds / SECONDS_PER_HOUR;
        Seconds -= SECONDS_PER_HOUR * pCalendar->Hour;
        pCalendar->Minute += Seconds / SECONDS_PER_MINUTE;
        Seconds -= SECONDS_PER_MINUTE * pCalendar->Minute;
        pCalendar->Second = Seconds;
    }
}

/**
 *  DateTime2TimeStamp - Convert Hour, Minute, and Second to a Second number of day
 *  @param[in] pEpoch epoch of unix time
 *  @param[in] pCalendar date/time
 *  @param[in] pSysTimeOfYear system time offset of each year
 *  @param[out] pSysTime system time of the target date
 *  @return error code
 */
static UINT32 DateTime2TimeStamp(const AMBA_RTC_DATE_TIME_s *pEpoch, const AMBA_RTC_DATE_TIME_s *pCalendar, const UINT32 *pSysTimeOfYear, UINT32 *pSysTime)
{
    UINT32 Seconds, EpochOffset;
    UINT32 i, RetVal = ERR_NONE;

    if (GetLeapYear(pCalendar->Year) != 0U) {
        AmbaCalendarDaysOfMonth[1U] = 29U; /* Feb. 29 if leap year */
    } else {
        AmbaCalendarDaysOfMonth[1U] = 28U; /* Feb. 28 if not leap year */
    }

    if ((pCalendar->Year < pEpoch->Year) ||
        (pCalendar->Year > (pEpoch->Year + SYSTEM_TIME_NUM_YEARS)) ||
        (pCalendar->Month > MONTHS_PER_YEAR) ||
        (pCalendar->Day > AmbaCalendarDaysOfMonth[pCalendar->Month - 1U])) {
        RetVal = ERR_ARG;
    } else {
        /* Year offset */
        EpochOffset = pCalendar->Year - pEpoch->Year;

        if (pCalendar->Month > 6U) {
            /* Second half-year */
            i = 6U;
            Seconds = pSysTimeOfYear[(EpochOffset << 1U)];
        } else {
            /* First half-year */
            i = 0U;
            if (EpochOffset > 0U) {
                Seconds = pSysTimeOfYear[(EpochOffset << 1U) - 1U];
            } else {
                Seconds = 0U;
            }
        }

        while (i < (pCalendar->Month - 1U)) {
            Seconds += SECONDS_PER_DAY * AmbaCalendarDaysOfMonth[i];
            i ++;
        }

        Seconds += SECONDS_PER_DAY * (pCalendar->Day - 1U);
        Seconds += SECONDS_PER_HOUR * pCalendar->Hour;
        Seconds += SECONDS_PER_MINUTE * pCalendar->Minute;
        Seconds += pCalendar->Second;

        /* Second offset */
        EpochOffset = (pEpoch->Second) +
                      (pEpoch->Minute * SECONDS_PER_MINUTE) +
                      (pEpoch->Hour * SECONDS_PER_HOUR);

        if (Seconds < EpochOffset) {
            /* Time before epoch */
            RetVal = ERR_ARG;
        } else {
            *pSysTime = Seconds - EpochOffset;
        }
    }

    return RetVal;
}
extern int printf(const char *fmt, ...);
/**
 *  TimeStamp2DateTime - Convert a Second number of day to Hour, Minute, and Second
 *  @param[in] pEpoch epoch of unix time
 *  @param[in] SysTime system time of the target date
 *  @param[in] pSysTimeOfYear system time offset of each year
 *  @param[out] pCalendar date/time
 *  @return error code
 */
static UINT32 TimeStamp2DateTime(const AMBA_RTC_DATE_TIME_s *pEpoch, UINT32 SysTime, const UINT32 *pSysTimeOfYear, AMBA_RTC_DATE_TIME_s *pCalendar)
{
    UINT32 Seconds, Days, DayOfWeek;
    UINT32 i;

    /* From Epoch */
    pCalendar->Year       = pEpoch->Year;
    pCalendar->Month      = pEpoch->Month;
    pCalendar->Day        = pEpoch->Day;
    pCalendar->WeekDay    = pEpoch->WeekDay;
    pCalendar->Hour       = pEpoch->Hour;
    pCalendar->Minute     = pEpoch->Minute;
    pCalendar->Second     = pEpoch->Second;

    Seconds = SysTime;
    Seconds += pCalendar->Second;
    Seconds += pCalendar->Minute * SECONDS_PER_MINUTE;
    Seconds += pCalendar->Hour * SECONDS_PER_HOUR;

    for (i = 0U; i < (SYSTEM_TIME_NUM_YEARS * 2U); i ++) {
        if (Seconds < pSysTimeOfYear[i]) {
            pCalendar->Year += (i >> 1U);
            if ((i & 1U) != 0U) {
                pCalendar->Month = 7U;  /* Second half-year */
            } else {
                pCalendar->Month = 1U;  /* First half-year */
            }
            break;
        }
    }


    /* DayOfWeek: 0=Sunday, 1=Monday, ..., 6=Saturday */
    /* WeekDay:             1=Monday, ..., 6=Saturday, 7=Sunday */
    DayOfWeek = pCalendar->WeekDay % 7U;
    DayOfWeek += Seconds / SECONDS_PER_DAY;
    pCalendar->WeekDay = (DayOfWeek % 7U);
    if (pCalendar->WeekDay == 0U) {
        pCalendar->WeekDay = 7U;
    }

    if (i > 0U) {
        Seconds -= pSysTimeOfYear[i - 1U];  /* Seconds from the starting of one year */
    }

    Days = Seconds / SECONDS_PER_DAY;
    MakeDate(Days, pCalendar);
    Seconds -= SECONDS_PER_DAY * Days;      /* Seconds from the starting of one day */
    MakeTime(Seconds, pCalendar);


    return ERR_NONE;
}

/**
 *  AmbaTime_Init - Calendar initialization
 */
void AmbaTime_Init(void)
{
    UINT32 DaysOfFirstHalfYear  = 31U + 28U + 31U + 30U + 31U + 30U;  /* No leap day in default value */
    UINT32 DaysOfSecondHalfYear = 31U + 31U + 30U + 31U + 30U + 31U;
    UINT32 Year;
    UINT32 i;

    Year = AmbaTaiEpoch.Year;
    /* Calculate seconds per half year */
    for (i = 0U; i < (SYSTEM_TIME_NUM_YEARS * 2U); i += 2U) {
        AmbaTaiTimeStampTable[i]      = SECONDS_PER_DAY * (DaysOfFirstHalfYear + GetLeapYear(Year));
        if ((i + 1U) < (SYSTEM_TIME_NUM_YEARS * 2U)) {
            AmbaTaiTimeStampTable[i + 1U] = SECONDS_PER_DAY * DaysOfSecondHalfYear;
        }
        Year ++;
    }

    /* Accumulate seconds per half year */
    Year = AmbaTaiEpoch.Year;
    AmbaUtcTimeStampTable[0U] = AmbaTaiTimeStampTable[0U] + GetLeapSecond(Year, 0U);
    for (i = 1U; i < (SYSTEM_TIME_NUM_YEARS * 2U); i ++) {
        AmbaUtcTimeStampTable[i] = AmbaUtcTimeStampTable[i-1U] + AmbaTaiTimeStampTable[i] + GetLeapSecond(Year + (i >> 1U), i & 1U);
        AmbaTaiTimeStampTable[i] = AmbaTaiTimeStampTable[i-1U] + AmbaTaiTimeStampTable[i];
    }
}

/**
 *  AmbaTime_TaiDateTime2TimeStamp - Convert TAI time to unix time
 *  @param[in] pDateTime TAI time
 *  @param[out] pTimeStamp UNIX time
 *  @return error code
 */
UINT32 AmbaTime_TaiDateTime2TimeStamp(const AMBA_RTC_DATE_TIME_s * pDateTime, UINT32 * pTimeStamp)
{
    UINT32 RetVal = ERR_NONE;

    if ((pDateTime == NULL) || (pTimeStamp == NULL)) {
        RetVal = ERR_ARG;
    } else {
        if ((pDateTime->Hour >= 24U) ||
            (pDateTime->Minute >= 60U) ||
            (pDateTime->Second >= 60U)) {
            RetVal = ERR_ARG;
        } else {
            if (DateTime2TimeStamp(&AmbaTaiEpoch, pDateTime, AmbaTaiTimeStampTable, pTimeStamp) != ERR_NONE) {
                RetVal = ERR_ARG;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaTime_UtcDateTime2TimeStamp - Convert UTC time to unix time
 *  @param[in] pDateTime UTC time
 *  @param[out] pTimeStamp UNIX time
 *  @return error code
 */
UINT32 AmbaTime_UtcDateTime2TimeStamp(const AMBA_RTC_DATE_TIME_s * pDateTime, UINT32 * pTimeStamp)
{
    UINT32 RetVal = ERR_NONE;

    if ((pDateTime == NULL) || (pTimeStamp == NULL)) {
        RetVal = ERR_ARG;
    } else {
        if ((pDateTime->Hour > 24U) ||
            (pDateTime->Minute >= 60U) ||
            (pDateTime->Second > 60U)) {
            RetVal = ERR_ARG;
        } else {
            if (DateTime2TimeStamp(&AmbaUtcEpoch, pDateTime, AmbaUtcTimeStampTable, pTimeStamp) != ERR_NONE) {
                RetVal = ERR_ARG;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaTime_TaiTimeStamp2DateTime - Convert unix time to TAI time
 *  @param[in] TimeStamp UNIX time
 *  @param[out] pDateTime TAI time
 *  @return error code
 */
UINT32 AmbaTime_TaiTimeStamp2DateTime(UINT32 TimeStamp, AMBA_RTC_DATE_TIME_s * pDateTime)
{
    UINT32 RetVal = ERR_NONE;

    if (pDateTime == NULL) {
        RetVal = ERR_ARG;
    } else {
        if (TimeStamp2DateTime(&AmbaTaiEpoch, TimeStamp, AmbaTaiTimeStampTable, pDateTime) != ERR_NONE) {
            RetVal = ERR_ARG;
        }
    }

    return RetVal;
}

/**
 *  AmbaTime_UtcTimeStamp2DateTime - Convert unix time to UTC time
 *  @param[in] TimeStamp UNIX time
 *  @param[out] pDateTime UTC time
 *  @return error code
 */
UINT32 AmbaTime_UtcTimeStamp2DateTime(UINT32 TimeStamp, AMBA_RTC_DATE_TIME_s * pDateTime)
{
    UINT32 RetVal = ERR_NONE;

    if (pDateTime == NULL) {
        RetVal = ERR_ARG;
    } else {
        if (TimeStamp2DateTime(&AmbaUtcEpoch, TimeStamp, AmbaUtcTimeStampTable, pDateTime) != ERR_NONE) {
            RetVal = ERR_ARG;
        }
    }

    return RetVal;
}
