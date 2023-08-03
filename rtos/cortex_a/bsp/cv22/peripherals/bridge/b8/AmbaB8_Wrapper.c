/**
*  @file AmbaB8_Wrapper.c
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
*  @details Definitions & Constants for B8 Wrapper Control APIs
*
*/
//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>

#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"

#include "AmbaB8.h"

#include "AmbaPrint.h"

#include "AmbaSPI.h"

#define B8_SPI_CHAN_ID_FOR_SOC_CHAN0        0
#define B8_SPI_CHAN_ID_FOR_SOC_CHAN1        0
#define B8_SPI_CHAN_ID_FOR_SOC_CHAN2        0
#define B8_SPI_CHAN_ID_FOR_SOC_CHAN3        0
#define B8_SPI_CHAN_ID_FOR_SOC_CHAN4        0
#define B8_SPI_CHAN_ID_FOR_SOC_CHAN5        0

#define B8_SPI_SLAVE_ID_FOR_SOC_CHAN0       0
#define B8_SPI_SLAVE_ID_FOR_SOC_CHAN1       1
#define B8_SPI_SLAVE_ID_FOR_SOC_CHAN2       0
#define B8_SPI_SLAVE_ID_FOR_SOC_CHAN3       0
#define B8_SPI_SLAVE_ID_FOR_SOC_CHAN4       0
#define B8_SPI_SLAVE_ID_FOR_SOC_CHAN5       0

typedef struct {
    UINT8 SpiChannel;
    UINT8 SpiSlaveID;
} B8_SOC_SPI_SELECT_s;

/* mapping between B8Ns and SoC Vin */
UINT32 B8SocVinChanSelect[B8_NUM_SOC_VIN_CHANNEL] = {
    [B8_SOC_VIN_CHANNEL0] = B8_CHANNEL0,
    [B8_SOC_VIN_CHANNEL1] = B8_CHANNEL1,
};

/* mapping between B8Ns and SoC Vout */
UINT32 B8SocVoutChanSelect[B8_NUM_SOC_VOUT_CHANNEL] = {
    [B8_SOC_VOUT_CHANNEL0] = B8_CHANNEL0,
    [B8_SOC_VOUT_CHANNEL1] = B8_CHANNEL1,
    [B8_SOC_VOUT_CHANNEL2] = B8_CHANNEL2,
    [B8_SOC_VOUT_CHANNEL3] = B8_CHANNEL3,
};

