/**
 *  @file AmbaB8_Wrapper.h
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Ambarella B8 Wrapper APIs
 *
 */
#ifndef AMBAB8_WRAP_H
#define AMBAB8_WRAP_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_KAL_H
#include "AmbaKAL.h"
#endif
#ifndef AMBA_VIN_DEF_H
#include "AmbaVIN_Def.h"
#endif

#ifndef AMBA_MISRA_FIX_H
#include "AmbaMisraFix.h"
#endif

#ifdef B8_DEV_VERSION
#include <math.h>
#endif

typedef AMBA_KAL_SEMAPHORE_t        AmbaB8_KAL_SEM_t;           /* Counting Semaphore */
typedef AMBA_KAL_MUTEX_t            AmbaB8_KAL_MUTEX_t;         /* Mutex */
typedef AMBA_KAL_EVENT_FLAG_t       AmbaB8_KAL_EVENT_FLAG_t;    /* Event Flags */

/* B8 Vin Channel ID, used to get corresponding SPI control channel */
#define B8_SOC_VIN_CHANNEL0         AMBA_VIN_CHANNEL0           /* B8 on VIN */
#define B8_SOC_VIN_CHANNEL1         AMBA_VIN_CHANNEL1           /* B8 on PIP */
#define B8_SOC_VIN_CHANNEL2         AMBA_VIN_CHANNEL2           /* B8 on PIP */
#define B8_SOC_VIN_CHANNEL3         AMBA_VIN_CHANNEL3           /* B8 on PIP */
#define B8_SOC_VIN_CHANNEL4         AMBA_VIN_CHANNEL4           /* B8 on PIP */
#define B8_SOC_VIN_CHANNEL5         AMBA_VIN_CHANNEL5           /* B8 on PIP */
#define B8_SOC_VIN_CHANNEL6         AMBA_VIN_CHANNEL6           /* B8 on PIP */
#define B8_SOC_VIN_CHANNEL7         AMBA_VIN_CHANNEL7           /* B8 on PIP */
#define B8_SOC_VIN_CHANNEL8         AMBA_VIN_CHANNEL8           /* B8 on PIP */
#define B8_NUM_SOC_VIN_CHANNEL      AMBA_NUM_VIN_CHANNEL

/* B8 Vout Channel ID, used to get corresponding SPI control channel */
#define B8_SOC_VOUT_CHANNEL0        0U                           /* B8 on VOUT0 */
#define B8_SOC_VOUT_CHANNEL1        1U                           /* B8 on VOUT1 */
#define B8_SOC_VOUT_CHANNEL2        2U                           /* B8 on VOUT2 */
#define B8_SOC_VOUT_CHANNEL3        3U                           /* B8 on VOUT3 */
#define B8_NUM_SOC_VOUT_CHANNEL     4U

#define AMBAB8_KAL_WAIT_FOREVER     AMBA_KAL_WAIT_FOREVER

static inline UINT32 AmbaB8_GetAbsValU32 (INT32 Val)
{
    return (((Val) >= 0) ? (UINT32)(Val) : (UINT32)(-(Val)));
}
static inline UINT64 AmbaB8_GetAbsValU64 (INT64 Val)
{
    return (((Val) >= 0) ? (UINT64)(Val) : (UINT64)(-(Val)));
}

static inline UINT32 AmbaB8_GetRoundUpValU32 (UINT32 Val, UINT32 NumDigits)
{
    return (0x0U != NumDigits) ? (((Val) + (NumDigits) - 0x1U) / (NumDigits)) : (Val);
}

/* Wrapper of Kernel Abstraction Layer */
UINT32 AmbaB8_KAL_GetSysTickCount(UINT32 * pSysTickCount);

