/**
 *  @file AmbaDMA.c
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
 *  @details DMA Control APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"

#include "AmbaDrvEntry.h"
#include "AmbaDMA.h"
#include "AmbaRTSL_DMA.h"

static AMBA_KAL_MUTEX_t AmbaDmaGlobalMutex;
static AMBA_KAL_MUTEX_t AmbaDmaMutex[AMBA_NUM_DMA_CHANNEL] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_EVENT_FLAG_t AmbaDmaEventFlags;
static UINT32 AmbaDmaTransferDoneISRErr[AMBA_NUM_DMA_CHANNEL];

static void DMA_TransferDoneISR(UINT32 DmaChanNo);

/**
 *  AmbaDMA_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaDMA_DrvEntry(void)
{
    static char AmbaDmaEventFlagName[18] = "AmbaDmaEventFlags";
    static char AmbaDmaMutexName[16] = "AmbaDmaMutex";
    UINT32 i, RetVal = DMA_ERR_NONE;

    if (AmbaKAL_EventFlagCreate(&AmbaDmaEventFlags, AmbaDmaEventFlagName) != KAL_ERR_NONE) {
        RetVal = DMA_ERR_UNEXPECTED;
    }

    if (RetVal == DMA_ERR_NONE) {
        if (AmbaKAL_MutexCreate(&AmbaDmaGlobalMutex, AmbaDmaMutexName) != KAL_ERR_NONE) {
            RetVal = DMA_ERR_UNEXPECTED;
        }
    }

    if (RetVal == DMA_ERR_NONE) {
        RetVal = AmbaWrap_memset(AmbaDmaMutex, 0x0, sizeof(AmbaDmaMutex));
        if (RetVal == DMA_ERR_NONE) {
            for (i = 0; i < AMBA_NUM_DMA_CHANNEL; i++) {
                if (AmbaKAL_MutexCreate(&AmbaDmaMutex[i], AmbaDmaMutexName) != KAL_ERR_NONE) {
                    RetVal = DMA_ERR_UNEXPECTED;
                    break;
                }
            }
        } else {
            RetVal = DMA_ERR_UNEXPECTED;
        }
    }

    if (RetVal == DMA_ERR_NONE) {
        AmbaRTSL_DmaInit();
        AmbaRTSL_DmaHookIntHandler(DMA_TransferDoneISR);
    }

    return RetVal;
}

/**
 *  AmbaDMA_ChannelAllocate - Allocate a dedidated dma channel for the specified purpose
 *  @param[in] DmaChanType The desired dma function
 *  @param[out] pDmaChanNo The allocated dma channel id
 *  @return error code
 */
