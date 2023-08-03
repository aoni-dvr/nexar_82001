/**
*  @file ArmMutex.c
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
*   @details The Arm mutex utility
*
*/

/* Arm header */
#include "ArmMutex.h"
#include "ArmErrCode.h"
#include "ArmLog.h"

#define ARM_LOG_MUTEX          "ArmUtil_Mutex"

/*---------------------------------------------------------------------------*\
 * APIs
\*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMutex_Create
 *
 *  @Description:: Create Mutex
 *
 *  @Input      ::
 *    pMutexName:  The mutex name
 *
 *  @Output     ::
 *    pMutex:      Pointer to the mutex
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMutex_Create(ArmMutex_t *pMutex, char * pMutexName)
{
  UINT32 Rval = ARM_OK;

  /* 1. Sanity check for input parameters */
  if (pMutex == NULL) {
    ArmLog_ERR(ARM_LOG_MUTEX, "## pMutex is null", 0U, 0U);
    Rval = ARM_NG;
  } else if (pMutexName == NULL) {
    ArmLog_ERR(ARM_LOG_MUTEX, "## pMutexName is null", 0U, 0U);
    Rval = ARM_NG;
  }

  /* 2. Create mutex */
  if (Rval == ARM_OK) {
    int r = 0;
    r = pthread_mutex_init(pMutex, NULL);
    if (r != 0) {
      ArmLog_ERR(ARM_LOG_MUTEX, "## pthread_mutex_init() fail (r = %d)", r, 0U);
      Rval = ARM_NG;
    }
  }

  if (Rval == ARM_OK) {
    ArmLog_STR(ARM_LOG_MUTEX, "Success to create mutex (%s)", pMutexName, NULL);
  }

  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMutex_Delete
 *
 *  @Description:: Delete Mutex
 *
 *  @Input      ::
 *    pMutex:      Pointer to the mutex
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMutex_Delete(ArmMutex_t *pMutex)
{
  UINT32 Rval = ARM_OK;

  /* 1. Sanity check for input parameters */
  if (pMutex == NULL) {
    ArmLog_ERR(ARM_LOG_MUTEX, "## pMutex is null", 0U, 0U);
    Rval = ARM_NG;
  }

  /* 2. Delete mutex */
  if (Rval == ARM_OK) {
    int r = 0;
    r = pthread_mutex_destroy(pMutex);
    if (r != 0) {
      ArmLog_ERR(ARM_LOG_MUTEX, "## pthread_mutex_destroy() fail (r = %d)", r, 0U);
      Rval = ARM_NG;
    }
  }
  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMutex_Take
 *
 *  @Description:: Take(Lock) Mutex
 *
 *  @Input      ::
 *    pMutex:      Pointer to the mutex
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMutex_Take(ArmMutex_t *pMutex)
{
  UINT32 Rval = ARM_OK;

  /* 1. Sanity check for input parameters */
  if (pMutex == NULL) {
    ArmLog_ERR(ARM_LOG_MUTEX, "## pMutex is null", 0U, 0U);
    Rval = ARM_NG;
  }

  /* 2. Lock mutex */
  if (Rval == ARM_OK) {
    int r = 0;
    r = pthread_mutex_lock(pMutex);
    if (r != 0) {
      ArmLog_ERR(ARM_LOG_MUTEX, "## pthread_mutex_lock() fail (r = %d)", r, 0U);
      Rval = ARM_NG;
    }
  }
  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMutex_Give
 *
 *  @Description:: Give(Unlock) Mutex
 *
 *  @Input      ::
 *    pMutex:      Pointer to the mutex
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMutex_Give(ArmMutex_t *pMutex)
{
  UINT32 Rval = ARM_OK;

  /* 1. Sanity check for input parameters */
  if (pMutex == NULL) {
    ArmLog_ERR(ARM_LOG_MUTEX, "## pMutex is null", 0U, 0U);
    Rval = ARM_NG;
  }

  /* 2. Unlock mutex */
  if (Rval == ARM_OK) {
    int r = 0;
    r = pthread_mutex_unlock(pMutex);
    if (r != 0) {
      ArmLog_ERR(ARM_LOG_MUTEX, "## pthread_mutex_unlock() fail (r = %d)", r, 0U);
      Rval = ARM_NG;
    }
  }

  return Rval;
}


