/**
 *  @file amba_osal.h
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
 *  @details Definitions for Ambarella KAL (Kernel Abstraction Layer) structures
 *
 */

#ifndef OSAL_QNX_H
#define OSAL_QNX_H

#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaCache.h"
#include "AmbaINT.h"
#include "AmbaSYS.h"
/******************************************************************************/
/*                                  AmbaKAL.h                                                                                       */
/******************************************************************************/
#define OSAL_TaskCreate             AmbaKAL_TaskCreate
#define OSAL_TaskSetSmpAffinity     AmbaKAL_TaskSetSmpAffinity
#define OSAL_TaskQuery              AmbaKAL_TaskQuery
#define OSAL_TaskDelete             AmbaKAL_TaskDelete
#define OSAL_TaskTerminate          AmbaKAL_TaskTerminate
#define OSAL_TaskSuspend            AmbaKAL_TaskSuspend
#define OSAL_TaskResume             AmbaKAL_TaskResume
#define OSAL_TaskSleep              AmbaKAL_TaskSleep

#define OSAL_EventFlagCreate        AmbaKAL_EventFlagCreate
#define OSAL_EventFlagDelete        AmbaKAL_EventFlagDelete
#define OSAL_EventFlagGet           AmbaKAL_EventFlagGet
#define OSAL_EventFlagSet           AmbaKAL_EventFlagSet
#define OSAL_EventFlagClear         AmbaKAL_EventFlagClear

#define OSAL_MutexCreate            AmbaKAL_MutexCreate
#define OSAL_MutexDelete            AmbaKAL_MutexDelete
#define OSAL_MutexTake              AmbaKAL_MutexTake
#define OSAL_MutexGive              AmbaKAL_MutexGive

#define OSAL_SemaphoreCreate        AmbaKAL_SemaphoreCreate
#define OSAL_SemaphoreDelete        AmbaKAL_SemaphoreDelete
#define OSAL_SemaphoreTake          AmbaKAL_SemaphoreTake
#define OSAL_SemaphoreGive          AmbaKAL_SemaphoreGive

#define OSAL_MsgQueueCreate         AmbaKAL_MsgQueueCreate
#define OSAL_MsgQueueDelete         AmbaKAL_MsgQueueDelete
#define OSAL_MsgQueueFlush          AmbaKAL_MsgQueueFlush
#define OSAL_MsgQueueReceive        AmbaKAL_MsgQueueReceive
#define OSAL_MsgQueueSend           AmbaKAL_MsgQueueSend
#define OSAL_MsgQueueQuery          AmbaKAL_MsgQueueQuery

/******************************************************************************/
/*                                  AmbaMisraFix.h                                                                                   */
/******************************************************************************/
#define OSAL_TypeCast               AmbaMisra_TypeCast

/******************************************************************************/
/*                                  AmbaPrint.h                                                                                       */
/******************************************************************************/
#define OSAL_PrintUInt5             AmbaPrint_PrintUInt5
#define OSAL_PrintInt5              AmbaPrint_PrintInt5
#define OSAL_PrintStr5              AmbaPrint_PrintStr5

#define OSAL_ModuleSetAllowList     AmbaPrint_ModuleSetAllowList
#define OSAL_ModulePrintStr5        AmbaPrint_ModulePrintStr5
#define OSAL_ModulePrintUInt5       AmbaPrint_ModulePrintUInt5
#define OSAL_ModulePrintInt5        AmbaPrint_ModulePrintInt5

#define OSAL_PrintFlush             AmbaPrint_Flush
#define OSAL_PrintStopAndFlush      AmbaPrint_StopAndFlush

/******************************************************************************/
/*                                  AmbaWrap.h                                                                                      */
/******************************************************************************/
#define OSAL_memcpy                 AmbaWrap_memcpy
#define OSAL_memset                 AmbaWrap_memset
#define OSAL_memcmp                 AmbaWrap_memcmp

/******************************************************************************/
/*                                  AmbaINT.h                                                                                   */
/******************************************************************************/
#define OSAL_INT_Config             AmbaINT_Config
#define OSAL_INT_Enable             AmbaINT_Enable
#define OSAL_INT_Disable            AmbaINT_Disable

/******************************************************************************/
/*                                  AmbaCache.h                                                                                   */
/******************************************************************************/
#define OSAL_Cache_DataClean        AmbaCache_DataClean
#define OSAL_Cache_DataInvalidate   AmbaCache_DataInvalidate

/******************************************************************************/
/*                                  AmbaUtility.h                                                                                   */
/******************************************************************************/
#define OSAL_StringLength           AmbaUtility_StringLength
#define OSAL_StringCompare          AmbaUtility_StringCompare
#define OSAL_StringCopy             AmbaUtility_StringCopy
#define OSAL_StringAppend           AmbaUtility_StringAppend
#define OSAL_StringPrintUInt32      AmbaUtility_StringPrintUInt32
#define OSAL_StringPrintStr         AmbaUtility_StringPrintStr
#define OSAL_StringPrintInt32       AmbaUtility_StringPrintInt32
#define OSAL_StringAppendInt32      AmbaUtility_StringAppendInt32
#define OSAL_StringAppendUInt32     AmbaUtility_StringAppendUInt32
#define OSAL_StringAppendFloat      AmbaUtility_StringAppendFloat
#define OSAL_StringAppendDouble     AmbaUtility_StringAppendDouble

#define OSAL_FloatToStr             AmbaUtility_FloatToStr
#define OSAL_DoubleToStr            AmbaUtility_DoubleToStr
#define OSAL_Int32ToStr             AmbaUtility_Int32ToStr
#define OSAL_Int64ToStr             AmbaUtility_Int64ToStr
#define OSAL_UInt32ToStr            AmbaUtility_UInt32ToStr
#define OSAL_UInt64ToStr            AmbaUtility_UInt64ToStr

#define OSAL_StringLength           AmbaUtility_StringLength
#define OSAL_StringCopy             AmbaUtility_StringCopy
#define OSAL_StringCompare          AmbaUtility_StringCompare
#define OSAL_StringToken            AmbaUtility_StringToken
#define OSAL_StringToUInt32         AmbaUtility_StringToUInt32
#define OSAL_StringToUInt64         AmbaUtility_StringToUInt64

#define OSAL_GetClkFreq             AmbaSYS_GetClkFreq

#define OSAL_TouchUnused            AmbaMisra_TouchUnused
#endif