UINT32 AmbaDMA_ChannelAllocate(UINT32 DmaChanType, UINT32 *pDmaChanNo)
{
    AMBA_KAL_MUTEX_t *pMutex = &AmbaDmaGlobalMutex;
    UINT32 RetVal = DMA_ERR_NONE;

    if ((DmaChanType >= AMBA_NUM_DMA_CHANNEL_TYPE) || (pDmaChanNo == NULL)) {
        RetVal = DMA_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = DMA_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_DmaChanAllocate(DmaChanType, pDmaChanNo);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = DMA_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaDMA_ChannelRelease - Release an allocated dma channel
 *  @param[in] DmaChanNo The allocated dma channel id
 *  @return error code
 */
UINT32 AmbaDMA_ChannelRelease(UINT32 DmaChanNo)
{
    AMBA_KAL_MUTEX_t *pMutex = &AmbaDmaGlobalMutex;
    UINT32 RetVal = DMA_ERR_NONE;

    if (DmaChanNo >= AMBA_NUM_DMA_CHANNEL) {
        RetVal = DMA_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = DMA_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_DmaChanRelease(DmaChanNo);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = DMA_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaDMA_Transfer - Start the dma transfer
 *  @param[in] DmaChanNo A dma channel id (must acquire one before the transfer)
 *  @param[in] pDmaDesc The control block of the dma transfer
 *  @return error code
 */
UINT32 AmbaDMA_Transfer(UINT32 DmaChanNo, AMBA_DMA_DESC_s *pDmaDesc)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = DMA_ERR_NONE;

    if ((DmaChanNo >= AMBA_NUM_DMA_CHANNEL) || (pDmaDesc == NULL)) {
        RetVal = DMA_ERR_ARG;
    } else {
        pMutex = &(AmbaDmaMutex[DmaChanNo]);

        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = DMA_ERR_MUTEX;
        } else {
            if (AmbaKAL_EventFlagClear(&AmbaDmaEventFlags, (UINT32)1U << DmaChanNo) != KAL_ERR_NONE) {
                RetVal = DMA_ERR_UNEXPECTED;
            } else {
                AmbaDmaTransferDoneISRErr[DmaChanNo] = DMA_ERR_NONE; /* clear error status reported by interrupt handler */
                RetVal = AmbaRTSL_DmaTransfer(DmaChanNo, pDmaDesc);
            }

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = DMA_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaDMA_Wait - Wait for dma transfer compeltion
 *  @param[in] DmaChanNo A dma channel id (must acquire one before the transfer to prevent conflict)
 *  @param[in] TimeOut The maximum wait time
 *  @return error code
 */
UINT32 AmbaDMA_Wait(UINT32 DmaChanNo, UINT32 TimeOut)
{
    UINT32 KalRetVal, RetVal = DMA_ERR_NONE;
    UINT32 ActualFlags;
    AMBA_KAL_MUTEX_t *pMutex;

    if (DmaChanNo >= AMBA_NUM_DMA_CHANNEL) {
        RetVal = DMA_ERR_ARG;
    } else {
        KalRetVal = AmbaKAL_EventFlagGet(&AmbaDmaEventFlags,
                                         (UINT32)1U << DmaChanNo,
                                         KAL_FLAGS_WAIT_ALL,
                                         KAL_FLAGS_CLEAR_AUTO,
                                         &ActualFlags,
                                         TimeOut);
        if (KalRetVal != KAL_ERR_NONE) {
            if (KalRetVal == KAL_ERR_TIMEOUT) {
                RetVal = DMA_ERR_TMO;
            } else {
                RetVal = DMA_ERR_UNEXPECTED;
            }
        }

        /* check if any error reported in interrupt handler */
        pMutex = &(AmbaDmaMutex[DmaChanNo]);
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = DMA_ERR_MUTEX;
        } else {
            if (AmbaDmaTransferDoneISRErr[DmaChanNo] != DMA_ERR_NONE) {
                RetVal = AmbaDmaTransferDoneISRErr[DmaChanNo];
            }
            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = DMA_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}


/**
 *  AmbaDMA_GetIntCount - Get interrupt count of a DMA channel
 *  @param[in] DmaChanNo DMA channel number
 *  @return error code
 */
UINT32 AmbaDMA_GetIntCount(UINT32 DmaChanNo)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = DMA_ERR_NONE;

    if (DmaChanNo >= AMBA_NUM_DMA_CHANNEL) {
        RetVal = DMA_ERR_ARG;
    } else {
        pMutex = &(AmbaDmaMutex[DmaChanNo]);

        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = DMA_ERR_MUTEX;
        } else {
             RetVal = AmbaRTSL_DmaGetIntCount(DmaChanNo);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = DMA_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaDMA_ResetIntCount - Reset interrupt count of a DMA channel
 *  @param[in] DmaChanNo DMA channel number
 *  @return error code
 */
UINT32 AmbaDMA_ResetIntCount(UINT32 DmaChanNo)
{
    AMBA_KAL_MUTEX_t *pMutex;
    UINT32 RetVal = DMA_ERR_NONE;

    if (DmaChanNo >= AMBA_NUM_DMA_CHANNEL) {
        RetVal = DMA_ERR_ARG;
    } else {
        pMutex = &(AmbaDmaMutex[DmaChanNo]);

        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = DMA_ERR_MUTEX;
        } else {
             AmbaRTSL_DmaResetIntCount(DmaChanNo);

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = DMA_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  DMA_TransferDoneISR - Interrupt handler for DMA.
 *  @param[in] DmaChanNo A dma channel id
 */
static void DMA_TransferDoneISR(UINT32 DmaChanNo)
{
    /* Set DMA Done Event Flag */
    if (AmbaKAL_EventFlagSet(&AmbaDmaEventFlags, (UINT32)1U << DmaChanNo) != KAL_ERR_NONE) {
        AmbaDmaTransferDoneISRErr[DmaChanNo] = DMA_ERR_UNEXPECTED;
    }
}

