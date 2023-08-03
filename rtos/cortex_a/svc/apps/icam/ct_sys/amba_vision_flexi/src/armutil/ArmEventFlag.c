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

#define MAX_EVENTFLAG_NUM   (32)

typedef struct {
  pthread_cond_t    *pCond;
  pthread_mutex_t   Mutex;
  UINT32            Flags;
} ARM_EVENTFLAG_CTRL_s;

static ARM_EVENTFLAG_CTRL_s g_ArmEventFlagCtrl[MAX_EVENTFLAG_NUM] = {0};

/*---------------------------------------------------------------------------*\
 * static functions
\*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CheckState
 *
 *  @Description:: Check if the current state is matched to the wait state
 *
 *  @Input      ::
 *    CurState:    The current state
 *    WaitState:   The waiting state
 *    AnyOrAll:    1: AND 0: OR
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *      int:       1: true 0: false
\*-----------------------------------------------------------------------------------------------*/
static int CheckState(UINT32 CurState, UINT32 WaitState, UINT32 AnyOrAll)
{
  int Rval = 0;

  if (AnyOrAll == 0) {  // OR
    if ((CurState & WaitState) != 0) {
      Rval = 1;
    }
  } else {
    if ((CurState & WaitState) == WaitState) {
      Rval = 1;
    }
  }

  return Rval;
}

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
  UINT32 Index = 0U;

  /* 1. Sanity check for input parameters */
  if (pEventFlag == NULL) {
    ArmLog_ERR(ARM_LOG_EF, "## pEventFlag is null", 0U, 0U);
    Rval = ARM_NG;
  } else if (pEventFlagName == NULL) {
    ArmLog_ERR(ARM_LOG_EF, "## pEventFlagName is null", 0U, 0U);
    Rval = ARM_NG;
  }

  /* 2. Get avaliable eventflag slot */
  if (Rval == ARM_OK) {
    Index = MAX_EVENTFLAG_NUM;
    for (UINT32 i = 0; i < MAX_EVENTFLAG_NUM; i++) {
      if (g_ArmEventFlagCtrl[i].pCond == NULL) {
        Index = i;
        break;
      }
    }

    if (Index == MAX_EVENTFLAG_NUM) {
      ArmLog_ERR(ARM_LOG_EF, "## Can't find avaliable eventflag slot", 0U, 0U);
      Rval = ARM_NG;
    }
  }

  /* 3. Create pthread_cond_t and pthread_mutex_t */
  if (Rval == ARM_OK) {
    int r = 0;
    r = pthread_cond_init(pEventFlag, NULL);
    if (r != 0) {
      ArmLog_ERR(ARM_LOG_EF, "## pthread_cond_init() fail (r = %d)", r, 0U);
      Rval = ARM_NG;
    } else {
      r = pthread_mutex_init(&g_ArmEventFlagCtrl[Index].Mutex, NULL);
      if (r != 0) {
        ArmLog_ERR(ARM_LOG_EF, "## pthread_mutex_init() fail (r = %d)", r, 0U);
        Rval = ARM_NG;
      } else {
        g_ArmEventFlagCtrl[Index].pCond = pEventFlag;
        g_ArmEventFlagCtrl[Index].Flags = 0;
      }
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
  UINT32 Index = 0U;

  /* 1. Sanity check for input parameters */
  if (pEventFlag == NULL) {
    ArmLog_ERR(ARM_LOG_EF, "## pEventFlag is null", 0U, 0U);
    Rval = ARM_NG;
  }

  /* 2. Find the deleted eventflag slot */
  if (Rval == ARM_OK) {
    Index = MAX_EVENTFLAG_NUM;
    for (UINT32 i = 0; i < MAX_EVENTFLAG_NUM; i++) {
      if (g_ArmEventFlagCtrl[i].pCond == pEventFlag) {
        Index = i;
        break;
      }
    }

    if (Index == MAX_EVENTFLAG_NUM) {
      ArmLog_ERR(ARM_LOG_EF, "## Can't find eventflag slot", 0U, 0U);
      Rval = ARM_NG;
    }
  }

  /* 3. Delete pthread_cond_t and pthread_mutex_t */
  if (Rval == ARM_OK) {
    int r = 0;
    r = pthread_cond_destroy(pEventFlag);
    if (r != 0) {
      ArmLog_ERR(ARM_LOG_EF, "## pthread_cond_destroy() fail (r = %d)", r, 0U);
      Rval = ARM_NG;
    } else {
      r = pthread_mutex_destroy(&g_ArmEventFlagCtrl[Index].Mutex);
      if (r != 0 && r != 16) {  // 16 = EBUSY
        ArmLog_ERR(ARM_LOG_EF, "## pthread_mutex_destroy() fail (r = %d)", r, 0U);
        Rval = ARM_NG;
      } else {
        g_ArmEventFlagCtrl[Index].pCond = NULL;
        g_ArmEventFlagCtrl[Index].Flags = 0;
      }
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
  UINT32 Index = 0U;

  /* 1. Sanity check for input parameters */
  if (pEventFlag == NULL) {
    ArmLog_ERR(ARM_LOG_EF, "## pEventFlag is null", 0U, 0U);
    Rval = ARM_NG;
  }

  /* 2. Find the eventflag slot */
  if (Rval == ARM_OK) {
    Index = MAX_EVENTFLAG_NUM;
    for (UINT32 i = 0; i < MAX_EVENTFLAG_NUM; i++) {
      if (g_ArmEventFlagCtrl[i].pCond == pEventFlag) {
        Index = i;
        break;
      }
    }

    if (Index == MAX_EVENTFLAG_NUM) {
      ArmLog_ERR(ARM_LOG_EF, "## Can't find eventflag slot", 0U, 0U);
      Rval = ARM_NG;
    }
  }

  /* 3. Set eventflag */
  if (Rval == ARM_OK) {
    pthread_mutex_lock(&g_ArmEventFlagCtrl[Index].Mutex);

    g_ArmEventFlagCtrl[Index].Flags |= SetFlags;
    pthread_cond_signal(g_ArmEventFlagCtrl[Index].pCond);

    pthread_mutex_unlock(&g_ArmEventFlagCtrl[Index].Mutex);
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
  UINT32 Index = 0U;

  /* 1. Sanity check for input parameters */
  if (pEventFlag == NULL) {
    ArmLog_ERR(ARM_LOG_EF, "## pEventFlag is null", 0U, 0U);
    Rval = ARM_NG;
  }

  /* 2. Find the eventflag slot */
  if (Rval == ARM_OK) {
    Index = MAX_EVENTFLAG_NUM;
    for (UINT32 i = 0; i < MAX_EVENTFLAG_NUM; i++) {
      if (g_ArmEventFlagCtrl[i].pCond == pEventFlag) {
        Index = i;
        break;
      }
    }

    if (Index == MAX_EVENTFLAG_NUM) {
      ArmLog_ERR(ARM_LOG_EF, "## Can't find eventflag slot", 0U, 0U);
      Rval = ARM_NG;
    }
  }

  /* 3. Clear eventflag */
  if (Rval == ARM_OK) {
    pthread_mutex_lock(&g_ArmEventFlagCtrl[Index].Mutex);

    g_ArmEventFlagCtrl[Index].Flags &= ~ClearFlags;

    pthread_mutex_unlock(&g_ArmEventFlagCtrl[Index].Mutex);
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
 *
 *  @Output     ::
 *    pActualFlags: The returned actual flags
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmEventFlag_Wait(ArmEventFlag_t *pEventFlag, UINT32 WaitFlags, UINT32 Options, UINT32 *pActualFlags)
{
  UINT32 Rval = ARM_OK;
  UINT32 Index = 0U;
  UINT32 AnyOrAll = 0U, AutoClear = 0U;

  if ((Options & (0x00000001U)) != 0) {
    AutoClear = 1;  // CLEAR
  } else {
    AutoClear = 0;  // NOT CLEAR
  }

  if ((Options & (0x00000010U)) != 0) {
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
  }

  /* 2. Find the eventflag slot */
  if (Rval == ARM_OK) {
    Index = MAX_EVENTFLAG_NUM;
    for (UINT32 i = 0; i < MAX_EVENTFLAG_NUM; i++) {
      if (g_ArmEventFlagCtrl[i].pCond == pEventFlag) {
        Index = i;
        break;
      }
    }

    if (Index == MAX_EVENTFLAG_NUM) {
      ArmLog_ERR(ARM_LOG_EF, "## Can't find eventflag slot", 0U, 0U);
      Rval = ARM_NG;
    }
  }

  /* 3. Wait eventflag */
  if (Rval == ARM_OK) {
    pthread_mutex_lock(&g_ArmEventFlagCtrl[Index].Mutex);
    while (CheckState(g_ArmEventFlagCtrl[Index].Flags, WaitFlags, AnyOrAll) != 1) {
      pthread_cond_wait(g_ArmEventFlagCtrl[Index].pCond, &g_ArmEventFlagCtrl[Index].Mutex);
    }

    *pActualFlags = g_ArmEventFlagCtrl[Index].Flags & WaitFlags;

    if (AutoClear == 1) {
      g_ArmEventFlagCtrl[Index].Flags &= ~WaitFlags;
    }
    pthread_mutex_unlock(&g_ArmEventFlagCtrl[Index].Mutex);
  }

  return Rval;
}

