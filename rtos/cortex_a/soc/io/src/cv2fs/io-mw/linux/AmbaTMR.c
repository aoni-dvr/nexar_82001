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

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"
#include "AmbaTMR.h"
//#include "AmbaRTSL_TMR.h"

/* Reserved timer 19 for rtos schdeuler ticks, timer 18 for rtos profiling */
#define AMBA_TMR_AVAILS     0x3ffffU


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
    (void) pTimerID;
    (void) TimeOut;

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
    (void) TimerID;
    (void) TimeOut;
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
    (void) TimerID;
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
    (void) TimerID;
    (void) ExpireFunc;
    (void) ExpireFuncArg;
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
    (void) TimerID;
    (void) TimerFreq;
    (void) NumPeriodicTick;
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
    (void) TimerID;
    (void) NumRemainTick;

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
    (void) TimerID;
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

    (void) TimerID;
    (void) pTimeLeft;

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