/* Wrapper of Kernel Abstraction Layer */

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_KAL_GetSysTickCount
 *
 *  @Description:: get current system tick counter value
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *              pSysTickCount : The number of elapsed system ticks.
 *
 *  @Return     ::
 *          UINT32 : KAL error code
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_KAL_GetSysTickCount(UINT32 * pSysTickCount)
{
    UINT32 RetVal;

    if (pSysTickCount == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        RetVal = AmbaKAL_GetSysTickCount(pSysTickCount);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_KAL_MutexCreate
 *
 *  @Description:: Create a mutex
 *
 *  @Input      ::
 *      pMutex : Pointer to a Mutex control block
 *      pMutexName : Pointer to a Mutex name
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : B8 error code
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_KAL_MutexCreate(AmbaB8_KAL_MUTEX_t *pMutex, char * pMutexName)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 TxRetVal;

    if (pMutex == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        TxRetVal = AmbaKAL_MutexCreate(pMutex, pMutexName);
        if (TxRetVal != OK) {
            RetVal = B8_ERR_MUTEX;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_KAL_MutexDelete
 *
 *  @Description:: The function deletes the specified mutex
 *
 *  @Input      ::
 *      pMutex: Pointer to the Mutex Control Block
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : B8 error code
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_KAL_MutexDelete(AmbaB8_KAL_MUTEX_t *pMutex)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 TxRetVal;

    if (pMutex == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        TxRetVal = AmbaKAL_MutexDelete(pMutex);
        if (TxRetVal != OK) {
            RetVal = B8_ERR_MUTEX;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_KAL_MutexTake
 *
 *  @Description:: Take the mutex
 *
 *  @Input      ::
 *      pMutex:  Pointer to the Mutex Control Block
 *      Timeout: Timeout value in ms (millisecond)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : B8 error code
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_KAL_MutexTake(AmbaB8_KAL_MUTEX_t *pMutex, UINT32 Timeout)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 TxRetVal;

    if (pMutex == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        TxRetVal = AmbaKAL_MutexTake(pMutex, Timeout);
        if (TxRetVal != OK) {
            RetVal = B8_ERR_MUTEX;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_KAL_MutexGive
 *
 *  @Description:: Give the mutex
 *
 *  @Input      ::
 *      pMutex: Pointer to the Mutex Control Block
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : B8 error code
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_KAL_MutexGive(AmbaB8_KAL_MUTEX_t *pMutex)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 TxRetVal;

    if (pMutex == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        TxRetVal = AmbaKAL_MutexGive(pMutex);
        if (TxRetVal != OK) {
            RetVal = B8_ERR_MUTEX;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_KAL_EventFlagCreate
 *
 *  @Description:: Create a group of 32 event flags
 *
 *  @Input      ::
 *      pEventFlag : Pointer to an event flags group control block
 *      pEventFlagName : Pointer to event flags name
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : B8 error code
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_KAL_EventFlagCreate(AmbaB8_KAL_EVENT_FLAG_t *pEventFlag, char * pEventFlagName)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 TxRetVal;

    if (pEventFlag == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        TxRetVal = AmbaKAL_EventFlagCreate(pEventFlag, pEventFlagName);
        if (TxRetVal != OK) {
            RetVal = B8_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_KAL_EventFlagDelete
 *
 *  @Description:: Delete the event flags group
 *
 *  @Input      ::
 *      pEventFlag: Pointer to the Event Flags Group Control Block
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : B8 error code
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_KAL_EventFlagDelete(AmbaB8_KAL_EVENT_FLAG_t *pEventFlag)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 TxRetVal;

    if (pEventFlag == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        TxRetVal = AmbaKAL_EventFlagDelete(pEventFlag);
        if (TxRetVal != OK) {
            RetVal = B8_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_KAL_EventFlagGet
 *
 *  @Description:: Take event flags from the event flags group
 *
 *  @Input      ::
 *      pEventFlag:     Pointer to the Event Flags Group Control Block
 *      ReqFlags:       A bit pattern indicating which bits to check
 *      AnyOrAll:       Specifies whether all or any of the requested event flags are required
 *      AutoClear:      Clear event flags that satisfy the request
 *      Timeout:        Timeout value in ms (millisecond)
 *
 *  @Output     ::
 *      pActualFlags: Actual event flags
 *
 *  @Return     ::
 *          UINT32 : B8 error code
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_KAL_EventFlagGet(AmbaB8_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ReqFlags, UINT32 AnyOrAll,
                               UINT32 AutoClear, UINT32 *pActualFlags, UINT32 Timeout)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 TxRetVal;
    UINT32 ActualFlags;

    if ((pEventFlag == NULL) || (pActualFlags == NULL)) {
        RetVal = B8_ERR_ARG;
    } else {
        TxRetVal = AmbaKAL_EventFlagGet(pEventFlag, ReqFlags, AnyOrAll, AutoClear, &ActualFlags, Timeout);
        if (TxRetVal == OK) {
            *pActualFlags = (UINT32) ActualFlags;
        } else {
            RetVal = B8_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_KAL_EventFlagSet
 *
 *  @Description:: The function sets event flags in an event flags group
 *
 *  @Input      ::
 *      pEventFlag : Pointer to the Event Flags Group Control Block
 *      SetFlags : Specify the event flags to give
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : B8 error code
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_KAL_EventFlagSet(AmbaB8_KAL_EVENT_FLAG_t *pEventFlag, UINT32 SetFlags)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 TxRetVal;

    if (pEventFlag == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        TxRetVal = AmbaKAL_EventFlagSet(pEventFlag, SetFlags);
        if (TxRetVal != OK) {
            RetVal = B8_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_KAL_EventFlagClear
 *
 *  @Description::
 *              Clear event flags to the event flags group
 *              All suspended threads whose event flags request is now satisfied are resumed.
 *
 *  @Input      ::
 *      pEventFlag : Pointer to the Event Flags Group Control Block
 *      ClearFlags : Specify the event flags to clear
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : B8 error code
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_KAL_EventFlagClear(AmbaB8_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ClearFlags)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 TxRetVal;

    if (pEventFlag == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        TxRetVal = AmbaKAL_EventFlagClear(pEventFlag, ClearFlags);
        if (TxRetVal != OK) {
            RetVal = B8_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_KAL_SemCreate
 *
 *  @Description:: This function creates a counting semaphore for inter-thread synchronization
 *
 *  @Input      ::
 *      pSemaphore :        Pointer to a Semaphore Control Block
 *      pSemaphoreName :    Pointer to the name string of a semaphore
 *      InitCount :         Initial count for this semaphore
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : B8 error code
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_KAL_SemCreate(AmbaB8_KAL_SEM_t *pSemaphore, char * pSemaphoreName, UINT32 InitCount)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 TxRetVal;

    if (pSemaphore == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        TxRetVal = AmbaKAL_SemaphoreCreate(pSemaphore, pSemaphoreName, InitCount);
        if (TxRetVal != OK) {
            RetVal = B8_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_KAL_SemDelete
 *
 *  @Description:: Delete the counting semaphore
 *
 *  @Input      ::
 *      pSemaphore: Pointer to a Semaphore Control Block
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : B8 error code
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_KAL_SemDelete(AmbaB8_KAL_SEM_t *pSemaphore)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 TxRetVal;

    if (pSemaphore == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        TxRetVal = AmbaKAL_SemaphoreDelete(pSemaphore);
        if (TxRetVal != OK) {
            RetVal = B8_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_KAL_SemTake
 *
 *  @Description:: Take the counting semaphore
 *
 *  @Input      ::
 *      pSem:    Pointer to the Semaphore Control Block
 *      Timeout: Timeout value in ms (millisecond)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : B8 error code
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_KAL_SemTake(AmbaB8_KAL_SEM_t *pSemaphore, UINT32 Timeout)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 TxRetVal;

    if (pSemaphore == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        TxRetVal = AmbaKAL_SemaphoreTake(pSemaphore, Timeout);
        if (TxRetVal != OK) {
            RetVal = B8_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_KAL_SemGive
 *
 *  @Description:: Give the counting semaphore
 *
 *  @Input      ::
 *      pSempahore: Pointer to the Semaphore Control Block
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : B8 error code
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_KAL_SemGive(AmbaB8_KAL_SEM_t *pSempahore)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 TxRetVal;

    if (pSempahore == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        TxRetVal = AmbaKAL_SemaphoreGive(pSempahore);
        if (TxRetVal != OK) {
            RetVal = B8_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_KAL_TaskSleep
 *
 *  @Description:: This function causes the calling thread to suspend for the specified number of system ticks.
 *                  If 0 is specified, the function returns immediately
 *
 *  @Input      ::
 *      NumTicks: The number of timer ticks(ms) to suspend the task
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : B8 error code
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_KAL_TaskSleep(UINT32 NumTicks)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 TxRetVal;

    TxRetVal = AmbaKAL_TaskSleep(NumTicks);
    if (TxRetVal != OK) {
        RetVal = B8_ERR_ARG;
    }

    return RetVal;
}

/* Wrapper of SPI Communication */
UINT32 AmbaB8_SPI_MasterTransfer(UINT32 ChipID, UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 i;
    UINT32 ChanID = 0U;
    UINT32 TxRetVal;

    /* SPI pin mapping between B8Ns and SoC */
    static B8_SOC_SPI_SELECT_s SocSpiSelect[B8_NUM_CHANNEL] = {
        [B8_CHANNEL0] = {
            .SpiChannel = B8_SPI_CHAN_ID_FOR_SOC_CHAN0,
            .SpiSlaveID = B8_SPI_SLAVE_ID_FOR_SOC_CHAN0,
        },
        [B8_CHANNEL1] = {
            .SpiChannel = B8_SPI_CHAN_ID_FOR_SOC_CHAN1,
            .SpiSlaveID = B8_SPI_SLAVE_ID_FOR_SOC_CHAN1,
        },
        [B8_CHANNEL2] = {
            .SpiChannel = B8_SPI_CHAN_ID_FOR_SOC_CHAN2,
            .SpiSlaveID = B8_SPI_SLAVE_ID_FOR_SOC_CHAN2,
        },
        [B8_CHANNEL3] = {
            .SpiChannel = B8_SPI_CHAN_ID_FOR_SOC_CHAN3,
            .SpiSlaveID = B8_SPI_SLAVE_ID_FOR_SOC_CHAN3,
        },
        [B8_CHANNEL4] = {
            .SpiChannel = B8_SPI_CHAN_ID_FOR_SOC_CHAN4,
            .SpiSlaveID = B8_SPI_SLAVE_ID_FOR_SOC_CHAN4,
        },
        [B8_CHANNEL5] = {
            .SpiChannel = B8_SPI_CHAN_ID_FOR_SOC_CHAN5,
            .SpiSlaveID = B8_SPI_SLAVE_ID_FOR_SOC_CHAN5,
        },
    };

    /* SPI config between B8Ns and SoC */
    static AMBA_SPI_CONFIG_s B8nSpiConfig = {
        .BaudRate      = 10000000U,                                        /* Transfer BaudRate in Hz */
        .CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,                  /* Slave select polarity */
        .ClkMode       = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,                     /* SPI Protocol mode */
        .DataFrameSize = 8,                                                /* Data Frame Size in Bit */
        .FrameBitOrder = AMBA_SPI_TRANSFER_MSB_FIRST,
    };

    if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
        for (i = 0U; i < B8_NUM_SOC_VIN_CHANNEL; i ++) {
            if ((ChipID & ((UINT32)1U << (i + 4U))) != 0U) {
                break;
            }
        }
        if (i < B8_NUM_SOC_VIN_CHANNEL) {
            ChanID = B8SocVinChanSelect[i];
        } else {
            RetVal = B8_ERR_ARG;
        }
    } else if ((ChipID & B8_MAIN_CHIP_ID_B8ND_MASK) != 0U) {
        for (i = 0U; i < B8_NUM_SOC_VOUT_CHANNEL; i ++) {
            if ((ChipID & ((UINT32)1U << (i + 12U))) != 0U) {
                break;
            }
        }
        if (i < B8_NUM_SOC_VOUT_CHANNEL) {
            ChanID = B8SocVoutChanSelect[i];
        } else {
            RetVal = B8_ERR_ARG;
        }
    } else {
        RetVal = B8_ERR_ARG;
    }

    if (RetVal == B8_ERR_NONE) {
//        B8nSpiConfig.SlaveID = SocSpiSelect[ChanID].SpiSlaveID;
        TxRetVal = AmbaSPI_MasterTransferD8(SocSpiSelect[ChanID].SpiChannel, ((UINT32)1U << SocSpiSelect[ChanID].SpiSlaveID), &B8nSpiConfig, BufSize, pTxBuf, pRxBuf, pActualSize, TimeOut);
        if (TxRetVal != OK) {
            RetVal = B8_ERR_COMMUNICATE;
        }
    }

    return RetVal;
}

/* Wrapper of Printf */
void AmbaB8_PrintStr5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5)
{
    AmbaPrint_PrintStr5(pFmt, pArg1, pArg2, pArg3, pArg4, pArg5);
}

void AmbaB8_PrintUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    AmbaPrint_PrintUInt5(pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
}

void AmbaB8_PrintInt5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5)
{
    AmbaPrint_PrintInt5(pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
}

void AmbaB8_PrintFlush(void)
{
    AmbaPrint_Flush();
}

/* Wrapper of Shell */
void AmbaB8_ShellPrintf(const char *pString)
{
    AmbaMisra_TouchUnused(&pString);
}

/* Wrapper of MisraC */
void AmbaB8_Misra_TouchUnused(void * pArg)
{
    AmbaMisra_TouchUnused(pArg);
}

void AmbaB8_Misra_TypeCast32(void * pNewType, const void * pOldType)
{
    AmbaMisra_TypeCast32(pNewType, pOldType);
}

/* Wrapper of  standard lib */
UINT32 AmbaB8_Wrap_memcpy(void *pDst, const void *pSrc, size_t num)
{
    UINT32 RetVal = B8_ERR_NONE;

    if ((pDst == NULL) ||
        (pSrc == NULL) ||
        (num == 0U)) {
        RetVal = B8_ERR_ARG;
    } else {
        RetVal = AmbaWrap_memcpy(pDst, pSrc, num);
    }

    return RetVal;
}

UINT32 AmbaB8_Wrap_memset(void *ptr, INT32 v, size_t n)
{
    UINT32 RetVal;

    if (ptr == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        RetVal = AmbaWrap_memset(ptr, v, n);
    }

    return RetVal;
}

UINT32 AmbaB8_Wrap_rand(void *pV)
{
    UINT32 RetVal;

    if (pV == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        RetVal = AmbaWrap_rand(pV);
    }

    return RetVal;
}

UINT32 AmbaB8_Wrap_srand(UINT32 v)
{
    return AmbaWrap_srand(v);
}

/**
 *  @brief  Wrapper of floor()
 *
 *  Calculating the largest integral value not greater than x.
 *
 *  @param[in]  x   Value to round down.
 *  @param[in]  pV  Return the largest integral value not greater than x in DOUBLE.
 *
 *  @return     B8 error code
 */
UINT32 AmbaB8_Wrap_floor(DOUBLE x, void *pV)
{
    UINT32 RetVal = B8_ERR_NONE;

    if (pV == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        RetVal = AmbaWrap_floor(x, pV);
    }

    return RetVal;
}

/**
 *  @brief  Wrapper of pow()
 *
 *  Calculating the value of base raised to the power of exponent.
 *
 *  @param[in]  base        Base value.
 *  @param[in]  exponent    Exponent value.
 *  @param[in]  pV          Return base^exponent in DOUBLE.
 *
 *  @return     B8 error code
 */
UINT32 AmbaB8_Wrap_pow(DOUBLE base, DOUBLE exponent, void *pV)
{
    UINT32 RetVal = B8_ERR_NONE;

    if (pV == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        RetVal = AmbaWrap_pow(base, exponent, pV);
    }

    return RetVal;
}

/**
 *  @brief  Wrapper of ceil()
 *
 *  Get the smallest integral value that is not less than x.
 *
 *  @param[in]  x   Value to round up.
 *  @param[in]  pV  The smallest integral value that is not less than x in DOUBLE.
 *
 *  @return     B8 error code
 */
UINT32 AmbaB8_Wrap_ceil(DOUBLE x, void *pV)
{
    UINT32 RetVal;

    if (pV == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        RetVal = AmbaWrap_ceil(x, pV);
    }

    return RetVal;
}

UINT32 AmbaB8_Wrap_exp(DOUBLE x, void *pV)
{
    UINT32 RetVal = B8_ERR_NONE;

    if (pV == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        RetVal = AmbaWrap_pow(2.718, x, pV);
    }

    return RetVal;
}

UINT32 AmbaB8_Utility_UInt32ToStr(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix)
{
    UINT32 StrLength;

    StrLength = AmbaUtility_UInt32ToStr(pBuffer, BufferSize, Value, Radix);

    return StrLength;
}

INT32 AmbaB8_Utility_StringCompare(const char *pString1, const char *pString2, size_t Size)
{
    INT32 RetVal = 0;

    if ((pString1 == NULL) || (pString2 == NULL) || (Size == 0U)) {
        // return no equal
        RetVal = -1;
    } else {
        RetVal = AmbaUtility_StringCompare(pString1, pString2, Size);
    }
    return RetVal;
}

UINT32 AmbaB8_Utility_StringAppend(char *pBuffer, UINT32 BufferSize, const char *pSource)
{
    UINT32 RetVal = B8_ERR_NONE;

    if ((pBuffer == NULL) || (pSource == NULL) || (BufferSize == 0U)) {
        RetVal = B8_ERR_ARG;
    } else {
        AmbaUtility_StringAppend(pBuffer, BufferSize, pSource);
    }

    return RetVal;
}

UINT32 AmbaB8_Utility_StringToUInt32(const char *pString, UINT32 *pValue)
{
    UINT32 RetVal;
    if ((pString == NULL) || (pValue == NULL)) {
        RetVal = B8_ERR_ARG;
    } else {
        RetVal = AmbaUtility_StringToUInt32(pString, pValue);
    }

    return RetVal;
}

UINT32 AmbaB8_Utility_StrPrintUInt32(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const UINT32 *pArgs)
{
    UINT32 RetVal;
    if ((pBuffer == NULL) || (pFmtString == NULL)) {
        RetVal = B8_ERR_ARG;
    } else {
        RetVal = AmbaUtility_StringPrintUInt32(pBuffer, BufferSize, pFmtString, Count, pArgs);
    }

    return RetVal;
}

