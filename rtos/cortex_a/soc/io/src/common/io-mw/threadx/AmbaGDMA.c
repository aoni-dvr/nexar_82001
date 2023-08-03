/**
 *  @file AmbaGDMA.c
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
 *  @details 2D Graphics DMA Middleware APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"
#include "AmbaGDMA.h"
#include "AmbaRTSL_GDMA.h"

#define AMBA_GDMA_TIMEOUT_FLAG      0x10000UL

typedef struct {
    UINT32  NumRequest;                 /* Number of the GDMA requests */
    UINT32  NumComplete;                /* Number of the finished GDMA requests */

    AMBA_GDMA_ISR_f CompletionCB[GDMA_NUM_INSTANCE];
    UINT32  CompletionCbArg[GDMA_NUM_INSTANCE];
} AMBA_GDMA_CTRL_s;

static AMBA_GDMA_CTRL_s AmbaGdmaCtrl;   /* GDMA Management Structure */

static AMBA_KAL_EVENT_FLAG_t AmbaGdmaEventFlags;
static AMBA_KAL_MUTEX_t AmbaGdmaMutex;
static AMBA_KAL_TIMER_t AmbaGdmaTimer;

static UINT32 AmbaGdmaTransferDoneISRErr;
static UINT32 AmbaGdmaTimeOutActFuncErr;

static void GDMA_PushIsrFunc(AMBA_GDMA_ISR_f NotifierFunc, UINT32 NotifierFuncArg);
static void GDMA_TransferDoneISR(UINT32 Arg);
static void GDMA_TimeOutActFunc(UINT32 Arg);
static void GDMA_TagForLastCommand(void);
static UINT32 GDMA_WaitAvailable(UINT32 TimeOut);

/**
 *  AmbaGDMA_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaGDMA_DrvEntry(void)
{
    static char AmbaGdmaMutexName[16] = "AmbaGdmaMutex";
    static char AmbaGdmaEventFlagsName[20] = "AmbaGdmaEventFlags";
    static char AmbaGdmaTimerName[16] = "AmbaGdmaTimer";
    UINT32 i, RetVal = GDMA_ERR_NONE;

    if (AmbaKAL_EventFlagCreate(&AmbaGdmaEventFlags, AmbaGdmaEventFlagsName) != KAL_ERR_NONE) {
        RetVal = GDMA_ERR_UNEXPECTED;
    }

    if (RetVal == GDMA_ERR_NONE) {
        if (AmbaKAL_MutexCreate(&AmbaGdmaMutex, AmbaGdmaMutexName) != KAL_ERR_NONE) {
            RetVal = GDMA_ERR_UNEXPECTED;
        }
    }

    if (RetVal == GDMA_ERR_NONE) {
        if (AmbaKAL_TimerCreate(&AmbaGdmaTimer, AmbaGdmaTimerName,
                                GDMA_TimeOutActFunc, 0U,
                                10U * AMBA_KAL_SYS_TICK_MS, 0U,
                                0U) != KAL_ERR_NONE) {
            RetVal = GDMA_ERR_UNEXPECTED;
        }
    }

    if (RetVal == GDMA_ERR_NONE) {
        AmbaGdmaCtrl.NumRequest  = 0U;
        AmbaGdmaCtrl.NumComplete = 0U;
        for (i = 0; i < GDMA_NUM_INSTANCE; i++) {
            AmbaGdmaCtrl.CompletionCB[i] = NULL;
        }

        AmbaGdmaTransferDoneISRErr  = GDMA_ERR_NONE;    /* clear error status reported by interrupt handler */
        AmbaGdmaTimeOutActFuncErr   = GDMA_ERR_NONE;    /* clear error status reported by timeout function */

        AmbaRTSL_GdmaInit();
        AmbaRTSL_GdmaHookIntHandler(GDMA_TransferDoneISR);      /* Callback function when completed transactions */
    }

    return RetVal;
}

/**
 *  AmbaGDMA_GetInfo - Retrieve the controller status
 *  @param[out] pNumAvails Number of available transaction slots
 *  @return error code
 */
UINT32 AmbaGDMA_GetInfo(UINT32 * pNumAvails)
{
    UINT32 RetVal = GDMA_ERR_NONE;

    if (pNumAvails == NULL) {
        RetVal = GDMA_ERR_ARG;
    } else {
        *pNumAvails = AmbaRTSL_GdmaGetNumAvails();
    }

    return RetVal;
}