UINT32 AmbaB8_KAL_MutexCreate(AmbaB8_KAL_MUTEX_t *pMutex, char * pMutexName);
UINT32 AmbaB8_KAL_MutexDelete(AmbaB8_KAL_MUTEX_t *pMutex);
UINT32 AmbaB8_KAL_MutexTake(AmbaB8_KAL_MUTEX_t *pMutex, UINT32 Timeout);
UINT32 AmbaB8_KAL_MutexGive(AmbaB8_KAL_MUTEX_t *pMutex);

UINT32 AmbaB8_KAL_EventFlagCreate(AmbaB8_KAL_EVENT_FLAG_t *pEventFlag, char * pEventFlagName);
UINT32 AmbaB8_KAL_EventFlagDelete(AmbaB8_KAL_EVENT_FLAG_t *pEventFlag);
UINT32 AmbaB8_KAL_EventFlagGet(AmbaB8_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ReqFlags, UINT32 AnyOrAll,
                               UINT32 AutoClear, UINT32 *pActualFlags, UINT32 Timeout);
UINT32 AmbaB8_KAL_EventFlagSet(AmbaB8_KAL_EVENT_FLAG_t *pEventFlag, UINT32 SetFlags);
UINT32 AmbaB8_KAL_EventFlagClear(AmbaB8_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ClearFlags);

UINT32 AmbaB8_KAL_SemCreate(AmbaB8_KAL_SEM_t *pSemaphore, char * pSemaphoreName, UINT32 InitCount);
UINT32 AmbaB8_KAL_SemDelete(AmbaB8_KAL_SEM_t *pSemaphore);
UINT32 AmbaB8_KAL_SemTake(AmbaB8_KAL_SEM_t *pSemaphore, UINT32 Timeout);
UINT32 AmbaB8_KAL_SemGive(AmbaB8_KAL_SEM_t *pSempahore);

UINT32 AmbaB8_KAL_TaskSleep(UINT32 NumTicks);

/* Wrapper of SPI Communication */
extern UINT32 B8SocVinChanSelect[B8_NUM_SOC_VIN_CHANNEL];
extern UINT32 B8SocVoutChanSelect[B8_NUM_SOC_VOUT_CHANNEL];
UINT32 AmbaB8_SPI_MasterTransfer(UINT32 ChipID, UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut);

/* Wrapper of Printf */
void AmbaB8_PrintStr5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);
void AmbaB8_PrintUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
void AmbaB8_PrintInt5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5);
void AmbaB8_PrintFlush(void);

/* Wrapper of Shell */
void AmbaB8_ShellPrintf(const char *pString);

/* Wrapper of MisraC */
void AmbaB8_Misra_TouchUnused(void * pArg);
void AmbaB8_Misra_TypeCast32(void * pNewType, const void * pOldType);
void AmbaB8_Misra_TypeCast(void * pNewType, const void * pOldType);

/* Wrapper of standard lib */
UINT32 AmbaB8_Wrap_memcpy(void *pDst, const void *pSrc, size_t num);
UINT32 AmbaB8_Wrap_memset(void *ptr, INT32 v, size_t n);
UINT32 AmbaB8_Wrap_rand(void *pV);
UINT32 AmbaB8_Wrap_srand(UINT32 v);
UINT32 AmbaB8_Wrap_floor(DOUBLE x, void *pV);
UINT32 AmbaB8_Wrap_pow(DOUBLE base, DOUBLE exponent, void *pV);
UINT32 AmbaB8_Wrap_ceil(DOUBLE x, void *pV);
UINT32 AmbaB8_Wrap_exp(DOUBLE x, void *pV);

/* Wrapper of Utility */
UINT32 AmbaB8_Utility_UInt32ToStr(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix);
UINT32 AmbaB8_Utility_StringToUInt32(const char *pString, UINT32 *pValue);
INT32 AmbaB8_Utility_StringCompare(const char *pString1, const char *pString2, size_t Size);
UINT32 AmbaB8_Utility_StringAppend(char *pBuffer, UINT32 BufferSize, const char *pSource);
UINT32 AmbaB8_Utility_StrPrintUInt32(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const UINT32 *pArgs);

#endif /* AMBAB8_WRAP_H */

