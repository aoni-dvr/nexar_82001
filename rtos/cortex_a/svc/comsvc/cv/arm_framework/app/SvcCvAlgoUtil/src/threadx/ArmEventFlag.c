/**
*  @file ArmEventFlag.c
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
*   @details The Arm event flag utility
*
*/

/* Arm header */
#include "ArmEventFlag.h"
#include "ArmErrCode.h"
#include "ArmLog.h"

#define ARM_LOG_EF          "ArmUtil_EventFlag"

/*---------------------------------------------------------------------------*\
 * APIs
\*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmEventFlag_Create
 *
 *  @Description:: Create event flag
 *
 *  @Input      ::
 *    pEventFlagName: The name of event flag
 *
 *  @Output     ::
 *    pEventFlag:     Pointer to the event flag
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmEventFlag_Create(ArmEventFlag_t *pEventFlag, char *pEventFlagName)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for input parameters */
    if (pEventFlag == NULL) {
        ArmLog_ERR(ARM_LOG_EF, "## pEventFlag is null", 0U, 0U);
        Rval = ARM_NG;
    } else if (pEventFlagName == NULL) {
        ArmLog_ERR(ARM_LOG_EF, "## pEventFlagName is null", 0U, 0U);
        Rval = ARM_NG;
    } else { // do nothing
    }

    /* 2. Create eventflag */
    if (Rval == ARM_OK) {
        Rval = AmbaKAL_EventFlagCreate(pEventFlag, pEventFlagName);
        if (Rval != KAL_ERR_NONE) {
            ArmLog_ERR(ARM_LOG_EF, "## AmbaKAL_EventFlagCreate() fail (Rval = 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        }
    }

    if (Rval == ARM_OK) {
        ArmLog_STR(ARM_LOG_EF, "Success to create eventflag (%s)", pEventFlagName, NULL);
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmEventFlag_Delete
 *
 *  @Description:: Delete event flag
 *
 *  @Input      ::
 *    pEventFlag:  Pointer to the event flag
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmEventFlag_Delete(ArmEventFlag_t *pEventFlag)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for input parameters */
    if (pEventFlag == NULL) {
        ArmLog_ERR(ARM_LOG_EF, "## pEventFlag is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Delete eventflag */
    if (Rval == ARM_OK) {
        Rval = AmbaKAL_EventFlagDelete(pEventFlag);
        if (Rval != KAL_ERR_NONE) {
            ArmLog_ERR(ARM_LOG_EF, "## AmbaKAL_EventFlagDelete() fail (Rval = 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmEventFlag_Set
 *
 *  @Description:: Set event flag
 *
 *  @Input      ::
 *    pEventFlag:  Pointer to the event flag
 *    SetFlags:    The set flags
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmEventFlag_Set(ArmEventFlag_t *pEventFlag, UINT32 SetFlags)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for input parameters */
    if (pEventFlag == NULL) {
        ArmLog_ERR(ARM_LOG_EF, "## pEventFlag is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Set eventflag */
    if (Rval == ARM_OK) {
        Rval = AmbaKAL_EventFlagSet(pEventFlag, SetFlags);
        if (Rval != KAL_ERR_NONE) {
            ArmLog_ERR(ARM_LOG_EF, "## AmbaKAL_EventFlagSet() fail (Rval = 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmEventFlag_Clear
 *
 *  @Description:: Clear event flag
 *
 *  @Input      ::
 *    pEventFlag:  Pointer to the event flag
 *    ClearFlags:  The clear flags
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmEventFlag_Clear(ArmEventFlag_t *pEventFlag, UINT32 ClearFlags)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for input parameters */
    if (pEventFlag == NULL) {
        ArmLog_ERR(ARM_LOG_EF, "## pEventFlag is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Clear eventflag */
    if (Rval == ARM_OK) {
        Rval = AmbaKAL_EventFlagClear(pEventFlag, ClearFlags);
        if (Rval != KAL_ERR_NONE) {
            ArmLog_ERR(ARM_LOG_EF, "## AmbaKAL_EventFlagClear() fail (Rval = 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmEventFlag_Wait
 *
 *  @Description:: Wait event flag
 *
 *  @Input      ::
 *    pEventFlag:   Pointer to the event flag
 *    WaitFlags:    The wait flags
 *    Options:      The wait options (AND/OR/CLEAR)
 *    Timeout:      The timeout setting
 *
 *  @Output     ::
 *    pActualFlags: The returned actual flags
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmEventFlag_Wait(ArmEventFlag_t *pEventFlag, UINT32 WaitFlags, UINT32 Options, UINT32 *pActualFlags, UINT32 Timeout)
{
    UINT32 Rval = ARM_OK;
    UINT32 AnyOrAll = 0U, AutoClear = 0U;

    if ((Options & (0x00000001U)) != 0U) {
        AutoClear = 1;  // CLEAR
    } else {
        AutoClear = 0;  // NOT CLEAR
    }

    if ((Options & (0x00000010U)) != 0U) {
        AnyOrAll = 1;   // AND
    } else {
        AnyOrAll = 0;   // OR
    }

    /* 1. Sanity check for input parameters */
    if (pEventFlag == NULL) {
        ArmLog_ERR(ARM_LOG_EF, "## pEventFlag is null", 0U, 0U);
        Rval = ARM_NG;
    } else if (pActualFlags == NULL) {
        ArmLog_ERR(ARM_LOG_EF, "## pActualFlags is null", 0U, 0U);
        Rval = ARM_NG;
    } else { // do nothing
    }

    /* 2. Wait eventflag */
    if (Rval == ARM_OK) {
        Rval = AmbaKAL_EventFlagGet(pEventFlag, WaitFlags, AnyOrAll, AutoClear, pActualFlags, Timeout);
        if (Rval != KAL_ERR_NONE) {
            ArmLog_ERR(ARM_LOG_EF, "## AmbaKAL_EventFlagGet() fail (Rval = 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        }
    }
    return Rval;
}