/**
 *  AmbaGDMA_WaitAllCompletion - Wait until the last transaction done
 *  @param[in] TimeOut The maximum wait time
 *  @return error code
 */
UINT32 AmbaGDMA_WaitAllCompletion(UINT32 TimeOut)
{
    AMBA_KAL_TIMER_t *pTimer = &AmbaGdmaTimer;
    UINT32 ActualFlags, RetVal = GDMA_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaGdmaMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = GDMA_ERR_MUTEX;
    } else {
        if (TimeOut == KAL_WAIT_NEVER) {
            /* Insert one operation at the end because we don't know the gdma engine is idle or not */
            if (AmbaRTSL_GdmaGetNumAvails() == GDMA_NUM_INSTANCE) {
                GDMA_TagForLastCommand();
            }
        } else {
            /* Insert one operation at the end */
            while (AmbaRTSL_GdmaGetNumAvails() == 0U) {
                continue;
            }
            GDMA_TagForLastCommand();

            if (AmbaKAL_TimerChange(pTimer, TimeOut, 0U, KAL_START_AUTO) == KAL_ERR_NONE) {
                do {
                    if (AmbaKAL_EventFlagGet(&AmbaGdmaEventFlags,
                                             (UINT32)AMBA_GDMA_TIMEOUT_FLAG,
                                             KAL_FLAGS_WAIT_ANY,
                                             KAL_FLAGS_CLEAR_AUTO,
                                             &ActualFlags,
                                             KAL_WAIT_NEVER) == KAL_ERR_NONE) {
                        /* Break on timeout */
                        break;
                    }
                } while (AmbaRTSL_GdmaGetNumAvails() != GDMA_NUM_INSTANCE);

                /* Stop the Software Timer */
                (void)AmbaKAL_TimerStop(pTimer);
            }
        }

        if (AmbaRTSL_GdmaGetNumAvails() != GDMA_NUM_INSTANCE) {
            RetVal = GDMA_ERR_TMO;
        }

        if (AmbaGdmaTransferDoneISRErr != GDMA_ERR_NONE) {  /* check if any error reported in interrupt handler */
            RetVal = AmbaGdmaTransferDoneISRErr;
            AmbaGdmaTransferDoneISRErr = GDMA_ERR_NONE;     /* clear error status reported in interrupt handler */
        }

        if (AmbaGdmaTimeOutActFuncErr != GDMA_ERR_NONE) {   /* check if any error reported in timeout function */
            RetVal = AmbaGdmaTimeOutActFuncErr;
            AmbaGdmaTimeOutActFuncErr = GDMA_ERR_NONE;      /* clear error status reported in timeout function */
        }

        if (AmbaKAL_MutexGive(&AmbaGdmaMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = GDMA_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaGDMA_LinearCopy - Schedule a linear copy transaction
 *  @param[in] pLinearBlit The control block of the linear copy
 *  @param[in] NotifierFunc The completion callback
 *  @param[in] NotifierFuncArg The optional argument of the completion callback
 *  @param[in] TimeOut The maximum tolerance time to schedule this transaction
 *  @return error code
 */
UINT32 AmbaGDMA_LinearCopy(const AMBA_GDMA_LINEAR_s * pLinearBlit, AMBA_GDMA_ISR_f NotifierFunc, UINT32 NotifierFuncArg, UINT32 TimeOut)
{
    UINT32 RetVal = GDMA_ERR_NONE;

    if (pLinearBlit == NULL) {
        RetVal = GDMA_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaGdmaMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = GDMA_ERR_MUTEX;
        } else {
            if (AmbaRTSL_GdmaGetNumAvails() == 0U) {
                RetVal = GDMA_WaitAvailable(TimeOut);
            }

            /* Always check if there is an available instance no matter a timeout occurs or not */
            if (AmbaRTSL_GdmaGetNumAvails() != 0U) {
                /* GDMA instance available */
                GDMA_PushIsrFunc(NotifierFunc, NotifierFuncArg);
                RetVal = AmbaRTSL_GdmaLinearCopy(pLinearBlit);
            }

            if (AmbaKAL_MutexGive(&AmbaGdmaMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = GDMA_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaGDMA_BlockCopy - Schedule a block copy transaction
 *  @param[in] pBlockBlit The control block of the block copy
 *  @param[in] NotifierFunc The completion callback
 *  @param[in] NotifierFuncArg The optional argument of the completion callback
 *  @param[in] TimeOut The maximum tolerance time to schedule this transaction
 *  @return error code
 */
UINT32 AmbaGDMA_BlockCopy(const AMBA_GDMA_BLOCK_s * pBlockBlit, AMBA_GDMA_ISR_f NotifierFunc, UINT32 NotifierFuncArg, UINT32 TimeOut)
{
    UINT32 RetVal = GDMA_ERR_NONE;

    if (pBlockBlit == NULL) {
        RetVal = GDMA_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaGdmaMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = GDMA_ERR_MUTEX;
        } else {
            if (AmbaRTSL_GdmaGetNumAvails() == 0U) {
                RetVal = GDMA_WaitAvailable(TimeOut);
            }

            /* Always check if there is an available instance no matter a timeout occurs or not */
            if (AmbaRTSL_GdmaGetNumAvails() != 0U) {
                /* GDMA instance available */
                GDMA_PushIsrFunc(NotifierFunc, NotifierFuncArg);
                RetVal = AmbaRTSL_GdmaBlockCopy(pBlockBlit);
            }

            if (AmbaKAL_MutexGive(&AmbaGdmaMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = GDMA_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaGDMA_ColorKeying - Schedule a chroma key compositing transaction
 *  @param[in] pBlockBlit The control block of the chroma key compositing
 *  @param[in] TransparentColor The color hue in foreground image treated as transparent
 *  @param[in] NotifierFunc The completion callback
 *  @param[in] NotifierFuncArg The optional argument of the completion callback
 *  @param[in] TimeOut The maximum tolerance time to schedule this transaction
 *  @return error code
 */
UINT32 AmbaGDMA_ColorKeying(const AMBA_GDMA_BLOCK_s * pBlockBlit, UINT32 TransparentColor, AMBA_GDMA_ISR_f NotifierFunc, UINT32 NotifierFuncArg, UINT32 TimeOut)
{
    UINT32 RetVal = GDMA_ERR_NONE;

    if (pBlockBlit == NULL) {
        RetVal = GDMA_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaGdmaMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = GDMA_ERR_MUTEX;
        } else {
            if (AmbaRTSL_GdmaGetNumAvails() == 0U) {
                RetVal = GDMA_WaitAvailable(TimeOut);
            }

            /* Always check if there is an available instance no matter a timeout occurs or not */
            if (AmbaRTSL_GdmaGetNumAvails() != 0U) {
                /* GDMA instance available */
                GDMA_PushIsrFunc(NotifierFunc, NotifierFuncArg);
                RetVal = AmbaRTSL_GdmaColorKeying(pBlockBlit, TransparentColor);
            }

            if (AmbaKAL_MutexGive(&AmbaGdmaMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = GDMA_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaGDMA_AlphaBlending - Schedule a alpha blending transaction
 *  @param[in] pBlockBlit The control block of the alpha blending
 *  @param[in] AlphaVal The global alpha value
 *  @param[in] BlendMode The alpha value is premultiplied or not
 *  @param[in] NotifierFunc The completion callback
 *  @param[in] NotifierFuncArg The optional argument of the completion callback
 *  @param[in] TimeOut The maximum tolerance time to schedule this transaction
 *  @return error code
 */
UINT32 AmbaGDMA_AlphaBlending(const AMBA_GDMA_BLOCK_s * pBlockBlit, UINT32 AlphaVal, UINT32 BlendMode, AMBA_GDMA_ISR_f NotifierFunc, UINT32 NotifierFuncArg, UINT32 TimeOut)
{
    UINT32 RetVal = GDMA_ERR_NONE;

    if (pBlockBlit == NULL) {
        RetVal = GDMA_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaGdmaMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = GDMA_ERR_MUTEX;
        } else {
            if (AmbaRTSL_GdmaGetNumAvails() == 0U) {
                RetVal = GDMA_WaitAvailable(TimeOut);
            }

            /* Always check if there is an available instance no matter a timeout occurs or not */
            if (AmbaRTSL_GdmaGetNumAvails() != 0U) {
                /* GDMA instance available */
                GDMA_PushIsrFunc(NotifierFunc, NotifierFuncArg);
                RetVal = AmbaRTSL_GdmaAlphaBlending(pBlockBlit, AlphaVal, BlendMode);
            }

            if (AmbaKAL_MutexGive(&AmbaGdmaMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = GDMA_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  GDMA_PushIsrFunc - Booking completion callback
 *  @param[in] NotifierFunc The completion callback
 *  @param[in] NotifierFuncArg The optional argument of the completion callback
 */
static void GDMA_PushIsrFunc(AMBA_GDMA_ISR_f NotifierFunc, UINT32 NotifierFuncArg)
{
    UINT32 SlotNo = AmbaGdmaCtrl.NumRequest & 0x07U;

    AmbaGdmaCtrl.CompletionCB[SlotNo] = NotifierFunc;
    AmbaGdmaCtrl.CompletionCbArg[SlotNo] = NotifierFuncArg;
    AmbaGdmaCtrl.NumRequest++;
}

/**
 *  GDMA_TransferDoneISR - Completion interrupt handler
 *  @param[in] Arg Optional argument
 */
static void GDMA_TransferDoneISR(UINT32 Arg)
{
    UINT32 SlotNo = (AmbaGdmaCtrl.NumComplete & 0x07U);

    AmbaMisra_TouchUnused(&Arg);

    /* Set GDMA Done Event Flag */
    if (AmbaKAL_EventFlagSet(&AmbaGdmaEventFlags, ((UINT32)1U << SlotNo)) != KAL_ERR_NONE) {
        AmbaGdmaTransferDoneISRErr = GDMA_ERR_UNEXPECTED;
    }

    /* Call the ISR function to notice the GDMA request has done */
    if (AmbaGdmaCtrl.CompletionCB[SlotNo] != NULL) {
        AmbaGdmaCtrl.CompletionCB[SlotNo](AmbaGdmaCtrl.CompletionCbArg[SlotNo]);
    }

    AmbaGdmaCtrl.NumComplete++;
}

/**
 *  GDMA_TimeOutActFunc - Time Out Acting Function called when Time Out
 *  @param[in] Arg Optional argument
 */
static void GDMA_TimeOutActFunc(UINT32 Arg)
{
    AmbaMisra_TouchUnused(&Arg);

    /* set Time Out Event Flag */
    if (AmbaKAL_EventFlagSet(&AmbaGdmaEventFlags, (UINT32)AMBA_GDMA_TIMEOUT_FLAG) != KAL_ERR_NONE) {
        AmbaGdmaTimeOutActFuncErr = GDMA_ERR_UNEXPECTED;
    }
}

/**
 *  GDMA_TagForLastCommand - Insert a dummy transaction as a tag
 */
static void GDMA_TagForLastCommand(void)
{
    UINT32 DummyData = 0U;
    AMBA_GDMA_LINEAR_s LinearCtrl = {
        .PixelFormat = GDMA_32_BIT,
        .NumPixels   = 1U,
        .pSrcImg     = &DummyData,
        .pDstImg     = &DummyData,
    };

    (void)AmbaRTSL_GdmaLinearCopy(&LinearCtrl);
}

/**
 *  GDMA_WaitAvailable - Wait for an available transaction slot
 *  @param[in] TimeOut The maximum wait time
 *  @return error code
 */
static UINT32 GDMA_WaitAvailable(UINT32 TimeOut)
{
    AMBA_KAL_TIMER_t *pTimer = &AmbaGdmaTimer;
    UINT32 ActualFlags, RetVal = GDMA_ERR_NONE;

    if (TimeOut == KAL_WAIT_NEVER) {
        if (AmbaRTSL_GdmaGetNumAvails() == 0U) {
            RetVal = GDMA_ERR_TMO;
        }
    } else {
        if (AmbaKAL_TimerChange(pTimer, TimeOut, 0U, KAL_START_AUTO) != KAL_ERR_NONE) {
            RetVal = GDMA_ERR_UNEXPECTED;
        } else {
            do {
                if (AmbaKAL_EventFlagGet(&AmbaGdmaEventFlags,
                                         (UINT32)AMBA_GDMA_TIMEOUT_FLAG,
                                         KAL_FLAGS_WAIT_ANY,
                                         KAL_FLAGS_CLEAR_AUTO,
                                         &ActualFlags,
                                         KAL_WAIT_NEVER) == KAL_ERR_NONE) {
                    RetVal = GDMA_ERR_TMO;
                    break;
                }
            } while (AmbaRTSL_GdmaGetNumAvails() == 0U);

            /* Stop the Software Timer */
            if (AmbaKAL_TimerStop(pTimer) != KAL_ERR_NONE) {
                RetVal = GDMA_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

